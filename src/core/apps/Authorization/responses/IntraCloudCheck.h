/********************************************************************************
 * Copyright (c) 2022 BME
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
#ifndef CORE_AUTHORIZER_RESPONE_INTRACLOUDCHECK_H_
#define CORE_AUTHORIZER_RESPONE_INTRACLOUDCHECK_H_


#include <string>

#include "utils/json/JsonBuilder.h"


namespace Responses {

    /// Helper class to craft device response.
    class IntraCloudCheck : public JsonBuilder {

        public:

            using JsonBuilder::JsonBuilder;

            struct ID {
                static constexpr const char *key = "id";
                long value;
            };

            struct SystemName {
                static constexpr const char *key = "systemName";
                const std::string &value;
            };

            struct Address {
                static constexpr const char *key = "address";
                const std::string &value;
            };

            struct Port {
                static constexpr const char *key = "port";
                long value;
            };

            struct AuthenticationInfo {
                static constexpr const char *key = "authenticationInfo";
                const std::string &value;
                const bool is_null;
            };

            struct CreatedAt {
                static constexpr const char *key = "createdAt";
                const std::string &value;
            };

            struct UpdatedAt {
                static constexpr const char *key = "updatedAt";
                const std::string &value;
            };

            struct ServiceDefinitionId {
                static constexpr const char *key = "serviceDefinitionId";
                long value;
            };

    };  // class IntraCloudCheck

}  // namespace Responses

#endif  /* CORE_AUTHORIZER_RESPONE_INTRACLOUDCHECK_H_ */
