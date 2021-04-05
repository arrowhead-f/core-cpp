////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      https
/// Date:      2021-01-08
/// Author(s): ng201
///
/// Description:
/// * Test the misuse of the HTTPS server.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <chrono>
#include <cstring>
#include <list>
#include <string>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "http/KeyProvider.h"
#include "http/serv/HTTPSServer.h"

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


/// A https client implementation.
namespace {

    /// Opens a connection.
    /// \param hostname             the name of the host
    /// \param port                 port to use
    int openConnection(const char *hostname, unsigned port) {
        struct hostent *host;
        struct sockaddr_in addr;
        if ((host = gethostbyname(hostname)) == nullptr)
            return -1;

        int sd = socket(PF_INET, SOCK_STREAM, 0);
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = *(long*)(host->h_addr);

        if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
            return -1;

        return sd;
    }

    /// Create the context.
    SSL_CTX* initCTX() {

        SSL_CTX *ctx = nullptr;
        OpenSSL_add_all_algorithms(); /* Load cryptos, et.al.                 */
        SSL_load_error_strings();     /* Bring in and register error messages */

        const SSL_METHOD *method = TLS_client_method();
        ctx = SSL_CTX_new(method);    /* Create new context                   */

        return ctx;
    }

    std::pair<std::string, std::string> getCertInfo(SSL *ssl) {
        X509 *cert;
        char *line;
        cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
        if (cert) {
            std::pair<std::string, std::string> result;

            // subject
            line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
            result.first = line;
            free(line);

            // issuer
            line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
            result.second = line;
            free(line);

            X509_free(cert);

            return result;
        }
        else {
            return { "", "" };
        }
    }

    /// Makes a HTTPS call.
    /// \param port             the port to connect
    /// \param msg              the message builder function
    /// \param ca               the server's certificate
    /// \return                 the response
    std::string client(const char *hostname, unsigned port, const std::function<std::pair<bool, std::string>(int)> &msg, std::pair<std::string, std::string>& ca, bool persistent = false) {

        SSL_library_init();

        SSL_CTX *ctx = initCTX();
        int server = openConnection(hostname, port);

        SSL *ssl = SSL_new(ctx);  /* create new SSL connection state */
        SSL_set_fd(ssl, server);  /* attach the socket descriptor    */

        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

        // now connect
        if (SSL_connect(ssl) != 1) {
            throw std::runtime_error{ "SSL connect failed"  };
        }

        // SSL_get_cipher(ssl) // encryption
        ca = getCertInfo(ssl);

        std::string res;  // we will store the reply here
        char buf[1024];   // the buffer for the reply
        int bytes;        // how many bytes were read

        int cnt = 0;
        std::pair<bool, std::string> q;
        do {
            q = msg(cnt++);
            SSL_write(ssl, q.second.c_str(), q.second.length());   /* encrypt & send message */

            if (persistent) {
                while ((bytes = SSL_read(ssl, buf, sizeof(buf))) > 0) {
                    buf[bytes] = 0;
                    res += buf;
                    break;
                }
            }

        } while (q.first);

        // read the reply for the last packet
        while ((bytes = SSL_read(ssl, buf, sizeof(buf))) > 0) {
            buf[bytes] = 0;
            res += buf;
        }

        SSL_free(ssl);      /* release connection state */
        close(server);      /* close socket             */
        SSL_CTX_free(ctx);  /* release context          */

        return res;
    }

}



