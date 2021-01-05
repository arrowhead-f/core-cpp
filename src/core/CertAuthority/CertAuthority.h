#ifndef _CORE_CERTAUTHORITY_H_
#define _CORE_CERTAUTHORITY_H_

#include "core/Core.h"

#include <cstring>

#include "json11/json11.hpp"

#include "utils/logger.h"
#include "utils/crypto.h"

#include "CAQueries.h"
#include "endpoints/Mgmt.h"
#include "responses/TrustedKeyCheckResponse.h"


template<typename DBPool, typename RB>class CertAuthority final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;

    private:

        Response _urlEcho(Request &&req) {
            return Response::from_stock(http::status_code::OK);
        }

        Response _urlCheckCertificate(Request &&req) {
            return Response::from_stock(http::status_code::NotImplemented);
        }

        Response _urlSign(Request &&req) {
            return Response::from_stock(http::status_code::NotImplemented);
        }

        Response _urlCheckTrustedKey(Request &&req) {

            std::string response;
            try {

                // { "publicKey": "string" }
                std::string err;
                const auto data = json11::Json::parse(req.content, err)["publicKey"].string_value();

                auto hash = std::string{};
                crypto::sha256(data, hash);

                auto db = Parent::database();
                if(const auto row = CAQuery(db).getTrustedKey(hash)) {

                    long id;
                    std::string createdAt, description;

                    row->get(0, id);
                    row->get(1, createdAt);
                    row->get(2, description);

                    {
                        using R = TrustedKeyCheckResponse;
                        response = (R{} << R::ID{ id } << R::CreatedAt{ createdAt } << R::Description{ description }).str();
                    }
                }

                return Response{ response };
            }
            catch(...) {
                return Response::from_stock(http::status_code::InternalServerError);
            }
        }

    public:

        using Core<DBPool, RB>::Core;

        Response handleGET(Request &&req) final {
            if (!req.uri.compare("/echo")) { return _urlEcho(std::move(req)); }
            if (!req.uri.compare(0, 5, "/mgmt")) {
                auto db = Parent::database();
                return Mgmt<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.handleGET(std::move(req));
            }
            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePOST(Request &&req) final {
            if (!req.uri.compare("/checkTrustedKey")) { return _urlCheckTrustedKey(std::move(req)); }
            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePATCH(Request &&req) final {
            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePUT(Request &&req) final {
            return Response::from_stock(http::status_code::NotFound);
        }

        Response handleDELETE(Request &&req) final {
            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_CERTAUTHORITY_H_ */
