#ifndef _CORE_SYSREGISTRY_H_
#define _CORE_SYSREGISTRY_H_


#include "core/Core.h"

#include "gason/gason.h"


template<typename DBPool, typename RB>class SysRegistry final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;

    private:

        Response _urlEcho(Request &&req) {
            return Response::from_stock(http::status_code::OK);
        }

    public:

        using Core<DBPool, RB>::Core;

        Response handle(Request &&req) final {
            if (!req.uri.compare("/echo"))
                return CoreUtils::call_if("GET", std::move(req), [this](Request &&r){ return this->_urlEcho(std::move(r)); });

            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_SYSREGISTRY_H_ */
