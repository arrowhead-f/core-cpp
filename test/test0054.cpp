////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      jsonwriter
/// Date:      2020-08-27
/// Author(s): ng201
///
/// Description:
/// * Test json writer
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <utility>

#include "utils/json.h"

#include "hlpr/JsonComp.h"


namespace {

    struct IntValue {
        static constexpr const char *key = "intValue";
        long value;
    };

    struct DoubleValue {
        static constexpr const char *key = "doubleValue";
        double value;
    };

    struct StringValue {
        static constexpr const char *key = "stringValue";
        const std::string &value;
    };

}


TEST_CASE("jsonwriter: Create dictionary", "[jsonwriter]") {

    const auto res = (JsonBuilder{} << IntValue{ 42 } << StringValue{ "2020-11-29" } << DoubleValue{ 3.1415 }).str();

    REQUIRE(JsonCompare(res, R"json({"intValue":42,"stringValue":"2020-11-29","doubleValue":3.1415})json") == true);
}


TEST_CASE("jsonwriter: Create array with to_array", "[jsonwriter]") {

    {
        std::vector<int> data{ 1,2,3,4,5,6 };
        const auto res = (JsonBuilder{}.to_array("key", std::cbegin(data), std::cend(data))).str();
        REQUIRE(JsonCompare(res, R"json({"key": [1,2,3,4,5,6]})json"));
    }
    {
        std::vector<std::string> data{ "A","BB","CC","D" };
        const auto res = (JsonBuilder{}.to_array("key", std::cbegin(data), std::cend(data))).str();
        REQUIRE(JsonCompare(res, R"json({"key": ["A","BB","CC","D"]})json"));
    }
    {
        std::vector<int> data{ 1,2,3,4,5,6 };
        const auto res = (JsonBuilder{}.to_array("key", std::cbegin(data), std::cend(data), [](auto &out, auto data){ out << data; })).str();
        REQUIRE(JsonCompare(res, R"json({"key": [1,2,3,4,5,6]})json"));
    }
}


TEST_CASE("jsonwriter: Create array of dicts", "[jsonwriter]") {

    std::vector<std::pair<int, std::string>> data{ {12, "alma"}, {13, "korte"}, {73, "szilva"}, {7, "barack"} };

    const auto res = (JsonBuilder{}.to_array("YYZ", std::cbegin(data), std::cend(data), [](auto &out, auto data){ out << "{\"int\":" << data.first << ",\"str\":\"" << data.second << "\"}"; })).str();

    REQUIRE(JsonCompare(res, R"json({"YYZ": [{"int":12,"str":"alma"},{"int":13,"str":"korte"},{"int":73,"str":"szilva"},{"int":7,"str":"barack"}]})json"));
}


TEST_CASE("jsonwriter: Test for_each", "[jsonwriter]") {

    std::vector<std::pair<int, std::string>> data{ {12, "alma"}, {13, "korte"}, {73, "szilva"}, {7, "barack"} };

    const auto res = (JsonBuilder{}.for_each("YYZ", std::cbegin(data), std::cend(data), [](auto &builder, auto data){ builder << IntValue{ data.first } << StringValue{ data.second }; })).str();

    REQUIRE(JsonCompare(res, R"json({"YYZ": [{"intValue":12,"stringValue":"alma"},{"intValue":13,"stringValue":"korte"},{"intValue":73,"stringValue":"szilva"},{"intValue":7,"stringValue":"barack"}]})json"));
}


TEST_CASE("jsonwriter: Test to_array with empty range", "[jsonwriter]") {

    std::vector<int> data{};
    const auto res = (JsonBuilder{}.to_array("key", std::cbegin(data), std::cend(data))).str();

    REQUIRE(JsonCompare(res, R"json({"key": []})json"));

}


TEST_CASE("jsonwriter: Test for_each with empty range", "[jsonwriter]") {

    std::vector<std::pair<int, std::string>> data{};

    const auto res = (JsonBuilder{}.for_each("YYZ", std::cbegin(data), std::cend(data), [](auto &builder, auto data){ builder << IntValue{ data.first } << StringValue{ data.second }; })).str();

    REQUIRE(JsonCompare(res, R"json({"YYZ": []})json"));
}
