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
#ifndef _CORE_HELPERS_COMMONPPARSER_H_
#define _CORE_HELPERS_COMMONPPARSER_H_


#include <initializer_list>

#include "http/crate/Uri.h"


/// This class can be used to parse common url paramaters like 'page', 'item_per_page', 'direction'
/// and 'sort_field'.
class CommonPParser {

    private:

        // get params
        std::string page;                   // current page
        std::string item_per_page;          // items per page
        std::string sort_field    = "id";   // sort field (default: id)
        std::string direction     = "ASC";  // sorting order (default: ASC)

        bool error = false;

    public:

        CommonPParser(const Uri &uri, std::initializer_list<const char*> sort) {
            auto p = Uri::Parser{ uri };
            while(p) {

                auto &&kv = *p;
                auto &&sink = Uri::Parser::Sink{ kv };

                const auto res =     sink.try_consume_as<unsigned long>("page", page)
                                 ||  sink.try_consume_as<unsigned long>("item_per_page", item_per_page)
                                 ||  sink.try_consume("direction",  direction,  {"ASC", "DESC" })
                                 ||  sink.try_consume_order("sort_field", sort_field, sort);

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
            return ((page.empty() && !item_per_page.empty()) || (!page.empty() && item_per_page.empty()));
        }

        const std::string& getPage() const {
            return page;
        }

        const std::string& getItemPerPage() const {
            return item_per_page;
        }


        const std::string& getSortField() const {
            return sort_field;
        }

        const std::string& getDirection() const {
            return direction;
        }

};

#endif  /* _CORE_HELPERS_COMMONPPARSER_H_ */
