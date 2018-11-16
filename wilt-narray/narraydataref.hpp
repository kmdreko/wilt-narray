////////////////////////////////////////////////////////////////////////////////
// FILE: narraydataref.hpp
// DATE: 2018-11-12
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Defines a shared-resource class for holding N-dimensional data

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

#ifndef WILT_NARRAYDATAREF_HPP
#define WILT_NARRAYDATAREF_HPP

#include <memory>
// - std::allocator
// - std::shared_ptr
// - std::make_shared
#include <cstddef>
// - std::size_t

namespace wilt
{
  enum PTR
  {
    ASSUME,
    COPY,
    REF
  };

  template <class T, class A = std::allocator<T>>
  class NArrayDataRef
  {
  public:
    class Node
    {
    public:
      T* data;
      std::size_t size;
      A alloc;
      bool owned;

      Node()
        : data(nullptr),
        size(0),
        alloc(),
        owned(true)
      {

      }

      Node(std::size_t size_)
        : data(nullptr),
        size(size_),
        alloc(),
        owned(true)
      {
        data = alloc.allocate(size_);
        if (!raw_t<T>::value())
          for (size_t i = 0; i < size_; ++i)
            alloc.construct(data + i);
      }

      Node(std::size_t size_, const T& val)
        : data(nullptr),
        size(size_),
        alloc(),
        owned(true)
      {
        data = alloc.allocate(size_);
        for (size_t i = 0; i < size_; ++i)
          alloc.construct(data + i, val);
      }

      Node(std::size_t size_, T* _data, PTR ltype)
        : data(nullptr),
        size(size_),
        alloc(),
        owned(true)
      {
        switch (ltype)
        {
        case PTR::ASSUME:
          data = _data;
          break;
        case PTR::COPY:
          data = alloc.allocate(size_);
          for (size_t i = 0; i < size_; ++i)
            alloc.construct(data + i, _data[i]);
          break;
        case PTR::REF:
          data = _data;
          owned = false;
          break;
        }
      }

      template <class Generator>
      Node(std::size_t size_, Generator gen)
        : data(nullptr),
        size(size_),
        alloc(),
        owned(true)
      {
        data = alloc.allocate(size_);
        for (size_t i = 0; i < size_; ++i)
          alloc.construct(data + i, gen());
      }

      ~Node()
      {
        if (data && owned)
        {
          if (!raw_t<T>::value())
            for (std::size_t i = 0; i < size; ++i)
              alloc.destroy(data + i);
          alloc.deallocate(data, size);
        }
      }

    }; // class Node

    NArrayDataRef()
      : m_node()
    {

    }

    NArrayDataRef(pos_t size)
      : m_node()
    {
      if (size > 0)
        m_node = std::make_shared<Node>(size);
    }

    NArrayDataRef(pos_t size, const T& val)
      : m_node()
    {
      if (size > 0)
        m_node = std::make_shared<Node>(size, val);
    }

    NArrayDataRef(pos_t size, T* ptr, PTR ltype)
      : m_node()
    {
      if (size > 0)
        m_node = std::make_shared<Node>(size, ptr, ltype);
    }

    template <class Generator>
    NArrayDataRef(pos_t size, Generator gen)
      : m_node()
    {
      if (size > 0)
        m_node = std::make_shared<Node>(size, gen);
    }


    NArrayDataRef(const NArrayDataRef<T, A>& header)
      : m_node(header.m_node)
    {

    }

    ~NArrayDataRef()
    {

    }

    NArrayDataRef<T, A>& operator= (const NArrayDataRef<T, A>& header)
    {
      m_node = header.m_node;

      return *this;
    }

    void clear()
    {
      m_node.reset();
    }

    bool unique() const
    {
      return m_node.unique();
    }

    Node& operator* () const
    {
      return *m_node.get();
    }
    Node* operator-> () const
    {
      return m_node.get();
    }

    Node* ptr_() const
    {
      return m_node.get();
    }

  private:
    std::shared_ptr<Node> m_node;

  }; // class NArrayDataRef

} // namespace wilt

#endif // !WILT_NARRAYDATAREF_HPP
