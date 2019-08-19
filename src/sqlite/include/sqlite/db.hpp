#ifndef SQLITE_DB_HPP
#define SQLITE_DB_HPP

#include <iostream>
#include <string_view>
#include <utility>

#include "sqlite3.h"

#include "exception.hpp"
#include "query.hpp"

namespace sqlite {

class db final {
    static constexpr const char *in_memory_db_filename = ":memory:";

public:
    static constexpr std::string_view version() {
        return SQLITE_VERSION;
    }

    db():
    db{in_memory_db_filename} {

    }

    db(const char *filename):
    _db{nullptr} {
        if (const int code = sqlite3_open(filename, &_db); !exception::is_ok(code)) {
            this->~db();
            throw exception{code};
        }
    }

    db(db &&rhs) noexcept:
    _db{std::exchange(rhs._db, nullptr)} {

    }

    db &operator=(db &&rhs) noexcept {
        this->~db();
        _db = std::exchange(rhs._db, nullptr);
        return *this;
    }

    ~db() noexcept {
        if (!_db) {
            return;
        }
        if (const int code = sqlite3_close(_db); exception::is_ok(code)) {
            _db = nullptr;
        } else {
            std::cerr << __func__ << ": " << exception{code} << std::endl;
        }
    }

    query prepare(const std::string_view &sql) const {
        return {_db, sql};
    }

private:
    db(const db&) = delete;
    db &operator=(const db&) = delete;

    sqlite3 *_db;
};

}

#endif
