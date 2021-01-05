#include "WG_Curl.h"


#include <iostream>

#include <cstring>
#include <stdexcept>


namespace {


    size_t curlWriteFunction(char *ptr, size_t size, size_t nmemb, void *storage) {
        auto *rd = static_cast<WG_Curl::RespData*>(storage);
        rd->data += ptr;

        return size * nmemb;
    }

}


WG_Curl::WG_Curl(const KeyProvider &keyProvider) : WebGet{ keyProvider }, curl{ nullptr }, hdrs{ nullptr } {

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();

    // init curl
    if (!curl)
        throw WebGet::Error{ "Cannot initialize LibCurl." };

    // set the agent
    const auto agent = std::string{ "libcurl/" } + curl_version_info(CURLVERSION_NOW)->version;
    if (const auto rc = curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str()))
        throw WebGet::Error{ curl_easy_strerror(rc) };

    // set the headers
    hdrs = curl_slist_append(hdrs, "Expect:");
    hdrs = curl_slist_append(hdrs, "Content-Type: application/json");

    if (!hdrs) {
        curl_easy_cleanup(curl);
        throw WebGet::Error{ "Cannot set request headers." };
    }

    if (const auto rc = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs))
        throw WebGet::Error{ curl_easy_strerror(rc) };

    if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L))
        throw WebGet::Error{ curl_easy_strerror(rc) };

    if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L))
        throw WebGet::Error{ curl_easy_strerror(rc) };

    if (keyProvider) {
        //if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L))
        //    throw WebGet::Error{ curl_easy_strerror(rc) };

        //if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L))
        //    throw WebGet::Error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSLCERT, keyProvider.sslCert.c_str()))
            throw WebGet::Error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, keyProvider.sslCertType.c_str()))
            throw WebGet::Error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSLKEY, keyProvider.sslKey.c_str()))
            throw WebGet::Error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, keyProvider.sslKeyType.c_str()))
            throw WebGet::Error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_KEYPASSWD, keyProvider.keyPasswd.c_str()))
            throw WebGet::Error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_CAINFO, keyProvider.caInfo.c_str()))
            throw WebGet::Error{ curl_easy_strerror(rc) };
    }

}


WG_Curl::~WG_Curl() {

    // clean local environment
    curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);

    // shut down curl
    curl_global_cleanup();
}


WebGet::result WG_Curl::send(const char *method, const std::string &url, long port, const std::string &payload) {

    auto *handle = curl_easy_duphandle(curl);

    if (!handle)
        throw WebGet::Error{ "Cannot create handle" };

    {
        auto _ = CurlGuard{ handle };
        return WG_Curl::send(handle, method, url, port, payload);
    }
}


WG_Curl::Req::Req(CURL *curl) : handle{ curl_easy_duphandle(curl) } {
    if (!handle)
        throw WebGet::Error{ "Cannot create handle" };
}


WG_Curl::Req::~Req() {
    curl_easy_cleanup(handle);
}


WebGet::result WG_Curl::send(CURL *handle, const char *method, const std::string &url, long port, const std::string &payload) {

    // set method
    if (const auto rc = curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, method))
        throw WebGet::Error{ curl_easy_strerror(rc) };

    if (!std::strcmp(method, "POST") || !std::strcmp(method, "PATCH") || !std::strcmp(method, "PUT")  ) {
        if (const auto rc = curl_easy_setopt(handle, CURLOPT_POSTFIELDS, payload.c_str()))
            throw WebGet::Error{ curl_easy_strerror(rc) };
        if (const auto rc = curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, -1L))
            throw WebGet::Error{ curl_easy_strerror(rc) };
    }

    RespData rd;

    if (const auto rc = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &curlWriteFunction))
        throw WebGet::Error{ curl_easy_strerror(rc) };
    if (const auto rc = curl_easy_setopt(handle, CURLOPT_WRITEDATA, &rd))
        throw WebGet::Error{ curl_easy_strerror(rc) };

    // set the url
    if (const auto rc = curl_easy_setopt(handle, CURLOPT_URL, url.c_str()))
        throw WebGet::Error{ curl_easy_strerror(rc) };
    if (port > 0) {
        if (const auto rc = curl_easy_setopt(handle, CURLOPT_PORT, port))
            throw WebGet::Error{ curl_easy_strerror(rc) };
    }

    if (const auto rc = curl_easy_perform(handle)) {
        return make_result(-rc, "");
    }

    long http_code;
    if (const auto rc = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_code))
        throw WebGet::Error{ curl_easy_strerror(rc) };

    if (!http_code)
        return make_result(-5000, std::move(rd.data));

    // get payload
    return make_result(http_code, std::move(rd.data));

}


WebGet::result WG_Curl::Req::send(const char *method, const std::string &url, long port, const std::string &payload) {
    return WG_Curl::send(handle, method, url, port, payload);
}

std::unique_ptr<WebGet::Req> WG_Curl::req() {
    return std::make_unique<WG_Curl::Req>(curl);
}
