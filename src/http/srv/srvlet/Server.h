#ifndef _HTTP_SERVER_SRVLET_H_
#define _HTTP_SERVER_SRVLET_H_

#include <future>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <list>

#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/poll.h>

#include <sys/signalfd.h>

#include "openssl/ssl.h"
#include "openssl/err.h"


#include <iostream>

#include "http/HTTPServer.h"
#include "http/KeyProvider.h"
#include "../RequestHandler.h"
#include "../RequestParser.h"
#include "../Reply.h"


namespace http {
namespace server {

    namespace srvlet {

        template<typename T>
        class HTTPSServer : public http::HTTPServer<T>, public http::server::RequestHandler {

            private:

                int server;    ///< The server's file descriptor.
                SSL_CTX *ctx;  ///< The SSL context.

            public:

                HTTPSServer(const HTTPSServer&) = delete;
                HTTPSServer& operator=(const HTTPSServer&) = delete;
                HTTPSServer(HTTPSServer&&) = delete;
                HTTPSServer& operator=(HTTPSServer&&) = delete;

                /// Construct the server to listen on the specified TCP address and port, and
                /// serve up files from the given directory.
                explicit HTTPSServer(const std::string &address, std::size_t port, T &dispatcher, http::KeyProvider &keyProvider)
                    : http::HTTPServer<T>{ address, port, dispatcher } {

                    SSL_library_init();

                    ctx = initCTX();          /* initialize SSL */
                    if (!ctx)
                        throw ServerException{ http::errc::SSLCTX, "Context exception." };


                    loadCertificates(ctx, keyProvider.sslCert.c_str(), keyProvider.sslKey.c_str(), const_cast<char*>(keyProvider.keyPasswd.data())); /* load certs */

                    server = openPort(port);    /* create server socket */
                    if (server == -1)
                        throw ServerException{ http::errc::TCPPORT, "Cannot open port." };

                }

                ~HTTPSServer() {
                    close(server);      /* close server socket */
                    SSL_CTX_free(ctx);  /* release context     */
                }

                void run() {

                    std::list<std::future<void>> servlets;

                    struct pollfd fds[2];

                    sigset_t mask;
                    struct signalfd_siginfo fdsi;

                    sigemptyset(&mask);
                    sigaddset(&mask, SIGINT);
                    sigaddset(&mask, SIGQUIT);

                    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
                        ; //handle_error("sigprocmask");

                    fds[1].fd = signalfd(-1, &mask, 0);
                    if (fds[1].fd == -1)
                        ; //       handle_error("signalfd");

                    fds[0].fd = server;
                    fds[0].events = POLLIN;
                    fds[1].events = POLLIN;

                    while (1) {

                        int rc = poll(fds, 2, -1);

                        if (rc < 0) {
                            std::cerr << "poll failed\n";
                            break;
                        }

                        if(fds[1].revents == POLLIN) {
                            const auto s = read(fds[1].fd, &fdsi, sizeof(fdsi));
                            if (s != sizeof(fdsi))
                                ; // handle_error("read");

                            if (fdsi.ssi_signo == SIGINT || fdsi.ssi_signo == SIGQUIT) {
                                ; //std::cout << "Got SIGQUIT\n";
                            }
                            else {
                                std::cout << "Read unexpected signal\n";
                            }
                            break; //        continue;
                        }

                        if(fds[0].revents != POLLIN) {
                            continue;
                        }

                        struct sockaddr_in addr;
                        socklen_t len = sizeof(addr);
                        SSL *ssl;

                        // select here on pipe and ssl
                        /////char buf;         // buffer for the data read through the pipe
                        /////ssize_t ret = 0;  // the return value of teh reading from the pipe

                        // signals may interrupt this system call
                        /////while ((ret = read(getThePipe()[0], &buf, 1)) == -1 && errno == EINTR)
                        /////continue;

                        int client = accept(server, (struct sockaddr*)&addr, &len);  /* accept connection as usual */
                        ssl = SSL_new(ctx);         /* get new SSL state with context     */
                        SSL_set_fd(ssl, client);    /* set connection socket to SSL state */


                        // create the new servlet: servlet(ssl);
                        servlets.push_back(std::async(std::launch::async, [this](auto *ssl){ this->servlet(ssl); }, ssl));

                        // remove finished servlets
                        for (auto it = servlets.begin(); it != servlets.end();) {
                            if(it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                                it = servlets.erase(it);
                            else
                                ++it;
                        }
                    }

                    // wait for all open connections here
                    for(auto &srv : servlets) {
                        srv.wait();
                    }

                }

            private:

                SSL_CTX* initCTX() {
                    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
                    SSL_load_error_strings();      /* load all error messages */

                    const SSL_METHOD *method = SSLv23_server_method(); //SSLv3_server_method();
                    SSL_CTX *ctx = SSL_CTX_new(method);   /* create new context from method */
                    if (!ctx) {
                        return nullptr;
                    }

                    SSL_CTX_set_cipher_list(ctx, "ALL:eNULL");
                    return ctx;
                }



                int openPort(int port) {
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
                        return -1;
                    }
                    return sd;
                }


