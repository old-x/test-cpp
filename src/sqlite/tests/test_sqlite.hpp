#ifndef TEST_SQLITE_HPP
#define TEST_SQLITE_HPP

#include <cassert>

#include <sstream>
#include <utility>
#include <vector>

#include "sqlite/sqlite.hpp"

void test_exception() {
    std::ostringstream out;
    out << sqlite::exception{123, "test error message"};
    assert(out.str() == "[123] test error message");
}

void test_db_and_query() {
    assert(sqlite::db::version() == "3.29.0");

    sqlite::db db;
    assert(db.prepare("create table test (a int, b text)").exec() == 0);

    sqlite::query q1 = db.prepare("insert into test (a, b) values (?, ?), (?, ?), (?, ?), (?, ?), (?, ?)");
    q1.bind(0, 1).bind(1, "qwert");
    q1.bind(2, 2).bind(3, "asdfg");
    q1.bind(4, 3).bind(5, "zxcvb");
    q1.bind(6, 4).bind(7, "yuiop");
    q1.bind(8, 5).bind(9, "hjkl;");
    assert(q1.exec() == 5);

    q1.bind(4, 15).bind(5, "qazwsx");
    assert(q1.exec() == 5);

    sqlite::query q2 = db.prepare("select count(*) from test");
    assert(q2.exec() == 0);
    assert(q2.first());
    assert(q2.get<int>(0).value() == 10);

    sqlite::query q3 = db.prepare("select * from test where a > ?");
    q3.bind(0, 4);
    assert(q3.exec() == 0);
    std::vector<std::pair<int, std::string>> rows;
    while (q3.next()) {
        rows.push_back(std::make_pair(q3.get<int>(0).value(), q3.get<std::string>(1).value()));
    }
    assert(rows.size() == 3);
    assert(rows[0].first == 5);
    assert(rows[0].second == "hjkl;");
    assert(rows[1].first == 15);
    assert(rows[1].second == "qazwsx");
    assert(rows[2].first == 5);
    assert(rows[2].second == "hjkl;");
}

#endif
