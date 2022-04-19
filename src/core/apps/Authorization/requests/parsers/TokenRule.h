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
#ifndef CORE_AUTHORIZATION_REQUESTS_PARSERS_TOKENRULE_H_
#define CORE_AUTHORIZATION_REQUESTS_PARSERS_TOKENRULE_H_


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
    namespace TokenRule {

        namespace {

            inline auto parseSystem(const JsonView::JsonNode &node) {
                Models::TokenRule::System sys;

                if (!node.is_object())
                    throw std::runtime_error{ "Wrong json format.1" };

                const auto &dict = node.as_object();

                if(const auto it = dict.find("address"); it != dict.cend() && it->second.is_string())
                    sys.address = it->second.as_string();
                else
                    throw std::runtime_error{ "Wrong json format.2" };

                if(const auto it = dict.find("authenticationInfo"); it != dict.cend() && it->second.is_string())
                    sys.authenticationInfo = it->second.as_string();
                //else
                //    throw std::runtime_error{ "Wrong json format." };

                if(const auto it = dict.find("systemName"); it != dict.cend() && it->second.is_string())
                    sys.systemName = it->second.as_string();
                else
                    throw std::runtime_error{ "Wrong json format.3" };

                if(const auto it = dict.find("port"); it != dict.cend() && it->second.is_int())
                    sys.port = it->second.as_int();
                else
                    throw std::runtime_error{ "Wrong json format.4" };

                return sys;
            }

        }


        inline Models::TokenRule parse(const char *json_str) {

            JsonView json{ json_str };
            const auto &root = json.root();

            if (!root.is_object())
                throw std::runtime_error{ "Wrong json format.5" };

            const auto &dict = root.as_object();

            Models::TokenRule result;

            {
                const auto it = dict.find("consumer");
                if(it == dict.cend())
                    throw std::runtime_error{ "Wrong json format.6" };

                result.consumer = parseSystem(it->second);
            }

            {
                const auto it = dict.find("service");
                if(it == dict.cend() || !it->second.is_string())
                    throw std::runtime_error{ "Wrong json format.7" };

                result.service = it->second.as_string();
            }

            {
                const auto it = dict.find("providers");
                if(it == dict.cend() || !it->second.is_array())
                    throw std::runtime_error{ "Wrong json format.8" };

                for(const auto &x : it->second.as_array()) {

                    if (!x.is_object())
                        throw std::runtime_error{ "Wrong json format.19" };

                    const auto &y = x.as_object();

                    const auto itp = y.find("provider");
                    if(itp == y.cend())
                        throw std::runtime_error{ "Wrong json format.9" };

                    auto provider = parseSystem(itp->second);

                    const auto its = y.find("serviceInterfaces");
                    if(its == y.cend() || !its->second.is_array())
                        throw std::runtime_error{ "Wrong json format.11" };

                    auto serviceInterfaces = std::vector<std::string>{};
                    for(const auto &z : its->second.as_array()) {
                        if(!z. is_string())
                            throw std::runtime_error{ "Wrong json format.12" };
                        serviceInterfaces.push_back(z.as_string());
                    }

                    result.providers.push_back(std::make_pair(provider, serviceInterfaces));

                }

            }

            return result;
        }


    }  // namespace TokenRule

}  // namespace Parsers

#endif  /* CORE_AUTHORIZATION_REQUESTS_PARSERS_TOKENRULE_H_ */
