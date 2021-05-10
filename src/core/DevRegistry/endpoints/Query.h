#ifndef _CORE_DR_QUERY_H_
#define _CORE_DR_QUERY_H_


#include <cstring>
#include <map>
#include <iostream>
#include <stdexcept>
#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/UrlParser.h"

#include "../responses/DevRegResponse.h"

#include "gason/gason.h"
#include "utils/json.h"


/// This endpont returns Device Registry data that fits the input specification.
///
/// Corresponds to URI: POST /query
/// Input JSON structure:
/// {
///     "addressRequirement": "string",
///     "deviceNameRequirements": "string",
///     "macAddressRequirement": "string",
///     "maxVersionRequirement": 0,
///     "minVersionRequirement": 0,
///     "versionRequirement": 0,
///     "metadataRequirements": {
///         "additionalProp1": "string",
///         "additionalProp2": "string",
///         "additionalProp3": "string"
///     }
/// }
template<typename DB>
class Query {

    private:

        DB &db;  ///< The connection to the database.

    public:

        /// Creates the Query endpoint.
        /// \param db           the database conenction to use
        Query(DB &db) : db{ db } {}

        /// Handle requests.
        /// \param req          the request to handle
        /// \return             the generated response
        Response handle(Request &&req) {
            try {
                if (req.content.empty()) {
                    return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Invalid payload for request.", "/query");
                }
                return query(std::move(req));
            }
            catch(const db::Exception &e) {
                return ErrorResponse::from_stock(http::status_code::InternalServerError, ErrorResponse::ARROWHEAD, "Database operation exception.", "/query");
            }
            catch(...) {
                return ErrorResponse::from_stock(http::status_code::InternalServerError, ErrorResponse::GENERIC, "Unknow error.", "/query");
            }
        }

    private:

        long getCount(const char *device_name, gason::JsonValue &root) const {
            std::string q = "SELECT COUNT(*) FROM device t1 INNER JOIN device_registry t2 ON (t1.id = t2.device_id) WHERE t1.device_name = " + db.escape(device_name);

            if (auto mac_address = root.child("macAddressRequirement")) {
                if (!mac_address.isString()) {
                    throw std::runtime_error{ "Mac address error" };
                }
                q += " AND t1.mac_address = " + db.escape(mac_address.toString());
            }

            (debug{ } << fmt("DRQuery: {}") << q).log(SOURCE_LOCATION);

            long count = 0;
            db.fetch(q.c_str(), count);

            return count;
        }

        auto buildQuery(const char *device_name, gason::JsonValue &root) const {

            std::string q = "SELECT t2.id, t2.version, t2.created_at, t2.updated_at, t2.end_of_validity, t2.metadata, t1.id, t1.created_at, t2.updated_at, t1.device_name, t1.mac_address, t1.address, t1.authentication_info FROM device t1 INNER JOIN device_registry t2 ON (t1.id = t2.device_id) WHERE t1.device_name = " + db.escape(device_name);

            if (auto mac_address = root.child("macAddressRequirement")) {
                if (!mac_address.isString()) {
                    throw std::runtime_error{ "Mac address error" };
                }
                q += " AND t1.mac_address = " + db.escape(mac_address.toString());
            }

            if (auto a = root.child("addressRequirement")) {
                if (!a.isString()) {
                    //return {};
                }
                q += " AND t1.address = " + db.escape(a.toString());
            }

            if (auto v = root.child("versionRequirement")) {
                if (!v.isNumber()) {
                    throw std::runtime_error{ "Mac address error" };
                }
                q += " AND t1.version = " + std::to_string(v.toInt());
            }
            else {
                if (auto v = root.child("maxVersionRequirement")) {
                    if (!v.isNumber()) {
                        throw std::runtime_error{ "Mac address error" };
                    }
                    q += " AND t1.version <= " + std::to_string(v.toInt());
                }
                if (auto v = root.child("minVersionRequirement")) {
                    if (!v.isNumber()) {
                        throw std::runtime_error{ "Mac address error" };
                    }
                    q += " AND t1.version >= " + std::to_string(v.toInt());
                }
            }

            (debug{ } << fmt("DRQuery: {}") << q).log(SOURCE_LOCATION);
            return db.fetch(q.c_str());
        }

        Response query(Request &&req) {

            gason::JsonAllocator   allocator;
            gason::JsonValue       root;
            gason::JsonParseStatus status = gason::jsonParse(req.content.data(), root, allocator);

            if (status != gason::JSON_PARSE_OK) {
                return Response::from_stock(http::status_code::BadRequest);
            }

            auto device_name = root.child("deviceNameRequirement");
            if (!device_name || !device_name.isString()) {
                return Response::from_stock(http::status_code::BadRequest);
            }

            std::map<std::string, std::string> metamap;
            if (auto meta = root.child("metadata")) {
                for (gason::JsonIterator it = gason::begin(meta); it != gason::end(meta); ++it) {
                    if (it->value.isString()) {
                        if (!metamap.try_emplace(it->key, it->value.toString()).second) {
                            return Response::from_stock(http::status_code::BadRequest);
                        }
                    }
                    else {
                        return Response::from_stock(http::status_code::BadRequest);
                    }
                }
            }

            try {

                const auto count = getCount(device_name.toString(), root);
                auto row = buildQuery(device_name.toString(), root);

                using R = DevRegResponse;
                auto cr = R{ };
                cr << R::UnfilteredHits{ count };
                const auto res = cr.for_each("deviceQueryData", row, [&metamap](auto &builder, const auto &row) {

                    std::string metadata;
                    row->get(5, metadata);

                    auto okey = true;
                    if (!metamap.empty()) {
                    }

                    if (okey) {
                        builder.template write_dictionary_items<decltype(row), R::ID, R::Version, R::CreatedAt, R::UpdatedAt, R::EndOfValidity>(row);
                        builder.template write_dictionary_items<decltype(row), R::ID, R::CreatedAt, R::UpdatedAt, R::DeviceName, R::MacAddress, R::Address, R::AuthenticationInfo>("device", row, 6);
                        if (!metadata.empty()) {
                            //builder.add_dict("metadata", meta);
                        }
                    }

                    return okey;
                }).str();

                return Response{ res };
            }
            catch(const std::runtime_error&) {
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Invalid payload for request.", "/query");
            }
        }

};

#endif  /* _CORE_DR_QUERY_H_ */
