#ifndef _ARROWHEAD_TRAITS_AUTHORIZER_H_
#define _ARROWHEAD_TRAITS_AUTHORIZER_H_

#include "config.h"

class Dispatcher;
template<typename, typename>class Authorizer;


struct CoreElement {

    static constexpr const char * const name = "Authorizer";  ///< The human readable name of the Core element.

    template<typename DBPool, typename RB>using Type           = ::Authorizer<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::Authorizer<DBPool, RB>;  ///< 

};

#endif  /* ARROWHEAD_TRAITS_AUTHORIZER_H_ */
