#ifndef _CORE_GATEWAY_H_
#define _CORE_GATEWAY_H_


#include "core/Core.h"
#include "core/helpers/InvokeIf.h"


template<typename DBPool, typename RB>class Gateway final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;

    private:

        Response urlEcho(Request &&req) {
            return Response::from_stock(http::status_code::OK);
        }

    public:

        using Core<DBPool, RB>::Core;

        Response handle(Request &&req) final {
            if (req.uri.compare("/echo"))
                return invokeIf("GET", std::move(req), [this](Request &&r){ return this->urlEcho(std::move(r)); });

            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_GATEWAY_H_ */
