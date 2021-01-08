#ifndef _HTTP_HTTPSSERVER_H_
#define _HTTP_HTTPSSERVER_H_


#include <iostream>

#include <algorithm>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/poll.h>
#include <sys/signalfd.h>
#include <signal.h>

#include "openssl/ssl.h"
#include "openssl/err.h"

#include "http/HTTPServer.h"
#include "http/KeyProvider.h"
#include "RequestParser.h"
#include "RequestBuffer.h"


/// OpenSSL based HTTPS server.
template<typename T>
class HTTPSServer final : public ::HTTPServerBase<T> {

    private:

        /// To be able to reference the parent easier.
        using Parent = ::HTTPServerBase<T>;

        int server;        ///< The server's file descriptor.
        SSL_CTX *ctx;      ///< The SSL context.

        pthread_t pth;     ///< The identifier of the main loop's thread.


        // -- variables for the optional parallelism --

        std::vector<std::future<void>> servlets;  /// List of active servlest.


        // -- variables for the mandatory parallelism --

        std::vector<std::thread> workers;         /// The thread pool of workers.
        std::deque<SSL*> ssl_sockets;             /// All the active ssl connections that wait for the workers.

        std::mutex mux;                           /// Mutex to manipulate shared storages (e.g., ssl_sockets, stop).
        std::condition_variable cond;             /// Cond. variable used to notify the threadpool of workers about new jobs.
        bool stop = false;                        /// Stop flag for the mandatory parallelism.

    public:

        HTTPSServer(const HTTPSServer&) = delete;
        HTTPSServer& operator=(const HTTPSServer&) = delete;
        HTTPSServer(HTTPSServer&&) = delete;
        HTTPSServer& operator=(HTTPSServer&&) = delete;

        /// Construct the server to listen on the specified TCP address and port.
        explicit HTTPSServer(const std::string &address, std::size_t port, T &dispatcher, KeyProvider &keyProvider)
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

        /// Starts the HTTPSserver with the given threading model.
        /// If this method is called with 0 as parameter, optional parallelism
        /// is used. Numbers bigger than 0 will spawn a thread pool with the 
        /// given number of threads (mandatory parallelsism).
        /// \param thnum            the number of the threads
        void run(std::size_t thnum = 0) {

            if (thnum) {

                // create the worker pool
                for(std::size_t i = 0; i < thnum; i++) {
                    workers.emplace_back([this] {

                        for(;;) {

                            SSL *ssl; // this will be set in the ubsequent lines

                            {
                                std::unique_lock<std::mutex> _{ mux };
                                cond.wait(_, [this]{ return this->stop || !this->ssl_sockets.empty(); });
                                if (stop && ssl_sockets.empty())
                                    return;
                                ssl = std::move(ssl_sockets.front());
                                ssl_sockets.pop_front();
                            }

                            // run the servlet
                            servlet(ssl);
                        }

                    });
                }
            }

            // start the servers main loop with the selected parallelism (e.g., mandatory or optional)
            run_internal(thnum > 0);
        }


        /// Stops the server.
        void kill() {
            pthread_kill(pth, SIGINT); // send kill to the pthread
        }


    private:

        /// Main loop of the server. The parameter decides whether mandatory or optional
        /// parallelism is used to serve incomming requests.
        /// \param mandatory        whether to use mandatory parallelism
        void run_internal(bool mandatory) {

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

            // the pthread; used by kill
            pth = pthread_self();

            // the timeout
//            struct timeval tv;
//            tv.tv_sec = 15;

            // the infinite main loop
            // we can exit is by sending a data to the signal pipe
            while (1) {

                int rc = poll(fds, 2, -1);

                if (rc < 0) {
                    Parent::error("127.0.0.1", "Polling failed.");
                    break;
                }
                if(fds[1].revents == POLLIN) {
                    const auto s = read(fds[1].fd, &fdsi, sizeof(fdsi));
                    if (s != sizeof(fdsi))
                        Parent::error("127.0.0.1", "Error reading signal.");
                    if (fdsi.ssi_signo == SIGINT || fdsi.ssi_signo == SIGQUIT) {
                        ; // everything is ok
                    }
                    else {
                        Parent::error("127.0.0.1", "Unexpected signal caught.");
                    }
                    break;
                }
                if(fds[0].revents != POLLIN) {
                    continue;
                }

                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                SSL *ssl;

                int client = accept(server, (struct sockaddr*)&addr, &len);  /* accept connection as usual */

                struct timeval tv;
                tv.tv_sec = 15;
                setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));

                ssl = SSL_new(ctx);         /* get new SSL state with context     */
                SSL_set_fd(ssl, client);    /* set connection socket to SSL state */

                if (mandatory) {
                    {
                        std::lock_guard<std::mutex> _{ mux };
                        ssl_sockets.emplace_back(ssl);
                    }
                    cond.notify_one();
                }
                else {
                    // create the new servlet: servlet(ssl);
                    servlets.push_back(std::async([this](auto *ssl){ this->servlet(ssl); }, ssl));

                    // remove finished servlets
                    //for (auto it = servlets.begin(); it != servlets.end();) {
                    //    const auto r = it->wait_for(std::chrono::seconds(0));
                    //    if(r == std::future_status::ready || r == std::future_status::deferred)
                    //        it = servlets.erase(it);
                    //    else
                    //        ++it;
                    //}

                    const auto rit = std::remove_if(servlets.begin(), servlets.end(), [](std::future<void> &f) {
                        const auto r = f.wait_for(std::chrono::seconds(0));
                        return (r == std::future_status::ready || r == std::future_status::deferred);
                    });
                    servlets.erase(rit, servlets.end());
                }
            }


            if (mandatory) {
                {
                    std::lock_guard<std::mutex> _{ mux };
                    stop = true;
                }
                cond.notify_all();
                for(std::thread &worker: workers)
                    worker.join();
            }
            else {
                // wait for all open connections here
                for(auto &srv : servlets) {
                    srv.wait();
                }
            }
        }


    private:

        /// Initializes the SSL context.
        /// \return                 the context; in case of error nullptr is returned
        SSL_CTX* initCTX() {
            OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
            SSL_load_error_strings();      /* load all error messages */

            const SSL_METHOD *method = TLS_server_method(); // SSLv23_server_method(); //SSLv3_server_method();
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

            SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

            int len = 0;
            while ((len = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {

                buffer[len] = 0;

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

                    // get the peer's address
                    struct sockaddr_in addr;
                    socklen_t slen;
                    getpeername(SSL_get_fd(ssl), (struct sockaddr*)&addr, &slen);

                    // notify
                    Parent::error(inet_ntoa(addr.sin_addr), "Cannot parse request.");

                    // create and send a factory response
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
