////////////////////////////////////////////////////////////////////////////////
// FILE: filters.hpp
// DATE: 2014-08-20
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Defines various NArray filtering functions

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

#ifndef WILT_FILTERS_HPP
#define WILT_FILTERS_HPP

#include <cstring>
// - memcpy

#include "util.h"
#include "narray.hpp"
#include "point.hpp"

WILT_BEGIN

  template <class T, dim_t N, class U, dim_t M, class Operator>
  void _compress(NArray<T, N>& dst, const NArray<U, M>& src, Point<N> pos, pos_t n, Operator op)
  {
    if (n == N-1)
    {
      for (int i = 0; i < dst.length(n); ++i)
      {
        pos[n] = i;
        dst.at(pos) = op(src.arrayAt(pos));
      }
    }
    else
    {
      for (int i = 0; i < dst.length(n); ++i)
      {
        pos[n] = i;
        _compress(dst, src, pos, n+1, op);
      }
    }
  }

  template <class T, class U, dim_t N, class Operator>
  void _filter(NArray<T, N>& dst, const NArray<U, N>& src, Point<N> pos, Point<N> size, pos_t n, Operator op, const BorderType<U>& border)
  {
    if (n == N-1)
    {
      for (pos_t i = 0; i < dst.length(n); ++i)
      {
        pos[n] = i;
        if (border.type() == Border::NONE)
          dst.at(pos) = op(NArrayWindow<U, N>(src, size, pos, border));
        else
          dst.at(pos) = op(NArrayWindow<U, N>(src, size, pos - size / 2, border));
      }
    }
    else
    {
      for (pos_t i = 0; i < dst.length(n); ++i)
      {
        pos[n] = i;
        _filter(dst, src, pos, size, n+1, op, border);
      }
    }
  }

  template <class T, class U, class Operator>
  void _filterReplicate(
      T* dst,             // destination array pointer
      const U* src,       // source array pointer
      const pos_t* dims,  // inverted dimensions array pointer
      const pos_t* dstep, // inverted destination step array pointer
      const pos_t* sstep, // inverted source step array pointer
      const pos_t* size,  // inverted filter size array pointer
      pos_t* idx,         // filter index array pointer
      pos_t pos,          // position of filled idx segment
      pos_t length,       // length of filled idx segment
      Operator op,        // final filter operator
      dim_t n)            // current dimension / recursion counter
  {
    // invert size
    // invert sstep into istep

    int width = size[0] / 2;

    // copy with steps for upper area
    for (int j = 1; j <= width; ++j)
      for (int k = 0; k < length; k++)
        idx[pos+j*length+k] = idx[pos+k] + sstep[0] * j;

    for (int i = 0; i < dims[0]; ++i, dst += dstep[0], src += sstep[0])
    {
      if (i <= width)
      {
        // adjust lower area
        if (i > 0)
          for (int k = 0; k < length; ++k)
            idx[pos-i*length+k] = idx[pos+k] - sstep[0] * i;
        for (int k = -width; k < -i; ++k)
          memcpy(idx+pos+k*length, idx+pos-i*length, length*sizeof(pos_t));
      }
      if (i >= dims[0] - width)
      {
        // adjust upper area
        int t_i = dims[0] - i;
        for (int k = t_i; k <= width; ++k)
          memcpy(idx+pos+k*length, idx+pos+(t_i-1)*length, length*sizeof(pos_t));
      }

      if (n == 1)
        dst[0] = op(src, idx, length*size[0]);
      else
        _filterReplicate(dst, src, dims-1, dstep-1, sstep-1, size-1, idx, pos-width*length, length*size[0], op, n-1);
    }
  }

  template <class T, class U, class Operator>
  void _filterReflect(
      T* dst,             // destination array pointer
      const U* src,       // source array pointer
      const pos_t* dims,  // inverted dimensions array pointer
      const pos_t* dstep, // inverted destination step array pointer
      const pos_t* sstep, // inverted source step array pointer
      const pos_t* size,  // inverted filter size array pointer
      pos_t* idx,         // filter index array pointer
      pos_t pos,          // position of filled idx segment
      pos_t length,       // length of filled idx segment
      Operator op,        // final filter operator
      dim_t n)            // current dimension / recursion counter
  {
    int width = size[0] / 2;

    // copy with steps for upper area
    for (int j = 1; j <= width; ++j)
      for (int k = 0; k < length; k++)
        idx[pos+j*length+k] = idx[pos+k] + sstep[0] * j;

    for (int i = 0; i < dims[0]; ++i, dst += dstep[0], src += sstep[0])
    {
      if (i <= width)
      {
        // adjust lower area
        if (i > 0)
          for (int k = 0; k < length; ++k)
            idx[pos-i*length+k] = idx[pos+k] - sstep[0] * i;
        for (int j = -width; j < -i; ++j)
          for (int k = 0; k < length; ++k)
            idx[pos+j*length+k] = idx[pos+k] + sstep[0] * (-2*i-j-1);
      }
      if (i >= dims[0] - width)
      {
        // adjust upper area
        int t_i = dims[0] - i;
        for (int j = t_i; j <= width; ++j)
          for (int k = 0; k < length; ++k)
            idx[pos+j*length+k] = idx[pos+k] + sstep[0] * (2*t_i-j-1);
      }

      if (n == 1)
        dst[0] = op(src, idx, length*size[0]);
      else
        _filterReflect(dst, src, dims-1, dstep-1, sstep-1, size-1, idx, pos-width*length, length*size[0], op, n-1);
    }
  }

  template <class T, class U, class Operator>
  void _filterReflect101(
      T* dst,             // destination array pointer
      const U* src,       // source array pointer
      const pos_t* dims,  // inverted dimensions array pointer
      const pos_t* dstep, // inverted destination step array pointer
      const pos_t* sstep, // inverted source step array pointer
      const pos_t* size,  // inverted filter size array pointer
      pos_t* idx,         // filter index array pointer
      pos_t pos,          // position of filled idx segment
      pos_t length,       // length of filled idx segment
      Operator op,        // final filter operator
      dim_t n)            // current dimension / recursion counter
  {
    int width = size[0] / 2;

    // copy with steps for upper area
    for (int j = 1; j <= width; ++j)
      for (int k = 0; k < length; k++)
        idx[pos+j*length+k] = idx[pos+k] + sstep[0] * j;

    for (int i = 0; i < dims[0]; ++i, dst += dstep[0], src += sstep[0])
    {
      if (i <= width)
      {
        // adjust lower area
        if (i > 0)
          for (int k = 0; k < length; ++k)
            idx[pos-i*length+k] = idx[pos+k] - sstep[0] * i;
        for (int j = -width; j < -i; ++j)
          for (int k = 0; k < length; ++k)
            idx[pos+j*length+k] = idx[pos+k] + sstep[0] * (-2*i-j);
      }
      if (i >= dims[0] - width)
      {
        // adjust upper area
        int t_i = dims[0] - i;
        for (int j = t_i; j <= width; ++j)
          for (int k = 0; k < length; ++k)
            idx[pos+j*length+k] = idx[pos+k] + sstep[0] * (2*t_i-j-2);
      }

      if (n == 1)
        dst[0] = op(src, idx, length*size[0]);
      else
        _filterReflect101(dst, src, dims-1, dstep-1, sstep-1, size-1, idx, pos-width*length, length*size[0], op, n-1);
    }
  }

  template <class T, class U, class Operator>
  void _filterWrap(
      T* dst,             // destination array pointer
      const U* src,       // source array pointer
      const pos_t* dims,  // inverted dimensions array pointer
      const pos_t* dstep, // inverted destination step array pointer
      const pos_t* sstep, // inverted source step array pointer
      const pos_t* size,  // inverted filter size array pointer
      pos_t* idx,         // filter index array pointer
      pos_t pos,          // position of filled idx segment
      pos_t length,       // length of filled idx segment
      Operator op,        // final filter operator
      dim_t n)            // current dimension / recursion counter
  {
    int width = size[0] / 2;

    // copy with steps for upper area
    for (int j = 1; j <= width; ++j)
      for (int k = 0; k < length; k++)
        idx[pos+j*length+k] = idx[pos+k] + sstep[0] * j;

    for (int i = 0; i < dims[0]; ++i, dst += dstep[0], src += sstep[0])
    {
      if (i <= width)
      {
        // adjust lower area
        if (i > 0)
          for (int k = 0; k < length; ++k)
            idx[pos-i*length+k] = idx[pos+k] - sstep[0] * i;
        for (int j = -width; j < -i; ++j)
          for (int k = 0; k < length; ++k)
            idx[pos+j*length+k] = idx[pos+k] + sstep[0] * (j+dims[0]);
      }
      if (i >= dims[0] - width)
      {
        // adjust upper area
        int t_i = dims[0] - i;
        for (int j = t_i; j <= width; ++j)
          for (int k = 0; k < length; ++k)
            idx[pos+j*length+k] = idx[pos+k] + sstep[0] * (j-dims[0]);
      }

      if (n == 1)
        dst[0] = op(src, idx, length*size[0]);
      else
        _filterWrap(dst, src, dims-1, dstep-1, sstep-1, size-1, idx, pos-width*length, length*size[0], op, n-1);
    }
  }

  template <class T, class U, class Operator>
  void _filterPadded(
      T* dst,             // destination array pointer
      const U* src,       // source array pointer
      const pos_t* dims,  // inverted dimensions array pointer
      const pos_t* dstep, // inverted destination step array pointer
      const pos_t* sstep, // inverted source step array pointer
      const pos_t* size,  // inverted filter size array pointer
      pos_t* idx,         // filter index array pointer
      pos_t pos,          // position of filled idx segment
      pos_t length,       // length of filled idx segment
      Operator op,        // final filter operator
      dim_t n)            // current dimension / recursion counter
  {
    int width = size[0] / 2;

    // solid
    for (int j = -width; j < 0; ++j)
      for (int k = 0; k < length; ++k)
        idx[pos+j*length+k] = WILT_OUTSIDE_ARRAY;

    // copy with steps for upper area
    for (int j = 1; j <= width; ++j)
      for (int k = 0; k < length; k++)
        if (idx[pos+k] == WILT_OUTSIDE_ARRAY)
          idx[pos+j*length+k] = WILT_OUTSIDE_ARRAY;
        else
          idx[pos+j*length+k] = idx[pos+k] + sstep[0] * j;

    for (int i = 0; i < dims[0]; ++i, dst += dstep[0], src += sstep[0])
    {
      if (i <= width)
      {
        // adjust lower area
        if (i > 0)
          for (int k = 0; k < length; ++k)
            if (idx[pos+k] == WILT_OUTSIDE_ARRAY)
              idx[pos-i*length+k] = WILT_OUTSIDE_ARRAY;
            else
              idx[pos-i*length+k] = idx[pos+k] - sstep[0] * i;
      }
      if (i >= dims[0] - width)
      {
        // adjust upper area
        int t_i = dims[0] - i;
        for (int k = 0; k < length; ++k)
          idx[pos+t_i*length+k] = WILT_OUTSIDE_ARRAY;
      }

      if (n == 1)
        dst[0] = op(src, idx, length*size[0]);
      else
        _filterPadded(dst, src, dims-1, dstep-1, sstep-1, size-1, idx, pos-width*length, length*size[0], op, n-1);
    }
  }

  template <class T, class U, class Operator>
  void _filterNone(
      T* dst,             // destination array pointer
      const U* src,       // source array pointer
      const pos_t* dims,  // inverted dimensions array pointer
      const pos_t* dstep, // inverted destination step array pointer
      const pos_t* sstep, // inverted source step array pointer
      const pos_t* size,  // inverted filter size array pointer
      pos_t* idx,         // filter index array pointer
      pos_t pos,          // position of filled idx segment
      pos_t length,       // length of filled idx segment
      Operator op,        // final filter operator
      dim_t n)            // current dimension / recursion counter
  {
    int width = size[0] / 2;

    // copy with steps for upper area
    for (int j = -width; j <= width; ++j)
      for (int k = 0; k < length; k++)
        idx[pos+j*length+k] = idx[pos+k] + sstep[0] * j;

    src += width * sstep[0];
    if (n == 1)
      for (int i = 0; i < dims[0]; ++i, dst += dstep[0], src += sstep[0])
        dst[0] = op(src, idx, length*size[0]);
    else
      for (int i = 0; i < dims[0]; ++i, dst += dstep[0], src += sstep[0])
        _filterNone(dst, src, dims-1, dstep-1, sstep-1, size-1, idx, pos-width*length, length*size[0], op, n-1);
  }

