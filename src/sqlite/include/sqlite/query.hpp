#ifndef SQLITE_QUERY_HPP
#define SQLITE_QUERY_HPP

#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "sqlite3.h"

#include "exception.hpp"

namespace sqlite {

class query final {
    friend class db;

public:
    query(query &&rhs) noexcept:
    _stmt{std::exchange(rhs._stmt, nullptr)},
    _metadata{std::move(rhs._metadata)} {

    }

    query &operator=(query &&rhs) noexcept {
        this->~query();
        _stmt = std::exchange(rhs._stmt, nullptr);
        _metadata = std::move(rhs._metadata);
        return *this;
    }

    ~query() noexcept {
        if (!_stmt) {
            return;
        }
        if (const int code = sqlite3_finalize(_stmt); exception::is_ok(code)) {
            _stmt = nullptr;
        } else {
            std::cerr << __func__ << ": " << exception{code} << std::endl;
        }
    }

    auto num_rows_affected() const {
        return sqlite3_changes(_metadata._db);
    }

    auto last_insert_id() const {
        return sqlite3_last_insert_rowid(_metadata._db);
    }

    const auto &get_column_names() const {
        return _metadata._column_names;
    }

    template <class type>
    query &bind(const int index, type &&value) {
        using type_t = std::remove_cv_t<std::remove_reference_t<type>>;
        static_assert(
            std::is_integral_v<type_t> || std::is_floating_point_v<type_t> ||
                std::is_same_v<type_t, std::string> || std::is_same_v<type_t, std::string_view> ||
                std::is_same_v<type_t, std::nullptr_t>,
            "type must be integral, floating point, string, string view or nullptr"
        );

        const int i = get_checked_bind_index(index);
        if constexpr (std::is_integral_v<type_t>) {
            if constexpr (sizeof(type_t) > sizeof(int)) {
                exception::check(sqlite3_bind_int64(_stmt, i, value));
            } else {
                exception::check(sqlite3_bind_int(_stmt, i, value));
            }
        }
        if constexpr (std::is_floating_point_v<type_t>) {
            exception::check(sqlite3_bind_double(_stmt, i, value));
        }
        if constexpr (std::is_same_v<type_t, std::string> || std::is_same_v<type_t, std::string_view>) {
            exception::check(
                sqlite3_bind_text(_stmt, i, value.data(),
                    static_cast<int>(value.size()), reinterpret_cast<sqlite3_destructor_type>(0)
                )
            );
        }
        if constexpr (std::is_null_pointer_v<type_t>) {
            exception::check(sqlite3_bind_null(_stmt, i));
        }
        return *this;
    }

    query &bind(const int index, const char *value) {
        return bind<std::string_view>(index, value);
    }

    auto exec() {
        if (_metadata._column_names.empty()) {
            if (const int code = sqlite3_step(_stmt); code != SQLITE_DONE) {
                this->~query();
                throw exception{code};
            }
            const auto n = num_rows_affected();
            exception::check(sqlite3_reset(_stmt));
            return n;
        }        
        return 0;
    }

    bool first() {
        exception::check(sqlite3_reset(_stmt));
        return next();
    }

    bool next() {
        switch (const int result = sqlite3_step(_stmt); result) {
        case SQLITE_ROW:
            return true;
        case SQLITE_DONE:
            return false;
        default:
            throw exception{result};
        }
    }

