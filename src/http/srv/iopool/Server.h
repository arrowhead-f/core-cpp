#ifndef _HTTP_SERVER_IOPOOL_HPP
#define _HTTP_SERVER_IOPOOL_HPP

#include <asio/asio.hpp>

#include <string>
#include <vector>
#include <memory>
#include <cstring>

#include <iostream>

#include "http/HTTPServer.h"
#include "http/KeyProvider.h"
#include "Connection.h"
#include "IOContextPool.h"
#include "../RequestHandler.h"

namespace http {
namespace server {

    namespace iopool {

        template<typename T>
        class HTTPServer : public http::HTTPServer<T>, public http::server::RequestHandler {

            private:

                IOContextPool ioc_pool;     ///< The pool of io_context objects used to perform asynchronous operations.

                asio::signal_set signals_;          ///< The signal_set is used to register for process termination notifications.
                asio::ip::tcp::acceptor acceptor_;  ///< Acceptor used to listen for incoming connections.

                ConnectionPtr new_connection_;      ///< The next connection to be accepted.

            public:

                HTTPServer(const HTTPServer&) = delete;
                HTTPServer& operator=(const HTTPServer&) = delete;
                HTTPServer(HTTPServer&&) = delete;
                HTTPServer& operator=(HTTPServer&&) = delete;

                /// Construct the server to listen on the specified TCP address and port, and
                /// serve up files from the given directory.
                explicit HTTPServer(const std::string &address, std::size_t port, T &dispatcher, std::size_t poolSize)
                    : http::HTTPServer<T>{ address, port, dispatcher }
                    , ioc_pool{ poolSize }
                    , signals_{ ioc_pool.master() }
                    , acceptor_{ ioc_pool.master() }
                    , new_connection_{} {

                    signals_.add(SIGINT);
                    signals_.add(SIGTERM);
                    #if defined(SIGQUIT)
                      signals_.add(SIGQUIT);
                    #endif
                    signals_.async_wait([this](const auto&...) { handle_stop(); });

                    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
                    asio::ip::tcp::resolver resolver{ acceptor_.get_executor() };
                    asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, std::to_string(port)).begin();

                    acceptor_.open(endpoint.protocol());
                    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
                    acceptor_.bind(endpoint);
                    acceptor_.listen();

                    start_accept();
                }

                /// Run the server's io_context loop.
                void run() {
                    ioc_pool.run();
                }

                int handle(const std::string &from, const char *method, const std::string &uri, std::string &response, const std::string &payload) final {
                    return http::HTTPServer<T>::handle(from, method, uri, response, payload);
                }

                void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) final {
                    http::HTTPServer<T>::denied(from, method, uri, reason);
                }

            private:

                /// Initiate an asynchronous accept operation.
                void start_accept() {
                    new_connection_.reset(new Connection{ ioc_pool.get(), *this });

                    acceptor_.async_accept(new_connection_->socket(), [this](const std::error_code &e) {
                        handle_accept(e);
                    });
                }

                /// Handle completion of an asynchronous accept operation.
                void handle_accept(const std::error_code &e) {
                    if (!e) {
                        new_connection_->start();
                    }
                    start_accept();
                }

                /// Handle a request to stop the server.
                void handle_stop() {
                    ioc_pool.stop();
                }

        };  // class HTTPServer


        template<typename T>
        class HTTPSServer : public http::HTTPServer<T>, public http::server::RequestHandler {

            private:

                IOContextPool ioc_pool;     ///< The pool of io_context objects used to perform asynchronous operations.

                asio::signal_set signals_;          ///< The signal_set is used to register for process termination notifications.
                asio::ip::tcp::acceptor acceptor_;  ///< Acceptor used to listen for incoming connections.
                asio::ssl::context context_;

                SSLConnectionPtr new_connection_;   ///< The next connection to be accepted.

            public:

                HTTPSServer(const HTTPSServer&) = delete;
                HTTPSServer& operator=(const HTTPSServer&) = delete;
                HTTPSServer(HTTPSServer&&) = delete;
                HTTPSServer& operator=(HTTPSServer&&) = delete;

                /// Construct the server to listen on the specified TCP address and port, and
                /// serve up files from the given directory.
                explicit HTTPSServer(const std::string &address, std::size_t port, T &dispatcher, http::KeyProvider &keyProvider, std::size_t poolSize)
                    : http::HTTPServer<T>{ address, port, dispatcher }
                    , ioc_pool{ poolSize }
                    , signals_{ ioc_pool.master() }
                    , acceptor_{ ioc_pool.master() }
                    , context_(asio::ssl::context::sslv23)
                    , new_connection_{} {

                    signals_.add(SIGINT);
                    signals_.add(SIGTERM);
                    #if defined(SIGQUIT)
                      signals_.add(SIGQUIT);
                    #endif
                    signals_.async_wait([this](const auto&...) { handle_stop(); });

                    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
                    asio::ip::tcp::resolver resolver{ acceptor_.get_executor() };
                    asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, std::to_string(port)).begin();

                    acceptor_.open(endpoint.protocol());
                    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
                    acceptor_.bind(endpoint);
                    acceptor_.listen();

                    context_.set_options(asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2);
                    //| asio::ssl::context::single_dh_use);
                    context_.set_password_callback([password = keyProvider.keyPasswd](std::size_t max_length, asio::ssl::context::password_purpose purpose){ return password; });
                    context_.use_certificate_chain_file(keyProvider.sslCert);
                    context_.use_private_key_file(keyProvider.sslKey, asio::ssl::context::pem);

                    start_accept();
                }

                /// Run the server's io_context loop.
                void run() {
                    ioc_pool.run();
                }

                int handle(const std::string &from, const char *method, const std::string &uri, std::string &response, const std::string &payload) final {
                    return http::HTTPServer<T>::handle(from, method, uri, response, payload);
                }

                void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) final {
                    http::HTTPServer<T>::denied(from, method, uri, reason);
                }

            private:

                /// Initiate an asynchronous accept operation.
                void start_accept() {
                    new_connection_.reset(new SSLConnection{ ioc_pool.get(), context_, *this });
                    acceptor_.async_accept(new_connection_->socket(), [this](const std::error_code &e) {
                        handle_accept(e);
                    });
                }

                /// Handle completion of an asynchronous accept operation.
                void handle_accept(const std::error_code &e) {
                    if (!e) {
                        new_connection_->start();
                    }
                    start_accept();
                }

                /// Handle a request to stop the server.
                void handle_stop() {
                    ioc_pool.stop();
                }

        };  // class HTTPSServer


    }  // namespace iopool

} // namespace server
} // namespace http

#endif // HTTP_SERVER2_SERVER_HPP
