#ifndef _BOOT_TRAITS_EVENTHANDLER_H_
#define _BOOT_TRAITS_EVENTHANDLER_H_


#include "config.h"

#include "core/EventHandler/EventHandler.h"
#include "http/wget/WG_Null.h"


struct CoreElement {

    static constexpr const char * const name = "EventHandler";      ///< The human readable name of the Core element.
    static constexpr const char * const ini  = "eventhandler.ini";  ///< The name fo the default ini file.

    template<typename DBPool, typename RB>using Type           = ::EventHandler<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::EventHandler<DBPool, RB>;  ///< 

    using WebGet = WG_Null;  ///< Class used to create https requests.
};

#endif  /* _BOOT_TRAITS_EVENTHANDLER_H_ */
