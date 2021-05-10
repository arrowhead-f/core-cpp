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
#ifndef _CORE_DEVREGISTRY_PASERS_DEVICEREGISTRY_H_
#define _CORE_DEVREGISTRY_PASERS_DEVICEREGISTRY_H_


#include "gason/gason.h"

#include "../models/Device.h"
#include "../models/DeviceRegistry.h"
#include "Device.h"


namespace Parsers {

    /// This class is to parse device registry from json.
    /// Input JSON structure:
    /// {
    ///     "device": {
    ///         "address": "string",
    ///         "authenticationInfo": "string",
    ///         "deviceName": "string",
    ///         "macAddress": "string"
    ///     },
    ///     "endOfValidity": "string",
    ///     "version": 0
    ///     "metadata": {
    ///         "additionalProp1": "string",
    ///         "additionalProp2": "string",
    ///         "additionalProp3": "string"
    ///     },
    /// }
    class DeviceRegistry {

        public:

            bool operator()(const gason::JsonValue &root, Models::DeviceRegistry &deviceRegistry) {

                Models::DeviceRegistry model;

                if (auto device = root.child("device")) {

                    if (Parsers::Device{}(device, model.device)) {

                        if (auto node = root.child("version")) {
                            if (!node.isNumber())
                                return false;
                            model.version = node.toInt();
                        }
                        if (auto node = root.child("endOfValidity")) {
                            if (!node.isString())
                                return false;
                            model.end_of_validity = node.toString();
                        }
                        if (auto node = root.child("metadata")) {
                            if (!node.isObject())
                                return false;
                            for (auto it = gason::begin(node); it != gason::end(node); ++it) {
                                model.metadata += std::string{  it->key } + "=" + std::string{ it->value.toString() } + ", ";
                            }
                            if (!model.metadata.empty()) {
                                model.metadata.pop_back();
                                model.metadata.pop_back();
                            }
                        }

                        deviceRegistry = std::move(model);
                        return true;
                    }
                }
                return false;
            }

    };  // namespace DeviceRegistry

}  // namespace Parsers

#endif  /* _CORE_DEVREGISTRY_PASERS_DEVICEREGISTRY_H_ */
