////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      selftest, helperdb
/// Date:      2020-12-07
/// Author(s): ng201
///
/// Description:
/// * Test the HelperDB class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include "hlpr/HelperDB.h"


TEST_CASE("[helperdb]: Test exception test", "[selftest] [helperdb]") {

    HelperDB db{ };

    db.table("table1", false, { "c1", "c2", "c3", "c4" }, {
        {1, "apple",      nullptr, 13},
        {2, "grape",      nullptr, 14},
        {3, "gooseberry", 3.1415,  15},
        {4, "plum",       3.1415,  16},
        {5, "cherry",     3.1415,  17},
        {6, "currant",    3.1415,  18},
        {7, "peach",      3.1415,  19},
    });

    // all possible outputs
    // f = call, e = exception
    //
    // f1f2f5f6
    // e1                         -> exception
    // f1e2f3f4f5f6
    // f1f2e5                     -> exception
    // f1e2e3                     -> exception
    // f1f2f5e6f7
    // f1e2f3e4                   -> exception
    // f1f2f5e6e7                 -> exception
    // f1e2f3f4e5                 -> exception
    // f1e2f3f4f5e6f7
    // f1e2f3f4f5e6e7             -> exception
    //                           -----------------
    //                            7. exceptions seen

    auto num = db.run_except([&db]() {
        int i;
        db.fetch("SELECT c4 FROM table1 WHERE c1 = 1");
        try {
            db.fetch("SELECT c4 FROM table1 WHERE c1 = 2");
        }
        catch(...){
            db.fetch("SELECT c4 FROM table1 WHERE c1 = 3");
            db.fetch("SELECT c4 FROM table1 WHERE c1 = 4");
        }
        db.fetch("SELECT c4 FROM table1 WHERE c1 = 5");
        try {
            db.fetch("SELECT c4 FROM table1 WHERE c1 = 6");
        }
        catch(...){
            db.fetch("SELECT c4 FROM table1 WHERE c1 = 7");
        }
    });

    REQUIRE(num == 7);
}


TEST_CASE("[helperdb]: Test that exception test uses the same db for each run", "[selftest] [helperdb]") {

    HelperDB db{ };

    db.table("table1", true, { "idx", "c2", "c3", "c4" }, {
        {1, "apple",      nullptr, 13},
        {2, "grape",      nullptr, 14},
        {3, "gooseberry", 3.1415,  15},
        {4, "plum",       3.1415,  16},
        {5, "cherry",     3.1415,  17},
        {6, "currant",    3.1415,  18},
        {7, "peach",      3.1415,  19},
    });

    // all possible outputs
    // f = call, e = exception
    //
    // f0f1f2f5f6
    // e0                         -> exception
    // f0e1                       -> exception
    // f0f1e2f3f4f5f6
    // f0f1f2e5                   -> exception
    // f0f1e2e3                   -> exception
    // f0f1f2f5e6f7
    // f0f1e2f3e4                 -> exception
    // f0f1f2f5e6e7               -> exception
    // f0f1e2f3f4e5               -> exception
    // f0f1e2f3f4f5e6f7
    // f0f1e2f3f4f5e6e7           -> exception
    //                           -----------------
    //                            8. exceptions seen

    db.query("INSERT INTO table1 (c2,c3,c4)  VALUES (\"raspberry\", 6.123, 10)");

    int i;
    bool result = db.fetch("SELECT COUNT(*) FROM table1", i);
    REQUIRE(result == true);
    REQUIRE(i == 8);

    auto num = db.run_except([&db]() {

        // f0
        db.query("INSERT INTO table1 (c2,c3,c4)  VALUES (\"strawberry\", 6.123, 10)");

        // f1
        int j;
        bool res = db.fetch("SELECT COUNT(*) FROM table1", j);

        // at this point the table should have 9 row; test that it was resetted
        REQUIRE(res == true);
        REQUIRE(j == 9);

        try {
            db.fetch("SELECT c4 FROM table1 WHERE idx = 2");
        }
        catch(...){
            db.fetch("SELECT c4 FROM table1 WHERE idx = 3");
            db.fetch("SELECT c4 FROM table1 WHERE idx = 4");
        }
        db.fetch("SELECT c4 FROM table1 WHERE idx = 5");

        try {
            db.fetch("SELECT c4 FROM table1 WHERE idx = 6");
        }
        catch(...){
            db.fetch("SELECT c4 FROM table1 WHERE idx = 7");
        }
    });

    REQUIRE(num == 8);
}
