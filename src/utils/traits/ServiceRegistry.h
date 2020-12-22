#ifndef _ARROWHEAD_TRAITS_SERVICEREGISTRY_H_
#define _ARROWHEAD_TRAITS_SERVICEREGISTRY_H_

#include "config.h"

class Dispatcher;
template<typename, typename>class ServiceRegistry;

struct CoreElement {

    static constexpr const char * const name = "Service Registry";  ///< The human readable name of the Core element.

    template<typename DBPool, typename RB>using Type           = ::ServiceRegistry<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::ServiceRegistry<DBPool, RB>;  ///< 

};

#endif  /* ARROWHEAD_TRAITS_SERVICEREGISTRY_H_ */
