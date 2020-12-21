////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      jsonwriter
/// Date:      2020-12-06
/// Author(s): ng201
///
/// Description:
/// * Test json writer
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include <iostream>
#include "utils/json.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockDBase.h"


namespace {

    struct IntValue {
        static constexpr const char *key = "pieces";
        long value;
    };

    struct DoubleValue {
        static constexpr const char *key = "weight";
        double value;
    };

    struct StringValue {
        static constexpr const char *key = "fruit";
        const std::string &value;
    };

}


TEST_CASE("jsonwriter: Test db::Row with for_each", "[jsonwriter]") {

    MockDBase mdb;
    mdb.table("jar", false, { "col_1", "col_2", "col_3", "col_4" }, { {42, "peach", 3.1215, 4}, {43, "pear", 3.1215, 4}, {44, "plum", 3.1215, 4}  });

    auto row = mdb.fetch("SELECT * FROM jar ORDER BY col_1 ASC");

    const auto res = (JsonBuilder{}.for_each("fruit-jar", row, [](auto &builder, auto &row){
        int i;
        row->get(0, i);

        std::string s;
        row->get(1, s);

        builder << IntValue{ i } << StringValue{ s };
    })).str();

    REQUIRE(JsonCompare(res, R"json({"fruit-jar": [{"pieces": 42,"fruit": "peach"},{"pieces": 43,"fruit": "pear"},{"pieces": 44,"fruit": "plum"}]})json"));
}


TEST_CASE("jsonwriter: Test db::Row with to_array", "[jsonwriter]") {

    MockDBase mdb;
    mdb.table("jar", false, { "col_1", "col_2", "col_3", "col_4" }, { {42, "peach", 3.1215, 4}, {43, "pear", 3.1215, 4}, {44, "plum", 3.1215, 4}  });

    auto row = mdb.fetch("SELECT * FROM jar ORDER BY col_1 ASC");

    const auto res = (JsonBuilder{}.to_array("fruit-jar", row, [](auto &builder, auto &row){
        std::string s;
        row->get(1, s);

        builder << "\"" << s << "\"";
    })).str();

    REQUIRE(JsonCompare(res, R"json({"fruit-jar": ["peach","pear","plum"]})json"));
}


TEST_CASE("jsonwriter: Test for_each with empty row", "[jsonwriter]") {

    MockDBase mdb;
    mdb.table("jar", true, { "col_1", "col_2", "col_3", "col_4" }, { {42, "peach", 3.1215, 4}, {43, "pear", 3.1215, 4}, {44, "plum", 3.1215, 4}  });

    auto row = mdb.fetch("SELECT * FROM jar WHERE col_1 > 1000 ORDER BY col_1 DESC");

    const auto res = (JsonBuilder{}.for_each("fruit-jar", row, [](auto &builder, auto &row){
        int i;
        row->get(0, i);

        std::string s;
        row->get(1, s);

        builder << IntValue{ i } << StringValue{ s };
    })).str();

    REQUIRE(JsonCompare(res, R"json({"fruit-jar": []})json"));
}


TEST_CASE("jsonwriter: Test to_array with empty row", "[jsonwriter]") {

    MockDBase mdb;
    mdb.table("jar", true, { "col_1", "col_2", "col_3", "col_4" }, { {42, "peach", 3.1215, 4}, {43, "pear", 3.1215, 4}, {44, "plum", 3.1215, 4}  });

    auto row = mdb.fetch("SELECT * FROM jar WHERE col_1 > 1000 ORDER BY col_1 DESC");

    const auto res = (JsonBuilder{}.to_array("fruit-jar", row, [](auto &builder, auto &row){
        std::string s;
        row->get(1, s);

        builder << "\"" << s << "\"";
    })).str();

    REQUIRE(JsonCompare(res, R"json({"fruit-jar": []})json"));
}
