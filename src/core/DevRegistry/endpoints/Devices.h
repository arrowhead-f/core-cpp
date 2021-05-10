#ifndef _CORE_DR_DEVICES_H_
#define _CORE_DR_DEVICES_H_


#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/UrlParser.h"

#include "../responses/DeviceResponse.h"
#include "../abstracts/JsonModels.h"
#include "../abstracts/JsonModels.h"
#include "../abstracts/DeviceQueries.h"
#include "core/CoreUtils.h"


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

        /// Get the device by the given id.
        /// \param req          the request
        /// \param id           the id of the device
        Response getDevice(Request &&req, unsigned long id) {
            if (auto row = DeviceQuery(db).getDevice(id)) {
                using R = DeviceResponse;
                auto cr = R{ };
                cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::Address, R::MacAddress, R::AuthenticationInfo>(row);
                return Response{ cr.str() };
            }
            return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PARAM_ERROR, ENDPOINT);
        }

        /// Returns all entries.
        /// \param req          the request to handle
        /// \return             the generated response
        Response doGet(Request &&req) {

            // get the number of stored entries
            const auto count = DeviceQuery(db).getDeviceCount();

            // get params
            std::string page;                   // current page
            std::string item_per_page;          // items per page
            std::string sort_field    = "id";   // sort field (default: id)
            std::string direction     = "ASC";  // sorting order (default: ASC)

            bool error = false;
            auto p = http::UrlParser{ req.uri };

            while(p) {

                auto &&kv = *p;
                auto &&sink = http::UrlParser::Sink{ kv };

                const auto res =     sink.try_consume_as<unsigned long>("page", page)
                                 ||  sink.try_consume_as<unsigned long>("item_per_page", item_per_page)
                                 ||  sink.try_consume("direction",  direction,  {"ASC", "DESC" })
                                 ||  sink.try_consume_order("sort_field", sort_field, {"id", "createdAt", "updatedAt" });

                if (!res || sink.failed()) {
                    error = true;
                    break;
                }

                ++p;
            }

            if(!p.check() || error) {
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PARAM_ERROR, ENDPOINT);
            }

            if((page.empty() && !item_per_page.empty()) || (!page.empty() && item_per_page.empty())) {
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PAGE_AND_SIZE, ENDPOINT);
            }

            // and now the entries
            auto row = DeviceQuery(db).getDevices(page, item_per_page, sort_field, direction);

            using R = DeviceResponse;  // shortener for the Respone type
            auto cr = R{ };
            cr << R::Count{ count };
            const auto res = cr.for_each("data", row, [](auto &builder, const auto &row) {
                builder.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::Address, R::MacAddress, R::AuthenticationInfo>(row);
            }).str();

            return Response{ res };
        }


        Response doPost(Request &&req) {
            if (const auto dev = JsonModels::Device::parse(req.content)) {
                if (dev.empty())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

                //if (const auto *err = dev.validate())
                //    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

                long id = 0;
                if (const auto res = DeviceQuery(db).postDevice(id, dev))
                    return getDevice(std::move(req), id);

                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
            }

            return Response::from_stock(http::status_code::BadRequest);
        }


        Response doPatch(Request &&req, long id) {
            if (const auto dev = JsonModels::Device::parse(req.content)) {
                if (dev.empty())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

                if (const auto *err = dev.validate())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

                if (const auto res = DeviceQuery(db).patchDevice(id, dev))
                    return getDevice(std::move(req), id);

                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
            }
            return Response::from_stock(http::status_code::BadRequest);
        }


        Response doPut(Request &&req, long id) {
            if (const auto dev = JsonModels::Device::parse(req.content)) {
                if (dev.empty())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

                if (const auto *err = dev.validate())
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

                if (const auto res = DeviceQuery(db).putDevice(id, dev))
                    return getDevice(std::move(req), id);

                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
            }
            return Response::from_stock(http::status_code::BadRequest);
        }


        Response doDelete(Request &&req, long id) {
            DeviceQuery(db).delDevice(id);
            return Response::from_stock(http::status_code::OK);
        }

};

#endif  /* _CORE_DR_DEVICES_H_ */
