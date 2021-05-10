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
#ifndef _CORE_DEVREGISTRY_ENDPOINTS_DEVICE_H_
#define _CORE_DEVREGISTRY_ENDPOINTS_DEVICE_H_


#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/crate/Uri.h"

#include "core/helpers/ErrorResponse.h"

#include "../queries/Device.h"
#include "../responses/Device.h"


namespace Endpoint {

    /// This endpont handles the devices.
    ///
    /// Corresponds to URI: ANY /mgmt/devices
    template<typename DB>
    class Device {

        private:

            DB &db;  ///< The connection to the database.

        public:

            /// Creates the /mgmt/device/{deviceId} endpoint.
            /// \param db           the database conenction to use
            Device(DB &db) : db{ db } {}

            /// Get the device by the given id.
            /// \param req          the request
            Response handle(Request &&req) {
                unsigned long id = 0;
                if (req.uri.pathId(id))
                    return getDeviceById(id);
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/device");
            }

        private:

            /// Get the device by the given id.
            /// \param id           the id of the device
            Response getDeviceById(unsigned long id) {
                auto row = Queries::Device(db).getDevice(id);
                if (row) {
                    using R = Responses::Device;
                    auto cr = R{ };
                    cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::Address, R::MacAddress, R::AuthenticationInfo>(row);
                    return Response{ cr.str() };
                }
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/device");
            }

    };  // class Device

}  // namespace Endpoint

#endif  /* _CORE_DEVREGISTRY_ENDPOINTS_DEVICE_H_ */
