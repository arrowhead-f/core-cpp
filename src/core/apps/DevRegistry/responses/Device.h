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
#ifndef _CORE_DEVREGISTRY_RESPONE_DEVICE_H_
#define _CORE_DEVREGISTRY_RESPONE_DEVICE_H_


#include <string>

#include "utils/json.h"


namespace Responses {

    /// Helper class to craft device response.
    class Device : public JsonBuilder {

        public:

            using JsonBuilder::JsonBuilder;

            struct Count {
                static constexpr const char *key = "count";
                long value;
            };

            struct ID {
                static constexpr const char *key = "id";
                long value;
            };

            struct CreatedAt {
                static constexpr const char *key = "createdAt";
                const std::string &value;
            };

            struct UpdatedAt {
                static constexpr const char *key = "updatedAt";
                const std::string &value;
            };

            struct Address {
                static constexpr const char *key = "address";
                const std::string &value;
            };

            struct AuthenticationInfo {
                static constexpr const char *key = "authenticationInfo";
                const std::string &value;
                const bool is_null;
            };

            struct DeviceName {
                static constexpr const char *key = "deviceName";
                const std::string &value;
            };

            struct MacAddress {
                static constexpr const char *key = "macAddress";
                const std::string &value;
            };

    };  // class Device

}  // namespace Responses

#endif  /* _CORE_DEVREGISTRY_RESPONE_DEVICE_H_ */
