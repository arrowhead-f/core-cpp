#ifndef _CORE_CORE_H_
#define _CORE_CORE_H_


#include <iostream>

#include <cstring>
#include <string>
#include <string_view>

#include "utils/logger.h"

#include "db/DB.h"

#include "CoreUtils.h"
#include "Dispatcher.h"


template<typename DBPool, typename RB>class Core : public Dispatcher {
    private:

        DBPool &dbPool;      ///< The database pool to be used.
        RB     &reqBuilder;  ///< Used to build and send requests.

    protected:

        auto database() {
            return db::DatabaseConnection<typename DBPool::DatabaseType>{ dbPool };
        };

        template<template<typename> typename EP>auto invoke(const char *method, Request &&req) {
            if (req.method == method) {
                auto db = database();
                return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.handle(std::move(req));
            }
            #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
              if (req.method == "OPTIONS")
                  return Response::options(http::status_code::OK, method);
            #endif
            return Response::from_stock(http::status_code::MethodNotAllowed);
        }

        template<template<typename> typename EP>auto invoke(const char *method, Response(EP<db::DatabaseConnection<typename DBPool::DatabaseType>>::*func)(Request&&), Request &&req) {
            if (req.method == method) {
                auto db = database();
                return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.func(std::move(req));
            }
            #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
              if (req.method == "OPTIONS")
                  return Response::options(http::status_code::OK, method);
            #endif
            return Response::from_stock(http::status_code::MethodNotAllowed);
        }

        template<template<typename> typename EP, std::size_t N>
        auto crudify(const char(&ar)[N], Request &&req) {
            return crudify<EP>(std::move(req), N);
        }

    public:

        Core(DBPool &dbPool, RB &reqBuilder) : dbPool{ dbPool }, reqBuilder{ reqBuilder } { }

        Response dispatch(Request &&req) noexcept final {
            (info{ } << fmt("{}: {} {}") << req.remote_address << req.method << req.uri).log(SOURCE_LOCATION);
            return handle(std::move(req));
        }

        virtual Response handle(Request &&req) {
            if (req.method == "GET") {
                return handleGET(std::move(req));
            }
            else if (req.method == "DELETE") {
                return handleDELETE(std::move(req));
            }
            else if (req.method == "POST") {
                return handlePOST(std::move(req));
            }
            else if (req.method == "PUT") {
                return handlePUT(std::move(req));
            }
            else if (req.method == "PATCH") {
                return handlePATCH(std::move(req));
            }
            else if (req.method == "HEAD") {
                return handleHEAD(std::move(req));
            }
            else if (req.method == "OPTIONS") {
                return handleOPTIONS(std::move(req));
            }

            return Response::from_stock(http::status_code::MethodNotAllowed);
        }

        void report(const std::string &from, const std::string &reason) final {
            (::error{ } << fmt("{}: error; reason: {}") << from << reason.c_str()).log(SOURCE_LOCATION);
        }

        void report(const std::string &from, const char *reason) final {
            (::error{ } << fmt("{}: error; reason: {}") << from << reason).log(SOURCE_LOCATION);
        }

        // HTTP callbacks
        virtual Response handleGET    (Request &&req)  {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handleDELETE (Request &&req)  {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handlePUT    (Request &&req)  {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handlePATCH  (Request &&req)  {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handlePOST   (Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handleHEAD   (Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

        virtual Response handleOPTIONS(Request &&req) {
            return Response::from_stock(http::status_code::NotFound);
        }

    private:

        template<template<typename> typename EP>auto crudify(Request &&req, std::size_t M) {

            const std::size_t N = M - 1;

            #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
              if (req.method == "OPTIONS")
                  return Response::options(http::status_code::OK, "GET, POST, PUT, PATCH, DELETE");
            #endif
            if (req.uri.length() == N) {
                if (req.method == "POST") {
                    auto db = database();
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.doPost(std::move(req));
                }
                if (req.method == "GET") {
                    auto db = database();
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.doGet(std::move(req));
                }
                return Response::from_stock(http::status_code::MethodNotAllowed);
            }
            if (req.uri[N] == '?') {
                if (req.method == "GET") {
                    auto db = database();
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.doGet(std::move(req));
                }
                return Response::from_stock(http::status_code::MethodNotAllowed);
            }
            if (req.uri[N] == '/') {
                unsigned long id = 0;
                try {
                    id = std::stoul(req.uri.substr(N + 1));
                }
                catch(...) {
                    return Response::from_stock(http::status_code::MethodNotAllowed);
                }
                auto db = database();
                if (req.method == "PUT")
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.doPut(std::move(req), id);
                if (req.method == "PATCH")
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.doPatch(std::move(req), id);
                if (req.method == "DELETE")
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.doDelete(std::move(req), id);
            }


            return Response::from_stock(http::status_code::MethodNotAllowed);
        }

};

#endif  /* _CORE_CORE_H_ */
