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
#ifndef _CORE_DEVREGISTRY_PARSERS_DEVICE_H_
#define _CORE_DEVREGISTRY_PARSERS_DEVICE_H_


#include "gason/gason.h"

#include "../models/Device.h"


namespace Parsers {


    /// This class is to parse device from json.
    /// Input JSON structure:
    /// {
    ///     "address": "string",
    ///     "authenticationInfo": "string",
    ///     "deviceName": "string",
    ///     "macAddress": "string"
    /// }
    class Device {

        public:

            bool operator()(const gason::JsonValue &root, Models::Device &device) {

                Models::Device model;

                if (auto node = root.child("address")) {
                    if (!node.isString())
                        return false;
                    model.address = node.toString();
                }
                if (auto node = root.child("deviceName")) {
                    if (!node.isString())
                        return false;
                    model.dev_name = node.toString();
                }
                if (auto node = root.child("macAddress")) {
                    if (!node.isString())
                        return false;
                    model.mac_address = node.toString();
                }
                if (auto node = root.child("authenticationInfo")) {
                    if (!node.isString())
                        return false;
                    model.auth_info = node.toString();
                }

                device = std::move(model);
                return true;
            }

    };  // class Device

}  // namespace Parsers

#endif  /* _CORE_DEVREGISTRY_PARSERS_DEVICE_H_ */
