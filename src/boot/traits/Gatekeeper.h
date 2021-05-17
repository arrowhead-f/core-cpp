#ifndef _BOOT_TRAITS_GATEKEEPER_H_
#define _BOOT_TRAITS_GATEKEEPER_H_


#include "config.h"

#include "core/apps/Gatekeeper/Gatekeeper.h"
#include "http/wget/WG_Null.h"


struct CoreElement {

    static constexpr const char * const name = "Gatekeeper";      ///< The human readable name of the Core element.
    static constexpr const char * const ini  = "gatekeeper.ini";  ///< The name fo the default ini file.

    template<typename DBPool, typename RB>using Type           = ::Gatekeeper<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::Gatekeeper<DBPool, RB>;  ///< 

    using WebGet = WG_Null;  ///< Class used to create https requests.
};

#endif  /* _BOOT_TRAITS_GATEKEEPER_H_ */
