////////////////////////////////////////////////////////////////////////////////
// FILE: util.hpp
// DATE: 2014-08-20
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: defines utilities like macros, typedefs, and required libraries

////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Trevor Wilson
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions :
// 
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef WILT_UTIL_HPP
#define WILT_UTIL_HPP

#include <cstddef>

#include "point.hpp"

namespace wilt
{
  // type aliases
  using pos_t = std::ptrdiff_t;

namespace detail
{
  // Calls a functor on all corresponding elements from three arrays that are
  // accessed by their provided size and step pointers. 
  //
  // Notes:
  // - the array sizes must all be the same (hence the single size parameter)
  // - this function makes no checks on the validity of the inputs
  // - the functor signature should be `void(T, U, V)` or similar

  template <std::size_t N, class T, class U, class V, class Functor>
  struct ternaryOpHelper {
    static void call(T* dst, const U* src1, const V* src2, const pos_t* sizes, const pos_t* dsteps, const pos_t* s1steps, const pos_t* s2steps, Functor& f) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps, src1 += *s1steps, src2 += *s2steps)
        ternaryOpHelper<N-1, T, U, V, Functor>::call(dst, src1, src2, sizes + 1, dsteps + 1, s1steps + 1, s2steps + 1, f);
    }
  };

  template <class T, class U, class V, class Functor>
  struct ternaryOpHelper<1u, T, U, V, Functor> {
    static void call(T* dst, const U* src1, const V* src2, const pos_t* sizes, const pos_t* dsteps, const pos_t* s1steps, const pos_t* s2steps, Functor& f) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps, src1 += *s1steps, src2 += *s2steps)
        f(*dst, *src1, *src2);
    }
  };

  template <std::size_t N, class T, class U, class V, class Functor>
  void ternaryOp(T* dst, const U* src1, const V* src2, const pos_t* sizes, const pos_t* dsteps, const pos_t* s1steps, const pos_t* s2steps, Functor f)
  {
    ternaryOpHelper<N, T, U, V, Functor>::call(dst, src1, src2, sizes, dsteps, s1steps, s2steps, f);
  }

  // Calls a functor on all corresponding elements from two arrays that are
  // accessed by their provided size and step pointers. 
  //
  // Notes:
  // - the array sizes must all be the same (hence the single size parameter)
  // - this function makes no checks on the validity of the inputs
  // - the functor signature should be `void(T, U)` or similar

  template <std::size_t N, class T, class U, class Functor>
  struct binaryOpHelper {
    static void call(T* dst, const U* src, const pos_t* sizes, const pos_t* dsteps, const pos_t* ssteps, Functor& f) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps, src += *ssteps)
        binaryOpHelper<N-1, T, U, Functor>::call(dst, src, sizes + 1, dsteps + 1, ssteps + 1, f);
    }
  };

  template <class T, class U, class Functor>
  struct binaryOpHelper<1u, T, U, Functor> {
    static void call(T* dst, const U* src, const pos_t* sizes, const pos_t* dsteps, const pos_t* ssteps, Functor& f) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps, src += *ssteps)
        f(*dst, *src);
    }
  };

  template <std::size_t N, class T, class U, class Functor>
  void binaryOp(T* dst, const U* src, const pos_t* sizes, const pos_t* dsteps, const pos_t* ssteps, Functor f)
  {
    binaryOpHelper<N, T, U, Functor>::call(dst, src, sizes, dsteps, ssteps, f);
  }

  // Calls a functor on all elements from an array that is accessed by its
  // provided size and step pointers. 
  //
  // Notes:
  // - this function makes no checks on the validity of the inputs
  // - the functor signature should be `void(T)` or similar

  template <std::size_t N, class T, class Functor>
  struct unaryOpHelper {
    static void call(T* dst, const pos_t* sizes, const pos_t* dsteps, Functor& f) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps)
        unaryOpHelper<N-1, T, Functor>::call(dst, sizes + 1, dsteps + 1, f);
    }
  };

  template <class T, class Functor>
  struct unaryOpHelper<1u, T, Functor> {
    static void call(T* dst, const pos_t* sizes, const pos_t* dsteps, Functor& f) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps)
        f(*dst);
    }
  };

  template <std::size_t N, class T, class Functor>
  void unaryOp(T* dst, const pos_t* sizes, const pos_t* dsteps, Functor f)
  {
    unaryOpHelper<N, T, Functor>::call(dst, sizes, dsteps, f);
  }

  // Calls a functor on corresponding elements from two arrays accessed by their
  // provided size and step pointers. The functor should return a bool. This
  // function returns false upon getting the first false or returns true if all
  // calls are true.
  //
  // Notes:
  // - the array sizes must all be the same (hence the single size parameter)
  // - this function makes no checks on the validity of the inputs
  // - the functor signature should be `bool(T, U)` or similar
  template <class T, class U, class Operator>
  bool allOf(
    const T* src1, 
    const U* src2, 
    const pos_t* sizes,
    const pos_t* s1steps, 
    const pos_t* s2steps,
    Operator op, 
    std::size_t n)
  {
    const T* end = src1 + sizes[0] * s1steps[0];
    if (n == 1)
    {
      for (; src1 != end; src1 += s1steps[0], src2 += s2steps[0])
        if (!op(src1[0], src2[0]))
          return false;
    }
    else
    {
      for (; src1 != end; src1 += s1steps[0], src2 += s2steps[0])
        if (!allOf(src1, src2, sizes + 1, s1steps + 1, s2steps + 1, op, n - 1))
          return false;
    }
    return true;
  }

  // Calls a functor on elements from an array accessed by its provided size and
  // step pointers. The functor should return a bool. This function returns
  // false upon getting the first false or returns true if all calls are true.
  //
  // Notes:
  // - this function makes no checks on the validity of the inputs
  // - the functor signature should be `bool(T)` or similar
  template <class T, class Operator>
  bool allOf(
    const T* src, 
    const pos_t* sizes,
    const pos_t* ssteps,
    Operator op, 
    std::size_t n)
  {
    const T* end = src + sizes[0] * ssteps[0];
    if (n == 1)
    {
      for (; src != end; src += ssteps[0])
        if (!op(src[0]))
          return false;
    }
    else
    {
      for (; src != end; src += ssteps[0])
        if (!allOf(src, sizes + 1, ssteps + 1, op, n - 1))
          return false;
    }
    return true;
  }

  template <class T>
  struct narray_source_traits
  {
    using type = T;

    static constexpr bool contiguous = true;
    static constexpr std::size_t dimensions = 0;

    static auto getSizes(T& t) { return Point<0>(); }
    static auto getSteps(T& t) { return Point<0>(); }
    static auto getData(T& t) { return &t; }
  };

  template <class T>
  struct narray_source_traits<std::vector<T>>
  {
    static_assert(narray_source_traits<T>::contiguous, "invalid source: data is not continguous");

    using type = typename narray_source_traits<T>::type;

    static constexpr bool contiguous = false;
    static constexpr std::size_t dimensions = 1 + narray_source_traits<T>::dimensions;

    static auto getSizes(std::vector<T>& v)
    {
      return v.empty()
        ? Point<dimensions>()
        : narray_source_traits<T>::getSizes(v[0]).inserted(0, v.size());
    }
    static auto getSteps(std::vector<T>& v)
    {
      return v.empty()
        ? Point<dimensions>()
        : narray_source_traits<T>::getSteps(v[0]).inserted(0, sizeof(T) / sizeof(type));
    }
    static auto getData(std::vector<T>& v)
    {
      return v.empty()
        ? nullptr
        : narray_source_traits<T>::getData(v[0]);
    }
  };

  template <class T, std::size_t N>
  struct narray_source_traits<std::array<T, N>>
  {
    static_assert(narray_source_traits<T>::contiguous, "invalid source: data is not continguous");

    using type = typename narray_source_traits<T>::type;

    static constexpr bool contiguous = true;
    static constexpr std::size_t dimensions = 1 + narray_source_traits<T>::dimensions;

    static auto getSizes(std::array<T, N>& a)
    {
      return narray_source_traits<T>::getSizes(a[0]).inserted(0, N);
    }
    static auto getSteps(std::array<T, N>& a)
    {
      return narray_source_traits<T>::getSteps(a[0]).inserted(0, sizeof(T) / sizeof(type));
    }
    static auto getData(std::array<T, N>& a)
    {
      return narray_source_traits<T>::getData(a[0]);
    }
  };

  template <class T, std::size_t N>
  struct narray_source_traits<T[N]>
  {
    static_assert(narray_source_traits<T>::contiguous, "invalid source: data is not continguous");

    using type = typename narray_source_traits<T>::type;

    static constexpr bool contiguous = true;
    static constexpr std::size_t dimensions = 1 + narray_source_traits<T>::dimensions;

    static auto getSizes(T(&a)[N])
    {
      return narray_source_traits<T>::getSizes(a[0]).inserted(0, N);
    }
    static auto getSteps(T(&a)[N])
    {
      return narray_source_traits<T>::getSteps(a[0]).inserted(0, sizeof(T) / sizeof(type));
    }
    static auto getData(T(&a)[N])
    {
      return narray_source_traits<T>::getData(a[0]);
    }
  };

} // namespace detail

} // namespace wilt

#endif // !WILT_UTIL_HPP
