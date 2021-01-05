////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      inifile
/// Date:      2020-08-27
/// Author(s): ng201
///
/// Description:
/// * This one checks the parse_opt functionality of the INIFile reader/parser
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include "utils/inifile.h"


TEST_CASE("inifile: Test parsing simple values", "[inifile]") {

    auto res = INIFile::parse_opt("key1=val1 key2=val2 key3=val3");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == "val3");
}


TEST_CASE("inifile: Test empty spaces around '='", "[inifile]") {

    auto res = INIFile::parse_opt("key1 =    val1     key2=val2 key3=val3");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == "val3");
}


TEST_CASE("inifile: Test empty spaces before key", "[inifile]") {

    auto res = INIFile::parse_opt("     key1 =   val1     key2=val2");

    REQUIRE(res.size() == 2);

    REQUIRE(res["key1"] == "val1");
    REQUIRE(res["key2"] == "val2");
}


TEST_CASE("inifile: Test spaces in values", "[inifile]") {

    auto res = INIFile::parse_opt("key1 = 'val1 val1'    key2='val2 ' key3=' val3 val3'");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1 val1");
    REQUIRE(res["key2"] == "val2 ");
    REQUIRE(res["key3"] == " val3 val3");
}


TEST_CASE("inifile: Test \\n, \\t, \\r\\n as delimiters", "[inifile]") {

    auto res1 = INIFile::parse_opt("key1 = 'val1 \n val1'\nkey2='val2'\n key3=' val3' \nkey4='val4\n'");
    auto res2 = INIFile::parse_opt("key1 = 'val1 \t val1'\tkey2='val2'\t key3=' val3' \tkey4='val4\t'");
    auto res3 = INIFile::parse_opt("key1 = 'val1 \r\n val1'\r\nkey2='val2'\r\n key3=' val3' \r\nkey4='val4\r\n'");

    REQUIRE(res1.size() == 4);
    REQUIRE(res2.size() == 4);
    REQUIRE(res3.size() == 4);

    REQUIRE(res1["key1"] == "val1 \n val1");
    REQUIRE(res1["key2"] == "val2");
    REQUIRE(res1["key3"] == " val3");
    REQUIRE(res1["key4"] == "val4\n");

    REQUIRE(res2["key1"] == "val1 \t val1");
    REQUIRE(res2["key2"] == "val2");
    REQUIRE(res2["key3"] == " val3");
    REQUIRE(res2["key4"] == "val4\t");

    REQUIRE(res3["key1"] == "val1 \r\n val1");
    REQUIRE(res3["key2"] == "val2");
    REQUIRE(res3["key3"] == " val3");
    REQUIRE(res3["key4"] == "val4\r\n");
}
