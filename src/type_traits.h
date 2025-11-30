#pragma once
#include <functional>

template <typename T>
struct lambda_traits;

template <typename R, typename... Args>
struct lambda_traits<R(Args...)>
{
    using ret = R;
    using args = std::tuple<Args...>;
};

template <typename R, typename... Args>
struct lambda_traits<R(*)(Args...)> : lambda_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct lambda_traits<R(C::*)(Args...) const> : lambda_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct lambda_traits<R(C::*)(Args...)> : lambda_traits<R(Args...)> {};

template <typename T>
struct lambda_traits
{
private:
    using call_t = decltype(&T::operator());
public:
    using ret = typename lambda_traits<call_t>::ret;
    using args = typename lambda_traits<call_t>::args;
};

template<typename Tuple>
struct tuple_tail;

template<typename T0, typename... Ts>
struct tuple_tail<std::tuple<T0, Ts...>>
{
    using type = std::tuple<Ts...>; 
};

template<typename>
struct function_signature;

template<typename R, typename... Args>
struct function_signature<R(__fastcall*)(Args...)> {
    using type = std::function<R(Args...)>;
};

template<typename T>
using function_signature_t = typename function_signature<T>::type;
