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

#include <iterator>
// - std::random_access_iterator_tag

#include "util.hpp"
#include "point.hpp"
#include "narray.hpp"

namespace wilt
{
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
    pos_t index_;

  public:
    ////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    ////////////////////////////////////////////////////////////////////////////

    //! @brief  Default constructor. Useless
    NArrayIterator()
      : array_(nullptr),
        index_(0)
    {
  
    }

    //! @brief      NArray constructor from an array and offset
    //! @param[in]  arr - array whose data to reference
    //! @param[in]  pos - data offset value, defaults to 0
    NArrayIterator(const wilt::NArray<T, N>& arr, pos_t pos = 0)
      : array_(&arr),
        index_(pos)
    {

    }

    //! @brief      Copy constructor
    //! @param[in]  iter - iterator to copy from
    NArrayIterator(const NArrayIterator<T, N, M>& iter)
      : array_(iter.array_),
        index_(iter.index_)
    {

    }

    //! @brief      Copy constructor with offset
    //! @param[in]  iter - iterator to copy from
    //! @param[in]  pos - data offset value
    NArrayIterator(const NArrayIterator<T, N, M>& iter, pos_t pos)
      : array_(iter.array_),
        index_(pos)
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
      index_ = iter.index_;

      return *this;
    }

    //! @brief      in-place addition operator, offsets data by +pos
    //! @param[in]  pos - value to offset
    //! @return     reference to this object
    NArrayIterator<T, N, M>& operator+= (const ptrdiff_t pos)
    {
      index_ += pos;
      return *this;
    }

    //! @brief      in-place addition operator, offsets data by -pos
    //! @param[in]  pos - value to offset
    //! @return     reference to this object
    NArrayIterator<T, N, M>& operator-= (const ptrdiff_t pos)
    {
      index_ -= pos;
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
      return at_(index_);
    }

    //! @brief      index operator
    //! @param[in]  pos - data offset value
    //! @return     reference to data at the iterator position + offset
    value_type operator[] (pos_t pos) const
    {
      return at_(index_ + pos);
    }

    //! @brief      gets the position of the iterator
    //! @return     point corresponding to the position in the NArray currently
    //!             pointing to
    Point<N-M> position() const
    {
      return wilt::detail::idx2pos_(wilt::detail::chopLow_<N-M>(array_->sizes()), index_);
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
      return array_ == iter.array_ && index_ == iter.index_;
    }

    //! @brief      not equal operator, determines both if it references the
    //!             same NArray data and if at same position
    //! @param[in]  iter - iterator to compare
    //! @return     false if iterators are the same, true otherwise
    bool operator!= (const NArrayIterator<T, N, M>& iter) const
    {
      return !(*this == iter);
    }

    //! @brief      less than operator, is only calculated from data offset, 
    //!             does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is less than that of iter
    bool operator<  (const NArrayIterator<T, N, M>& iter) const
    {
      return index_ < iter.index_;
    }

    //! @brief      greater than operator, is only calculated from data offset,
    //!             does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is greater than that of iter
    bool operator>  (const NArrayIterator<T, N, M>& iter) const
    {
      return index_ > iter.index_;
    }

    //! @brief      less than or equal operator, is only calculated from data 
    //!             offset, does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is less than or equal to that of iter
    bool operator<= (const NArrayIterator<T, N, M>& iter) const
    {
      return index_ <= iter.index_;
    }

    //! @brief      greater than or equal operator, is only calculated from data
    //!             offset, does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is greater than or equal to that of iter
    bool operator>= (const NArrayIterator<T, N, M>& iter) const
    {
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
      ++index_;
      return *this;
    }

    //! @brief      decrements the data offset value
    //! @return     reference to this object
    NArrayIterator<T, N, M>& operator-- ()
    {
      --index_;
      return *this;
    }

    //! @brief      increments the data offset value
    //! @return     copy of this object before incrementing
    NArrayIterator<T, N, M> operator++ (int)
    {
      return NArrayIterator<T, N, M>(*this, index_++);
    }

    //! @brief      decrements the data offset value
    //! @return     copy of this object before decrementing
    NArrayIterator<T, N, M> operator-- (int)
    {
      return NArrayIterator<T, N, M>(*this, index_--);
    }

    //! @brief      the addition of two iterators
    //! @param[in]  iter - the iterator to add
    //! @return     the addition of the two data offset values
    //!
    //! This value doesn't mean much, is only here to pair with operator-
    difference_type operator+ (const NArrayIterator<T, N, M>& iter) const
    {
      return index_ + iter.index_;
    }

    //! @brief      the difference of two iterators
    //! @param[in]  iter - the iterator to diff
    //! @return     the difference of the two data offset values
    difference_type operator- (const NArrayIterator<T, N, M>& iter) const
    {
      return index_ - iter.index_;
    }

    //! @brief      addition operator
    //! @param[in]  pos - the offset to add
    //! @return     this iterator plus the offset
    NArrayIterator<T, N, M> operator+ (const ptrdiff_t pos)
    {
      return NArrayIterator<T, N, M>(*this, index_ + pos);
    }

    //! @brief      subtraction operator
    //! @param[in]  pos - the offset to subtract
    //! @return     this iterator minus the offset
    NArrayIterator<T, N, M> operator- (const ptrdiff_t pos)
    {
      return NArrayIterator<T, N, M>(*this, index_ - pos);
    }

  private:
    ////////////////////////////////////////////////////////////////////////////
    // PRIVATE FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    //! @brief      gets the pointer at the data offset
    //! @param[in]  pos - data offset value
    //! @return     pointer to data at the offset
    value_type at_(pos_t pos) const
    {
      return array_->subarrayAt(position());
    }

  }; // class NArrayIterator

} // namespace wilt

#endif // !WILT_NARRAYITERATOR_HPP
