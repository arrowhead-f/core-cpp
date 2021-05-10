#ifndef _CORE_DR_DEVICE_H_
#define _CORE_DR_DEVICE_H_


#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "../responses/DeviceResponse.h"
#include "../abstracts/DeviceQueries.h"
#include "core/CoreUtils.h"


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
        /// \param id           the id of the device
        Response handle(Request &&req) {
            unsigned long id = 0;
            try {
                std::size_t idx;
                id = std::stoul(req.uri.substr(13), &idx);
                if (13 + idx != req.uri.length())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/device");
            }
            catch(...) {
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/device");
            }
            return getDevice(std::move(req), id);
        }

    private:

        /// Get the device by the given id.
        /// \param req          the request
        /// \param id           the id of the device
        Response getDevice(Request &&req, unsigned long id) {
            auto row = DeviceQuery(db).getDevice(id);
            if (row) {
                using R = DeviceResponse;
                auto cr = R{ };
                cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::Address, R::MacAddress, R::AuthenticationInfo>(row);
                return Response{ cr.str() };
            }
            return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/device");
        }

};

#endif  /* _CORE_DR_DEVICE_H_ */
