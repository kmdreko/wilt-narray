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

  //////////////////////////////////////////////////////////////////////////////
  // This class is designed to iterate through all elements or subarrays of an
  // `NArray`.
  //
  // This class works by keeping the point that its currently referencing and
  // uses its reference to the array to get the element or subarray. It also
  // provides the functions required to make it a "random access" iterator.
  //
  // The template parameters `T` and `N` correspond to the `NArray` that is
  // being iterated over, but the parameter `M` is the dimensionality of the
  // value yielded. If `M` is zero, the resultant elements are `T`s and the
  // point stored in the iterator covers all the elements. If `M` is larger, the
  // iterator will yield `NArray<T, M>` and the point stored will only cover the
  // first N-M dimensions of the array. This is done so that the same iterator
  // implementation can satisfy both the normal `begin()` and `end()` iterations
  // as well as those returned by `subarrays<M>()` since almost all the logic is
  // the same.

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

    const wilt::NArray<T, N>* array_; // pointer to the array
    Point<N-M> position_;             // position of the iterator in the array

  public:
    ////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    ////////////////////////////////////////////////////////////////////////////

    NArrayIterator()
      : array_(nullptr)
      , position_()
    { }

    // Creates the iterator from an array
    NArrayIterator(const wilt::NArray<T, N>& arr)
      : array_(&arr)
      , position_()
    { }

    // Creates the iterator from an array and position
    NArrayIterator(const wilt::NArray<T, N>& arr, const Point<N-M>& pos)
      : array_(&arr)
      , position_(pos)
    { }

    // Creates the iterator from another iterator
    NArrayIterator(const NArrayIterator<T, N, M>& iter)
      : array_(iter.array_)
      , position_(iter.position_)
    { }

    // Creates the iterator from another iterator and position
    NArrayIterator(const NArrayIterator<T, N, M>& iter, const Point<N-M>& pos)
      : array_(iter.array_)
      , position_(pos)
    { }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // ASSIGNMENT OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    NArrayIterator<T, N, M>& operator= (const NArrayIterator<T, N, M>& iter)
    {
      array_ = iter.array_;
      position_ = iter.position_;

      return *this;
    }

    // in-place addition operator, offsets the position by +pos
    NArrayIterator<T, N, M>& operator+= (const ptrdiff_t pos)
    {
      wilt::detail::addValueToPosition(position_, array_->sizes().data(), pos);
      return *this;
    }

    // in-place subtraction operator, offsets the position by -pos
    NArrayIterator<T, N, M>& operator-= (const ptrdiff_t pos)
    {
      wilt::detail::subValueFromPosition(position_, array_->sizes().data(), pos);
      return *this;
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // ACCESS FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    value_type operator* () const
    {
      return at_(position_);
    }

    value_type operator[] (pos_t pos) const
    {
      auto newposition = position_;
      wilt::detail::addValueToPosition(newposition, array_->sizes().data(), pos);
      return at_(newposition);
    }

    // gets the position of the iterator
    Point<N-M> position() const
    {
      return position_;
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // COMPARISON FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    // equal operator, returns true if they point to the same position
    bool operator== (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return position_ == iter.position_;
    }

    // not equal operator, returns false if they point to the same position
    bool operator!= (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return !(*this == iter);
    }

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

    bool operator>  (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return iter < *this;
    }

    bool operator<= (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return !(iter < *this);
    }

    bool operator>= (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      return !(*this < iter);
    }

    difference_type operator- (const NArrayIterator<T, N, M>& iter) const
    {
      assert(array_ == iter.array_);

      pos_t diff = 0;
      for (std::size_t i = 0; i < N - M; ++i)
      {
        diff *= array_->sizes()[i];
        diff += (position_[i] - iter.position_[i]);
      }
      return diff;
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // MODIFIER FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    NArrayIterator<T, N, M>& operator++ ()
    {
      wilt::detail::addOneToPosition(position_, array_->sizes().data());
      return *this;
    }

    NArrayIterator<T, N, M>& operator-- ()
    {
      wilt::detail::subOneFromPosition(position_, array_->sizes().data());
      return *this;
    }

    NArrayIterator<T, N, M> operator++ (int)
    {
      auto oldposition = position_;
      ++(*this);
      return NArrayIterator<T, N, M>(*this, oldposition);
    }

    NArrayIterator<T, N, M> operator-- (int)
    {
      auto oldposition = position_;
      --(*this);
      return NArrayIterator<T, N, M>(*this, oldposition);
    }

    NArrayIterator<T, N, M> operator+ (const ptrdiff_t pos)
    {
      auto newiter = *this;
      newiter += pos;
      return newiter;
    }

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
