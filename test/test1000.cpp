////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      selftest, mockcurl
/// Date:      2020-12-05
/// Author(s): ng201
///
/// Description:
/// * Test the MockCurl class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include "hlpr/MockCurl.h"


TEST_CASE("[mockcurl]: Get single response", "[selftest] [mockcurl]") {

    auto mc = MockCurl{ { "METHOD:url", { 200, "apple"} } };

    const auto res1 = mc.send("METHOD", "url", "payload");
    REQUIRE(res1 == http::status_code::OK);
    REQUIRE(res1.value() == "apple");

    const auto res2 = mc.send("METHOD", "url", "payload");
    REQUIRE(res2 == http::status_code::OK);
    REQUIRE(res2.value() == "apple");

    const auto res3 = mc.send("METHOD", "url", "payload");
    REQUIRE(res3 == http::status_code::OK);
    REQUIRE(res3.value() == "apple");
}


TEST_CASE("[mockcurl]: Get multiple results", "[selftest] [mockcurl]") {

    auto mc = MockCurl{ { "METHOD:url", { 200, "apple"} },
                        { "METHOD:url", { 201, "peach"} },
                        { "METHOD:url", { 202, "melon"} }
    };

    const auto res1 = mc.send("METHOD", "url", 15000, "payload");
    REQUIRE(res1 == http::status_code{ 200 });
    REQUIRE(res1.value() == "apple");

    const auto res2 = mc.send("METHOD", "url", 15000, "payload");
    REQUIRE(res2 == http::status_code{ 201 });
    REQUIRE(res2.value() == "peach");

    const auto res3 = mc.send("METHOD", "url", 15000, "payload");
    REQUIRE(res3 == http::status_code{ 202 });
    REQUIRE(res3.value() == "melon");

    const auto res4 = mc.send("METHOD", "url", 15000, "payload");
    REQUIRE(res4 == http::status_code{ 200 });
    REQUIRE(res4.value() == "apple");

    const auto res5 = mc.send("METHOD", "url", 15000, "payload");
    REQUIRE(res5 == http::status_code{ 201 });
    REQUIRE(res5.value() == "peach");
}


TEST_CASE("[mockcurl]: Use different methods with same url", "[selftest] [mockcurl]") {

    auto mc = MockCurl{ { "GET:url", { 200, "apple"} },
                        { "GET:url", { 200, "peach"} },
                        { "POST:url", { 200, "melon"} },
                        { "url", { 200, "strawberry"} }
    };

    const auto res1 = mc.send("GET", "url", "payload");
    REQUIRE(res1 == http::status_code::OK);
    REQUIRE(res1.value() == "apple");

    const auto res2 = mc.send("GET", "url", "payload");
    REQUIRE(res2 == http::status_code::OK);
    REQUIRE(res2.value() == "peach");

    const auto res3 = mc.send("POST", "url", "payload");
    REQUIRE(res3 == http::status_code::OK);
    REQUIRE(res3.value() == "melon");

    const auto res4 = mc.send("GET", "url", "payload");
    REQUIRE(res4 == http::status_code::OK);
    REQUIRE(res4.value() == "apple");

    const auto res5 = mc.send("PUT", "url", "payload");
    REQUIRE(res5 == http::status_code::OK);
    REQUIRE(res5.value() == "strawberry");

    const auto res6 = mc.send("GET", "url", "payload");
    REQUIRE(res6 == http::status_code::OK);
    REQUIRE(res6.value() == "peach");

}


