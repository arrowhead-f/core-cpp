#ifndef _TEST_MOCKSERVLET_H_
#define _TEST_MOCKSERVLET_H_


#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <regex>
#include <string>
#include <string>
#include <thread>
#include <utility>
#include <iostream>

#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"


namespace MockServlet {

    std::atomic<bool> ready = false;
    std::atomic<int>  port  = 15000;
    std::atomic<int>  stop  = false;

    typedef std::string (*ResponseGenerator)(const std::string&, const std::string&, const std::string&);

    std::atomic<ResponseGenerator> gen = nullptr;

    class ServletGuard {
        private:
            std::thread &th;
        public:
            ServletGuard(std::thread &th) : th{ th } {}
            ~ServletGuard(){ MockServlet::stop = true; th.join(); }
    };
}

namespace {

    int OpenListener(int port) {
        int sd;
        struct sockaddr_in addr;

        sd = socket(PF_INET, SOCK_STREAM, 0);
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
            return -1;
        }

        if (listen(sd, 10) != 0) {
            return -2;
        }
        return sd;
    }

    SSL_CTX* InitServerCTX(void) {
        const SSL_METHOD *method;
        SSL_CTX *ctx;

        OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
        SSL_load_error_strings();   /* load all error messages */

        method = SSLv23_server_method();
        ctx = SSL_CTX_new(method);   /* create new context from method */
        if (!ctx) {
            return nullptr;
        }

        SSL_CTX_set_cipher_list(ctx, "ALL:eNULL");
        return ctx;
    }

    bool LoadCertificates(SSL_CTX* ctx, const char* CertFile, const char* KeyFile, char *keyPhrase) {
        //New lines 
        if (SSL_CTX_load_verify_locations(ctx, CertFile, KeyFile) != 1)
            return false;

        if (SSL_CTX_set_default_verify_paths(ctx) != 1)
            return false;
        //End new lines

        /* set the local certificate from CertFile */
        if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0) {
            return false;
        }

        /* set the private key from KeyFile (may be the same as CertFile) */
        SSL_CTX_set_default_passwd_cb_userdata(ctx, keyPhrase);
        if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0) {
            return false;
        }

        /* verify private key */
        if (!SSL_CTX_check_private_key(ctx)) {
            return false;
        }

        //New lines - Force the client-side have a certificate
        //SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
        //SSL_CTX_set_verify_depth(ctx, 4);
        //End new lines

        return true;
    }


    // Factory responses
    static const char *http_OK                    = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 49\r\n\r\n<html><body><h1>Winnie-the-Pooh</h1></body></html>";
    static const char *json_OK                    = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: 36\r\n\r\n{\"Piglet\": 12,\"Tigger\": [12, 13, 15]}";
    static const char *http_BAD                   = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 49\r\n\r\n<html><body><h1>Winnie-the-Pooh</h1></body></html>";
    static const char *json_BAD                   = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: 36\r\n\r\n{\"Piglet\": 12,\"Tigger\": [12, 13, 15]}";
    static const char *http_NF                    = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 49\r\n\r\n<html><body><h1>Winnie-the-Pooh</h1></body></html>";
    static const char *json_NF                    = "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\nContent-Length: 74\r\n\r\n{\"title\": \"Winnie-the-Pooh\", \"characters\": [\"Tigger\", \"Piglet\", \"Rabbit\"]}";
    static const char *json_WP                    = "HXXP/1.1 200 OK\r\nContent-Type: application/jsonl\r\nContent-Length: 14\r\n\r\n{\"piglet\": 12}";
    static const char *json_WL                    = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: 536\r\n\r\n{\"Piglet\": 12,\"Tigger\": [12, 13, 15]}";


    void Servlet(SSL* ssl) {
        char buf[4096] = {0};
        int sd, bytes;

        if (SSL_accept(ssl) == -1)     /* do SSL-protocol accept */
        {
        }
        else {
            // add loop read...
            bytes = SSL_read(ssl, buf, sizeof(buf));    /* get request */
            if (bytes > 0) {
                buf[bytes] = 0;

                std::cmatch match;
                if (std::regex_search(buf, match, std::regex{ "([A-Z]+) ([^ ]+) HTTP/1.1\r\n" })) {

                    std::string met = match[1].str();
                    std::string url = match[2].str();

                    // if the generator is set, we use it
                    // otherwise the factory responses are
                    // sent beack
                    auto genfun = MockServlet::gen.load();

                    if (genfun) {
                        const auto r = genfun(met, url, std::string{ buf });
                        SSL_write(ssl, r.c_str(), r.length());
                    }
                    else if (url == "/echo") {
                        SSL_write(ssl, http_OK, std::strlen(http_OK));
                    }
                    else if (url == "/bad-echo") {
                        SSL_write(ssl, http_BAD, std::strlen(http_BAD));
                    }
                    else if (url == "/not-found") {
                        SSL_write(ssl, http_NF, std::strlen(http_NF));
                    }
                    else if (url == "/slow-ok") {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        SSL_write(ssl, http_OK, std::strlen(http_OK));
                    }
                    else if (url == "/json/echo") {
                        SSL_write(ssl, json_OK, std::strlen(json_OK));
                    }
                    else if (url == "/json/bad-echo") {
                        SSL_write(ssl, json_BAD, std::strlen(json_BAD));
                    }
                    else if (url == "/json/not-found") {
                        SSL_write(ssl, json_NF, std::strlen(json_NF));
                    }
                    else if (url == "/json/wrong-protocol") {
                        SSL_write(ssl, json_WP, std::strlen(json_WP));
                    }
                    else if (url == "/json/wrong-content-length") {
                        SSL_write(ssl, json_WL, std::strlen(json_WL));
                    }
                    else {
                    }
                }
            }
            else {
            }
        }

        sd = SSL_get_fd(ssl);  /* get socket connection */
        SSL_free(ssl);         /* release SSL state     */
        close(sd);             /* close connection      */
    }

}


namespace MockServlet {

    void run(const char *certFile, const char *keyFile, char *keyPhrase) {

        SSL_CTX *ctx;

        int portnum = MockServlet::port;

        SSL_library_init();
        ctx = InitServerCTX();                     /* initialize SSL       */

        if(!ctx)
            return;

        /* load certs */
        if(!LoadCertificates(ctx, certFile, keyFile, keyPhrase))
            return;

        int server = -1;
        do {
            server = OpenListener(portnum++);  /* create server socket */
        } while(server < 0);

        // set global data
        MockServlet::port  = portnum - 1;
        MockServlet::ready = true;

        do {

            /* Watch server to see when it has input. */
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(server, &rfds);

            /* Wait up to 0.5 seconds. */
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 500;

            int retval = select(server + 1, &rfds, NULL, NULL, &tv);
            /* Don't rely on the value of tv now! */

            if (retval == -1) {
                break;
            }
            else if (retval) {
                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                SSL *ssl;

                int client = accept(server, (struct sockaddr*)&addr, &len);  /* accept connection as usual */

                ssl = SSL_new(ctx);       /* get new SSL state with context     */
                SSL_set_fd(ssl, client);  /* set connection socket to SSL state */
                Servlet(ssl);             /* service connection                 */
            }
        } while(!MockServlet::stop);

        close(server);      /* close server socket */
        SSL_CTX_free(ctx);  /* release context     */

        MockServlet::stop  = false;
        MockServlet::ready = false;
        MockServlet::gen   = nullptr;
    }

}

#endif  /* _TEST_MOCKSERVLET_H_ */
