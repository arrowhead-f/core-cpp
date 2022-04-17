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
#ifndef CORE_AUTHORIZATION_REQUESTS_PARSERS_INTERCLOUDCHECK_H_
#define CORE_AUTHORIZATION_REQUESTS_PARSERS_INTERCLOUDCHECK_H_


#include <stdexcept>

#include "utils/json/JsonView.h"
#include "../models/IntraCloudCheck.h"


namespace Parsers {


    /// This class is to parse device from json.
    /// Input JSON structure:
    /// {
    ///     "cloud": {
    ///         "authenticationInfo": "string",
    ///         "name": "string",
    ///         "neighbor": true,
    ///         "operator": "string",
    ///         "secure": true
    ///         "gatekeeperRelayIds": [ 0 ],
    ///         "gatewayRelayIds": [ 0 ],
    ///     },
    ///     "providerIdsWithInterfaceIds": [ { "id": 0, "idList": [ 0 ] } ],
    ///     "serviceDefinition": "string"
    /// }
    namespace InterCloudCheck {

        inline Models::InterCloudCheck parse(const char *json_str) {

            JsonView json{ json_str };
            const auto &root = json.root();

            if (!root.is_object())
                throw std::runtime_error{ "Wrong json format." };
            const auto &dict = root.as_object();

            Models::InterCloudCheck result;

            // serviceDefinitionId
            {
                const auto it = dict.find("serviceDefinitionId");
                if (it == dict.cend())
                    throw std::runtime_error{ "Format error: consumerId is missing." };

                if (!it->second.is_int())
                    throw std::runtime_error{ "Format error: consumerId has wrong type." };

                result.serviceDefinitionId = it->second.as_int();
            }

            // cloud
            {
                const auto it = dict.find("cloud");
                if (it == dict.cend())
                    throw std::runtime_error{ "Format error: cloud is missing." };

                if (!it->second.is_object())
                    throw std::runtime_error{ "Format error: cloud has wrong type." };

                const auto &obj = it->second.as_object();
                {
                    const auto oit = obj.find("name");
                    if (oit == dict.cend())
                        throw std::runtime_error{ "Format error: consumer/name is missing." };
                    if (!oit->second.is_string())
                        throw std::runtime_error{ "Format error: consumer/name has wrong type." };
                    result.name = oit->second.as_string();
                }
                {
                    const auto oit = obj.find("authenticationInfo");
                    if (oit == dict.cend())
                        throw std::runtime_error{ "Format error: consumer/authenticationInfo is missing." };
                    if (!oit->second.is_string())
                        throw std::runtime_error{ "Format error: consumer/authenticationInfo has wrong type." };
                    result.authenticationInfo = oit->second.as_string();
                }
                {
                    const auto oit = obj.find("operator");
                    if (oit == dict.cend())
                        throw std::runtime_error{ "Format error: consumer/operator is missing." };
                    if (!oit->second.is_string())
                        throw std::runtime_error{ "Format error: consumer/operator has wrong type." };
                    result.op = oit->second.as_string();
                }
                {
                    const auto oit = obj.find("neighbor");
                    if (oit == dict.cend())
                        throw std::runtime_error{ "Format error: consumer/neighbor is missing." };
                    if (!oit->second.is_bool())
                        throw std::runtime_error{ "Format error: consumer/neighbor has wrong type." };
                    result.neighbor = oit->second.as_bool();
                }
                {
                    const auto oit = obj.find("secure");
                    if (oit == dict.cend())
                        throw std::runtime_error{ "Format error: consumer/secure is missing." };
                    if (!oit->second.is_bool())
                        throw std::runtime_error{ "Format error: consumer/secure has wrong type." };
                    result.secure = oit->second.as_bool();
                }

                {
                    const auto oit = obj.find("gatekeeperRelayIds");
                    if (oit == dict.cend())
                        throw std::runtime_error{ "Format error: consumer/gatekeeperRelayIds is missing." };
                    if (!oit->second.is_array())
                        throw std::runtime_error{ "Format error: consumer/gatekeeperRelayIds has wrong type." };
                    for(const auto &y : oit->second.as_array()) {
                        if (!y.is_int())
                            throw std::runtime_error{ "Format error: consumer/gatekeeperRelayIds should contain integers." };
                        result.gatekeeperRelayIds.push_back(y.as_int());
                    }
                }
                {
                    const auto oit = obj.find("gatewayRelayIds");
                    if (oit == dict.cend())
                        throw std::runtime_error{ "Format error: consumer/gatewayRelayIds is missing." };
                    if (!oit->second.is_array())
                        throw std::runtime_error{ "Format error: consumer/gatewayRelayIds has wrong type." };
                    for(const auto &y : oit->second.as_array()) {
                        if (!y.is_int())
                            throw std::runtime_error{ "Format error: consumer/gatewayRelayIds should contain integers." };
                        result.gatewayRelayIds.push_back(y.as_int());
                    }
                }
            }

            // providerIdsWithInterfaceIds
            {
                const auto ith = dict.find("providerIdsWithInterfaceIds");
                if (ith != dict.cend()) {
                    if (!ith->second.is_array())
                        throw std::runtime_error{ "Format error: providerIdsWithInterfaceIds should be an array." };

                    const auto &arr = ith->second.as_array();
                    for (const auto &x : arr) {
                        if (!x.is_object())
                            throw std::runtime_error{ "Format error: providerIdsWithInterfaceIdswrong should contain dictionaries." };

                        const auto &obj = x.as_object();

                        const auto oi = obj.find("id");
                        if (oi == obj.cend() || !oi->second.is_int())
                            throw std::runtime_error{ "Format error: providerIdsWithInterfaceIds/id should be an int." };

                        unsigned long id = oi->second.as_int();

                        const auto oj = obj.find("idList");
                        if (oj == dict.cend() || !oj->second.is_array())
                            throw std::runtime_error{ "Format error: providerIdsWithInterfaceIds/idList should be an array." };

                        std::vector<long> idList;
                        idList.reserve(oj->second.as_array().size());
                        for (const auto &y : oj->second.as_array()) {
                            if (!y.is_int())
                                throw std::runtime_error{ "Format error: providerIdsWithInterfaceIds/idList should contain integers." };
                            idList.push_back(y.as_int());
                        }

                        result.providerIdsWithInterfaceIds.push_back(std::make_pair(id, std::move(idList)));
                    }
                }
                else
                    throw std::runtime_error{ "Format error: providerIds is mandatory." };
            }

            return result;
        }


    }  // namespace InterCloudCheck

}  // namespace Parsers

#endif  /* CORE_AUTHORIZATION_REQUESTS_PARSERS_INTERCLOUDCHECK_H_ */
