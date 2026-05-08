#ifndef __PICKLE_UTIL_ALLOCATOR_H__
#define __PICKLE_UTIL_ALLOCATOR_H__

#include <efi.h>
#include <efibind.h>
#include <efidef.h>
#include <efilib.h>

#include <pickle/util/result.h>
#include <pickle/util/type_traits.h>

namespace Pickle::util
{

// Pool Memory Allocator
// Not works with class initiator
template<typename T, EFI_MEMORY_TYPE MEM_TYPE = EfiLoaderData>
    requires (!is_class_v<T>)
struct PoolAllocator
{
    EFI_STATUS Allocate(UINTN Size, T** OutBuffer)
    {
        EFI_STATUS status = EFI_SUCCESS;

        status = BS->AllocatePool(
            MEM_TYPE,
            (Size * sizeof(T)),
            reinterpret_cast<VOID**>(OutBuffer)
        );

        return status;
    }

    VOID Free(T* InBuffer)
    {
        BS->FreePool(InBuffer);
    }
};

};

#endif