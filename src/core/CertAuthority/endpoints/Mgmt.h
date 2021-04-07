#ifndef _CORE_CA_MGMT_H_
#define _CORE_CA_MGMT_H_


#include "http/crate/Request.h"
#include "http/crate/Response.h"
#include "http/UrlParser.h"

#include "core/CoreUtils.h"
#include "core/CertAuthority/responses/CertificatesResponse.h"
#include "core/CertAuthority/CAQueries.h"
#include "core/CertAuthority/CAUtilities.h"

#include "utils/json.h"


template<typename DB>
class Mgmt {

    private:

        static constexpr std::size_t preflen = 6;  ///< The length of the path prefix, i.e., the length of "/mgmt/".
        DB &db;                                    ///< The database to use.

        /// Function to handle /mgmt/certificates url.
        /// \param req          the request to handle
        /// \return             the generated response
        Response _urlCertificates(Request &&req) {

            using R = CertificatesResponse;  // shortener for the Respone type

            // get the number of stored entries
            const auto count = CAQuery(db).getCertCount();

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
                                 ||  sink.try_consume_order("sort_field", sort_field, {"id", "commonName", "serial", "createdAt", "createdBy", "validFrom", "validUntil" });

                if (!res || sink.failed()) {
                    error = true;
                    break;
                }

                ++p;
            }

            if(!p.check() || error) {
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Parameter error.", "/mgmt/certificates");
            }

            if((page.empty() && !item_per_page.empty()) || (!page.empty() && item_per_page.empty())) {
                return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Only both or none of page and size may be defined.", "/mgmt/certificates");
            }

            // and now the entries
            auto row = CAQuery(db).getCertificates(page, item_per_page, sort_field, direction);

            auto cr = R{ };
            cr << R::Count{ count };
            const auto res = cr.for_each("issuedCertificates", row, [](auto &builder, const auto &row) {

                builder.template write_dictionary_items<decltype(row), R::ID, R::CommonName, R::SerialNumber, R::CreatedAt, R::CreatedBy>(row);

                std::string valid_after;
                row->get(5, valid_after);

                std::string valid_before;
                row->get(6, valid_before);

                std::string revoked_at;
                bool revoked = row->get(7, revoked_at);

                const auto status = CAUtilities::getCertStatus(valid_after, valid_before, revoked);

                builder << R::ValidFrom{ valid_after } << R::ValidUntil{ valid_before } << R::RevokedAt{ revoked_at, !revoked } << R::Status{ status };

            }).str();

            return Response{ res };
        }

    public:

        /// Ctor.
        /// \param db           the database conenction to use
        Mgmt(DB &db) : db{ db } {}

        /// Handle requests.
        /// \param req          the request to handle
        /// \return             the generated response
        Response handle(Request &&req) {

            if (!req.uri.compare(0, 18, "/mgmt/certificates") && (req.uri[18] == 0 || req.uri[18] == '&')) {
                return CoreUtils::call_if("GET", std::move(req), [this](Request &&r) {
                    try {
                        if (!r.content.empty()) {
                            return ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::BAD_PAYLOAD, "Invalid payload for request.", "/mgmt/certificates");
                        }
                        return this->_urlCertificates(std::move(r));
                    }
                    catch(const db::Exception &e) {
                        return ErrorResponse::from_stock(http::status_code::InternalServerError, ErrorResponse::ARROWHEAD, "Database error.", "/mgmt/certificates");
                    }
                    catch(...) {
                        return ErrorResponse::from_stock(http::status_code::InternalServerError, ErrorResponse::GENERIC, "Unknow error.", "/mgmt/certificates");
                    }
                });
            }

            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_CA_MGMT_H_ */