TEST_CASE("https: Test server's timeout set and ca used", "[server] [https]") {

    using namespace std::literals;

    auto mc = MockCore{};
    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");

    std::size_t port = 12500;
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            {
                std::pair<std::string, std::string> caInfo;
                const std::string res = client("127.0.0.1", port, [](int i){ return std::make_pair(false, "GET"s); }, caInfo);

                REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
                REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");

                // timeout      // only the method part of the HTTP request was sent in time
                REQUIRE(res == "");
            }

            // enabling the next few block would increase the running time of the test
            // thus by default are disabled
            #if 0
              {
                  std::pair<std::string, std::string> caInfo;
                  const std::string res = client("127.0.0.1", port, [](int i){ return std::make_pair(false, "PUT"s); }, caInfo);

                  REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
                  REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");

                  // timeout      // only the method part of the HTTP request was sent in time
                  REQUIRE(res == "");
              }

              {
                  std::pair<std::string, std::string> caInfo;
                  const std::string res = client("127.0.0.1", port, [](int i){ return std::make_pair(false, "PATCH"s); }, caInfo);

                  REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
                  REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");

                  // timeout      // only the method part of the HTTP request was sent in time
                  REQUIRE(res == "");
              }

              {
                  std::pair<std::string, std::string> caInfo;
                  const std::string res = client("127.0.0.1", port, [](int i){ return std::make_pair(false, "POST"s); }, caInfo);

                  REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
                  REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");

                  // timeout      // only the method part of the HTTP request was sent in time
                  REQUIRE(res == "");
              }

              {
                  std::pair<std::string, std::string> caInfo;
                  const std::string res = client("127.0.0.1", port, [](int i){ return std::make_pair(false, "DELETE"s); }, caInfo);

                  REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
                  REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");

                  // timeout      // only the method part of the HTTP request was sent in time
                  REQUIRE(res == "");
              }
            #endif

            {
                std::pair<std::string, std::string> caInfo;
                const std::string res = client("127.0.0.1", port, [](int i){ return std::make_pair(false, "GEX"s); }, caInfo);

                REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
                REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");

                // timeout      // only the first characte rof the request mthod is checked during the parsing
                REQUIRE(res == "");
            }

            {
                std::pair<std::string, std::string> caInfo;
                const std::string res = client("127.0.0.1", port, [](int i){ return std::make_pair(false, "XXX"s); }, caInfo);

                REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
                REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");

                // BadRequest   //the first character of the request method is not valid
                REQUIRE(res == "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 87\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>400 Bad Request</h1></body></html>");
            }

            break;
        }
        catch(const HTTPServer::Error) {
            port++;
        }
        catch(...) {
            auto is_error = true;
            REQUIRE_FALSE(is_error);
        }
    }
}


TEST_CASE("https: Test client sends a very long message one-by-one", "[server] [https]") {

    auto mc = MockCore{};
    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");

    std::size_t port = 12500;

    // generate the big request
    std::string header = "POST /who-cares.html HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.arrowhead.com\r\nContent-Type: text/plain\r\nConnection: close\r\nContent-Length: 2048\r\nAccept-Language: en-us\r\n\r\n";
    std::string req = header;
    for (std::size_t i = 0; i < 2048; i++)
        req += 'a';

    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            // One big packet
            {
                std::pair<std::string, std::string> caInfo;
                const std::string res = client("127.0.0.1", port, [&req](int i) { return std::make_pair(false, req); }, caInfo);

                REQUIRE(res == "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>");
            }

            // Multiple small packets
            {
                std::pair<std::string, std::string> caInfo;
                const std::string res = client("127.0.0.1", port, [&req](int i) {
                    std::string r = req.substr(i * 24, 24);
                    return std::make_pair((i +1) * 24 < req.length(), r);
                }, caInfo);

                REQUIRE(res == "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>");
            }

            // Wait between the parts of the packet
            {
                std::pair<std::string, std::string> caInfo;
                const std::string res = client("127.0.0.1", port, [&req](int i) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(125));
                    std::string r = req.substr(i * 24, 24);
                    return std::make_pair((i +1) * 24 < req.length(), r);
                }, caInfo);

                REQUIRE(res == "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>");
            }

            break;
        }
        catch(const HTTPServer::Error) {
            port++;
        }
        catch(...) {
            auto is_error = true;
            REQUIRE_FALSE(is_error);
        }
    }
}


TEST_CASE("https: Test clients send a very long messages parallelly", "[server] [https]") {

    auto mc = MockCore{};
    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");

    std::size_t port  = 12500;
    std::size_t thnum = 4;          // the size of the server's threadpool
    std::size_t clnum = 2 * thnum;  // number of parallelly running clients

    // generate the big request
    std::string header = "POST /who-cares.html HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.arrowhead.com\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 2048\r\nAccept-Language: en-us\r\n\r\n";
    std::string req = header;
    for (std::size_t i = 0; i < 2048; i++)
        req += 'b';

    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp, thnum };

            // One big packet
            {
                std::vector<std::thread> ths;
                for (std::size_t i = 0; i < clnum; i++) {
                    ths.push_back(std::thread{ [&req, port] {
                        std::pair<std::string, std::string> caInfo;
                        const std::string res = client("127.0.0.1", port, [&req](int i) { return std::make_pair(false, req); }, caInfo);

                        REQUIRE(res == "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>");
                    } });
                }

                for (auto &th : ths)
                    th.join();
            }

            // Multiple small packets
            {
                std::vector<std::thread> ths;
                for (std::size_t i = 0; i < clnum; i++) {
                    ths.push_back(std::thread{ [&req, port] {
                        std::pair<std::string, std::string> caInfo;
                        const std::string res = client("127.0.0.1", port, [&req](int i) {
                            std::string r = req.substr(i * 24, 24);
                            return std::make_pair((i +1) * 24 < req.length(), r);
                        }, caInfo);
                        REQUIRE(res == "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>");
                    } });
                }

                for (auto &th : ths)
                    th.join();
            }

            // Wait between the parts of the packet
            {
                std::vector<std::thread> ths;
                for (std::size_t i = 0; i < clnum; i++) {
                    ths.push_back(std::thread{ [&req, port] {
                        std::pair<std::string, std::string> caInfo;
                        const std::string res = client("127.0.0.1", port, [&req](int i) {
                            std::this_thread::sleep_for(std::chrono::milliseconds(125));
                            std::string r = req.substr(i * 24, 24);
                            return std::make_pair((i +1) * 24 < req.length(), r);
                        }, caInfo);

                        REQUIRE(res == "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>");
                    } });
                }

                for (auto &th : ths)
                    th.join();
            }

            break;
        }
        catch(const HTTPServer::Error) {
            port++;
        }
        catch(...) {
            auto is_error = true;
            REQUIRE_FALSE(is_error);
        }
    }
}


