////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      ???
/// Date:      2020-08-27
/// Author(s): ng201
///
/// Description:
/// * [parsers] - This will be some kind of config reader
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include "utils/parsers.h"


TEST_CASE("parsers: Options Parser I.", "[parsers]") {

    auto res = parser::parseOptions("key1=val1 key2=val2 key3=val3");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == "val3");
}


TEST_CASE("parsers: Options Parser II.", "parsers]") {

    auto res = parser::parseOptions("key1 =    val1     key2=val2 key3=val3");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == "val3");
}


TEST_CASE("parsers: Options Parser III.", "[parsers]") {

    auto res = parser::parseOptions("     key1 =   val1     key2=val2 key3=val3");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == "val3");
}


TEST_CASE("parsers: Options Parser IV.", "[parsers]") {

    auto res = parser::parseOptions("key1 = 'val1  val1'    key2='val2' key3=' val3'");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1  val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == " val3");
}
