// FILE: point.hpp
// DATE: 7/30/2014
// AUTH: Trevor Wilson [trwq75@mst.edu]
// DESC: Defines a multi-dimensional point class

// include guards
#ifndef WILT_POINT_HPP
#define WILT_POINT_HPP

// libraries
#include <array>
// - std::array

#include "util.h"

//! @brief  wilt scope
WILT_BEGIN
WILT_COMMON_BEGIN

  //! @class  Point
  //! @brief  An integer point class with arithmetic operations
  template <dim_t N>
  class Point
  {
  public:

    //! @brief  Default constructor. Sets all values to 0
    Point()
    {
      m_size.fill(0);
    }

#if WILT_BRACE_INITIALIZATION_ENABLED == 1
    //! @brief      Value constructor. 
    //! @param[in]  list - initializer list to copy from
    //! @exception  std::exception if list size doesn't match dimensions
    Point(std::initializer_list<pos_t> list)
    {
      if (list.size() == 1)
        m_size.fill(*list.begin());
      else if (list.size() != N)
        throw std::invalid_argument("Initializer list must match dimensions");
      else
      {
        auto iter = list.begin();
        for (pos_t i = 0; i < N; ++i, ++iter)
          m_size[i] = *iter;
      }
    }
#endif

    //! @brief      Copy constructor
    //! @param[in]  pt - Point to copy from
    Point(const Point<N>& pt)
      : m_size(pt.m_size)
    {

    }


    //! @brief      Assignment operator
    //! @param[in]  pt - the point to copy from
    //! @return     reference to this point
    Point<N>& operator = (const Point<N>& pt)
    {
      m_size = pt.m_size;
      return *this;
    }

    //! @brief      In-place addition operator
    //! @param[in]  pt - the point to add
    //! @return     reference to this point
    Point<N>& operator+= (const Point<N>& pt)
    {
      for (dim_t i = 0; i < N; ++i)
        m_size[i] += pt.m_size[i];
      return *this;
    }

    //! @brief      In-place subtraction operator
    //! @param[in]  pt - the point to subtract
    //! @return     reference to this point
    Point<N>& operator-= (const Point<N>& pt)
    {
      for (dim_t i = 0; i < N; ++i)
        m_size[i] -= pt.m_size[i];
      return *this;
    }

    //! @brief      In-place addition operator
    //! @param[in]  val - the value to add
    //! @return     reference to this point
    Point<N>& operator+= (pos_t val)
    {
      for (dim_t i = 0; i < N; ++i)
        m_size[i] += val;
      return *this;
    }

    //! @brief      In-place subtraction operator
    //! @param[in]  val - the value to subtract
    //! @return     reference to this point
    Point<N>& operator-= (pos_t val)
    {
      for (dim_t i = 0; i < N; ++i)
        m_size[i] -= val;
      return *this;
    }

    //! @brief      In-place multiplication operator
    //! @param[in]  val - the value to multiply
    //! @return     reference to this point
    Point<N>& operator*= (pos_t val)
    {
      for (dim_t i = 0; i < N; ++i)
        m_size[i] *= val;
      return *this;
    }

    //! @brief      In-place division operator
    //! @param[in]  val - the value to divide
    //! @return     reference to this point
    Point<N>& operator/= (pos_t val)
    {
      for (dim_t i = 0; i < N; ++i)
        m_size[i] /= val;
      return *this;
    }


    //! @brief      Indexing operator
    //! @param[in]  n - index
    //! @return     reference to value at the index
    pos_t& operator[] (dim_t n)
    {
      return m_size[n];
    }

    //! @brief      Indexing operator
    //! @param[in]  n - index
    //! @return     reference to value at the index
    const pos_t& operator[] (dim_t n) const
    {
      return m_size[n];
    }

    void clear()
    {
      m_size.fill(0);
    }
    
    void fill(pos_t val)
    {
      m_size.fill(val);
    }

  private:
    //! @brief  internal storage array
    std::array<pos_t, N> m_size;
    
  }; // class Point

