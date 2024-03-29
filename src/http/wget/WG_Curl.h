#ifndef _HTTP_WGCURL_H_
#define _HTTP_WGCURL_H_


#include <string>

#include <curl/curl.h>

#include "http/WebGet.h"
#include "http/KeyProvider.h"


class WG_Curl final : WebGet {

    static_assert(CURLE_OK == 0, "Curl's CURLE_OK should have value 0.");

    private:

        /// Helper class to destroy the CURL handle right aftere exiting the scope.
        class CurlGuard {
            private:

                CURL *curl;    ///< The protected handle.

            public:

                CurlGuard(CURL *curl) : curl{ curl } { }
                ~CurlGuard() { curl_easy_cleanup(curl); }
        };


        /// The request class to make subsequent curl call from the same thread convenient and fast.
        class Req final : public WebGet::Req {
            private:

                CURL *handle;  ///< The curl handle for this specific request.

            public:

                Req(CURL *curl);
                ~Req();
                result send(const char *method, const std::string &url, long port, const std::string &payload) final;
                using WebGet::Req::send;
        };


    public:


        /// Helper class to store response data.
        struct RespData {
            std::string data;
        };


    private:

        CURL       *curl;  ///< Our master curl.
        curl_slist *hdrs;  ///< The headers used during transmission.

    public:

        WG_Curl(const KeyProvider &keyProvider);

        ~WG_Curl();

        /// Send a request to the given url with the given method and payload.
        /// \param method       the request method to be used
        /// \param url          the url of the request
        /// \param por          the remote end's port to use
        /// \param payload      the payload of the reqquest
        /// \return             the pair that consists of the http code and returned data
        result send(const char *method, const std::string &url, long port, const std::string &payload) final;

        /// To make inherited methods available.
        using WebGet::send;

        /// Creates a new request,
        /// \return             the new request object
        std::unique_ptr<WebGet::Req> req() final;

    private:

        /// Send a request to the given url with the given method and payload.
        /// \param handle       the curl object
        /// \param method       the request method to be used
        /// \param url          the url of the request
        /// \param por          the remote end's port to use
        /// \param payload      the payload of the reqquest
        /// \return             the pair that consists of the http code and returned data
        static result send(CURL *hanle, const char *method, const std::string &url, long port, const std::string &payload);


};


#endif  /* _HTTP_WGCURL_H_ */
