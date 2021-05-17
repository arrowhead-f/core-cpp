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
#ifndef _CORE_HELPERS_INVOKEIF_H_
#define _CORE_HELPERS_INVOKEIF_H_


#ifdef __cpp_lib_invoke
  #include <functional>
#endif
#include "http/crate/Request.h"


/// This function calls f if the method of the request matches the one given as 
/// the first parameter.
/// \param method                   method
/// \param req                      the request
/// \param f                        callable to call
/// \return                         the response
template<typename F>inline auto invokeIf(const char *method, Request &&req, F f) {

    if (req.method == method) {
        #ifdef __cpp_lib_invoke
          return std::invoke(std::move(f), std::move(req));
        #else
          return f(std::move(req));
        #endif
    }
    #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
      if (req.method == "OPTIONS")
          return Response::options(http::status_code::OK, method);
    #endif
    return Response::from_stock(http::status_code::MethodNotAllowed);
}

#endif  /* _CORE_HELPERS_INVOKEIF_H_ */
