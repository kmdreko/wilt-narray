////////////////////////////////////////////////////////////////////////////////
// FILE: narray.hpp
// DATE: 2014-07-30
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Defines an N-dimensional templated array class

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

#ifndef WILT_NARRAY_HPP
#define WILT_NARRAY_HPP

#include <memory>
// - std::allocator
#include <type_traits>
// - std::enable_if
// - std::is_const
// - std::remove_const
// - std::is_same
#include <initializer_list>
// - std::initializer list

#include "util.h"
#include "point.hpp"
#include "narraydatablock.hpp"

namespace wilt
{
  template <class T, std::size_t N, std::size_t M = 0> class NArrayIterator;
  // - defined in "narrayiterator.hpp"

  template <class T, std::size_t N, std::size_t M> class SubNArrays;
  // - defined below

  //////////////////////////////////////////////////////////////////////////////
  // This class is designed to access a sequence of data and to manipulate it in
  // an N-dimensional manner.
  // 
  // This class works by keeping a reference to a sequence of Ts that is shared
  // between NArrays to avoid copying data and allow separate slices and
  // subarrays to manipulate the same data. It also keeps a separate pointer to
  // the base of its own segment of the shared data.
  // 
  // An NArray will keep its own size of each dimension as well as its own step
  // values. By step value, I mean it keeps track of the distance to the next
  // value in the shared data along that dimension. Keeping this for each 
  // dimensions is what allows slicing, flipping, transposing, etc. without 
  // needing to copy data. Slicing works by dropping a dimension/step. Flipping
  // just negates its step value. Transposing swaps dimension/step values. 
  // Ranges and subarrays just reduce the dimensions. Each operation does adjust
  // its base pointer to ensure it still covers the proper segment of data.
  // 
  // NArray<int, 3>({ 4, 3, 2 })        creates an NArray like so:
  //   .rangeX(1, 3)
  //   .flipY()                         dimensions = {  3,  2,  3 }
  //   .transpose(1, 2);                steps      = {  6,  1, -2 }
  // x--x--x--x--x--x--2--5--1--4--0--3--8--11-7--10-6--9--14-17-13-16-12-15
  //                               |data
  // 
  // NArray<int, 2>({ 4, 6 })           creates an NArray like so:
  //   .sliceY(1);
  //                                    dimensions = {  4 }
  //                                    steps      = {  6 }
  // x--0--x--x--x--x--x--1--x--x--x--x--x--2--x--x--x--x--x--3--x--x--x--x
  //    |data
  // 
  // These cases show how a single data-set can have multiple representations
  // but also how the data can be fragmented and unordered, but there are ways
  // to see and adjust it. For example, isContiguous() will return whether there
  // are gaps or not. The function isAligned() will return if the data would be
  // accessed in increasing order and asAligned() will return an NArray with the
  // same view that would be accessed in increasing order.
  //
  // Because the data is a shared reference, NArrays have the semantics of
  // shared_ptr meaning that even though the NArray is const, the data isn't and
  // can still be modified. Which is why most functions, even those that modify
  // the data are marked as const. To provide const protection, the NArray needs
  // to be converted to a 'const T' array. This conversion is allowed implicitly
  // but can be called more explicitly by 'asConst()'.
  // 
  // As a side note, all manipulations are thread-safe; two threads can safely
  // use the same data-set, but modification of data is not protected.

  template <class T, std::size_t N>
  class NArray
  {
  public:
    ////////////////////////////////////////////////////////////////////////////
    // TYPE DEFINITIONS
    ////////////////////////////////////////////////////////////////////////////

    using type = typename std::remove_const<T>::type;
    using value = T;
    using cvalue = const T;
    using pointer = T*;
    using cpointer = const T*;
    using reference = T&;
    using creference = const T&;

    using iterator = NArrayIterator<value, N>;
    using const_iterator = NArrayIterator<cvalue, N>;

    using exposed_type = NArray<T, N>;

  private:
    ////////////////////////////////////////////////////////////////////////////
    // PRIVATE MEMBERS
    ////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<type> data_; // pointer to referenced data
    Point<N> sizes_;             // dimension sizes
    Point<N> steps_;             // step sizes

  public:
    ////////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    ////////////////////////////////////////////////////////////////////////////

    // Default constructor, makes an empty NArray.
    NArray();

    // Creates an array of the given size, elements are default constructed.
    explicit NArray(const Point<N>& size);

    // Creates an array of the given size, elements are copy constructed from
    // 'val'.
    NArray(const Point<N>& size, const T& val);

    // Creates an array of the given size, elements are constructed or not 
    // based on 'type'
    //   - ASSUME = uses provided data, will delete when complete
    //   - COPY   = copies the data
    //   - REFERENCE:   = uses provided data, will not delete when complete
    //
    // NOTE: ptr must be at least the size indicated by 'size'
    NArray(const Point<N>& size, T* ptr, NArrayDataAcquireType atype);

    // Creates an array of the given size, elements are copy constructed from
    // the list
    //
    // NOTE: ptr must be at least the size indicated by size
    NArray(const Point<N>& size, std::initializer_list<T> list);

    // Creates an array of the given size, elements are constructed from the
    // result of 'gen()'
    //
    // NOTE: 'gen()' is called once per element
    template <class Generator>
    NArray(const Point<N>& size, Generator gen);

    template <class Iterator>
    NArray(const Point<N>& size, Iterator first, Iterator last);

    // Copy and move constructor, shares data and uses the same data segment as
    // 'arr'
    //
    // NOTE: 'arr' is empty after being moved
    NArray(const NArray<T, N>& arr);
    NArray(NArray<T, N>&& arr);

    // Copy and move constructor from 'T' to 'const T'
    //
    // NOTE: 'arr' is empty after being moved
    // NOTE: only exists on 'const T' arrays
    template <class U, typename = std::enable_if<std::is_const<T>::value && std::is_same<U, std::remove_const<T>::type>::type>::type>
    NArray(const NArray<U, N>& arr);
    template <class U, typename = std::enable_if<std::is_const<T>::value && std::is_same<U, std::remove_const<T>::type>::type>::type>
    NArray(NArray<U, N>&& arr);

  public:
    ////////////////////////////////////////////////////////////////////////////
    // ASSIGNMENT OPERATORS
    ////////////////////////////////////////////////////////////////////////////

    // standard copy and assignment, assumes the same data segment as 'arr'. 
    // Original data reference is abandoned (if it doesn't hold the last 
    // reference) or destroyed (if it holds the last reference).
    //
    // NOTE: 'arr' is empty after being moved
    NArray<T, N>& operator= (const NArray<T, N>& arr);
    NArray<T, N>& operator= (NArray<T, N>&& arr);

