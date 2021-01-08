////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      https
/// Date:      2021-01-08
/// Author(s): ng201
///
/// Description:
/// * Test the HTTPS request parser
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <cstring>
#include <list>
#include <string>
#include <stdexcept>
#include <thread>
#include <utility>

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
    std::string client(const char *hostname, unsigned port, const std::function<std::pair<bool, std::string>(int)> &msg, std::pair<std::string, std::string>& ca) {

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

        int cnt = 0;
        std::pair<bool, std::string> q;
        do {
            q = msg(cnt++);
            SSL_write(ssl, q.second.c_str(), q.second.length());   /* encrypt & send message */
        } while (q.first);


        /* get reply & decrypt */
        std::string res;

        char buf[1024];
        int bytes;
        while ((bytes = SSL_read(ssl, buf, sizeof(buf)))) {
            buf[bytes] = 0;
            res += buf;
            break;
        }

        SSL_free(ssl);      /* release connection state */
        close(server);      /* close socket             */
        SSL_CTX_free(ctx);  /* release context          */

        return res;
    }

}



TEST_CASE("https: Test ca used by the server", "[server] [https]") {

    using namespace std::literals;

    auto mc = MockCore{};

    auto kp = KeyProvider{ "data/test0503/tempsensor.testcloud1.publicCert.pem", "PEM", "data/test0503/tempsensor.testcloud1.private.key", "PEM", "12345", "data/test0503/tempsensor.testcloud1.caCert.pem" };

    std::size_t port = 12500;
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            {
                std::pair<std::string, std::string> caInfo;
                std::string res = client("127.0.0.1", port, [](int i){ return std::make_pair(false, "XXX"s); }, caInfo);

                REQUIRE(caInfo.first  == "/C=HU/CN=SecureTemperatureSensor.testcloud1.aitia.arrowhead.eu");
                REQUIRE(caInfo.second == "/C=HU/ST=Hungary/L=Budapest/O=AITIA/CN=TestCloud1.aitia.arrowhead.eu");
                REQUIRE(res == "");  // timeouted
            }

            break;
        }
        catch(const HTTPServer::Error) {
            port++;
        }
        catch(const std::exception &e) {
            auto webget_error = true;
            REQUIRE_FALSE(webget_error);
        }
    }
}

/*
TEST_CASE("https: Test early hung up by the client", "[server] [https]") {

    auto mc = MockCore{};

    auto kp = KeyProvider{ "data/test0503/tempsensor.testcloud1.publicCert.pem", "PEM", "data/test0503/tempsensor.testcloud1.private.key", "PEM", "12345", "data/test0503/tempsensor.testcloud1.caCert.pem" };

    std::size_t port = 12500;
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            {

                auto cl = WG_Curl{ KeyProvider{} };
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


TEST_CASE("https: Test client sends a very long message", "[server] [https]") {

    auto mc = MockCore{};

    auto kp = KeyProvider{ "data/test0503/tempsensor.testcloud1.publicCert.pem", "PEM", "data/test0503/tempsensor.testcloud1.private.key", "PEM", "12345", "data/test0503/tempsensor.testcloud1.caCert.pem" };

    std::size_t port = 12500;
    while(1) {

        try {
            auto ms = HTTPSServerGuard<MockCore>{ "127.0.0.1", port, mc, kp };

            SECTION("One big packet") {

                auto cl = WG_Curl{ KeyProvider{} };
                auto resp = cl.send("GET", "https://127.0.0.1/index.html", port, "");

                REQUIRE(resp == http::status_code::NotImplemented);
            }

            SECTION("Wait between the packets") {

                auto cl = WG_Curl{ KeyProvider{} };
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
*/