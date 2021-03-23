////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      selftest, mockdbase
/// Date:      2020-12-05
/// Author(s): ng201
///
/// Description:
/// * Test the MockDBase class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include <chrono>
#include <thread>

#include "hlpr/MockDBase.h"


TEST_CASE("[mockdbase]: Test index column", "[selftest] [mockdbase]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "c3" }, {
        {1, "bbb",   "3"},
        {2, "a'a",   "b"},
        {3, nullptr, "D"}
    });

    CHECK_NOTHROW(mdb.query("INSERT INTO table1 (c2, c3) VALUES ('ccc', 'E')"));

    int i;

    REQUIRE(mdb.fetch("SELECT c1 FROM table1 WHERE c1 = 4", i) == true);
    REQUIRE(i == 4);
}


TEST_CASE("[mockdbase]: Read undefined table", "[selftest] [mockdbase]") {

    MockDBase mdb;

    CHECK_THROWS(mdb.query("SELECT * FROM table3"));
    CHECK_THROWS(mdb.query("INSERT INTO table1 (c2, c3) VALUES ('ccc', 'E')"));
}


TEST_CASE("[mockdbase]: Test fetch single numeric value", "[selftest] [mockdbase]") {

    MockDBase mdb;

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {43, "pear", 3.1415, 4},
        {42, "plum", 6.2830, 8},
    });

    SECTION("fetch int") {
        int i;

        REQUIRE(mdb.fetch("SELECT c4 FROM table1 WHERE c1 = 43", i) == true);
        REQUIRE(i == 4);

        REQUIRE(mdb.fetch("SELECT c1 FROM table1 WHERE c4 = 8", i) == true);
        REQUIRE(i == 42);

        REQUIRE(mdb.fetch("SELECT c4 FROM table1 WHERE c1 = 77", i) == false);
    }

    SECTION("fetch long") {
        long l;

        REQUIRE(mdb.fetch("SELECT c4 FROM table1 WHERE c1 = 43", l) == true);
        REQUIRE(l == 4);

        REQUIRE(mdb.fetch("SELECT c1 FROM table1 WHERE c4 = 8", l) == true);
        REQUIRE(l == 42);

        REQUIRE(mdb.fetch("SELECT c4 FROM table1 WHERE c1 = 77", l) == false);
    }

    SECTION("fetch double") {
        double d;

        REQUIRE(mdb.fetch("SELECT c3 FROM table1 WHERE c1 = 43", d) == true);
        REQUIRE(d == 3.1415);

        REQUIRE(mdb.fetch("SELECT c1 FROM table1 WHERE c4 = 8", d) == true);
        REQUIRE(d == 42);

        REQUIRE(mdb.fetch("SELECT c3 FROM table1 WHERE c1 = 77", d) == false);
    }

}


TEST_CASE("[mockdbase]: Test fetch single string value", "[selftest] [mockdbase]") {

    MockDBase mdb;

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {43, "pear", 3.1415, 4},
        {42, "plum", 6.2830, 8},
    });

    std::string s;

    REQUIRE(mdb.fetch("SELECT c2 FROM table1 WHERE c1 = 42", s) == true);
    REQUIRE(s == "plum");

    REQUIRE(mdb.fetch("SELECT c3 FROM table1 WHERE c1 = 43", s) == true);
    REQUIRE(s.substr(0, 6) == "3.1415");

    REQUIRE(mdb.fetch("SELECT c4 FROM table1 WHERE c1 = 42", s) == true);
    REQUIRE(s == "8");
}


TEST_CASE("[mockdbase]: Test different result types together", "[selftest] [mockdbase]") {

    MockDBase mdb{ };

    mdb.table("table_c", false, { "c1", "c2", "c3", "c4" }, { {3, "peach", 3.1415, 4} });
    mdb.table("table_b", false, { "b1", "b2", "b3" },       { {4, "peach", 3.1415} });
    mdb.table("table_a", false, { "a1", "a2" },             { {5, "peach"} });

    int i;
    REQUIRE(mdb.fetch("SELECT a1 FROM table_a WHERE a2 LIKE 'peach'", i) == true);
    REQUIRE(i == 5);

    double d = 0;
    REQUIRE(mdb.fetch("SELECT c3 FROM table_c WHERE c1 = 7", d) == false);

    std::string s;
    REQUIRE(mdb.fetch("SELECT b2 FROM table_b WHERE b1 = 4", s) == true);
    REQUIRE(s == "peach");

    CHECK_THROWS(mdb.fetch("SELECT q3 FROM table_b WHERE c1 = 7", s));
}