TEST_CASE("[mockcurl]: Test  exception", "[selftest] [mockcurl]") {

    auto mc = MockCurl{ { "METHOD:url", { 200, "apple"} },
                        { "METHOD:url", { "an--except"} },
                        { "METHOD:url", { 200, "melon"} }
    };

    CHECK_NOTHROW([&mc]() {
        const auto res = mc.send("METHOD", "url", "payload");
        REQUIRE(res == http::status_code{ 200 });
        REQUIRE(res.value() == "apple");
    }());

    CHECK_THROWS([&mc]() {
        mc.send("METHOD", "url", "payload");
    }());

    CHECK_NOTHROW([&mc]() {
        const auto res = mc.send("METHOD", "url", "payload");
        REQUIRE(res == http::status_code{ 200 });
        REQUIRE(res.value() == "melon");
    }());

    CHECK_NOTHROW([&mc]() {
        const auto res = mc.send("METHOD", "url", "payload");
        REQUIRE(res == http::status_code{ 200 });
        REQUIRE(res.value() == "apple");
    }());

    CHECK_THROWS([&mc]() {
        mc.send("METHOD", "url", "payload");
    }());

}


TEST_CASE("[mockcurl]: Test req()", "[selftest] [mockcurl]") {

    auto mc = MockCurl{ { "GET:/fruit-jar", { 200, "pear"} },
                        { "GET:/fruit-jar", { 200, "plum"} }
    };

    auto req = mc.req();

    const auto res1 = req->send("GET", "/fruit-jar", "payload");
    REQUIRE(res1 == http::status_code::OK);
    REQUIRE(res1.value() == "pear");

    const auto res2 = req->send("GET", "/fruit-jar", "payload");
    REQUIRE(res2 == http::status_code::OK);
    REQUIRE(res2.value() == "plum");

    const auto res3 = req->send("GET", "/fruit-jar", "payload");
    REQUIRE(res3 == http::status_code::OK);
    REQUIRE(res3.value() == "pear");

}


TEST_CASE("[mockcurl]: Test undefined url", "[selftest] [mockcurl]") {

    auto mc = MockCurl{ { "GET:/fruit-jar", { 200, "pear"} },
                        { "GET:/fruit-jar", { 200, "plum"} }
    };

    auto req = mc.req();

    CHECK_THROWS([&req]() {
        req->send("POST", "/fruit-jar", "payload");
    }());

    CHECK_THROWS([&req]() {
        req->send("GET", "/fruits", "payload");
    }());

}


TEST_CASE("[mockcurl]: Use pattern matched URI", "[selftest] [mockcurl]") {

    auto mc = MockCurl{ { "P([^:])+:/fruit-jar", { 200, "apple"} } };

    CHECK_THROWS([&mc]() {
        mc.send("GET", "/fruit-jar", "payload");
    }());

    const auto res2 = mc.send("PATCH", "/fruit-jar", "payload");
    REQUIRE(res2 == http::status_code::OK);
    REQUIRE(res2.value() == "apple");

    const auto res3 = mc.send("PUT", "/fruit-jar", "payload");
    REQUIRE(res3 == http::status_code::OK);
    REQUIRE(res3.value() == "apple");
}


/*
TEST_CASE("[mockcurl]: Use custom transformer", "[selftest] [mockcurl]") {

        auto mc = MockCurl{ std::make_pair(302, ">>!!YYZ!!<<") };

        mc.useTFun([](const std::string &str, const std::string &url, const std::string &pld){return std::regex_replace(str, std::regex{"!!YYZ!!"}, pld);});
        const auto res1 = mc.send("method", "url", "Rush");
        REQUIRE(res1 == http::status_code{ 302 });
        REQUIRE(res1.value() == ">>Rush<<");

        mc.useTFun([](const std::string &str, const std::string &url, const std::string &pld){return std::regex_replace(str, std::regex{"!!YYZ!!"}, url);});
        const auto res2 = mc.send("method", "Tom Sawyer", "Rush");
        REQUIRE(res2 == http::status_code{ 302 });
        REQUIRE(res2.value() == ">>Tom Sawyer<<");

        mc.useTFun(false);
        const auto res3 = mc.send("method", "url", "apple");
        REQUIRE(res3 == http::status_code{ 302 });
        REQUIRE(res3.value() == ">>!!YYZ!!<<");
}
*/