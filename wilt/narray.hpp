// FILE: narray.hpp
// DATE: 7/30/2014
// AUTH: Trevor Wilson [kmdreko@gmail.com]
// DESC: Defines an N-dimensional templated array class

// include guards
#ifndef WILT_NARRAY_HPP
#define WILT_NARRAY_HPP

// libraries
#include <memory>
// - std::allocator
#include <type_traits>
// - std::remove_const

#include "util.h"
#include "point.hpp"

//! @brief  wilt scope
WILT_BEGIN

  //! @brief         applies an operation on two source arrays and stores the
  //!                result in a destination array
  //! @param[out]    dst - the destination array
  //! @param[in]     src1 - pointer to 1st source array
  //! @param[in]     src2 - pointer to 2nd source array
  //! @param[in]     dims - pointer to dimension array
  //! @param[in]     dstep - pointer to dst step array
  //! @param[in]     s1step - pointer to 1st source step array
  //! @param[in]     s2step - pointer to 2nd source step array
  //! @param[in]     op - function or function object with the signature 
  //!                T(U, V) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! arrays must be the same size (hence the single dimension array), and
  //! this function makes no checks whether the inputs are valid.
  template <class T, class U, class V, class Operator>
  void _binaryOp(
    T* dst, const U* src1, const V* src2, const pos_t* dims, 
    const pos_t* dstep, const pos_t* s1step, const pos_t* s2step, 
    Operator op, dim_t N)
  {
    T* end = dst + dims[0] * dstep[0];
    if (N == 1)
    {
#if WILT_LOOP_UNROLLING_ENABLED
      if (dstep[0] == 1 && s1step[0] == 1 && s2step[0] == 1)
      for (T* endr = end - 3; dst < endr; dst += 4, src1 += 4, src2 += 4)
      {
        dst[0] = op(src1[0], src2[0]);
        dst[1] = op(src1[1], src2[1]);
        dst[2] = op(src1[2], src2[2]);
        dst[3] = op(src1[3], src2[3]);
      }
#endif

      for ( ; dst != end; dst += dstep[0], src1 += s1step[0], src2 += s2step[0])
        dst[0] = op(src1[0], src2[0]);
    }
    else
    {
      for ( ; dst != end; dst += dstep[0], src1 += s1step[0], src2 += s2step[0])
        _binaryOp(dst, src1, src2, dims+1, dstep+1, s1step+1, s2step+1, op, N-1);
    }
  }

  //! @brief         applies an operation on two source arrays and stores the
  //!                result in a destination array
  //! @param[in,out] dst - the destination array
  //! @param[in]     src1 - pointer to 1st source array
  //! @param[in]     src2 - pointer to 2nd source array
  //! @param[in]     dims - pointer to dimension array
  //! @param[in]     dstep - pointer to dst step array
  //! @param[in]     s1step - pointer to 1st source step array
  //! @param[in]     s2step - pointer to 2nd source step array
  //! @param[in]     op - function or function object with the signature 
  //!                void(T&, U, V) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! arrays must be the same size (hence the single dimension array), and
  //! this function makes no checks whether the inputs are valid.
  template <class T, class U, class V, class Operator>
  void _binaryOp2(
    T* dst, const U* src1, const V* src2, const pos_t* dims, 
    const pos_t* dstep, const pos_t* s1step, const pos_t* s2step, 
    Operator op, dim_t N)
  {
    T* end = dst + dims[0] * dstep[0];
    if (N == 1)
    {
#if WILT_LOOP_UNROLLING_ENABLED
      if (dstep[0] == 1 && s1step[0] == 1 && s2step[0] == 1)
      for (T* endr = end - 3; dst < endr; dst += 4, src1 += 4, src2 += 4)
      {
        op(dst[0], src1[0], src2[0]);
        op(dst[1], src1[1], src2[1]);
        op(dst[2], src1[2], src2[2]);
        op(dst[3], src1[3], src2[3]);
      }
#endif

      for ( ; dst != end; dst += dstep[0], src1 += s1step[0], src2 += s2step[0])
        op(dst[0], src1[0], src2[0]);
    }
    else
    {
      for ( ; dst != end; dst += dstep[0], src1 += s1step[0], src2 += s2step[0])
        _binaryOp2(dst, src1, src2, dims+1, dstep+1, s1step+1, s2step+1, op, N-1);
    }
  }

  //! @brief         applies an operation on a source array and stores the
  //!                result in a destination array
  //! @param[out]    dst - the destination array
  //! @param[in]     src - pointer to 1st source array
  //! @param[in]     dims - pointer to dimension array
  //! @param[in]     dstep - pointer to dst step array
  //! @param[in]     sstep - pointer to source step array
  //! @param[in]     op - function or function object with the signature 
  //!                T(U) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! arrays must be the same size (hence the single dimension array), and
  //! this function makes no checks whether the inputs are valid.
  template <class T, class U, class Operator>
  void _unaryOp(
    T* dst, const U* src, const pos_t* dims, 
    const pos_t* dstep, const pos_t* sstep, 
    Operator op, dim_t N)
  {
    T* end = dst + dstep[0] * dims[0];
    if (N == 1)
    {
#if WILT_LOOP_UNROLLING_ENABLED
      if (dstep[0] == 1 && sstep[0] == 1)
      for (T* endr = end - 3; dst < endr; dst += 4, src += 4)
      {
        dst[0] = op(src[0]);
        dst[1] = op(src[1]);
        dst[2] = op(src[2]);
        dst[3] = op(src[3]);
      }
#endif

      for ( ; dst != end; dst += dstep[0], src += sstep[0])
        dst[0] = op(src[0]);
    }
    else
    {
      for ( ; dst != end; dst += dstep[0], src += sstep[0])
        _unaryOp(dst, src, dims+1, dstep+1, sstep+1, op, N-1);
    }
  }

  //! @brief         applies an operation on a source array and stores the
  //!                result in a destination array
  //! @param[in,out] dst - the destination array
  //! @param[in]     src - pointer to 1st source array
  //! @param[in]     dims - pointer to dimension array
  //! @param[in]     dstep - pointer to dst step array
  //! @param[in]     sstep - pointer to source step array
  //! @param[in]     op - function or function object with the signature 
  //!                void(T&, U) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! arrays must be the same size (hence the single dimension array), and
  //! this function makes no checks whether the inputs are valid.
  template <class T, class U, class Operator>
  void _unaryOp2(
    T* dst, const U* src, const pos_t* dims, 
    const pos_t* dstep, const pos_t* sstep, 
    Operator op, dim_t N)
  {
    T* end = dst + dstep[0] * dims[0];
    if (N == 1)
    {
#if WILT_LOOP_UNROLLING_ENABLED
      if (dstep[0] == 1 && sstep[0] == 1)
      for (T* endr = end - 3; dst < endr; dst += 4, src += 4)
      {
        op(dst[0], src[0]);
        op(dst[1], src[1]);
        op(dst[2], src[2]);
        op(dst[3], src[3]);
      }
#endif

      for ( ; dst < end; dst += dstep[0], src += sstep[0])
        op(dst[0], src[0]);
    }
    else
    {
      for ( ; dst < end; dst += dstep[0], src += sstep[0])
        _unaryOp2(dst, src, dims+1, dstep+1, sstep+1, op, N-1);
    }
  }

  //! @brief         applies an operation and stores the result in a destination
  //!                array
  //! @param[out]    dst - the destination array
  //! @param[in]     dims - pointer to dimension array
  //! @param[in]     dstep - pointer to dst step array
  //! @param[in]     op - function or function object with the signature 
  //!                T() or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! this function makes no checks whether the inputs are valid.
  template <class T, class Operator>
  void _singleOp(
    T* dst, const pos_t* dims, 
    const pos_t* dstep, 
    Operator op, dim_t N)
  {
    T* end = dst + dims[0] * dstep[0];
    if (N == 1)
    {
#if WILT_LOOP_UNROLLING_ENABLED
      if (dstep[0] == 1)
      for (T* endr = end - 3; dst < endr; dst += 4)
      {
        dst[0] = op();
        dst[1] = op();
        dst[2] = op();
        dst[3] = op();
      }
#endif

      for ( ; dst != end; dst += dstep[0])
        dst[0] = op();
    }
    else
    {
      for ( ; dst != end; dst += dstep[0])
        _singleOp(dst, dims+1, dstep+1, op, N-1);
    }
  }

  //! @brief         applies an operation and stores the result in a destination
  //!                array
  //! @param[in,out] dst - the destination array
  //! @param[in]     dims - pointer to dimension array
  //! @param[in]     dstep - pointer to dst step array
  //! @param[in]     op - function or function object with the signature 
  //!                void(T&) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //!
  //! this function makes no checks whether the inputs are valid.
  template <class T, class Operator>
  void _singleOp2(
    T* dst, const pos_t* dims, 
    const pos_t* dstep, 
    Operator op, dim_t N)
  {
    T* end = dst + dims[0] * dstep[0];
    if (N == 1)
    {
#if WILT_LOOP_UNROLLING_ENABLED
      if (dstep[0] == 1)
      for (T* endr = end - 3; dst < endr; dst += 4)
      {
        op(dst[0]);
        op(dst[1]);
        op(dst[2]);
        op(dst[3]);
      }
#endif

      for ( ; dst != end; dst += dstep[0])
        op(dst[0]);
    }
    else
    {
      for ( ; dst != end; dst += dstep[0])
        _singleOp2(dst, dims+1, dstep+1, op, N-1);
    }
  }

  //! @brief         applies an operation between two source arrays and returns
  //!                a bool. Returns false upon getting the first false, or
  //!                or returns true if all operations return true
  //! @param[in]     src1 - pointer to 1st source array
  //! @param[in]     src2 - pointer to 2nd source array
  //! @param[in]     dims - pointer to dimension array
  //! @param[in]     s1step - pointer to 1st source step array
  //! @param[in]     s2step - pointer to 2nd source step array
  //! @param[in]     op - function or function object with the signature 
  //!                bool(T, U) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //! @return        true if all operations return true, else false
  //!
  //! this function makes no checks whether the inputs are valid.
  template <class T, class U, class Operator>
  bool _allOf(
    const T* src1, const U* src2, const pos_t* dims,
    const pos_t* s1step, const pos_t* s2step,
    Operator op, dim_t N)
  {
    T* end = src1 + dims[0] * s1step[0];
    if (N == 1)
    {
      for ( ; src1 != end; src1 += s1step[0], src2 += s2step[0])
        if (!op(src1[0], src2[0]))
          return false;
    }
    else
    {
      for ( ; src1 != end; src1 += s1step[0], src2 += s2step[0])
        if (!_allOf(src1, src2, dims+1, s1step+1, s2step+1, op, N-1))
          return false;
    }
    return true;
  }

  //! @brief         applies an operation on a source array and returns a bool.
  //!                Returns false upon getting the first false, or returns true
  //!                if all operations return true
  //! @param[in]     src - pointer to source array
  //! @param[in]     dims - pointer to dimension array
  //! @param[in]     sstep - pointer to source step array
  //! @param[in]     op - function or function object with the signature 
  //!                bool(T) or similar
  //! @param[in]     N - the dimensionality of the arrays
  //! @return        true if all operations return true, else false
  //!
  //! this function makes no checks whether the inputs are valid.
  template <class T, class Operator>
  bool _allOf(
    const T* src, const pos_t* dims,
    const pos_t* sstep,
    Operator op, dim_t N)
  {
    T* end = src + dims[0] * sstep[0];
    if (N == 1)
    {
      for ( ; src != end; src += sstep[0])
        if (!op(src[0]))
          return false;
    }
    else
    {
      for ( ; src != end; src += sstep[0])
        if (!_allOf(src, dims+1, sstep+1, op, N-1))
          return false;
    }
    return true;
  }

