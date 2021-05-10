#ifndef _CORE_DR_REGISTER_H_
#define _CORE_DR_REGISTER_H_

#include <iostream>
#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/UrlParser.h"

#include "../responses/DevRegResponse.h"

#include "gason/gason.h"

/// This endpoint registers a device. A client is allowed to register 
/// only its own device. It means that device name and certificate common
/// name must match for successful registration.
///
/// Correspond to URI: POST /query
/// Input JSON structure:
///{
///     "device": {
///         "address": "string",
///         "authenticationInfo": "string",
///         "deviceName": "string",
///         "macAddress": "string"
///     },
///     "endOfValidity": "string",
///     "metadata": {
///         "additionalProp1": "string",
///         "additionalProp2": "string",
///         "additionalProp3": "string"
///     },
///     "version": 0
/// }
template<typename DB>
class Register {

    private:

        DB &db;  ///< The connection to the database.

    public:

        /// Creates the UnRegister endpoint.
        /// \param db           the database conenction to use
        Register(DB &db) : db{ db } {}

        /// Handle requests.
        /// \param req          the request to handle
        /// \return             the generated response
        Response handle(Request &&req) {
            try {
                if (req.content.empty()) {
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Invalid payload for request.", "/register");
                }
                return process(std::move(req));
            }
            catch(const db::Exception &e) {
                return ErrorResponse::from_stock(http::status_code::InternalServerError, ErrorResponse::ARROWHEAD, "Database operation exception.", "/register");
            }
            catch(...) {
                return ErrorResponse::from_stock(http::status_code::InternalServerError, ErrorResponse::GENERIC, "Unknow error.", "/register");
            }
        }

    private:

        std::string emptyToNull(const char *data) {
            if (!data)
                return "NULL";
            else
                return db.escape(data);
        }

        long findOrCreateDevice(const gason::JsonValue &device) {

            std::string device_name;
            if (auto node = device.child("deviceName")) {
                if (node.isString())
                    device_name = node.toString();
            }

            if (device_name.empty())
                throw std::runtime_error{ "x"};

            std::string mac_address;
            if (auto node = device.child("macAddress")) {
                if (node.isString())
                    mac_address = node.toString();
            }

            if (mac_address.empty())
                throw std::runtime_error{ "x"};

            char *address = nullptr, *auth_info = nullptr;
            if (auto node = device.child("address")) {
                if (node.isString())
                    address = node.toString();
                else
                    throw std::runtime_error{ "x"};
            }
            if (auto node = device.child("authenticationInfo")) {
                if (node.isString())
                    auth_info = node.toString();
                else
                    throw std::runtime_error{ "x"};
            }

            std::string q = "SELECT id FROM device WHERE device_name = " + db.escape(device_name) + " AND mac_address = " + db.escape(mac_address);
            (debug{ } << fmt("DRQuery: {}") << q).log(SOURCE_LOCATION);

            long id = 0;
            if (!db.fetch(q.c_str(), id)) {
                std::string r = "INSERT INTO device (device_name, mac_address, address, authentication_info) VALES (" + db.escape(device_name) + ", " + db.escape(mac_address) + ", " + emptyToNull(address) + ", " + emptyToNull(auth_info) + ")";
                (debug{ } << fmt("DRQuery: {}") << r).log(SOURCE_LOCATION);
                db.insert(r.c_str(), id);
            }
            else {
                std::string r = "UPDATE device SET address = "+ emptyToNull(address) + ", authentication_info = " + emptyToNull(auth_info);
                (debug{ } << fmt("DRQuery: {}") << r).log(SOURCE_LOCATION);
                db.query(r.c_str());
            }

            return id;
        }

        Response process(Request &&req) {

            gason::JsonAllocator   allocator;
            gason::JsonValue       root;
            gason::JsonParseStatus status = gason::jsonParse(req.content.data(), root, allocator);

            if (status != gason::JSON_PARSE_OK) {
                return Response::from_stock(http::status_code::BadRequest);
            }

            auto device = root.child("device");
            if (!device || !device.isObject()) {
                return Response::from_stock(http::status_code::BadRequest);
            }

            const auto device_id = findOrCreateDevice(device);

            std::string metadata;
            auto meta = root.child("metadata");
            if (meta && !meta.isObject()) {
                // error
            }

            for (gason::JsonIterator it = gason::begin(meta); it != gason::end(meta); ++it) {
                if (it->value.isString()) {
                    metadata = metadata + ", " + it->key + "=" + it->value.toString();
                }
                else {
                    return Response::from_stock(http::status_code::BadRequest);
                }
            }
            if (!metadata.empty()) {
                metadata.pop_back();
                metadata.pop_back();
            }

            int version = 1;
            if (auto v = root.child("version")) {
                if (v.isNumber())
                    version = v.toInt();
            }

            std::string endOfValidity;
            if (auto v = root.child("endOfValidity")) {
                if (v.isString())
                    endOfValidity = v.toString();
                // check format: "yyyy-MM-dd HH:mm:ss"
                // BadPayloadException("End of validity is specified in the wrong format. Please provide UTC time using " + Utilities.getDatetimePattern() + " pattern.", HttpStatus.SC_BAD_REQUEST,
            }

            long id = 0;
            if (endOfValidity.empty()) {
                const std::string q = "INSERT INTO device_registry (device_id, metadata, version) VALUES (" + std::to_string(device_id) + ", " + db.escape(metadata) + ", " + std::to_string(version) + ")";
                (debug{ } << fmt("DRQuery: {}") << q).log(SOURCE_LOCATION);
                db.insert(q.c_str(), id);
            }
            else {
                const std::string q = "INSERT INTO device_registry (device_id, metadata, version, end_of_validity) VALUES (" + std::to_string(device_id) + ", " + db.escape(metadata) + ", " + std::to_string(version) + ", '" + endOfValidity + "')";
                (debug{ } << fmt("DRQuery: {}") << q).log(SOURCE_LOCATION);
                db.insert(q.c_str(), id);
            }

            const std::string q = "SELECT t2.id, t2.version, t2.created_at, t2.updated_at, t2.end_of_validity, t1.id, t1.created_at, t2.updated_at, t1.device_name, t1.mac_address, t1.address, t1.authentication_info FROM device t1 INNER JOIN device_registry t2 ON (t1.id = t2.device_id) WHERE t1.id = " + std::to_string(device_id) + " AND t2.id = " + std::to_string(id);
            auto row = db.fetch(q.c_str());

            if (row) {
                using R = DevRegResponse;

                auto cr = R{ };
                cr.template write_dictionary_items<decltype(row), R::ID, R::Version, R::CreatedAt, R::UpdatedAt, R::EndOfValidity>(row);
                cr.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::MacAddress, R::Address, R::AuthenticationInfo>("device", row, 5);
                if (!metadata.empty()) {
                    //builder.add_dict("metadata", meta);
                }

                return Response{ cr.str() };
            }

            return Response::from_stock(http::status_code::BadRequest);
        }

};

#endif  /* _CORE_DR_REGISTER_H_ */
