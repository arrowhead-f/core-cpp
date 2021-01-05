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
        virtual Response dispatch(Request &&req) = 0;

        /// Handles error.
        /// \param from             the remote address
        /// \param method           the HTTP method
        /// \param uri              the requested URI
        /// \param reason           the reason/description of the error
        virtual void error(const std::string &from, const char *method, const std::string &uri, const std::string &reason) = 0;

        static std::string subpath(const std::string &uri) {
            const auto found = uri.find('/', 1);
            return found == std::string::npos ? uri : uri.substr(found);
        }

};

#endif  /* _CORE_DISPATCHER_H_ */
