#ifndef _HTTP_SERVER_REQUESTPARSER_H_
#define _HTTP_SERVER_REQUESTPARSER_H_

#include <tuple>
#include <iostream>

#include "Request.h"

namespace http {
namespace server {

    struct Request;

    /// Parser for incoming requests.
    class RequestParser {

        private:

            /// The current state of the parser.
            enum state {
                method_start,
                method,
                uri,
                http_version_h,
                http_version_t_1,
                http_version_t_2,
                http_version_p,
                http_version_slash,
                http_version_major_start,
                http_version_major,
                http_version_minor_start,
                http_version_minor,
                expecting_newline_1,
                header_line_start,
                header_lws,
                header_name,
                space_before_header_value,
                header_value,
                expecting_newline_2,
                expecting_newline_3
            } state_ = method_start;

            bool parsing_content_length = false;

        public:

            /// Reset to initial parser state.
            void reset() {
                state_ = method_start;
                parsing_content_length = false;
            }

                /// Result of parse.
            enum result_type {
                good, bad, indeterminate, requestlength
            };

            /// Parse some data. The enum return value is good when a complete request has
            /// been parsed, bad if the data is invalid, indeterminate when more data is
            /// required. The InputIterator return value indicates how much of the input
            /// has been consumed.
            template<typename InputIterator>
            std::tuple<result_type, InputIterator> parse(Request &req, InputIterator begin, InputIterator end) {
                while (begin != end) {
                    result_type result = consume(req, *begin++);
                    if (result == bad)
                        return std::make_tuple(bad, begin);
                    if (result == good) {
                        if (begin == end) {
                            return std::make_tuple(good, begin);
                        }
                        else {
                            if (req.has_content) {
                                const std::size_t payload_length = std::distance(begin, end);
                                if (payload_length == req.content_length) {
                                    req.payload << std::string{ begin, end };
                                    return std::make_tuple(good, begin);
                                }
                                return std::make_tuple(bad, begin);
                            }
                            else {
                                return std::make_tuple(requestlength, begin);
                            }
                        }
                    }

                }
                return std::make_tuple(indeterminate, begin);
            }

        private:

            /// Handle the next character of input.
            result_type consume(Request &req, char input);

            /// Check if a byte is an HTTP character.
            static bool is_char(int c);

            /// Check if a byte is an HTTP control character.
            static bool is_ctl(int c);

            /// Check if a byte is defined as an HTTP tspecial character.
            static bool is_tspecial(int c);

            /// Check if a byte is a digit.
            static bool is_digit(int c);

    };  // class RequestHandler

}  // namespace server
}  // namespace http

#endif  /* _HTTP_SERVER_REQUESTPARSER_H_ */
