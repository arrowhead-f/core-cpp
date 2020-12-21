////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      selftest, mockdbase, sql
/// Date:      2020-12-12
/// Author(s): ng201
///
/// Description:
/// * Test different sql queries with the MockDBase
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include "hlpr/MockDBase.h"


TEST_CASE("[mockdbase]: Test SELECT", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "c3" }, { {1, "b'b", "3"}, {2, "a'a", "b"}, {3, "c'c", "D"} });

    int i;

    REQUIRE(mdb.fetch("SELECT c1 FROM table1 WHERE c3 = 'D'", i) == true);
    REQUIRE(i == 3);
}


TEST_CASE("[mockdbase]: Test WHERE, AND and OR", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "c3" }, { {1, "b'b", "3"}, {2, "a'a", "b"}, {3, "c'c", "D"}, {4, "a'a", "q"} });

    int i = 0;

    REQUIRE(mdb.fetch("SELECT c3 FROM table1 WHERE c1 >= 1000", i) == false);

    REQUIRE(mdb.fetch("SELECT c1 FROM table1 WHERE c3 = 'X' OR c3 = 'b'", i) == true);
    REQUIRE(i == 2);

    REQUIRE(mdb.fetch("SELECT c1 FROM table1 WHERE c2 = 'a''a' AND c3 = 'q'", i) == true);
    REQUIRE(i == 4);
}


TEST_CASE("[mockdbase]: Test ORDER BY", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "c3" }, { {2, "b'b", "55"}, {1, "a'a", "66"}, {3, "c'c", "44"} });

    int i;

    auto row = mdb.fetch("SELECT c2, c3 FROM table1 ORDER BY c1");
    REQUIRE(row);

    std::string c2, c3;
    do {
        std::string res;

        row->get(0, res);
        c2 += res;

        row->get(1, res);
        c3 += res;

    } while(row->next());

    REQUIRE(c2 == "a'ab'bc'c");
    REQUIRE(c3 == "665544");
}


TEST_CASE("[mockdbase]: Test INSERT", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "c3" }, { {1, "b'b", "3"}, {2, "a'a", "b"}, {3, "c'c", "D"} });
    mdb.unique("table1", "c2");

    CHECK_NOTHROW(mdb.query("INSERT INTO table1 (c2, c3) VALUES ('d''d', 'U')"));  // existing table

    CHECK_THROWS(mdb.query("INSERT INTO table1 (c2, c4) VALUES ('d''d', 'U')"));   // existing table, wrong col
    CHECK_THROWS(mdb.query("INSERT INTO table2 (c2, c3) VALUES ('d''d', 'U')"));   // not existing table
    CHECK_THROWS(mdb.query("INSERT INTO table1 (c2, c3) VALUES ('a''a', 'U')"));   // unique value is not satisfied

    std::string s;
    REQUIRE(mdb.fetch("SELECT c2 FROM table1 WHERE c1 = 4", s) == true);
    REQUIRE(s == "d'd");
}


TEST_CASE("[mockdbase]: Test RETURNING", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "c3" }, { {1, "b'b", "3"}, {2, "a'a", "b"}, {3, "c'c", "D"} });
    mdb.unique("table1", "c2");

    int i = 0;
    REQUIRE(mdb.insert("INSERT INTO table1 (c2, c3) VALUES ('d''d', 'Q')", i) == true);
    REQUIRE(i == 4);

    std::string s;
    REQUIRE(mdb.fetch("SELECT c2 FROM table1 WHERE c1 = 4", s) == true);
    REQUIRE(s == "d'd");
}


TEST_CASE("[mockdbase]: Test COUNT", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", false, { "c1", "c2", "c3" }, { {1, "b'b", "3"}, {2, "a'a", "b"}, {3, nullptr, "D"} });

    int i;

    REQUIRE(mdb.fetch("SELECT COUNT(*) FROM table1", i) == true);
    REQUIRE(i == 3);

    REQUIRE(mdb.fetch("SELECT COUNT(c2) FROM table1", i) == true);
    REQUIRE(i == 2);
}


TEST_CASE("[mockdbase]: Test INNER JOIN", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", false, { "id", "c2",  "c3" }, { {1, "b'b", "3'3"}, {2, "a'a", "4'4"}, {3, "c'c", "5'5"} });

    mdb.table("table2", false, { "id", "fk1", "c3" }, { {11, 1, "a"}, {12, 2, "b"}, {13, 3, "c"} });     // fk1 is a foreign key to table1
    mdb.table("table3", false, { "id", "fk2", "c3" }, { {1, 11, "aa"}, {2, 12, "bb"}, {3, 13, "cc"} });  // fk2 is a foreign key to table2


    std::string s;

    REQUIRE(mdb.fetch("SELECT t1.c3 FROM table2 t2 INNER JOIN table1 t1 ON (t2.fk1 = t1.id) WHERE t2.c3 = 'b'", s) == true);
    REQUIRE(s == "4'4");

    REQUIRE(mdb.fetch("SELECT t1.c2 FROM table3 t3 INNER JOIN table2 t2 ON (t3.fk2 = t2.id) INNER JOIN table1 t1 ON (t2.fk1 = t1.id) WHERE t3.c3 = 'cc'", s) == true);
    REQUIRE(s == "c'c");
}


