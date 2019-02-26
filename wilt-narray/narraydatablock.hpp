////////////////////////////////////////////////////////////////////////////////
// FILE: narraydatablock.hpp
// DATE: 2018-11-12
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Defines a shared-resource class for holding N-dimensional data

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

#ifndef WILT_NARRAYDATABLOCK_HPP
#define WILT_NARRAYDATABLOCK_HPP

#include <memory>
// - std::allocator
// - std::shared_ptr
// - std::enable_shared_from_this
#include <cstddef>
// - std::size_t
#include <type_traits>
// - std::is_trivially_default_constructible
// - std::is_trivially_destructible

namespace wilt
{
  enum NArrayDataAcquireType
  {
    ASSUME,
    COPY,
    REFERENCE
  };

  template <class T, class A = std::allocator<T>>
  class NArrayDataBlock : public std::enable_shared_from_this<NArrayDataBlock<T, A>>
  {
  private:
    ////////////////////////////////////////////////////////////////////////////
    // PRIVATE MEMBERS
    ////////////////////////////////////////////////////////////////////////////

    T* data_;
    std::size_t size_;
    A alloc_;
    bool owned_;

  public:
    ////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    ////////////////////////////////////////////////////////////////////////////

    NArrayDataBlock()
      : data_(nullptr),
        size_(0),
        alloc_(),
        owned_(true)
    {

    }

    NArrayDataBlock(std::size_t size)
      : data_(nullptr),
        size_(size),
        alloc_(),
        owned_(true)
    {
      data_ = alloc_.allocate(size);
      if (!std::is_trivially_default_constructible<T>::value)
        for (std::size_t i = 0; i < size; ++i)
          alloc_.construct(data_ + i);
    }

    NArrayDataBlock(std::size_t size, const T& val)
      : data_(nullptr),
        size_(size),
        alloc_(),
        owned_(true)
    {
      data_ = alloc_.allocate(size);
      for (std::size_t i = 0; i < size; ++i)
        alloc_.construct(data_ + i, val);
    }

    NArrayDataBlock(std::size_t size, T* data, NArrayDataAcquireType atype)
      : data_(nullptr),
        size_(size),
        alloc_(),
        owned_(true)
    {
      switch (atype)
      {
      case wilt::ASSUME:
        data_ = data;
        break;
      case wilt::COPY:
        data_ = alloc_.allocate(size);
        for (size_t i = 0; i < size; ++i)
          alloc_.construct(data_ + i, data[i]);
        break;
      case wilt::REFERENCE:
        data_ = data;
        owned_ = false;
        break;
      }
    }

    template <class Generator>
    NArrayDataBlock(std::size_t size, Generator gen)
      : data_(nullptr),
        size_(size),
        alloc_(),
        owned_(true)
    {
      data_ = alloc_.allocate(size);
      for (std::size_t i = 0; i < size; ++i)
        alloc_.construct(data_ + i, gen());
    }

    template <class Iterator>
    NArrayDataBlock(std::size_t size, Iterator first, Iterator last)
      : data_(nullptr),
        size_(size),
        alloc_(),
        owned_(true)
    {
      data_ = alloc_.allocate(size);

      std::size_t i = 0;
      for (; i < size && first != last; ++i, ++first)
        alloc_.construct(data_ + i, *first);

      if (i != size)
        for (; i < size; ++i)
          alloc_.construct(data_ + i);
    }

    ~NArrayDataBlock()
    {
      if (data_ && owned_)
      {
        if (!std::is_trivially_destructible<T>::value)
          for (std::size_t i = 0; i < size_; ++i)
            alloc_.destroy(data_ + i);
        alloc_.deallocate(data_, size_);
      }
    }

  public:
    ////////////////////////////////////////////////////////////////////////////
    // ACCESS FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<T> data() const
    {
      return std::shared_ptr<T>(shared_from_this(), data_);
    }

  }; // class NArrayDataBlock

} // namespace wilt

#endif // !WILT_NARRAYDATABLOCK_HPP
