//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_SERVER_HPP
#define HTTP_SERVER3_SERVER_HPP

#include <memory>
#include <string>
#include <vector>

#include "http/HTTPServer.h"
#include "http/KeyProvider.h"

#include <asio/asio.hpp>

#include "Connection.h"
#include "../RequestHandler.h"

namespace http {
namespace server {

    namespace thpool {

        /// The top-level class of the HTTP server.
        template<typename T>
        class HTTPServer : public http::HTTPServer<T>, public http::server::RequestHandler {

            public:

                HTTPServer(const HTTPServer&) = delete;
                HTTPServer& operator=(const HTTPServer&) = delete;
                HTTPServer(HTTPServer&&) = delete;
                HTTPServer& operator=(HTTPServer&&) = delete;

                /// Construct the server to listen on the specified TCP address and port, and
                /// serve up files from the given directory.
                explicit HTTPServer(const std::string &address, std::size_t port, T &dispatcher, std::size_t thPoolSize)
                    : http::HTTPServer<T>{ address, port, dispatcher }
                    , thread_pool_size_{ thPoolSize }
                    , signals_{ io_service_ }
                    , acceptor_{ io_service_ }
                    , new_connection_{} {

                    // Register to handle the signals that indicate when the server should exit.
                    // It is safe to register for the same signal multiple times in a program,
                    // provided all registration for the specified signal is made through Asio.
                    signals_.add(SIGINT);
                    signals_.add(SIGTERM);
                    #if defined(SIGQUIT)
                    signals_.add(SIGQUIT);
                    #endif // defined(SIGQUIT)

                    signals_.async_wait([this](std::error_code /*ec*/, int /*signo*/) {
                        handle_stop();
                    });

                    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
                    asio::ip::tcp::resolver resolver(io_service_);
                    asio::ip::tcp::resolver::query query(address, std::to_string(port));
                    asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
                    acceptor_.open(endpoint.protocol());
                    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
                    acceptor_.bind(endpoint);
                    acceptor_.listen();

                    start_accept();
                }

                /// Run the server's io_service loop.
                void run() {
                    // Create a pool of threads to run all of the io_services.
                    std::vector<std::shared_ptr<std::thread>> threads;
                    for (std::size_t i = 0; i < thread_pool_size_; ++i) {
                        std::shared_ptr<std::thread> thread{ new std::thread{
                            [this]() {
                                io_service_.run();
                            }
                        }};
                        threads.push_back(thread);
                    }

                    // Wait for all threads in the pool to exit.
                    for (std::size_t i = 0; i < threads.size(); ++i)
                        threads[i]->join();
                }

                int handle(const std::string &from, const char *method, const std::string &uri, std::string &response, const std::string &payload) final {
                    return http::HTTPServer<T>::handle(from, method, uri, response, payload);
                }

                void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) final {
                    http::HTTPServer<T>::handle_error(from, method, uri, reason);
                }

            private:

                /// Initiate an asynchronous accept operation.
                void start_accept() {
                    new_connection_.reset(new Connection{ io_service_, *this });
                    acceptor_.async_accept(new_connection_->socket(), [this](std::error_code ec) {
                        handle_accept(ec);
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
                    io_service_.stop();
                }

                /// The number of threads that will call io_service::run().
                std::size_t thread_pool_size_;

                /// The io_service used to perform asynchronous operations.
                asio::io_service io_service_;

                /// The signal_set is used to register for process termination notifications.
                asio::signal_set signals_;

                /// Acceptor used to listen for incoming connections.
                asio::ip::tcp::acceptor acceptor_;

                /// The next connection to be accepted.
                ConnectionPtr new_connection_;

        };


        /// The top-level class of the HTTP server.
        template<typename T>
        class HTTPSServer : public http::HTTPServer<T>, public http::server::RequestHandler {
            private:

                std::size_t thread_pool_size_;      ///< The number of threads that will call io_service::run().
                asio::io_service io_service_;       ///< The io_service used to perform asynchronous operations.
                asio::signal_set signals_;          ///< The signal_set is used to register for process termination notifications.
                asio::ip::tcp::acceptor acceptor_;  ///< Acceptor used to listen for incoming connections.

                asio::ssl::context context_;

                SSLConnectionPtr new_connection_;              ///< The next connection to be accepted.

            public:

                HTTPSServer(const HTTPSServer&) = delete;
                HTTPSServer& operator=(const HTTPSServer&) = delete;
                HTTPSServer(HTTPSServer&&) = delete;
                HTTPSServer& operator=(HTTPSServer&&) = delete;

                /// Construct the server to listen on the specified TCP address and port, and
                /// serve up files from the given directory.
                explicit HTTPSServer(const std::string &address, std::size_t port, T &dispatcher, http::KeyProvider &keyProvider, std::size_t thPoolSize)
                        : http::HTTPServer<T>{ address, port, dispatcher }
                        , thread_pool_size_{ thPoolSize }
                        , signals_{ io_service_ }
                        , acceptor_{ io_service_ }
                        , context_(asio::ssl::context::sslv23)
                        , new_connection_{} {

                    // Register to handle the signals that indicate when the server should exit.
                    // It is safe to register for the same signal multiple times in a program,
                    // provided all registration for the specified signal is made through Asio.
                    signals_.add(SIGINT);
                    signals_.add(SIGTERM);
                    #if defined(SIGQUIT)
                    signals_.add(SIGQUIT);
                    #endif // defined(SIGQUIT)

                    signals_.async_wait([this](std::error_code /*ec*/, int /*signo*/) {
                        handle_stop();
                    });

                    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
                    asio::ip::tcp::resolver resolver(io_service_);
                    asio::ip::tcp::resolver::query query(address, std::to_string(port));
                    asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
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

                /// Run the server's io_service loop.
                void run() {
                    // Create a pool of threads to run all of the io_services.
                    std::vector<std::shared_ptr<std::thread>> threads;
                    for (std::size_t i = 0; i < thread_pool_size_; ++i) {
                        std::shared_ptr<std::thread> thread{ new std::thread{
                                [this]() {
                                    io_service_.run();
                                }
                        }};
                        threads.push_back(thread);
                    }

                    // Wait for all threads in the pool to exit.
                    for (std::size_t i = 0; i < threads.size(); ++i)
                        threads[i]->join();
                }

                int handle(const std::string &from_address, const char *method, const std::string &uri, std::string &response, const std::string &payload) final {
                    return http::HTTPServer<T>::handle(from_address, method, uri, response, payload);
                }

                void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) final {
                    http::HTTPServer<T>::handle_error(from, method, uri, reason);
                }

            private:

                /// Initiate an asynchronous accept operation.
                void start_accept() {
                    new_connection_.reset(new SSLConnection{ io_service_, context_, *this });
                    acceptor_.async_accept(new_connection_->socket(), [this](std::error_code ec) {
                        handle_accept(ec);
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
                    io_service_.stop();
                }

        };

    }  // namespace thpool

} // namespace server
} // namespace http

#endif // HTTP_SERVER3_SERVER_HPP