    // copy and assignment from 'T' to 'const T', only on non-const T templates.
    // Original data reference is abandoned (if it doesn't hold the last 
    // reference) or destroyed (if it holds the last reference).
    //
    // NOTE: 'arr' is empty after being moved
    template <class U, typename = std::enable_if<std::is_const<T>::value && std::is_same<U, std::remove_const<T>::type>::type>::type>
    NArray<T, N>& operator= (const NArray<U, N>& arr);
    template <class U, typename = std::enable_if<std::is_const<T>::value && std::is_same<U, std::remove_const<T>::type>::type>::type>
    NArray<T, N>& operator= (NArray<U, N>&& arr);

    // Element-wise modifying assigment operators, modifies the underlying data,
    // arrays must have the same dimensions
    NArray<T, N>& operator+= (const NArray<cvalue, N>& arr);
    NArray<T, N>& operator-= (const NArray<cvalue, N>& arr);

    // Element-wise modifying assigment operators, modifies the underlying data
    NArray<T, N>& operator+= (const T& val);
    NArray<T, N>& operator-= (const T& val);
    NArray<T, N>& operator*= (const T& val);
    NArray<T, N>& operator/= (const T& val);

  public:
    ////////////////////////////////////////////////////////////////////////////
    // QUERY FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////
    // functions only report the state of the array

    // Gets the dimension sizes and step values, see class description. They
    // determine the size of data and how that data is accessed.
    const Point<N>& sizes() const;
    const Point<N>& steps() const;

    // The total count of elements that can be accessed. This is simply the
    // compound of the dimension sizes.
    std::size_t size() const;

    // Functions for data reference
    //   - empty  = no data is referenced
    //   - unique = data is referenced and hold the only reference
    //   - shared = data is referenced and doesn't hold the only reference
    bool empty() const;
    bool unique() const;
    bool shared() const;

    // Convenience functions for dimension sizes (though may be confusing
    // depending on how the array is used)
    //   - width  = dimension 0
    //   - height = dimension 1
    //   - depth  = dimension 2
    //
    // NOTE: some functions are only available if they have that dimension
    pos_t width() const;
    pos_t height() const;
    pos_t depth() const;
    pos_t length(std::size_t dim) const;

    // Functions for determining the data organization for this array.
    //   - isContinuous = the array accesses data with no gaps
    //   - isAligned    = the array accesses data linearly
    bool isContinuous() const;
    bool isAligned() const;

  public:
    ////////////////////////////////////////////////////////////////////////////
    // ACCESS FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////
    // Functions for accessing different segments of the array.
    //
    // NOTE: any function that would return an NArray of dimension 0, will
    // instead return a T&.

    // Gets the element at that location.
    reference at(const Point<N>& loc) const;

    // Indexing operator, will return an N-1 NArray at the location 'x' along
    // the 0th dimension.
    //
    // NOTE: identical to 'sliceX(n)'
    // NOTE: 'arr.at({ x, y, z, ... })' is preferred over 'arr[x][y][z]' to
    // access elements because '[]' will create temporary NArrays
    typename NArray<T, N-1>::exposed_type operator[] (pos_t n) const;

    // Gets the N-1 dimension slice along the specified dimension
    //   - dim = specified dimension
    //   - X = 0th dimension
    //   - Y = 1st dimension
    //   - Z = 2nd dimension
    //   - W = 3rd dimension
    //
    // NOTE: some functions are only available if they have that dimension
    typename NArray<T, N-1>::exposed_type slice(std::size_t dim, pos_t n) const;
    typename NArray<T, N-1>::exposed_type sliceX(pos_t x) const;
    NArray<T, N-1> sliceY(pos_t y) const;
    NArray<T, N-1> sliceZ(pos_t z) const;
    NArray<T, N-1> sliceW(pos_t w) const;

    // Gets the subarray with range along the specified dimension
    //   - dim = specified dimension
    //   - X = 0th dimension
    //   - Y = 1st dimension
    //   - Z = 2nd dimension
    //   - W = 3rd dimension
    //
    // NOTE: some functions are only available if they have that dimension
    NArray<T, N> range(std::size_t dim, pos_t n, pos_t length) const;
    NArray<T, N> rangeX(pos_t x, pos_t length) const;
    NArray<T, N> rangeY(pos_t y, pos_t length) const;
    NArray<T, N> rangeZ(pos_t z, pos_t length) const;
    NArray<T, N> rangeW(pos_t w, pos_t length) const;

    // Gets an NArray with the specified dimension reversed
    //   - dim = specified dimension
    //   - X = 0th dimension
    //   - Y = 1st dimension
    //   - Z = 2nd dimension
    //   - W = 3rd dimension
    //   - N = specified dimension
    //
    // NOTE: some functions are only available if they have that dimension
    NArray<T, N> flip(std::size_t dim) const;
    NArray<T, N> flipX() const;
    NArray<T, N> flipY() const;
    NArray<T, N> flipZ() const;
    NArray<T, N> flipW() const;

    // Gets an NArray with skipping every 'n' indexes along that dimension,
    // optional 'start' that denotes where the skipping starts from
    //   - dim = specified dimension
    //   - X = 0th dimension
    //   - Y = 1st dimension
    //   - Z = 2nd dimension
    //   - W = 3rd dimension
    //
    // NOTE: some functions are only available if they have that dimension
    NArray<T, N> skip(std::size_t dim, pos_t n, pos_t start = 0) const;
    NArray<T, N> skipX(pos_t n, pos_t start = 0) const;
    NArray<T, N> skipY(pos_t n, pos_t start = 0) const;
    NArray<T, N> skipZ(pos_t n, pos_t start = 0) const;
    NArray<T, N> skipW(pos_t n, pos_t start = 0) const;

    // Gets an NArray with two dimensions swapped
    //
    // NOTE: 'transpose()' is identical to 'transpose(0, 1)'
    NArray<T, N> transpose() const;
    NArray<T, N> transpose(std::size_t dim1, std::size_t dim2) const;

    // Gets the subarray at that location and size
    //
    // NOTE: can use chain of 'rangeN()' to get the same result
    NArray<T, N> subarray(const Point<N>& loc, const Point<N>& size) const;

    // Gets the array at that location. M must be less than N.
    template <std::size_t M>
    typename NArray<T, N-M>::exposed_type subarrayAt(const Point<M>& pos) const;

