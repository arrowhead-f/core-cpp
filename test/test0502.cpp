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


#include <cstring>
#include <string>
#include <thread>

#include "http/KeyProvider.h"
#include "http/serv/HTTPSServer.h"
#include "http/wget/WG_Curl.h"

#include "hlpr/MockCore.h"


class HTTPServerGuard {
    private:

        HTTPSServer<MockCore> &http;  ///< The guarded server.
        std::thread           &th;    ///< The guarded thread.

    public:

        HTTPServerGuard(HTTPSServer<MockCore> &http, std::thread &th) : http{ http }, th{ th } {}
        ~HTTPServerGuard() {
            http.kill();
            th.join();
        }
};


TEST_CASE("https: Test https server's reachability", "[server] [https]") {

    auto mc = MockCore{};

    auto kp = KeyProvider{ "data/test0502/tempsensor.testcloud1.publicCert.pem", "PEM", "data/test0502/tempsensor.testcloud1.private.key", "PEM", "12345", "data/test0502/tempsensor.testcloud1.caCert.pem" };

    int port = 12500;
    while(1) {

        try {
            auto http = HTTPSServer<MockCore>("127.0.0.1", port, mc, kp /*, thrd*/);
            std::thread th{ [&http](){ http.run(); } };

            auto _ = HTTPServerGuard{ http, th };

            {
                auto cl = WG_Curl{ KeyProvider{} };
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

