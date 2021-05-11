#ifndef _CORE_DEVREGISTRY_H_
#define _CORE_DEVREGISTRY_H_


#include "core/Core.h"

#include "endpoints/Device.h"
#include "endpoints/DeviceName.h"
#include "endpoints/Devices.h"
#include "endpoints/DevReg.h"
#include "endpoints/Register.h"
#include "endpoints/Query.h"
#include "endpoints/UnRegister.h"


template<typename DBPool, typename RB>class DevRegistry final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;
        using DB     = db::DatabaseConnection<typename DBPool::DatabaseType>;

    private:

        Response urlEcho(Request &&req) {
            return Response::from_stock(http::status_code::OK);
        }

    public:

        using Core<DBPool, RB>::Core;

        Response handle(Request &&req) final {

            if (!req.uri.compare("/echo"))
                return CoreUtils::call_if("GET", std::move(req), [this](Request &&r){ return this->urlEcho(std::move(r)); });

            // client services
            if (!req.uri.compare(0, 6, "/query"))
                return Parent::template invoke<Query>("POST", std::move(req));
            if (!req.uri.compare(0, 9, "/register"))
                return Parent::template invoke<Register>("POST", std::move(req));
            if (!req.uri.compare(0, 11, "/unregister"))
                return Parent::template invoke<UnRegister>("DELETE", std::move(req));

            // management services
            if (!req.uri.compare(0, 13, "/mgmt/device/") && req.uri.length() > 13)
                return Parent::template invoke<Device>("GET", std::move(req));
            if (!req.uri.compare(0, 17, "/mgmt/devicename/") && req.uri.length() > 17)
                return Parent::template invoke<DeviceName>("GET", std::move(req));

            if (!req.uri.compare(0, 13, "/mgmt/devices"))
                return Parent::template crudify<Devices>("/mgmt/devices", std::move(req));
            if (!req.uri.compare(0, 5, "/mgmt"))
                return Parent::template crudify<DevReg>("/mgmt", std::move(req));

            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_DEVREGISTRY_H_ */
