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
  //! @brief         applies an operation on two source arrays and stores the
  //!                result in a destination array
  //! @param[in,out] dst - the destination array
  //! @param[in]     src1 - pointer to 1st source array
  //! @param[in]     src2 - pointer to 2nd source array
  //! @param[in]     sizes - pointer to dimension array
  //! @param[in]     dsteps - pointer to dst step array
  //! @param[in]     s1steps - pointer to 1st source step array
  //! @param[in]     s2steps - pointer to 2nd source step array
  //! @param[in]     op - function or function object with the signature 
  //!                void(T&, U, V) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! arrays must be the same size (hence the single dimension array), and
  //! this function makes no checks whether the inputs are valid.
  template <std::size_t N, class T, class U, class V, class Operator>
  struct ternaryOpHelper {
    static void call(T* dst, const U* src1, const V* src2, const pos_t* sizes, const pos_t* dsteps, const pos_t* s1steps, const pos_t* s2steps, Operator& op) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps, src1 += *s1steps, src2 += *s2steps)
        ternaryOpHelper<N-1, T, U, V, Operator>::call(dst, src1, src2, sizes + 1, dsteps + 1, s1steps + 1, s2steps + 1, op);
    }
  };

  template <class T, class U, class V, class Operator>
  struct ternaryOpHelper<1u, T, U, V, Operator> {
    static void call(T* dst, const U* src1, const V* src2, const pos_t* sizes, const pos_t* dsteps, const pos_t* s1steps, const pos_t* s2steps, Operator& op) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps, src1 += *s1steps, src2 += *s2steps)
        op(*dst, *src1, *src2);
    }
  };

  template <std::size_t N, class T, class U, class V, class Operator>
  void ternaryOp(T* dst, const U* src1, const V* src2, const pos_t* sizes, const pos_t* dsteps, const pos_t* s1steps, const pos_t* s2steps, Operator op)
  {
    ternaryOpHelper<N, T, U, V, Operator>::call(dst, src1, src2, sizes, dsteps, s1steps, s2steps, op);
  }

  //! @brief         applies an operation on a source array and stores the
  //!                result in a destination array
  //! @param[in,out] dst - the destination array
  //! @param[in]     src - pointer to 1st source array
  //! @param[in]     sizes - pointer to dimension array
  //! @param[in]     dsteps - pointer to dst step array
  //! @param[in]     ssteps - pointer to source step array
  //! @param[in]     op - function or function object with the signature 
  //!                void(T&, U) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! arrays must be the same size (hence the single dimension array), and
  //! this function makes no checks whether the inputs are valid.
  template <std::size_t N, class T, class U, class Operator>
  struct binaryOpHelper {
    static void call(T* dst, const U* src, const pos_t* sizes, const pos_t* dsteps, const pos_t* ssteps, Operator& op) {
      for (T* end = dst + *dsteps * *sizes; dst < end; dst += *dsteps, src += *ssteps)
        binaryOpHelper<N-1, T, U, Operator>::call(dst, src, sizes + 1, dsteps + 1, ssteps + 1, op);
    }
  };

  template <class T, class U, class Operator>
  struct binaryOpHelper<1u, T, U, Operator> {
    static void call(T* dst, const U* src, const pos_t* sizes, const pos_t* dsteps, const pos_t* ssteps, Operator& op) {
      for (T* end = dst + *dsteps * *sizes; dst < end; dst += *dsteps, src += *ssteps)
        op(*dst, *src);
    }
  };

  template <std::size_t N, class T, class U, class Operator>
  void binaryOp(T* dst, const U* src, const pos_t* sizes, const pos_t* dsteps, const pos_t* ssteps, Operator op)
  {
    binaryOpHelper<N, T, U, Operator>::call(dst, src, sizes, dsteps, ssteps, op);
  }

  //! @brief         applies an operation and stores the result in a destination
  //!                array
  //! @param[in,out] dst - the destination array
  //! @param[in]     sizes - pointer to dimension array
  //! @param[in]     dsteps - pointer to dst step array
  //! @param[in]     op - function or function object with the signature 
  //!                void(T&) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! this function makes no checks whether the inputs are valid.
  template <std::size_t N, class T, class Operator>
  struct unaryOpHelper {
    static void call(T* dst, const pos_t* sizes, const pos_t* dsteps, Operator& op) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps)
        unaryOpHelper<N-1, T, Operator>::call(dst, sizes + 1, dsteps + 1, op);
    }
  };

  template <class T, class Operator>
  struct unaryOpHelper<1u, T, Operator> {
    static void call(T* dst, const pos_t* sizes, const pos_t* dsteps, Operator& op) {
      for (T* end = dst + *sizes * *dsteps; dst != end; dst += *dsteps)
        op(*dst);
    }
  };

  template <std::size_t N, class T, class Operator>
  void unaryOp(T* dst, const pos_t* sizes, const pos_t* dsteps, Operator op)
  {
    unaryOpHelper<N, T, Operator>::call(dst, sizes, dsteps, op);
  }

  //! @brief         applies an operation between two source arrays and returns
  //!                a bool. Returns false upon getting the first false, or
  //!                or returns true if all operations return true
  //! @param[in]     src1 - pointer to 1st source array
  //! @param[in]     src2 - pointer to 2nd source array
  //! @param[in]     sizes - pointer to dimension array
  //! @param[in]     s1steps - pointer to 1st source step array
  //! @param[in]     s2steps - pointer to 2nd source step array
  //! @param[in]     op - function or function object with the signature 
  //!                bool(T, U) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //! @return        true if all operations return true, else false
  //!
  //! this function makes no checks whether the inputs are valid.
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

  //! @brief         applies an operation on a source array and returns a bool.
  //!                Returns false upon getting the first false, or returns true
  //!                if all operations return true
  //! @param[in]     src - pointer to source array
  //! @param[in]     sizes - pointer to dimension array
  //! @param[in]     ssteps - pointer to source step array
  //! @param[in]     op - function or function object with the signature 
  //!                bool(T) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //! @return        true if all operations return true, else false
  //!
  //! this function makes no checks whether the inputs are valid.
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
