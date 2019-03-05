# wilt::NArray<T, N> ![logo](/docs/images/logo.png)

This library is designed to access, transform, and modify N-dimensional data in an efficient and uniform manner. It is similar to Boost's `multi_array` and OpenCV's `Mat` classes but focuses on generalized ease-of-use without sacrificing performance.

## Access

Element access is pretty straightforward:

```C++
// create 100x100 array of zeros
wilt::NArray<int, 2> data({ 100, 100 }, 0);

// access with traditional array syntax
data[0][0] = 42;

// iterate over all elements
for (auto& elem : data)
{
  // will loop 10,000 times in order
  //   [0][0] ... [0][99]
  //   [1][0] ... [1][99]
  //   ...
  //   [99][0] ... [99][99]
}
```

You can iterate over transformed arrays and even iterate over subarrays.

## Transform

An array can be transformed a number of ways; including ranging, skipping, and flipping:

![1-D demo](/docs/images/1D-demo.png)

These works for any number of dimensions and most transformations have X, Y, Z, and W variants for manipulating the those dimensions. Some transformations can manipulate multiple dimensions or are shown better in two dimensions:

![2-D demo](/docs/images/2D-demo.png)

Transformations can be chained together to create more complex access patterns to access the data in whatever way is necessary. All transformations reference the same shared data; no data is copied.

## Modify

You can of course assign to individual elements, but array assignment (`=`) only changes what data is referenced. To assign to all elements at once, use `setTo()` with a single value or an equally-sized array.

The operators `+=`, `-=`, `*=`, and `/=`  are also available for element-wise or single-element reassignment.

## Usage

This library is header-only. So you would just use the library like so:

```C++
#include "wilt-narray/narray.hpp"
```

This library was built and tested using C++14.

## Contact

If you have any questions or if you notice a bug, you can create an issue here or e-mail me at kmdreko@gmail.com.
