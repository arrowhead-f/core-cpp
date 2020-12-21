////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      selftest, jsoncomp
/// Date:      2020-12-05
/// Author(s): ng201
///
/// Description:
/// * Test json object comparison with JsonComp
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include "hlpr/JsonComp.h"


TEST_CASE("[jsoncomp]: Same string representation", "[selftest] [jsoncomp]") {

    const auto *json1 = R"json({ "name": "Yoda", "age": 900, "planet": "Dagobah" })json";
    const auto *json2 = R"json({ "name": "Yoda", "age": 900, "planet": "Dagobah" })json";

    REQUIRE(JsonCompare(json1, json2) == true);
}


TEST_CASE("[jsoncomp]: Whitespace differences", "[selftest] [jsoncomp]") {

    const auto *json1 = R"json({
                                 "name":   "Yoda",
                                 "age":    900,
                                 "planet": "Dagobah"
                               })json";
    const auto *json2 = R"json({ "name": "Yoda", "age": 900, "planet": "Dagobah" })json";

    REQUIRE(JsonCompare(json1, json2) == true);
}


TEST_CASE("[jsoncomp]: Dictionaries are not ordered", "[selftest] [jsoncomp]") {

    const auto *json1 = R"json({
                                  "name":   "Yoda",
                                  "age":    900,
                                  "planet": "Dagobah"
                               })json";
    const auto *json2 = R"json({
                                  "name":   "Yoda",
                                  "planet": "Dagobah",
                                  "age":    900
                               })json";

    REQUIRE(JsonCompare(json1, json2) == true);
}


TEST_CASE("[jsoncomp]: Arrays are ordered", "[selftest] [jsoncomp]") {

    const auto *json1 = R"json({
                                  "name": "Yoda",
                                  "age":  900,
                                  "padawans": [ "Mace Windu", "Ki-Adi Mundi", "Kit Fisto", "Ikrit", "Obi-Wan", "Ahsoka Tano" ]
                               })json";
    const auto *json2 = R"json({
                                  "name": "Yoda",
                                  "age":  900,
                                  "padawans": [ "Obi-Wan", "Mace Windu", "Ki-Adi Mundi", "Kit Fisto", "Ikrit", "Ahsoka Tano" ]
                               })json";
    const auto *json3 = R"json({
                                  "name": "Yoda",
                                  "age":  900,
                                  "padawans": [ "Obi-Wan",             "Mace Windu","Ki-Adi Mundi","Kit Fisto",    "Ikrit", "Ahsoka Tano" ]
                               })json";

    REQUIRE(JsonCompare(json1, json2) == false);  // the order inside the array is different
    REQUIRE(JsonCompare(json2, json3) == true);   // only the whitespaces inside the array are different
}


TEST_CASE("[jsoncomp]: Test validity of string representations", "[selftest] [jsoncomp]") {

    const auto *json1 = R"json({ "name": "Yoda", "age": 900, "planet": "Dagobah" })json";
    const auto *json2 = R"json({ "name": "Yoda,  "age": 900, "planet": "Dagobah" })json";
    const auto *json3 = R"json({ "name": "Yoda", "age": 900, "planet": "Dagobah", "citation": "\"Wars not make one great.\"" })json";
    const auto *json4 = R"json({
                                  "name":   "Yoda"
                                  "planet": "Dagobah",
                                  "age":    900
                               })json";

    REQUIRE(JsonParse(json1) == true);
    REQUIRE(JsonParse(json2) == false);  // missing " after Yoda
    REQUIRE(JsonParse(json3) == true);   // parse escaped character
    REQUIRE(JsonParse(json4) == false);  // missing , after the first key of the dictionary 
}
