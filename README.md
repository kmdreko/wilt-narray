# NArray Library

## Overview

This library is designed to access and modify N-dimensional data in an efficient and uniform manner. The primary class of the library is `wilt::NArray<T, N>` which represents an `N`-dimensional array of `T`. This library is similar to Boost::MultiArray and OpenCV cv::Mat but focuses on ease-of-use with simple interfaces and consistent operations without sacrificing performance. 

## Arrays

### Creating the Array

There exist many methods for creating an array:

```C++
// Creates an empty array
NArray<int, 2> arr;

// Creates a 3x3 array with default values
NArray<int, 2> arr({ 3, 3 });

// Creates a 3x3 array with all elements set to 1
NArray<int, 2> arr({ 3, 3 }, 1);

// Creates a 3x3 array from an initializer list
NArray<int, 2> arr({ 3, 3 }, { 1, 2, 3, 4, 5, 6, 7, 8, 9 });

// Creates a 3x3 array with data from iterators
NArray<int, 2> arr({ 3, 3 }, container.begin(), container.end());

// Creates a 3x3 array with data from a pointer. 
// Choose either:
//   - referencing existing data
//   - assume ownership of the data (deletes it when unused)
//   - copy data
NArray<int, 2> arr({ 3, 3 }, raw_ptr, wilt::REFERENCE);

// Creates a 3x3 array from a generator function
NArray<int, 2> arr({ 3, 3 }, [](){ return rand(); });
```

The initial argument is a `wilt::Point<N>` that defines the array size.

### Accessing the Array

Accessing elements in the array is pretty straightforward:

```C++
NArray<int, 2> arr({ 100, 100 }, [](){ return rand() % 100; });

// Access with traditional array syntax
arr[0][12] = 42;

// Access with a single function (better since it avoids temporaries needed for multiple []s)
arr.at({ 0, 12 }) = 42;

// Iterate over all elements
for (auto& elem : arr)
{
    // Will loop 10,000 times
    // Accesses elements in last-dimension-first order [0][0] ... [0][99] then [1][0] ... [1][99] and so on
}

// Iterate over all subarrays
for (auto subarr : arr.subarrays<1>())
{
    // Will loop 100 times, one for each X-index
}
```

### Querying the Array

Get the array size:

```C++
NArray<int, 2> arr({ 100, 100 }, [](){ return rand() % 100; });

// Get total element count
std::size_t size = arr.size(); // 10000

// Get single dimension
std::size_t w = arr.size(0);  // size of dimension 0
std::size_t w = arr.width();  // size of dimension 0
std::size_t h = arr.height(); // size of dimension 1
std::size_t d = arr.depth();  // size of dimension 2

// Get set of dimension sizes
Point<2> sizes = arr.sizes(); // { 100, 100 }
```

Get meta info about the data:

```C++
NArray<int, 2> arr({ 100, 100 }, [](){ return rand() % 100; });

// Check if there's no data referenced
bool empty = arr.empty();

// Check if this array holds only reference
bool unique = arr.unique();

// Check if there are other references
bool shared = arr.shared();
```

Get meta info about the access:

```C++
NArray<int, 2> arr({ 100, 100 }, [](){ return rand() % 100; });

// Check if there are no gaps in the accessed data (not necessarily all the data)
bool contiguous = arr.isContiguous();

// Check if elements are accessed in increasing-memory order
bool aligned = arr.isAligned();
```

### Transforming the Array

None of the transformations copy data, they only access the existing data in a new way. This works by resource-sharing the source data which means that even if the initial array for a dataset is destroyed, other arrays may keep the data alive. Many of the following functions have X, Y, Z, and W variants that correspond to the dimensions 0, 1, 2, 3. 

