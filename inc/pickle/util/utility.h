#ifndef __PICKLE_UTIL_UTILITY_H__
#define __PICKLE_UTIL_UTILITY_H__

#include <pickle/util/type_traits.h>

namespace Pickle::util
{

template<typename T>
constexpr Pickle::util::remove_reference_t<T>&&
move(T&& v)
{
    return static_cast<Pickle::util::remove_reference_t<T>&&>(v);
}

};

#endif