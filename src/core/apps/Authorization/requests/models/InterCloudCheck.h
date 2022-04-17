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
#ifndef CORE_AUTHORIZATION_REQUESTS_MODELS_INTERCLOUDCHECK_H_
#define CORE_AUTHORIZATION_REQUESTS_MODELS_INTERCLOUDCHECK_H_


#include <vector>
#include <utility>

namespace Models {

    /// Simple struct to represent an InterCloudCheck request.
    struct InterCloudCheck {

        std::string   name;
        std::string   authenticationInfo;
        std::string   op;
        bool          neighbor;
        bool          secure;

        std::vector<long> gatekeeperRelayIds;
        std::vector<long> gatewayRelayIds;

        std::vector<std::pair<long, std::vector<long>>> providerIdsWithInterfaceIds;

        long serviceDefinitionId;

        const char* validate(bool strict = false) const noexcept {
            return nullptr;
        }

    };  // class InterCloud

}  // namespace Models

#endif  /* CORE_AUTHORIZATION_REQUESTS_MODELS_INTERCLOUDCHECK_H_ */
