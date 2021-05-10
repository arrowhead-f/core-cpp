/********************************************************************************
 * Copyright (c) 2021 BME
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   * Budapest University of Technology and Economics - implementation
 *     * ng201
 ********************************************************************************/
#ifndef _CORE_DEVREGISTRY_H_
#define _CORE_DEVREGISTRY_H_


#include "core/Core.h"
#include "core/helpers/InvokeIf.h"

#include "endpoints/Device.h"
//#include "endpoints/DeviceName.h"
#include "endpoints/DeviceRegistry.h"
#include "endpoints/Devices.h"
#include "endpoints/Query.h"
#include "endpoints/Register.h"
#include "endpoints/UnRegister.h"


template<typename DBPool, typename RB>class DevRegistry final : public Core<DBPool, RB> {

    private:

        using Parent = Core<DBPool, RB>;
        using DB     = db::DatabaseConnection<typename DBPool::DatabaseType>;

    private:

        /// Creates response for the /echo request.
        /// \param req              the request
        /// \return                 the response
        Response urlEcho(Request &&req) const noexcept {
            return Response::from_stock(http::status_code::OK);
        }

    public:

        using Core<DBPool, RB>::Core;

        Response handle(Request &&req) final {

            if (req.uri.compare("/echo"))
                return invokeIf("GET", std::move(req), [this](Request &&r){ return this->urlEcho(std::move(r)); });

            // client services
            if (req.uri.compare("/query"))
                return Parent::template invoke<Endpoint::Query>("POST", std::move(req));
            if (req.uri.compare("/register"))
                return Parent::template invoke<Endpoint::Register>("POST", std::move(req));
            if (req.uri.compare("/unregister"))
                return Parent::template invoke<Endpoint::UnRegister>("DELETE", std::move(req));

            // management services
            if (req.uri.consume("/mgmt/device"))
                return Parent::template invoke<Endpoint::Device>("GET", std::move(req));
//            if (req.uri.consume("/mgmt/devicename"))
//                return Parent::template invoke<Endpoint::DeviceName>("GET", std::move(req));

            if (req.uri.consume("/mgmt/devices"))
                return Parent::template crudify<Endpoint::Devices>(std::move(req));
            if (req.uri.consume("/mgmt"))
                return Parent::template crudify<Endpoint::DeviceRegistry>(std::move(req));

            return Response::from_stock(http::status_code::NotFound);
        }

};

#endif  /* _CORE_DEVREGISTRY_H_ */
