#ifndef _BOOT_TRAITS_DEVREGISTRY_H_
#define _BOOT_TRAITS_DEVREGISTRY_H_


#include "config.h"

#include "core/DevRegistry/DevRegistry.h"
#include "http/wget/WG_Null.h"


struct CoreElement {

    static constexpr const char * const name = "DevRegistry";      ///< The human readable name of the Core element.
    static constexpr const char * const ini  = "devregistry.ini";  ///< The name fo the default ini file.

    template<typename DBPool, typename RB>using Type           = ::DevRegistry<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::DevRegistry<DBPool, RB>;  ///< 

    using WebGet = WG_Null;  ///< Class used to create https requests.
};

#endif  /* _BOOT_TRAITS_DEVREGISTRY_H_ */
