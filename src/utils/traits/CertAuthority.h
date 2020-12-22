#ifndef _ARROWHEAD_TRAITS_CERTAUTHORITY_H_
#define _ARROWHEAD_TRAITS_CERTAUTHORITY_H_


#include "config.h"

#include "core/CertAuthority/CertAuthority.h"


class Dispatcher;
//template<typename, typename>class CertAuthority;

struct CoreElement {

    static constexpr const char * const name = "Certificate Authority";  ///< The human readable name of the Core element.

    template<typename DBPool, typename RB>using Type           = ::CertAuthority<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::CertAuthority<DBPool, RB>;  ///< 
/*
#ifdef USE_SHAREDHTTP
    using DispatcherType = ::Dispatcher;
#else
    using DispatcherType = ::CertAuthority<DBPool, RB>;
#endif
*/
};

#endif  /* ARROWHEAD_TRAITS_CERTAUTHORITY_H_ */
