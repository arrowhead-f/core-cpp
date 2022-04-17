#ifndef BOOT_TRAITS_AUTHORIZATION_H_
#define BOOT_TRAITS_AUTHORIZATION_H_


#include "config.h"

#include "core/apps/Authorization/Authorization.h"
#include "http/wget/WG_Null.h"


struct CoreElement {

    static constexpr const char * const name = "Authorization";      ///< The human readable name of the Core element.
    static constexpr const char * const ini  = "authorization.ini";  ///< The human readable name of the Core element.

    template<typename DBPool, typename RB>using Type           = ::Authorization<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::Authorization<DBPool, RB>;  ///< 

    using WebGet = WG_Null;
};

#endif  /* BOOT_TRAITS_AUTHORIZATION_H_ */
