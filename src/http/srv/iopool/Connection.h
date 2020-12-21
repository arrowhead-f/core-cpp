#ifndef HTTP_SERVER2_CONNECTION_HPP
#define HTTP_SERVER2_CONNECTION_HPP

#include <asio/asio.hpp>

#include <memory>
#include <array>

#include "asio/asio/ssl.hpp"

#include "../Reply.h"
#include "../Request.h"
#include "../RequestHandler.h"
#include "../RequestParser.h"

namespace http {
namespace server {

    namespace iopool {

        /// Represents a single connection from a client.
        class Connection : public std::enable_shared_from_this<Connection> {

            private:

                asio::ip::tcp::socket socket_;   ///< Socket for the connection.
                std::array<char, 8192> buffer_;  ///< Buffer for incoming data.

                RequestHandler& request_handler_;  ///< The handler used to process the incoming request.
                Request request_;                  ///< The incoming request.
                RequestParser request_parser_;     ///< The parser for the incoming request.
                Reply reply_;                      ///< The reply to be sent back to the client.


            public:

                /// Construct a connection with the given io_context.
                explicit Connection(asio::io_context &io_context, RequestHandler &handler);

                Connection(const Connection&) = delete;
                Connection& operator=(Connection&) = delete;

                /// Get the socket associated with the connection.
                auto& socket() {
                    return socket_;
                }

                /// Start the first asynchronous operation for the connection.
                void start();

            private:

                /// Handle completion of a read operation.
                void handle_read(const std::error_code &e, std::size_t bytes_transferred);

                /// Handle completion of a write operation.
                void handle_write(const std::error_code &e);

        };  // class Connection


        /// Represents a single connection from a client.
        class SSLConnection : public std::enable_shared_from_this<SSLConnection> {

            private:

                asio::ssl::stream<asio::ip::tcp::socket> socket_;  ///< Socket for the connection.
                std::array<char, 8192> buffer_;                    ///< Buffer for incoming data.

                RequestHandler& request_handler_;  ///< The handler used to process the incoming request.
                Request request_;                  ///< The incoming request.
                RequestParser request_parser_;     ///< The parser for the incoming request.
                Reply reply_;                      ///< The reply to be sent back to the client.


            public:

                /// Construct a connection with the given io_context.
                explicit SSLConnection(asio::io_context &io_context, asio::ssl::context &context_, RequestHandler &handler);

                SSLConnection(const SSLConnection&) = delete;
                SSLConnection& operator=(SSLConnection&) = delete;

                /// Get the socket associated with the connection.
                auto& socket() {
                    return socket_.lowest_layer();
                }

                /// Start the first asynchronous operation for the connection.
                void start();

            private:

                /// Handle completion of a read operation.
                void handle_read(const std::error_code &e, std::size_t bytes_transferred);

                /// Handle completion of a write operation.
                void handle_write(const std::error_code &e);

        };  // class SSLConnection


        using ConnectionPtr    = std::shared_ptr<Connection>;
        using SSLConnectionPtr = std::shared_ptr<SSLConnection>;

    }  // namespace iopool

} // namespace server
} // namespace http

#endif // HTTP_SERVER2_CONNECTION_HPP
