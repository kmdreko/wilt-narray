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

namespace wilt
{
  // type aliases
  using pos_t = std::ptrdiff_t;

namespace detail
{
  //! @brief         applies an operation on two source arrays and stores the
  //!                result in a destination array
  //! @param[out]    dst - the destination array
  //! @param[in]     src1 - pointer to 1st source array
  //! @param[in]     src2 - pointer to 2nd source array
  //! @param[in]     sizes - pointer to dimension array
  //! @param[in]     dsteps - pointer to dst step array
  //! @param[in]     s1steps - pointer to 1st source step array
  //! @param[in]     s2steps - pointer to 2nd source step array
  //! @param[in]     op - function or function object with the signature 
  //!                T(U, V) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! arrays must be the same size (hence the single dimension array), and
  //! this function makes no checks whether the inputs are valid.
  template <class T, class U, class V, class Operator>
  void binaryOp_(
    T* dst, 
    const U* src1, 
    const V* src2, 
    const pos_t* sizes,
    const pos_t* dsteps, 
    const pos_t* s1steps, 
    const pos_t* s2steps,
    Operator op, 
    std::size_t n)
  {
    T* end = dst + sizes[0] * dsteps[0];
    if (n == 1)
    {
      for (; dst != end; dst += dsteps[0], src1 += s1steps[0], src2 += s2steps[0])
        dst[0] = op(src1[0], src2[0]);
    }
    else
    {
      for (; dst != end; dst += dsteps[0], src1 += s1steps[0], src2 += s2steps[0])
        binaryOp_(dst, src1, src2, sizes + 1, dsteps + 1, s1steps + 1, s2steps + 1, op, n - 1);
    }
  }

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
  template <class T, class U, class V, class Operator>
  void binaryOp2_(
    T* dst, 
    const U* src1, 
    const V* src2, 
    const pos_t* sizes,
    const pos_t* dsteps, 
    const pos_t* s1steps, 
    const pos_t* s2steps,
    Operator op, 
    std::size_t n)
  {
    T* end = dst + sizes[0] * dsteps[0];
    if (n == 1)
    {
      for (; dst != end; dst += dsteps[0], src1 += s1steps[0], src2 += s2steps[0])
        op(dst[0], src1[0], src2[0]);
    }
    else
    {
      for (; dst != end; dst += dsteps[0], src1 += s1steps[0], src2 += s2steps[0])
        binaryOp2_(dst, src1, src2, sizes + 1, dsteps + 1, s1steps + 1, s2steps + 1, op, n - 1);
    }
  }

  //! @brief         applies an operation on a source array and stores the
  //!                result in a destination array
  //! @param[out]    dst - the destination array
  //! @param[in]     src - pointer to 1st source array
  //! @param[in]     sizes - pointer to dimension array
  //! @param[in]     dsteps - pointer to dst step array
  //! @param[in]     ssteps - pointer to source step array
  //! @param[in]     op - function or function object with the signature 
  //!                T(U) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! arrays must be the same size (hence the single dimension array), and
  //! this function makes no checks whether the inputs are valid.
  template <class T, class U, class Operator>
  void unaryOp_(
    T* dst, 
    const U* src, 
    const pos_t* sizes,
    const pos_t* dsteps, 
    const pos_t* ssteps,
    Operator op, 
    std::size_t n)
  {
    T* end = dst + dsteps[0] * sizes[0];
    if (n == 1)
    {
      for (; dst != end; dst += dsteps[0], src += ssteps[0])
        dst[0] = op(src[0]);
    }
    else
    {
      for (; dst != end; dst += dsteps[0], src += ssteps[0])
        unaryOp_(dst, src, sizes + 1, dsteps + 1, ssteps + 1, op, n - 1);
    }
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
  template <class T, class U, class Operator>
  void unaryOp2_(
    T* dst, 
    const U* src, 
    const pos_t* sizes,
    const pos_t* dsteps, 
    const pos_t* ssteps,
    Operator op, 
    std::size_t n)
  {
    T* end = dst + dsteps[0] * sizes[0];
    if (n == 1)
    {
      for (; dst < end; dst += dsteps[0], src += ssteps[0])
        op(dst[0], src[0]);
    }
    else
    {
      for (; dst < end; dst += dsteps[0], src += ssteps[0])
        unaryOp2_(dst, src, sizes + 1, dsteps + 1, ssteps + 1, op, n - 1);
    }
  }

  //! @brief         applies an operation and stores the result in a destination
  //!                array
  //! @param[out]    dst - the destination array
  //! @param[in]     sizes - pointer to dimension array
  //! @param[in]     dsteps - pointer to dst step array
  //! @param[in]     op - function or function object with the signature 
  //!                T() or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! this function makes no checks whether the inputs are valid.
  template <class T, class Operator>
  void singleOp_(
    T* dst, 
    const pos_t* sizes,
    const pos_t* dsteps,
    Operator op, 
    std::size_t n)
  {
    T* end = dst + sizes[0] * dsteps[0];
    if (n == 1)
    {
      for (; dst != end; dst += dsteps[0])
        dst[0] = op();
    }
    else
    {
      for (; dst != end; dst += dsteps[0])
        singleOp_(dst, sizes + 1, dsteps + 1, op, n - 1);
    }
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
  template <class T, class Operator>
  void singleOp2_(
    T* dst, 
    const pos_t* sizes,
    const pos_t* dsteps,
    Operator op, 
    std::size_t n)
  {
    T* end = dst + sizes[0] * dsteps[0];
    if (n == 1)
    {
      for (; dst != end; dst += dsteps[0])
        op(dst[0]);
    }
    else
    {
      for (; dst != end; dst += dsteps[0])
        singleOp2_(dst, sizes + 1, dsteps + 1, op, n - 1);
    }
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
  bool allOf_(
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
        if (!allOf_(src1, src2, sizes + 1, s1steps + 1, s2steps + 1, op, n - 1))
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
  bool allOf_(
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
        if (!allOf_(src, sizes + 1, ssteps + 1, op, n - 1))
          return false;
    }
    return true;
  }

} // namespace detail

} // namespace wilt

#endif // !WILT_UTIL_HPP
