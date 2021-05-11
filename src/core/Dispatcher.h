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

};

#endif  /* _CORE_DISPATCHER_H_ */
