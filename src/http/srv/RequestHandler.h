#ifndef _HTTP_SERVER_REQUESTHANDLER_H_
#define _HTTP_SERVER_REQUESTHANDLER_H_

#include <string>

namespace http {
namespace server {

    struct Reply;
    struct Request;

    /// The common handler for all incoming requests.
    class RequestHandler {

        public:
            RequestHandler() = default;
            RequestHandler(const RequestHandler&) = delete;
            RequestHandler& operator=(const RequestHandler&) = delete;

            /// Handle a request and produce a reply.
            void handle_request(const Request &req, Reply &rep);

            virtual int handle(const std::string &from, const char *method, const std::string &uri, std::string &response, const std::string &payload) = 0;
            virtual void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) = 0;

        private:

            /// Perform URL-decoding on a string. Returns false if the encoding was
            /// invalid.
            static bool url_decode(const std::string &in, std::string &out);

    };  // class RequestHandler

}  // namespace server
}  // namespace http

#endif  /* _HTTP_SERVER_REQUESTHANDLER_H_ */
