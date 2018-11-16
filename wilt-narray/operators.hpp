////////////////////////////////////////////////////////////////////////////////
// FILE: operators.hpp
// DATE: 2014-07-30
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Defines operators for an N-dimensional templated array class

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

#ifndef WILT_OPERATORS_HPP
#define WILT_OPERATORS_HPP

#include "util.h"
#include "narray.hpp"

namespace wilt
{
  // helper structures for applying move semantics to operators
  template <class Ret, class T, class U, dim_t N>
  struct move_op
  {
    template <class Operator>
    static NArray<Ret, N> none(const NArray<T, N>& lhs, const NArray<U, N>& rhs, Operator op)
    {
      NArray<Ret, N> ret(lhs.dims());
      binaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<Ret, N> left(NArray<T, N>&& lhs, const NArray<U, N>& rhs, Operator op)
    {
      return none(lhs, rhs, op);
    }
    template <class Operator>
    static NArray<Ret, N> right(const NArray<T, N>& lhs, NArray<U, N>&& rhs, Operator op)
    {
      return none(lhs, rhs, op);
    }
  
    template <class Operator>
    static NArray<Ret, N> none(const NArray<T, N>& lhs, Operator op)
    {
      NArray<Ret, N> ret(lhs.dims());
      unaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<Ret, N> none(const NArray<U, N>& rhs, Operator op)
    {
      NArray<Ret, N> ret(rhs.dims());
      unaryOp2_(
        ret.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<Ret, N> left(NArray<T, N>&& lhs, Operator op)
    {
      return none(lhs, op);
    }
    template <class Operator>
    static NArray<Ret, N> right(NArray<U, N>&& rhs, Operator op)
    {
      return none(rhs, op);
    }
  };

  template <class T, class U, dim_t N>
  struct move_op<T, T, U, N>
  {
    template <class Operator>
    static NArray<T, N> none(const NArray<T, N>& lhs, const NArray<U, N>& rhs, Operator op)
    {
      NArray<T, N> ret(lhs.dims());
      binaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> left(NArray<T, N>&& lhs, const NArray<U, N>& rhs, Operator op)
    {
      NArray<T, N> ret(std::forward<NArray<U, N>>(lhs));
      binaryOp2_(
        ret.basePtr_(), ret.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), ret.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> right(const NArray<T, N>& lhs, NArray<U, N>&& rhs, Operator op)
    {
      return none(lhs, rhs, op);
    }
  
    template <class Operator>
    static NArray<T, N> none(const NArray<T, N>& lhs, Operator op)
    {
      NArray<T, N> ret(lhs.dims());
      unaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> none(const NArray<U, N>& rhs, Operator op)
    {
      NArray<T, N> ret(rhs.dims());
      unaryOp2_(
        ret.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> left(NArray<T, N>&& lhs, Operator op)
    {
      NArray<T, N> ret(std::forward<wilt::NArray<T, N>>(lhs)); 
      unaryOp2_(
        ret.basePtr_(), ret.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), ret.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> right(NArray<U, N>&& rhs, Operator op)
    {
      return none(rhs, op);
    }
  };

  template <class T, class U, dim_t N>
  struct move_op<U, T, U, N>
  {
    template <class Operator>
    static NArray<U, N> none(const NArray<T, N>& lhs, const NArray<U, N>& rhs, Operator op)
    {
      NArray<U, N> ret(lhs.dims());
      binaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<U, N> left(NArray<T, N>&& lhs, const NArray<U, N>& rhs, Operator op)
    {
      return none(lhs, rhs, op);
    }
    template <class Operator>
    static NArray<U, N> right(const NArray<T, N>& lhs, NArray<U, N>&& rhs, Operator op)
    {
      NArray<U, N> ret(std::forward<NArray<U, N>>(rhs));
      binaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), ret.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), ret.stepPtr_(), op, N);
      return ret;
    }
  
    template <class Operator>
    static NArray<U, N> none(const NArray<T, N>& lhs, Operator op)
    {
      NArray<U, N> ret(lhs.dims());
      unaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<U, N> none(const NArray<U, N>& rhs, Operator op)
    {
      NArray<U, N> ret(rhs.dims());
      unaryOp2_(
        ret.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<U, N> left(NArray<T, N>&& lhs, Operator op)
    {
      return none(lhs, op);
    }
    template <class Operator>
    static NArray<U, N> right(NArray<U, N>&& rhs, Operator op)
    {
      NArray<U, N> ret(std::forward<wilt::NArray<U, N>>(rhs)); 
      unaryOp2_(
        ret.basePtr_(), ret.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), ret.stepPtr_(), op, N);
      return ret;
    }
  };

  template <class T, dim_t N>
  struct move_op<T, T, T, N>
  {
    template <class Operator>
    static NArray<T, N> none(const NArray<T, N>& lhs, const NArray<T, N>& rhs, Operator op)
    {
      NArray<T, N> ret(lhs.dims());
      binaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> left(NArray<T, N>&& lhs, const NArray<T, N>& rhs, Operator op)
    {
      NArray<T, N> ret(std::forward<wilt::NArray<T, N>>(lhs));
      binaryOp2_(
        ret.basePtr_(), ret.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), ret.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> right(const NArray<T, N>& lhs, NArray<T, N>&& rhs, Operator op)
    {
      NArray<T, N> ret(std::forward<wilt::NArray<T, N>>(rhs));
      binaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), ret.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), ret.stepPtr_(), op, N);
      return ret;
    }
  
    template <class Operator>
    static NArray<T, N> none(const NArray<T, N>& lhs, Operator op)
    {
      NArray<T, N> ret(lhs.dims());
      unaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> left(NArray<T, N>&& lhs, Operator op)
    {
      NArray<T, N> ret(std::forward<wilt::NArray<T, N>>(lhs));
      unaryOp2_(
        ret.basePtr_(), ret.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), ret.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> right(NArray<T, N>&& rhs, Operator op)
    {
      NArray<T, N> ret(std::forward<NArray<T, N>>(rhs)); 
      unaryOp2_(
        ret.basePtr_(), ret.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), ret.stepPtr_(), op, N);
      return ret;
    }
  };

  template <class Ret, class T, dim_t N>
  struct move_op<Ret, T, T, N>
  {
    template <class Operator>
    static NArray<Ret, N> none(const NArray<T, N>& lhs, const NArray<T, N>& rhs, Operator op)
    {
      NArray<Ret, N> ret(lhs.dims());
      binaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), rhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), rhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<Ret, N> left(NArray<T, N>&& lhs, const NArray<T, N>& rhs, Operator op)
    {
      return none(lhs, rhs, op);
    }
    template <class Operator>
    static NArray<Ret, N> right(const NArray<T, N>& lhs, NArray<T, N>&& rhs, Operator op)
    {
      return none(lhs, rhs, op);
    }
  
    template <class Operator>
    static NArray<Ret, N> none(const NArray<T, N>& lhs, Operator op)
    {
      NArray<Ret, N> ret(lhs.dims());
      unaryOp2_(
        ret.basePtr_(), lhs.basePtr_(), ret.dimsPtr_(), 
        ret.stepPtr_(), lhs.stepPtr_(), op, N);
      return ret;
    }
    template <class Operator>
    static NArray<T, N> left(NArray<T, N>&& lhs, Operator op)
    {
      return none(lhs, op);
    }
    template <class Operator>
    static NArray<T, N> right(NArray<T, N>&& rhs, Operator op)
    {
      return none(rhs, op);
    }
  };

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> add(const NArray<T, N>& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t + u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("add(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> add(NArray<T, N>&& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t + u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("add(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> add(const NArray<T, N>& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t + u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("add(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> add(NArray<T, N>&& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t + u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("add(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> add(const NArray<T, N>& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t + rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> add(NArray<T, N>&& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t + rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<wilt::NArray<T, N>>(lhs), op);
    
    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> add(const T& lhs, const NArray<U, N>& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs + u; };

    if (rhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> add(const T& lhs, NArray<U, N>&& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs + u; };

    if (rhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> sub(const NArray<T, N>& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t - u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("sub(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> sub(NArray<T, N>&& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t - u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("sub(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> sub(const NArray<T, N>& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t - u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("sub(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> sub(NArray<T, N>&& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t - u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("sub(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> sub(const NArray<T, N>& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t - rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> sub(NArray<T, N>&& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t - rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), op);
    
    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> sub(const T& lhs, const NArray<U, N>& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs - u; };

    if (rhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> sub(const T& lhs, NArray<U, N>&& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs - u; };

    if (rhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(std::forward<wilt::NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mul(const NArray<T, N>& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t * u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("mul(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mul(NArray<T, N>&& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t * u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("mul(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mul(const NArray<T, N>& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t * u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("mul(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mul(NArray<T, N>&& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t * u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("mul(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mul(const NArray<T, N>& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t * rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mul(NArray<T, N>&& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t * rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), op);
    
    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mul(const T& lhs, const NArray<U, N>& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs * u; };

    if (rhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mul(const T& lhs, NArray<U, N>&& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs * u; };

    if (rhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> div(const NArray<T, N>& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t / u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("div(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> div(NArray<T, N>&& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t / u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("div(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> div(const NArray<T, N>& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t / u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("div(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> div(NArray<T, N>&& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t / u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("div(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> div(const NArray<T, N>& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t / rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> div(NArray<T, N>&& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t / rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<wilt::NArray<T, N>>(lhs), op);
    
    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> div(const T& lhs, const NArray<U, N>& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs / u; };

    if (rhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> div(const T& lhs, NArray<U, N>&& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs / u; };

    if (rhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mod(const NArray<T, N>& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t % u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("mod(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mod(NArray<T, N>&& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t % u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("mod(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mod(const NArray<T, N>& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t % u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("mod(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mod(NArray<T, N>&& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t % u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("mod(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mod(const NArray<T, N>& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t % rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mod(NArray<T, N>&& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t % rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), op);
    
    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mod(const T& lhs, const NArray<U, N>& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs % u; };

    if (rhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> mod(const T& lhs, NArray<U, N>&& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs % u; };

    if (rhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_and(const NArray<T, N>& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t & u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_and(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_and(NArray<T, N>&& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t & u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_and(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_and(const NArray<T, N>& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t & u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_and(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_and(NArray<T, N>&& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t & u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_and(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_and(const NArray<T, N>& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t & rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_and(NArray<T, N>&& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t & rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), op);
    
    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_and(const T& lhs, const NArray<U, N>& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs & u; };

    if (rhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_and(const T& lhs, NArray<U, N>&& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs & u; };

    if (rhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_or(const NArray<T, N>& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t | u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_or(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_or(NArray<T, N>&& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t | u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_or(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_or(const NArray<T, N>& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t | u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_or(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_or(NArray<T, N>&& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t | u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_or(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_or(const NArray<T, N>& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t | rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_or(NArray<T, N>&& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t | rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), op);
    
    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_or(const T& lhs, const NArray<U, N>& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs | u; };

    if (rhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_or(const T& lhs, NArray<U, N>&& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs | u; };

    if (rhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_xor(const NArray<T, N>& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t ^ u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_xor(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_xor(NArray<T, N>&& lhs, const NArray<U, N>& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t ^ u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_xor(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_xor(const NArray<T, N>& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t ^ u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_xor(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_xor(NArray<T, N>&& lhs, NArray<U, N>&& rhs)
  {
    auto op = [](Ret& r, const T& t, const U& u){ r = t ^ u; };

    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("bit_xor(): dimensions must match");
    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), rhs, op);
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(lhs, std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(lhs, rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_xor(const NArray<T, N>& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t ^ rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_xor(NArray<T, N>&& lhs, const U& rhs)
  {
    auto op = [&rhs](Ret& r, const T& t){ r = t ^ rhs; };

    if (lhs.empty())
      return NArray<Ret, N>();
    if (lhs.isUnique() && lhs.isContinuous() && lhs.isAligned())
      return move_op<Ret, T, U, N>::left(std::forward<NArray<T, N>>(lhs), op);
    
    return move_op<Ret, T, U, N>::none(lhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_xor(const T& lhs, const NArray<U, N>& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs ^ u; };

    if (rhs.empty())
      return NArray<Ret, N>();

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  template <class Ret, class T, class U, dim_t N>
  NArray<Ret, N> bit_xor(const T& lhs, NArray<U, N>&& rhs)
  {
    auto op = [&lhs](Ret& r, const U& u){ r = lhs ^ u; };

    if (rhs.empty())
      return NArray<Ret, N>();
    if (rhs.isUnique() && rhs.isContinuous() && rhs.isAligned())
      return move_op<Ret, T, U, N>::right(std::forward<NArray<U, N>>(rhs), op);

    return move_op<Ret, T, U, N>::none(rhs, op);
  }

  enum CMP
  {
    LT, GT, EQ,
    GE, LE, NE
  }; // enum CMP

  template <class T, class U, dim_t N>
  NArray<uint8_t, N> compare(const NArray<T, N>& lhs, const NArray<U, N>& rhs, CMP mode)
  {
    if (lhs.dims() != rhs.dims())
      throw std::invalid_argument("compare(): dimensions must match");
    if (lhs.empty())
      return NArray<uint8_t, N>();

    switch (mode)
    {
    case CMP::LT:
      return move_op<uint8_t, T, U, N>::none(lhs, rhs, 
        [](uint8_t& b, const T& t, const U& u){b = (t < u)?255:0;});

    case CMP::GT:
      return move_op<uint8_t, T, U, N>::none(lhs, rhs, 
        [](uint8_t& b, const T& t, const U& u){b = (t > u)?255:0;});

    case CMP::EQ:
      return move_op<uint8_t, T, U, N>::none(lhs, rhs, 
        [](uint8_t& b, const T& t, const U& u){b = (t == u)?255:0;});

    case CMP::GE:
      return move_op<uint8_t, T, U, N>::none(lhs, rhs, 
        [](uint8_t& b, const T& t, const U& u){b = (t >= u)?255:0;});

    case CMP::LE:
      return move_op<uint8_t, T, U, N>::none(lhs, rhs, 
        [](uint8_t& b, const T& t, const U& u){b = (t <= u)?255:0;});

    case CMP::NE:
      return move_op<uint8_t, T, U, N>::none(lhs, rhs, 
        [](uint8_t& b, const T& t, const U& u){b = (t != u)?255:0;});
    }
  }

  template <class T, class U, dim_t N>
  NArray<uint8_t, N> compare(const NArray<T, N>& lhs, const U& rhs, CMP mode)
  {
    if (lhs.empty())
      return NArray<uint8_t, N>();

    switch (mode)
    {
    case CMP::LT:
      return move_op<uint8_t, T, U, N>::none(lhs, 
        [&rhs](uint8_t& b, const T& t){b = (t < rhs)?255:0;});

    case CMP::GT:
      return move_op<uint8_t, T, U, N>::none(lhs, 
        [&rhs](uint8_t& b, const T& t){b = (t > rhs)?255:0;});

    case CMP::EQ:
      return move_op<uint8_t, T, U, N>::none(lhs, 
        [&rhs](uint8_t& b, const T& t){b = (t == rhs)?255:0;});

    case CMP::GE:
      return move_op<uint8_t, T, U, N>::none(lhs, 
        [&rhs](uint8_t& b, const T& t){b = (t >= rhs)?255:0;});

    case CMP::LE:
      return move_op<uint8_t, T, U, N>::none(lhs, 
        [&rhs](uint8_t& b, const T& t){b = (t <= rhs)?255:0;});

    case CMP::NE:
      return move_op<uint8_t, T, U, N>::none(lhs, 
        [&rhs](uint8_t& b, const T& t){b = (t != rhs)?255:0;});
    }
  }

  template <class T, class U, dim_t N>
  NArray<uint8_t, N> compare(const T& lhs, const NArray<U, N>& rhs, CMP mode)
  {
    if (rhs.empty())
      return NArray<uint8_t, N>();

    switch (mode)
    {
    case CMP::LT:
      return move_op<uint8_t, T, U, N>::none(rhs, 
        [&lhs](uint8_t& b, const U& u){b = (lhs < u)?255:0;});

    case CMP::GT:
      return move_op<uint8_t, T, U, N>::none(rhs, 
        [&lhs](uint8_t& b, const U& u){b = (lhs > u)?255:0;});

    case CMP::EQ:
      return move_op<uint8_t, T, U, N>::none(rhs, 
        [&lhs](uint8_t& b, const U& u){b = (lhs == u)?255:0;});

    case CMP::GE:
      return move_op<uint8_t, T, U, N>::none(rhs, 
        [&lhs](uint8_t& b, const U& u){b = (lhs >= u)?255:0;});

    case CMP::LE:
      return move_op<uint8_t, T, U, N>::none(rhs, 
        [&lhs](uint8_t& b, const U& u){b = (lhs <= u)?255:0;});

    case CMP::NE:
      return move_op<uint8_t, T, U, N>::none(rhs, 
        [&lhs](uint8_t& b, const U& u){b = (lhs != u)?255:0;});
    }
  }

} // namespace wilt

template <class T, class U, dim_t N>
wilt::NArray<typename wilt::add_ret<T, U>::type, N> operator+ (const wilt::NArray<T, N>& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::add<typename wilt::add_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::add_ret<T, U>::type, N> operator+ (wilt::NArray<T, N>&& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::add<typename wilt::add_ret<T, U>::type>(std::forward<NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::add_ret<T, U>::type, N> operator+ (const wilt::NArray<T, N>& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::add<typename wilt::add_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::add_ret<T, U>::type, N> operator+ (wilt::NArray<T, N>&& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::add<typename wilt::add_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::add_ret<T, U>::type, N> operator+ (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::add<typename wilt::add_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::add_ret<T, U>::type, N> operator+ (wilt::NArray<T, N>&& lhs, const U& rhs)
{
  return wilt::add<typename wilt::add_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::add_ret<T, U>::type, N> operator+ (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::add<typename wilt::add_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::add_ret<T, U>::type, N> operator+ (const T& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::add<typename wilt::add_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}

template <class T, class U, dim_t N>
wilt::NArray<typename wilt::sub_ret<T, U>::type, N> operator- (const wilt::NArray<T, N>& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::sub<typename wilt::sub_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::sub_ret<T, U>::type, N> operator- (wilt::NArray<T, N>&& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::sub<typename wilt::sub_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::sub_ret<T, U>::type, N> operator- (const wilt::NArray<T, N>& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::sub<typename wilt::sub_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::sub_ret<T, U>::type, N> operator- (wilt::NArray<T, N>&& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::sub<typename wilt::sub_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::sub_ret<T, U>::type, N> operator- (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::sub<typename wilt::sub_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::sub_ret<T, U>::type, N> operator- (wilt::NArray<T, N>&& lhs, const U& rhs)
{
  return wilt::sub<typename wilt::sub_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::sub_ret<T, U>::type, N> operator- (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::sub<typename wilt::sub_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::sub_ret<T, U>::type, N> operator- (const T& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::sub<typename wilt::sub_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}

template <class T, class U, dim_t N>
wilt::NArray<typename wilt::mul_ret<T, U>::type, N> operator* (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::mul<typename wilt::mul_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::mul_ret<T, U>::type, N> operator* (wilt::NArray<T, N>&& lhs, const U& rhs)
{
  return wilt::mul<typename wilt::mul_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::mul_ret<T, U>::type, N> operator* (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::mul<typename wilt::mul_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::mul_ret<T, U>::type, N> operator* (const T& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::mul<typename wilt::mul_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}

template <class T, class U, dim_t N>
wilt::NArray<typename wilt::div_ret<T, U>::type, N> operator/ (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::div<typename wilt::div_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::div_ret<T, U>::type, N> operator/ (wilt::NArray<T, N>&& lhs, const U& rhs)
{
  return wilt::div<typename wilt::div_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::div_ret<T, U>::type, N> operator/ (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::div<typename wilt::div_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::div_ret<T, U>::type, N> operator/ (const T& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::div<typename wilt::div_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}

template <class T, class U, dim_t N>
wilt::NArray<typename wilt::mod_ret<T, U>::type, N> operator% (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::mod<typename wilt::mod_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::mod_ret<T, U>::type, N> operator% (wilt::NArray<T, N>&& lhs, const U& rhs)
{
  return wilt::mod<typename wilt::mod_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::mod_ret<T, U>::type, N> operator% (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::mod<typename wilt::mod_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::mod_ret<T, U>::type, N> operator% (const T& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::mod<typename wilt::mod_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}

template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator< (const wilt::NArray<T, N>& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::LT);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator< (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::LT);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator< (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::LT);
}

template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator> (const wilt::NArray<T, N>& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::GT);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator> (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::GT);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator> (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::GT);
}

template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator<= (const wilt::NArray<T, N>& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::LE);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator<= (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::LE);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator<= (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::LE);
}

template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator>= (const wilt::NArray<T, N>& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::GE);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator>= (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::GE);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator>= (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::GE);
}

template <class T, dim_t N>
bool operator== (const wilt::NArray<T, N>& lhs, const wilt::NArray<T, N>& rhs)
{
  return allOf_(
    lhs.basePtr_(), rhs.basePtr_(), 
    lhs.dimsPtr_(), lhs.stepPtr_(), rhs.stepPtr_(), 
    [](const T& a, const T& b){return a == b;}, N);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator== (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::EQ);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator== (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::EQ);
}

template <class T, dim_t N>
bool operator!= (const wilt::NArray<T, N>& lhs, const wilt::NArray<T, N>& rhs)
{
  return !(lhs == rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator!= (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::NE);
}
template <class T, class U, dim_t N>
wilt::NArray<uint8_t, N> operator!= (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::compare(lhs, rhs, wilt::CMP::NE);
}

template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_and_ret<T, U>::type, N> operator& (const wilt::NArray<T, N>& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::bit_and<typename wilt::bit_and_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_and_ret<T, U>::type, N> operator& (wilt::NArray<T, N>&& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::bit_and<typename wilt::bit_and_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_and_ret<T, U>::type, N> operator& (const wilt::NArray<T, N>& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::bit_and<typename wilt::bit_and_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_and_ret<T, U>::type, N> operator& (wilt::NArray<T, N>&& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::bit_and<typename wilt::bit_and_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_and_ret<T, U>::type, N> operator& (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::bit_and<typename wilt::bit_and_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_and_ret<T, U>::type, N> operator& (wilt::NArray<T, N>&& lhs, const U& rhs)
{
  return wilt::bit_and<typename wilt::bit_and_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_and_ret<T, U>::type, N> operator& (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::bit_and<typename wilt::bit_and_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_and_ret<T, U>::type, N> operator& (const T& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::bit_and<typename wilt::bit_and_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}

template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_or_ret<T, U>::type, N> operator| (const wilt::NArray<T, N>& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::bit_or<typename wilt::bit_or_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_or_ret<T, U>::type, N> operator| (wilt::NArray<T, N>&& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::bit_or<typename wilt::bit_or_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_or_ret<T, U>::type, N> operator| (const wilt::NArray<T, N>& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::bit_or<typename wilt::bit_or_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_or_ret<T, U>::type, N> operator| (wilt::NArray<T, N>&& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::bit_or<typename wilt::bit_or_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_or_ret<T, U>::type, N> operator| (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::bit_or<typename wilt::bit_or_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_or_ret<T, U>::type, N> operator| (wilt::NArray<T, N>&& lhs, const U& rhs)
{
  return wilt::bit_or<typename wilt::bit_or_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_or_ret<T, U>::type, N> operator| (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::bit_or<typename wilt::bit_or_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_or_ret<T, U>::type, N> operator| (const T& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::bit_or<typename wilt::bit_or_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}

template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_xor_ret<T, U>::type, N> operator^ (const wilt::NArray<T, N>& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::bit_xor<typename wilt::bit_xor_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_xor_ret<T, U>::type, N> operator^ (wilt::NArray<T, N>&& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::bit_xor<typename wilt::bit_xor_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_xor_ret<T, U>::type, N> operator^ (const wilt::NArray<T, N>& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::bit_xor<typename wilt::bit_xor_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_xor_ret<T, U>::type, N> operator^ (wilt::NArray<T, N>&& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::bit_xor<typename wilt::bit_xor_ret<T, U>::type>(std::forward<wilt::NArray<T, N>>(lhs), std::forward<wilt::NArray<U, N>>(rhs));
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_xor_ret<T, U>::type, N> operator^ (const wilt::NArray<T, N>& lhs, const U& rhs)
{
  return wilt::bit_xor<typename wilt::bit_xor_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_xor_ret<T, U>::type, N> operator^ (wilt::NArray<T, N>&& lhs, const U& rhs)
{
  return wilt::bit_xor<typename wilt::bit_xor_ret<T, U>::type>(std::forward<wilt::NArray<U, N>>(lhs), rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_xor_ret<T, U>::type, N> operator^ (const T& lhs, const wilt::NArray<U, N>& rhs)
{
  return wilt::bit_xor<typename wilt::bit_xor_ret<T, U>::type>(lhs, rhs);
}
template <class T, class U, dim_t N>
wilt::NArray<typename wilt::bit_xor_ret<T, U>::type, N> operator^ (const T& lhs, wilt::NArray<U, N>&& rhs)
{
  return wilt::bit_xor<typename wilt::bit_xor_ret<T, U>::type>(lhs, std::forward<wilt::NArray<U, N>>(rhs));
}

#endif // !WILT_OPERATORS_HPP
