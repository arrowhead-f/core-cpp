#ifndef _ARROWHEAD_TRAITS_CERTAUTHORITY_H_
#define _ARROWHEAD_TRAITS_CERTAUTHORITY_H_

#include "config.h"


#include "core/CertAuthority/CertAuthority.h"


class Dispatcher;
//template<typename, typename>class CertAuthority;

template<typename DBPool, typename RB>struct CoreElement {
    using Type           = ::CertAuthority<DBPool, RB>;
#ifdef USE_SHAREDHTTP
    using DispatcherType = ::Dispatcher;
#else
    using DispatcherType = ::CertAuthority<DBPool, RB>;
#endif
};

#endif  /* ARROWHEAD_TRAITS_CERTAUTHORITY_H_ */