WILT_COMMON_BEGIN

  enum PTR
  {
    ASSUME,
    COPY,
    REF
  };

WILT_COMMON_END

  template <class T, class A = std::allocator<T>>
  class NArrayHeader
  {
  public:
    class Node
    {
    public:
      T* data;
      std::size_t size;
      std::size_t refs;
      A alloc;
      bool owned;

      Node()
        : data(nullptr),
          size(0),
          refs(0),
          alloc(),
          owned(true)
      {

      }

      Node(std::size_t _size)
        : data(nullptr),
          size(_size),
          refs(1),
          alloc(),
          owned(true)
      {
        data = alloc.allocate(_size);
        if (!raw_t<T>::value())
          for (size_t i = 0; i < _size; ++i)
            alloc.construct(data+i);
      }

      Node(std::size_t _size, const T& val)
        : data(nullptr),
          size(_size),
          refs(1),
          alloc(),
          owned(true)
      {
        data = alloc.allocate(_size);
        for (size_t i = 0; i < _size; ++i)
          alloc.construct(data+i, val);
      }

      Node(std::size_t _size, T* _data, PTR ltype)
        : data(nullptr),
          size(_size),
          refs(1),
          alloc(),
          owned(true)
      {
        switch (ltype)
        {
        case PTR::ASSUME:
          data = _data;
          break;
        case PTR::COPY:
          data = alloc.allocate(_size);
          for (size_t i = 0; i < _size; ++i)
            alloc.construct(data+i, _data[i]);
          break;
        case PTR::REF:
          data = _data;
          owned = false;
          break;
        }
      }

      template <class Generator>
      Node(std::size_t _size, Generator gen)
        : data(nullptr),
          size(_size),
          refs(1),
          alloc(),
          owned(true)
      {
        data = alloc.allocate(_size);
        for (size_t i = 0; i < _size; ++i)
          alloc.construct(data+i, gen());
      }

      ~Node()
      {
        if (data && owned)
        {
          if (!raw_t<T>::value())
            for (std::size_t i = 0; i < size; ++i)
              alloc.destroy(data+i);
          alloc.deallocate(data, size);
        }
      }

    }; // class Node

    NArrayHeader()
      : m_node(nullptr)
    {

    }

    NArrayHeader(pos_t size)
      : m_node(nullptr)
    {
      if (size > 0)
        m_node = new Node(size);
    }

    NArrayHeader(pos_t size, const T& val)
      : m_node(nullptr)
    {
      if (size > 0)
        m_node = new Node(size, val);
    }

    NArrayHeader(pos_t size, T* ptr, PTR ltype)
      : m_node(nullptr)
    {
      if (size > 0)
        m_node = new Node(size, ptr, ltype);
    }

    template <class Generator>
    NArrayHeader(pos_t size, Generator gen)
      : m_node(nullptr)
    {
      if (size > 0)
        m_node = new Node(size, gen);
    }


    NArrayHeader(const NArrayHeader<T, A>& header)
      : m_node(header.m_node)
    {
      _incref();
    }

    ~NArrayHeader()
    {
      _decref();
    }

    NArrayHeader<T, A>& operator= (const NArrayHeader<T, A>& header)
    {
      _decref();
      m_node = header.m_node;
      _incref();
      return *this;
    }

    void clear()
    {
      _decref();
      m_node = nullptr;
    }

    Node& operator* () const
    {
      return *m_node;
    }
    Node* operator-> () const
    {
      return m_node;
    }

    Node* _ptr() const
    {
      return m_node;
    }

  private:
    Node* m_node;

    void _incref()
    {
      if (m_node)
        ++m_node->refs;
    }
    void _decref()
    {
      if (m_node && --m_node->refs == 0)
        delete m_node;
    }

  }; // class NArrayHeader

