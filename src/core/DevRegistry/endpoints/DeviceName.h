#ifndef _CORE_DR_DEVICENAME_H_
#define _CORE_DR_DEVICENAME_H_


#include <iostream>
#include <string>
#include <tuple>

#include "gason/gason.h"

#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/UrlParser.h"

#include "../responses/DevRegResponse.h"
#include "../abstracts/DevRegQueries.h"
#include "core/CoreUtils.h"

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
            const auto name = req.uri.substr(17); // till the ? character, parse everithing
             if (name.find('/') != std::string::npos)
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/devicename");

            return getDeviceByName(std::move(req), name);
        }

    private:

        /// Get the device by the given id.
        /// \param req          the request
        /// \param name         the name of the device
        Response getDeviceByName(Request &&req, const std::string &name) {
//            auto row = DevRegQuery(db).getDevice(name);
//            if (row) {
//                using R = DeviceResponse;
//                auto cr = R{ };
//                cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::Address, R::MacAddress, R::AuthenticationInfo>(row);
//                return Response{ cr.str() };
//            }
            return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/devicename");
        }

};

#endif  /* _CORE_DR_DEVICENAME_H_ */
