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
#ifndef _CORE_DEVREGISTRY_MODELS_DEVICEREGISTRY_H_
#define _CORE_DEVREGISTRY_MODELS_DEVICEREGISTRY_H_


#include <string>

#include "Device.h"


namespace Models {


    /// Simple struct to represent a device registry request.
    struct DeviceRegistry {

        Device device;

        const char *end_of_validity = nullptr;
        int version = 1;
        std::string metadata;


        const char* validate(bool strict = false) const noexcept {
            return device.validate(strict);
        }

    };  // class DeviceRegistry

}  // namespace Models

#endif  /* _CORE_DEVREGISTRY_MODELS_DEVICEREGISTRY_H_ */
