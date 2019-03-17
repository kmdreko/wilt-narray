////////////////////////////////////////////////////////////////////////////////
// FILE: narrayiterator.hpp
// DATE: 2014-07-30
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Defines an N-dimensional templated matrix iterator class

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

#ifndef WILT_NARRAYITERATOR_HPP
#define WILT_NARRAYITERATOR_HPP

#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>

#include "util.hpp"
#include "point.hpp"

namespace wilt
{
  // - defined in "narray.hpp"
  template <class T, std::size_t N> class NArray;

namespace detail
{
  // - defined below
  template <std::size_t N> void addValueToPosition(Point<N>& pos, const pos_t* sizes, pos_t value);
  template <std::size_t N> void addOneToPosition(Point<N>& pos, const pos_t* sizes);
  template <std::size_t N> void subValueFromPosition(Point<N>& pos, const pos_t* sizes, pos_t value);
  template <std::size_t N> void subOneFromPosition(Point<N>& pos, const pos_t* sizes);

} // namespace detail

  //! @class  NArrayIterator
  //!
  //! The iterator class retains its own reference to the array data, meaning
  //! the NArray it is constructed from may be deleted and the iterator will
  //! still iterate over the data
  template <class T, std::size_t N, std::size_t M>
  class NArrayIterator
  {
  public:
    ////////////////////////////////////////////////////////////////////////////
    // ASSERTS
    ////////////////////////////////////////////////////////////////////////////
    static_assert(N > 0, "source size must be greater than 0");
    static_assert(M < N, "result size must be less than source size");

  public:
    ////////////////////////////////////////////////////////////////////////////
    // TYPE DEFINITIONS
    ////////////////////////////////////////////////////////////////////////////

    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename NArray<T, M>::exposed_type;
    using difference_type = std::ptrdiff_t;
    using pointer = std::remove_reference<value_type>*;
    using reference = value_type;

  private:
    ////////////////////////////////////////////////////////////////////////////
    // PRIVATE MEMBERS
    ////////////////////////////////////////////////////////////////////////////

    const wilt::NArray<T, N>* array_;
    Point<N-M> position_;

  public:
    ////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    ////////////////////////////////////////////////////////////////////////////

    //! @brief  Default constructor. Useless
    NArrayIterator()
      : array_(nullptr),
        position_()
    {
  
    }

    NArrayIterator(const wilt::NArray<T, N>& arr)
      : array_(&arr),
        position_()
    {

    }

    //! @brief      NArray constructor from an array and offset
    //! @param[in]  arr - array whose data to reference
    //! @param[in]  pos - data offset value, defaults to 0
    NArrayIterator(const wilt::NArray<T, N>& arr, const Point<N-M>& pos)
      : array_(&arr),
        position_(pos)
    {

    }

    //! @brief      Copy constructor
    //! @param[in]  iter - iterator to copy from
    NArrayIterator(const NArrayIterator<T, N, M>& iter)
      : array_(iter.array_),
        position_(iter.position_)
    {

    }