TEST_CASE("[mockdbase]: Check NULL response", "[selftest] [mockdbase]") {

    MockDBase mdb;

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {43, nullptr, 3.1415, 4},
        {42, nullptr, 6.2830, nullptr},
    });


    {
        int i;
        REQUIRE(mdb.fetch("SELECT c4 FROM table1 WHERE c1 = 42", i) == false);
    }

    {
        std::string s;
        REQUIRE(mdb.fetch("SELECT c2 FROM table1 WHERE c1 = 42", s) == false);
    }

    {
        std::string s;
        REQUIRE(mdb.fetch("SELECT c2 FROM table1 WHERE c4 IS NULL", s) == false);
    }

}


TEST_CASE("[mockdbase]: Test basic row features", "[selftest] [mockdbase]") {

    MockDBase mdb;

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {43, nullptr, 3.1415, 4},
        {42, nullptr, 6.2830, nullptr},
    });

    mdb.table("table2", false, { "c1", "c2", "c3", "c4" }, {});

    REQUIRE(mdb.fetch("SELECT * FROM table1"));
    REQUIRE(!mdb.fetch("SELECT * FROM table2"));
    CHECK_THROWS(mdb.fetch("SELECT * FROM table3"));
}


TEST_CASE("[mockdbase]: Test mutiple response count", "[selftest] [mockdbase]") {

    MockDBase mdb{ };

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {42, "peach", 3.1415, 3},
        {43, "peach", 3.1415, 2},
        {44, "peach", 3.1415, 1},
    });

    SECTION("row count") {
        auto row = mdb.fetch("SELECT * FROM table1");
        int count = 0;

        if(row) {
            do {
                count++;
            } while(row->next());
        }

        REQUIRE(count == 3);
    }

    SECTION("row entries") {
        auto row = mdb.fetch("SELECT * FROM table1");
        std::string result1, result2;

        if(row) {
            do {
                std::string res;

                row->get(0, res);
                result1 += res;

                row->get(3, res);
                result2 += res;
            } while(row->next());
        }

        REQUIRE(result1 == "424344");
        REQUIRE(result2 == "321");
    }
}


TEST_CASE("[mockdbase]: Test row's get method", "[selftest] [mockdbase]") {

    MockDBase mdb{ };

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {2, "peach", 3.1415, 4},
        {3, "peach", 3.1415, 4},
        {5, "peach", 3.1415, 4},
    });

    auto row = mdb.fetch("SELECT * FROM table1");
    int result = 1;
    int count  = 0;

    if(row) {
        do {
            count++;

            int res;
            row->get(0, res);
            result *= res;
        } while(row->next());
    }

    REQUIRE(count  == 3);
    REQUIRE(result == 30);
}


TEST_CASE("[mockdbase]: Test row's get method with NULL values", "[selftest] [mockdbase]") {

    MockDBase mdb{ };

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {2, "grape", nullptr, 4},
        {3, nullptr, 3.1415,  4},
        {5, nullptr, 3.1415,  4},
        {7, "peach", 3.1415,  4},
    });

    auto row = mdb.fetch("SELECT * FROM table1");

    int count = 0;
    int count_at_pos[4] { 0 };  // here we count nulls position-wise

    if(row) {
        do {
            count++;

            std::string res;
            for(int i = 0; i < 4; i++)
                count_at_pos[i] += ((row->get(i, res) == false) ? 1 : 0);

        } while(row->next());
    }

    REQUIRE(count == 4);
    REQUIRE(count_at_pos[0] == 0);
    REQUIRE(count_at_pos[1] == 2);
    REQUIRE(count_at_pos[2] == 1);
    REQUIRE(count_at_pos[3] == 0);
}


TEST_CASE("[mockdbase]: Test index past the row size", "[selftest] [mockdbase]") {

    MockDBase mdb{ };

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {2, "grape", nullptr, 4},
        {3, nullptr, 3.1415,  4},
        {5, nullptr, 3.1415,  4},
        {7, "peach", 3.1415,  4},
    });

    auto row = mdb.fetch("SELECT * FROM table1");

    REQUIRE(row);
    CHECK_THROWS([&](){
        if (row) {
            do {
                std::string res;
                row->get(4, res);  // index past the row size
            } while(row->next());
        }
    }());
}


