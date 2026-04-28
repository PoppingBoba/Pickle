/*
 *
 *              iterator.h
 * 
 * Desc: <iterator> for Pickle framework
 * Author: Nakada Tokumei <nakada_tokumei@protonmail.com>
 * 
 */

#ifndef __PICKLE_UTIL_ITERATOR_H__
#define __PICKLE_UTIL_ITERATOR_H__

namespace Pickle::util
{

template<typename T>
class Iterator
{
    T* ptr;

public:
    Iterator(T* ptr) : ptr(ptr) { }

    T& operator*() const
    {
        return *ptr;
    }

    Iterator& operator++()
    {
        ++ptr;
        return *this;
    }

    bool operator!=(const Iterator& in) const 
    {
        return ptr != in.ptr;
    }
};

};

#endif