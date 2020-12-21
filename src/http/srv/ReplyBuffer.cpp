#include "ReplyBuffer.h"

#include <string>

namespace http {
namespace server {

    namespace status_strings {

            const std::string ok                    = "HTTP/1.0 200 OK\r\n";
            const std::string created               = "HTTP/1.0 201 Created\r\n";
            const std::string accepted              = "HTTP/1.0 202 Accepted\r\n";
            const std::string no_content            = "HTTP/1.0 204 No Content\r\n";
            const std::string multiple_choices      = "HTTP/1.0 300 Multiple Choices\r\n";
            const std::string moved_permanently     = "HTTP/1.0 301 Moved Permanently\r\n";
            const std::string moved_temporarily     = "HTTP/1.0 302 Moved Temporarily\r\n";
            const std::string not_modified          = "HTTP/1.0 304 Not Modified\r\n";
            const std::string bad_request           = "HTTP/1.0 400 Bad Request\r\n";
            const std::string unauthorized          = "HTTP/1.0 401 Unauthorized\r\n";
            const std::string forbidden             = "HTTP/1.0 403 Forbidden\r\n";
            const std::string not_found             = "HTTP/1.0 404 Not Found\r\n";
            const std::string length_required       = "HTTP/1.0 411 Length Required\r\n";
            const std::string internal_server_error = "HTTP/1.0 500 Internal Server Error\r\n";
            const std::string not_implemented       = "HTTP/1.0 501 Not Implemented\r\n";
            const std::string bad_gateway           = "HTTP/1.0 502 Bad Gateway\r\n";
            const std::string service_unavailable   = "HTTP/1.0 503 Service Unavailable\r\n";

            asio::const_buffer to_buffer(Reply::status_type status) {
                switch (status) {
                    case Reply::ok:
                        return asio::buffer(ok);
                    case Reply::created:
                        return asio::buffer(created);
                    case Reply::accepted:
                        return asio::buffer(accepted);
                    case Reply::no_content:
                        return asio::buffer(no_content);
                    case Reply::multiple_choices:
                        return asio::buffer(multiple_choices);
                    case Reply::moved_permanently:
                        return asio::buffer(moved_permanently);
                    case Reply::moved_temporarily:
                        return asio::buffer(moved_temporarily);
                    case Reply::not_modified:
                        return asio::buffer(not_modified);
                    case Reply::bad_request:
                        return asio::buffer(bad_request);
                    case Reply::unauthorized:
                        return asio::buffer(unauthorized);
                    case Reply::forbidden:
                        return asio::buffer(forbidden);
                    case Reply::not_found:
                        return asio::buffer(not_found);
                    case Reply::length_required:
                        return asio::buffer(length_required);
                    case Reply::internal_server_error:
                        return asio::buffer(internal_server_error);
                    case Reply::not_implemented:
                        return asio::buffer(not_implemented);
                    case Reply::bad_gateway:
                        return asio::buffer(bad_gateway);
                    case Reply::service_unavailable:
                        return asio::buffer(service_unavailable);
                    default:
                        return asio::buffer(internal_server_error);
                }
            }

        } // namespace status_strings

        namespace {

            const char name_value_separator[] = {':', ' '};
            const char crlf[] = {'\r', '\n'};

        }

        std::vector<asio::const_buffer> to_buffers(Reply &reply) {
            std::vector<asio::const_buffer> buffers;
            buffers.push_back(status_strings::to_buffer(reply.status));
            for (std::size_t i = 0; i < reply.headers.size(); ++i) {
                Header &h = reply.headers[i];
                buffers.push_back(asio::buffer(h.name));
                buffers.push_back(asio::buffer(name_value_separator));
                buffers.push_back(asio::buffer(h.value));
                buffers.push_back(asio::buffer(crlf));
            }
            buffers.push_back(asio::buffer(crlf));
            buffers.push_back(asio::buffer(reply.content));
            return buffers;
        }

} // namespace server
} // namespace http