    // Gets an iterable for all subarrays
    template <std::size_t M>
    SubNArrays<T, N, M> subarrays() const;

    // Transforms the array into a new size, typically to create sub-dimension
    // splits but can form it into any set of dimensions if allowed by the
    // segments representation. Any new dimensions will try to keep the element
    // ordering or throw if it can't, it won't create a new dataset.
    // 
    // NOTE: total sizes much match
    // NOTE: aligned and continuous arrays can be made into any shape
    template <std::size_t M>
    NArray<T, M> reshape(const Point<M>& size) const;

    // Creates an additional dimension of size {n} that repeats the same data
    NArray<T, N+1> repeat(pos_t n) const;

    // Creates an additional dimension that essentially creates a sliding window
    // along that dimension. It reduces that dimension by n+1 and creates a new
    // dimension with size n.
    //   - dim = specified dimension
    //   - X = 0th dimension
    //   - Y = 1st dimension
    //   - Z = 2nd dimension
    //   - W = 3rd dimension
    //
    // NOTE: some functions are only avaliable if they have that dimension
    // NOTE: adds the dimension to the end
    NArray<T, N+1> window(std::size_t dim, pos_t n) const;
    NArray<T, N+1> windowX(pos_t n) const;
    NArray<T, N+1> windowY(pos_t n) const;
    NArray<T, N+1> windowZ(pos_t n) const;
    NArray<T, N+1> windowW(pos_t n) const;

    // Iterators for all the elements in-order
    iterator begin() const;
    iterator end() const;

    // Iterates over all elements in-order and calls operator
    template <class Operator>
    void foreach(Operator op) const;

    // Gets a pointer to the segment base. Can be used to access the whole
    // segment if isContinuous() and isAligned() or by respecting sizes() and 
    // steps()
    T* base() const;

    // creates a constant version of the NArray, not strictly necessary since a
    // conversion constructor exists but is still nice to have.
    NArray<const T, N> asConst() const;

    // creates a NArray that references the data in increasing order in memory
    //
    // NOTE: may get a performance increase if the access order doesn't matter
    NArray<T, N> asAligned() const;

    // Creates an NArray that has its dimension and step values merged to their
    // most condensed form. A continuous and aligned array will be condensed to
    // a single dimension with others being 1.
    //
    // NOTE: this isn't really intended to be used like this, but is used
    // internally to reduce recursive calls and can be useful for reshaping.
    NArray<T, N> asCondensed() const;

  public:
    ////////////////////////////////////////////////////////////////////////////
    // TRANSFORMATION FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////
    // Functions designed to create a new dataset

    // Copies the data referenced into a new NArray
    NArray<typename std::remove_const<T>::type, N> clone() const;

    // Converts the NArray to a new data type either directly or with a
    // conversion function
    // 
    // NOTE: func should have the signature 'U(const T&)' or similar
    template <class U>
    NArray<U, N> convertTo() const;
    template <class U, class Converter>
    NArray<U, N> convertTo(Converter func) const;

    template <std::size_t M, class Compressor>
    NArray<T, M> compress(Compressor func) const;

  public:
    ////////////////////////////////////////////////////////////////////////////
    // MODIFIER FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    // Sets the data referenced to a given value or that of an array of the same
    // size with an optional mask
    void setTo(const NArray<const T, N>& arr) const;
    void setTo(const T& val) const;
    void setTo(const NArray<const T, N>& arr, const NArray<uint8_t, N>& mask) const;
    void setTo(const T& val, const NArray<uint8_t, N>& mask) const;

    // Clears the array by dropping its reference to the data, destructing it if
    // it was the last reference.
    void clear();

  private:
    ////////////////////////////////////////////////////////////////////////////
    // FRIEND DECLARATIONS
    ////////////////////////////////////////////////////////////////////////////

    template <class U, std::size_t M>
    friend class NArray;
    friend class NArrayIterator<type, N>;
    friend class NArrayIterator<cvalue, N>;

  private:
    ////////////////////////////////////////////////////////////////////////////
    // PRIVATE FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////

    NArray(std::shared_ptr<type> data, Point<N> sizes, Point<N> steps);

    typename NArray<T, N-1>::exposed_type slice_(std::size_t dim, pos_t n) const;
    NArray<T, N> range_(std::size_t dim, pos_t n, pos_t length) const;
    NArray<T, N> flip_(std::size_t dim) const;
    NArray<T, N> skip_(std::size_t dim, pos_t n, pos_t start) const;
    NArray<T, N+1> window_(std::size_t dim, pos_t n) const;

    template <class U, class Converter>
    static void convertTo_(const wilt::NArray<value, N>& lhs, wilt::NArray<U, N>& rhs, Converter func);

  }; // class NArray

  template <class T, std::size_t N, std::size_t M>
  class SubNArrays
  {
  private:
    NArray<T, N> array_;

  public:
    SubNArrays(const NArray<T, N>& arr) : array_(arr) {}

    NArrayIterator<T, N, M> begin() { return{ array_ }; }
    NArrayIterator<T, N, M> end() { return{ array_, size_(chopLow_<N - M>(array_.sizes())) }; }
  }; // class SubNArrays

  //////////////////////////////////////////////////////////////////////////////
  // This is only here to make it easier to do NArray<T, 1> slices. Making a
  // specialization that can be converted meant that the code only had to change
  // the return type, not the construction

  template <class T>
  class NArray<T, 0>
  {
  public:
    using type = typename std::remove_const<T>::type;
    using exposed_type = T&;

    NArray(std::shared_ptr<type> data, const Point<0>&, const Point<0>&)
      : data_(std::move(data))
    {

    }

    operator T&() const
    {
      if (!data_)
        throw std::runtime_error("Mat<T, 0> references no data");

      return *data_;
    }

  private:
    NArray() { }

    std::shared_ptr<type> data_;

  }; // class NArray<T, 0>

  //! @brief         applies an operation on two source arrays and stores the
  //!                result in a destination array
  //! @param[in]     src1 - 1st source array
  //! @param[in]     src2 - 2nd source array
  //! @param[in]     op - function or function object with the signature 
  //!                T(U, V) or similar
  //! @return        the destination array
  template <class T, class U, class V, std::size_t N, class Operator>
  NArray<T, N> binaryOp(const NArray<U, N>& src1, const NArray<V, N>& src2, Operator op)
  {
    NArray<T, N> ret(src1.sizes());
    binaryOp_(ret.base(), src1.base(), src2.base(), ret.sizes().data(), 
              ret.steps().data(), src1.steps().data(), src2.steps().data(), op, N);
    return ret;
  }

