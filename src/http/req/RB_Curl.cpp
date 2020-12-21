#include "RB_Curl.h"


#include <iostream>

#include <cstring>
#include <stdexcept>


namespace {


    size_t curlWriteFunction(char *ptr, size_t size, size_t nmemb, void *storage) {
        auto *rd = static_cast<http::RB_Curl::RespData*>(storage);
        rd->data += ptr;

        return size * nmemb;
    }

}


http::RB_Curl::RB_Curl(const http::KeyProvider &keyProvider) : ReqBuilder{ keyProvider }, curl{ nullptr }, hdrs{ nullptr } {

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();

    // init curl
    if (!curl)
        throw std::runtime_error{ "Cannot initialize LibCurl." };

    // set the agent
    const auto agent = std::string{ "libcurl/" } + curl_version_info(CURLVERSION_NOW)->version;
    if (const auto rc = curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str()))
        throw std::runtime_error{ curl_easy_strerror(rc) };

    // set the headers
    hdrs = curl_slist_append(hdrs, "Expect:");
    hdrs = curl_slist_append(hdrs, "Content-Type: application/json");

    if (!hdrs) {
        curl_easy_cleanup(curl);
        throw std::runtime_error{ "Cannot set request headers." };
    }

    if (const auto rc = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs))
        throw std::runtime_error{ curl_easy_strerror(rc) };

    if (keyProvider) {
        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L))
            throw std::runtime_error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L))
            throw std::runtime_error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSLCERT, keyProvider.sslCert.c_str()))
            throw std::runtime_error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, keyProvider.sslCertType.c_str()))
            throw std::runtime_error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSLKEY, keyProvider.sslKey.c_str()))
            throw std::runtime_error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, keyProvider.sslKeyType.c_str()))
            throw std::runtime_error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_KEYPASSWD, keyProvider.keyPasswd.c_str()))
            throw std::runtime_error{ curl_easy_strerror(rc) };

        if (const auto rc = curl_easy_setopt(curl, CURLOPT_CAINFO, keyProvider.caInfo.c_str()))
            throw std::runtime_error{ curl_easy_strerror(rc) };
    }

}


http::RB_Curl::~RB_Curl() {

    // clean local environment
    curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);

    // shut down curl
    curl_global_cleanup();
}


http::ReqBuilder::result http::RB_Curl::send(const char *method, const std::string &url, long port, const std::string &payload) {

    auto *handle = curl_easy_duphandle(curl);

    if (!handle)
        throw std::runtime_error{ "Cannot create handle" };

    {
        auto _ = CurlGuard{ handle };
        return RB_Curl::send(handle, method, url, port, payload);
    }
}


http::RB_Curl::Request::Request(CURL *curl) : handle{ curl_easy_duphandle(curl) } {
    if (!handle)
        throw std::runtime_error{ "Cannot create handle" };
}


http::RB_Curl::Request::~Request() {
    curl_easy_cleanup(handle);
}


http::ReqBuilder::result http::RB_Curl::send(CURL *handle, const char *method, const std::string &url, long port, const std::string &payload) {

    // set method
    if (const auto rc = curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, method))
        throw std::runtime_error{ curl_easy_strerror(rc) };

    if (!std::strcmp(method, "POST") || !std::strcmp(method, "PATCH") || !std::strcmp(method, "PUT")  ) {
        if (const auto rc = curl_easy_setopt(handle, CURLOPT_POSTFIELDS, payload.c_str()))
            throw std::runtime_error{ curl_easy_strerror(rc) };
        if (const auto rc = curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, -1L))
            throw std::runtime_error{ curl_easy_strerror(rc) };
    }

    RespData rd;

    if (const auto rc = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &curlWriteFunction))
        throw std::runtime_error{ curl_easy_strerror(rc) };
    if (const auto rc = curl_easy_setopt(handle, CURLOPT_WRITEDATA, &rd))
        throw std::runtime_error{ curl_easy_strerror(rc) };

    // set the url
    if (const auto rc = curl_easy_setopt(handle, CURLOPT_URL, url.c_str()))
        throw std::runtime_error{ curl_easy_strerror(rc) };
    if (port > 0) {
        if (const auto rc = curl_easy_setopt(handle, CURLOPT_PORT, port))
            throw std::runtime_error{ curl_easy_strerror(rc) };
    }


    //std::cerr << "PERfvvv\n";
    // Perform the request, res will get the return code
    if (const auto rc = curl_easy_perform(handle)) {
        //std::cout << rc << ": " << curl_easy_strerror(rc) << "\n";
        return { result_code{ -rc }, "" };
    }
//        throw std::runtime_error{ curl_easy_strerror(rc) };


    long http_code;
    if (const auto rc = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_code))
        throw std::runtime_error{ curl_easy_strerror(rc) };
        // return { result_code{ -rc }, "" };

    if (!http_code) {
        return { result_code{ -1 }, std::move(rd.data) };
    }

//        char *ct;
//        if (const auto rc = curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &http_code))
//            return make_result(-rc, curl_easy_strerror(rc));

    // get payload
    return { result_code{ http_code }, std::move(rd.data) };

}


http::ReqBuilder::result http::RB_Curl::Request::send(const char *method, const std::string &url, long port, const std::string &payload) {
    return RB_Curl::send(handle, method, url, port, payload);
}

std::unique_ptr<http::ReqBuilder::Request> http::RB_Curl::req() {
    return std::make_unique<http::RB_Curl::Request>(curl);
}
