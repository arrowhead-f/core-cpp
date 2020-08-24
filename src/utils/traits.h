#ifndef _ARROWHEAD_TRAITS_H_
#define _ARROWHEAD_TRAITS_H_

#include "core/Elements.h"

enum class CoreElementType : unsigned {
    DummyRegistry,
    ServiceRegistry
};

template<CoreElementType, typename DBPool>struct CoreElement {
    static_assert(sizeof(DBPool) == 0, "Undefined Core System Element.");
};

template<typename DBPool>struct CoreElement<CoreElementType::DummyRegistry, DBPool> {
    using Type = DummyRegistry<DBPool>;
};

template<typename DBPool>struct CoreElement<CoreElementType::ServiceRegistry, DBPool> {
    using Type = ServiceRegistry<DBPool>;
};

#endif  /* _ARROWHEAD_TRAITS_H_ */