  //! @brief         applies an operation on two source arrays and stores the
  //!                result in a destination array
  //! @param[in,out] dst - the destination array
  //! @param[in]     src1 - 1st source array
  //! @param[in]     src2 - 2nd source array
  //! @param[in]     op - function or function object with the signature 
  //!                (T&, U, V) or similar
  template <class T, class U, class V, std::size_t N, class Operator>
  void binaryOp(NArray<T, N>& dst, const NArray<U, N>& src1, const NArray<V, N>& src2, Operator op)
  {
    binaryOp2_(dst.base(), src1.base(), src2.base(), dst.sizes().data(),
               dst.steps().data(), src1.steps().data(), src2.steps().data(), op, N);
  }

  //! @brief         applies an operation on a source array and stores the
  //!                result in a destination array
  //! @param[in]     src - pointer to 1st source array
  //! @param[in]     op - function or function object with the signature 
  //!                T(U) or similar
  //! @return        the destination array
  template <class T, class U, std::size_t N, class Operator>
  NArray<T, N> unaryOp(const NArray<U, N>& src, Operator op)
  {
    NArray<T, N> ret(src.sizes());
    unaryOp_(ret.base(), src.base(), ret.sizes().data(),
             ret.steps().data(), src.steps().data(), op, N);
    return ret;
  }

  //! @brief         applies an operation on a source array and stores the
  //!                result in a destination array
  //! @param[in,out] dst - the destination array
  //! @param[in]     src - pointer to 1st source array
  //! @param[in]     op - function or function object with the signature 
  //!                (T&, U) or similar
  template <class T, class U, std::size_t N, class Operator>
  void unaryOp(NArray<T, N>& dst, const NArray<U, N>& src, Operator op)
  {
    unaryOp2_(dst.base(), src.base(), dst.sizes().data(),
              dst.steps().data(), src.steps().data(), op, N);
  }

  template <class T, std::size_t N>
  NArray<T, N>::NArray()
    : data_()
    , sizes_()
    , steps_()
  {

  }

  template <class T, std::size_t N>
  NArray<T, N>::NArray(const Point<N>& size)
    : data_()
    , sizes_()
    , steps_()
  {
    if (!validSize_(size))
      return;

    sizes_ = size;
    steps_ = step_(size);
    data_ = std::make_shared<NArrayDataBlock<type>>(size_(size))->data();
  }

  template <class T, std::size_t N>
  NArray<T, N>::NArray(const Point<N>& size, const T& val)
    : data_()
    , sizes_()
    , steps_()
  {
    if (!validSize_(size))
      return;

    sizes_ = size;
    steps_ = step_(size);
    data_ = std::make_shared<NArrayDataBlock<type>>(size_(size), val)->data();
  }

  template <class T, std::size_t N>
  NArray<T, N>::NArray(const Point<N>& size, T* ptr, NArrayDataAcquireType atype)
    : data_()
    , sizes_()
    , steps_()
  {
    if (!validSize_(size))
      return;

    sizes_ = size;
    steps_ = step_(size);
    data_ = std::make_shared<NArrayDataBlock<type>>(size_(size), ptr, atype)->data();
  }

  template <class T, std::size_t N>
  NArray<T, N>::NArray(const Point<N>& size, std::initializer_list<T> list)
    : data_()
    , sizes_()
    , steps_()
  {
    if (!validSize_(size))
      return;

    sizes_ = size;
    steps_ = step_(size);
    data_ = std::make_shared<NArrayDataBlock<type>>(size_(size), list.begin(), list.end())->data();
  }

  template <class T, std::size_t N>
  template <class Generator>
  NArray<T, N>::NArray(const Point<N>& size, Generator gen)
    : data_()
    , sizes_()
    , steps_()
  {
    if (!validSize_(size))
      return;

    sizes_ = size;
    steps_ = step_(size);
    data_ = std::make_shared<NArrayDataBlock<type>>(size_(size), gen)->data();
  }

  template <class T, std::size_t N>
  template <class Iterator>
  NArray<T, N>::NArray(const Point<N>& size, Iterator first, Iterator last)
    : data_()
    , sizes_()
    , steps_()
  {
    if (!validSize_(size))
      return;

    sizes_ = size;
    steps_ = step_(size);
    data_ = std::make_shared<NArrayDataBlock<type>>(size_(size), first, last)->data();
  }

  template <class T, std::size_t N>
  NArray<T, N>::NArray(const NArray<T, N>& arr)
    : data_(arr.data_)
    , sizes_(arr.sizes_)
    , steps_(arr.steps_)
  {

  }

  template <class T, std::size_t N>
  NArray<T, N>::NArray(NArray<T, N>&& arr)
    : data_(arr.data_)
    , sizes_(arr.sizes_)
    , steps_(arr.steps_)
  {
    arr.clear();
  }

  template <class T, std::size_t N>
  template <class U, typename>
  NArray<T, N>::NArray(const NArray<U, N>& arr)
    : data_()
    , sizes_(arr.sizes_)
    , steps_(arr.steps_)
  {
    if constexpr (std::is_const<T>::value)
    {
      data_ = arr.data_;
    }
    else
    {
      data_ = std::make_shared<NArrayDataBlock<type>>(size_(sizes_))->data();
      setTo(arr);
    }
  }

  template <class T, std::size_t N>
  template <class U, typename>
  NArray<T, N>::NArray(NArray<U, N>&& arr)
    : data_()
    , sizes_(arr.sizes_)
    , steps_(arr.steps_)
  {
    if constexpr (std::is_const<T>::value)
    {
      data_ = arr.data_;
    }
    else
    {
      data_ = std::make_shared<NArrayDataBlock<type>>(size_(sizes_))->data();
      setTo(arr);
    }

    arr.clear();
  }

  template <class T, std::size_t N>
  NArray<T, N>& NArray<T, N>::operator= (const NArray<T, N>& arr)
  {
    data_ = arr.data_;
    sizes_ = arr.sizes_;
    steps_ = arr.steps_;

    return *this;
  }

  template <class T, std::size_t N>
  NArray<T, N>& NArray<T, N>::operator= (NArray<T, N>&& arr)
  {
    data_ = arr.data_;
    sizes_ = arr.sizes_;
    steps_ = arr.steps_;

    arr.clear();

    return *this;
  }