WILT_COMMON_BEGIN

  enum Filter
  {
    MEAN,
    MAX,
    MIN,
    MEDIAN
  }; // enum filter

  template <class T, dim_t N, class U, dim_t M, class Operator>
  NArray<T, N> compress(const NArray<U, M>& src, Operator op)
  {
    WILT_STATIC_ASSERT(N < M, "compress(): return must be smaller than src");

    NArray<T, N> ret(_chopLow<N>(src.dims()));
    _compress<T, M-N>(ret, src, Point<N>(), 0, op);
    return ret;
  }

  template <class T, class U, dim_t N, class Operator>
  NArray<T, N> filter(const NArray<U, N>& src, const Point<N>& size, Operator op, BorderType<U> border)
  {
    NArray<T, N> ret;
    if (border.type() == Border::NONE)
      ret = NArray<T, N>(src.dims()-size+1);
    else
      ret = NArray<T, N>(src.dims());
    _filter(ret, src, Point<N>(), size, 0, op, border);
    return ret;
  }

  template <class T, class U, dim_t N, class Operator>
  NArray<T, N> filter(const NArray<U, N>& src, const Point<N>& size, Operator op, int border)
  {
    return filter<T>(src, size, op, BorderType<U>(border));
  }

  template <class T, class U, dim_t N>
  NArray<T, N> filterMean(const NArray<U, N>& src, const Point<N>& size, BorderType<U> border)
  {
    // validate input
    for (dim_t i = 0; i < N; ++i)
    {
      if (src.length(i) <= size[i] / 2)
        throw std::invalid_argument("filterMean(): filter size too large");
      if (size[i] % 2 != 1)
        throw std::invalid_argument("filterMean(): filter size must be odd");
    }

    NArray<T, N> ret;
    const U* pad = border._padPtr();

    // create sliding index window
    pos_t length = _size(size);
    pos_t* idx = new pos_t[length];
    pos_t pos = length / 2;
    idx[pos] = 0;

    auto normalOp = [](const U* src, pos_t* idx, pos_t n)
    {
      T total = src[idx[0]];
      for (int i = 1; i < n; ++i)
        total += src[idx[i]];
      return total / n;
    };
    auto paddedOp = [pad](const U* src, pos_t* idx, pos_t n)
    {
      T total = (idx[0] == WILT_OUTSIDE_ARRAY) ? *pad : src[idx[0]];
      for (int i = 1; i < n; ++i)
        if (idx[i] == WILT_OUTSIDE_ARRAY)
          total += *pad;
        else
          total += src[idx[i]];
      return total / n;
    };
    auto ignoreOp = [](const U* src, pos_t* idx, pos_t n)
    {
      T total = src[idx[n/2]];
      pos_t m = 0;
      for (int i = 0; i < n; ++i)
        if (i != n/2)
          if (idx[i] == WILT_OUTSIDE_ARRAY)
            m++;
          else
            total += src[idx[i]];
      return total / (n-m);
    };

    try { switch(border.type())
    {
    case Border::REPLICATE:
      ret = NArray<T, N>(src.dims());
      _filterReplicate(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT:
      ret = NArray<T, N>(src.dims());
      _filterReflect(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT_101:
      ret = NArray<T, N>(src.dims());
      _filterReflect101(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::WRAP:
      ret = NArray<T, N>(src.dims());
      _filterWrap(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::PADDED:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, paddedOp, N);
      break;

    case Border::IGNORE:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, ignoreOp, N);
      break;

    case Border::NONE:
      ret = NArray<T, N>(src.dims()-size+1);
      _filterNone(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    default:
      throw std::domain_error("filterMean(): Invalid border type");
      break;

    } } catch(...) { delete[] idx; throw; }

    delete[] idx;
    return ret;
  }

  template <class T, class U, dim_t N>
  NArray<T, N> filterMean(const NArray<U, N>& src, const Point<N>& size, int border)
  {
    return filterMean<T>(src, size, BorderType<U>(border));
  }

  template <class T, dim_t N>
  NArray<T, N> filterMax(const NArray<T, N>& src, const NArray<uint8_t, N>& kernel, BorderType<T> border)
  {
    // validate input
    Point<N> size = kernel.dims();
    if (kernel.empty() || !kernel.isContinuous() || !kernel.isAligned())
      throw std::invalid_argument("filterMax(): kernel must be non-empty, continuous, and aligned. use clone()");
    for (dim_t i = 0; i < N; ++i)
    {
      if (src.length(i) <= size[i] / 2)
        throw std::invalid_argument("filterMax(): kernel too large");
      if (size[i] % 2 != 1)
        throw std::invalid_argument("filterMax(): kernel must be odd");
    }

    // find start and end of the kernel
    int start = -1, end;
    const uint8_t* kern = kernel._basePtr();
    for (int i = 0; i < kernel.size(); ++i)
    {
      if (*kern != 0)
      {
        end = i;
        if (start == -1)
          start = i;
      }
    }
    if (start == -1)
      throw std::invalid_argument("filterMax(): kernel must not be blank");

    NArray<T, N> ret;
    const T* pad = border._padPtr();

    // create sliding index window
    pos_t length = _size(size);
    pos_t* idx = new pos_t[length];
    pos_t pos = length / 2;
    idx[pos] = 0;

    auto normalOp = [start,end,kern](const T* src, pos_t* idx, pos_t n)
    {
      T max = src[idx[start]];
      for (int i = start+1; i < end; ++i)
        if (kern[i] != 0 && src[idx[i]] > max)
          max = src[idx[i]];
      return max;
    };
    auto paddedOp = [start,end,kern,pad](const T* src, pos_t* idx, pos_t n)
    {
      T max = (idx[start] == WILT_OUTSIDE_ARRAY) ? *pad : src[idx[start]];
      for (int i = start+1; i < end; ++i)
      {
        const T* elem = (idx[i] == WILT_OUTSIDE_ARRAY) ? pad : src+idx[i];
        if (kern[i] != 0 && *elem > max)
          max = *elem;
      }
      return max;
    };
    auto ignoreOp = [start,end,kern](const T* src, pos_t* idx, pos_t n)
    {
      int i = start;
      for ( ; i < end; ++i)
        if (idx[i] != WILT_OUTSIDE_ARRAY && kern[i] != 0)
          break;
      T max = src[idx[i]];
      for (++i ; i < end; ++i)
        if (idx[i] != WILT_OUTSIDE_ARRAY && kern[i] != 0 && src[idx[i]] > max)
          max = src[idx[i]];
      return max;
    };

    try { switch(border.type())
    {
    case Border::REPLICATE:
      ret = NArray<T, N>(src.dims());
      _filterReplicate(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT:
      ret = NArray<T, N>(src.dims());
      _filterReflect(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT_101:
      ret = NArray<T, N>(src.dims());
      _filterReflect101(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::WRAP:
      ret = NArray<T, N>(src.dims());
      _filterWrap(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::PADDED:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, paddedOp, N);
      break;

    case Border::IGNORE:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, ignoreOp, N);
      break;

    case Border::NONE:
      ret = NArray<T, N>(src.dims()-size+1);
      _filterNone(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    default:
      throw std::domain_error("filterMax(): Invalid border type");
      break;

    } } catch(...) { delete[] idx; throw; }

    delete[] idx;
    return ret;
  }

  template <class T, dim_t N>
  NArray<T, N> filterMax(const NArray<T, N>& src, const NArray<uint8_t, N>& kernel, int border)
  {
    return filterMax<T>(src, kernel, BorderType<T>(border));
  }

  template <class T, dim_t N>
  NArray<T, N> filterMax(const NArray<T, N>& src, const Point<N>& size, BorderType<T> border)
  {
    return filterMax(src, NArray<uint8_t, N>(size, uint8_t(255)), border);
  }

  template <class T, dim_t N>
  NArray<T, N> filterMax(const NArray<T, N>& src, const Point<N>& size, int border)
  {
    return filterMax<T>(src, size, BorderType<T>(border));
  }

  template <class T, dim_t N>
  NArray<T, N> filterMin(const NArray<T, N>& src, const NArray<uint8_t, N>& kernel, BorderType<T> border)
  {
    // validate input
    Point<N> size = kernel.dims();
    if (kernel.empty() || !kernel.isContinuous() || !kernel.isAligned())
      throw std::invalid_argument("filterMin(): kernel must be non-empty, continuous, and aligned. use clone()");
    for (dim_t i = 0; i < N; ++i)
    {
      if (src.length(i) <= size[i] / 2)
        throw std::invalid_argument("filterMin(): kernel too large");
      if (size[i] % 2 != 1)
        throw std::invalid_argument("filterMin(): kernel must be odd");
    }

    // find start and end of the kernel
    int start = -1, end;
    const uint8_t* kern = kernel._basePtr();
    for (int i = 0; i < kernel.size(); ++i)
    {
      if (*kern != 0)
      {
        end = i;
        if (start == -1)
          start = i;
      }
    }
    if (start == -1)
      throw std::invalid_argument("filterMin(): kernel must not be blank");

    NArray<T, N> ret;
    const T* pad = border._padPtr();

    // create sliding index window
    pos_t length = _size(size);
    pos_t* idx = new pos_t[length];
    pos_t pos = length / 2;
    idx[pos] = 0;

    auto normalOp = [start,end,kern](const T* src, pos_t* idx, pos_t n)
    {
      T min = src[idx[start]];
      for (int i = start+1; i < end; ++i)
        if (kern[i] != 0 && src[idx[i]] < min)
          min = src[idx[i]];
      return min;
    };
    auto paddedOp = [start,end,kern,pad](const T* src, pos_t* idx, pos_t n)
    {
      T min = (idx[start] == WILT_OUTSIDE_ARRAY) ? *pad : src[idx[start]];
      for (int i = start+1; i < end; ++i)
      {
        const T* elem = (idx[i] == WILT_OUTSIDE_ARRAY) ? pad : src+idx[i];
        if (kern[i] != 0 && *elem < min)
          min = *elem;
      }
      return min;
    };
    auto ignoreOp = [start,end,kern](const T* src, pos_t* idx, pos_t n)
    {
      int i = start;
      for ( ; i < end; ++i)
        if (idx[i] != WILT_OUTSIDE_ARRAY && kern[i] != 0)
          break;
      T min = src[idx[i]];
      for (++i ; i < end; ++i)
        if (idx[i] != WILT_OUTSIDE_ARRAY && kern[i] != 0 && src[idx[i]] < min)
          min = src[idx[i]];
      return min;
    };

    try { switch(border.type())
    {
    case Border::REPLICATE:
      ret = NArray<T, N>(src.dims());
      _filterReplicate(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT:
      ret = NArray<T, N>(src.dims());
      _filterReflect(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT_101:
      ret = NArray<T, N>(src.dims());
      _filterReflect101(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::WRAP:
      ret = NArray<T, N>(src.dims());
      _filterWrap(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::PADDED:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, paddedOp, N);
      break;

    case Border::IGNORE:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, ignoreOp, N);
      break;

    case Border::NONE:
      ret = NArray<T, N>(src.dims()-size+1);
      _filterNone(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    default:
      throw std::domain_error("filterMin(): Invalid border type");
      break;

    } } catch(...) { delete[] idx; throw; }

    delete[] idx;
    return ret;
  }

  template <class T, dim_t N>
  NArray<T, N> filterMin(const NArray<T, N>& src, const NArray<uint8_t, N>& kernel, int border)
  {
    return filterMin<T>(src, kernel, BorderType<T>(border));
  }

  template <class T, dim_t N>
  NArray<T, N> filterMin(const NArray<T, N>& src, const Point<N>& size, BorderType<T> border)
  {
    return filterMin(src, NArray<uint8_t, N>(size, uint8_t(255)), border);
  }

  template <class T, dim_t N>
  NArray<T, N> filterMin(const NArray<T, N>& src, const Point<N>& size, int border)
  {
    return filterMin<T>(src, size, BorderType<T>(border));
  }

  template <class T, class U, dim_t N>
  NArray<T, N> filterMedian(const NArray<U, N>& src, const Point<N>& size, BorderType<U> border)
  {
    // validate input
    for (dim_t i = 0; i < N; ++i)
    {
      if (src.length(i) <= size[i] / 2)
        throw std::invalid_argument("filterMedian(): filter size too large");
      if (size[i] % 2 != 1)
        throw std::invalid_argument("filterMedian(): filter size must be odd");
    }

    NArray<T, N> ret;
    const U* pad = border._padPtr();

    // create sliding index window
    pos_t length = _size(size);
    pos_t* idx = new pos_t[length];
    pos_t pos = length / 2;
    idx[pos] = 0;

    auto normalOp = [](const U* src, pos_t* idx, pos_t n) -> T
    {
      // copy ptrs
      const U** ptrs = new const U*[n];
      for (int i = 0; i < n; ++i)
        ptrs[i] = src + idx[i];

      // fastNth algorithm with N = n/2
      int A = 0;
      int B = n-1;
      while (A < B)
      {
        int a = A;
        int b = B;

        const U* x = ptrs[n/2];
        while (true)
        {
          while (*ptrs[a] < *x) ++a;
          while (*ptrs[b] > *x) --b;

          if (a > b) break;
          std::swap(ptrs[a++], ptrs[b--]);
        }

        if (b < n/2) A = a;
        if (a > n/2) B = b;
      }

      const U* ret = ptrs[n/2];
      delete[] ptrs;
      return *ret;
    };
    auto paddedOp = [pad](const U* src, pos_t* idx, pos_t n) -> T
    {
      // copy ptrs
      const U** ptrs = new const U*[n];
      for (int i = 0; i < n; ++i)
        if (idx[i] == WILT_OUTSIDE_ARRAY)
          ptrs[i] = pad;
        else
          ptrs[i] = src + idx[i];

      // fastNth algorithm with N = n/2
      int A = 0;
      int B = n-1;
      while (A < B)
      {
        int a = A;
        int b = B;

        const U* x = ptrs[n/2];
        while (true)
        {
          while (*ptrs[a] < *x) ++a;
          while (*ptrs[b] > *x) --b;

          if (a > b) break;
          std::swap(ptrs[a++], ptrs[b--]);
        }

        if (b < n/2) A = a;
        if (a > n/2) B = b;
      }

      const U* ret = ptrs[n/2];
      delete[] ptrs;
      return *ret;
    };

    try { switch(border.type())
    {
    case Border::REPLICATE:
      ret = NArray<T, N>(src.dims());
      _filterReplicate(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT:
      ret = NArray<T, N>(src.dims());
      _filterReflect(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT_101:
      ret = NArray<T, N>(src.dims());
      _filterReflect101(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::WRAP:
      ret = NArray<T, N>(src.dims());
      _filterWrap(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::PADDED:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, paddedOp, N);
      break;

    case Border::NONE:
      ret = NArray<T, N>(src.dims()-size+1);
      _filterNone(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    default:
      throw std::domain_error("filterMedian(): Invalid border type");
      break;

    } } catch(...) { delete[] idx; throw; }

    delete[] idx;
    return ret;
  }

  template <class T, class U, dim_t N>
  NArray<T, N> filterMedian(const NArray<U, N>& src, const Point<N>& size, int border)
  {
    return filterMedian<T>(src, size, BorderType<U>(border));
  }

  template <class T, class U, class V, dim_t N>
  NArray<T, N> filterKernel(const NArray<U, N>& src, const NArray<V, N>& kernel, BorderType<U> border)
  {
    // validate input
    Point<N> size = kernel.dims();
    if (kernel.empty() || !kernel.isContinuous() || !kernel.isAligned())
      throw std::invalid_argument("filterKernel(): kernel must be non-empty, continuous, and aligned. use clone()");
    for (dim_t i = 0; i < N; ++i)
    {
      if (src.length(i) <= size[i] / 2)
        throw std::invalid_argument("filterKernel(): kernel size too large");
      if (size[i] % 2 != 1)
        throw std::invalid_argument("filterKernel(): kernel size must be odd");
    }

    NArray<T, N> ret;
    const U* pad = border._padPtr();
    const V* kern = kernel._basePtr();

    // create sliding index window
    pos_t length = _size(size);
    pos_t* idx = new pos_t[length];
    pos_t pos = length / 2;
    idx[pos] = 0;

    auto normalOp = [kern](const U* src, pos_t* idx, pos_t n)
    {
      V total = src[idx[0]] * kern[0];
      for (int i = 1; i < n; ++i)
        total += src[idx[i]] * kern[i];
      return total;
    };
    auto paddedOp = [kern,pad](const U* src, pos_t* idx, pos_t n)
    {
      T total = (idx[0] == WILT_OUTSIDE_ARRAY) ? (*pad * kern[0]) : (src[idx[0]] * kern[0]);
      for (int i = 1; i < n; ++i)
        if (idx[i] == WILT_OUTSIDE_ARRAY)
          total += *pad * kern[i];
        else
          total += src[idx[i]] * kern[i];
      return total;
    };
    auto ignoreOp = [kern](const U* src, pos_t* idx, pos_t n)
    {
      T total = src[idx[n/2]] * kern[n/2];
      for (int i = 0; i < n; ++i)
        if (i != n/2 && idx[i] != WILT_OUTSIDE_ARRAY)
          total += src[idx[i]] * kern[i];
      return total;
    };

    try { switch(border.type())
    {
    case Border::REPLICATE:
      ret = NArray<T, N>(src.dims());
      _filterReplicate(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT:
      ret = NArray<T, N>(src.dims());
      _filterReflect(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::REFLECT_101:
      ret = NArray<T, N>(src.dims());
      _filterReflect101(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::WRAP:
      ret = NArray<T, N>(src.dims());
      _filterWrap(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    case Border::PADDED:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, paddedOp, N);
      break;

    case Border::IGNORE:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, ignoreOp, N);
      break;

    case Border::NONE:
      ret = NArray<T, N>(src.dims()-size+1);
      _filterNone(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, normalOp, N);
      break;

    default:
      throw std::domain_error("filterKernel(): Invalid border type");
      break;

    } } catch(...) { delete[] idx; throw; }

    delete[] idx;
    return ret;
  }

  template <class T, class U, class V, dim_t N>
  NArray<T, N> filterKernel(const NArray<U, N>& src, const NArray<V, N>& kernel, int border)
  {
    return filterKernel<T>(src, kernel, BorderType<U>(border));
  }

  template <class T, class U, dim_t N, class Operator>
  NArray<T, N> filterCustom(const NArray<U, N>& src, const Point<N>& size, Operator op, BorderType<U> border)
  {
    // validate input
    for (dim_t i = 0; i < N; ++i)
    {
      if (src.length(i) <= size[i] / 2)
        throw std::invalid_argument("filterCustom(): filter size too large");
      if (size[i] % 2 != 1)
        throw std::invalid_argument("filterCustom(): filter size must be odd");
    }

    NArray<T, N> ret;

    // create sliding index window
    pos_t length = _size(size);
    pos_t* idx = new pos_t[length];
    pos_t pos = length / 2;
    idx[pos] = 0;

    try { switch(border.type())
    {
    case Border::REPLICATE:
      ret = NArray<T, N>(src.dims());
      _filterReplicate(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, op, N);
      break;

    case Border::REFLECT:
      ret = NArray<T, N>(src.dims());
      _filterReflect(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, op, N);
      break;

    case Border::REFLECT_101:
      ret = NArray<T, N>(src.dims());
      _filterReflect101(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, op, N);
      break;

    case Border::WRAP:
      ret = NArray<T, N>(src.dims());
      _filterWrap(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, op, N);
      break;

    case Border::PADDED:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, op, N);
      break;

    case Border::IGNORE:
      ret = NArray<T, N>(src.dims());
      _filterPadded(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, op, N);
      break;

    case Border::NONE:
      ret = NArray<T, N>(src.dims()-size+1);
      _filterNone(ret._basePtr(), src._basePtr(), 
        ret._dimsPtr()+N-1, ret._stepPtr()+N-1, src._stepPtr()+N-1, 
        &size[N-1], idx, pos, 1, op, N);
      break;

    default:
      throw std::domain_error("filterCustom(): Invalid border type");
      break;

    } } catch(...) { delete[] idx; throw; }

    delete[] idx;
    return ret;
  }

  template <class T, class U, dim_t N, class Operator>
  NArray<T, N> filterCustom(const NArray<U, N>& src, const Point<N>& size, Operator op, int border)
  {
    return filterCustom<T>(src, size, op, BorderType<U>(border));
  }

WILT_COMMON_END
WILT_END

#endif // !WILT_FILTERS_HPP
