// FILE: borders.hpp
// DATE: 8/7/2014
// AUTH: Trevor Wilson [trwq75@mst.edu]
// DESC: Defines an N-dimensional templated array class with a border

// include guards
#ifndef WILT_BORDERS_HPP
#define WILT_BORDERS_HPP

#include "util.h"
#include "narray.hpp"
#include "point.hpp"

//! @brief  wilt scope
WILT_BEGIN
WILT_COMMON_BEGIN

  // forward declaration
  template <class T> class BorderType;

  class Border
  {
  public:
    enum : int
    {
      NONE        = 0, //     abcdef     border values are in the array, used for pre-padded data
      REPLICATE   = 1, // aaa|abcdef|fff border values are nearest edge value 
      REFLECT     = 2, // cba|abcdef|fed border values are reflected around the border
      REFLECT_101 = 3, // dcb|abcdef|edc border values are reflected around the edge values
      WRAP        = 4, // def|abcdef|abc border values are copied from the other edge
      IGNORE      = 5, // ---|abcdef|--- border values are ignored, may not be valid for all functions
      PADDED      = 6  // ggg|abcdef|ggg border values are a set value (created with Border::PAD())
    };

    template <class T>
    static BorderType<T> PAD(const T& u)
    {
      BorderType<T> b(PADDED);
      b.m_pad = new T(u);
      return b;
    }

  }; // class Border

  template <class T>
  class BorderType
  {
  public:
    BorderType(int type)
      : m_type(type),
        m_pad(nullptr)
    {

    }

    BorderType(const BorderType<T>& b)
      : m_type(b.m_type),
        m_pad(nullptr)
    {
      if (b.m_pad)
        m_pad = new T(*b.m_pad);
    }

    BorderType(BorderType&& b)
      : m_type(b.m_type),
        m_pad(b.m_pad)
    {
      b.m_pad = nullptr;
    }

    ~BorderType()
    {
      delete m_pad;
    }

    BorderType<T>& operator= (const BorderType<T>& b)
    {
      delete m_pad;

      m_type = b.m_type;
      if (b.m_pad)
        m_pad = new T(*b.m_pad);
      else
        m_pad = nullptr;

      return *this;
    }

    BorderType<T>& operator= (BorderType<T>&& b)
    {
      m_type = b.m_type;
      m_pad = b.m_pad;
      b.m_pad = nullptr;
      return *this;
    }

    const T& pad() const
    {
      return *m_pad;
    }

    T* _padPtr()
    {
      return m_pad;
    }

    int type() const
    {
      return m_type;
    }

    friend class Border;

  private:
    int m_type;
    T* m_pad;

  }; // class BorderType

  template <class T, dim_t N>
  class NArrayWindow
  {
  public:
    NArrayWindow()
      : m_arr(),
        m_size(),
        m_offs(),
        m_border(Border::NONE)
    {

    }

    NArrayWindow(const NArray<T, N>& arr, const Point<N>& size, const Point<N>& offset, BorderType<T> type)
      : m_arr(arr),
        m_size(size),
        m_offs(offset),
        m_border(type)
    {

    }

    T at(const Point<N>& loc) const
    {
      for (dim_t i = 0; i < N; ++i)
        if (loc[i] < 0 || loc[i] >= m_size[i])
          throw std::out_of_range("at() element larger then dimensions");

      Point<N> arr_pos = loc + m_offs;
      for (dim_t i = 0; i < N; ++i)
      {
        if (arr_pos[i] < 0 || arr_pos[i] >= m_arr.length(i))
        {
          switch (m_border.type())
          {
          case Border::REPLICATE:
            if (arr_pos[i] < 0) 
              arr_pos[i] = 0;
            else 
              arr_pos[i] = m_arr.length(i) - 1;
            break;
          case Border::REFLECT:
            if (arr_pos[i] < 0) 
              arr_pos[i] = -1 - arr_pos[i];
            else 
              arr_pos[i] = 2 * m_arr.length(i) - arr_pos[i] - 1;
            break;
          case Border::REFLECT_101:
            if (arr_pos[i] < 0) 
              arr_pos[i] = -arr_pos[i];
            else 
              arr_pos[i] = 2 * m_arr.length(i) - arr_pos[i] - 2;
            break;
          case Border::WRAP:
            if (arr_pos[i] < 0) 
              arr_pos[i] += m_arr.length(i);
            else 
              arr_pos[i] -= m_arr.length(i);
            break;
          case Border::PADDED:
            return m_border.pad();
            break;
          default:
            throw std::domain_error("NArrayWindow at() invalid border type");
            break;
          }
        }
      }

      return m_arr.at(arr_pos);
    }

    NArray<T, N> clone() const
    {
      // get bounds
      Point<N> min;
      Point<N> max;
      Point<N> mins;
      Point<N> maxs;
      bool enclosed = true;
      for (dim_t i = 0; i < N; ++i)
      {
        // min[i] = max(-m_offs[i], 0); with enclosed flag
        if (m_offs[i] < 0)
        {
          enclosed = false;
          min[i] = -m_offs[i];
        }
        else
        {
          min[i] = 0;
        }

        // max[i] = min(m_size[i], m_dims[i] - m_offs[i]); with enclosed flag
        if (m_size[i] > m_arr.length(i) - m_offs[i])
        {
          enclosed = false;
          max[i] = m_arr.length(i) - m_offs[i];

        }
        else
        {
          max[i] = m_size[i];
        }

        mins[i] = std::max(m_offs[i], 0l);
        maxs[i] = std::min(m_arr.length(i), m_size[i] + m_offs[i]);
      }

      if (enclosed)
      {
        // meaning there are no borders
        return m_arr.subarray(mins, maxs-mins).clone();
      }
      else
      {
        // create return array
        NArray<T, N> ret;
        if (m_border.type() == Border::PADDED)
          ret = NArray<T, N>(m_size, m_border.pad());
        else
          ret = NArray<T, N>(m_size);
        ret.subarray(min, max-min).setTo(m_arr.subarray(mins, maxs-mins));

        // there are borders
        switch (m_border.type())
        {
        case Border::REPLICATE:
          _cloneReplicate(ret, min, max);
          return ret;
        case Border::REFLECT:
          _cloneReflect(ret, min, max);
          return ret;
        case Border::REFLECT_101:
          _cloneReflect101(ret, min, max);
          return ret;
        case Border::WRAP:
          _cloneWrap(ret, min, max);
          return ret;
        case Border::PADDED:
          return ret;
        default:
          // Border::NONE or invalid code
          throw std::domain_error("NArrayWindow clone() invalid border type");
        }
      }
    }

    operator NArray<T, N> ()
    {
      return clone();
    }

  protected:
    NArray<T, N> m_arr;
    Point<N> m_size;
    Point<N> m_offs;
    BorderType<T> m_border;

  private:
    NArray<T, N> _wrapArea(const Point<N>& loc, const Point<N>& size) const
    {
      NArray<T, N> ret(size);
      _wrapArea(ret, Point<N>(), loc, 0);
      return ret;
    }

    void _wrapArea(NArray<T, N>& arr, Point<N> dloc, Point<N> sloc, dim_t n) const
    {
      if (n == N-1)
      {
        for (pos_t i = 0; i < arr.length(n); ++i, ++dloc[n], ++sloc[n])
          arr.at(dloc) = at(sloc);
      }
      else
      {
        for (pos_t i = 0; i < arr.length(n); ++i, ++dloc[n], ++sloc[n])
          _wrapArea(arr, dloc, sloc, n+1);
      }
    }

    void _cloneReplicate(NArray<T, N>& window, const Point<N>& min, const Point<N>& max) const
    {
      Point<N> s_min = min;
      Point<N> s_max = max;
      for (dim_t i = 0; i < N; ++i)
      {
        s_min[i] = 0;
        s_max[i] = m_size[i];
        NArray<T, N> sub = window.subarray(s_min, s_max-s_min);

        if (min[i] > 0)
        {
          for (pos_t j = 0; j < min[i]; ++j)
            sub.sliceN(j, i).setTo(sub.sliceN(min[i], i));
        }
        
        if (max[i] < m_size[i])
        {
          for (pos_t j = max[i]; j < m_size[i]; ++j)
            sub.sliceN(j, i).setTo(sub.sliceN(max[i]-1, i));
        }
      }
    }

    void _cloneReflect(NArray<T, N>& window, const Point<N>& min, const Point<N>& max) const
    {
      Point<N> s_min = min;
      Point<N> s_max = max;
      for (dim_t i = 0; i < N; ++i)
      {
        s_min[i] = 0;
        s_max[i] = m_size[i];
        NArray<T, N> sub = window.subarray(s_min, s_max-s_min);

        if (min[i] > 0)
        {
          sub.rangeN(0, min[i], i).setTo(sub.rangeN(min[i], min[i], i).flipN(i));
        }
        
        if (max[i] < m_size[i])
        {
          pos_t width = m_size[i] - max[i];
          sub.rangeN(max[i], width, i).setTo(sub.rangeN(m_size[i]-2*width, width, i).flipN(i));
        }
      }
    }

    void _cloneReflect101(NArray<T, N>& window, const Point<N>& min, const Point<N>& max) const
    {
      Point<N> s_min = min;
      Point<N> s_max = max;
      for (dim_t i = 0; i < N; ++i)
      {
        s_min[i] = 0;
        s_max[i] = m_size[i];
        NArray<T, N> sub = window.subarray(s_min, s_max-s_min);

        if (min[i] > 0)
        {
          sub.rangeN(0, min[i], i).setTo(sub.rangeN(min[i]+1, min[i], i).flipN(i));
        }
        
        if (max[i] < m_size[i])
        {
          pos_t width = m_size[i] - max[i];
          sub.rangeN(max[i], width, i).setTo(sub.rangeN(m_size[i]-2*width-1, width, i).flipN(i));
        }
      }
    }

    void _cloneWrap(NArray<T, N>& window, const Point<N>& min, const Point<N>& max) const
    {
      Point<N> s_min = min;
      Point<N> s_max = max;
      for (dim_t i = 0; i < N; ++i)
      {
        s_min[i] = 0;
        s_max[i] = m_size[i];
        NArray<T, N> sub = window.subarray(s_min, s_max-s_min);

        if (min[i] > 0)
        {
          if (max[i] < m_size[i])
          {
            // special case
            sub.rangeN(0, min[i], i).setTo(sub.rangeN(max[i]-min[i], min[i], i));
          }
          else
          {
            NArray<T, N> minside = sub.rangeN(0, min[i], i);
            minside.setTo(_wrapArea(s_min, minside.dims()));
          }
        }
        
        if (max[i] < m_size[i])
        {
          pos_t width = m_size[i] - max[i];
          if (min[i] > 0)
          {
            sub.rangeN(max[i], width, i).setTo(sub.rangeN(min[i], m_size[i]-max[i], i));
          }
          else
          {
            Point<N> loc = s_min;
            loc[i] = max[i];
            NArray<T, N> maxside = sub.rangeN(max[i], width, i);
            maxside.setTo(_wrapArea(loc, maxside.dims()));
          }
        }
      }
    }

  }; // class NArrayBorder

  template <class T, dim_t N>
  NArray<T, N> padArray(const NArray<T, N>& src, const Point<N>& width, BorderType<T> border)
  {
    return NArrayWindow<T, N>(src, -width, src.dims() + 2 * width, border).clone();
  }

  template <class T, dim_t N>
  NArray<T, N> padArray(const NArray<T, N>& src, const Point<N>& width, int border)
  {
    return padArray(src, width, BorderType<T>(border));
  }

WILT_COMMON_END
WILT_END

// WILT_BORDERS_HPP
#endif