WILT_COMMON_END

  //! @brief      Slices a point by removing an index
  //! @param[in]  pt - point to slice
  //! @param[in]  n - index to remove
  //! @return     point with the index removed
  //!
  //! Is used to simplify NArray::slice_() calls
  //! Will fail if N==0
  template <dim_t N>
  Point<N-1> _slice(const Point<N>& pt, dim_t n)
  {
    Point<N-1> ret;
    for (dim_t i = 0, j = 0; i < N; ++i)
      if (i != n)
        ret[j++] = pt[i];
    return ret;
  }

  //! @brief      Creates a point with two index values swapped
  //! @param[in]  pt - the point to swap indices
  //! @param[in]  a - first index to swap
  //! @param[in]  b - second index to swap
  //! @return     point with the indices swapped
  //!
  //! Is used to simplify NArray::t() calls
  //! Will fail if N==0
  template <dim_t N>
  Point<N> _swap(const Point<N>& pt, dim_t a, dim_t b)
  {
    Point<N> ret = pt;
    std::swap(ret[a], ret[b]);
    return ret;
  }

  template <dim_t N, dim_t M>
  Point<N> _chopLow(const Point<M>& pt)
  {
    Point<N> ret;
    for (dim_t i = 0; i < N; ++i)
      ret[i] = pt[i];
    return ret;
  }

  template <dim_t N, dim_t M>
  Point<N> _chopHigh(const Point<M>& pt)
  {
    Point<N> ret;
    for (dim_t i = 0; i < N; ++i)
      ret[i] = pt[M-N+i];
    return ret;
  }

  //! @brief      Creates a step array from a dim array
  //! @param[in]  dims - the dimension array as a point
  //! @return     step array created from dims as a point
  //!
  //! Step array for {..., a, b, c, d} would be {..., b*c*d, c*d, d, 1}
  //! Dimension array should all be positive and non-zero to produce a
  //! meaningful result
  //! Will fail if N==0
  template <dim_t N>
  Point<N> _step(const Point<N>& dims)
  {
    Point<N> ret;
    ret[N-1] = 1;
    for (dim_t i = N-1; i > 0; --i)
      ret[i-1] = ret[i] * dims[i];
    return ret;
  }

  //! @brief      Determines the size from a dim array
  //! @param[in]  dims - the dimension array as a point
  //! @return     total size denoted by the dimensions
  //!
  //! Size of {a, b, c, d} would be a*b*c*d
  //! Dimension array should all be positive to produce a meaningful result
  //! Will be zero if any dimension is zero
  //! Will fail if N==0
  template <dim_t N>
  pos_t _size(const Point<N>& dims)
  {
    pos_t ret = dims[0];
    for (dim_t i = 1; i < N; ++i)
      ret *= dims[i];
    return ret;
  }

  //! @brief         Aligns the dim and step arrays such that the step values
  //!                are increasing and positive
  //! @param[in,out] dims - the dimension array as a point
  //! @param[in,out] step - the step array as a point
  //! @return        offset to adjust the original base pointer
  //!
  //! Is used exclusively in NArray::align() to create an aligned NArray
  template <dim_t N>
  pos_t _align(Point<N>& dims, Point<N>& step)
  {
    pos_t offset = 0;
    for (dim_t i = 0; i < N; ++i)
    {
      if (step[i] < 0)
      {
        step[i] = -step[i];
        offset -= step[i] * (dims[i] - 1);
      }
    }
    for (dim_t i = 1; i < N; ++i)
    {
      for (dim_t j = i; j > 0 && step[j] > step[j-1]; --j)
      {
        std::swap(step[j], step[j-1]);
        std::swap(dims[j], dims[j-1]);
      }
    }
    return offset;
  }

  //! @brief         Condenses a dim array and step arrays into smaller arrays
  //!                if able to
  //! @param[in,out] dims - dimension array as a point
  //! @param[in,out] step1 - step array as a point
  //! @param[in,out] step2 - step array as a point that relates to another
  //!                NArray, must be aligned and related to the same dimension
  //!                array
  //! @return        the dimension of the arrays after condensing, values at or
  //!                after n in the arrays are junk
  //!
  //! Is used when applying operations on arrays to effectively reduce the
  //! dimensionality of the data which will reduce loops and function calls.
  //! Condensing the dim and step arrays from an aligned and continuous NArray
  //! should result in return=1, dims=_size(dims), step1={1}, step2={1}
  //! Dimension array should all be positive and non-zero and step arrays must 
  //! be valid to produce a meaningful result
  template <dim_t N>
  dim_t _condense(Point<N>& dims, Point<N>& step1, Point<N>& step2)
  {
    dim_t j = 0;
    for (dim_t i = 1; i < N; ++i)
    {
      if (dims[i] * step1[i] == step1[i-1])
        dims[j] *= dims[i];
      else
      {
        step1[j] = step1[i-1];
        step2[j] = step2[i-1];
        dims[++j] = dims[i];
      }
    }
    step1[j] = step1[N-1];
    step2[j] = step2[N-1];
    return j+1;
  }

  //! @brief      Convertor from an index to a point
  //! @param[in]  dims - dimension array, catalyst for the conversion
  //! @param[in]  idx - index to convert from
  //! @return     point that corresponds to the index
  template <dim_t N>
  Point<N> _idx2pos(const Point<N>& dims, pos_t idx)
  {
    Point<N> ret;
    for (int i = N-1; i >= 0; --i)
    {
      ret[i] = idx % dims[i];
      idx /= dims[i];
    }
    return ret;
  }

  //! @brief      Convertor from a point to an index
  //! @param[in]  dims - dimension array, catalyst for the conversion
  //! @param[in]  pos - point to convert from
  //! @return     index that corresponds to the point
  template <dim_t N>
  pos_t _pos2idx(const Point<N>& dims, const Point<N>& pos)
  {
    pos_t idx = pos[N-1];
    for (int i = N-2; i >= 0; --i)
      pos += pos[i] * dims[i+1];
    return idx;
  }

  inline Point<1> _point(pos_t a)
  {
    Point<1> ret;
    ret[0] = a;
    return ret;
  }

  inline Point<2> _point(pos_t a, pos_t b)
  {
    Point<2> ret;
    ret[0] = a;
    ret[1] = b;
    return ret;
  }

  inline Point<3> _point(pos_t a, pos_t b, pos_t c)
  {
    Point<3> ret;
    ret[0] = a;
    ret[1] = b;
    ret[2] = c;
    return ret;
  }

  inline Point<4> _point(pos_t a, pos_t b, pos_t c, pos_t d)
  {
    Point<4> ret;
    ret[0] = a;
    ret[1] = b;
    ret[2] = c;
    ret[3] = d;
    return ret;
  }

