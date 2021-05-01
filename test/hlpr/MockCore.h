#ifndef _TEST_MOCKCORE_H_
#define _TEST_MOCKCORE_H_


#include <iostream>

#include <chrono>
#include <functional>
#include <initializer_list>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <utility>

#include "http/StatusCode.h"
#include "http/crate/Request.h"
#include "http/crate/Response.h"


class MockCore {

    private:

        std::map<std::string, std::pair<http::status_code, std::string>> factory_resp;  ///< The factory responses.
        std::map<std::string, std::function<Response(Request&&)>>        funtory_resp;  ///< Response function. First always the fun responses are selected.

        std::mutex mux;       ///< The mutex for the thread counter.
        std::size_t mc = 0;   ///< Number of max. parallel callers.
        std::size_t ac = 0;   ///< Current number of callers.

    public:

        MockCore(std::initializer_list<std::tuple<std::string, http::status_code, std::string>> r) {
            for(const auto x : r) {
                factory_resp.emplace(std::get<0>(x), std::make_pair(std::get<1>(x), std::get<2>(x)));
            }
        }

        template<typename Fun>
        MockCore& addFuntion(std::string key, Fun &&f) {
            funtory_resp[key] = f;
        }

        Response dispatch(Request &&req) {

            {
                std::lock_guard<std::mutex> _{ mux };
                ac++;
                if (ac > mc)
                    mc = ac;
            }

            if (req.method == "GET" || req.method == "DELETE" || req.method == "POST" || req.method == "PUT" || req.method == "PATCH") {

                if (!req.uri.compare(0, 7, "/sleep/")) {
                    auto t = std::stoi(req.uri.substr(7, std::string::npos));
                    std::this_thread::sleep_for(std::chrono::milliseconds(t));
                }

                {
                    const auto it = funtory_resp.find(req.uri);
                    if (it != funtory_resp.cend()) {


                        {
                            std::lock_guard<std::mutex> _{ mux };
                            ac--;
                        }
                        return it->second(std::move(req));
                    }
                }

                {
                    const auto it = factory_resp.find(req.uri);
                    if (it != factory_resp.cend()) {

                        {
                            std::lock_guard<std::mutex> _{ mux };
                            ac--;
                        }
                        return Response{ it->second.first, it->second.second };
                    }
                }


                {
                    std::lock_guard<std::mutex> _{ mux };
                    ac--;
                }

                return Response::from_stock(http::status_code::NotImplemented);
            }


            {
                std::lock_guard<std::mutex> _{ mux };
                ac--;
            }

            return Response::from_stock(http::status_code::MethodNotAllowed);
        }

        void report(const std::string &from, const std::string &reason) {
           /// need a member variable to keed track the errors
        }

        std::size_t getNumOfParallelCalls() {
            std::lock_guard<std::mutex> _{ mux };
            return mc;
        }

};

#endif  /* _TEST_MOCKCORE_H_ */