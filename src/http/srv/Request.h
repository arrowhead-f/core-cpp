#ifndef _HTTP_SERVER_REQUEST_H_
#define _HTTP_SERVER_REQUEST_H_

#include <sstream>
#include <string>
#include <vector>

#include "Header.h"

namespace http {
namespace server {

    /// A request received from a client.
    struct Request {

        std::string address;          ///< The remote address.

        std::string method;           ///< The request method, e.g., GET, POST, PATCH, ...
        std::string uri;              ///< The requested URI.
        int http_version_major;
        int http_version_minor;
        std::vector<Header> headers;  ///< All the headers in the http request.

        bool has_content           = false;   ///< Whether the request has content.
        std::size_t content_length = 0;       ///< The length of the content.
        std::stringstream payload;            ///< The content.
    };

} // namespace server
} // namespace http

#endif  /* _HTTP_SERVER_REQUEST_H_ */
