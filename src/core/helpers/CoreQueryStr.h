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
#ifndef CORE_HELPERS_COREQUERYSTR_H_
#define CORE_HELPERS_COREQUERYSTR_H_


#include <initializer_list>

#include "http/crate/Uri.h"


/// This class can be used to parse common url paramaters like 'page', 'item_per_page', 'direction'
/// and 'sort_field'.
class CoreQueryStr {

    private:

        // get params
        unsigned long page          = -1;   // current page
        unsigned long item_per_page = -1;   // items per page

        std::string sort_field    = "id";   // sort field (default: id)
        std::string direction     = "ASC";  // sorting order (default: ASC)

        bool error = false;

    public:

        CoreQueryStr(const Uri &uri, std::initializer_list<const char*> sort) {
            auto p = Uri::Parser{ uri };
            while(p) {

                auto &&kv = *p;
                auto &&sink = Uri::Parser::Sink{ kv };

                const auto res =     sink.try_consume("page", page)
                                 ||  sink.try_consume("item_per_page", item_per_page)
                                 ||  sink.try_consume("direction",  direction,  {"ASC", "DESC" })
                                 ||  sink.try_consume("sort_field", sort_field, sort);

                if (!res || sink.failed()) {
                    error = true;
                    break;
                }

                ++p;
            }

            if (!error)
                error = !p.check();
        }

        operator bool() const noexcept {
            return !error;
        }

        bool isInvalid() const noexcept {
            return ((!hasPage() && hasItemPerPage()) || (hasPage() && !hasItemPerPage()));
        }

        auto getPage() const noexcept {
            return page;
        }

        auto getItemPerPage() const noexcept {
            return item_per_page;
        }

        bool hasPage() const noexcept {
            return page != static_cast<unsigned long>(-1);
        }

        bool hasItemPerPage() const noexcept {
            return item_per_page != static_cast<unsigned long>(-1);
        }

        const std::string& sortField() const noexcept {
            return sort_field;
        }

        const std::string& sortOrder() const noexcept {
            return direction;
        }

};

#endif  /* CORE_HELPERS_COREQUERYSTR_H_ */
