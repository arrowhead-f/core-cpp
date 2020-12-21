//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_CONNECTION_HPP
#define HTTP_SERVER3_CONNECTION_HPP

#include <array>
#include <memory>

#include <asio/asio.hpp>
#include <asio/asio/ssl.hpp>

#include "../Reply.h"
#include "../Request.h"
#include "../RequestHandler.h"
#include "../RequestParser.h"

namespace http {
namespace server {

    namespace thpool {

        /// Represents a single connection from a client.
        class Connection : public std::enable_shared_from_this<Connection> {

            public:

                /// Construct a connection with the given io_service.
                explicit Connection(asio::io_service &io_service, http::server::RequestHandler &handler)
                    : strand_{ io_service }
                    , socket_{ io_service }
                    , request_handler_{ handler } {
                }

                /// Get the socket associated with the connection.
                auto& socket() {
                    return socket_;
                }

                /// Start the first asynchronous operation for the connection.
                void start() {
                    socket_.async_read_some(asio::buffer(buffer_),
                        strand_.wrap([this, self = shared_from_this()](const std::error_code &ec, std::size_t bytes_transferred) {
                            handle_read(ec, bytes_transferred);
                        })
                    );
                }

            private:

                /// Handle completion of a read operation.
                void handle_read(const std::error_code &ec, std::size_t bytes_transferred);

                /// Handle completion of a write operation.
                void handle_write(const std::error_code &ec) {
                    if (!ec) {
                        // Initiate graceful connection closure.
                        std::error_code ignored_ec;
                        socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
                    }

                    // No new asynchronous operations are started. This means that all shared_ptr
                    // references to the connection object will disappear and the object will be
                    // destroyed automatically after this handler returns. The connection class's
                    // destructor closes the socket.
                }

                /// Strand to ensure the connection's handlers are not called concurrently.
                asio::io_service::strand strand_;

                /// The handler used to process the incoming request.
                http::server::RequestHandler &request_handler_;

                /// Buffer for incoming data.
                std::array<char, 8192> buffer_;

                asio::ip::tcp::socket socket_;

                /// The incoming request.
                http::server::Request request_;

                /// The parser for the incoming request.
                http::server::RequestParser request_parser_;

                /// The reply to be sent back to the client.
                http::server::Reply reply_;
        };


        /// Represents a single connection from a client.
        class SSLConnection : public std::enable_shared_from_this<SSLConnection> {

            public:

                /// Construct a connection with the given io_service.
                explicit SSLConnection(asio::io_service &io_service, asio::ssl::context &context_, http::server::RequestHandler &handler)
                    : strand_{ io_service }
                    , socket_{ asio::ip::tcp::socket{ io_service }, context_ }
                    , request_handler_{ handler } {
                }

                /// Get the socket associated with the connection.
                auto& socket() {
                    return socket_.lowest_layer();
                }

                void start() {
                    socket_.async_handshake(asio::ssl::stream_base::server, strand_.wrap([this, self = shared_from_this()](const std::error_code &error) {
                        if (!error) {
                            socket_.async_read_some(asio::buffer(buffer_),
                                strand_.wrap([this, self](const std::error_code &ec, std::size_t bytes_transferred) {
                                    handle_read(ec, bytes_transferred);
                                })
                            );
                        }
                    }));
                }

            private:

                /// Handle completion of a read operation.
                void handle_read(const std::error_code &ec, std::size_t bytes_transferred);

                /// Handle completion of a write operation.
                void handle_write(const std::error_code &ec) {
                    if (!ec) {
                        // Initiate graceful connection closure.
                        std::error_code ignored_ec;
                        socket_.lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
                    }

                    // No new asynchronous operations are started. This means that all shared_ptr
                    // references to the connection object will disappear and the object will be
                    // destroyed automatically after this handler returns. The connection class's
                    // destructor closes the socket.
                }

                /// Strand to ensure the connection's handlers are not called concurrently.
                asio::io_service::strand strand_;

                /// Socket for the connection.
                asio::ssl::stream<asio::ip::tcp::socket> socket_;  ///< Socket for the connection.

                /// The handler used to process the incoming request.
                http::server::RequestHandler &request_handler_;

                /// Buffer for incoming data.
                std::array<char, 8192> buffer_;

                /// The incoming request.
                http::server::Request request_;

                /// The parser for the incoming request.
                http::server::RequestParser request_parser_;

                /// The reply to be sent back to the client.
                http::server::Reply reply_;
        };



        using ConnectionPtr    = std::shared_ptr<Connection>;
        using SSLConnectionPtr = std::shared_ptr<SSLConnection>;

    }  // namespace thpool

} // namespace server
} // namespace http

#endif // HTTP_SERVER3_CONNECTION_HPP