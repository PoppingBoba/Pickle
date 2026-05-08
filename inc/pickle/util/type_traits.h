/*
 *
 *              type_traits.h
 * 
 * Desc: <type_traits> for Pickle framework
 * Author: Nakada Tokumei <nakada_tokumei@protonmail.com>
 * 
 */

#ifndef __PICKLE_UTIL_TYPE_TRAITS_H__
#define __PICKLE_UTIL_TYPE_TRAITS_H__

namespace Pickle::util
{

template<typename T, T val>
struct type_compare
{
    static constexpr T value = val;
};

// __is_same is compiler builtin function on GCC
template<typename T1, typename T2>
struct is_same : public type_compare<bool, __is_same(T1, T2)>
{ };

// __is_class() is compiler builtin function on GCC
template<typename T>
struct is_class : public type_compare<bool, __is_class(T)>
{ };

template<typename T>
struct is_bool : public is_same<T, bool>
{ };

template<typename T>
constexpr bool is_class_v = is_class<T>::value;

template<typename T>
constexpr bool is_bool_v = is_bool<T>::value;

// ================= Implementate remove_reference =================
template<typename T>
struct remove_reference
{
    using type = T;
};

template<typename T>
struct remove_reference<T&>
{
    using type = T;
};

template<typename T>
struct remove_reference<T&&>
{
    using type = T;
};

template<typename T>
using remove_reference_t = remove_reference<T>::type;

};
#endif