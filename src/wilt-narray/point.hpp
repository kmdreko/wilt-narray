////////////////////////////////////////////////////////////////////////////////
// FILE: point.hpp
// DATE: 2014-07-30
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Defines a multi-dimensional point class

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

#ifndef WILT_POINT_HPP
#define WILT_POINT_HPP

#include <array>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace wilt
{
  // type aliases
  using pos_t = std::ptrdiff_t;

  //////////////////////////////////////////////////////////////////////////////
  // This class is designed to resemble an N-dimensional integral-point object.
  //
  // This class is basically a wrapper around an `std::array<int, N>` member
  // that provides both functions and semantics to use it as a generic integral-
  // point object as well as those needed to manipulate the dimensions as
  // required by the `NArray` class.
  //
  // The point has the traditional access methods like `operator[]` and `data()`
  // but it also provides the expected mathematical operators (`+`, `-`, `*`,
  // and `\`) to use it like a geometric point. In addition, there are methods
  // like `inserted()`, `removed()`, `high()`, and `low()` that return a new
  // object with the dimensionality manipulated as needed. These functions help
  // the `NArray` class perform its transformations.

  template <std::size_t N>
  class Point
  {
  private:
    ////////////////////////////////////////////////////////////////////////////
    // PRIVATE MEMBERS
    ////////////////////////////////////////////////////////////////////////////

    std::array<pos_t, N> data_; // internal storage

  public:
    ////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    ////////////////////////////////////////////////////////////////////////////

    // Creates a point with all values initialized to zero
    constexpr Point() noexcept
      : data_{ }
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] = 0;
    }

    // Creates a point with the given values (number of arguments must match N)
    template <class... Args, typename std::enable_if<sizeof...(Args) == N, int>::type* = nullptr>
    constexpr Point(Args... args) noexcept
      : data_{ args... }
    { }

    // Creates a point with values copied from another point
    constexpr Point(const Point<N>& pt) noexcept
      : data_{ pt.data_ }
    { }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // ASSIGNMENT OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    // Copies values from another point
    constexpr Point<N>& operator = (const Point<N>& pt) noexcept
    {
      data_ = pt.data_;
      return *this;
    }

    // Element-wise assignment operators

    constexpr Point<N>& operator+= (const Point<N>& pt) noexcept
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] += pt.data_[i];
      return *this;
    }

    constexpr Point<N>& operator-= (const Point<N>& pt) noexcept
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] -= pt.data_[i];
      return *this;
    }

    // Per-element assignment operators

    constexpr Point<N>& operator+= (pos_t val) noexcept
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] += val;
      return *this;
    }

    constexpr Point<N>& operator-= (pos_t val) noexcept
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] -= val;
      return *this;
    }

    constexpr Point<N>& operator*= (pos_t val) noexcept
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] *= val;
      return *this;
    }

    constexpr Point<N>& operator/= (pos_t val) noexcept
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] /= val;
      return *this;
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // ACCESS FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    constexpr pos_t& operator[] (std::size_t n) noexcept
    {
      return data_[n];
    }

    constexpr const pos_t& operator[] (std::size_t n) const noexcept
    {
      return data_[n];
    }

    constexpr pos_t* data() noexcept
    {
      return data_.data();
    }

    constexpr const pos_t* data() const noexcept
    {
      return data_.data();
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // MODIFIER FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    constexpr void clear() noexcept
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] = 0;
    }
    
    constexpr void fill(pos_t val) noexcept
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] = val;
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // GENERATIVE FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    // Creates a new point with a value removed at the index (will fail if N==0)
    constexpr Point<N-1> removed(std::size_t n) const noexcept
    {
      Point<N-1> ret;
      for (std::size_t i = 0, j = 0; i < N; ++i)
        if (i != n)
          ret[j++] = data_[i];
      return ret;
    }

    // Creates a new point with a value added at the index
    constexpr Point<N+1> inserted(std::size_t n, pos_t v) const noexcept
    {
      Point<N+1> ret;
      for (std::size_t i = 0; i < n; ++i)
        ret[i] = data_[i];
      ret[n] = v;
      for (std::size_t i = n + 1; i < N + 1; ++i)
        ret[i] = data_[i - 1];
      return ret;
    }

    // Creates a new point with a two values swapped (will fail if N==0)
    constexpr Point<N> swapped(std::size_t a, std::size_t b) const noexcept
    {
      Point<N> ret = *this;
      std::swap(ret.data_[a], ret.data_[b]);
      return ret;
    }

    // Creates a new point with the first M values
    template <std::size_t M>
    constexpr Point<M> high() const noexcept
    {
      Point<M> ret;
      for (std::size_t i = 0; i < M; ++i)
        ret[i] = data_[i];
      return ret;
    }

    // Creates a new point with the last M values
    template <std::size_t M>
    constexpr Point<M> low() const noexcept
    {
      Point<M> ret;
      for (std::size_t i = 0; i < M; ++i)
        ret[i] = data_[N - M + i];
      return ret;
    }
    
  }; // class Point

  template <std::size_t N>
  constexpr bool operator== (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs) noexcept
  {
    for (std::size_t i = 0; i < N; ++i)
      if (!(lhs[i] == rhs[i]))
        return false;

    return true;
  }

  template <std::size_t N>
  constexpr bool operator!= (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs) noexcept
  {
    for (std::size_t i = 0; i < N; ++i)
      if (!(lhs[i] == rhs[i]))
        return true;

    return false;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator+ (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] + rhs[i];
    return ret;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator- (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] - rhs[i];
    return ret;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator+ (const wilt::Point<N>& lhs, pos_t rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] + rhs;
    return ret;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator- (const wilt::Point<N>& lhs, pos_t rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] - rhs;
    return ret;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator* (const wilt::Point<N>& lhs, pos_t rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] * rhs;
    return ret;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator/ (const wilt::Point<N>& lhs, pos_t rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] / rhs;
    return ret;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator+ (pos_t lhs, const wilt::Point<N>& rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs + rhs[i];
    return ret;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator- (pos_t lhs, const wilt::Point<N>& rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs - rhs[i];
    return ret;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator* (pos_t lhs, const wilt::Point<N>& rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs * rhs[i];
    return ret;
  }

  template <std::size_t N>
  constexpr wilt::Point<N> operator/ (pos_t lhs, const wilt::Point<N>& rhs) noexcept
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs / rhs[i];
    return ret;
  }

} // namespace wilt

#endif // !WILT_POINT_HPP
