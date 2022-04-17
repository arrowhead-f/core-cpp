/********************************************************************************
 * Copyright (c) 2022 BME
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
#ifndef CORE_AUTHORIZER_ENDPOINTS_INTRACLOUD_H_
#define CORE_AUTHORIZER_ENDPOINTS_INTRACLOUD_H_


#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/crate/Uri.h"

#include "core/helpers/ErrorResponse.h"
#include "core/helpers/CoreQueryStr.h"

#include "../queries/IntraCloud.h"
#include "../requests/models/IntraCloud.h"
#include "../requests/parsers/IntraCloud.h"
#include "../responses/IntraCloud.h"


namespace Endpoint {

    /// This endpont handles all intracloud requests.
    /// Corresponds to URI: ANY /mgmt/intracloud
    template<typename DB>
    class IntraCloud {

        private:

            DB &db;  ///< The connection to the database.

        public:

            /// Creates the endpoints for all /mgmt/intracloud requests.
            /// \param db           the database conenction to use
            IntraCloud(DB &db) : db{ db } {}

            /// Get the device by the given id.
            /// \param req          the request
            Response handle(Request &&req) {

                if (req.method == "GET") {

                    // we have parameters with ?
                    if (req.uri.empty()) {
                        const auto qs = CoreQueryStr{ req.uri, { "id", "createdAt", "updatedAt" } };
                        if (!qs)
                            return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");
                        if (qs.isInvalid())
                            return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");

                        return getRecordsByParams(qs);
                    }

                    // we have an id at the end
                    unsigned long id = 0;
                    if (req.uri.pathId(id)) {
                        return getRecordById(id);
                    }

                }

                else if (req.method == "POST") {
                    try {
                        const auto model = Parsers::IntraCloud::parse(req.content.c_str());
                        if (!model.validate()) {
                            return postRecord(model);
                        }

                        return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");
                    }
                    catch(const std::exception &e) {
                        return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");
                    }
                }

                else if (req.method == "DELETE") {
                    // we have an id at the end
                    unsigned long id = 0;
                    if (req.uri.pathId(id)) {
                        return delRecordById(id);
                    }
                }

                #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
                else if (req.method == "OPTION") {
                    // list all valid urls
                    return Response::options(http::status_code::OK, "GET");
                }
                #endif

                return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::UNAVAILABLE, "Parameter error.", "/mgmt/intracloud");
            }

        private:

            /// Get the intra-cloud athorization records by id.
            /// \param id           The id of the requested record.
            Response getRecordById(unsigned long id) {
                auto row = Queries::GetIntraCloud(db).getRecords(id);
                if (row) {
                    using R = Responses::IntraCloud;
                    auto cr = R{ };

                    // add base data
                    cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt>(row);

                    // add definitions
                    cr.template write_dictionary_items<decltype(row), R::ID, R::SystemName, R::Address, R::Port, R::AuthenticationInfo, R::CreatedAt, R::UpdatedAt>("consumerSystem", row, 3);
                    cr.template write_dictionary_items<decltype(row), R::ID, R::SystemName, R::Address, R::Port, R::AuthenticationInfo, R::CreatedAt, R::UpdatedAt>("providerSystem", row, 10);
                    cr.template write_dictionary_items<decltype(row), R::ID, R::ServiceDefinition, R::CreatedAt, R::UpdatedAt>("serviceDefinition", row, 17);

                    // add interfaces
                    cr.for_each("interfaces", row, [](JsonBuilder &builder, auto &r){
                        builder.template write_dictionary_items<decltype(r), R::ID, R::InterfaceName, R::CreatedAt, R::UpdatedAt>(r, 21);
                    });

                    return Response{ cr.str() };
                }

                //return Response{ "{}" };
                return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");
            }

            /// Get the intra-cloud athorization records given by the ids.
            /// \param id           The vector of ids of the requested record.
            Response getRecordsByIds(const std::vector<unsigned long> &ids) {

                if (ids.empty())
                    return Response{ "{}" };

                auto row = Queries::GetIntraCloud(db).getRecords(ids);
                if (row) {
                    using R = Responses::IntraCloud;
                    auto cr = R{ };

                    cr << R::Count{ static_cast<long>(ids.size()) };
                    cr.for_each("data", row, [](JsonBuilder &builder, auto &r) {
                        builder.template write_dictionary_items<decltype(r), R::ID, R::CreatedAt, R::UpdatedAt>(r);
                        builder.template write_dictionary_items<decltype(r), R::ID, R::SystemName, R::Address, R::Port, R::AuthenticationInfo, R::CreatedAt, R::UpdatedAt>("consumerSystem", r, 3);
                        builder.template write_dictionary_items<decltype(r), R::ID, R::SystemName, R::Address, R::Port, R::AuthenticationInfo, R::CreatedAt, R::UpdatedAt>("providerSystem", r, 10);
                        builder.template write_dictionary_items<decltype(r), R::ID, R::ServiceDefinition, R::CreatedAt, R::UpdatedAt>("serviceDefinition", r, 17);

                        long id;
                        r->get(0, id);
                        return builder.take_while("interfaces", r, [id](auto &rr){ long sid; rr->get(0, sid); return id == sid; }, [](JsonBuilder &jb, auto &rr){
                            jb.template write_dictionary_items<decltype(r), R::ID, R::InterfaceName, R::CreatedAt, R::UpdatedAt>(rr, 21);
                        });
                    });

                    return Response{ cr.str() };
                }

                //return Response{ "{}" };
                return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");
            }

            /// Get the intra-cloud athorization records by params.
            /// \param params           The query parameters.
            Response getRecordsByParams(const CoreQueryStr &qs) {

                auto count = Queries::GetIntraCloud(db).getRecords();
                auto row   = Queries::GetIntraCloud(db).getRecords(qs);

                if (row) {
                    using R = Responses::IntraCloud;
                    auto cr = R{ };

                    cr << R::Count{ count };
                    cr.for_each("data", row, [](JsonBuilder &builder, auto &r) {
                        builder.template write_dictionary_items<decltype(r), R::ID, R::CreatedAt, R::UpdatedAt>(r);
                        builder.template write_dictionary_items<decltype(r), R::ID, R::SystemName, R::Address, R::Port, R::AuthenticationInfo, R::CreatedAt, R::UpdatedAt>("consumerSystem", r, 3);
                        builder.template write_dictionary_items<decltype(r), R::ID, R::SystemName, R::Address, R::Port, R::AuthenticationInfo, R::CreatedAt, R::UpdatedAt>("providerSystem", r, 10);
                        builder.template write_dictionary_items<decltype(r), R::ID, R::ServiceDefinition, R::CreatedAt, R::UpdatedAt>("serviceDefinition", r, 17);

                        long id;
                        r->get(0, id);
                        return builder.take_while("interfaces", r, [id](auto &rr){ long sid; rr->get(0, sid); return id == sid; }, [](JsonBuilder &jb, auto &rr){
                            jb.template write_dictionary_items<decltype(r), R::ID, R::InterfaceName, R::CreatedAt, R::UpdatedAt>(rr, 21);
                        });
                    });

                    return Response{ cr.str() };
                }
                return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");
            }

            /// Delete the intra-cloud athorization records by id.
            /// \param id           The id of the requested record.
            Response delRecordById(unsigned long id) {
                //if(Queries::GetIntraCloud(db).delRecord(id))
                //    return Response::from_stock(http::status_code::OK);
                //return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");

                Queries::GetIntraCloud(db).delRecord(id);
                return Response::from_stock(http::status_code::OK);
            }

            /// Delete the intra-cloud athorization records by id.
            /// \param id           The id of the requested record.
            Response postRecord(const Models::IntraCloud &model) {
                try {
                    auto ids = Queries::GetIntraCloud(db).insRecord(model);
                    return getRecordsByIds(ids);
                }
                catch(...) {
                    return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");
                }
            }


    };  // class IntraCloud

}  // namespace Endpoint

#endif  /* CORE_AUTHORIZER_ENDPOINTS_INTRACLOUD_H_ */
