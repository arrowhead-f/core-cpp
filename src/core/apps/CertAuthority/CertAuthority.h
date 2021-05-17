#ifndef _CORE_CERTAUTHORITY_H_
#define _CORE_CERTAUTHORITY_H_

#include "core/Core.h"
#include "core/helpers/InvokeIf.h"

#include <cstring>

#include "gason/gason.h"

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

                gason::JsonAllocator allocator;
                gason::JsonValue     root;
                auto status = gason::jsonParse(req.content.data(), root, allocator);

                if (status != gason::JSON_PARSE_OK) {
                    return Response::from_stock(http::status_code::BadRequest);
                }

                auto data = root.child("publicKey").toString();
                if (!data) {
                    return Response::from_stock(http::status_code::BadRequest);
                }

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

        Response handle(Request &&req) final {
            if (req.uri.compare("/echo"))
                return invokeIf("GET", std::move(req), [this](Request &&r){ return this->_urlEcho(std::move(r)); });

            if (req.uri.consume("/mgmt")) {
                auto db = Parent::database();
                return Mgmt<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.handle(std::move(req));
            }

            if (req.uri.compare("/checkTrustedKey"))
                return invokeIf("POST", std::move(req), [this](Request &&r){ return this->_urlCheckTrustedKey(std::move(r)); });

            if (req.uri.compare("/checkCertificate"))
                return invokeIf("POST", std::move(req), [this](Request &&r){ return this->_urlCheckCertificate(std::move(r)); });

            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_CERTAUTHORITY_H_ */
