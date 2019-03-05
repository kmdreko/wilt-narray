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

  //! @class  Point
  //! @brief  An integer point class with arithmetic operations
  template <std::size_t N>
  class Point
  {
  public:

    //! @brief  Default constructor. Sets all values to 0
    Point()
    {
      data_.fill(0);
    }

    //! @brief      Value constructor. 
    //! @param[in]  list - initializer list to copy from
    //! @exception  std::exception if list size doesn't match dimensions
    Point(std::initializer_list<pos_t> list)
    {
      if (list.size() != N)
        throw std::invalid_argument("Initializer list must match dimensions");

      auto iter = list.begin();
      for (std::size_t i = 0; i < N; ++i, ++iter)
        data_[i] = *iter;
    }

    //! @brief      Copy constructor
    //! @param[in]  pt - Point to copy from
    Point(const Point<N>& pt)
      : data_(pt.data_)
    {

    }


    //! @brief      Assignment operator
    //! @param[in]  pt - the point to copy from
    //! @return     reference to this point
    Point<N>& operator = (const Point<N>& pt)
    {
      data_ = pt.data_;
      return *this;
    }

    //! @brief      In-place addition operator
    //! @param[in]  pt - the point to add
    //! @return     reference to this point
    Point<N>& operator+= (const Point<N>& pt)
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] += pt.data_[i];
      return *this;
    }

    //! @brief      In-place subtraction operator
    //! @param[in]  pt - the point to subtract
    //! @return     reference to this point
    Point<N>& operator-= (const Point<N>& pt)
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] -= pt.data_[i];
      return *this;
    }

    //! @brief      In-place addition operator
    //! @param[in]  val - the value to add
    //! @return     reference to this point
    Point<N>& operator+= (pos_t val)
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] += val;
      return *this;
    }

    //! @brief      In-place subtraction operator
    //! @param[in]  val - the value to subtract
    //! @return     reference to this point
    Point<N>& operator-= (pos_t val)
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] -= val;
      return *this;
    }

    //! @brief      In-place multiplication operator
    //! @param[in]  val - the value to multiply
    //! @return     reference to this point
    Point<N>& operator*= (pos_t val)
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] *= val;
      return *this;
    }

    //! @brief      In-place division operator
    //! @param[in]  val - the value to divide
    //! @return     reference to this point
    Point<N>& operator/= (pos_t val)
    {
      for (std::size_t i = 0; i < N; ++i)
        data_[i] /= val;
      return *this;
    }


    //! @brief      Indexing operator
    //! @param[in]  n - index
    //! @return     reference to value at the index
    pos_t& operator[] (std::size_t n)
    {
      return data_[n];
    }

    //! @brief      Indexing operator
    //! @param[in]  n - index
    //! @return     reference to value at the index
    const pos_t& operator[] (std::size_t n) const
    {
      return data_[n];
    }

    pos_t* data()
    {
      return data_.data();
    }

    const pos_t* data() const
    {
      return data_.data();
    }

    void clear()
    {
      data_.fill(0);
    }
    
    void fill(pos_t val)
    {
      data_.fill(val);
    }

    //! @brief      Slices a point by removing an index
    //! @param[in]  n - index to remove
    //! @return     copy with the index removed
    //!
    //! Is used to simplify NArray::slice() calls
    //! Will fail if N==0
    Point<N-1> removed(std::size_t n) const
    {
      Point<N-1> ret;
      for (std::size_t i = 0, j = 0; i < N; ++i)
        if (i != n)
          ret[j++] = data_[i];
      return ret;
    }

    //! @brief      Adds an element to a point at an index
    //! @param[in]  n - index to add at
    //! @param[in]  v - value to add
    //! @return     copy with the value added
    Point<N+1> inserted(std::size_t n, pos_t v) const
    {
      Point<N+1> ret;
      for (std::size_t i = 0; i < n; ++i)
        ret[i] = data_[i];
      ret[n] = v;
      for (std::size_t i = n + 1; i < N + 1; ++i)
        ret[i] = data_[i - 1];
      return ret;
    }

    //! @brief      Creates a point with two index values swapped
    //! @param[in]  a - first index to swap
    //! @param[in]  b - second index to swap
    //! @return     point with the indices swapped
    //!
    //! Is used to simplify NArray::transpose() calls
    //! Will fail if N==0
    Point<N> swapped(std::size_t a, std::size_t b) const
    {
      Point<N> ret = *this;
      std::swap(ret.data_[a], ret.data_[b]);
      return ret;
    }

    // gets the first M values
    template <std::size_t M>
    Point<M> high() const
    {
      Point<M> ret;
      for (std::size_t i = 0; i < M; ++i)
        ret[i] = data_[i];
      return ret;
    }

    // gets the last M values
    template <std::size_t M>
    Point<M> low() const
    {
      Point<M> ret;
      for (std::size_t i = 0; i < M; ++i)
        ret[i] = data_[N - M + i];
      return ret;
    }

  private:
    //! @brief  internal storage array
    std::array<pos_t, N> data_;
    
  }; // class Point

  //! @brief      Compares the two points
  //! @param[in]  lhs - point to compare
  //! @param[in]  rhs - point to compare
  //! @return     true if points are equal, false if elements are not equal
  template <std::size_t N>
  bool operator== (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs)
  {
    for (std::size_t i = 0; i < N; ++i)
      if (!(lhs[i] == rhs[i]))
        return false;

    return true;
  }

  //! @brief      Compares the two points
  //! @param[in]  lhs - point to compare
  //! @param[in]  rhs - point to compare
  //! @return     false if points are equal, true if elements are not equal
  template <std::size_t N>
  bool operator!= (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs)
  {
    for (std::size_t i = 0; i < N; ++i)
      if (!(lhs[i] == rhs[i]))
        return true;

    return false;
  }

  //! @brief      Adds two points
  //! @param[in]  lhs - point to add
  //! @param[in]  rhs - point to add
  //! @return     result from addition of the two points
  template <std::size_t N>
  wilt::Point<N> operator+ (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] + rhs[i];
    return ret;
  }

  //! @brief      Subtracts two points
  //! @param[in]  lhs - point to subtract
  //! @param[in]  rhs - point to subtract
  //! @return     result from subtraction of the two points
  template <std::size_t N>
  wilt::Point<N> operator- (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] - rhs[i];
    return ret;
  }


  //! @brief      Adds a point and a scalar
  //! @param[in]  lhs - point to add
  //! @param[in]  rhs - scalar to add
  //! @return     result from addition of the point and scalar
  template <std::size_t N>
  wilt::Point<N> operator+ (const wilt::Point<N>& lhs, pos_t rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] + rhs;
    return ret;
  }

  //! @brief      Subtracts a point and a scalar
  //! @param[in]  lhs - point to subtract
  //! @param[in]  rhs - scalar to subtract
  //! @return     result from subtraction of the point and scalar
  template <std::size_t N>
  wilt::Point<N> operator- (const wilt::Point<N>& lhs, pos_t rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] - rhs;
    return ret;
  }

  //! @brief      Multiplies a point and a scalar
  //! @param[in]  lhs - point to multiply
  //! @param[in]  rhs - scalar to multiply
  //! @return     result from multipliction of the point and scalar
  template <std::size_t N>
  wilt::Point<N> operator* (const wilt::Point<N>& lhs, pos_t rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] * rhs;
    return ret;
  }

  //! @brief      Divides a point and a scalar
  //! @param[in]  lhs - point to divide
  //! @param[in]  rhs - scalar to divide
  //! @return     result from division of the point and scalar
  template <std::size_t N>
  wilt::Point<N> operator/ (const wilt::Point<N>& lhs, pos_t rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs[i] / rhs;
    return ret;
  }


  //! @brief      Adds a point and a scalar
  //! @param[in]  lhs - scalar to add
  //! @param[in]  rhs - point to add
  //! @return     result from addition of the point and scalar
  template <std::size_t N>
  wilt::Point<N> operator+ (pos_t lhs, const wilt::Point<N>& rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs + rhs[i];
    return ret;
  }

  //! @brief      Subtracts a point and a scalar
  //! @param[in]  lhs - scalar to subtract
  //! @param[in]  rhs - point to subtract
  //! @return     result from subtraction of the point and scalar
  template <std::size_t N>
  wilt::Point<N> operator- (pos_t lhs, const wilt::Point<N>& rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs - rhs[i];
    return ret;
  }

  //! @brief      Multiplies a point and a scalar
  //! @param[in]  lhs - scalar to multiply
  //! @param[in]  rhs - point to multiply
  //! @return     result from multipliction of the point and scalar
  template <std::size_t N>
  wilt::Point<N> operator* (pos_t lhs, const wilt::Point<N>& rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs * rhs[i];
    return ret;
  }

  //! @brief      Divides a point and a scalar
  //! @param[in]  lhs - scalar to divide
  //! @param[in]  rhs - point to divide
  //! @return     result from division of the point and scalar
  template <std::size_t N>
  wilt::Point<N> operator/ (pos_t lhs, const wilt::Point<N>& rhs)
  {
    wilt::Point<N> ret;
    for (std::size_t i = 0; i < N; ++i)
      ret[i] = lhs / rhs[i];
    return ret;
  }

} // namespace wilt

#endif // !WILT_POINT_HPP
