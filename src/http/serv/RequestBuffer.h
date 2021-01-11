#ifndef _HTTP_REQUESTBUFFER_H_
#define _HTTP_REQUESTBUFFER_H_


#include <sstream>
#include <string>
#include <vector>

#include "http/crate/Request.h"


/// A request received from a client.
struct RequestBuffer {

    /// HTTP header structure.
    struct Header {
        std::string name;    ///< The name of the header.
        std::string value;   ///< The value of the header.
    };


    std::string remote_address;   ///< The remote address.

    std::string method;           ///< The request method, e.g., GET, POST, PATCH, ...
    std::string uri;              ///< The requested URI.

    int http_version_major;
    int http_version_minor;
    bool keepAlive = false;

    std::vector<Header> headers;  ///< All the headers in the http request.

    std::string content;          ///< The content.


    /// Clears  the internal state of the buffer.
    void clear() {
        method.clear();
        uri.clear();
        headers.clear();
        content.clear();
    }

    /// Converts the buffer into a request.
    Request to_request() && {
        return { std::move(remote_address), std::move(method), std::move(uri), std::move(content) };
    }

    /// Converts the buffer into a request.
    Request to_request() & {
        return { remote_address, method, uri, content };
    }

    /// Creates the string representation of the stored HTTP request.
    std::string to_string() const {

        std::stringstream stream;
        stream << method << " " << uri << " HTTP/"
               << http_version_major << "." << http_version_minor << "\n";

        for(const auto &header : headers)
        {
            stream << header.name << ": " << header.value << "\n";
        }

        stream << content << "\n";
        stream << "+ keep-alive: " << keepAlive << "\n";;
        return stream.str();
    }

};


#endif  /* _HTTP_REQUESTBUFFER_H_ */
