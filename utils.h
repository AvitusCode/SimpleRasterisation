#pragma once
#include <type_traits>

template<typename T1, typename T2>
auto MAX(const T1& a, const T2& b) -> std::common_type_t<const T1&, const T2&> {
    return (a < b) ? b : a;
}

template<typename T1, typename T2, typename... Args>
auto MAX(const T1& a, const T2& b, const Args&... args) -> std::common_type_t<const T1&, const T2&, const Args&...> {
    return MAX(a, MAX(b, args...));
}

template<typename T1, typename T2>
auto MIN(const T1& a, const T2& b) -> std::common_type_t<const T1&, const T2&> {
    return (b < a) ? b : a;
}

template<typename T1, typename T2, typename... Args>
auto MIN(const T1& a, const T2& b, const Args&... args) -> std::common_type_t<const T1&, const T2&, const Args&...> {
    return MIN(a, MIN(b, args...));
}

template <typename T>
T CLAMP(const T& value, const T& low, const T& high) {
    return value < low ? low : (value > high ? high : value);
}