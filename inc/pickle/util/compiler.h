#ifndef __PICKLE_UTIL_COMPILER_H__
#define __PICKLE_UTIL_COMPILER_H__

#define W(T) reinterpret_cast<const CHAR16*>(L##T)

namespace Pickle::util
{

using nullptr_t = decltype(nullptr);

};

#endif