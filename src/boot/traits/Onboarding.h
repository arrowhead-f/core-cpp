#ifndef _BOOT_TRAITS_ONBOARDING_H_
#define _BOOT_TRAITS_ONBOARDING_H_


#include "config.h"

#include "core/Onboarding/Onboarding.h"
#include "http/wget/WG_Null.h"


struct CoreElement {

    static constexpr const char * const name = "Onboarding";      ///< The human readable name of the Core element.
    static constexpr const char * const ini  = "onboarding.ini";  ///< The name fo the default ini file.

    template<typename DBPool, typename RB>using Type           = ::Onboarding<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::Onboarding<DBPool, RB>;  ///< 

    using WebGet = WG_Null;  ///< Class used to create https requests.
};

#endif  /* _BOOT_TRAITS_ONBOARDING_H_ */
