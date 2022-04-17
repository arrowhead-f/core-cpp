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
#ifndef _CORE_HELPERS_MODELBUILDER_H_
#define _CORE_HELPERS_MODELBUILDER_H_


#include "gason/gason.h"


template<typename M>
class ModelBuilder {

    private:

        /// To avoid the automatic creation of the Model, this union is
        /// used.
        union storage_t {
            unsigned char dummy;
            M value;

            constexpr storage_t() noexcept : dummy{} {}
            ~storage_t(){}
        };

        storage_t theModel;  ///< The stored model if any.
        bool error = true;   ///< Whether there's a model stored in the builder.

        gason::JsonAllocator allocator;
        gason::JsonValue     root;

    public:

        /// Creates the model builder. It is responsible to keep all gason
        /// structures that are neccessary for later accessing the elements
        /// of the parsed json structure. Note that the parser might be
        /// destructive, thus the string parameters is not safe to access.
        /// \param data             string to parse
        /// \param builder          the builder instance
        template<typename Builder>
        ModelBuilder(std::string &data, Builder &&builder) {
            const auto status = gason::jsonParse(data.data(), root, allocator);
            if (status != gason::JSON_PARSE_OK) {
                return;
            }

            error = !builder(root, theModel.value);
        }

        /// Checks whether the model was created.
        /// \return                 true, if the model was cerated
        operator bool() const noexcept {
            return !error;
        }

        /// Returns the model. Should be only called after the builder
        /// is checked whether the moel was created.
        /// \return                 regerence to the stored model
        const M& model() const & noexcept {
            return theModel.value;
        }

        /// Returns the model. Should be only called after the builder
        /// is checked whether the moel was created.
        /// \return                 regerence to the stored model
        M&& model() const && noexcept {
            return std::move(theModel.value);
        }
};

#endif /* _CORE_HELPERS_MODELBUILDER_H_ */