WILT_COMMON_BEGIN

  // forward declaration
  template <class T, dim_t N> class NArrayIterator;

  //! @class NArray
  template <class T, dim_t N>
  class NArray
  {
  public:
    // type definitions
    typedef typename std::remove_const<T>::type type;
    typedef       T  value;
    typedef const T  cvalue;
    typedef       T* pointer;
    typedef const T* cpointer;
    typedef       T& reference;
    typedef const T& creference;

    typedef NArrayIterator<value,  N> iterator;
    typedef NArrayIterator<cvalue, N> const_iterator;

    //! @brief  Default constructor. Makes an empty NArray
    NArray()
      : m_header(),
        m_base(nullptr),
        m_dims(),
        m_step()
    {

    }

    //! @brief      Value constructor. Constructs an NArray with size. Elements
    //!             are defaulted. 
    //! @param[in]  size - list of dimension lengths
    explicit NArray(const Point<N>& size)
      : m_header(),
        m_base(nullptr),
        m_dims(size),
        m_step(_step(size))
    {
      if (_valid())
        _create();
      else
        _clean();
    }

    NArray(const Point<N>& size, const T& val)
      : m_header(),
        m_base(nullptr),
        m_dims(size),
        m_step(_step(size))
    {
      if (_valid())
        _create(val);
      else
        _clean();
    }

    NArray(const Point<N>& size, T* ptr, PTR ltype)
      : m_header(),
        m_base(nullptr),
        m_dims(size),
        m_step(_step(size))
    {
      if (_valid())
        _create(ptr, ltype);
      else
        _clean();
    }

#if WILT_BRACE_INITIALIZATION_ENABLED == 1
    NArray(const Point<N>& size, std::initializer_list<T> list)
      : m_header(),
        m_base(nullptr),
        m_dims(size),
        m_step(_step(size))
    {
      if (_valid() && list.size() == _size(m_dims))
        _create((T*)list.begin(), PTR::COPY);
      else
        _clean();
    }
#endif

    template <class Generator>
    NArray(const Point<N>& size, Generator gen)
      : m_header(),
        m_base(nullptr),
        m_dims(size),
        m_step(_step(size))
    {
      if (_valid())
        _create(gen);
      else
        _clean();
    }

    //! @brief      Copy constructor. Just references the data from arr. copies
    //!             all internal structures and increments internal reference
    //!             counter (if data exists)
    //! @param[in]  arr - NArray to copy from
    NArray(const NArray<type, N>& arr)
      : m_header(arr.m_header),
        m_base(arr.m_base),
        m_dims(arr.m_dims),
        m_step(arr.m_step)
    {

    }

    //! @brief      Move constructor. Assumes the data from arr. 
    //! @param[in]  arr - NArray to move
    NArray(NArray<type, N>&& arr)
      : m_header(arr.m_header),
        m_base(arr.m_base),
        m_dims(arr.m_dims),
        m_step(arr.m_step)
    {
      arr.clear();
    }

    //! @brief      Copy constructor for const NArray. If T is also const, then
    //!             perform shallow copy and increment reference counter (if
    //!             data exists), otherwise construct new NArray of same size
    //!             and deep copy
    //! @param[in]  arr - const NArray to copy from
    NArray(const NArray<cvalue, N>& arr)
      : m_header(),
        m_base(nullptr),
        m_dims(arr.m_dims),
        m_step(arr.m_step)
    {
      if (std::is_const<T>::value)
      {
        m_header = arr.m_header;
        m_base = arr.m_base;
      }
      else
      {
        _create();
        setTo(arr);
      }
    }

    //! @brief      Move constructor. Assumes the data from arr if able to.
    //! @param[in]  arr - NArray to move
    //!
    //! I actually don't think this will be utilized fully because a const T
    //! NArray moved as the only reference should not ever exist, its possible
    //! but not under normal operation of the library
    NArray(NArray<cvalue, N>&& arr)
      : m_header(),
        m_base(nullptr),
        m_dims(arr.m_dims),
        m_step(arr.m_step)
    {
      if (std::is_const<T>::value || arr.isUnique())
      {
        m_header = arr.m_header;
        m_base = arr.m_base;
      }
      else
      {
        _create();
        setTo(arr);
      }

      arr.clear();
    }

    //! @brief      Assignment operator. References the data from arr. Copies
    //!             all internal structures and increments internal reference
    //!             counter (if data exists). Deletes previous data if last
    //!             reference
    //! @param[in]  arr - NArray to copy from
    //! @return     reference to this object
    NArray<T, N>& operator = (const NArray<type, N>& arr)
    {
      m_header = arr.m_header;
      m_dims   = arr.m_dims;
      m_step   = arr.m_step;
      m_base   = arr.m_base;

      return *this;
    }

    NArray<T, N>& operator = (NArray<type, N>&& arr)
    {
      m_header = arr.m_header;
      m_dims   = arr.m_dims;
      m_step   = arr.m_step;
      m_base   = arr.m_base;

      arr.clear();

      return *this;
    }

    //! @brief      Assignment operator for const NArray. If T is also const,
    //!             then perform shallow copy and increment reference counter
    //!             (if data exists), otherwise construct new NArray of same 
    //!             size and deep copy. Deletes previous data if last reference
    //! @param[in]  arr - const NArray to copy from
    //! @return     reference to this object
    NArray<T, N>& operator = (const NArray<cvalue, N>& arr)
    {
      m_dims = arr.m_dims;
      m_step = arr.m_step;

      if (std::is_const<T>::value)
      {
        m_header = arr.m_header;
        m_base = arr.m_base;
      }
      else
      {
        _create();
        setTo(arr);
      }

      return *this;
    }

    NArray<T, N>& operator = (NArray<cvalue, N>&& arr)
    {
      m_dims = arr.m_dims;
      m_step = arr.m_step;

      if (std::is_const<T>::value || arr.isUnique())
      {
        m_header = arr.m_header;
        m_base = arr.m_base;
      }
      else
      {
        _create();
        setTo(arr);
      }

      return *this;
    }

    //! @brief      In-place addition operator, dimensions of both NArrays must
    //!             match, should trigger compiler error if attempted on const
    //!             type
    //! @param[in]  arr - NArray to add
    //! @return     reference to this object
    NArray<T, N>& operator+= (const NArray<cvalue, N>& arr)
    {
      WILT_STATIC_ASSERT(!std::is_const<T>::value, "operator+= invalid on const type");

      if (m_dims != arr.m_dims)
        throw std::invalid_argument("NArray+=(arr) dimensions must match");
      if (empty())
        return *this;

      _unaryOp2(m_base, arr.m_base, &m_dims[0], &m_step[0], &arr.m_step[0], [](T& lhs, const T& rhs){lhs+=rhs;}, N);

      return *this;
    }

    //! @brief      In-place addition operator, should trigger compiler error if
    //!             attempted on const type
    //! @param[in]  val - value to add to each element
    //! @return     reference to this object
    NArray<T, N>& operator+= (const T& val)
    {
      WILT_STATIC_ASSERT(!std::is_const<T>::value, "operator+= invalid on const type");

      if (empty())
        return *this;

      _singleOp2(m_base, &m_dims[0], &m_step[0], [&val](T& lhs){lhs+=val;}, N);

      return *this;
    }

    //! @brief      In-place subtraction operator, dimensions of both NArrays 
    //!             must match, should trigger compiler error if attempted on
    //!             const type
    //! @param[in]  arr - NArray to subtract
    //! @return     reference to this object
    NArray<T, N>& operator-= (const NArray<cvalue, N>& arr)
    {
      WILT_STATIC_ASSERT(!std::is_const<T>::value, "operator-= invalid on const type");

      if (m_dims != arr.m_dims)
        throw std::invalid_argument("NArray-=(arr) dimensions must match");
      if (empty())
        return *this;

      _unaryOp2(m_base, arr.m_base, &m_dims[0], &m_step[0], &arr.m_step[0], [](T& lhs, const T& rhs){lhs-=rhs;}, N);

      return *this;
    }

    //! @brief      In-place subtraction operator, should trigger compiler error
    //!             if attempted on const type
    //! @param[in]  val - value to subtract from each element
    //! @return     reference to this object
    NArray<T, N>& operator-= (const T& val)
    {
      WILT_STATIC_ASSERT(!std::is_const<T>::value, "operator-= invalid on const type");

      if (empty())
        return *this;

      _singleOp2(m_base, &m_dims[0], &m_step[0], [&val](T& lhs){lhs-=val;}, N);

      return *this;
    }

    //! @brief      In-place multiplication operator, should trigger compiler
    //!             error if attempted on const type
    //! @param[in]  val - value to multiply each element by
    //! @return     reference to this object
    NArray<T, N>& operator*= (const T& val)
    {
      WILT_STATIC_ASSERT(!std::is_const<T>::value, "operator*= invalid on const type");

      if (empty())
        return *this;

      _singleOp2(m_base, &m_dims[0], &m_step[0], [&val](T& lhs){lhs*=val;}, N);

      return *this;
    }

    //! @brief      In-place division operator, should trigger compiler error
    //!             if attempted on const type
    //! @param[in]  val - value to divide each element by
    //! @return     reference to this object
    NArray<T, N>& operator/= (const T& val)
    {
      WILT_STATIC_ASSERT(!std::is_const<T>::value, "operator/= invalid on const type");

      if (empty())
        return *this;

      _singleOp2(m_base, &m_dims[0], &m_step[0], [&val](T& lhs){lhs/=val;}, N);

      return *this;
    }

    //! @brief      Indexing operator to return an N-1 NArray at the x location
    //! @param[in]  x - the dimension 0 location
    //! @return     N-1 NArray referencing the same data
    //! @exception  std::out_of_range if x < length
    //! 
    //! Identical function to sliceX(x)
    //! It is preferable to do arr.at({x, y, y, ...}) than arr[x][y][z]... 
    //! because the operator[] must make a new NArray for each use.
    NArray<value , N-1> operator[] (pos_t x)
    {
      if (x >= m_dims[0])
        throw std::out_of_range("operator[] index out of bounds");

      return _nSlice(x, 0);
    }
    NArray<cvalue, N-1> operator[] (pos_t x) const
    {
      if (x >= m_dims[0])
        throw std::out_of_range("operator[] index out of bounds");

      return _nSlice(x, 0);
    }

    //! @brief      Gets the dimensions of the NArray
    //! @return     Point containing the length of each dimension
    Point<N> dims() const
    {
      return m_dims;
    }

    //! @brief      Gets the step sizes of the NArray
    //! @return     Point containing the step sizes
    //!
    //! A step value is the offset from one element to the next, therefore the
    //! step sizes correspond the the step values for each dimension
    Point<N> step() const
    {
      return m_step;
    }

    //! @brief      Gets the number of elements accessible by the NArray
    //! @return     The size of the NArray
    pos_t size() const
    {
      return _size(m_dims);
    }

    //! @brief      Gets the length of the data in a dimension
    //! @param[in]  dim - the dimension to get the length of
    //! @return     the length of the dimension dim
    //! @exception  std::out_of_range if dim < N
    //! 
    //! Return value undefined for empty Mats; might be 0, last value, or junk.
    pos_t length(dim_t dim) const
    {
      if (dim >= N)
        throw std::out_of_range("length() argument out of bounds");

      return m_dims[dim];
    }

    //! @brief      Returns whether the NArray is empty
    //! @return     True if NArray references no data
    bool empty() const
    {
      return m_header._ptr() == nullptr;
    }

    //! @brief      Gets the width of the NArray
    //! @return     the x dimension size of the NArray
    //!
    //! Equivalent to length(0). Refer to length() description for caveats.
    pos_t width() const
    {
      return m_dims[0];
    }

    //! @brief      Gets the height of the NArray
    //! @return     the y dimension size of the NArray
    //!
    //! Equivalent to length(1). Refer to length() description for caveats.
    //! static_assert triggered if N < 2
    pos_t height() const
    {
      WILT_STATIC_ASSERT(N >= 2, "height(): unavailable for N < 2");

      return m_dims[1];
    }

    //! @brief      Gets the depth of the NArray
    //! @return     the z dimension size of the NArray
    //!
    //! Equivalent to length(2). Refer to length() description for caveats.
    //! static_assert triggered if N < 3
    pos_t depth() const
    {
      WILT_STATIC_ASSERT(N >= 3, "depth(): unavailable for N < 3");

      return m_dims[2];
    }

    //! @brief      Gets the element at the location
    //! @param[in]  loc - the location of the element to get
    //! @return     reference to the element at the location
    //! @exception  std::runtime_error if NArray is empty
    //! @exception  std::out_of_range if loc element invalid
    //!
    //! Use arr.at({x, y, z, ...}) instead of arr[x][y][z][]... 
    reference  at(const Point<N>& loc)
    {
      if (empty()) 
        throw std::runtime_error("at() invalid call on empty NArray");

      T* ptr = m_base;
      for (dim_t i = 0; i < N; ++i)
        if (loc[i] >= m_dims[i] || loc[i] < 0)
          throw std::out_of_range("at() element larger then dimensions");
        else
          ptr += loc[i] * m_step[i];

      return *ptr;
    }
    creference at(const Point<N>& pt) const
    {
      if (empty()) 
        throw std::runtime_error("at() invalid call on empty NArray");

      T* ptr = m_base;
      for (dim_t i = 0; i < N; ++i)
        if (pt[i] >= m_dims[i] || pt[i] < 0)
          throw std::out_of_range("at() element larger then dimensions");
        else
          ptr += pt[i] * m_step[i];

      return *ptr;
    }

    //! @brief      Gets the N-1 dimension slice at the location
    //! @param[in]  x, y, z, w - the location to get the slice
    //! @return     N-1 NArray that references the same data
    //! @exception  std::out_of_range if x|y|z|w invalid
    //! @exception  std::domain_error if call for invalid N
    //!
    //! Equivalent to sliceN(x|y|z|w, n)
    NArray<value , N-1> sliceX(pos_t x)
    {
      if (x >= m_dims[0] || x < 0)
        throw std::out_of_range("sliceX(x) index out of bounds");

      return _nSlice(x, 0);
    }
    const NArray<cvalue, N-1> sliceX(pos_t x) const
    {
      if (x >= m_dims[0] || x < 0)
        throw std::out_of_range("sliceX(x) index out of bounds");

      return _nSlice(x, 0);
    }
    NArray<value , N-1> sliceY(pos_t y)
    {
      WILT_STATIC_ASSERT(N >= 2, "sliceY() only valid when N >= 2");

      if (y >= m_dims[1] || y < 0)
        throw std::out_of_range("sliceY(y) index out of bounds");

      return _nSlice(y, 1);
    }
    const NArray<cvalue, N-1> sliceY(pos_t y) const
    {
      WILT_STATIC_ASSERT(N >= 2, "sliceY() only valid when N >= 2");

      if (y >= m_dims[1] || y < 0)
        throw std::out_of_range("sliceY(y) index out of bounds");

      return _nSlice(y, 1);
    }
    NArray<value , N-1> sliceZ(pos_t z)
    {
      WILT_STATIC_ASSERT(N >= 3, "sliceZ() only valid when N >= 3");

      if (z >= m_dims[2] || z < 0)
        throw std::out_of_range("sliceZ(z) index out of bounds");

      return _nSlice(z, 2);
    }
    const NArray<cvalue, N-1> sliceZ(pos_t z) const
    {
      WILT_STATIC_ASSERT(N >= 3, "sliceZ() only valid when N >= 3");

      if (z >= m_dims[2] || z < 0)
        throw std::out_of_range("sliceZ(z) index out of bounds");

      return _nSlice(z, 2);
    }
    NArray<value , N-1> sliceW(pos_t w)
    {
      WILT_STATIC_ASSERT(N >= 4, "sliceW() only valid when N >= 4");

      if (w >= m_dims[3] || w < 0)
        throw std::out_of_range("sliceW(w) index out of bounds");

      return _nSlice(w, 3);
    }
    const NArray<cvalue, N-1> sliceW(pos_t w) const
    {
      WILT_STATIC_ASSERT(N >= 4, "sliceW() only valid when N >= 4");

      if (w >= m_dims[3] || w < 0)
        throw std::out_of_range("sliceW(w) index out of bounds");

      return _nSlice(w, 3);
    }

    //! @brief      Gets the N-1 dimension slice at the location
    //! @param[in]  n - the location to get the slice
    //! @param[in]  dim - the dimension of the slice
    //! @return     N-1 NArray that references the same data
    //! @exception  std::out_of_range if dim or n invalid
    NArray<value , N-1> sliceN(pos_t n, dim_t dim)
    {
      if (dim >= N || n >= m_dims[dim] || n < 0)
        throw std::out_of_range("nSlice(n, dim) index out of bounds");

      return _nSlice(n, dim);
    }
    const NArray<cvalue, N-1> sliceN(pos_t n, dim_t dim) const
    {
      if (dim >= N || n >= m_dims[dim] || n < 0)
        throw std::out_of_range("nSlice(n, dim) index out of bounds");

      return _nSlice(n, dim);
    }

    //! @brief      Gets the NArray at the position
    //! @param[in]  pos - position of the NArray, dimensionality must be less
    //!             than N and positive
    //! @return     the NArray at the position
    //! @exception  std::domain_error if incorrect M
    //! @exception  std::out_of_range if pos element invalid
    template <dim_t M>
    NArray<value, N-M> arrayAt(const Point<M>& pos)
    {
      WILT_STATIC_ASSERT(M<N && M>0, "arrayAt(): pos is not less than N");

      type* base = m_base;
      for (dim_t i = 0; i < M; ++i)
        if (pos[i] >= m_dims[i] || pos[i] < 0)
          throw std::out_of_range("arrayAt(): pos out of range");
        else
          base += m_step[i] * pos[i];

      return NArray<value, N-M>(m_header, base, _chopHigh<N-M>(m_dims), _chopHigh<N-M>(m_step));
    }

    template <dim_t M>
    const NArray<cvalue, N-M> arrayAt(const Point<M>& pos) const
    {
      WILT_STATIC_ASSERT(M<N && M>0, "arrayAt(): pos is not less than N");

      type* base = m_base;
      for (dim_t i = 0; i < M; ++i)
        if (pos[i] >= m_dims[i] || pos[i] < 0)
          throw std::out_of_range("arrayAt(): pos out of range");
        else
          base += m_step[i] * pos[i];

      return NArray<cvalue, N-M>(m_header, base, _chopHigh<N-M>(m_dims), _chopHigh<N-M>(m_step));
    }

    //! @brief      Gets a NArray within the range specified along the dimension
    //! @param[in]  x, y, z, w - the starting index of the range
    //! @param[in]  length - the length of the range
    //! @return     N-1 NArray that references the same data
    //! @exception  std::out_of_range if x|y|z|w invalid
    //! @exception  std::domain_error if call for invalid N
    //!
    //! Equivalent to rangeN(x|y|z|w, length, n)
    NArray<value , N> rangeX(pos_t x, pos_t length)
    {
      if (x >= m_dims[0] || x+length > m_dims[0] || length <= 0 || x < 0)
        throw std::out_of_range("xRange(x, length) index out of bounds");

      return _nRange(x, length, 0);
    }
    const NArray<cvalue, N> rangeX(pos_t x, pos_t length) const
    {
      if (x >= m_dims[0] || x+length > m_dims[0] || length <= 0 || x < 0)
        throw std::out_of_range("xRange(x, length) index out of bounds");

      return _nRange(x, length, 0);
    }
    NArray<value , N> rangeY(pos_t y, pos_t length)
    {
      WILT_STATIC_ASSERT(N >= 2, "rangeY() only valid when N >= 2");

      if (y >= m_dims[1] || y+length > m_dims[1] || length <= 0 || y < 0)
        throw std::out_of_range("yRange(y, length) index out of bounds");

      return _nRange(y, length, 1);
    }
    const NArray<cvalue, N> rangeY(pos_t y, pos_t length) const
    {
      WILT_STATIC_ASSERT(N >= 2, "rangeY() only valid when N >= 2");

      if (y >= m_dims[1] || y+length > m_dims[1] || length <= 0 || y < 0)
        throw std::out_of_range("yRange(y, length) index out of bounds");

      return _nRange(y, length, 1);
    }
    NArray<value , N> rangeZ(pos_t z, pos_t length)
    {
      WILT_STATIC_ASSERT(N >= 3, "rangeZ() only valid when N >= 3");

      if (z >= m_dims[2] || z+length > m_dims[2] || length <= 0 || z < 0)
        throw std::out_of_range("zRange(z, length) index out of bounds");

      return _nRange(z, length, 2);
    }
    const NArray<cvalue, N> rangeZ(pos_t z, pos_t length) const
    {
      WILT_STATIC_ASSERT(N >= 3, "rangeZ() only valid when N >= 3");

      if (z >= m_dims[2] || z+length > m_dims[2] || length <= 0 || z < 0)
        throw std::out_of_range("zRange(z, length) index out of bounds");

      return _nRange(z, length, 2);
    }
    NArray<value , N> rangeW(pos_t w, pos_t length)
    {
      WILT_STATIC_ASSERT(N >= 4, "rangeW() only valid when N >= 4");

      if (w >= m_dims[3] || w+length > m_dims[3] || length <= 0 || w < 0)
        throw std::out_of_range("wRange(w, length) index out of bounds");

      return _nRange(w, length, 3);
    }
    const NArray<cvalue, N> rangeW(pos_t w, pos_t length) const
    {
      WILT_STATIC_ASSERT(N >= 4, "rangeW() only valid when N >= 4");

      if (w >= m_dims[3] || w+length > m_dims[3] || length <= 0 || w < 0)
        throw std::out_of_range("wRange(w, length) index out of bounds");

      return _nRange(w, length, 3);
    }

    //! @brief      Gets a NArray within the range specified along the dimension
    //! @param[in]  n - the starting index of the range
    //! @param[in]  length - the length of the range
    //! @param[in]  dim - the dimension of the range
    //! @return     N-1 NArray that references the same data
    //! @exception  std::out_of_range if dim, n, or length invalid
    NArray<value , N> rangeN(pos_t n, pos_t length, dim_t dim)
    {
      if (n >= m_dims[dim] || n+length > m_dims[dim] || length <= 0 || n < 0 || dim >= N)
        throw std::out_of_range("nRange(n, length, dim) index out of bounds");

      return _nRange(n, length, dim);
    }
    const NArray<cvalue, N> rangeN(pos_t n, pos_t length, dim_t dim) const
    {
      if (n >= m_dims[dim] || n+length > m_dims[dim] || length <= 0 || n < 0 || dim >= N)
        throw std::out_of_range("nRange(n, length, dim) index out of bounds");

      return _nRange(n, length, dim);
    }

    //! @brief      Gets an NArray withthe first two dimensions swapped
    //! @return     NArray that references the data
    //! @exception  std::domain_error if call for invalid N
    //!
    //! t() is identical to t(0, 1)
    NArray<value , N> t()
    {
      WILT_STATIC_ASSERT(N >= 2, "t() only valid when N >= 2");

      return t(0, 1);
    }
    const NArray<cvalue, N> t() const
    {
      WILT_STATIC_ASSERT(N >= 2, "t() only valid when N >= 2");

      return t(0, 1);
    }

    //! @brief      Gets an NArray with two dimensions swapped
    //! @param[in]  dim1 - first dimension to swap
    //! @param[in]  dim2 - second dimension to swap
    //! @return     NArray that references the same data
    //! @exception  std::out_of_range if dim1 or dim2 invalid
    NArray<value , N> t(dim_t dim1, dim_t dim2)
    {
      if (dim1 >= N || dim2 >= N)
        throw std::out_of_range("t(dim1, dim2) index out of bounds");

      return NArray<value, N>(m_header, m_base, _swap(m_dims, dim1, dim2), _swap(m_step, dim1, dim2));
    }
    const NArray<cvalue, N> t(dim_t dim1, dim_t dim2) const
    {
      if (dim1 >= N || dim2 >= N)
        throw std::out_of_range("t(dim1, dim2) index out of bounds");

      return NArray<cvalue, N>(m_header, m_base, _swap(m_dims, dim1, dim2), _swap(m_step, dim1, dim2));
    }

    //! @brief      Gets an NArray with a dimension reversed
    //! @return     NArray that references the same data
    //! @exception  std::domain_error if call for invalid N
    //!
    //! Equivalent to flipN(0|1|2|3)
    NArray<value , N> flipX()
    {
      return _nFlip(0);
    }
    const NArray<cvalue, N> flipX() const
    {
      return _nFlip(0);
    }
    NArray<value , N> flipY()
    {
      WILT_STATIC_ASSERT(N >= 2, "flipY() only valid when N >= 2");

      return _nFlip(1);
    }
    const NArray<cvalue, N> flipY() const
    {
      WILT_STATIC_ASSERT(N >= 2, "flipY() only valid when N >= 2");

      return _nFlip(1);
    }
    NArray<value , N> flipZ()
    {
      WILT_STATIC_ASSERT(N >= 3, "flipZ() only valid when N >= 3");

      return _nFlip(2);
    }
    const NArray<cvalue, N> flipZ() const
    {
      WILT_STATIC_ASSERT(N >= 3, "flipZ() only valid when N >= 3");

      return _nFlip(2);
    }
    NArray<value , N> flipW()
    {
      WILT_STATIC_ASSERT(N >= 4, "flipW() only valid when N >= 4");

      return _nFlip(3);
    }
    const NArray<cvalue, N> flipW() const
    {
      WILT_STATIC_ASSERT(N >= 4, "flipW() only valid when N >= 4");

      return _nFlip(3);
    }

    //! @brief      Gets an NArray with a dimension reversed
    //! @param[in]  dim - the dimension to flip
    //! @return     NArray that references the same data
    //! @exception  std::out_of_range if dim1 or dim2 invalid
    NArray<value , N> flipN(dim_t dim)
    {
      if (dim >= N)
        throw std::out_of_range("nFlip(dim) index out of bounds");

      return _nFlip(dim);
    }
    const NArray<cvalue, N> flipN(dim_t dim) const
    {
      if (dim >= N)
        throw std::out_of_range("nFlip(dim) index out of bounds");

      return _nFlip(dim);
    }

    NArray<value, N> subarray(const Point<N>& loc, const Point<N>& size)
    {
      type* base = m_base;
      for (dim_t i = 0; i < N; ++i)
      {
        if (size[i]+loc[i] > m_dims[i] || size[i] <= 0 || loc[i] < 0 || loc[i] >= m_dims[i])
          throw std::out_of_range("subarray() index out of bounds");
        base += m_step[i] * loc[i];
      }

      return NArray<value, N>(m_header, base, size, m_step);
    }
    const NArray<cvalue, N> subarray(const Point<N>& loc, const Point<N>& size) const
    {
      type* base = m_base;
      for (dim_t i = 0; i < N; ++i)
      {
        if (size[i]+loc[i] > m_dims[i] || size[i] <= 0 || loc[i] < 0 || loc[i] >= m_dims[i])
          throw std::out_of_range("subarray() index out of bounds");
        base += m_step[i] * loc[i];
      }

      return NArray<cvalue, N>(m_header, base, size, m_step);
    }

    template <class Operator>
    void apply(Operator op)
    {
      _singleOp2((pointer)m_base, _dimsPtr(), _stepPtr(), op, N);
    }

    template <class Operator>
    void apply(Operator op) const
    {
      _singleOp2((cpointer)m_base, _dimsPtr(), _stepPtr(), op, N);
    }

    //! @brief      Converts the data to a new type using static_cast or custom
    //!             converter function
    //! @param[in]  func - the function or function object with the signature
    //!             U(const T&) or U(T)
    //! @return     NArray that references the converted data
    template <class U>
    NArray<U, N> convertTo() const
    {
      NArray<U, N> ret(m_dims);
      _convertTo(*this, ret, [](const T& t){return static_cast<U>(t);});
      return ret;
    }
    template <class U, class Converter>
    NArray<U, N> convertTo(Converter func) const
    {
      NArray<U, N> ret(m_dims);
      _convertTo(*this, ret, func);
      return ret;
    }

    //! @brief      Sets the data currently refenced to values in arr or a
    //!             constant
    //! @param[in]  arr - NArray to set values from, dimensions must match
    //! @param[in]  val - value to set to each element
    void setTo(const NArray<cvalue, N>& arr)
    {
      if (m_dims != arr.dims())
        throw std::invalid_argument("setTo(arr): dimensions must match");
      if (std::is_const<T>::value)
        throw std::domain_error("setTo(arr): cannot set to const type");

      _unaryOp2(m_base, arr._basePtr(), _dimsPtr(), _stepPtr(), arr._stepPtr(), 
        [](type& r, const type& v){r = v;}, N);
    }
    void setTo(const T& val)
    {
      if (std::is_const<T>::value)
        throw std::domain_error("setTo(val): cannot set to const type");

      _singleOp2(m_base, _dimsPtr(), _stepPtr(), [&val](type& r){r = val;}, N);
    }

    //! @brief      Sets the data currently refenced to values in arr or a
    //!             constant where the mask != 0
    //! @param[in]  arr - NArray to set values from, dimensions must match
    //! @param[in]  val - value to set to each element
    //! @param[in]  mask - uchar array where != 0 sets the corresponding value,
    //!             dimensions must match
    void setTo(const NArray<cvalue, N>& arr, const NArray<uint8_t, N>& mask)
    {
      if (m_dims != arr.dims() || m_dims != mask.dims())
        throw std::invalid_argument("setTo(arr, mask): dimensions must match");
      if (std::is_const<T>::value)
        throw std::domain_error("setTo(arr, mask): cannot set to const type");

      _binaryOp2(m_base, arr._basePtr(), mask._basePtr(), _dimsPtr(), _stepPtr(), arr._stepPtr(), mask._stepPtr(), 
        [](type& r, const type& v, uint8_t m){if (m != 0) r = v;}, N);
    }
    void setTo(const T& val, const NArray<uint8_t, N>& mask)
    {
      if (std::is_const<T>::value)
        throw std::domain_error("setTo(val): cannot set to const type");

      _unaryOp2(m_base, mask._basePtr(), _dimsPtr(), _stepPtr(), mask._stepPtr(), 
        [&val](type& r, uint8_t m){if (m != 0) r = val;}, N);
    }

    //! @brief      copies the data referenced into a new NArray
    //! @return     NArray that references a copy of the data
    NArray<type, N> clone() const
    {
      NArray<type, N> ret(m_dims);
      ret.setTo(*this);
      return ret;
    }

    //! @brief      creates a NArray that references the data in increasing order
    //!             in memory
    //! @return     NArray that references the data
    //!
    //! Usefull for traversing data efficiently when order doesn't matter
    NArray<value , N> aligned()
    {
      Point<N> dims = m_dims;
      Point<N> step = m_step;
      pos_t offset = _align(dims, step);

      return NArray<value, N>(m_header, m_base + offset, dims, step);
    }
    const NArray<cvalue, N> aligned() const
    {
      Point<N> dims = m_dims;
      Point<N> step = m_step;
      pos_t offset = _align(dims, step);

      return NArray<cvalue, N>(m_header, m_base + offset, dims, step);
    }

    //! @brief      clears this object, deallocates the data if its the last
    //!             reference
    void clear()
    {
      m_header.clear();
      m_base = nullptr;

      _clean();
    }

    //! @brief      returns whether the data is continuous
    //! @return     true if data is continuous, false if gaps exist
    //!
    //! if the NArray is continuous and aligned, you can traverse the data
    //! linearly starting from _basePtr()
    bool isContinuous() const
    {
      pos_t stepSize = 0;
      for (dim_t i = 0; i < N; ++i)
        stepSize += m_step[i]*(m_dims[i]-1);

      return stepSize+1 == this->size();
    }

    //! @brief      returns whether the data is part of a larger NArray
    //! @return     true if data accessible is less than data shared, false if
    //!             otherwise or if empty
    bool isSubmatrix() const
    {
      if (empty())
        return false;
      else
        return (std::size_t)size() < m_header->size;
    }

    //! @brief      returns whether the data is accessed linearly in memory
    //! @return     true if internal steps are positive and descreasing, false
    //!             otherwise
    //!
    //! if the NArray is continuous and aligned, you can traverse the data
    //! linearly starting from _basePtr()
    bool isAligned() const
    {
      for (dim_t i = 0; i < N; ++i)
        if (m_step[i] <= 0)
          return false;
      for (dim_t i = 1; i < N; ++i)
        if (m_step[i-1] < m_step[i])
          return false;
      return true;
    }

    //! @brief      returns whether the data is unique to this object
    //! @return     true if only this references the shared data, false if empty
    //!             or other references exist
    bool isUnique() const
    {
      return m_header._ptr() && m_header->refs == 1;
    }

    //! @brief      returns an iterator pointing to the beginning
    iterator begin()
    {
      return iterator(*this);
    }
    const_iterator begin() const
    {
      return const_iterator(*this);
    }
    const_iterator cbegin() const
    {
      return const_iterator(*this);
    }

    //! @brief      returns an iterator pointing past the end
    iterator end()
    {
      return iterator(*this, size());
    }
    const_iterator end() const
    {
      return const_iterator(*this, size());
    }
    const_iterator cend() const
    {
      return const_iterator(*this, size());
    }

    //! @brief      gets a pointer to the start of the referenced data
    pointer  _basePtr()
    {
      return m_base;
    }
    cpointer _basePtr() const
    {
      return m_base;
    }

    //! @brief      gets the pointer to the array array, size N
    const pos_t* _dimsPtr() const
    {
      return &m_dims[0];
    }

    //! @brief      gets the pointer of the step array, size N
    const pos_t* _stepPtr() const
    {
      return &m_step[0];
    }

    friend class NArray<value, N+1>;
    friend class NArray<typename std::toggle_const<T>::type, N>;
    friend class NArray<typename std::toggle_const<T>::type, N+1>;
    friend class NArrayIterator<type, N>;
    friend class NArrayIterator<cvalue, N>;

  protected:
    NArrayHeader<type> m_header;
    type* m_base;
    Point<N> m_dims;
    Point<N> m_step;

    // Raw constructor
    NArray(NArrayHeader<type> header, type* base, Point<N> dims, Point<N> step)
      : m_header(header),
        m_base(base),
        m_dims(dims),
        m_step(step)
    {
      
    }

  private:
    NArray<value , N-1> _nSlice(pos_t n, dim_t dim)
    {
      return NArray<value, N-1>(m_header, m_base+m_step[dim]*n, _slice(m_dims, dim), _slice(m_step, dim));
    }
    const NArray<cvalue, N-1> _nSlice(pos_t n, dim_t dim) const
    {
      return NArray<cvalue, N-1>(m_header, m_base+m_step[dim]*n, _slice(m_dims, dim), _slice(m_step, dim));
    }
    NArray<value,  N> _nRange(pos_t n, pos_t length, dim_t dim)
    {
      Point<N> temp = m_dims;
      temp[dim] = length;
      return NArray<value, N>(m_header, m_base+m_step[dim]*n, temp, m_step);
    }
    const NArray<cvalue, N> _nRange(pos_t n, pos_t length, dim_t dim) const
    {
      Point<N> temp = m_dims;
      temp[dim] = length;
      return NArray<cvalue, N>(m_header, m_base+m_step[dim]*n, temp, m_step);
    }
    NArray<value,  N> _nFlip(dim_t dim)
    {
      Point<N> temp = m_step;
      temp[dim] = -temp[dim];
      return NArray<value, N>(m_header, m_base+m_step[dim]*(m_dims[dim]-1), m_dims, temp);
    }
    const NArray<cvalue, N> _nFlip(dim_t dim) const
    {
      Point<N> temp = m_step;
      temp[dim] = -temp[dim];
      return NArray<cvalue, N>(m_header, m_base+m_step[dim]*(m_dims[dim]-1), m_dims, temp);
    }

    bool _valid() const
    {
      for (dim_t i = 0; i < N; ++i)
        if (m_dims[i] == 0)
          return false;
        else if (m_dims[i] < 0)
          throw std::invalid_argument("dimension cannot be negative");
      return true;
    }

    void _create()
    {
      pos_t size = _size(m_dims);
      if (size > 0)
      {
        m_header = NArrayHeader<type>(size);
        m_base = m_header->data;
      }
    }

    void _create(const T& val)
    {
      pos_t size = _size(m_dims);
      if (size > 0)
      {
        m_header = NArrayHeader<type>(size, val);
        m_base = m_header->data;
      }
    }
    
    void _create(T* ptr, PTR ltype)
    {
      pos_t size = _size(m_dims);
      if (size > 0)
      {
        m_header = NArrayHeader<type>(size, ptr, ltype);
        m_base = m_header->data;
      }
    }

    template <class Generator>
    void _create(Generator gen)
    {
      pos_t size = _size(m_dims);
      if (size > 0)
      {
        m_header = NArrayHeader<type>(size, gen);
        m_base = m_header->data;
      }
    }

    void _clean()
    {
      m_dims.clear();
      m_step.clear();
    }

    template <class U, class Converter>
    static void _convertTo(const wilt::NArray<value, N>& lhs, wilt::NArray<U, N>& rhs, Converter func)
    {
      Point<N> dims = lhs.dims();
      Point<N> step1 = lhs.step();
      Point<N> step2 = rhs.step();
      dim_t n = _condense(dims, step1, step2);
      _unaryOp(rhs._basePtr(), lhs._basePtr(), &dims[0], &step2[0], &step1[0], func, n);
    }

  }; // class NArray

  template <class T>
  class NArray<T, 0>
  {
  public:
    typedef typename std::remove_const<T>::type type;

    NArray(const NArrayHeader<T>& header, type* base, const Point<0>&, const Point<0>&)
      : m_header(header),
        m_base(base)
    {

    }

    NArray<T, 0>& operator= (const T& t)
    {
      WILT_STATIC_ASSERT(!std::is_const<T>::value, "cannot assign to a const value");

      if (!m_header._ptr())
        throw std::domain_error("NArray<T, 0> references no data");

      *m_base = t;

      return *this;
    }

    operator T() const
    {
      if (!m_header._ptr())
        throw std::runtime_error("Mat<T, 0> references no data");

      return *m_base;
    }

  private:
    NArray() { }

    NArrayHeader<T> m_header;
    type* m_base;

  }; // class NArray<T, 0>

  //! @brief         applies an operation on two source arrays and stores the
  //!                result in a destination array
  //! @param[in]     src1 - 1st source array
  //! @param[in]     src2 - 2nd source array
  //! @param[in]     op - function or function object with the signature 
  //!                T(U, V) or similar
  //! @return        the destination array
  template <class T, class U, class V, dim_t N, class Operator>
  NArray<T, N> binaryOp(const NArray<U, N>& src1, const NArray<V, N>& src2, Operator op)
  {
    NArray<T, N> ret(src1.dims());
    _binaryOp(ret._basePtr(), src1._basePtr(), src2._basePtr(), ret._dimsPtr(), 
              ret._stepPtr(), src1._stepPtr(), src2._stepPtr(), op, N);
    return ret;
  }

  //! @brief         applies an operation on two source arrays and stores the
  //!                result in a destination array
  //! @param[in,out] dst - the destination array
  //! @param[in]     src1 - 1st source array
  //! @param[in]     src2 - 2nd source array
  //! @param[in]     op - function or function object with the signature 
  //!                (T&, U, V) or similar
  template <class T, class U, class V, dim_t N, class Operator>
  void binaryOp(NArray<T, N>& dst, const NArray<U, N>& src1, const NArray<V, N>& src2, Operator op)
  {
    _binaryOp2(dst._basePtr(), src1._basePtr(), src2._basePtr(), dst._dimsPtr(), 
               dst._stepPtr(), src1._stepPtr(), src2._stepPtr(), op, N);
  }

  //! @brief         applies an operation on a source array and stores the
  //!                result in a destination array
  //! @param[in]     src - pointer to 1st source array
  //! @param[in]     op - function or function object with the signature 
  //!                T(U) or similar
  //! @return        the destination array
  template <class T, class U, dim_t N, class Operator>
  NArray<T, N> unaryOp(const NArray<U, N>& src, Operator op)
  {
    NArray<T, N> ret(src.dims());
    _unaryOp(ret._basePtr(), src._basePtr(), ret._dimsPtr(), 
             ret._stepPtr(), src._stepPtr(), op, N);
    return ret;
  }

  //! @brief         applies an operation on a source array and stores the
  //!                result in a destination array
  //! @param[in,out] dst - the destination array
  //! @param[in]     src - pointer to 1st source array
  //! @param[in]     op - function or function object with the signature 
  //!                (T&, U) or similar
  template <class T, class U, dim_t N, class Operator>
  void unaryOp(NArray<T, N>& dst, const NArray<U, N>& src, Operator op)
  {
    _unaryOp2(dst._basePtr(), src._basePtr(), dst._dimsPtr(), 
              dst._stepPtr(), src._stepPtr(), op, N);
  }

  template <class T, dim_t N>
  typename sum_t<T>::type sum(const NArray<T, N>& src)
  {
    typename sum_t<T>::type sum = typename sum_t<T>::type();
    src.apply([&sum](const T& t){ sum += t; });
    return sum;
  }

  template <class T, dim_t N>
  T max(const NArray<T, N>& src)
  {
    typename std::remove_const<T>::type max = src.at(Point<N>());
    src.apply([&max](const T& t){ if (t > max) max = t; });
    return max;
  }

  template <class T, dim_t N>
  Point<N> maxAt(const NArray<T, N>& src)
  {
    typename std::remove_const<T>::type max = src.at(Point<N>());
    pos_t i = 0, idx = 0;
    src.apply([&max,&idx,&i](const T& t){ if (t > max) { max = t; idx = i; } ++i; });
    return _idx2pos(src.dims(), idx);
  }

  template <class T, dim_t N>
  T min(const NArray<T, N>& src)
  {
    typename std::remove_const<T>::type min = src.at(Point<N>());
    src.apply([&min](const T& t){ if (t < min) min = t; });
    return min;
  }

  template <class T, dim_t N>
  Point<N> minAt(const NArray<T, N>& src)
  {
    typename std::remove_const<T>::type min = src.at(Point<N>());
    pos_t i = 0, idx = 0;
    src.apply([&min,&idx,&i](const T& t){ if (t < min) { min = t; idx = i; } ++i; });
    return _idx2pos(src.dims(), idx);
  }

  template <class T, dim_t N>
  typename sum_t<T>::type mean(const NArray<T, N>& src)
  {
    return sum(src) / src.size();
  }

  template <class T, dim_t N>
  T median(const NArray<T, N>& src)
  {
    int n = src.size();
    const T** ptrs = new const T*[n];
    pos_t i = 0;
    src.apply([&i,&ptrs](const T& t){ ptrs[i] = &t; ++i; });

    // fastNth algorithm with N = n/2
    int A = 0;
    int B = n-1;
    while (A < B)
    {
      int a = A;
      int b = B;

      const T* x = ptrs[n/2];
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

    const T* ret = ptrs[n/2];
    delete[] ptrs;
    return *ret;
  }

  template <class T, dim_t N>
  pos_t count(const NArray<T, N>& src)
  {
    pos_t cnt = 0;
    src.apply([&cnt](const T& t){ if(t) ++cnt; });
    return cnt;
  }

WILT_COMMON_END
WILT_END

// additional
#include "narrayiterator.hpp"
#include "operators.hpp"

// WILT_NARRAY_HPP
#endif