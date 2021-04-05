////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      https
/// Date:      2021-01-02
/// Author(s): ng201
///
/// Description:
/// * Test the HTTPS request parser
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include <iostream>
#include <cstring>
#include <list>
#include <string>
#include <thread>

#include "http/KeyProvider.h"
#include "http/serv/HTTPSServer.h"
#include "http/wget/WG_Curl.h"

#include "hlpr/MockCore.h"


template<typename MC>
class HTTPSServerGuard {

    private:

        HTTPSServer<MC> http;  ///< The guarded server.
        std::thread     th;    ///< The guarded thread.

    public:

        /// Create a https server on the given port in a separate thread.
        /// \param address          the IP address to use
        /// \param port             the listener port to use
        /// \param mc               the (probably mock) core element
        /// \param kp               the key provider
        /// \param thnum            the number of the threads; see HTTPSServer for the meaning
        HTTPSServerGuard(const std::string &address, std::size_t port, MC &mc, KeyProvider &kp, std::size_t thnum = 0)
            : http{ HTTPSServer<MockCore>{ address, port, mc, kp } }
            , th{ [this, thnum](){ http.run(thnum); } } {
        }

        /// Destructs the server and waits for its thread to finish.
        ~HTTPSServerGuard() {
            http.kill();
            th.join();
        }

        /// Returns a reference to the server.
        /// \return                 the server (reference)
        auto& server() {
            return http;
        }
};


TEST_CASE("https: Test https server's reachability", "[server] [https]") {

    auto mc = MockCore{};
    auto kp = KeyProvider{}.loadKeyStore("data/test0502/key-store", "cert", "123456").loadTrustStore("data/test0502/trust-store", "123456");

    std::size_t port = 12500;
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            {
                auto cl = WG_Curl{ kp };
                auto resp = cl.send("GET", "https://127.0.0.1/index.html", port, "");

                REQUIRE(resp == http::status_code::NotImplemented);
            }

            {
                 auto cl = WG_Curl{ kp };
                 auto resp = cl.send("GET", "https://127.0.0.1/index.html", port, "");

                 REQUIRE(resp == http::status_code::NotImplemented);
            }

            break;
        }
        catch(const HTTPServer::Error) {
            port++;
        }
        catch(...) {
            auto webget_error = true;
            REQUIRE_FALSE(webget_error);
        }
    }
}


TEST_CASE("https: HTTPS stress tests (optional parallelism)", "[server] [https]") {

    auto mc = MockCore{};
    auto kp = KeyProvider{}.loadKeyStore("data/test0502/key-store", "cert", "123456").loadTrustStore("data/test0502/trust-store", "123456");

    std::size_t port = 12500;  // the initial guess for an empty port
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            SECTION("Call server in series") {

                for (std::size_t i = 0; i < 25; i++) {

                    auto cl = WG_Curl{ KeyProvider{} };
                    auto resp = cl.send("GET", "https://127.0.0.1/index.html", port, "");

                    REQUIRE(resp == http::status_code::NotImplemented);
                }
            }

            SECTION("Call server in parallel") {

                std::list<std::thread> th;
                for (std::size_t i = 0; i < 25; i++) {
                    th.emplace_back([&port]() {
                        auto cl = WG_Curl{ KeyProvider{} };
                        auto resp = cl.send("GET", "https://127.0.0.1/index.html", port, "");

                        REQUIRE(resp == http::status_code::NotImplemented);
                    });
                }

                for (auto &t : th) {
                    t.join();
                }
            }

            SECTION("Call server with long timeout in parallel") {

                std::list<std::thread> th;
                for (std::size_t i = 0; i < 25; i++) {
                    th.emplace_back([&port]() {
                        auto cl = WG_Curl{ KeyProvider{} };
                        auto resp = cl.send("GET", "https://127.0.0.1/sleep/250", port, "");

                        REQUIRE(resp == http::status_code::NotImplemented);
                    });
                }

                for (auto &t : th) {
                    t.join();
                }
            }

            break;
        }
        catch(const HTTPServer::Error&) {
            port++;
        }
        catch(const WebGet::Error&) {
            auto webget_error = true;
            REQUIRE_FALSE(webget_error);
        }
    }
}


TEST_CASE("https: HTTPS stress tests (mandatory parallelism)", "[server] [https]") {

    auto mc = MockCore{};
    auto kp = KeyProvider{}.loadKeyStore("data/test0502/key-store", "cert", "123456").loadTrustStore("data/test0502/trust-store", "123456");

    std::size_t port  = 12500;  // try to grab a pot starting this one
    std::size_t thnum = 4;      // the thread pool's size for mandatory parallelism
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp, thnum };

            SECTION("Call server in series") {

                for (std::size_t i = 0; i < 25; i++) {

                    auto cl = WG_Curl{ KeyProvider{} };
                    auto resp = cl.send("GET", "https://127.0.0.1/index.html", port, "");

                    REQUIRE(resp == http::status_code::NotImplemented);
                }

                REQUIRE(mc.getNumOfParallelCalls() <= thnum);
            }

            SECTION("Call server in parallel") {

                std::list<std::thread> th;
                for (std::size_t i = 0; i < 25; i++) {
                    th.emplace_back([&port]() {
                        auto cl = WG_Curl{ KeyProvider{} };
                        auto resp = cl.send("GET", "https://127.0.0.1/index.html", port, "");

                        REQUIRE(resp == http::status_code::NotImplemented);
                    });
                }

                for (auto &t : th) {
                    t.join();
                }

                REQUIRE(mc.getNumOfParallelCalls() <= thnum);
            }

            SECTION("Call server with long timeout in parallel") {

                std::list<std::thread> th;
                for (std::size_t i = 0; i < 25; i++) {
                    th.emplace_back([&port]() {
                        auto cl = WG_Curl{ KeyProvider{} };
                        auto resp = cl.send("GET", "https://127.0.0.1/sleep/250", port, "");

                        REQUIRE(resp == http::status_code::NotImplemented);
                    });
                }

                for (auto &t : th) {
                    t.join();
                }

                REQUIRE(mc.getNumOfParallelCalls() <= thnum);
            }


            break;
        }
        catch(const HTTPServer::Error&) {
            port++;
        }
        catch(const WebGet::Error&) {
            auto webget_error = true;
            REQUIRE_FALSE(webget_error);
        }
    }
}
