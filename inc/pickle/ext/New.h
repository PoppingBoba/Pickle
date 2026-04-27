#ifndef __PICKLE_EXT_NEW_H__
#define __PICKLE_EXT_NEW_H__

#include <efibind.h>

inline void* operator new(UINTN, void* p) noexcept 
{
    return p;
}

#endif