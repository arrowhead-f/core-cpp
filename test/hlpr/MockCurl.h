#ifndef _TEST_MOCKCURL_H_
#define _TEST_MOCKCURL_H_

#include <iostream>

#include <initializer_list>
#include <functional>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "http/WebGet.h"
#include "http/KeyProvider.h"


class MockCurl : public WebGet {

    public:

        struct RespItem {

            bool except       = false;
            int code          = 0;
            std::string msg;

            RespItem(int code, const std::string &resp) : code{ code }, msg{ resp } {}
            RespItem(const std::string &e) : except{ true }, msg{ e } {}
        };

    private:

        KeyProvider& getKP() {
            static KeyProvider kp;
            return kp;
        }

        std::map<std::string, std::pair<std::size_t, std::vector<RespItem>>> resp;


        /// The request class to make subsequent curl call from the same thread convenient and fast.
        class Req : public WebGet::Req {
            private:
                MockCurl &mc;
            public:
                Req(MockCurl &mc) : mc{ mc } {}
                result send(const char *method, const std::string &url, long port, const std::string &payload) final {
                    return mc.send(method, url, port, payload);
                }
                using WebGet::Req::send;
            };


    public:


        MockCurl() : WebGet{ getKP() } {}

        MockCurl(std::initializer_list<std::pair<std::string, RespItem>> r) : WebGet{ getKP() } {

            for(const auto x : r) {
                auto &y = resp[x.first];
                y.first = 0;
                y.second.push_back(std::move(x.second));
            }

        }

        ~MockCurl() = default;

        WebGet::result send(const char *method, const std::string &url, long port, const std::string &payload) final {
            using namespace http;

            const std::string uri = std::string{method} + ":" + url;

            auto it = resp.find(uri);
            if (it == resp.end()) {
                it = resp.find(url);
            }

            if (it != resp.end()) {
                auto &cursor = it->second.first;
                auto &&data  = it->second.second;

                if (!data.empty()) {

                    if (cursor >= data.size())
                        cursor = 0;

                    if (data[cursor].except)
                        throw WebGet::Error{ data[cursor++].msg };

                    cursor++;
                    return make_result(data[cursor-1].code, data[cursor-1].msg);
                }
            }

            // try find an appropriate result with pattern matching
            for(auto x : resp) {

                const std::regex base_regex(x.first);
                std::smatch base_match;

                if (std::regex_match(uri, base_match, base_regex)) {

                    auto &cursor = x.second.first;
                    auto &&data  = x.second.second;

                    if (!data.empty()) {

                        if (cursor >= data.size())
                            cursor = 0;

                        if (data[cursor].except)
                            throw WebGet::Error{ data[cursor++].msg };

                        cursor++;
                        return make_result(data[cursor-1].code, data[cursor-1].msg);
                    }
                }
            }

            throw std::runtime_error{ "General error." };
        }

        using WebGet::send;

        std::unique_ptr<WebGet::Req> req() final { return std::make_unique<MockCurl::Req>(*this); }
};

#endif  /* _TEST_MOCKCURL_H_ */
