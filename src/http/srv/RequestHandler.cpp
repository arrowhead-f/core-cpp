#include "RequestHandler.h"

#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

//#include "MimeTypes.h"
#include "Reply.h"
#include "Request.h"

namespace http {
namespace server {

    void RequestHandler::handle_request(const Request &req, Reply &rep) {
        // Decode url to path.
        std::string request_path;
        if (!url_decode(req.uri, request_path)) {
            rep = Reply::stock_reply(Reply::bad_request);
            return;
        }

        // Request path must be absolute and not contain "..".
        if (request_path.empty() || request_path[0] != '/' || request_path.find("..") != std::string::npos) {
            rep = Reply::stock_reply(Reply::bad_request);
            return;
        }

        std::string response;
        if (handle(req.address, req.method.c_str(), std::move(req.uri), response, req.payload.str())) {
            rep = Reply::stock_reply(Reply::bad_request);
            return;
        }

        // Fill out the reply to be sent to the client.
        rep.status  = Reply::ok;
        rep.content = response;

        rep.headers.resize(2);
        rep.headers[0].name = "Content-Length";
        rep.headers[0].value = std::to_string(rep.content.size());
        rep.headers[1].name = "Content-Type";
        rep.headers[1].value = "application/json";

    }

    bool RequestHandler::url_decode(const std::string &in, std::string &out) {
        out.clear();
        out.reserve(in.size());
        for (std::size_t i = 0; i < in.size(); ++i) {
            if (in[i] == '%') {
                if (i + 3 <= in.size()) {
                    int value = 0;
                    std::istringstream is(in.substr(i + 1, 2));
                    if (is >> std::hex >> value) {
                        out += static_cast<char>(value);
                        i += 2;
                    }
                    else {
                        return false;
                    }
                }
                else {
                    return false;
                }
            }
            else if (in[i] == '+') {
                out += ' ';
            }
            else {
                out += in[i];
            }
        }
        return true;
    }

} // namespace server
} // namespace http
