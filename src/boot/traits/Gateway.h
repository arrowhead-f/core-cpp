#ifndef _BOOT_TRAITS_GATEWAY_H_
#define _BOOT_TRAITS_GATEWAY_H_


#include "config.h"

#include "core/Gateway/Gateway.h"
#include "http/wget/WG_Null.h"


struct CoreElement {

    static constexpr const char * const name = "Gateway";      ///< The human readable name of the Core element.
    static constexpr const char * const ini  = "gateway.ini";  ///< The name fo the default ini file.

    template<typename DBPool, typename RB>using Type           = ::Gateway<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::Gateway<DBPool, RB>;  ///< 

    using WebGet = WG_Null;  ///< Class used to create https requests.
};

#endif  /* _BOOT_TRAITS_GATEWAY_H_ */
