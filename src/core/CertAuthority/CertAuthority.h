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

        int _urlEcho(const std::string &uri, std::string &response) {
            return 0;
        }

        int _urlCheckCertificate(const std::string &uri, std::string &response, const std::string &payload) {



            return 1;
        }

        int _urlSign(const std::string &uri, std::string &response, const std::string &payload) {
            return 1;
        }

        int _urlCheckTrustedKey(const std::string &uri, std::string &response, const std::string &payload) {

            try {

                // { "publicKey": "string" }
                std::string err;
                const auto data = json11::Json::parse(payload, err)["publicKey"].string_value();

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

                return 0;
            }
            catch(const std::exception &e) {
                //std::cerr << e.what() << "\n";
                return 1;
            }
        }

    public:

        using Core<DBPool, RB>::Core;

        int handleGET(const std::string &uri, std::string &response) final {
            switch(uri[1]) {
                case 'e':
                    if(uri == "/echo")
                        return _urlEcho(uri, response);
                case 'm':
                    if(!strncmp(uri.c_str(), "/mgmt/", 6)) {
                    //    Mgmt<typename DBPool::DatabaseType>{ database() }.dispatch(subpath(uri), response, "");
                    }
            }
            return 1;
        }

        int handlePOST(const std::string &uri, std::string &response, const std::string &payload) final {
            if(uri == "/sign")
                return _urlSign(uri, response, payload);
            if(uri == "/checkCertificate")
                return _urlSign(uri, response, payload);
            if(uri == "/checkTrustedKey")
                return _urlCheckTrustedKey(uri, response, payload);

            return 1;
        }

        int handlePUT(const std::string &uri, std::string &response, const std::string &payload) final {
            if(uri == "//mgmt/keys")
                return 0;
            return 1;
        }

        int handlePATCH(const std::string &uri, std::string &response, const std::string &payload) final {
            if(uri == "//mgmt/keys")
                return 0;
            return 1;
        }

        int handleDELETE(const std::string &uri, std::string &response, const std::string &payload) final {
            if(uri == "//mgmt/keys")
                return 0;
            return 1;
        }

};

#endif  /* _CORE_CERTAUTHORITY_H_ */
