#pragma once
#include <functional>
namespace lexov {

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

constexpr std::size_t isqrt_impl
    (std::size_t sq, std::size_t dlt, std::size_t value){
    return sq <= value ?
        isqrt_impl(sq+dlt, dlt+2, value) : (dlt >> 1) - 1;
}

constexpr std::size_t isqrt(std::size_t value){
    return isqrt_impl(1, 3, value);
}

} // namespace lexov
