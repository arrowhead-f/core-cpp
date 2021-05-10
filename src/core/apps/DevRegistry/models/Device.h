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
#ifndef _CORE_DEVREGISTRY_MODELS_DEVICE_H_
#define _CORE_DEVREGISTRY_MODELS_DEVICE_H_


namespace Models {

    /// Simple struct to represent a device request.
    struct Device {

        const char *dev_name    = nullptr;
        const char *mac_address = nullptr;
        const char *address     = nullptr;
        const char *auth_info   = nullptr;


        const char* validate(bool strict = false) const noexcept {
            if (dev_name == nullptr)
                return "Device name must have value.";
            if (mac_address == nullptr)
                return "Device MAC address must have value.";
            if (address == nullptr)
                return "Device address must have value.";
            return nullptr;
        }

    };  // class Device

}  // namespace Models

#endif  /* _CORE_DEVREGISTRY_MODELS_DEVICE_H_ */