TEST_CASE("https: Test keep-alive with malformed packets", "[server] [https]") {

    using namespace std::literals;

    auto mc = MockCore{};
    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");

    std::size_t port = 12500;
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            {
                std::pair<std::string, std::string> caInfo;

                // note the wrong method!
                const std::string res = client("127.0.0.1", port, [](int i){ return std::make_pair(false, "XGET / HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: keep-alive\r\n\r\n"s); }, caInfo);

                REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
                REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");
                REQUIRE(res == "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 87\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>400 Bad Request</h1></body></html>");
            }

            break;
        }
        catch(const HTTPServer::Error) {
            port++;
        }
        catch(...) {
            auto is_error = true;
            REQUIRE_FALSE(is_error);
        }
    }

}


TEST_CASE("https: Test HTTP persistent (keep-alive) connection", "[server] [https]") {

    using namespace std::literals;

    auto mc = MockCore{};
    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");

    std::size_t port = 12500;
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            std::pair<std::string, std::string> caInfo;
            const std::string res = client("127.0.0.1", port, [](int i){
                switch(i) {
                    case 0:
                        return std::make_pair(true,  "GET /0.html HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: keep-alive\r\n\r\n"s);
                    case 1:
                        return std::make_pair(true,  "GET /1.html HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: keep-alive\r\n\r\n"s);
                    default:
                        return std::make_pair(false, "GET /2.html HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: close\r\n\r\n"s);
                }
            }, caInfo, true);

            REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
            REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");

            REQUIRE(res ==
                           "HTTP/1.1 501 Not Implemented\r\nConnection: keep-alive\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>"
                           "HTTP/1.1 501 Not Implemented\r\nConnection: keep-alive\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>"
                           "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>");
            break;
        }
        catch(const HTTPServer::Error) {
            port++;
        }
        catch(...) {
            auto is_error = true;
            REQUIRE_FALSE(is_error);
        }
    }
}


TEST_CASE("https: Test HTTP pipelining", "[server] [https]") {

    using namespace std::literals;

    auto mc = MockCore{};
    auto kp = KeyProvider{}.loadKeyStore("data/test0093/key-store", "cert", "123456").loadTrustStore("data/test0093/trust-store", "123456");

    std::size_t port = 12500;
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            std::pair<std::string, std::string> caInfo;
            const std::string res = client("127.0.0.1", port, [](int i){
                switch(i) {
                    case 0:
                        return std::make_pair(true,  "GET /0.html HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: keep-alive\r\n\r\n"s);
                    case 1:
                        return std::make_pair(true,  "GET /1.html HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: keep-alive\r\n\r\n"s);
                    default:
                        return std::make_pair(false, "GET /2.html HTTP/1.1\r\nHost: dev.arrowheads.com\r\nAccept-Language: hu\r\nConnection: close\r\n\r\n"s);
                }
            }, caInfo);

            REQUIRE(caInfo.first  == "/CN=authorization.testcloud2.aitia.arrowhead.eu");
            REQUIRE(caInfo.second == "/CN=testcloud2.aitia.arrowhead.eu");

            REQUIRE(res ==
                           "HTTP/1.1 501 Not Implemented\r\nConnection: keep-alive\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>"
                           "HTTP/1.1 501 Not Implemented\r\nConnection: keep-alive\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>"
                           "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\nContent-Length: 91\r\nContent-Type: text/html\r\n\r\n<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>");
            break;
        }
        catch(const HTTPServer::Error) {
            port++;
        }
        catch(...) {
            auto is_error = true;
            REQUIRE_FALSE(is_error);
        }
    }
}
