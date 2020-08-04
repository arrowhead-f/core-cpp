#ifndef _TRAITS_H_
#define _TRAITS_H_

struct Dummy {
};


enum class ModuleType : unsigned {
    ServiceRegistry
};

template<ModuleType T>struct Module{
    //static_assert(sizeof(T) == 0, "Undefined module.");
    using Type = void;
};

template<>struct Module<ModuleType::ServiceRegistry>{
    using Type = Dummy; //ServiceRegistryClass;
};

#endif  /* _TRAITS_H_ */