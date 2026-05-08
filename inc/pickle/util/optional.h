/*
 *
 *              optional.h
 * 
 * Desc: <optional> for Pickle framework
 * Author: Nakada Tokumei <nakada_tokumei@protonmail.com>
 * 
 */

#ifndef __PICKLE_UTIL_OPTIONAL_H__
#define __PICKLE_UTIL_OPTIONAL_H__

#include <pickle/util/type_traits.h>

namespace Pickle::util
{

/////////////////////////////////// None Type Implementation /////////////////////////////////

struct None_t
{

    enum class Token
    {
        InternalNoneToken
    };

    constexpr None_t(Token) {}
};

// Real None Value
inline constexpr None_t None { None_t::Token::InternalNoneToken };

/////////////////////////////////// None Type Implementation /////////////////////////////////

template<typename T>
class Optional
{
    // Null Opt Flag
    bool _isNullOpt;

    // Real Value
    T _val;

    // Custom Operator Section
public:
    Optional<T>& operator=(None_t)
    {
        _isNullOpt = true;
        return *this;
    }

    Optional<T>& operator=(T& val)
    {
        _val = val;
        _isNullOpt = false;
        return *this;
    }

    bool operator==(None_t)
    {
        return _isNullOpt;
    }

    bool operator!=(None_t)
    {
        return !_isNullOpt;
    }

    operator T() const
        requires(!::Pickle::util::is_bool_v<T>)
    {
        return _val;
    }

    explicit operator bool() const
    {
        return _isNullOpt;
    }

    // Member Method
public:
    T& Value()
    {
        return _val;
    }

    T ValueOr(T defaultVal)
    {
        return (_isNullOpt) ? defaultVal : _val; 
    }
};

};

#endif