#ifndef CORE_APPS_AUTHORIZATION_H_
#define CORE_APPS_AUTHORIZATION_H_


#include "core/Core.h"

#include "endpoints/InterCloud.h"
#include "endpoints/IntraCloud.h"
#include "endpoints/InterCloudCheck.h"
#include "endpoints/IntraCloudCheck.h"
#include "endpoints/Token.h"


template<typename DBPool, typename RB>class Authorization final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;

        /// The endpoint for the /echo request.
        /// \param req              the request
        /// \return                 the response
        Response urlEcho(Request &&req) const noexcept {
            return Response::from_stock(http::status_code::OK);
        }

        /// The endpoint for the /publickey request.
        /// \param req              the request
        /// \return                 the response
        Response urlPublicKey(Request &&req) const noexcept {
            return Response::from_stock(http::status_code::OK);
        }

    public:

        using Core<DBPool, RB>::Core;

        Response handle(Request &&req) final {
            if (req.uri.compare("/echo"))
                return invokeIf("GET", std::move(req), [this](Request &&r){ return this->urlEcho(std::move(r)); });

            // client services
            if (req.uri.compare("/publickey"))
                return invokeIf("GET", std::move(req), [this](Request &&r){ return this->urlPublicKey(std::move(r)); });

            // management services
            if (req.uri.consume("/mgmt/intracloud"))
                return Parent::template invoke<Endpoint::IntraCloud>(std::move(req));
            if (req.uri.consume("/mgmt/intercloud"))
                return Parent::template invoke<Endpoint::InterCloud>(std::move(req));

            // private endpoints
            if (req.uri.consume("/intracloud/check"))
                return Parent::template invoke<Endpoint::IntraCloudCheck>("POST", std::move(req));
            if (req.uri.consume("/intercloud/check"))
                return Parent::template invoke<Endpoint::InterCloudCheck>("POST", std::move(req));
            if (req.uri.consume("/token"))
                return Parent::template invoke<Endpoint::Token>("POST", std::move(req));

            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* CORE_APPS_AUTHORIZATION_H_ */
