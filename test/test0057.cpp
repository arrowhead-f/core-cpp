////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      jsonview
/// Date:      2022-04-18
/// Author(s): ng201
///
/// Description:
/// * Test JsonView and JsonCView
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include <iostream>
#include <string>

#include "utils/json/JsonCView.h"
#include "utils/json/JsonView.h"

#include "hlpr/JsonComp.h"


TEST_CASE("jsoncview: Simple parsing", "[jsonview]") {

    using namespace std::literals;

    std::string simple_test = R"json({ "k1":"v\"1", "k2":42, "k3": ["a",123,true,false,null] })json";

    const auto json = JsonCView{ simple_test.data() };

    REQUIRE(json.root().at("k1").as_string()       == "v\"1"s);
    REQUIRE(json.root().at("k2").as_int()          == 42);
    REQUIRE(json.root().value_or_null("k4").dump() == "null"s);

    REQUIRE(JsonCompare(json.root().at("k3").dump(), "[\"a\", 123, true, false, null]"));
    for (auto &k : json.root().at("k3").as_array()) {
        REQUIRE((k.dump() == "\"a\""s || k.dump() == "123"s || k.dump() == "true"s || k.dump() == "false"s || k.dump() == "null"s));
    }

    REQUIRE(JsonCompare(json.dump(), R"json({ "k1":"v\"1", "k2":42, "k3": ["a",123,true,false,null] })json"));
}


TEST_CASE("jsoncview: Parser error throws exception", "[jsonview]") {
    {
        std::string simple_test = R"json({ "k1":"v1, "k2":42, "k3": ["a",123,true,false,null] })json";
        CHECK_THROWS(JsonCView{ simple_test.data() });
    }

    {
        std::string simple_test = R"json({ "k1":"v\"1", "k2":42, "k3": "\u123" })json";
        CHECK_THROWS(JsonCView{ simple_test.data() });
    }
}


TEST_CASE("jsoncview: Test erase", "[jsonview]") {

    std::string simple_test = R"json({ "k1":"v1", "k2":42, "k3": ["a",123,true,false,null] })json";

    auto json = JsonCView{ simple_test.data() };

    json.root().as_object().erase("k2");
    REQUIRE(JsonCompare(json.dump(), R"json({ "k1":"v1", "k3": ["a",123,true,false,null] })json"));
}


TEST_CASE("jsoncview: Test as_bool and as_double", "[jsonview]") {

    using namespace std::literals;

    char simple_test[] = "{ \"k1\": false, \"k2\" : 42.2, \"k3\" : \"uu\\tuu\" }";
    const auto json = JsonCView{ simple_test };

    REQUIRE(json.root().at("k1").as_bool()   == false);
    REQUIRE(json.root().at("k2").as_double() == 42.2);
    REQUIRE(json.root().at("k3").as_string() == "uu\tuu"s);
}


TEST_CASE("jsoncview: Test is_null and as_number", "[jsonview]") {

    char simple_test[] = "{ \"k1\": true, \"k2\" : null, \"k3\" : 3.1415 }";
    const auto json = JsonCView{ simple_test };

    REQUIRE(json.root().at("k1").as_bool()   == true);
    REQUIRE(json.root().at("k2").is_null()   == true );
    REQUIRE(json.root().at("k3").as_number() == 3.1415);
}


TEST_CASE("jsonview: Simple parsing", "[jsonview]") {

    using namespace std::literals;

    const std::string simple_test = R"json({ "k1":"v\"1", "k2":42, "k3": ["a",123,true,false,null] })json";

    const auto json = JsonView{ simple_test.c_str() };

    REQUIRE(json.root().at("k1").as_string()       == "v\"1"s);
    REQUIRE(json.root().at("k2").as_int()          == 42);
    REQUIRE(json.root().value_or_null("k4").dump() == "null"s);

    REQUIRE(JsonCompare(json.root().at("k3").dump(), "[\"a\", 123, true, false, null]"));
    for (auto &k : json.root().at("k3").as_array()) {
        REQUIRE((k.dump() == "\"a\""s || k.dump() == "123"s || k.dump() == "true"s || k.dump() == "false"s || k.dump() == "null"s));
    }

    // this is not destructive, we can reuse siple_test
    REQUIRE(JsonCompare(json.dump(), simple_test));
}


TEST_CASE("jsonview: Parser error throws exception", "[jsonview]") {
    {
        const std::string simple_test = R"json({ "k1":"v1, "k2":42, "k3": ["a",123,true,false,null] })json";
        CHECK_THROWS(JsonView{ simple_test.c_str() });
    }

    {
        const std::string simple_test = R"json({ "k1":"v\"1", "k2":42, "k3": "\u123" })json";
        CHECK_THROWS(JsonView{ simple_test.c_str() });
    }
}


TEST_CASE("jsonview: Test erase", "[jsonview]") {

    const std::string simple_test = R"json({ "k1":"v1", "k2":42, "k3": ["a",123,true,false,null] })json";

    auto json = JsonView{ simple_test.c_str() };

    json.root().as_object().erase("k2");
    REQUIRE(JsonCompare(json.dump(), R"json({ "k1":"v1", "k3": ["a",123,true,false,null] })json"));

    auto &arr = json.root().at("k3").as_array(); arr.erase(1);
    REQUIRE(JsonCompare(json.dump(), R"json({ "k1":"v1", "k3": ["a",true,false,null] })json"));
}


TEST_CASE("jsonview: Change value", "[jsonview]") {

    const std::string simple_test = R"json({ "k1":"v1", "k2":42, "k3": ["a",123,true,false,null] })json";

    auto json = JsonView{ simple_test.c_str() };

    json.root().at("k1").as_string() = std::string_view("v2");
    json.root().at("k2").as_int() = 13;
    REQUIRE(JsonCompare(json.dump(), R"json({ "k1":"v2", "k2": 13, "k3": ["a",123,true,false,null] })json"));
}



TEST_CASE("jsonview: Test as_bool and as_double", "[jsonview]") {

    using namespace std::literals;

    const char *simple_test = "{ \"k1\": false, \"k2\" : 42.2, \"k3\" : \"uu\\tuu\" }";
    const auto json = JsonView{ simple_test };

    REQUIRE(json.root().at("k1").as_bool()   == false);
    REQUIRE(json.root().at("k2").as_double() == 42.2);
    REQUIRE(json.root().at("k3").as_string() == "uu\tuu"s);
}


TEST_CASE("jsonview: Test is_null and as_number", "[jsonview]") {

    const char *simple_test = "{ \"k1\": true, \"k2\" : null, \"k3\" : 3.1415 }";
    const auto json = JsonView{ simple_test };

    REQUIRE(json.root().at("k1").as_bool()   == true);
    REQUIRE(json.root().at("k2").is_null()   == true );
    REQUIRE(json.root().at("k3").as_number() == 3.1415);
}
