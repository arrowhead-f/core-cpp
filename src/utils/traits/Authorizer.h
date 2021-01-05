#ifndef _TRAITS_AUTHORIZER_H_
#define _TRAITS_AUTHORIZER_H_


#include "config.h"

#include "core/Authorizer/Authorizer.h"
#include "http/wget/WG_Curl.h"


struct CoreElement {

    static constexpr const char * const name = "Authorizer";  ///< The human readable name of the Core element.

    template<typename DBPool, typename RB>using Type           = ::Authorizer<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::Authorizer<DBPool, RB>;  ///< 

    using WebGet = WG_Curl;
};

#endif  /* _TRAITS_AUTHORIZER_H_ */
