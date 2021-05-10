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
#ifndef _CORE_DEVREGISTRY_ENDPOINTS_DEVICEREGISTRY_H_
#define _CORE_DEVREGISTRY_ENDPOINTS_DEVICEREGISTRY_H_


#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/crate/Uri.h"

#include "core/CoreUtils.h"
#include "core/helpers/CommonPParser.h"
#include "core/helpers/ModelBuilder.h"

#include "../models/DeviceRegistry.h"
#include "../parsers/DeviceRegistry.h"
#include "../queries/DeviceRegistry.h"
#include "../responses/DeviceRegistry.h"


namespace Endpoint {

    /// This endpont handles the devices.
    ///
    /// Corresponds to URI: ANY /mgmt
    template<typename DB>
    class DeviceRegistry {

        private:

            DB &db;  ///< The connection to the database.

            // Error messages:
            static constexpr const char *ENDPOINT      = "/mgmt";
            static constexpr const char *EMPTY_REQUEST = "Request is empty.";
            static constexpr const char *NOT_FOUND     = "Device not found.";
            static constexpr const char *PARAM_ERROR   = "Parameter error.";
            static constexpr const char *CANNOT_CREATE = "Cannot create device.";
            static constexpr const char *PAGE_AND_SIZE = "Only both or none of page and size may be defined.";

        public:

            /// Creates the /mgmt/devices/{deviceId} endpoint.
            /// \param db           the database conenction to use
            DeviceRegistry(DB &db) : db{ db } {}

            /// Returns all entries.
            /// \param req          the request to handle
            /// \return             the generated response
            Response handleGet(Request &&req) {

                // get the number of stored entries
                const auto count = Queries::DeviceRegistry(db).getCount();

                // parse parameters
                const auto pp = CommonPParser{ req.uri, { "id", "createdAt", "updatedAt" } };
                if (!pp)
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PARAM_ERROR, ENDPOINT);

                if (pp.isInvalid())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PAGE_AND_SIZE, ENDPOINT);

                // and now the entries
                auto row = Queries::DeviceRegistry(db).getAll(pp.getPage(), pp.getItemPerPage(), pp.getSortField(), pp.getDirection());
                if (row) {
                    using R = Responses::DeviceRegistry;

                    auto cr = R{ };
                    cr << R::Count{ count };
                    cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::Version, R::EndOfValidity>(row);
                    cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::MacAddress, R::Address, R::AuthenticationInfo>("device", row, 5);
                    //if (!metadata.empty()) {
                        //builder.add_dict("metadata", meta);
                    //}
                    return Response{ cr.str() };
                }

                return Response::from_stock(http::status_code::BadRequest);
            }

            Response handleGet(Request &&req, long id) {
                return Response::from_stock(http::status_code::BadRequest);
            }

            Response handlePost(Request &&req) {
                if (req.content.empty())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

                const auto builder = ModelBuilder<Models::DeviceRegistry>{ req.content, Parsers::DeviceRegistry{} };
                if (builder) {
                    auto &&model = builder.model();
                    if (const auto *err = model.validate())
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

                    unsigned long id;
                    if (const auto res = Queries::DeviceRegistry(db).postDeviceRegistry(id, model))
                        return getById(id);

                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
                }

                return Response::from_stock(http::status_code::BadRequest);
            }


            Response handlePatch(Request &&req, long id) {
                if (req.content.empty())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

                const auto builder = ModelBuilder<Models::DeviceRegistry>{ req.content, Parsers::DeviceRegistry{} };
                if (builder) {
                    auto &&model = builder.model();
                    if (const auto *err = model.validate())
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

                    {
                        auto &&q = Queries::DeviceRegistry(db);
                        auto row = q.getById(id);
                        if (row) {
                            const auto theDevId = q.findOrCreateDevice(model.device);

                            long devId;
                            if (row->get(5, devId)) {
                                if (theDevId != devId) {
                                    if (q.isDeviceRegistered(theDevId))
                                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, "Device already registered.", ENDPOINT);
                                }
                            }

                            q.putDeviceRegistry(id, model);
                            return getById(id);
                        }
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, "Device Registry entry does not exist.", ENDPOINT);
                    }

                }

                return Response::from_stock(http::status_code::BadRequest);
            }


            Response handlePut(Request &&req, long id) {
                if (req.content.empty())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

                const auto builder = ModelBuilder<Models::DeviceRegistry>{ req.content, Parsers::DeviceRegistry{} };
                if (builder) {
                    auto &&model = builder.model();
                    if (const auto *err = model.validate())
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

                    {
                        auto &&q = Queries::DeviceRegistry(db);
                        auto row = q.getById(id);
                        if (row) {
                            const auto theDevId = q.findOrCreateDevice(model.device);

                            long devId;
                            if (row->get(5, devId)) {
                                if (theDevId != devId) {
                                    if (q.isDeviceRegistered(theDevId))
                                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, "Device already registered.", ENDPOINT);
                                }
                            }

                            q.putDeviceRegistry(id, model);
                            return getById(id);
                        }

                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, "Device Registry entry does not exist.", ENDPOINT);
                    }
                }
                return Response::from_stock(http::status_code::BadRequest);
            }


            Response handleDelete(Request &&req, long id) {
                Queries::DeviceRegistry(db).remById(id);
                return Response::from_stock(http::status_code::OK);
            }

        private:

            /// Get the device registry resposne by the given id.
            /// \param req          the request
            /// \param id           the id of the device
            Response getById(unsigned long id) {
                auto row = Queries::DeviceRegistry(db).getById(id);
                if (row) {
                    using R = Responses::DeviceRegistry;

                    auto cr = R{ };
                    cr.template write_dictionary_items<decltype(row), R::ID, R::Version, R::CreatedAt, R::UpdatedAt, R::EndOfValidity>(row);
                    cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::MacAddress, R::Address, R::AuthenticationInfo>("device", row, 5);
                    //if (!metadata.empty()) {
                        //builder.add_dict("metadata", meta);
                    //}
                    return Response{ cr.str() };
                }
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PARAM_ERROR, ENDPOINT);
            }

    };  // class DeviceRegistry

}  // namespace Endpoint

#endif  /* _CORE_DEVREGISTRY_ENDPOINTS_DEVICEREGISTRY_H_ */
