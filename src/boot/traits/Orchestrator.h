#ifndef _BOOT_TRAITS_ORCHESTRATOR_H_
#define _BOOT_TRAITS_ORCHESTRATOR_H_


#include "config.h"

#include "core/apps/Orchestrator/Orchestrator.h"
#include "http/wget/WG_Null.h"


struct CoreElement {

    static constexpr const char * const name = "Orchestrator";      ///< The human readable name of the Core element.
    static constexpr const char * const ini  = "orchestrator.ini";  ///< The name fo the default ini file.

    template<typename DBPool, typename RB>using Type           = ::Orchestrator<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::Orchestrator<DBPool, RB>;  ///< 

    using WebGet = WG_Null;  ///< Class used to create https requests.
};

#endif  /* _BOOT_TRAITS_ORCHESTRATOR_H_ */