    template <class type>
    std::optional<type> get(const int index) const {
        static_assert(
            std::is_integral_v<type> || std::is_floating_point_v<type> ||
                std::is_same_v<type, std::string> || std::is_same_v<type, std::string_view>,
            "type must be integral, floating point, string or string view"
        );

        const int checked_type = _metadata.get_checked_type(_stmt, index);
        if (checked_type == SQLITE_NULL) {
            return {};
        }

        if constexpr (std::is_integral_v<type>) {
            check_type(checked_type, SQLITE_INTEGER);
            if (sqlite3_column_bytes(_stmt, index) > static_cast<int>(sizeof(int))) {
                return sqlite3_column_int64(_stmt, index);
            } else {
                return sqlite3_column_int(_stmt, index);
            }
        }
        if constexpr (std::is_floating_point_v<type>) {
            check_type(checked_type, SQLITE_FLOAT);
            return sqlite3_column_double(_stmt, index);
        }
        if constexpr (std::is_same_v<type, std::string> || std::is_same_v<type, std::string_view>) {
            if (checked_type != SQLITE_BLOB && checked_type != SQLITE_TEXT) {
                std::ostringstream err;
                err << get_type_name(SQLITE_BLOB) << '|' << get_type_name(SQLITE_TEXT);
                throw type_mismatch(checked_type, err.str().c_str());
            }
            const int size = sqlite3_column_bytes(_stmt, index);
            const char *data = reinterpret_cast<const char*>(checked_type == SQLITE_BLOB ?
                sqlite3_column_blob(_stmt, index) :
                sqlite3_column_text(_stmt, index)
            );
            return type{data, static_cast<std::size_t>(size)};
        }
    }

private:
    struct metadata {
        metadata(sqlite3 *db):
        _db{db},
        _column_names{} {

        }

        void init(sqlite3_stmt *stmt) {
            const int n = sqlite3_column_count(stmt);
            _column_names.clear();
            _column_names.reserve(static_cast<std::size_t>(n));
            for (int i = 0; i < n; ++i) {
                _column_names.emplace_back(sqlite3_column_name(stmt, i));
            }
        }

        int get_checked_type(sqlite3_stmt *stmt, const int index) const {
            check_range(index, static_cast<int>(_column_names.size()));
            return sqlite3_column_type(stmt, index);
        }

        sqlite3 *_db;
        std::vector<std::string> _column_names;
    };

    static constexpr const char *get_type_name(const int type) {
        switch (type) {
        case SQLITE_INTEGER:
            return "integer";
        case SQLITE_FLOAT:
            return "float";
        case SQLITE_BLOB:
            return "blob";
        case SQLITE3_TEXT:
            return "text";
        case SQLITE_NULL:
            return "null";
        default:
            return "unknown";
        }
    }

    static void check_type(const int type, const int expected_type) {
        if (type != expected_type) {
            throw type_mismatch(type, expected_type);
        }
    }

    static exception type_mismatch(const int type, const int expected_type) {
        return type_mismatch(type, get_type_name(expected_type));
    }

    static exception type_mismatch(const int type, const char *expected_message) {
        std::ostringstream err;
        err << exception{SQLITE_MISMATCH}.message();
        err << ": type=" << get_type_name(type) << ", expected=" << expected_message;
        return exception{SQLITE_MISMATCH, err.str()};
    }

    static exception range_error(const int index, const int size) {
        std::ostringstream err;
        err << exception{SQLITE_RANGE}.message();
        err << ": " << __func__ << ": index=" << index << ", size=" << size;
        return exception{SQLITE_RANGE, err.str()};
    }

    static void check_range(const int index, const int size) {
        if (index < 0 || index >= size) {
            throw range_error(index, size);
        }
    }

    query(sqlite3 *db, const std::string_view &sql):
    _stmt{nullptr},
    _metadata{db} {
        if (const int code = prepare(sql); !exception::is_ok(code)) {
            this->~query();
            throw exception{code};
        }
        _metadata.init(_stmt);
    }

    int prepare(const std::string_view &sql) {
        return sqlite3_prepare(_metadata._db, sql.data(), static_cast<int>(sql.size()), &_stmt, nullptr);
    }

    int get_checked_bind_index(const int index) const {
        check_range(index, sqlite3_bind_parameter_count(_stmt));
        return index + 1;
    }

    query(const query&) = delete;
    query &operator=(const query&) = delete;

    sqlite3_stmt *_stmt;
    metadata _metadata;
};

}

#endif
