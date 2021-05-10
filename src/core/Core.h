/********************************************************************************
 * Copyright (c) 2020 BME
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   * Budapest University of Technology and Economics - implementation
 *     * ng201
 ********************************************************************************/
#ifndef _CORE_CORE_H_
#define _CORE_CORE_H_


#include <string>

#include "utils/logger.h"

#include "db/DB.h"

#include "CoreUtils.h"
#include "Dispatcher.h"


template<typename DBPool, typename RB>class Core : public Dispatcher {

    private:

        DBPool &dbPool;      ///< The database pool to be used.
        RB     &reqBuilder;  ///< Used to build and send requests.

    protected:

        /// Returns a locked database connection from the pool.
        auto database() {
            return db::DatabaseConnection<typename DBPool::DatabaseType>{ dbPool };
        };

    public:

        /// Ctor.
        Core(DBPool &dbPool, RB &reqBuilder) : dbPool{ dbPool }, reqBuilder{ reqBuilder } { }

        /// Dispatches the requests.
        Response dispatch(Request &&req) noexcept final {
            (info{ } << fmt("{}: {} {}") << req.remote_address << req.method << req.uri.str()).log(SOURCE_LOCATION);
            return handle(std::move(req));
        }

        /// Request handler method. This default implementation calls the corresponding handleMETHOD member function.
        /// \param req              The request to handle.
        /// \return                 The produced response.
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

    protected:

        /// Calls the EP's handle method with the given req, if the request's method is the given one.
        /// Otherwise returns method not allowed.
        /// \param method           Desired method for the endpoint.
        /// \param req              The request to handle.
        /// \return                 The response.
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


        /// Calls the EP's given member function with the given req, if the request's method is the given one.
        /// Otherwise returns method not allowed.
        /// \param method           Desired method for the endpoint.
        /// \param func             Member function to call.
        /// \param req              The request to handle.
        /// \return                 The response.
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

        /// Handles all calls to a CRUD capable endpoint.
        /// \param req              The request to handle.
        /// \return                 The response.
        template<template<typename> typename EP>
        auto crudify(Request &&req) {

            #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
              if (req.method == "OPTIONS")
                  return Response::options(http::status_code::OK, "GET, POST, PUT, PATCH, DELETE");
            #endif

            if (req.uri.empty()) {
                if (req.method == "POST") {
                    auto db = database();
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.handlePost(std::move(req));
                }
                if (req.method == "GET") {
                    auto db = database();
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.handleGet(std::move(req));
                }
                return Response::from_stock(http::status_code::MethodNotAllowed);
            }

            unsigned long id;
            if (req.uri.pathId(id)) {
                auto db = database();
                if (req.method == "GET")
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.handleGet(std::move(req), id);
                if (req.method == "PUT")
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.handlePut(std::move(req), id);
                if (req.method == "PATCH")
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.handlePatch(std::move(req), id);
                if (req.method == "DELETE")
                    return EP<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.handleDelete(std::move(req), id);
            }

            return Response::from_stock(http::status_code::MethodNotAllowed);
        }

};

#endif  /* _CORE_CORE_H_ */
