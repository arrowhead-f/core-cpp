/********************************************************************************
 * Copyright (c) 2021 BME
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
#ifndef _CORE_DEVREGISTRY_ENDPOINTS_DEVICENAME_H_
#define _CORE_DEVREGISTRY_ENDPOINTS_DEVICENAME_H_

#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/crate/Uri.h"

#include "core/helpers/ErrorResponse.h"

#include "../queries/DeviceRegistry.h"
#include "../responses/DeviceRegistry.h"


namespace Endpoint {

    /// This endpont handles the devices.
    ///
    /// Corresponds to URI: ANY /mgmt/devicename
    template<typename DB>
    class DeviceName {

        private:

            DB &db;  ///< The connection to the database.

        public:

            /// Creates the /mgmt/devicename/ endpoint.
            /// \param db           the database conenction to use
            DeviceName(DB &db) : db{ db } {}

            /// Returns all entries.
            /// \param req          the request to handle
            /// \return             the generated response
            Response handle(Request &&req) {
                std::string name;
                if (req.uri.pathParam(name)) {
                    // parse parameters
                    const auto pp = CommonPParser{ req.uri, { "id", "createdAt", "updatedAt" } };
                    if (!pp)
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/devicename");
                    if (pp.isInvalid())
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Only both or none of page and size may be defined.", "/mgmt/devicename");
                    return getDeviceByName(std::move(req), name);
                }
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/devicename");
            }

        private:

            /// Get the device by the given id.
            /// \param req          the request
            /// \param name         the name of the device
            Response getDeviceByName(Request &&req, const std::string &name) {
                auto row = Queries::DeviceRegistry(db).getDevice(name);
                //if (row) {
                //    using R = DeviceResponse;
                //    auto cr = R{ };
                //    cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::Address, R::MacAddress, R::AuthenticationInfo>(row);
                //    return Response{ cr.str() };
                //}
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/devicename");
            }

    };  // class DeviceName

}  // namespace Endpoint

#endif  /* _CORE_DEVREGISTRY_ENDPOINTS_DEVICENAME_H_ */
