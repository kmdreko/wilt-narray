////////////////////////////////////////////////////////////////////////////////
// FILE: narrayiterator.hpp
// DATE: 2014-07-30
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Defines an N-dimensional templated matrix iterator class

////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Trevor Wilson
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

#include "util.h"
#include "point.hpp"
#include "narray.hpp"

namespace wilt
{
  //! @class  NArrayIterator
  //!
  //! The iterator class retains its own reference to the array data, meaning
  //! the NArray it is constructed from may be deleted and the iterator will
  //! still iterate over the data
  template <class T, std::size_t N>
  class NArrayIterator
  {
  public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename std::remove_const<T>::type type;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T& reference;
    typedef T* pointer;

    //! @brief  Default constructor. Useless
    NArrayIterator()
      : data_(),
        base_(nullptr),
        sizes_(),
        steps_(),
        m_pos(0)
    {
  
    }

    //! @brief      NArray constructor from an array and offset
    //! @param[in]  arr - array whose data to reference
    //! @param[in]  pos - data offset value, defaults to 0
    NArrayIterator(const wilt::NArray<T, N>& arr, pos_t pos = 0)
      : data_(arr.data_),
        base_(arr.base_),
        sizes_(arr.sizes_),
        steps_(arr.steps_),
        m_pos(pos)
    {

    }

    //! @brief      Copy constructor
    //! @param[in]  iter - iterator to copy from
    NArrayIterator(const NArrayIterator<T, N>& iter)
      : data_(iter.data_),
        base_(iter.base_),
        sizes_(iter.sizes_),
        steps_(iter.steps_),
        m_pos(iter.m_pos)
    {

    }

    //! @brief      Copy constructor with offset
    //! @param[in]  iter - iterator to copy from
    //! @param[in]  pos - data offset value
    NArrayIterator(const NArrayIterator<T, N>& iter, pos_t pos)
      : data_(iter.data_),
        base_(iter.base_),
        sizes_(iter.sizes_),
        steps_(iter.steps_),
        m_pos(pos)
    {

    }

    //! @brief      assignment operator
    //! @param[in]  iter - iterator to copy from
    //! @return     reference to this object
    NArrayIterator<T, N>& operator= (const NArrayIterator<T, N>& iter)
    {
      data_ = iter.data_;
      base_ = iter.base_;
      sizes_ = iter.sizes_;
      steps_ = iter.steps_;
      m_pos = iter.m_pos;

      return *this;
    }

    //! @brief      in-place addition operator, offsets data by +pos
    //! @param[in]  pos - value to offset
    //! @return     reference to this object
    NArrayIterator<T, N>& operator+= (const ptrdiff_t pos)
    {
      m_pos += pos;
      return *this;
    }

    //! @brief      in-place addition operator, offsets data by -pos
    //! @param[in]  pos - value to offset
    //! @return     reference to this object
    NArrayIterator<T, N>& operator-= (const ptrdiff_t pos)
    {
      m_pos -= pos;
      return *this;
    }

    //! @brief      de-reference operator, invalid if <begin or >=end
    //! @return     reference to data at the iterator position
    reference operator* () const
    {
      return *at_(m_pos);
    }

    //! @brief      pointer operator, invalid if <begin or >=end
    //! @return     pointer to data at the iterator position
    pointer operator-> () const
    {
      return at_(m_pos);
    }

    //! @brief      index operator
    //! @param[in]  pos - data offset value
    //! @return     reference to data at the iterator position + offset
    reference operator[] (pos_t pos) const
    {
      return *at_(m_pos + pos);
    }

    //! @brief      equal operator, determines both if it references the same
    //!             NArray data and if at same position
    //! @param[in]  iter - iterator to compare
    //! @return     true if iterators are the same, false otherwise
    bool operator== (const NArrayIterator<T, N>& iter) const
    {
      return base_ == iter.base_ && 
             sizes_ == iter.sizes_ && 
             steps_ == iter.steps_ && 
             m_pos  == iter.m_pos;
    }

