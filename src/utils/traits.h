#ifndef _TRAITS_H_
#define _TRAITS_H_

class DummyRegistry;
class ServiceRegistry;

enum class ModuleType : unsigned {
    DummyRegistry,
    ServiceRegistry
};

template<ModuleType T>struct Module{
    //static_assert(sizeof(T) == 0, "Undefined module.");
    using Type = void;
};

template<>struct Module<ModuleType::DummyRegistry>{
    using Type = DummyRegistry;
};

template<>struct Module<ModuleType::ServiceRegistry>{
    using Type = ServiceRegistry;
};

#endif  /* _TRAITS_H_ */
