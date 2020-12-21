#ifndef _ARROWHEAD_TRAITS_SERVICEREGISTRY_H_
#define _ARROWHEAD_TRAITS_SERVICEREGISTRY_H_

#include "config.h"

class Dispatcher;
template<typename, typename>class ServiceRegistry;

template<typename DBPool, typename RB>struct CoreElement {
    using Type           = ::ServiceRegistry<DBPool, RB>;
#ifdef USE_SHAREDHTTP
    using DispatcherType = ::Dispatcher;
#else
    using DispatcherType = ::ServiceRegistry<DBPool, RB>;
#endif
};

#endif  /* ARROWHEAD_TRAITS_SERVICEREGISTRY_H_ */