WILT_END

//! @brief      Compares the two points
//! @param[in]  lhs - point to compare
//! @param[in]  rhs - point to compare
//! @return     true if points are equal, false if different dimensions or
//!             elements are not equal
template <dim_t N, dim_t M>
bool operator== (const wilt::Point<N>& lhs, const wilt::Point<M>& rhs)
{
  if (N != M)
    return false;

  for (dim_t i = 0; i < N; ++i)
    if (!(lhs[i] == rhs[i]))
      return false;

  return true;
}

//! @brief      Compares the two points
//! @param[in]  lhs - point to compare
//! @param[in]  rhs - point to compare
//! @return     false if points are equal, true if different dimensions or
//!             elements are not equal
template <dim_t N, dim_t M>
bool operator!= (const wilt::Point<N>& lhs, const wilt::Point<M>& rhs)
{
  if (N != M)
    return true;

  for (dim_t i = 0; i < N; ++i)
    if (!(lhs[i] == rhs[i]))
      return true;

  return false;
}


//! @brief      Adds two points
//! @param[in]  lhs - point to add
//! @param[in]  rhs - point to add
//! @return     result from addition of the two points
template <dim_t N>
wilt::Point<N> operator+ (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs[i] + rhs[i];
  return ret;
}

//! @brief      Subtracts two points
//! @param[in]  lhs - point to subtract
//! @param[in]  rhs - point to subtract
//! @return     result from subtraction of the two points
template <dim_t N>
wilt::Point<N> operator- (const wilt::Point<N>& lhs, const wilt::Point<N>& rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs[i] - rhs[i];
  return ret;
}


//! @brief      Adds a point and a scalar
//! @param[in]  lhs - point to add
//! @param[in]  rhs - scalar to add
//! @return     result from addition of the point and scalar
template <dim_t N>
wilt::Point<N> operator+ (const wilt::Point<N>& lhs, pos_t rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs[i] + rhs;
  return ret;
}

//! @brief      Subtracts a point and a scalar
//! @param[in]  lhs - point to subtract
//! @param[in]  rhs - scalar to subtract
//! @return     result from subtraction of the point and scalar
template <dim_t N>
wilt::Point<N> operator- (const wilt::Point<N>& lhs, pos_t rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs[i] - rhs;
  return ret;
}

//! @brief      Multiplies a point and a scalar
//! @param[in]  lhs - point to multiply
//! @param[in]  rhs - scalar to multiply
//! @return     result from multipliction of the point and scalar
template <dim_t N>
wilt::Point<N> operator* (const wilt::Point<N>& lhs, pos_t rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs[i] * rhs;
  return ret;
}

//! @brief      Divides a point and a scalar
//! @param[in]  lhs - point to divide
//! @param[in]  rhs - scalar to divide
//! @return     result from division of the point and scalar
template <dim_t N>
wilt::Point<N> operator/ (const wilt::Point<N>& lhs, pos_t rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs[i] / rhs;
  return ret;
}


//! @brief      Adds a point and a scalar
//! @param[in]  lhs - scalar to add
//! @param[in]  rhs - point to add
//! @return     result from addition of the point and scalar
template <dim_t N>
wilt::Point<N> operator+ (pos_t lhs, const wilt::Point<N>& rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs + rhs[i];
  return ret;
}

//! @brief      Subtracts a point and a scalar
//! @param[in]  lhs - scalar to subtract
//! @param[in]  rhs - point to subtract
//! @return     result from subtraction of the point and scalar
template <dim_t N>
wilt::Point<N> operator- (pos_t lhs, const wilt::Point<N>& rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs - rhs[i];
  return ret;
}

//! @brief      Multiplies a point and a scalar
//! @param[in]  lhs - scalar to multiply
//! @param[in]  rhs - point to multiply
//! @return     result from multipliction of the point and scalar
template <dim_t N>
wilt::Point<N> operator* (pos_t lhs, const wilt::Point<N>& rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs * rhs[i];
  return ret;
}

//! @brief      Divides a point and a scalar
//! @param[in]  lhs - scalar to divide
//! @param[in]  rhs - point to divide
//! @return     result from division of the point and scalar
template <dim_t N>
wilt::Point<N> operator/ (pos_t lhs, const wilt::Point<N>& rhs)
{
  wilt::Point<N> ret;
  for (dim_t i = 0; i < N; ++i)
    ret[i] = lhs / rhs[i];
  return ret;
}

// WILT_POINT_HPP
#endif 