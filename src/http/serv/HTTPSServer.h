#ifndef _HTTP_HTTPSSERVER_H_
#define _HTTP_HTTPSSERVER_H_

#include <future>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <list>

#ifdef TESTRUNNER
  #include <atomic>
#endif

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


#include "http/HTTPServer.h"
#include "http/KeyProvider.h"
#include "RequestParser.h"
#include "RequestBuffer.h"


/// OpenSSL based server.
template<typename T>
class HTTPSServer final : public ::HTTPServerBase<T> {

    private:

        using Parent = ::HTTPServerBase<T>;

        int thrd;      ///< Number of threads.

        int server;    ///< The server's file descriptor.
        SSL_CTX *ctx;  ///< The SSL context.

        #ifdef TESTRUNNER
            std::atomic_bool runflag = true;
        #endif

    public:

        HTTPSServer(const HTTPSServer&) = delete;
        HTTPSServer& operator=(const HTTPSServer&) = delete;
        HTTPSServer(HTTPSServer&&) = delete;
        HTTPSServer& operator=(HTTPSServer&&) = delete;

        /// Construct the server to listen on the specified TCP address and port.
        explicit HTTPSServer(const std::string &address, std::size_t port, T &dispatcher, KeyProvider &keyProvider, int thrd = 0)
            : Parent{ address, port, dispatcher } {

            SSL_library_init();

            ctx = initCTX();          /* initialize SSL */
            if (!ctx)
                throw HTTPServer::Error{ "Cannot initialize SSL context." };


            // this throws exception in case of error
            loadCertificates(ctx, keyProvider.sslCert.c_str(), keyProvider.sslKey.c_str(), const_cast<char*>(keyProvider.keyPasswd.data())); /* load certs */

            server = openPort(port);    /* create server socket */
            if (server == -1)
                throw HTTPServer::Error{ "Cannot open listening port." };
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
                ; //handle_error("sigprocmask"); // throw

            fds[1].fd = signalfd(-1, &mask, 0);
            if (fds[1].fd == -1)
                ; //       handle_error("signalfd"); // throw

            fds[0].fd = server;
            fds[0].events = POLLIN;
            fds[1].events = POLLIN;

            #ifdef TESTRUNNER
              while (runflag) {
            #else
              while (1) {
            #endif

                #ifdef TESTRUNNER
                  int rc = poll(fds, 2, 250);
                #else
                  int rc = poll(fds, 2, -1);
                #endif

                if (rc < 0) {
                    //std::cerr << "poll failed\n";
                    break;
                }
                if(fds[1].revents == POLLIN) {
                    const auto s = read(fds[1].fd, &fdsi, sizeof(fdsi));
                    if (s != sizeof(fdsi))
                        ; // handle_error("read");
                    if (fdsi.ssi_signo == SIGINT || fdsi.ssi_signo == SIGQUIT) {
                        //std::cerr << "got sigquit\n";
                        ; //std::cout << "Got SIGQUIT\n";
                    }
                    else {
                        //std::cout << "Read unexpected signal\n";
                    }
                    break; //        continue;
                }
                if(fds[0].revents != POLLIN) {
                    continue;
                }

                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                SSL *ssl;

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

        #ifdef TESTRUNNER
          void kill() {
              runflag = false;
          }
        #endif

    private:

        /// Initializes the SSL context.
        /// \return                 the context; in case of error nullptr is returned
        SSL_CTX* initCTX() {
            OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
            SSL_load_error_strings();      /* load all error messages */

            const SSL_METHOD *method = SSLv23_server_method(); //SSLv3_server_method();
            SSL_CTX *ctx = SSL_CTX_new(method);   /* create new context from method */
            if (!ctx)
                return nullptr;

            SSL_CTX_set_cipher_list(ctx, "ALL:eNULL");
            return ctx;
        }

        /// Opens the requested listening port.
        /// \param port             port to open
        /// \return                 the created listening socket; in case of error -1 is returned
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


        /// Loads the server cetificates.
        /// \param ctx              the context used
        /// \param certFile         the cert file
        /// \param keyFile          the key file
        /// \param keyPhrase        the keyphrase password
        void loadCertificates(SSL_CTX *ctx, const char *certFile, const char *keyFile, char *keyPhrase) {

            //New lines 
            if (SSL_CTX_load_verify_locations(ctx, certFile, keyFile) != 1)
                throw HTTPServer::Error{ "Certificate error." };

            if (SSL_CTX_set_default_verify_paths(ctx) != 1)
                throw HTTPServer::Error{ "Certificate error." };  // ERR_print_errors_fp(stderr);
            //End new lines

            /* set the local certificate from CertFile */
            if (SSL_CTX_use_certificate_file(ctx, certFile, SSL_FILETYPE_PEM) <= 0) {
                throw HTTPServer::Error{ "Certificate error." };
            }

            /* set the private key from KeyFile (may be the same as CertFile) */
            SSL_CTX_set_default_passwd_cb_userdata(ctx, keyPhrase);
            if (SSL_CTX_use_PrivateKey_file(ctx, keyFile, SSL_FILETYPE_PEM) <= 0) {
                throw HTTPServer::Error{ "Certificate error." };
            }

            /* verify private key */
            if (!SSL_CTX_check_private_key(ctx)) {
                throw HTTPServer::Error{ "Private key does not match the public certificate." };
            }

            //New lines - Force the client-side have a certificate
            //SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
            //SSL_CTX_set_verify_depth(ctx, 4);
            //End new lines
        }

        /// Serve the connection.
        void servlet(SSL *ssl) {

            char buffer[1024];     // we use this buffer for the incomming data
            RequestParser parser;  // the parser

            if (SSL_accept(ssl) == -1)     /* do SSL-protocol accept */
                return;

            int len = 0;
            while ((len = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {

                RequestParser::result_t result;     // the result of the parsing step
                const char *start_parse = buffer;   // start parsing the buffer here

                keep_alive:

                // parse the buffer
                std::tie(result, start_parse) = parser.parse(start_parse, start_parse + len);

                if (result == RequestParser::result_t::completed) {

                    // get the peer's address
                    struct sockaddr_in addr;
                    socklen_t slen;
                    getpeername(SSL_get_fd(ssl), (struct sockaddr*)&addr, &slen);

                    // get the parsed request and set the remote address
                    auto req = parser.request();
                    req.remote_address = inet_ntoa(addr.sin_addr);

                    // generate the response
                    const auto resp = Parent::handle(std::move(req));
                    const auto reply_str = resp.to_string();

                    // send the reply
                    SSL_write(ssl, reply_str.c_str(), reply_str.length());

                    // there's still some data in the buffer
                    if (start_parse != buffer + len) {
                        if (parser.inspect().keepAlive) {
                            parser.reset();
                            goto keep_alive;
                        }

                        break;
                    }

                }
                else if (result == RequestParser::result_t::failed) {

                    auto reply = Response::from_stock(http::status_code::BadRequest);
                    const auto reply_str = reply.to_string();
                    SSL_write(ssl, reply_str.c_str(), reply_str.length());

                    break;
                }

            }

            if (len < 0) {
                // handle error
            }

            int sd = SSL_get_fd(ssl);  /* get socket connection */
            SSL_free(ssl);             /* release SSL state     */
            close(sd);                 /* close connection      */
        }


};


#endif  /* _HTTP_HTTPSSERVER_H_ */
