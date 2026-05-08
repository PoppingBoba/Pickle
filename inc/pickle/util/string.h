#ifndef __PICKLE_UTIL_STRING_H__
#define __PICKLE_UTIL_STRING_H__

#include <efi.h>
#include <efilib.h>

#include <pickle/util/dynamic_array.h>

namespace Pickle::util
{

template<typename T>
class String
{
    Pickle::util::DynamicArray<T> _data;

public:
    UINTN Length(void)
    {
        return _data.Size();
    }

    T* Str(void)
    {
        return _data.Data();
    }

public:
    template<UINTN SIZE>
    String<T>& operator=(const T (&Str)[SIZE])
    {
        _data.Move(Str, SIZE);
        return *this;
    }

    String<T>& operator=(const String<T>& Str)
    {
        _data.Move(Str.Str(), Str.Length());
        return *this;
    }

public:
    String() { };
    ~String() { };
};

class String16 : public String<CHAR16>
{
public:
};

class String8 : public String<CHAR8>
{
public:
};

};

#endif