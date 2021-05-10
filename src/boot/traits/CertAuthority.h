#ifndef _BOOT_TRAITS_CERTAUTHORITY_H_
#define _BOOT_TRAITS_CERTAUTHORITY_H_


#include "config.h"

#include "core/apps/CertAuthority/CertAuthority.h"
#include "http/wget/WG_Null.h"


struct CoreElement {

    static constexpr const char * const name = "Certificate Authority";  ///< The human readable name of the Core element.
    static constexpr const char * const ini  = "certauthority.ini";

    template<typename DBPool, typename RB>using Type           = ::CertAuthority<DBPool, RB>;  ///< The type of the Core element.
    template<typename DBPool, typename RB>using DispatcherType = ::CertAuthority<DBPool, RB>;  ///< 

    using WebGet = WG_Null;
};

#endif  /* _BOOT_TRAITS_CERTAUTHORITY_H_ */