TEST_CASE("[mockdbase]: Have only some columns in rows", "[selftest] [mockdbase]") {

    MockDBase mdb{ };

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {2, "grape", nullptr, 14},
        {3, nullptr, 3.1415,  15},
        {5, nullptr, 3.1415,  16},
        {7, "peach", 3.1415,  17},
    });

    auto row = mdb.fetch("SELECT c1, c2, c3 FROM table1");

    int count = 0;
    int count_at_pos[3] { 0 };  // here we count nulls position-wise

    if(row) {
        do {
            count++;

            std::string res;
            for(int i = 0; i < 3; i++)
                count_at_pos[i] += ((row->get(i, res) == false) ? 1 : 0);

        } while(row->next());
    }

    REQUIRE(count == 4);
    REQUIRE(count_at_pos[0] == 0);
    REQUIRE(count_at_pos[1] == 2);
    REQUIRE(count_at_pos[2] == 1);
}


TEST_CASE("[mockdbase]: Only the selected number of columns are returned", "[selftest] [mockdbase]") {

    MockDBase mdb{ };

    mdb.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {2, "grape", nullptr, 14},
        {3, nullptr, 3.1415,  15},
        {5, nullptr, 3.1415,  16},
        {7, "peach", 3.1415,  17},
    });

    auto row = mdb.fetch("SELECT c1, c2, c3 FROM table1");
    REQUIRE(row);
    CHECK_THROWS([&](){
        if (row) {
            do {
                std::string res;
                row->get(3, res);  // index past the row size
            } while(row->next());
        }
    }());
}


TEST_CASE("[mockdbase]: Test unique column", "[selftest] [mockdbase]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "c3" }, {
        {1, "bbb",   13},
        {2, "a'a",   14},
        {3, nullptr, 15}
    });

    // make column unique
    mdb.unique("table1", "c3");

    CHECK_THROWS(mdb.query("INSERT INTO table1 (c2, c3) VALUES ('ccc', 13)"));

    int i;
    REQUIRE(mdb.fetch("SELECT COUNT(*) FROM table1", i) == true);
    REQUIRE(i == 3);
}


TEST_CASE("[mockdbase]: Test date column", "[selftest] [mockdbase]") {

    MockDBase mdb;

    mdb.table("table1", true, { "c1", "c2", "created_at", "updated_at" }, {});
    mdb.date("table1", "c1", "created_at");
    mdb.date("table1", "c1", "updated_at", true);

    mdb.query("INSERT INTO table1 (c1, c2) VALUES (13, 'peach')");
    mdb.query("INSERT INTO table1 (c1, c2) VALUES (14, 'pear')");


    std::string cat13_1, cat14_1, uat13_1, uat14_1;
    REQUIRE(mdb.fetch("SELECT created_at FROM table1 WHERE c1 = 13", cat13_1) == true);
    REQUIRE(mdb.fetch("SELECT updated_at FROM table1 WHERE c1 = 13", uat13_1) == true);
    REQUIRE(mdb.fetch("SELECT created_at FROM table1 WHERE c1 = 14", cat14_1) == true);
    REQUIRE(mdb.fetch("SELECT updated_at FROM table1 WHERE c1 = 14", uat14_1) == true);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    mdb.query("UPDATE table1 SET c2 = 'peach' WHERE c1 = 14");

    std::string cat13_2, cat14_2, uat13_2, uat14_2;
    REQUIRE(mdb.fetch("SELECT created_at FROM table1 WHERE c1 = 13", cat13_2) == true);
    REQUIRE(mdb.fetch("SELECT updated_at FROM table1 WHERE c1 = 13", uat13_2) == true);
    REQUIRE(mdb.fetch("SELECT created_at FROM table1 WHERE c1 = 14", cat14_2) == true);
    REQUIRE(mdb.fetch("SELECT updated_at FROM table1 WHERE c1 = 14", uat14_2) == true);

    // creation time did not change
    REQUIRE(cat13_1 == cat13_2);
    REQUIRE(cat14_1 == cat14_2);

    // update time changed for the second row with id = 14
    REQUIRE(uat13_1 == uat13_2);
    REQUIRE(uat14_1 != uat14_2);
}