```C++
NArray<int, 2> arr({ 100, 100 }, [](){ return rand() % 10; });

// Creates an array for the data with a fixed value along a dimension
NArray<int, 1> subarr = arr.sliceX(0); // identical to arr[0]
NArray<int, 1> subarr = arr.sliceY(0);
NArray<int, 1> subarr = arr.slice(1, 0); // slice on dimension 1 (Y), identical to arr.sliceY(0)

// Creates an array for the data with a dimension limited
NArray<int, 2> subarr = arr.rangeX(5, 95); // access the X-data from 5 to 95
NArray<int, 2> subarr = arr.rangeX(5, 95)  // calls can be chained
                           .rangeY(5, 95); // results in 90x90 array

// Creates an array for the data with each dimension limited
NArray<int, 2> subarr = arr.subarray({ 5, 5 }, { 95, 95 });

// Creates an array for the data with dimenions swapped
NArray<int, 2> subarr = arr.transpose(0, 1);
NArray<int, 2> subarr = arr.transpose(); // X and Y by default

// Creates an array with access through a dimension reversed
NArray<int, 2> subarr = arr.flipX(); // arr[0] == subarr[99]
NArray<int, 2> subarr = arr.flip(1); // flip dimension 1 (Y)

// Create an array with access skipping over elements
NArray<int, 2> subarr = arr.skipX(5); // skip every 5 values in the X-dimension
NArray<int, 2> subarr = arr.skipY(5, 3); // skip every 5 values starting at the
                                         // 3rd value in the Y-dimension
```

These ones are a bit more complicated.

```C++
NArray<int, 2> arr({ 100, 100 }, [](){ return rand() % 10; });

// Creates an array with the new size
NArray<int, 3> subarr = arr.reshape<3>({ 100, 20, 5 }); // splits the Y-dimension into Y and Z dimensions, splitting a 
                                                        // dimension can always be done as long as the product equals
                                                        // the original size
NArray<int, 3> subarr = arr.reshape<3>({ 100, 5, 5 }); // fails
NArray<int, 5> subarr = arr.reshape<3>({ 100, 1, 10, 5, 2 }); // works
NArray<int, 1> subarr = arr.reshape<1>({ 10000 }); // transforms it into a 1-dimensional array, this can be done only
                                                   // if access to the underlying data is uniform across the dimensions
NArray<int, 1> subarr = arr.rangeX(5, 95).reshape<1>({ 9000 }); // fails
NArray<int, 1> subarr = arr.rangeY(5, 95).reshape<1>({ 9000 }); // this is ok since the underlying access is uniform

// Creates an array with an additional dimension that serves as a sliding-window over another dimension
NArray<int, 2> subarr = arr.slicex(0)   // reduce dimension for this example
                           .windowY(5); // resulting size is 95x5 and accesses elements at: 0...4, 1...5, ..., 95...99

// Creates an array with an additional dimension that repeats its elements
NArray<int, 3> subarr = arr.repeat(100); // resulting size is 100x100x100 where [0][0][0] is refers to the same element 
                                         // as at [0][0][1] and [0][0][99]

// Creates an array that accesses elements in memory-increasing order (can improve performance by forsaking order)
NArray<int, 2> subarr = arr.flipX().transpose() // change access order for this example
                           .asAligned(); // in this instance, gives the original array since it was in proper order
```

### Modifying the Array

All of the above operations do not copy the data but actually access the same data and therfore can modify the data. Meaning that `data.subarray({1, 1}, {2, 2}).at({0, 1}) = 5` makes `data.at({1, 2}) == 5`. 

Since assignment (`=`) only changes what data is referenced and doesn't modify the data, it can't be used for assigning to the data. You can assign to all values at once using `setTo()` which accepts a single value or an equivaliently-sized array.

If you wish to create a copy to modify separately, you can use `.clone()`. You can protect the data by specifying `const NArray<const int, 2>` or using `.asConst()` when passing your data into functions.

The operators `+=` and `-=` are available for element-wise modification for arrays. The operators `+=`, `-=`, `*=`, and `/=` also allow a single-element argument.

## Operations

### Arithmetic

Operators (`+`, `-`, `*`, `/`, `%`, `&`, `|`, `^`) and specific-return functions (`add`, `sub`, `div`, `mul`, `mod`, `bit_and`, `bit_or`, `bit_xor`) are provided that apply array-to-array and array-to-single calculations. Comparisons operators (`>`, `<`, `>=`, `<=`, `==`, `!=`) work the same way except `==` and `!=` for array-to-array returns a single bool. To calculate element-wise equality use `compare(arr1, arr2, CMP::EQ);`.

### Miscellaneous 

There are also `singleOp`, `unaryOp`, and `binaryOp` that can apply an element-wise operation between arrays.

## Using the Library

You would normally initialize the library like so:

```C++
#include "wilt-narray/narray.hpp"

using wilt::NArray;
using wilt::Point;
```

This library was built and tested with C++14.

## Contact

If you have any questions, concerns, recommendations please feel free to e-mail me at kmdreko@gmail.com. If you notice a bug or something that seems unintentional, create an issue to report it.
