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
//#include "../responses/Device.h"


namespace Endpoint {

    /// This endpont handles all intracloud requests.
    /// Corresponds to URI: ANY /mgmt/intracloud
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
                return Response::from_stock<ErrorResponse>(http::status_code::BadRequest, ErrorResponse::UNAVAILABLE, "Parameter error.", "/token");
            }

        private:


    };  // class Token

}  // namespace Endpoint

#endif  /* CORE_AUTHORIZER_ENDPOINTS_TOKEN_H_ */
