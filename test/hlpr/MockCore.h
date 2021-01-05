#ifndef _TEST_MOCKCORE_H_
#define _TEST_MOCKCORE_H_

#include <iostream>

#include <initializer_list>
#include <map>
#include <string>
#include <tuple>
#include <utility>

#include "http/StatusCode.h"
#include "http/crate/Request.h"
#include "http/crate/Response.h"


class MockCore {

    private:

        std::map<std::string, std::pair<http::status_code, std::string>> factory_resp;  ///< The factory responses.

    public:

        MockCore(std::initializer_list<std::tuple<std::string, http::status_code, std::string>> r) {
            for(const auto x : r) {
                factory_resp.emplace(std::get<0>(x), std::make_pair(std::get<1>(x), std::get<2>(x)));
            }
        }

        Response dispatch(Request &&req) {

            if (req.method == "GET" || req.method == "DELETE" || req.method == "POST" || req.method == "PUT" || req.method == "PATCH") {

                const auto it = factory_resp.find(req.uri);
                if (it != factory_resp.cend()) {
                    return Response{ it->second.first, it->second.second };
                }



                return Response::from_stock(http::status_code::NotImplemented);
            }

            return Response::from_stock(http::status_code::MethodNotAllowed);
        }

        void error(const std::string &from, const char *method, const std::string &uri, const std::string &reason) {
           /// need a member variable to keed track the errors
        }

};

#endif  /* _TEST_MOCKCORE_H_ */