#ifndef _CORE_DR_DEVREG_H_
#define _CORE_DR_DEVREG_H_


#include <string>

#include "gason/gason.h"

#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/UrlParser.h"

#include "../responses/DevRegResponse.h"
#include "../abstracts/Models.h"
#include "../abstracts/JsonModels.h"
#include "../abstracts/DevRegQueries.h"
#include "core/CoreUtils.h"


/// This endpont handles the devices.
///
/// Corresponds to URI: ANY /mgmt
template<typename DB>
class DevReg {

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
        DevReg(DB &db) : db{ db } {}

        /// Returns all entries.
        /// \param req          the request to handle
        /// \return             the generated response
        Response doGet(Request &&req) {

            // get the number of stored entries
            const auto count = DevRegQuery(db).getCount();

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
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", ENDPOINT);
            }

            if((page.empty() && !item_per_page.empty()) || (!page.empty() && item_per_page.empty())) {
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, PAGE_AND_SIZE, ENDPOINT);
            }

            // and now the entries
            auto row = DevRegQuery(db).getAll(page, item_per_page, sort_field, direction);
            if (row) {
                using R = DevRegResponse;

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


        Response doPost(Request &&req) {
            //if (const auto dev = JsonModels::DeviceRegistry::parse(req.content)) {
            //    if (dev.empty())
            //        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

            //    //if (const auto *err = dev.validate())
            //    //    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

            //    long id = 0;
            //    if (const auto res = DevRegQuery(db).createDeviceRegistry(id, dev))
            //        return getById(id);

            //    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
            //}
            return Response::from_stock(http::status_code::BadRequest);
        }


        Response doPatch(Request &&req, long id) {
            //if (const auto dev = JsonModels::DeviceRegistry::parse(req.content)) {
            //    if (dev.empty())
            //        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

            //    //if (const auto *err = dev.validate())
            //    //    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

            //    if (const auto res = DevRegQuery(db).patchDeviceRegistry(id, dev))
            //        return getById(id);

            //    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
            //}
            return Response::from_stock(http::status_code::BadRequest);
        }


        Response doPut(Request &&req, long id) {
            //if (const auto dev = JsonModels::DeviceRegistry::parse(req.content)) {
            //    if (dev.empty())
            //        return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, EMPTY_REQUEST, ENDPOINT);

            //    //if (const auto *err = dev.validate())
            //    //    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, err, ENDPOINT);

            //    if (const auto res = DevRegQuery(db).putDeviceRegistry(id, dev))
            //        return getById(id);

            //    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::INVALID_PARAMETER, NOT_FOUND, ENDPOINT);
            //}
            return Response::from_stock(http::status_code::BadRequest);
        }


        Response doDelete(Request &&req, long id) {
            DevRegQuery(db).remove(id);
            return Response::from_stock(http::status_code::OK);
        }

    private:

        /// Get the device registry resposne by the given id.
        /// \param req          the request
        /// \param id           the id of the device
        Response getById(unsigned long id) {
            auto row = DevRegQuery(db).getById(id);
            if (row) {
                using R = DevRegResponse;

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

};

#endif  /* _CORE_DR_DEVREG_H_ */
