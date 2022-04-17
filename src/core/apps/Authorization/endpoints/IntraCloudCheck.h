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
#ifndef CORE_AUTHORIZER_ENDPOINTS_INTRACLOUDCHECK_H_
#define CORE_AUTHORIZER_ENDPOINTS_INTRACLOUDCHECK_H_


#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/crate/Uri.h"

#include "core/helpers/ErrorResponse.h"
#include "core/helpers/CoreQueryStr.h"

#include "../queries/IntraCloudCheck.h"
#include "../requests/models/IntraCloudCheck.h"
#include "../requests/parsers/IntraCloudCheck.h"
#include "../responses/IntraCloudCheck.h"


namespace Endpoint {

    /// This endpont handles all intracloud requests.
    /// Corresponds to URI: ANY /intracloud/check
    template<typename DB>
    class IntraCloudCheck {

        private:

            DB &db;  ///< The connection to the database.

        public:

            /// Creates the endpoint for handling /intracloud/check.
            /// \param db           the database conenction to use
            IntraCloudCheck(DB &db) : db{ db } {}

            /// Get the device by the given id.
            /// \param req          the request
            Response handle(Request &&req) {
                if (req.method == "POST") {
                    try {
                        const auto model = Parsers::IntraCloudCheck::parse(req.content.c_str());
                        if (!model.validate()) {
                            return checkRule(model);
                        }
                        return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/intracloud/check");
                    }
                    catch(const std::exception &e) {
                        return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/intracloud/check");
                    }
                }
                #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
                else if (req.method == "OPTION") {
                    return Response::options(http::status_code::OK, "POST");
                }
                #endif
                return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::UNAVAILABLE, "Parameter error.", "/intracloud/check");
            }

        private:

            /// Delete the intra-cloud athorization records by id.
            /// \param id           The id of the requested record.
            Response checkRule(const Models::IntraCloudCheck &model) {
                try {
                    auto row = Queries::GetIntraCloudCheck(db).checkRule(model);

                    if (row) {
                        using R = Responses::IntraCloudCheck;
                        auto cr = R{ };

                        cr << R::ServiceDefinitionId{ model.serviceDefinitionId };
                        cr.template write_dictionary_items<decltype(row), R::ID, R::Address, R::Port, R::SystemName, R::AuthenticationInfo, R::CreatedAt, R::UpdatedAt>("consumer", row);


                        cr.for_each("authorizedProviderIdsWithInterfaceIds", row, [](JsonBuilder &builder, auto &r) {
                            long id;
                            r->get(7, id);

                            builder << R::ID{ id };

                            return builder.take_while_raw("idList", r, [id](auto &rr){ long sid; rr->get(7, sid); return id == sid; }, [](auto &out, auto &rr){
                                long iid;
                                rr->get(8, iid);
                                out << iid << ',';
                            });
                        });

                        return Response{ cr.str() };
                    }
                    return Response{ "{}" };
                }
                catch(const std::exception &e) {

                    return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");
                }
            }


    };  // class IntraCloudCheck

}  // namespace Endpoint

#endif  /* CORE_AUTHORIZER_ENDPOINTS_INTRACLOUDCHECK_H_ */
