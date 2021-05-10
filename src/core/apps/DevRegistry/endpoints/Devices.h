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
#ifndef _CORE_DEVREGISTRY_ENDPOINTS_DEVICES_H_
#define _CORE_DEVREGISTRY_ENDPOINTS_DEVICES_H_


#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/crate/Uri.h"

#include "core/helpers/ErrorResponse.h"
#include "core/helpers/ModelBuilder.h"
#include "core/helpers/CommonPParser.h"

#include "../models/Device.h"
#include "../parsers/Device.h"
#include "../responses/Device.h"
#include "../queries/Device.h"


namespace Endpoint {

    /// This endpont handles the devices.
    ///
    /// Corresponds to URI: ANY /mgmt/devices
    template<typename DB>
    class Devices {

        private:

            DB &db;  ///< The connection to the database.

            // Error messages:
            static constexpr const char *ENDPOINT      = "/mgmt/devices";
            static constexpr const char *EMPTY_REQUEST = "Request is empty.";
            static constexpr const char *NOT_FOUND     = "Device not found.";
            static constexpr const char *PARAM_ERROR   = "Parameter error.";
            static constexpr const char *CANNOT_CREATE = "Cannot create device.";
            static constexpr const char *PAGE_AND_SIZE = "Only both or none of page and size may be defined.";

        public:

            /// Creates the /mgmt/devices/{deviceId} endpoint.
            /// \param db           the database conenction to use
            Devices(DB &db) : db{ db } {}


            /// Returns all entries.
            /// \param req          the request to handle
            /// \return             the generated response
            Response handleGet(Request &&req) {

                // get the number of stored entries
                const auto count = Queries::Device(db).getDeviceCount();

                // parse parameters
                const auto pp = CommonPParser{ req.uri, { "id", "createdAt", "updatedAt" } };
                if (!pp)
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PARAM_ERROR, ENDPOINT);

                if (pp.isInvalid())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PAGE_AND_SIZE, ENDPOINT);

                // and now the entries
                auto row = Queries::Device(db).getDevices(pp.getPage(), pp.getItemPerPage(), pp.getSortField(), pp.getDirection());

                using R = Responses::Device;  // shortener for the Respone type
                auto cr = R{ };
                cr << R::Count{ count };
                const auto res = cr.for_each("data", row, [](auto &builder, const auto &row) {
                    builder.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::Address, R::MacAddress, R::AuthenticationInfo>(row);
                }).str();

                return Response{ res };
            }


            /// Returns the entry with the given id.
            /// \param req          the request to handle
            /// \param id           the id of the device
            /// \return             the generated response
            Response handleGet(Request &&req, long id) {
                return getDeviceById(id);
            }


            Response handlePost(Request &&req) {
                if (req.content.empty())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

                const auto builder = ModelBuilder<Models::Device>{ req.content, Parsers::Device{} };
                if (builder) {
                    auto &&model = builder.model();
                    if (const auto *err = model.validate())
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

                    long id = 0;
                    if (const auto res = Queries::Device(db).postDevice(id, model))
                        return getDeviceById(id);

                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
                }

                return Response::from_stock(http::status_code::BadRequest);
            }


            Response handlePatch(Request &&req, long id) {
                if (req.content.empty())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

                const auto builder = ModelBuilder<Models::Device>{ req.content, Parsers::Device{} };
                if (builder) {
                    auto &&model = builder.model();
                    if (const auto *err = model.validate())
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

                    if (const auto res = Queries::Device(db).patchDevice(id, model))
                        return getDeviceById(id);

                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
                }

                return Response::from_stock(http::status_code::BadRequest);
            }


            Response handlePut(Request &&req, long id) {
                if (req.content.empty())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

                const auto builder = ModelBuilder<Models::Device>{ req.content, Parsers::Device{} };
                if (builder) {
                    auto &&model = builder.model();
                    if (const auto *err = model.validate())
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

                    if (const auto res = Queries::Device(db).putDevice(id, model))
                        return getDeviceById(id);

                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
                }

                return Response::from_stock(http::status_code::BadRequest);
            }


            Response handleDelete(Request &&req, long id) {
                Queries::Device(db).delDevice(id);
                return Response::from_stock(http::status_code::OK);
            }

        private:

            /// Get the device by the given id.
            /// \param id           the id of the device
            Response getDeviceById(unsigned long id) {
                if (auto row = Queries::Device(db).getDevice(id)) {
                    using R = Responses::Device;
                    auto cr = R{ };
                    cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::Address, R::MacAddress, R::AuthenticationInfo>(row);
                    return Response{ cr.str() };
                }
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PARAM_ERROR, ENDPOINT);
            }

    };  // class Devices

}  // namespace Endpoint

#endif  /* _CORE_DEVREGISTRY_ENDPOINTS_DEVICES_H_ */
