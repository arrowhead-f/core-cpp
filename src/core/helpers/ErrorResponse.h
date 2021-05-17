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
#ifndef _CORE_ERRORRESPONSE_H_
#define _CORE_ERRORRESPONSE_H_


#include <string>

#include "http/StatusCode.h"
#include "http/crate/Response.h"


class ErrorResponse {

    public:

        /// The available error types.
        enum Type {
            ARROWHEAD,
            AUTH,
            BAD_PAYLOAD,
            INVALID_PARAMETER,
            DATA_NOT_FOUND,
            TIMEOUT,
            GENERIC,
            UNAVAILABLE
        };


    private:

        /// Converst exception code to string.
        /// \param et           the exception name
        /// \return             the exception string representation
        static std::string e2s(ErrorResponse::Type et) {
            switch (et) {
                case ARROWHEAD:
                        return "ARROWHEAD";
                case AUTH:
                        return "AUTH";
                case BAD_PAYLOAD:
                        return "BAD_PAYLOAD";
                case INVALID_PARAMETER:
                        return "INVALID_PARAMETER";
                case DATA_NOT_FOUND:
                        return "DATA_NOT_FOUND";
                case TIMEOUT:
                        return "TIMEOUT";
                case GENERIC:
                        return "GENERIC";
                case UNAVAILABLE:
                        return "UNAVAILABLE";
            }
            return "UNDEFINED";
        }

    public:

        /// Creates a json response from the given arguments.
        /// \param sc           the http status code of the response
        /// \param et           the type of the exception/error
        /// \param msg          the message sent
        /// \param origin       the url of the origin
        /// \return             the Response
        static Response from_stock(http::status_code sc, ErrorResponse::Type et, const std::string &msg, const std::string &origin) {
            #ifdef ARROWHEAD_FEAT_NO_ERROR_MSG
              return Response{ sc, "{\"errorMessage\":\"\",\"errorCode\":" + std::to_string(static_cast<int>(sc)) + ",\"exceptionType\":\"" + e2s(et) + "\",\"origin\":\"" + origin + "\"}" };
            #else
              return Response{ sc, "{\"errorMessage\":\"" + msg + "\",\"errorCode\":" + std::to_string(static_cast<int>(sc)) + ",\"exceptionType\":\"" + e2s(et) + "\",\"origin\": \"" + origin + "\"}" };
            #endif
        }

};

#endif  /* _CORE_ERRORRESPONSE_H_ */
