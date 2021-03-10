#ifndef _CORE_SERVICEREGISTRY_H_
#define _CORE_SERVICEREGISTRY_H_


#include "core/Core.h"

#include "endpoints/Register.h"

template<typename DBPool, typename RB>class ServiceRegistry final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;

    private:


    public:

        using Core<DBPool, RB>::Core;

        Response handleGET(Request &&req) final {
            if (!req.uri.compare("/echo")){ return Response{"Got it!"}; }

            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePOST(Request &&req) final {
            if (!req.uri.compare("/register")) {
                printf("POST message received, url: /register\n");
                auto db = Parent::database();
                return Register<db::DatabaseConnection<typename DBPool::DatabaseType>>{ db }.processRegister(std::move(req));
            }

            return Response::from_stock(http::status_code::NotFound);
        }

        Response handlePATCH(Request &&req) final {
            return Response::from_stock(http::status_code::NotImplemented);
        }

        Response handlePUT(Request &&req) final {
            return Response::from_stock(http::status_code::NotImplemented);
        }

        Response handleDELETE(Request &&req) final {
            return Response::from_stock(http::status_code::NotImplemented);
        }
};

#endif  /* _CORE_SERVICEREGISTRY_H_ */
