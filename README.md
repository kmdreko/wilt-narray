# wilt::NArray<T, N> ![logo](/docs/images/logo.png)

This library is designed to handle N-dimensional data in an efficient and uniform manner. It is similar to Boost's `multi_array` and OpenCV's `Mat` classes but focuses on generalized ease-of-use without sacrificing performance.

An array can be transformed a number of ways; including ranging, skipping, and flipping:

![1-D demo](/docs/images/1D-demo.png)

These works for any number of dimensions and most transformations have X, Y, Z, and W variants for manipulating the those dimensions. Some transformations can manipulate multiple dimensions or are shown better in two dimensions:

![2-D demo](/docs/images/2D-demo.png)

Transformations can be chained together to create more complex access patterns to access the data in whatever way is necessary. All transformations reference the same shared data; no data is copied.

Element access is pretty straightforward:

```C++
// create 100x100 array of zeros
wilt::NArray<int, 2> arr({ 100, 100 }, 0);

// access with traditional array syntax
arr[0][0] = 42;

// access with function (preferred)
arr.at(0, 0) -= 36;
```

You can iterate over the array, over transformed arrays, and even over subarrays.

```C++
for (auto& elem : arr)
{
  // will loop 10,000 times in order
  //   [0][0] ... [0][99]
  //   [1][0] ... [1][99]
  //   ...
  //   [99][0] ... [99][99]
}

for (auto& elem : arr.sliceY(1).skipX(4))
  ; // will loop 20 times

for (auto subarray : arr.subarrays<1>())
  ; // will loop 100 times
```

You can adapt it to your existing data whether it be a plain array, `std::array`, or `std::vector`; and it will reference it.

```C++
int data[2][3] = { ... };

// arr.at(0, 0) points to data[0][0]
auto arr = wilt::make_narray(data);
```

## Usage

This library is header-only, so it can either be added through include directories or copied in directly. You would just use the library but including the `narray.hpp` header like so:

```C++
#include "wilt-narray/narray.hpp"
```

This library was built using C++14 and tested using Catch2.

## Resources

Check out the:
- [examples](/docs/examples.md) for small samples of what the library can do
- [overview](/docs/overview.md) for implementation details
- [documentation](/docs/narray.md) for function-specific documentation
- [tests](/tests/narraytests.cpp) for further usage, guarantees, and coverage

## Contact

If you have any questions or if you notice a bug, you can create an issue here or e-mail me at kmdreko@gmail.com.