  template <class T, std::size_t N>
  template <class U, typename>
  NArray<T, N>& NArray<T, N>::operator= (const NArray<U, N>& arr)
  {
    sizes_ = arr.sizes_;
    steps_ = arr.steps_;

    if (std::is_const<T>::value)
    {
      data_ = arr.data_;
    }
    else
    {
      data_ = std::make_shared<NArrayDataBlock<type>>(size_(sizes_))->data();
      setTo(arr);
    }

    return *this;
  }

  template <class T, std::size_t N>
  template <class U, typename>
  NArray<T, N>& NArray<T, N>::operator= (NArray<U, N>&& arr)
  {
    sizes_ = arr.sizes_;
    steps_ = arr.steps_;

    if (std::is_const<T>::value || arr.unique())
    {
      data_ = arr.data_;
    }
    else
    {
      data_ = std::make_shared<NArrayDataBlock<type>>(size_(sizes_))->data();
      setTo(arr);
    }

    return *this;
  }

  template <class T, std::size_t N>
  NArray<T, N>::NArray(std::shared_ptr<type> data, Point<N> sizes, Point<N> steps)
    : data_(std::move(data))
    , sizes_(sizes)
    , steps_(steps)
  {

  }

  template <class T, std::size_t N>
  NArray<T, N>& NArray<T, N>::operator+= (const NArray<cvalue, N>& arr)
  {
    static_assert(!std::is_const<T>::value, "operator+=(arr): invalid on const type");

    if (sizes_ != arr.sizes_)
      throw std::invalid_argument("operator+=(arr): dimensions must match");
    if (empty())
      return *this;

    unaryOp2_(data_.get(), arr.data_.get(), sizes_.data(), steps_.data(), arr.steps_.data(), [](T& lhs, const T& rhs) {lhs += rhs; }, N);

    return *this;
  }

  template <class T, std::size_t N>
  NArray<T, N>& NArray<T, N>::operator+= (const T& val)
  {
    static_assert(!std::is_const<T>::value, "operator+=(val): invalid on const type");

    if (empty())
      return *this;

    singleOp2_(data_.get(), sizes_.data(), steps_.data(), [&val](T& lhs) {lhs += val; }, N);

    return *this;
  }

  template <class T, std::size_t N>
  NArray<T, N>& NArray<T, N>::operator-= (const NArray<cvalue, N>& arr)
  {
    static_assert(!std::is_const<T>::value, "operator-=(arr): invalid on const type");

    if (sizes_ != arr.sizes_)
      throw std::invalid_argument("operator-=(arr): dimensions must match");
    if (empty())
      return *this;

    unaryOp2_(data_.get(), arr.data_.get(), sizes_.data(), steps_.data(), arr.steps_.data(), [](T& lhs, const T& rhs) {lhs -= rhs; }, N);

    return *this;
  }

  template <class T, std::size_t N>
  NArray<T, N>& NArray<T, N>::operator-= (const T& val)
  {
    static_assert(!std::is_const<T>::value, "operator-=(val): invalid on const type");

    if (empty())
      return *this;

    singleOp2_(data_.get(), sizes_.data(), steps_.data(), [&val](T& lhs) {lhs -= val; }, N);

    return *this;
  }

  template <class T, std::size_t N>
  NArray<T, N>& NArray<T, N>::operator*= (const T& val)
  {
    static_assert(!std::is_const<T>::value, "operator*=(val): invalid on const type");

    if (empty())
      return *this;

    singleOp2_(data_.get(), sizes_.data(), steps_.data(), [&val](T& lhs) {lhs *= val; }, N);

    return *this;
  }

  template <class T, std::size_t N>
  NArray<T, N>& NArray<T, N>::operator/= (const T& val)
  {
    static_assert(!std::is_const<T>::value, "operator/=(val): invalid on const type");

    if (empty())
      return *this;

    singleOp2_(data_.get(), sizes_.data(), steps_.data(), [&val](T& lhs) {lhs /= val; }, N);

    return *this;
  }

  template <class T, std::size_t N>
  const Point<N>& NArray<T, N>::sizes() const
  {
    return sizes_;
  }

  template <class T, std::size_t N>
  const Point<N>& NArray<T, N>::steps() const
  {
    return steps_;
  }

  template <class T, std::size_t N>
  std::size_t NArray<T, N>::size() const
  {
    return (std::size_t)size_(sizes_);
  }

  template <class T, std::size_t N>
  bool NArray<T, N>::empty() const
  {
    return data_.use_count() == 0;
  }

  template <class T, std::size_t N>
  bool NArray<T, N>::unique() const
  {
    return data_.use_count() == 1;
  }

  template <class T, std::size_t N>
  bool NArray<T, N>::shared() const
  {
    return data_.use_count() > 0;
  }

  template <class T, std::size_t N>
  pos_t NArray<T, N>::width() const
  {
    return sizes_[0];
  }

  template <class T, std::size_t N>
  pos_t NArray<T, N>::height() const
  {
    static_assert(N >= 2, "height(): invalid when N < 2");

    return sizes_[1];
  }

  template <class T, std::size_t N>
  pos_t NArray<T, N>::depth() const
  {
    static_assert(N >= 3, "depth(): invalid when N < 3");

    return sizes_[2];
  }

  template <class T, std::size_t N>
  pos_t NArray<T, N>::length(std::size_t dim) const
  {
    if (dim >= N)
      throw std::out_of_range("length(dim): dim out of bounds");

    return sizes_[dim];
  }

  template <class T, std::size_t N>
  bool NArray<T, N>::isContinuous() const
  {
    pos_t stepSize = 0;
    for (std::size_t i = 0; i < N; ++i)
      stepSize += steps_[i] * (sizes_[i] - 1);

    return stepSize + 1 == this->size();
  }

  template <class T, std::size_t N>
  bool NArray<T, N>::isAligned() const
  {
    for (std::size_t i = 0; i < N; ++i)
      if (steps_[i] <= 0)
        return false;
    for (std::size_t i = 1; i < N; ++i)
      if (steps_[i - 1] < steps_[i])
        return false;
    return true;
  }

  template <class T, std::size_t N>
  typename NArray<T, N>::reference NArray<T, N>::at(const Point<N>& loc) const
  {
    if (empty())
      throw std::runtime_error("at(): invalid when empty");

    T* ptr = data_.get();
    for (std::size_t i = 0; i < N; ++i)
      if (loc[i] >= sizes_[i] || loc[i] < 0)
        throw std::out_of_range("at(loc): element larger then dimensions");
      else
        ptr += loc[i] * steps_[i];

    return *ptr;
  }

