//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "Connection.h"

#include <vector>

#include "../RequestHandler.h"
#include "../Reply.h"
#include "../ReplyBuffer.h"

namespace http {
namespace server {

    namespace thpool {


        void Connection::handle_read(const std::error_code &ec, std::size_t bytes_transferred) {
            if (!ec) {
                http::server::RequestParser::result_type result;
                std::tie(result, std::ignore) = request_parser_.parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);

                if (result == http::server::RequestParser::good) {
                    request_.address = socket().remote_endpoint().address().to_string();
                    request_handler_.handle_request(request_, reply_);
                    asio::async_write(socket_, http::server::to_buffers(reply_),
                        strand_.wrap([this, self = shared_from_this()](const std::error_code &ec, std::size_t) {
                            handle_write(ec);
                        })
                    );
                }
                else if (result == http::server::RequestParser::bad) {
                    reply_ = http::server::Reply::stock_reply(http::server::Reply::bad_request);
                    asio::async_write(socket_, http::server::to_buffers(reply_),
                        strand_.wrap([this, self = shared_from_this()](const std::error_code &ec, std::size_t) {
                            handle_write(ec);
                        })
                    );
                }
                else if (result == http::server::RequestParser::requestlength) {
                    reply_ = http::server::Reply::stock_reply(http::server::Reply::length_required);
                    asio::async_write(socket_, http::server::to_buffers(reply_),
                        strand_.wrap([this, self = shared_from_this()](const std::error_code &ec, std::size_t) {
                            handle_write(ec);
                        })
                    );
                }
                else {
                    socket_.async_read_some(asio::buffer(buffer_),
                        strand_.wrap([this, self = shared_from_this()](const std::error_code &ec, std::size_t bytes_transferred) {
                            handle_read(ec, bytes_transferred);
                        })
                    );
                }
            }

            // If an error occurs then no new asynchronous operations are started. This
            // means that all shared_ptr references to the connection object will
            // disappear and the object will be destroyed automatically after this
            // handler returns. The connection class's destructor closes the socket.
        }


        void SSLConnection::handle_read(const std::error_code &ec, std::size_t bytes_transferred) {
            if (!ec) {
                http::server::RequestParser::result_type result;
                std::tie(result, std::ignore) = request_parser_.parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);

                if (result == http::server::RequestParser::good) {
                    request_.address = socket().remote_endpoint().address().to_string();
                    request_handler_.handle_request(request_, reply_);
                    asio::async_write(socket_, http::server::to_buffers(reply_),
                        strand_.wrap([this, self = shared_from_this()](const std::error_code &ec, std::size_t) {
                            handle_write(ec);
                        })
                    );
                }
                else if (result == http::server::RequestParser::bad) {
                    reply_ = http::server::Reply::stock_reply(http::server::Reply::bad_request);
                    asio::async_write(socket_, http::server::to_buffers(reply_),
                        strand_.wrap([this, self = shared_from_this()](const std::error_code &ec, std::size_t) {
                            handle_write(ec);
                        })
                    );
                }
                else if (result == http::server::RequestParser::requestlength) {
                    reply_ = http::server::Reply::stock_reply(http::server::Reply::length_required);
                    asio::async_write(socket_, http::server::to_buffers(reply_),
                        strand_.wrap([this, self = shared_from_this()](const std::error_code &ec, std::size_t) {
                            handle_write(ec);
                        })
                    );
                }
                else {
                    socket_.async_read_some(asio::buffer(buffer_),
                        strand_.wrap([this, self = shared_from_this()](const std::error_code &ec, std::size_t bytes_transferred) {
                            handle_read(ec, bytes_transferred);
                        })
                    );
                }
            }

            // If an error occurs then no new asynchronous operations are started. This
            // means that all shared_ptr references to the connection object will
            // disappear and the object will be destroyed automatically after this
            // handler returns. The connection class's destructor closes the socket.
        }

    }  // namespace thpool

} // namespace server
} // namespace http
