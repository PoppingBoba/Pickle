#ifndef __PICKLE_UTIL_COMPILER_H__
#define __PICKLE_UTIL_COMPILER_H__

#define W(T) reinterpret_cast<const CHAR16*>(L##T)
#define T(T) const_cast<CHAR16*>(L##T)

#endif