#ifndef __PICKLE_UTIL_DYNAMIC_ARRAY_H__
#define __PICKLE_UTIL_DYNAMIC_ARRAY_H__

#include <efi.h>
#include <efilib.h>

#include <pickle/util/allocator.h>
#include <pickle/util/compiler.h>
#include <pickle/util/iterator.h>
#include <pickle/util/type_traits.h>

namespace Pickle::util
{

// Class Type doesn't accept now...
template<typename T>
    requires(!::Pickle::util::is_class_v<T>)
class DynamicArray
{
    T* _data;
    UINTN _size;
    PoolAllocator<T> alloc;

public:
    EFI_STATUS Allocate(UINTN Size)
    {
        EFI_STATUS status = EFI_SUCCESS;
        
        if (_data)
            this->Deallocate();

        status = alloc.Allocate(Size, &_data);
        if (!EFI_ERROR(status) && 
            (status != EFI_BUFFER_TOO_SMALL))
            _size = Size;

        return status;
    }

    void Deallocate(void)
    {
        if (_data)
        {
            alloc.Free(_data);
            _data = nullptr;
            _size = 0;
        }
    }

    UINTN Size(void)
    {
        return _size;
    }

    T* Data(void)
    {
        return _data;
    }

    void Copy(const T* val, UINTN len)
    {
        UINTN realSize = len;
        
        if (realSize > _size)
            realSize = _size;

        CopyMem(_data, val, sizeof(T) * realSize);
    }

    void Copy(DynamicArray<T>& val)
    {
        Copy(val.Data(), val.Size());
    }

    void Move(const T* val, UINTN len)
    {

        // Deallocate and Allocate before copy memory
        //
        // Cause the purpose of the Move() method is to make everything, 
        // from length to data, match the input data
        {
            this->Deallocate();
            this->Allocate(len);
        }

        this->Copy(val, len);
    }

    void Move(DynamicArray<T>&& val)
    {
        this->Move(val.Data(), val.Size());

        val.Deallocate();
    }

    // Iterator Section
public:
    Pickle::util::Iterator<T> begin()
    {
        return Pickle::util::Iterator<T>(_data);
    }

    Pickle::util::Iterator<T> end()
    {
        return Pickle::util::Iterator<T>(_data + _size);
    }

    // Custom Operator Section 
public:
    template<UINTN SIZE>
    DynamicArray<T>& operator=(const T (&Arr)[SIZE])
    {
        this->Copy(Arr, SIZE);
        return *this;
    }

    DynamicArray<T>& operator=(DynamicArray<T>&& val)
    {
        this->Copy(val);
        val.Deallocate();
        return *this;
    }

    // For null pointer checking
    bool operator!=(Pickle::util::nullptr_t)
    {
        return _data != nullptr;
    }

    bool operator==(Pickle::util::nullptr_t)
    {
        return _data == nullptr;
    }

    T& operator[](int index)
    {
        return _data[index];
    }

    // For Struct Pointer Type
    T* operator->()
    {
        return _data;
    }

    const T* operator->() const
    {
        return _data;
    }

public:
    DynamicArray()
    {
        _data = nullptr;
        _size = 0;
    }

    ~DynamicArray()
    {
        if (_data)
        {
            this->Deallocate();
        }
    }
};

};

#endif