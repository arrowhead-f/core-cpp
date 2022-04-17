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
#ifndef CORE_AUTHORIZATION_REQUESTS_PARSERS_INTRACLOUD_H_
#define CORE_AUTHORIZATION_REQUESTS_PARSERS_INTRACLOUD_H_


#include <stdexcept>

#include "utils/json/JsonView.h"
#include "../models/IntraCloud.h"


namespace Parsers {


    /// This class is to parse device from json.
    /// Input JSON structure:
    ///{
    ///    "consumerId": 0,
    ///    "providerIds": [ 0,1 ],
    ///    "interfaceIds": [ 2,5 ],
    ///    "serviceDefinitionIds": [ 0 ]
    ///}
    namespace IntraCloud {


        inline Models::IntraCloud parse(const char *json_str) {

            JsonView json{ json_str };
            const auto &root = json.root();

            if (!root.is_object())
                throw std::runtime_error{ "Wrong json format." };

            const auto &dict = root.as_object();

            const auto it = dict.find("consumerId");
            if (it == dict.cend())
                throw std::runtime_error{ "Format error: consumerId is missing." };

            if (!it->second.is_int())
                throw std::runtime_error{ "Format error: consumerId has wrong type." };

            Models::IntraCloud result{ static_cast<unsigned long>(it->second.as_int()) };

            {
                const auto ith = dict.find("providerIds");
                if (ith != dict.cend()) {
                    if (!ith->second.is_array())
                        throw std::runtime_error{ "Format error: providerIds should be an array." };

                    const auto &arr = ith->second.as_array();
                    for (const auto &x : arr) {
                        if (!x.is_int())
                            throw std::runtime_error{ "Format error: wrong number in providerIds array." };
                        result.providerIds.push_back(x.as_int());
                    }

                }
                else
                    throw std::runtime_error{ "Format error: providerIds is mandatory." };
            }

            {
                const auto ith = dict.find("interfaceIds");
                if (ith != dict.cend()) {
                    if (!ith->second.is_array())
                        throw std::runtime_error{ "Format error: interfaceIds should be an array." };

                    const auto &arr = ith->second.as_array();
                    for (const auto &x : arr) {
                        if (!x.is_int())
                            throw std::runtime_error{ "Format error: wrong number in interfaceIds array." };
                        result.interfaceIds.push_back(x.as_int());
                    }
                }
                else
                    throw std::runtime_error{ "Format error: interfaceIds is mandatory." };
            }

            {
                const auto ith = dict.find("serviceDefinitionIds");
                if (ith != dict.cend()) {
                    if (!ith->second.is_array())
                        throw std::runtime_error{ "Format error: serviceDefinitionIds should be an array." };

                    const auto &arr = ith->second.as_array();
                    for (const auto &x : arr) {
                        if (!x.is_int())
                            throw std::runtime_error{ "Format error: wrong number in serviceDefinitionIds array." };
                        result.serviceDefIds.push_back(x.as_int());
                    }
                }
                else
                    throw std::runtime_error{ "Format error: serviceDefinitionIds is mandatory." };
            }

            return result;
        }


    }  // namespace IntraCloud

}  // namespace Parsers

#endif  /* CORE_AUTHORIZATION_REQUESTS_PARSERS_INTRACLOUD_H_ */
