#ifndef CORE_CPP_HASHUTILS_H
#define CORE_CPP_HASHUTILS_H

/// STD libs
#include <cstddef>
#include <functional>

/// Base forward decl
inline void hash_combine(std::size_t& seed) { }

/// Template function to calculate and combine hash codes
template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}
#endif //CORE_CPP_HASHUTILS_H
