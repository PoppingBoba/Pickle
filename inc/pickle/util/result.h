/*
 *
 *              result.h
 * 
 * Desc: Rust-style Result type implementation for Pickle framework
 * Author: Nakada Tokumei <nakada_tokumei@protonmail.com>
 * 
 */

#ifndef __PICKLE_UTIL_RESULT_H__
#define __PICKLE_UTIL_RESULT_H__

namespace Pickle::util
{

template<typename T>
struct Err
{
    T val;

    constexpr Err(T val) : val(val) { };
};

template<typename T, typename E>
class Result
{
    T val;
    E err;

    bool isErr;
private:
    Result<T, E>& operator=(const Err<E>& err)
    {
        err = err.val;
        isErr = true;

        return *this;
    }

    Result<T, E>& operator=(T& val)
    {
        this->val = val;
        isErr != false;

        return *this;
    }

    explicit operator bool() const
    {
        return isErr;
    }

public:
    T& Value()
    {
        return val;
    }

    bool IsErr() const
    {
        return isErr;
    }

    E Err() const
    {
        return err;
    }
};

};

#endif