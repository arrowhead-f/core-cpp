#include "net/ReqBuilder.h"

#include <curl/curl.h>

ReqBuilder::ReqBuilder(const KeyProvider &keyProvider) : keyProvider{ keyProvider } {
    curl_global_init(CURL_GLOBAL_ALL);
}

ReqBuilder::~ReqBuilder() {
    curl_global_cleanup();
}

template<>std::pair<int, std::string> ReqBuilder::send<ReqBuilder::GET>(const std::string &url, const std::string &payload) {
    return send(url, "GET", payload);
}

template<>std::pair<int, std::string> ReqBuilder::send<ReqBuilder::POST>(const std::string &url, const std::string &payload) {
    return send(url, "POST", payload);
}

template<>std::pair<int, std::string> ReqBuilder::send<ReqBuilder::DELETE>(const std::string &url, const std::string &payload) {
    return send(url, "DELETE", payload);
}

template<>std::pair<int, std::string> ReqBuilder::send<ReqBuilder::PUT>(const std::string &url, const std::string &payload) {
    return send(url, "PUT", payload);
}

template<>std::pair<int, std::string> ReqBuilder::send<ReqBuilder::PATCH>(const std::string &url, const std::string &payload) {
    return send(url, "PATCH", payload);
}


namespace {

    struct ResponseData {
        std::string data;
    };

    void httpResponseHandler(char *ptr, size_t size, size_t nmemb, void *storage) {
        auto *rd = static_cast<ResponseData*>(storage);
        rd->data += ptr; // is ptr nullterminated
    }

}

std::pair<int, std::string> ReqBuilder::send(const std::string &url, const char *method, const std::string &payload){

    if(auto *curl = curl_easy_init()) {

        // set the agent
        const std::string agent = std::string{ "libcurl/" } + curl_version_info(CURLVERSION_NOW)->version;
        curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str());

        // set the headers
        curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Expect:");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // set method
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);

        if(keyProvider) {
            //---------------HTTPS SECTION--------------------------------------------------------
            //--verbose
            //if ( curl_easy_setopt(curl, CURLOPT_VERBOSE,        1L)            != CURLE_OK)
            //   printf("error: CURLOPT_VERBOSE\n");
            //--insecure
            if ( curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L) != CURLE_OK)
                 printf("error: CURLOPT_SSL_VERIFYPEER\n");
            if ( curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L) != CURLE_OK)
                 printf("error: CURLOPT_SSL_VERIFYHOST\n");
            //--cert
            if ( curl_easy_setopt(curl, CURLOPT_SSLCERT, keyProvider.sslCert.c_str()) != CURLE_OK)
                 printf("error: CURLOPT_SSLCERT\n");
            //--cert-type
            if ( curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, keyProvider.sslCertType.c_str()) != CURLE_OK)
                 printf("error: CURLOPT_SSLCERTTYPE\n");
            //--key
            if ( curl_easy_setopt(curl, CURLOPT_SSLKEY, keyProvider.sslKey.c_str()) != CURLE_OK)
                 printf("error: CURLOPT_SSLKEY\n");
            //--key-type
            if ( curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, keyProvider.sslKeyType.c_str()) != CURLE_OK)
                 printf("error: CURLOPT_SSLKEYTYPE\n");
            //--pass
            if ( curl_easy_setopt(curl, CURLOPT_KEYPASSWD, keyProvider.keyPasswd.c_str()) != CURLE_OK)
                 printf("error: CURLOPT_KEYPASSWD\n");
            //--cacert
            if ( curl_easy_setopt(curl, CURLOPT_CAINFO, keyProvider.caInfo.c_str()) != CURLE_OK)
                 printf("error: CURLOPT_CAINFO\n");
            //
            //---------------END OF HTTPS SECTION-------------------------------------------------
        }

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);

        ResponseData rd;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &httpResponseHandler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rd);

        // set the url
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Perform the request, res will get the return code
        auto res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            printf("Error: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        int http_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return std::make_pair(http_code, rd.data);
    }

    return { 0, ""};
}
