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
#ifndef _CORE_DEVREGSTRY_ENDPOINTS_UNREGISTER_H_
#define _CORE_DEVREGSTRY_ENDPOINTS_UNREGISTER_H_


#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/crate/Uri.h"


namespace Endpoint {

    /// This endpont is removes a registered device. A client is allowed to unregister
    /// only its own device. It means that device name and certificate common name must
    /// match for successful unregistration.
    ///
    /// Corresponds to URI: DELETE /unregister
    /// Query params:
    ///   * device_name - name of provider
    ///   * mac_address - mac-address of the device
    template<typename DB>
    class UnRegister {

        private:

            DB &db;  ///< The connection to the database.

        public:

            /// Creates the UnRegister endpoint.
            /// \param db           the database conenction to use
            UnRegister(DB &db) : db{ db } {}

            /// Handle requests.
            /// \param req          the request to handle
            /// \return             the generated response
            Response handle(Request &&req) {
                try {
                    if (!req.content.empty()) {
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Invalid payload for request.", "/unregister");
                    }
                    return unregister(std::move(req));
                }
                catch(const db::Exception &e) {
                    return ErrorResponse::from_stock(http::status_code::InternalServerError, ErrorResponse::ARROWHEAD, "Database operation exception.", "/unregister");
                }
                catch(...) {
                    return ErrorResponse::from_stock(http::status_code::InternalServerError, ErrorResponse::GENERIC, "Unknow error.", "/unregister");
                }
            }

        private:

            Response unregister(Request &&req) {

                // get params
                std::string device_name;
                std::string mac_address;

                bool error = false;
                auto p = Uri::Parser{ req.uri };

                while(p) {

                    auto &&kv = *p;
                    auto &&sink = Uri::Parser::Sink{ kv };

                    const auto res =     sink.try_consume("device_name", device_name)
                                     ||  sink.try_consume("mac_address", mac_address);

                    if (!res || sink.failed()) {
                        error = true;
                        break;
                    }

                    ++p;
                }

                // error checking
                if (!p.check() || error) {
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/unregister");
                }

                if (device_name.empty() || mac_address.empty()) {
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Only both or none of page and size may be defined.", "/unregister");
                }

                if (!req.hasCert() || device_name != req.getCert().common_name()) {
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::AUTH, "Cannot unregister someone else's device.", "/unregister");
                }

                long device_id;

                {
                    std::string q = "SELECT id FROM device WHERE device_name = " + db.escape(device_name) +  " AND mac_address = " + db.escape(mac_address);
                    (debug{ } << fmt("DRQuery: {}") << q).log(SOURCE_LOCATION);

                    if (!db.fetch(q.c_str(), device_id)) {
                        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, "Device entry with name '" + device_name + "' and MAC address '" + mac_address + "' does not exist", "/unregister");
                    }
                }

                {
                    std::string q = "DELETE FROM device_registry WHERE device_id = " + std::to_string(device_id);
                    (debug{ } << fmt("DRQuery: {}") << q).log(SOURCE_LOCATION);

                    db.query(q.c_str());
                }

                return Response::from_stock(http::status_code::OK);
            }

    };  // class UnRegister

}  // namespace Endpoint

#endif  /* _CORE_DEVREGSTRY_ENDPOINTS_UNREGISTER_H_ */
