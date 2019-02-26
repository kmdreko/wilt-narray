////////////////////////////////////////////////////////////////////////////////
// FILE: operators.hpp
// DATE: 2014-07-30
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Defines operators for an N-dimensional templated array class

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

#ifndef WILT_OPERATORS_HPP
#define WILT_OPERATORS_HPP

#include "util.hpp"
#include "narray.hpp"

namespace wilt
{
  template <class T, std::size_t N>
  bool operator== (const wilt::NArray<T, N>& lhs, const wilt::NArray<T, N>& rhs)
  {
    return lhs.base() == rhs.base() && lhs.sizes() == rhs.sizes() && lhs.steps() == rhs.steps();
  }

  template <class T, std::size_t N>
  bool operator!= (const wilt::NArray<T, N>& lhs, const wilt::NArray<T, N>& rhs)
  {
    return !(lhs == rhs);
  }

#define MAKE_COMPARE_OP(NAME, OP) \
  template <class T, class U, std::size_t N>                                        \
  NArray<bool, N> NAME(const NArray<T, N>& lhs, const NArray<U, N>& rhs)            \
  {                                                                                 \
    if (lhs.sizes() != rhs.sizes())                                                 \
      throw std::invalid_argument(#NAME "(): dimensions must match");               \
    if (lhs.empty())                                                                \
      return NArray<bool, N>();                                                     \
                                                                                    \
    return binaryOp<bool>(lhs, rhs, [](const T& t, const U& u) { return t OP u; }); \
  }                                                                                 \
                                                                                    \
  template <class T, class U, std::size_t N>                                        \
  NArray<bool, N> NAME(const NArray<T, N>& lhs, const U& rhs)                       \
  {                                                                                 \
    if (lhs.empty())                                                                \
      return NArray<bool, N>();                                                     \
                                                                                    \
    return unaryOp<bool>(lhs, [&rhs](const T& t) { return t OP rhs; });             \
  }                                                                                 \
                                                                                    \
  template <class T, class U, std::size_t N>                                        \
  NArray<bool, N> NAME(const T& lhs, const NArray<U, N>& rhs)                       \
  {                                                                                 \
    if (rhs.empty())                                                                \
      return NArray<bool, N>();                                                     \
                                                                                    \
    return unaryOp<bool>(rhs, [&lhs](const U& u) { return lhs OP u; });             \
  }                                                                                 \

  MAKE_COMPARE_OP(compareEQ, ==)
  MAKE_COMPARE_OP(compareNE, !=)
  MAKE_COMPARE_OP(compareLT, < )
  MAKE_COMPARE_OP(compareLE, <=)
  MAKE_COMPARE_OP(compareGT, > )
  MAKE_COMPARE_OP(compareGE, >=)

#define MAKE_BINARY_OP(NAME, OP) \
  template <class Ret, class T, class U, std::size_t N>                                                                      \
  NArray<Ret, N> NAME(const NArray<T, N>& lhs, const NArray<U, N>& rhs)                                                      \
  {                                                                                                                          \
    if (lhs.sizes() != rhs.sizes())                                                                                          \
      throw std::invalid_argument(#NAME "(): dimensions must match");                                                        \
    if (lhs.empty())                                                                                                         \
      return NArray<Ret, N>();                                                                                               \
                                                                                                                             \
    return binaryOp<Ret>(lhs, rhs, [](const T& t, const U& u) { return t OP u; });                                           \
  }                                                                                                                          \
                                                                                                                             \
  template <class Ret, class T, class U, std::size_t N>                                                                      \
  NArray<Ret, N> NAME(const NArray<T, N>& lhs, const U& rhs)                                                                 \
  {                                                                                                                          \
    if (lhs.empty())                                                                                                         \
      return NArray<Ret, N>();                                                                                               \
                                                                                                                             \
    return unaryOp<Ret>(lhs, [&rhs](const T& t) { return t OP rhs; });                                                       \
  }                                                                                                                          \
                                                                                                                             \
  template <class Ret, class T, class U, std::size_t N>                                                                      \
  NArray<Ret, N> NAME(const T& lhs, const NArray<U, N>& rhs)                                                                 \
  {                                                                                                                          \
    if (rhs.empty())                                                                                                         \
      return NArray<Ret, N>();                                                                                               \
                                                                                                                             \
    return unaryOp<Ret>(rhs, [&lhs](const U& u) { return lhs OP u; });                                                       \
  }                                                                                                                          \
                                                                                                                             \
  template <class T, class U, std::size_t N>                                                                                 \
  NArray<decltype(std::declval<T>() OP std::declval<U>()), N> operator OP (const NArray<T, N>& lhs, const NArray<U, N>& rhs) \
  {                                                                                                                          \
    return wilt::NAME<decltype(std::declval<T>() OP std::declval<U>())>(lhs, rhs);                                           \
  }                                                                                                                          \
                                                                                                                             \
  template <class T, class U, std::size_t N>                                                                                 \
  NArray<decltype(std::declval<T>() OP std::declval<U>()), N> operator OP (const NArray<T, N>& lhs, const U& rhs)            \
  {                                                                                                                          \
    return wilt::NAME<decltype(std::declval<T>() OP std::declval<U>())>(lhs, rhs);                                           \
  }                                                                                                                          \
                                                                                                                             \
  template <class T, class U, std::size_t N>                                                                                 \
  NArray<decltype(std::declval<T>() OP std::declval<U>()), N> operator OP (const T& lhs, const NArray<U, N>& rhs)            \
  {                                                                                                                          \
    return wilt::NAME<decltype(std::declval<T>() OP std::declval<U>())>(lhs, rhs);                                           \
  }                                                                                                                          \

  MAKE_BINARY_OP(add, +)
  MAKE_BINARY_OP(sub, -)
  MAKE_BINARY_OP(mul, *)
  MAKE_BINARY_OP(div, /)
  MAKE_BINARY_OP(mod, %)

  MAKE_BINARY_OP(bitwiseAnd, &)
  MAKE_BINARY_OP(bitwiseOr,  |)
  MAKE_BINARY_OP(bitwiseXor, ^)

#undef MAKE_COMPARE_OP
#undef MAKE_BINARY_OP

} // namespace wilt

#endif // !WILT_OPERATORS_HPP
