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
#ifndef CORE_AUTHORIZATION_REQUESTS_MODELS_TOKENRULE_H_
#define CORE_AUTHORIZATION_REQUESTS_MODELS_TOKENRULE_H_


#include <string>
#include <utility>
#include <vector>


namespace Models {

    /// Simple struct to represent a TokenRule request.
    struct TokenRule {

        struct System {
            std::string   address;
            std::string   authenticationInfo;
            std::string   systemName;
            long          port;
        };

        using ServiceInterfaces = std::vector<std::string>;

        System      consumer;
        std::string service;
        std::vector<std::pair<System, ServiceInterfaces>> providers;

        const char* validate(bool strict = false) const noexcept {
            return nullptr;
        }

    };  // class TokenRule

}  // namespace Models

#endif  /* CORE_AUTHORIZATION_REQUESTS_MODELS_TOKENRULE_H_ */
