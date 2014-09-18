// FILE: util.h
// AUTH: Trevor Wilson [trwq75@mst.edu]
// DATE: 8/20/14
// DESC: defines utilities like macros, typedefs, and required libraries

// include guards
#ifndef WILT_UTIL_H
#define WILT_UTIL_H

// disabling static assert is useful for forcing template instatiations
#ifndef WILT_DISABLE_STATIC_ASSERT
  #define WILT_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#else
  #define WILT_STATIC_ASSERT(cond, msg) if (!(cond)) throw std::domain_error(msg)
#endif

// disabling loop unrolling is useful for reducing code size
#ifndef WILT_DISABLE_LOOP_UNROLLING
  #define WILT_LOOP_UNROLLING_ENABLED 1
#else
  #define WILT_LOOP_UNROLLING_ENABLED 0
#endif

// disabling SSE2 is useful for compiling for systems where its not supported
#ifndef WILT_DISABLE_SSE2
  #define WILT_SSE2_ENABLED 1
  #include <emmintrin.h>
#else
  #define WILT_SSE2_ENABLED 0
#endif

// disabling brace initialization is useful for compiling without c++11 support
#ifndef WILT_DISABLE_BRACE_INITIALIZATION
  #define WILT_BRACE_INITIALIZATION_ENABLED 1
#else
  #define WILT_BRACE_INITIALIZATION_ENABLED 0
#endif

// disabling inline namepaces is useful for compiling without c++11 support
#ifndef WILT_DISABLE_INLINE_NAMESPACES
  #define WILT_COMMON_BEGIN inline namespace common {
  #define WILT_COMMON_END } 
#else
  #define WILT_COMMON_BEGIN
  #define WILT_COMMON_END
#endif

// a flag for invalid index
#define WILT_OUTSIDE_ARRAY PTRDIFF_MIN

// macros for namespace
#define WILT_BEGIN namespace wilt {
#define WILT_END }

// libraries
#include <utility>
// - std::move
// - std::forward
// - std::initializer_list
// - std::declval
// - std::swap
#include <cstddef>
// - std::size_t
// - std::ptrdiff_t
#include <cstdint>
// - uint8_t
#include <stdexcept>
// - std::invalid_argument
// - std::domain_error
// - std::out_of_range

// type aliases
typedef std::size_t    dim_t;
typedef std::ptrdiff_t pos_t;

//! @brief  wilt scope
WILT_BEGIN

  // operation type returns
  template <class T, class U> struct add_ret { typedef decltype(std::declval<T>() + std::declval<U>()) type; };
  template <class T, class U> struct sub_ret { typedef decltype(std::declval<T>() - std::declval<U>()) type; };
  template <class T, class U> struct mul_ret { typedef decltype(std::declval<T>() * std::declval<U>()) type; };
  template <class T, class U> struct div_ret { typedef decltype(std::declval<T>() / std::declval<U>()) type; };
  template <class T, class U> struct mod_ret { typedef decltype(std::declval<T>() % std::declval<U>()) type; };

  template <class T, class U> struct bit_and_ret { typedef decltype(std::declval<T>() & std::declval<U>()) type; };
  template <class T, class U> struct bit_or_ret  { typedef decltype(std::declval<T>() | std::declval<U>()) type; };
  template <class T, class U> struct bit_xor_ret { typedef decltype(std::declval<T>() ^ std::declval<U>()) type; };

  template <class T> struct sum_t { typedef T type; };
  template <class T> struct mean_t { typedef T type; };
  template <class T> struct raw_t { static bool value() { return false; } };
  
  template <> struct sum_t<uint8_t> { typedef int type; };
  template <> struct mean_t<uint8_t> { typedef double type; };

  template <> struct raw_t<uint8_t> { static bool value() { return true; } };
  template <> struct raw_t<uint16_t> { static bool value() { return true; } };
  template <> struct raw_t<uint32_t> { static bool value() { return true; } };
  template <> struct raw_t<uint64_t> { static bool value() { return true; } };
  template <> struct raw_t<int8_t> { static bool value() { return true; } };
  template <> struct raw_t<int16_t> { static bool value() { return true; } };
  template <> struct raw_t<int32_t> { static bool value() { return true; } };
  template <> struct raw_t<int64_t> { static bool value() { return true; } };

WILT_END

namespace std
{
  template <class T>
  struct toggle_const
  {
    typedef const T type;
  };

  template <class T>
  struct toggle_const<const T>
  {
    typedef T type;
  };
}

// WILT_UTIL_H
#endif