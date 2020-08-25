#ifndef _ARROWHEAD_REQBUILDER_H_
#define _ARROWHEAD_REQBUILDER_H_

#include <string>
#include <utility>

#include "net/KeyProvider.h"

class ReqBuilder {

    private:

        const KeyProvider &keyProvider;   ///< The key provider

    public:

        enum ReqMethod {
            GET    = 0,
            POST   = 1,
            DELETE = 2,
            PUT    = 3,
            PATCH  = 4
        };

        ReqBuilder(const KeyProvider&);
        ~ReqBuilder();

        /// Send a request to the given url with the given payload. The method is defined as the template parameter.
        /// \param url        the url od the request
        /// \param payload    the payload of the reqquest
        /// \return           the pair that consists of the http code and returned data
        template<ReqMethod M>std::pair<int, std::string> send(const std::string &url, const std::string &payload);

        /// Send a request to the given url with the given method and payload.
        /// \param url        the url od the request
        /// \param method     the request method to be used
        /// \param payload    the payload of the reqquest
        /// \return           the pair that consists of the http code and returned data
        std::pair<int, std::string> send(const std::string &url, const char *method, const std::string &payload);

        /// Send a request to the given url with the given method and payload.
        /// \param url        the url od the request
        /// \param method     the request method to be used
        /// \param payload    the payload of the reqquest
        /// \return           the pair that consists of the http code and returned data
        std::pair<int, std::string> send(const std::string &url, const std::string &method, const std::string &payload) {
            return send(url, method.c_str(), payload);
        }

};

#endif  /*_ARROWHEAD_REQBUILDER_H_*/