  template <class T, std::size_t N>
  typename NArray<T, N-1>::exposed_type NArray<T, N>::operator[] (pos_t n) const
  {
    if (n >= sizes_[0])
      throw std::out_of_range("operator[](): n out of bounds");

    return slice_(0, n);
  }

  template <class T, std::size_t N>
  typename NArray<T, N-1>::exposed_type NArray<T, N>::slice(std::size_t dim, pos_t n) const
  {
    if (dim >= N)
      throw std::out_of_range("slice(dim, n): dim out of bounds");
    if (n >= sizes_[dim] || n < 0)
      throw std::out_of_range("slice(dim, n): n out of bounds");

    return slice_(dim, n);
  }

  template <class T, std::size_t N>
  typename NArray<T, N-1>::exposed_type NArray<T, N>::sliceX(pos_t x) const
  {
    if (x >= sizes_[0] || x < 0)
      throw std::out_of_range("sliceX(x): x out of bounds");

    return slice_(0, x);
  }

  template <class T, std::size_t N>
  NArray<T, N-1> NArray<T, N>::sliceY(pos_t y) const
  {
    static_assert(N >= 2, "sliceY(y): invalid when N < 2");

    if (y >= sizes_[1] || y < 0)
      throw std::out_of_range("sliceY(y): y out of bounds");

    return slice_(1, y);
  }

  template <class T, std::size_t N>
  NArray<T, N-1> NArray<T, N>::sliceZ(pos_t z) const
  {
    static_assert(N >= 3, "sliceZ(z): invalid when N < 3");

    if (z >= sizes_[2] || z < 0)
      throw std::out_of_range("sliceZ(z): z out of bounds");

    return slice_(2, z);
  }

  template <class T, std::size_t N>
  NArray<T, N-1> NArray<T, N>::sliceW(pos_t w) const
  {
    static_assert(N >= 4, "sliceW(w): invalid when N < 4");

    if (w >= sizes_[3] || w < 0)
      throw std::out_of_range("sliceW(w): w out of bounds");

    return slice_(3, w);
  }

