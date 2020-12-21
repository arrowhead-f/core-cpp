#include "Connection.h"

#include <vector>

#include <iostream>

#include "../ReplyBuffer.h"
#include "../RequestHandler.h"

namespace http {
namespace server {

    namespace iopool {

        Connection::Connection(asio::io_context &io_context, RequestHandler &handler) : socket_(io_context), request_handler_(handler) {
        }

        void Connection::start() {
            socket_.async_read_some(asio::buffer(buffer_), [this, self = shared_from_this()](const std::error_code &e, std::size_t bytes_transferred) {
                handle_read(e, bytes_transferred);
            });
        }

        void Connection::handle_read(const std::error_code &e, std::size_t bytes_transferred) {
            if (!e) {
                http::server::RequestParser::result_type result;
                std::tie(result, std::ignore) = request_parser_.parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);

                if (result) {
                    request_.address = socket().remote_endpoint().address().to_string();
                    request_handler_.handle_request(request_, reply_);

                    asio::async_write(socket_, http::server::to_buffers(reply_), [this, self = shared_from_this()](const std::error_code &e, std::size_t) {
                        handle_write(e);
                    });
                }
                else if (!result) {
                    reply_ = http::server::Reply::stock_reply(http::server::Reply::bad_request);

                    asio::async_write(socket_, http::server::to_buffers(reply_),  [this, self = shared_from_this()](const std::error_code &e, std::size_t) {
                        handle_write(e);
                    });
                }
                else {
                    socket_.async_read_some(asio::buffer(buffer_), [this, self = shared_from_this()](const std::error_code &e, std::size_t bytes_transferred) {
                        handle_read(e, bytes_transferred);
                    });
                }
            }
        }

        void Connection::handle_write(const std::error_code &e) {
            if (!e) {
                std::error_code ignored_ec;
                socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }
        }


        SSLConnection::SSLConnection(asio::io_context &io_context, asio::ssl::context &context_, RequestHandler &handler)
            : socket_{ asio::ip::tcp::socket{ io_context }, context_ }
            , request_handler_{ handler } {
        }

        void SSLConnection::start() {
            socket_.async_handshake(asio::ssl::stream_base::server, [this, self = shared_from_this()](const std::error_code &ec) {
                if (!ec) {
                    socket_.async_read_some(asio::buffer(buffer_), [this, self](const std::error_code &e, std::size_t bytes_transferred) {
                        handle_read(e, bytes_transferred);
                    });
                }
            });
        }

        void SSLConnection::handle_read(const std::error_code &e, std::size_t bytes_transferred) {
            if (!e) {
                http::server::RequestParser::result_type result;
                std::tie(result, std::ignore) = request_parser_.parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);

                if (result == http::server::RequestParser::good) {
                    request_.address = socket().remote_endpoint().address().to_string();
                    request_handler_.handle_request(request_, reply_);

                    asio::async_write(socket_, http::server::to_buffers(reply_), [this, self = shared_from_this()](const std::error_code &e, std::size_t) {
                        handle_write(e);
                    });
                }
                else if (result == http::server::RequestParser::bad) {
                    reply_ = http::server::Reply::stock_reply(http::server::Reply::bad_request);
                    asio::async_write(socket_, http::server::to_buffers(reply_),  [this, self = shared_from_this()](const std::error_code &e, std::size_t) {
                        handle_write(e);
                    });
                }
                else if (result == http::server::RequestParser::requestlength) {
                    reply_ = http::server::Reply::stock_reply(http::server::Reply::length_required);
                    asio::async_write(socket_, http::server::to_buffers(reply_),  [this, self = shared_from_this()](const std::error_code &e, std::size_t) {
                        handle_write(e);
                    });
                }
                else {
                    socket_.async_read_some(asio::buffer(buffer_), [this, self = shared_from_this()](const std::error_code &e, std::size_t bytes_transferred) {
                        handle_read(e, bytes_transferred);
                    });
                }
            }
        }

        void SSLConnection::handle_write(const std::error_code &e) {
            if (!e) {
                std::error_code ignored_ec;
                socket_.lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }
        }

    }  // namespace iopool

} // namespace server
} // namespace http
