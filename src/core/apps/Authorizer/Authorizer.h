#ifndef _CORE_APPS_AUTHORIZER_H_
#define _CORE_APPS_AUTHORIZER_H_


#include "core/Core.h"


template<typename DBPool, typename RB>class Authorizer final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;

    public:

        using Core<DBPool, RB>::Core;

        Response handleGET(Request &&req) final {
            return Response::from_stock(http::status_code::NotImplemented);
        }

        Response handlePOST(Request &&req) final {
            return Response::from_stock(http::status_code::NotImplemented);
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

#endif  /* _CORE_APPS_AUTHORIZER_H_ */