  template <class T, std::size_t N>
  typename NArray<T, N-1>::exposed_type NArray<T, N>::slice_(std::size_t dim, pos_t n) const
  {
    auto newdata = data_.get() + steps_[dim] * n;
    auto newsizes = wilt::slice_(sizes_, dim);
    auto newsteps = wilt::slice_(steps_, dim);

    return NArray<T, N-1>(std::shared_ptr<type>(data_, newdata), newsizes, newsteps);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::range(std::size_t dim, pos_t n, pos_t length) const
  {
    if (dim >= N)
      throw std::out_of_range("range(dim, n, length): dim out of bounds");
    if (n < 0 || n >= sizes_[dim])
      throw std::out_of_range("range(dim, n, length): n out of bounds");
    if (length <= 0 || n + length > sizes_[dim])
      throw std::out_of_range("range(dim, n, length): length out of bounds");

    return range_(dim, n, length);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::rangeX(pos_t x, pos_t length) const
  {
    if (x < 0 || x >= sizes_[0])
      throw std::out_of_range("rangeX(x, length): x out of bounds");
    if (length <= 0 || x + length > sizes_[0])
      throw std::out_of_range("rangeX(x, length): length out of bounds");

    return range_(0, x, length);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::rangeY(pos_t y, pos_t length) const
  {
    static_assert(N >= 2, "rangeY(y, length): invalid when N < 2");

    if (y < 0 || y >= sizes_[1])
      throw std::out_of_range("rangeY(y, length): y out of bounds");
    if (length <= 0 || y + length > sizes_[1])
      throw std::out_of_range("rangeY(y, length): length out of bounds");

    return range_(1, y, length);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::rangeZ(pos_t z, pos_t length) const
  {
    static_assert(N >= 3, "rangeZ(z, length): invalid when N < 3");

    if (z < 0 || z >= sizes_[2])
      throw std::out_of_range("rangeZ(z, length): z out of bounds");
    if (length <= 0 || z + length > sizes_[2])
      throw std::out_of_range("rangeZ(z, length): length out of bounds");

    return range_(2, z, length);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::rangeW(pos_t w, pos_t length) const
  {
    static_assert(N >= 4, "rangeW(w, length): invalid when N < 4");

    if (w < 0 || w >= sizes_[3])
      throw std::out_of_range("rangeW(w, length): w out of bounds");
    if (length <= 0 || w + length > sizes_[3])
      throw std::out_of_range("rangeW(w, length): length out of bounds");

    return range_(3, w, length);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::range_(std::size_t dim, pos_t n, pos_t length) const
  {
    auto newdata = data.get() + steps_[dim] * n;
    auto newsizes = sizes_;
    newsizes[dim] = length;

    return NArray<T, N>(std::shared_ptr<type>(data_, newdata), newsizes, steps_);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::flip(std::size_t dim) const
  {
    if (dim >= N)
      throw std::out_of_range("flip(dim): dim out of bounds");

    return flip_(dim);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::flipX() const
  {
    return flip_(0);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::flipY() const
  {
    static_assert(N >= 2, "flipY(): invalid when N < 2");

    return flip_(1);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::flipZ() const
  {
    static_assert(N >= 3, "flipZ(): invalid when N < 3");

    return flip_(2);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::flipW() const
  {
    static_assert(N >= 4, "flipW(): invalid when N < 4");

    return flip_(3);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::flip_(std::size_t dim) const
  {
    auto newdata = data_.get() + steps_[dim] * (sizes_[dim] - 1);
    auto newsteps = steps_;
    newsteps[dim] = -newsteps[dim];

    return NArray<T, N>(std::shared_ptr<type>(data_, newdata), sizes_, newsteps);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::skip(std::size_t dim, pos_t n, pos_t start) const
  {
    if (dim >= N)
      throw std::out_of_range("skip(dim, n, start): dim out of bounds");
    if (n < 1 || n >= sizes_[dim])
      throw std::out_of_range("skip(dim, n, start): n out of bounds");
    if (start < 0 || start >= sizes_[dim])
      throw std::out_of_range("skip(dim, n, start): start out of bounds");

    return skip_(dim, n, start);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::skipX(pos_t n, pos_t start) const
  {
    if (n < 1 || n >= sizes_[0])
      throw std::out_of_range("skipX(n, start): n out of bounds");
    if (start < 0 || start >= sizes_[0])
      throw std::out_of_range("skipX(n, start): start out of bounds");

    return skip_(0, n, start);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::skipY(pos_t n, pos_t start) const
  {
    static_assert(N >= 2, "skipY(n, start): invalid when N < 2");

    if (n < 1 || n >= sizes_[1])
      throw std::out_of_range("skipY(n, start): n out of bounds");
    if (start < 0 || start >= sizes_[1])
      throw std::out_of_range("skipY(n, start): start out of bounds");

    return skip_(1, n, start);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::skipZ(pos_t n, pos_t start) const
  {
    static_assert(N >= 3, "skipZ(n, start): invalid when N < 3");

    if (n < 1 || n >= sizes_[2])
      throw std::out_of_range("skipZ(n, start): n out of bounds");
    if (start < 0 || start >= sizes_[2])
      throw std::out_of_range("skipZ(n, start): start out of bounds");

    return skip_(2, n, start);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::skipW(pos_t n, pos_t start) const
  {
    static_assert(N >= 4, "skipW(n, start): invalid when N < 4");

    if (n < 1 || n >= sizes_[3])
      throw std::out_of_range("skipW(n, start): n out of bounds");
    if (start < 0 || start >= sizes_[3])
      throw std::out_of_range("skipW(n, start): start out of bounds");

    return skip_(3, n, start);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::skip_(std::size_t dim, pos_t n, pos_t start) const
  {
    auto newdata = data_.get() + steps_[dim] * start;
    auto newsizes = sizes_;
    auto newsteps = steps_;
    newsizes[dim] = (sizes_[dim] - start + n - 1) / n;
    newsteps[dim] = steps_[dim] * n;

    return NArray<T, N>(std::shared_ptr<type>(data_, newdata), newsizes, newsteps);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::transpose() const
  {
    static_assert(N >= 2, "transpose(): invalid when N < 2");

    return transpose(0, 1);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::transpose(std::size_t dim1, std::size_t dim2) const
  {
    if (dim1 >= N)
      throw std::out_of_range("transpose(dim1, dim2): dim1 out of bounds");
    if (dim2 >= N)
      throw std::out_of_range("transpose(dim1, dim2): dim2 out of bounds");

    auto newsizes = swap_(sizes_, dim1, dim2);
    auto newsteps = swap_(steps_, dim1, dim2);

    return NArray<T, N>(data_, newsizes, newsteps);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::subarray(const Point<N>& loc, const Point<N>& size) const
  {
    type* base = data_.get();
    for (std::size_t i = 0; i < N; ++i)
    {
      if (size[i] + loc[i] > sizes_[i] || size[i] <= 0 || loc[i] < 0 || loc[i] >= sizes_[i])
        throw std::out_of_range("subarray(loc, size): index out of bounds");
      base += steps_[i] * loc[i];
    }

    return NArray<T, N>(std::shared_ptr<type>(data_, base), size, steps_);
  }

  template <class T, std::size_t N>
  template <std::size_t M>
  typename NArray<T, N-M>::exposed_type NArray<T, N>::subarrayAt(const Point<M>& pos) const
  {
    static_assert(M>0, "subarrayAt(pos): invalid when pos dimensionality is 0");
    static_assert(M<=N, "subarrayAt(pos): invalid when pos dimensionality is <= N");

    auto newdata = data_.get();
    auto newsizes = chopHigh_<N - M>(sizes_);
    auto newsteps = chopHigh_<N - M>(steps_);
    for (std::size_t i = 0; i < M; ++i)
      if (pos[i] >= sizes_[i] || pos[i] < 0)
        throw std::out_of_range("subarrayAt(pos): pos out of range");
      else
        newdata += steps_[i] * pos[i];

    return NArray<T, N-M>(std::shared_ptr<type>(data_, newdata), newsizes, newsteps);
  }

  template<class T, std::size_t N>
  template<std::size_t M>
  SubNArrays<T, N, M> NArray<T, N>::subarrays() const
  {
    return SubNArrays<T, N, M>(*this);
  }

  template <class T, std::size_t N>
  template <std::size_t M>
  NArray<T, M> NArray<T, N>::reshape(const Point<M>& size) const
  {
    Point<N> oldsizes = sizes_;
    Point<N> oldsteps = steps_;
    Point<M> newsizes = size;
    Point<M> newsteps;
    std::size_t n = condense_(oldsizes, oldsteps);

    int j = 0;
    int i = N - n;
    for (; i < N && j < M; )
    {
      if (oldsizes[i] / newsizes[j] * newsizes[j] == oldsizes[i])
      {
        newsteps[j] = oldsizes[i] / newsizes[j] * oldsteps[i];
        oldsizes[i] /= newsizes[j];
        ++j;
      }
      else if (oldsizes[i] == 1)
      {
        ++i;
      }
      else
      {
        throw std::domain_error("reshape(size): size not compatible");
      }
    }

    for (int k = N; k < N; ++k)
      if (oldsizes[k] != 1)
        throw std::domain_error("reshape(size): size not compatible");
    for (int k = j; k < M; ++k)
      if (newsizes[k] != 1)
        throw std::domain_error("reshape(size): size not compatible");
      else
        newsteps[k] = 1;

    return NArray<T, M>(data_, newsizes, newsteps);
  }

  template<class T, std::size_t N>
  NArray<T, N+1> NArray<T, N>::repeat(pos_t n) const
  {
    if (n <= 0)
      throw std::invalid_argument("repeat(n): n must be positive");

    auto newsizes = push_(sizes_, N, n);
    auto newsteps = push_(steps_, N, 0);

    return NArray<T, N+1>(data_, newsizes, newsteps);
  }

  template<class T, std::size_t N>
  NArray<T, N+1> NArray<T, N>::window(std::size_t dim, pos_t n) const
  {
    if (dim >= N)
      throw std::out_of_range("window(n, dim): dim out of bounds");
    if (n < 1 || n > sizes_[dim])
      throw std::out_of_range("window(n, dim): n out of bounds");

    return window_(dim, n);
  }

  template<class T, std::size_t N>
  NArray<T, N+1> NArray<T, N>::windowX(pos_t n) const
  {
    if (n < 1 || n > sizes_[0])
      throw std::out_of_range("windowX(n): n out of bounds");

    return window_(0, n);
  }

  template<class T, std::size_t N>
  NArray<T, N+1> NArray<T, N>::windowY(pos_t n) const
  {
    static_assert(N >= 2, "windowY(n): invalid when N < 2");

    if (n < 1 || n > sizes_[1])
      throw std::out_of_range("windowY(n): n out of bounds");

    return window_(1, n);
  }

  template<class T, std::size_t N>
  NArray<T, N+1> NArray<T, N>::windowZ(pos_t n) const
  {
    static_assert(N >= 3, "windowZ(n): invalid when N < 3");

    if (n < 1 || n > sizes_[2])
      throw std::out_of_range("windowZ(n): n out of bounds");

    return window_(2, n);
  }

  template<class T, std::size_t N>
  NArray<T, N+1> NArray<T, N>::windowW(pos_t n) const
  {
    static_assert(N >= 4, "windowW(n): invalid when N < 4");

    if (n < 1 || n > sizes_[3])
      throw std::out_of_range("windowW(n): n out of bounds");

    return window_(3, n);
  }

  template<class T, std::size_t N>
  NArray<T, N+1> NArray<T, N>::window_(std::size_t dim, pos_t n) const
  {
    auto newsizes = push_(sizes_, N, n);
    auto newsteps = push_(steps_, N, steps_[dim]);
    newsizes[dim] -= n - 1;

    return NArray<T, N+1>(data_, newsizes, newsteps);
  }

  template <class T, std::size_t N>
  typename NArray<T, N>::iterator NArray<T, N>::begin() const
  {
    return iterator(*this);
  }

  template <class T, std::size_t N>
  typename NArray<T, N>::iterator NArray<T, N>::end() const
  {
    return iterator(*this, size());
  }

  template <class T, std::size_t N>
  template <class Operator>
  void NArray<T, N>::foreach(Operator op) const
  {
    singleOp2_(data_.get(), sizes_.data(), steps_.data(), op, N);
  }

  template <class T, std::size_t N>
  T* NArray<T, N>::base() const
  {
    return data_.get();
  }

  template<class T, std::size_t N>
  NArray<const T, N> NArray<T, N>::asConst() const
  {
    return NArray<const T, N>(*this);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::asAligned() const
  {
    if (empty())
      return NArray<T, N>();

    auto newsizes = sizes_;
    auto newsteps = steps_;
    auto offset = align_(newsizes, newsteps);
    auto newdata = data_.get() + offset;

    return NArray<T, N>(std::shared_ptr<type>(data_, newdata), newsizes, newsteps);
  }

  template <class T, std::size_t N>
  NArray<T, N> NArray<T, N>::asCondensed() const
  {
    if (empty())
      return NArray<T, N>();

    auto newsizes = sizes_;
    auto newsteps = steps_;
    std::size_t n = condense_(newsizes, newsteps);

    return NArray<T, N>(data_, newsizes, newsteps);
  }

  template <class T, std::size_t N>
  NArray<typename std::remove_const<T>::type, N> NArray<T, N>::clone() const
  {
    NArray<typename std::remove_const<T>::type, N> ret(sizes_);
    ret.setTo(*this);
    return ret;
  }

  template <class T, std::size_t N>
  template <class U>
  NArray<U, N> NArray<T, N>::convertTo() const
  {
    NArray<U, N> ret(sizes_);
    convertTo_(*this, ret, [](const T& t) {return static_cast<U>(t); });
    return ret;
  }

  template <class T, std::size_t N>
  template <class U, class Converter>
  NArray<U, N> NArray<T, N>::convertTo(Converter func) const
  {
    NArray<U, N> ret(sizes_);
    convertTo_(*this, ret, func);
    return ret;
  }

  template<class T, std::size_t N>
  template<std::size_t M, class Compressor>
  NArray<T, M> NArray<T, N>::compress(Compressor func) const
  {
    static_assert(M <= N, "compress(func): invalid when M > N");
    static_assert(M != 0, "compress(func): invalid when M is zero");

    NArray<T, M> ret(chopLow_<M>(sizes_));

    auto dstIt = ret.begin();
    for (auto&& val : subarrays<N-M>())
      (*dstIt++) = func(val);

    return ret;
  }

  template <class T, std::size_t N>
  template <class U, class Converter>
  void NArray<T, N>::convertTo_(const wilt::NArray<value, N>& lhs, wilt::NArray<U, N>& rhs, Converter func)
  {
    Point<N> sizes = lhs.sizes();
    Point<N> step1 = lhs.steps();
    Point<N> step2 = rhs.steps();
    std::size_t n = condense_(sizes, step1, step2);
    unaryOp_(rhs.base(), lhs.base(), sizes.data(), step2.data(), step1.data(), func, n);
  }

  template <class T, std::size_t N>
  void NArray<T, N>::setTo(const NArray<const T, N>& arr) const
  {
    static_assert(!std::is_const<T>::value, "setTo(arr): invalid when element type is const");

    if (sizes_ != arr.sizes())
      throw std::invalid_argument("setTo(arr): dimensions must match");

    unaryOp2_(data_.get(), arr.base(), sizes_.data(), steps_.data(), arr.steps().data(),
      [](type& r, const type& v) {r = v; }, N);
  }

  template <class T, std::size_t N>
  void NArray<T, N>::setTo(const T& val) const
  {
    static_assert(!std::is_const<T>::value, "setTo(val): invalid when element type is const");

    singleOp2_(data_.get(), sizes_.data(), steps_.data(), [&val](type& r) {r = val; }, N);
  }

  template <class T, std::size_t N>
  void NArray<T, N>::setTo(const NArray<const T, N>& arr, const NArray<uint8_t, N>& mask) const
  {
    static_assert(!std::is_const<T>::value, "setTo(arr, mask): invalid when element type is const");

    if (sizes_ != arr.sizes() || sizes_ != mask.sizes())
      throw std::invalid_argument("setTo(arr, mask): dimensions must match");

    binaryOp2_(data_.get(), arr.base(), mask.base(), sizes_.data(), steps_.data(), arr.steps().data(), mask.steps().data(),
      [](type& r, const type& v, uint8_t m) {if (m != 0) r = v; }, N);
  }

  template <class T, std::size_t N>
  void NArray<T, N>::setTo(const T & val, const NArray<uint8_t, N>& mask) const
  {
    static_assert(!std::is_const<T>::value, "setTo(val, mask): invalid when element type is const");

    unaryOp2_(data_.get(), mask.base(), sizes_.data(), steps_.data(), mask.steps().data(),
      [&val](type& r, uint8_t m) {if (m != 0) r = val; }, N);
  }

  template <class T, std::size_t N>
  void NArray<T, N>::clear()
  {
    data_.reset();
    sizes_.clear();
    steps_.clear();
  }

} // namespace wilt

#include "narrayiterator.hpp"
#include "operators.hpp"

#endif // !WILT_NARRAY_HPP
