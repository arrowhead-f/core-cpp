/********************************************************************************
 * Copyright (c) 2020 BME
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
#ifndef _CORE_DISPATCHER_H_
#define _CORE_DISPATCHER_H_


#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"


class Dispatcher {

    public:

        /// Dispatches the given request.
        /// \param req              the request
        /// \return                 the response
        virtual Response dispatch(Request &&req) noexcept = 0;

        /// Reports the error.
        /// \param from             the remote address
        /// \param reason           the reason/description of the error
        virtual void report(const std::string &from, const std::string &reason) = 0;

        /// Reports the error.
        /// \param from             the remote address
        /// \param reason           the reason/description of the error
        virtual void report(const std::string &from, const char *reason) = 0;

        /// Destructor.
        virtual ~Dispatcher() = default;

};

#endif  /* _CORE_DISPATCHER_H_ */