    //! @brief      not equal operator, determines both if it references the
    //!             same NArray data and if at same position
    //! @param[in]  iter - iterator to compare
    //! @return     false if iterators are the same, true otherwise
    bool operator!= (const NArrayIterator<T, N>& iter) const
    {
      return !(*this == iter);
    }

    //! @brief      less than operator, is only calculated from data offset, 
    //!             does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is less than that of iter
    bool operator<  (const NArrayIterator<T, N>& iter) const
    {
      return m_pos < iter.m_pos;
    }

    //! @brief      greater than operator, is only calculated from data offset,
    //!             does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is greater than that of iter
    bool operator>  (const NArrayIterator<T, N>& iter) const
    {
      return m_pos > iter.m_pos;
    }

    //! @brief      less than or equal operator, is only calculated from data 
    //!             offset, does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is less than or equal to that of iter
    bool operator<= (const NArrayIterator<T, N>& iter) const
    {
      return m_pos <= iter.m_pos;
    }

    //! @brief      greater than or equal operator, is only calculated from data
    //!             offset, does not determine if they share a data-space
    //! @param[in]  iter - iterator to compare
    //! @return     true if data offset is greater than or equal to that of iter
    bool operator>= (const NArrayIterator<T, N>& iter) const
    {
      return !(*this < iter);
    }

    //! @brief      increments the data offset value
    //! @return     reference to this object
    NArrayIterator<T, N>& operator++ ()
    {
      ++m_pos;
      return *this;
    }

    //! @brief      decrements the data offset value
    //! @return     reference to this object
    NArrayIterator<T, N>& operator-- ()
    {
      --m_pos;
      return *this;
    }

    //! @brief      increments the data offset value
    //! @return     copy of this object before incrementing
    NArrayIterator<T, N> operator++ (int)
    {
      return NArrayIterator<T, N>(*this, m_pos++);
    }

    //! @brief      decrements the data offset value
    //! @return     copy of this object before decrementing
    NArrayIterator<T, N> operator-- (int)
    {
      return NArrayIterator<T, N>(*this, m_pos--);
    }

    //! @brief      the addition of two iterators
    //! @param[in]  iter - the iterator to add
    //! @return     the addition of the two data offset values
    //!
    //! This value doesn't mean much, is only here to pair with operator-
    difference_type operator+ (const NArrayIterator<T, N>& iter) const
    {
      return m_pos + iter.m_pos;
    }

    //! @brief      the difference of two iterators
    //! @param[in]  iter - the iterator to diff
    //! @return     the difference of the two data offset values
    difference_type operator- (const NArrayIterator<T, N>& iter) const
    {
      return m_pos - iter.m_pos;
    }

    //! @brief      addition operator
    //! @param[in]  pos - the offset to add
    //! @return     this iterator plus the offset
    NArrayIterator<T, N> operator+ (const ptrdiff_t pos)
    {
      return NArrayIterator<T, N>(*this, m_pos + pos);
    }

    //! @brief      subtraction operator
    //! @param[in]  pos - the offset to subtract
    //! @return     this iterator minus the offset
    NArrayIterator<T, N> operator- (const ptrdiff_t pos)
    {
      return NArrayIterator<T, N>(*this, m_pos - pos);
    }

    //! @brief      gets the position of the iterator
    //! @return     point corresponding to the position in the NArray currently
    //!             pointing to
    Point<N> position() const
    {
      return idx2pos_(sizes_, m_pos);
    }

  private:
    NArrayDataRef<type> data_;
    type* base_;
    Point<N> sizes_;
    Point<N> steps_;
    pos_t m_pos;

    //! @brief      gets the pointer at the data offset
    //! @param[in]  pos - data offset value
    //! @return     pointer to data at the offset
    pointer at_(pos_t pos) const
    {
      Point<N> loc = idx2pos_(sizes_, m_pos);
      pointer ptr = base_;
      for (std::size_t i = 0; i < N; ++i)
        ptr += loc[i] * steps_[i];

      return ptr;
    }

  }; // class NArrayIterator

} // namespace wilt

#endif // !WILT_NARRAYITERATOR_HPP
