#ifndef _ARROWHEAD_TRAITS_AUTHORIZER_H_
#define _ARROWHEAD_TRAITS_AUTHORIZER_H_

#include "config.h"

class Dispatcher;
template<typename, typename>class Authorizer;

template<typename DBPool, typename RB>struct CoreElement {
    using Type           = ::Authorizer<DBPool, RB>;
#ifdef USE_SHAREDHTTP
    using DispatcherType = ::Dispatcher;
#else
    using DispatcherType = ::Authorizer<DBPool, RB>;
#endif
};

#endif  /* ARROWHEAD_TRAITS_AUTHORIZER_H_ */
