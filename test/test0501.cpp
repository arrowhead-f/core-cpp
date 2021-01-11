////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      request_parser
/// Date:      2021-01-02
/// Author(s): ng201
///
/// Description:
/// * Test the HTTP request parser
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <cstring>
#include <string>

#include "http/serv/RequestParser.h"


TEST_CASE("request_parser: Test GET request", "[server] [request_parser]") {

    auto parser = RequestParser{};

    const char *msg = "GET / HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\n\r\n";

    const auto res = parser.parse(msg, msg + std::strlen(msg));

    REQUIRE(res.first == RequestParser::result_t::completed);

    const auto &req = parser.inspect();

    REQUIRE(req.method == "GET");
    REQUIRE(req.uri    == "/");

    REQUIRE(req.http_version_major == 1);
    REQUIRE(req.http_version_major == 1);
}


TEST_CASE("request_parser: Test POST request", "[server] [request_parser]") {

    auto parser = RequestParser{};

    const char *msg = "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: dev.arrowheads.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 49\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\rlicenseID=string&content=string&/paramsXML=string";

    const auto res = parser.parse(msg, msg + std::strlen(msg));

    REQUIRE(res.first == RequestParser::result_t::completed);

    const auto &req = parser.inspect();

    REQUIRE(req.method == "POST");
    REQUIRE(req.uri    == "/cgi-bin/process.cgi");

    REQUIRE(req.http_version_major == 1);
    REQUIRE(req.http_version_major == 1);
    REQUIRE(req.keepAlive == true);

    REQUIRE(req.content == "licenseID=string&content=string&/paramsXML=string");
}


TEST_CASE("request_parser: Test chunked POST request without trailing", "[server] [request_parser]") {

    auto parser = RequestParser{};

    const char *msg = "POST /post HTTP/1.1\r\nAccept: */*\r\nAccept-Encoding: gzip\r\nAccept-Language: uk,en-US;q=0.8,en;q=0.5,ru;q=0.3\r\nCache-Control: no-cache\r\nConnection: Keep-Alive\r\nHost: dev.arrowheads.com\r\nTransfer-Encoding: chunked\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko\r\ncontent-type: application/json\r\n\r\n6\r\nABCDEF\r\n6\r\nZXCVBN\r\n0\r\n\r\n";

    const auto res = parser.parse(msg, msg + std::strlen(msg));

    REQUIRE(res.first == RequestParser::result_t::completed);

    const auto &req = parser.inspect();

    REQUIRE(req.method == "POST");
    REQUIRE(req.uri    == "/post");

    REQUIRE(req.http_version_major == 1);
    REQUIRE(req.http_version_major == 1);
    REQUIRE(req.content == "ABCDEFZXCVBN");
}


TEST_CASE("request_parser: Test chunked POST request with trailing", "[server] [request_parser]") {
}


TEST_CASE("request_parser: Test PATCH request", "[server] [request_parser]") {
}


TEST_CASE("request_parser: Test PUT request", "[server] [request_parser]") {
}


TEST_CASE("request_parser: Test DELETE request", "[server] [request_parser]") {
}


TEST_CASE("request_parser: Test Content-Length is too short", "[server] [request_parser]") {

    using namespace std::literals;

    auto parser = RequestParser{};

    const char *msg = "POST /index.html HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: dev.arrowheads.com\r\nContent-Type: text/html\r\nContent-Length: 5\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\r123456789";

    const auto res = parser.parse(msg, msg + std::strlen(msg));

    REQUIRE(res.first == RequestParser::result_t::completed);

    const auto &req = parser.inspect();

    REQUIRE(req.method == "POST");
    REQUIRE(req.uri    == "/index.html");

    REQUIRE(req.content == "12345");
    REQUIRE(res.second == "6789"s);  // the unparsed remainder
}


TEST_CASE("request_parser: Test Content-Length is too big", "[server] [request_parser]") {

    auto parser = RequestParser{};

    const char *msg = "POST /index.html HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: dev.arrowheads.com\r\nContent-Type: text/html\r\nContent-Length: 19\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\r123456789";

    const auto res = parser.parse(msg, msg + std::strlen(msg));

    REQUIRE(res.first == RequestParser::result_t::incomplete);
}


TEST_CASE("request_parser: Test wrong method", "[server] [request_parser]") {

    auto parser = RequestParser{};

    const char *msg = "DRUM / HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\n\r\n";

    const auto res = parser.parse(msg, msg + std::strlen(msg));

    REQUIRE(res.first == RequestParser::result_t::completed);

    const auto &req = parser.inspect();
    REQUIRE(req.method == "DRUM");
}


