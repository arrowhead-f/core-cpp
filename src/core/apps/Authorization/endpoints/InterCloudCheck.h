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
#ifndef CORE_AUTHORIZER_ENDPOINTS_INTERCLOUDCHECK_H_
#define CORE_AUTHORIZER_ENDPOINTS_INTERCLOUDCHECK_H_


#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/crate/Uri.h"

#include "core/helpers/ErrorResponse.h"
#include "core/helpers/CoreQueryStr.h"

#include "../queries/InterCloudCheck.h"
#include "../requests/models/InterCloudCheck.h"
#include "../requests/parsers/InterCloudCheck.h"
#include "../responses/InterCloudCheck.h"


namespace Endpoint {

    /// This endpont handles all intercloud requests.
    /// Corresponds to URI: ANY /intercloud/check
    template<typename DB>
    class InterCloudCheck {

        private:

            DB &db;  ///< The connection to the database.

        public:

            /// Creates the endpoint for handling /intracloud/check.
            /// \param db           the database conenction to use
            InterCloudCheck(DB &db) : db{ db } {}

            /// Handle the request.
            /// \param req          the request
            Response handle(Request &&req) {
                try {
                    const auto model = Parsers::InterCloudCheck::parse(req.content.c_str());
                    if (!model.validate()) {
                        return checkRule(model);
                    }
                    return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/intercloud/check");
                }
                catch(const std::exception &e) {
                    return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/intercloud/check");
                }
                return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::UNAVAILABLE, "Parameter error.", "/intercloud/check");
            }

        private:

            /// Checinter cloud athorization records.
            Response checkRule(const Models::InterCloudCheck &model) {
                try {
                    auto row = Queries::GetInterCloudCheck(db).checkRule(model);

                    if (row) {

                        using R = Responses::InterCloudCheck;
                        auto cr = R{ };

                        cr << R::ServiceDefinitionId{ model.serviceDefinitionId };
                        cr.template write_dictionary_items<decltype(row), R::ID, R::Name, R::Operator, R::Secure, R::Neighbor, R::OwnCloud, R::AuthenticationInfo, R::CreatedAt, R::UpdatedAt>("cloud", row);

                        cr.for_each("authorizedProviderIdsWithInterfaceIds", row, [](JsonBuilder &builder, auto &r) {
                            long id;
                            r->get(9, id);

                            builder << R::ID{ id };

                            return builder.take_while_raw("idList", r, [id](auto &rr){ long sid; rr->get(9, sid); return id == sid; }, [](auto &out, auto &rr){
                                long iid;
                                rr->get(10, iid);
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


    };  // class InterCloudCheck

}  // namespace Endpoint

#endif  /* CORE_AUTHORIZER_ENDPOINTS_INTERCLOUDCHECK_H_ */
