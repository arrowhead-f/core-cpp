////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      rbcurl
/// Date:      2020-12-12
/// Author(s): ng201
///
/// Description:
/// * [rbcurl] - Testing the curl based request builder
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include <iostream>
#include <chrono>
#include <future>
#include <string>
#include <thread>

#include "http/KeyProvider.h"
#include "http/wget/WG_Curl.h"

#include "hlpr/MockServlet.h"


TEST_CASE("rbcurl: Test connection to an on-line server", "[rbcurl] [client]") {

    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");
    auto rb = WG_Curl{ kp };

    auto th = std::thread{ [](){
        char key[] = "123456";
        MockServlet::run("data/test0093/server.pem", "data/test0093/server.key", key);
    } };
    MockServlet::ServletGuard _{ th };

    while(!MockServlet::ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int port = MockServlet::port;

    {
        auto ret = rb.send("GET", "https://127.0.0.1/echo", port, "");

        REQUIRE(ret == http::status_code::OK);
        REQUIRE(ret.is_ok());

        REQUIRE(ret.value() == "<html><body><h1>Winnie-the-Pooh</h1></body></html>");
    }

    {
        auto ret = rb.send("GET", "https://127.0.0.1/json/echo", port, "");

        REQUIRE(ret == http::status_code::OK);
        REQUIRE(ret.is_ok());

        REQUIRE(ret.value() == "{\"Piglet\": 12,\"Tigger\": [12, 13, 15]}");
    }

    {
        auto ret = rb.send("GET", "https://127.0.0.1/bad-echo", port, "");

        REQUIRE(!ret.is_ok());
        REQUIRE(ret == http::status_code::BadRequest);
    }

    {
        auto ret = rb.send("GET", "https://127.0.0.1/json/bad-echo", port, "");

        REQUIRE(!ret.is_ok());
        REQUIRE(ret == http::status_code::BadRequest);
    }

    {
        auto ret = rb.send("GET", "https://127.0.0.1/not-found", port, "");

        REQUIRE(!ret.is_ok());
        REQUIRE(ret.has_status_code());
        REQUIRE(ret == http::status_code::NotFound);
    }

    {

        auto ret = rb.send("GET", "https://127.0.0.1/json/not-found", port, "");

        REQUIRE(!ret.is_ok());
        REQUIRE(ret.has_status_code());
        REQUIRE(ret == http::status_code::NotFound);
    }

}


TEST_CASE("rbcurl: Test the reuse of req", "[rbcurl] [client]") {

    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");
    auto rb = WG_Curl{ kp };

    auto th = std::thread{ [](){
        char key[] = "123456";
        MockServlet::run("data/test0093/server.pem", "data/test0093/server.key", key);
    } };
    MockServlet::ServletGuard _{ th };

    while(!MockServlet::ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int port = MockServlet::port;

    auto req = rb.req();

    {
        auto ret = req->send("GET", "https://127.0.0.1/echo", port, "");

        REQUIRE(ret == http::status_code::OK);
        REQUIRE(ret.is_ok());

        REQUIRE(ret.value() == "<html><body><h1>Winnie-the-Pooh</h1></body></html>");
    }

    {
        auto ret = req->send("GET", "https://127.0.0.1/json/echo", port, "");

        REQUIRE(ret == http::status_code::OK);
        REQUIRE(ret.is_ok());

        REQUIRE(ret.value() == "{\"Piglet\": 12,\"Tigger\": [12, 13, 15]}");
    }

    {
        auto ret = req->send("GET", "https://127.0.0.1/bad-echo", port, "");

        REQUIRE(!ret.is_ok());
        REQUIRE(ret == http::status_code::BadRequest);
    }

    {
        auto ret = req->send("GET", "https://127.0.0.1/json/bad-echo", port, "");

        REQUIRE(!ret.is_ok());
        REQUIRE(ret == http::status_code::BadRequest);
    }

}


TEST_CASE("rbcurl: Test reqs from multiple threads", "[rbcurl] [client]") {

    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");
    auto rb = WG_Curl{ kp };

    auto th = std::thread{ [](){
        char key[] = "123456";
        MockServlet::run("data/test0093/server.pem", "data/test0093/server.key", key);
    } };
    MockServlet::ServletGuard _{ th };

    while(!MockServlet::ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int port = MockServlet::port;

    auto reqA = rb.req();
    auto reqB = rb.req();
    auto reqC = rb.req();

    std::promise<void> sig;
    std::future<void> flag = sig.get_future();

    auto futA = std::async([&reqA, &port, &flag]() {
        {
            auto ret = reqA->send("GET", "https://127.0.0.1/echo", port, "");

            REQUIRE(ret == http::status_code::OK);
            REQUIRE(ret.is_ok());

            REQUIRE(ret.value() == "<html><body><h1>Winnie-the-Pooh</h1></body></html>");
        }

        flag.wait();

        {
            auto ret = reqA->send("GET", "https://127.0.0.1/echo", port, "");

            REQUIRE(ret == http::status_code::OK);
            REQUIRE(ret.is_ok());

            REQUIRE(ret.value() == "<html><body><h1>Winnie-the-Pooh</h1></body></html>");
        }
    });

    auto futB = std::async([&reqB, &port]() {
        auto ret = reqB->send("GET", "https://127.0.0.1/echo", port, "");

        REQUIRE(ret == http::status_code::OK);
        REQUIRE(ret.is_ok());

        REQUIRE(ret.value() == "<html><body><h1>Winnie-the-Pooh</h1></body></html>");
    });

    auto futC = std::async([&reqC, &port, &sig]() {
        auto ret = reqC->send("GET", "https://127.0.0.1/echo", port, "");

        sig.set_value();

        REQUIRE(ret == http::status_code::OK);
        REQUIRE(ret.is_ok());

        REQUIRE(ret.value() == "<html><body><h1>Winnie-the-Pooh</h1></body></html>");
    });

    // wait for all the futures
    futA.wait();
    futB.wait();
    futC.wait();

}


TEST_CASE("rbcurl: Test connecting to an offline server", "[rbcurl] [client]") {

    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");
    auto rb = WG_Curl{ kp };

    auto th = std::thread{ [](){
        char key[] = "123456";
        MockServlet::run("data/test0093/server.pem", "data/test0093/server.key", key);
    } };
    MockServlet::ServletGuard _{ th };

    while(!MockServlet::ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int port = MockServlet::port + 1;  // hopefuly an invalid port

    try {
        auto ret = rb.send("GET", "https://127.0.0.1/echo", port, "{}");

        REQUIRE(ret != http::status_code::OK);
        REQUIRE(!ret.is_ok());
        REQUIRE(ret.has_library_error());
    }
    catch(const std::exception &e) {
        REQUIRE(1 == 0);
    }

}


TEST_CASE("rbcurl: Test using http instead of https", "[rbcurl] [client]") {

    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");
    auto rb = WG_Curl{ kp };

    auto th = std::thread{ [](){
        char key[] = "123456";
        MockServlet::run("data/test0093/server.pem", "data/test0093/server.key", key);
    } };
    MockServlet::ServletGuard _{ th };

    while(!MockServlet::ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int port = MockServlet::port;
    auto ret = rb.send("GET", "http://127.0.0.1/echo", port, "{}");

    REQUIRE(ret != http::status_code::OK);
    REQUIRE(!ret.is_ok());
    REQUIRE(ret.has_library_error());

}


TEST_CASE("rbcurl: Test invalid protocol response", "[rbcurl] [client]") {

    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");
    auto rb = WG_Curl{ kp };

    auto th = std::thread{ [](){
        char key[] = "123456";
        MockServlet::run("data/test0093/server.pem", "data/test0093/server.key", key);
    } };
    MockServlet::ServletGuard _{ th };

    while(!MockServlet::ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int port = MockServlet::port;
    auto ret = rb.send("GET", "https://127.0.0.1/json/wrong-protocol", port, "{}");

    REQUIRE(ret.has_library_error());
    REQUIRE(ret != http::status_code::OK);

    MockServlet::stop = true;
}


TEST_CASE("rbcurl: Test slow response", "[rbcurl] [client]") {

    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");
    auto rb = WG_Curl{ kp };

    auto th = std::thread{ [](){
        char key[] = "123456";
        MockServlet::run("data/test0093/server.pem", "data/test0093/server.key", key);
    } };
    MockServlet::ServletGuard _{ th };

    while(!MockServlet::ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int port = MockServlet::port;
    auto ret = rb.send("GET", "https://127.0.0.1/slow-ok", port, "{}");

    REQUIRE(ret.has_status_code());
    REQUIRE(!ret.has_library_error());
    REQUIRE(ret == http::status_code::OK);

}



TEST_CASE("rbcurl: Test wrong content length set by the server", "[rbcurl] [client]") {

    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");
    auto rb = WG_Curl{ kp };

    auto th = std::thread{ [](){
        char key[] = "123456";
        MockServlet::run("data/test0093/server.pem", "data/test0093/server.key", key);
    } };
    MockServlet::ServletGuard _{ th };

    while(!MockServlet::ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int port = MockServlet::port;
    auto ret = rb.send("GET", "https://127.0.0.1/json/wrong-content-length", port, "{}");

    REQUIRE(ret.has_library_error());
    REQUIRE(ret != http::status_code::OK);

}


TEST_CASE("rbcurl: Test connection 201", "[rbcurl] [client]") {

    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");
    auto rb = WG_Curl{ kp };

    // set the response to created
    MockServlet::gen.store([](const std::string &method, const std::string &url, const std::string &full) -> std::string {
        if (method == "PUT" || method == "POST")
            return "HTTP/1.0 201 Created\r\nContent-Type: text/html\r\nContent-Length: 87\r\n\r\n<html><head><title>Created</title></head><body><h1>Hundred Acre Wood</h1></body></html>";

        return "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 90\r\n\r\n<html><head><title>Bad Reuest</title></head><body><h1>Hundred Acre Wood</h1></body></html>";
    });

    auto th = std::thread{ [](){
        char key[] = "123456";
        MockServlet::run("data/test0093/server.pem", "data/test0093/server.key", key);
    } };
    MockServlet::ServletGuard _{ th };

    while(!MockServlet::ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int port = MockServlet::port;
    auto req = rb.req();

    {
        auto ret = req->send("POST", "https://127.0.0.1/create", port, "");

        REQUIRE(ret == http::status_code::Created);
        REQUIRE(ret.value() == "<html><head><title>Created</title></head><body><h1>Hundred Acre Wood</h1></body></html>");
    }

    {
        auto ret = req->send("PUT", "https://127.0.0.1/create", port, "");

        REQUIRE(ret == http::status_code::Created);
        REQUIRE(ret.value() == "<html><head><title>Created</title></head><body><h1>Hundred Acre Wood</h1></body></html>");
    }

    {
        auto ret = req->send("GET", "https://127.0.0.1/create", port, "");

        REQUIRE(!ret.is_ok());
        REQUIRE(ret == http::status_code::BadRequest);
        REQUIRE(ret.value() == "<html><head><title>Bad Reuest</title></head><body><h1>Hundred Acre Wood</h1></body></html>");
    }

}