    //! @brief      Copy constructor with offset
    //! @param[in]  iter - iterator to copy from
    //! @param[in]  pos - data offset value
    NArrayIterator(const NArrayIterator<T, N, M>& iter, const Point<N-M>& pos)
      : array_(iter.array_),
        position_(pos)
    {

    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // ASSIGNMENT OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    //! @brief      assignment operator
    //! @param[in]  iter - iterator to copy from
    //! @return     reference to this object
    NArrayIterator<T, N, M>& operator= (const NArrayIterator<T, N, M>& iter)
    {
      array_ = iter.array_;
      position_ = iter.position_;

      return *this;
    }

    //! @brief      in-place addition operator, offsets data by +pos
    //! @param[in]  pos - value to offset
    //! @return     reference to this object
    NArrayIterator<T, N, M>& operator+= (const ptrdiff_t pos)
    {
      wilt::detail::addValueToPosition(position_, array_->sizes().data(), pos);
      return *this;
    }

    //! @brief      in-place addition operator, offsets data by -pos
    //! @param[in]  pos - value to offset
    //! @return     reference to this object
    NArrayIterator<T, N, M>& operator-= (const ptrdiff_t pos)
    {
      wilt::detail::subValueFromPosition(position_, array_->sizes().data(), pos);
      return *this;
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // ACCESS FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    //! @brief      de-reference operator, invalid if <begin or >=end
    //! @return     reference to data at the iterator position
    value_type operator* () const
    {
      return at_(position_);
    }

    //! @brief      index operator
    //! @param[in]  pos - data offset value
    //! @return     reference to data at the iterator position + offset
    value_type operator[] (pos_t pos) const
    {
      auto newposition = position_;
      wilt::detail::addValueToPosition(newposition, array_->sizes().data(), pos);
      return at_(newposition);
    }

    //! @brief      gets the position of the iterator
    //! @return     point corresponding to the position in the NArray currently
    //!             pointing to
    Point<N-M> position() const
    {
      return position_;
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // COMPARISON FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    //! @brief      equal operator, determines both if it references the same
    //!             NArray data and if at same position
    //! @param[in]  iter - iterator to compare
    //! @return     true if iterators are the same, false otherwise
    bool operator== (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return position_ == iter.position_;
    }

    //! @brief      not equal operator, determines both if it references the
    //!             same NArray data and if at same position
    //! @param[in]  iter - iterator to compare
    //! @return     false if iterators are the same, true otherwise
    bool operator!= (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return !(*this == iter);
    }

    //! @brief      less than operator, is only calculated from data offset, 
    //!             does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is less than that of iter
    bool operator<  (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      // simple lexigraphical comparison
      for (std::size_t i = 0; i < N-M; ++i)
      {
        if (position_[i] < iter.position_[i]) return true;
        if (position_[i] > iter.position_[i]) return false;
      }

      return false;
    }

    //! @brief      greater than operator, is only calculated from data offset,
    //!             does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is greater than that of iter
    bool operator>  (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return iter < *this;
    }

    //! @brief      less than or equal operator, is only calculated from data 
    //!             offset, does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is less than or equal to that of iter
    bool operator<= (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return !(iter < *this);
    }

    //! @brief      greater than or equal operator, is only calculated from data
    //!             offset, does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is greater than or equal to that of iter
    bool operator>= (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return !(*this < iter);
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // MODIFIER FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    //! @brief      increments the data offset value
    //! @return     reference to this object
    NArrayIterator<T, N, M>& operator++ ()
    {
      wilt::detail::addOneToPosition(position_, array_->sizes().data());
      return *this;
    }

    //! @brief      decrements the data offset value
    //! @return     reference to this object
    NArrayIterator<T, N, M>& operator-- ()
    {
      wilt::detail::subOneFromPosition(position_, array_->sizes().data());
      return *this;
    }

    //! @brief      increments the data offset value
    //! @return     copy of this object before incrementing
    NArrayIterator<T, N, M> operator++ (int)
    {
      auto oldposition = position_;
      ++(*this);
      return NArrayIterator<T, N, M>(*this, oldposition);
    }

    //! @brief      decrements the data offset value
    //! @return     copy of this object before decrementing
    NArrayIterator<T, N, M> operator-- (int)
    {
      auto oldposition = position_;
      --(*this);
      return NArrayIterator<T, N, M>(*this, oldposition);
    }

    //! @brief      the difference of two iterators
    //! @param[in]  iter - the iterator to diff
    //! @return     the difference of the two data offset values
    difference_type operator- (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      pos_t diff = 0;
      for (std::size_t i = 0; i < N-M; ++i)
      {
        diff *= array_->sizes()[i];
        diff += (position_[i] - iter.position_[i]);
      }
      return diff;
    }

    //! @brief      addition operator
    //! @param[in]  pos - the offset to add
    //! @return     this iterator plus the offset
    NArrayIterator<T, N, M> operator+ (const ptrdiff_t pos)
    {
      auto newiter = *this;
      newiter += pos;
      return newiter;
    }

    //! @brief      subtraction operator
    //! @param[in]  pos - the offset to subtract
    //! @return     this iterator minus the offset
    NArrayIterator<T, N, M> operator- (const ptrdiff_t pos)
    {
      auto newiter = *this;
      newiter -= pos;
      return newiter;
    }

  private:
    ////////////////////////////////////////////////////////////////////////////
    // PRIVATE FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    //! @brief      gets the pointer at the data offset
    //! @param[in]  pos - data offset value
    //! @return     pointer to data at the offset
    value_type at_(const Point<N-M>& pos) const
    {
      return array_->subarrayAtUnchecked(pos);
    }

  }; // class NArrayIterator

namespace detail
{
  template <std::size_t N>
  void addValueToPosition(Point<N>& pos, const pos_t* sizes, pos_t value)
  {
    for (std::size_t i = N-1; i > 0; --i)
    {
      pos[i] += value;
      if (pos[i] < sizes[i])
        return;
      value = pos[i] / sizes[i];
      pos[i] = pos[i] % sizes[i];
    }
    pos[0] += value;
  }

  template <std::size_t N>
  void addOneToPosition(Point<N>& pos, const pos_t* sizes)
  {
    for (std::size_t i = N-1; i > 0; --i)
    {
      pos[i] += 1;
      if (pos[i] < sizes[i])
        return;
      pos[i] = 0;
    }
    pos[0] += 1;
  }

  template <std::size_t N>
  void subValueFromPosition(Point<N>& pos, const pos_t* sizes, pos_t value)
  {
    for (std::size_t i = N-1; i > 0; --i)
    {
      pos[i] -= value;
      if (pos[i] >= 0)
        return;
      value = -(pos[i] / sizes[i]);
      pos[i] = (pos[i] % sizes[i]) + sizes[i];
    }
    pos[0] -= value;
  }

  template <std::size_t N>
  void subOneFromPosition(Point<N>& pos, const pos_t* sizes)
  {
    for (std::size_t i = N-1; i > 0; --i)
    {
      pos[i] -= 1;
      if (pos[i] >= 0)
        return;
      pos[i] = sizes[i]-1;
    }
    pos[0] -= 1;
  }

} // namespace detail

} // namespace wilt

#endif // !WILT_NARRAYITERATOR_HPP