TEST_CASE("[mockdbase]: Test LEFT JOIN", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", false, { "id", "c2",  "c3" }, { {1, "b'b", "3'3"}, {2, "a'a", "4'4"}, {3, "c'c", "5'5"} });
    mdb.table("table2", false, { "id", "fk1", "c3" }, { {11, 1, "a"}, {12, 2, "b"}, {13, 5, "c"} });     // fk1 is a foreign key to table1

    std::string s;

    REQUIRE(mdb.fetch("SELECT t1.c3 FROM table2 t2 LEFT OUTER JOIN table1 t1 ON (t2.fk1 = t1.id) WHERE t2.c3 = 'c'", s) == false);  // NULL
    REQUIRE(mdb.fetch("SELECT t1.c3 FROM table2 t2 LEFT OUTER JOIN table1 t1 ON (t2.fk1 = t1.id) WHERE t2.c3 = 'b'", s) == true);   // 4'4

    auto row = mdb.fetch("SELECT t2.id, t1.c3 FROM table2 t2 LEFT OUTER JOIN table1 t1 ON (t2.fk1 = t1.id) WHERE t2.c3 = 'c'");
    REQUIRE(row);

    std::size_t count = 0;
    bool res = false;
    do {

        res = row->get(1, s);

        count++;
    } while(row->next());


    REQUIRE(count == 1);   // there was one matching row
    REQUIRE(res == false);
}


TEST_CASE("[mockdbase]: Test GROUP BY", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "c3" }, { {1, "bbb", "3"}, {2, "aaa", "3"}, {3, "rrr", "2"}, {4, "OOO", "1"}, {5, "qqq", "2"}, {6, "ccc", "3"} });

    auto row = mdb.fetch("SELECT MAX(c2) FROM table1 GROUP BY c3");
    REQUIRE(row);

    std::size_t count = 0;
    std::string s;
    bool res = false;

    do {
        std::string m;

        res = row->get(0, m);
        REQUIRE(res == true);

        s += m;

        count++;
    } while(row->next());


    REQUIRE(count == 3);   // there were 3 groups

    REQUIRE(s.length() == 9);
    REQUIRE(s.find("ccc") != std::string::npos);
    REQUIRE(s.find("rrr") != std::string::npos);
    REQUIRE(s.find("OOO") != std::string::npos);
}


TEST_CASE("[mockdbase]: Test UPDATE", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "c3" }, { {1, "bbb", "3"}, {2, "aaa", "4"}, {3, nullptr, "5"} });

    std::string s;
    REQUIRE(mdb.fetch("SELECT c2 FROM table1 WHERE c1 = 3", s) == false);

    mdb.query("UPDATE table1 SET c2 = 'ccc' WHERE c1 = 3");

    REQUIRE(mdb.fetch("SELECT c2 FROM table1 WHERE c1 = 3", s) == true);
    REQUIRE(s == "ccc");
}


TEST_CASE("[mockdbase]: Test subquery", "[selftest] [mockdbase] [sql]") {

    MockDBase mdb;

    mdb.table("table1", false, { "c1", "c2", "c3" }, { {1, "bbb", "3"}, {2, "aaa", "4"}, {3, "ccc", "5"} });
    mdb.table("table2", false, { "c1", "c2", "c3" }, { {4, "ddd", "3"}, {5, "eee", "4"}, {6, "fff", "5"} });
    mdb.table("table3", false, { "c1", "c2", "c3" }, { {1, "b", "3"}, {3, "c", "5"}, {4, "d", "3"}, {5, "e", "4"}, {6, "f", "5"} });


    mdb.query("INSERT INTO table1 (c1, c2, c3) SELECT t2.c1, t2.c2, t2.c3 FROM table2 t2 WHERE t2.c1 > 4");

    int i;

    REQUIRE(mdb.fetch("SELECT COUNT(*) FROM table1", i) == true);
    REQUIRE(i == 5);

    mdb.query("UPDATE table1 SET c2 = (SELECT table3.c2 FROM table3 WHERE table3.c1 = table1.c1)");

    REQUIRE(mdb.fetch("SELECT COUNT(*) FROM table1", i) == true);
    REQUIRE(i == 5);

    auto row = mdb.fetch("SELECT c2 FROM table1 ORDER BY c1");
    REQUIRE(row);

    std::size_t count = 0;
    std::string s;
    do {
        std::string c2;

        row->get(0, c2);

        s += c2;
        count++;
    } while(row->next());


    REQUIRE(count == 5);
    REQUIRE(s == "bcef");
}
