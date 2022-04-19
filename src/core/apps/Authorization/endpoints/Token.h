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
#ifndef CORE_AUTHORIZER_ENDPOINTS_TOKEN_H_
#define CORE_AUTHORIZER_ENDPOINTS_TOKEN_H_


#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/crate/Uri.h"

#include "core/helpers/ErrorResponse.h"

//#include "../queries/Device.h"
#include "../requests/models/TokenRule.h"
#include "../requests/parsers/TokenRule.h"
#include "../responses/TokenData.h"


namespace Endpoint {

    /// This endpoint handles the token requests.
    /// Corresponds to URI: ANY /token
    template<typename DB>
    class Token {

        private:

            DB &db;  ///< The connection to the database.

        public:

            /// Creates the endpoint for the /token request.
            /// \param db           the database conenction to use
            Token(DB &db) : db{ db } {}

            /// Get the device by the given id.
            /// \param req          the request
            Response handle(Request &&req) {
                try {
                    const auto model = Parsers::TokenRule::parse(req.content.c_str());
                    if (!model.validate()) {
                        return generateTokens(model);
                    }
                    return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/token");
                }
                catch(const std::exception &e) {
                    return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/token");
                }
                return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::UNAVAILABLE, "Parameter error.", "/token");
            }

        private:

            /// Generates tokens.
            /// \param id           The id of the requested record.
            /// \return             TokenData json.
            Response generateTokens(const Models::TokenRule &model) {
                int i = 0;

                try {
                    using R = Responses::TokenData;
                    auto cr = R{ };

                    cr.for_each("tokenData", std::cbegin(model.providers), std::cend(model.providers), [&i](JsonBuilder &builder, const auto &data) {
                        builder << R::ProviderAddress{ data.first.address } << R::ProviderName{ data.first.systemName } << R::ProviderPort{ data.first.port };
                        builder.write_dictionary_items("tokens", std::cbegin(data.second), std::cend(data.second), [&i](auto &out, const auto &data) {
                            out << "\"" << data << "\": " << "\"token" << std::to_string(i++) << "\"";
                        });
                    });

                    return Response{ cr.str() };
                }
                catch(const std::exception &e) {

                    return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/intracloud");
                }
            }


    };  // class Token

}  // namespace Endpoint

#endif  /* CORE_AUTHORIZER_ENDPOINTS_TOKEN_H_ */