TEST_CASE("request_parser: Test wrong delimiter", "[server] [request_parser]") {

    auto parser = RequestParser{};

    //                                vvv - note the double \r\r
    const char *msg = "GET / HTTP/1.1\r\rHost: dev.arrowheads.com\r\nAccept-Language: hu\r\n\r\n";

    const auto res = parser.parse(msg, msg + std::strlen(msg));

    REQUIRE(res.first == RequestParser::result_t::failed);
}


TEST_CASE("request_parser: Test the continuation of the parsing", "[server] [request_parser]") {

    auto parser = RequestParser{};

    const char *msg1 = "POST /index.html HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: dev.arrowheads.com\r\nContent-Type: text/html\r\nContent-Length: 19\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\r123456789";
    const char *msg2 = "0123456789";

    {
        const auto res = parser.parse(msg1, msg1 + std::strlen(msg1));
        REQUIRE(res.first == RequestParser::result_t::incomplete);
    }

    {
        const auto res = parser.parse(msg2, msg2 + std::strlen(msg2));
        REQUIRE(res.first == RequestParser::result_t::completed);

        const auto &req = parser.inspect();
        REQUIRE(req.content == "1234567890123456789");
    }

}


TEST_CASE("request_parser: Test keep-alive for HTTP/1.0", "[server] [request_parser]") {

    // connection set to close
    {
        auto parser = RequestParser{};

        const char *msg = "GET / HTTP/1.0\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: close\r\n\r\n";
        const auto res = parser.parse(msg, msg + std::strlen(msg));

        REQUIRE(res.first == RequestParser::result_t::completed);

        const auto &req = parser.inspect();
        REQUIRE(req.keepAlive == false);
    }

    // connection set to keep-alive
    {
        auto parser = RequestParser{};

        const char *msg = "GET / HTTP/1.0\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: keep-alive\r\n\r\n";
        const auto res = parser.parse(msg, msg + std::strlen(msg));

        REQUIRE(res.first == RequestParser::result_t::completed);

        const auto &req = parser.inspect();
        REQUIRE(req.keepAlive == true);
    }

    // connection defaults to close
    {
        auto parser = RequestParser{};

        const char *msg = "GET / HTTP/1.0\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\n\r\n";
        const auto res = parser.parse(msg, msg + std::strlen(msg));

        REQUIRE(res.first == RequestParser::result_t::completed);

        const auto &req = parser.inspect();
        REQUIRE(req.keepAlive == false);
    }
}


TEST_CASE("request_parser: Test keep-alive for HTTP/1.1", "[server] [request_parser]") {

    // connection set to close
    {
        auto parser = RequestParser{};

        const char *msg = "GET / HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: close\r\n\r\n";
        const auto res = parser.parse(msg, msg + std::strlen(msg));

        REQUIRE(res.first == RequestParser::result_t::completed);

        const auto &req = parser.inspect();
        REQUIRE(req.keepAlive == false);
    }

    // connection set to keep-alive
    {
        auto parser = RequestParser{};

        const char *msg = "GET / HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: keep-alive\r\n\r\n";
        const auto res = parser.parse(msg, msg + std::strlen(msg));

        REQUIRE(res.first == RequestParser::result_t::completed);

        const auto &req = parser.inspect();
        REQUIRE(req.keepAlive == true);
    }

    // connection defaults to keep-alive
    {
        auto parser = RequestParser{};

        const char *msg = "GET / HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\n\r\n";
        const auto res = parser.parse(msg, msg + std::strlen(msg));

        REQUIRE(res.first == RequestParser::result_t::completed);

        const auto &req = parser.inspect();
        REQUIRE(req.keepAlive == true);
    }
}


TEST_CASE("request_parser: Test reset internal state", "[server] [request_parser]") {

    auto parser = RequestParser{};

    {
        const char *msg = "POST /cgi-bin/process1.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: dev.arrowheads.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 49\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\n\r\nlicenseID=123456&content=string&/paramsXML=string";
        const auto res = parser.parse(msg, msg + std::strlen(msg));

        REQUIRE(res.first == RequestParser::result_t::completed);

        const auto &req = parser.inspect();

        REQUIRE(req.method == "POST");
        REQUIRE(req.uri    == "/cgi-bin/process1.cgi");
        REQUIRE(req.content == "licenseID=123456&content=string&/paramsXML=string");
    }

    parser.reset();

    {
        const char *msg = "PATCH /cgi-bin/process2.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: dev.arrowheads.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 49\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\n\r\nlicenseID=654321&content=string&/paramsXML=string";
        const auto res = parser.parse(msg, msg + std::strlen(msg));

        REQUIRE(res.first == RequestParser::result_t::completed);

        const auto &req = parser.inspect();

        REQUIRE(req.method == "PATCH");
        REQUIRE(req.uri    == "/cgi-bin/process2.cgi");
        REQUIRE(req.content == "licenseID=654321&content=string&/paramsXML=string");
    }

}