                void loadCertificates(SSL_CTX *ctx, const char *certFile, const char *keyFile, char *keyPhrase) {
                    //New lines 
                    if (SSL_CTX_load_verify_locations(ctx, certFile, keyFile) != 1)
                        ERR_print_errors_fp(stderr);

                    if (SSL_CTX_set_default_verify_paths(ctx) != 1)
                        ERR_print_errors_fp(stderr);
                    //End new lines

                    /* set the local certificate from CertFile */
                    if (SSL_CTX_use_certificate_file(ctx, certFile, SSL_FILETYPE_PEM) <= 0) {
                        ERR_print_errors_fp(stderr);
                        abort();
                    }

                    /* set the private key from KeyFile (may be the same as CertFile) */
                    SSL_CTX_set_default_passwd_cb_userdata(ctx, keyPhrase);
                    if (SSL_CTX_use_PrivateKey_file(ctx, keyFile, SSL_FILETYPE_PEM) <= 0) {
                        ERR_print_errors_fp(stderr);
                        abort();
                    }

                    /* verify private key */
                    if (!SSL_CTX_check_private_key(ctx)) {
                        fprintf(stderr, "Private key does not match the public certificate\n");
                        abort();
                    }

                    //New lines - Force the client-side have a certificate
                    //SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
                    //SSL_CTX_set_verify_depth(ctx, 4);
                    //End new lines
                }


                /// Serve the connection.
                void servlet(SSL *ssl) {

                    char enter[3] = { 0x0d, 0x0a, 0x00 };
                    char buffer[1024];
                    http::server::RequestParser parser;
                    http::server::Request request;

                    if (SSL_accept(ssl) == -1)     /* do SSL-protocol accept */
                        ERR_print_errors_fp(stderr);

                    int len = 0;
                    while ((len = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {

                        http::server::RequestParser::result_type result;
                        std::tie(result, std::ignore) = parser.parse(request, buffer, buffer + len);

                        if (result == http::server::RequestParser::good) {

                            http::server::Reply reply;

                            struct sockaddr_in addr;
                            socklen_t len;
                            getpeername(SSL_get_fd(ssl), (struct sockaddr*)&addr, &len);

                            request.address = inet_ntoa(addr.sin_addr);
                            handle_request(request, reply);

                            const auto reply_str = reply.to_string();
                            SSL_write(ssl, reply_str.c_str(), reply_str.length()); /* send reply */
                        }
                        else if (result == http::server::RequestParser::bad) {

                            http::server::Reply reply = http::server::Reply::stock_reply(http::server::Reply::bad_request);
                            const auto reply_str = reply.to_string();
                            SSL_write(ssl, reply_str.c_str(), reply_str.length()); /* send reply */
                        }
                        else if (result == http::server::RequestParser::requestlength) {

                            const auto reply = http::server::Reply::stock_reply(http::server::Reply::length_required);
                            const auto reply_str = reply.to_string();
                            SSL_write(ssl, reply_str.c_str(), reply_str.length()); /* send reply */
                        }

                    }
                    // error!!!

                    int sd = SSL_get_fd(ssl);  /* get socket connection */
                    SSL_free(ssl);             /* release SSL state     */
                    close(sd);                 /* close connection      */
                }

                int handle(const std::string &from, const char *method, const std::string &uri, std::string &response, const std::string &payload) final {
                    return http::HTTPServer<T>::handle(from, method, uri, response, payload);
                }

                void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) final {
                    http::HTTPServer<T>::denied(from, method, uri, reason);
                }


        };  // class HTTPSServer


    }  // namespace srvlet

} // namespace server
} // namespace http

#endif  /* _HTTP_SERVER_SRVLET_H_ */
