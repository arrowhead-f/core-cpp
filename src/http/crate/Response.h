#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_


#include <string>

#include "http/StatusCode.h"


/// The reply to be send to the remote party.
class Response {

    private:

        http::status_code status_code = http::status_code::OK;  ///< The status code.

        std::string mime_type;  ///< The mime type.
        std::string content;    ///< The content.

        std::string allow;      ///< Allow header.

    public:

        /// Creates an OK reply with the given content.
        /// \param content          the content of the reply
        /// \param mime_type        the mime type of the content, defaults to application/json
        Response(std::string content, std::string mime_type = "application/json") : mime_type{ std::move(mime_type) }, content{ std::move(content) } {}

        /// Creates reply with the given sttus_code and content.
        /// \param sc               the status code of the reply
        /// \param content          the content of the reply
        /// \param mime_type        the mime type of the content, defaults to application/json
        Response(http::status_code sc, std::string content, std::string mime_type = "application/json") : status_code{ sc }, mime_type{ std::move(mime_type) }, content{ std::move(content) } {}

        /// Get the reply in the form of a string.
        /// \param keep_alive       whether keep-alive should be set; defaults to false
        /// \return                 the string representation of the reply
        std::string to_string(bool keep_alive = false) const;

        /// Returns the status code of the response.
        /// \return                 the status code
        operator http::status_code() const {
            return status_code;
        }

        /// Returns the content of the response.
        /// \return                 the content of the response
        const std::string& value() const {
            return content;
        }

        /// Get a stock reply.
        /// \param sc               the status code of the reply
        /// \return                 a stock reply set base the status code
        static Response from_stock(http::status_code sc);

        /// Get a reply for the OPTIONS request.
        /// \param sc               the status code of the reply
        /// \param method           allowed methods
        /// \return                 a stock reply set base the status code
        static Response options(http::status_code sc, const char *method);

        /// Get a stock reply for the given ErrorT.
        /// \param sc               the status code of the reply
        /// \param et               the type of the error
        /// \param msg              the message sent
        /// \param origin           the url of the or-igin
        /// \return                 a stock reply set base on the error's type and the status code
        template<typename ErrorType>
        static Response from_stock(http::status_code sc, typename ErrorType::Type et, const std::string &msg, const std::string &origin) {
            return ErrorType::from_stock(sc, et, msg, origin);
        }
};


#endif  /* _HTTP_RESPONSE_H_ */
