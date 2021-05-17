#ifndef _BOOT_TRAITS_CHOREOGRAPHER_H_
#define _BOOT_TRAITS_CHOREOGRAPHER_H_


#include "config.h"

#include "core/apps/Choreographer/Choreographer.h"
#include "http/wget/WG_Null.h"


struct CoreElement {

    static constexpr const char * const name = "Choreographer";      ///< The human readable name of the Core element.
    static constexpr const char * const ini  = "choreographer.ini";  ///< The name fo the default ini file.

    template<typename DBPool, typename RB>using Type           = ::Choreographer<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::Choreographer<DBPool, RB>;  ///< 

    using WebGet = WG_Null;  ///< Class used to create https requests.
};

#endif  /* _BOOT_TRAITS_CHOREOGRAPHER_H_ */
