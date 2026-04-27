#ifndef __PICKLE_EXT_OBJECT_H__
#define __PICKLE_EXT_OBJECT_H__

#include <efi.h>

namespace Pickle::ext 
{

#define PickleDeclareCtorAndDtor(_class)    \
    _class() { Initialize(); };             \
    ~_class() { Uninitialize(); }

struct Object
{
    virtual void Initialize() = 0;
    virtual void Uninitialize() = 0;
};

};

#endif