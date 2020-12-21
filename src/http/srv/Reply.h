#ifndef _HTTP_SERVER_REPLY_H_
#define _HTTP_SERVER_REPLY_H_

#include <string>
#include <vector>

//#include "asio/asio.hpp"
#include "Header.h"

namespace http {
namespace server {

    /// A reply to be sent to a client.
    struct Reply {

        /// The status of the reply.
        enum status_type {
            ok                    = 200,
            created               = 201,
            accepted              = 202,
            no_content            = 204,
            multiple_choices      = 300,
            moved_permanently     = 301,
            moved_temporarily     = 302,
            not_modified          = 304,
            bad_request           = 400,
            unauthorized          = 401,
            forbidden             = 403,
            not_found             = 404,
            length_required       = 411,
            internal_server_error = 500,
            not_implemented       = 501,
            bad_gateway           = 502,
            service_unavailable   = 503
        } status;

        /// The headers to be included in the reply.
        std::vector<Header> headers;

        /// The content to be sent in the reply.
        std::string content;

        // Get the reply in the form of a string.
        std::string to_string() const;
        operator std::string() const { return to_string(); }

        /// Get a stock reply.
        static Reply stock_reply(status_type status);
    };

} // namespace server
} // namespace http

#endif  /* _HTTP_SERVER_REPLY_H_ */
