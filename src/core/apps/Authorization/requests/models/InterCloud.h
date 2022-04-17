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
#ifndef CORE_AUTHORIZATION_REQUESTS_MODELS_INTERCLOUD_H_
#define CORE_AUTHORIZATION_REQUESTS_MODELS_INTERCLOUD_H_


#include <vector>


namespace Models {

    /// Simple struct to represent a device request.
    struct InterCloud {

        long cloudId;

        std::vector<long> providerIds;
        std::vector<long> interfaceIds;
        std::vector<long> serviceDefIds;


        InterCloud(long cloudId) : cloudId{ cloudId }{}


        const char* validate(bool strict = false) const noexcept {

            // Validation rules:
            // * One provider ID, one interface ID with multiple service definition IDs.
            // * Multiple provider IDs, multiple interface IDs with one service definition ID.

            if (serviceDefIds.size() > 1) {
                if (providerIds.size() != 1 || interfaceIds.size() != 1) {
                    return "One provider ID, one interface ID with multiple service definition IDs";
                }
                return nullptr;
            }

            if (serviceDefIds.size() == 1) {
                if (providerIds.empty() || interfaceIds.empty()) {
                    return "Multiple provider IDs, multiple interface IDs with one service definition ID.";
                }
                return nullptr;
            }
            return "Service Definition not given.";
        }

    };  // class InterCloud

}  // namespace Models

#endif  /* CORE_AUTHORIZATION_REQUESTS_MODELS_INTERCLOUD_H_ */
