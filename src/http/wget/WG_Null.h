#ifndef _HTTP_WGNULL_H_
#define _HTTP_WGNULL_H_


#include <string>

#include "http/KeyProvider.h"
#include "http/WebGet.h"


/// A placeholder class to make as-if successfull webget call would have been made.
class WG_Null final : WebGet {

    private:

        /// The request class to make subsequent calls from the same thread convenient and fast.
        struct Req final : public WebGet::Req {
            Req() = default;
            ~Req() = default;
            result send(const char *method, const std::string &url, long port, const std::string &payload) final { return { 200, "" }; }
            using WebGet::Req::send;
        };

    public:

        WG_Null(const KeyProvider &keyProvider) : WebGet{ keyProvider } {}
        ~WG_Null() = default;

        /// Send a request to the given url with the given method and payload.
        /// \param method     the request method to be used
        /// \param url        the url of the request
        /// \param por        the remote end's port to use
        /// \param payload    the payload of the reqquest
        /// \return           the pair that consists of the http code and returned data
        result send(const char *method, const std::string &url, long port, const std::string &payload) final { return { 200, "" }; }

        /// To make inherited methods available.
        using WebGet::send;

        /// Creates a new request,
        /// \return           the new request object
        std::unique_ptr<WebGet::Req> req() final { return std::make_unique<WG_Null::Req>(); }

};

#endif  /* _HTTP_WGNULL_H_ */
