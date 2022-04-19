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
#ifndef CORE_AUTHORIZER_RESPONE_TOKENDATA_H_
#define CORE_AUTHORIZER_RESPONE_TOKENDATA_H_


#include <string>

#include "utils/json/JsonBuilder.h"


namespace Responses {

    /// Helper class to craft the token data response.
    class TokenData : public JsonBuilder {

        public:

            using JsonBuilder::JsonBuilder;

            struct ProviderPort {
                static constexpr const char *key = "providerPort";
                long value;
            };

            struct ProviderAddress {
                static constexpr const char *key = "providerAddress";
                const std::string &value;
            };

            struct ProviderName {
                static constexpr const char *key = "providerName";
                const std::string &value;
            };


    };  // class TokenData

}  // namespace Responses

#endif  /* CORE_AUTHORIZER_RESPONE_TOKENDATA_H_ */
