# Wilt Library

## Overview
This library is designed to reference and modify N-dimensional data in an efficient manor. The primary class of the library is `NArray<T, N>` where `T` is the type and `N` is the number of dimensions which provides many methods for accessing the data. This library is similar to Boost::MultiArray and OpenCV cv::Mat but focuses on ease-of-use with simple interfaces and consistent operations without sacrificing performance. 

## Arrays
### Creating an Array
There exist many methods for creating an array: 

    NArray<int, 2> data({3, 3});
    NArray<int, 2> data({3, 3}, 1);
    NArray<int, 2> data({3, 3}, ptr, PTR::REF);
    NArray<int, 2> data({3, 3}, [](){ return rand(); });
    NArray<int, 2> data({3, 3}, {1, 2, 3, 4, 5, 6, 7, 8, 9});

1. Creates a 3x3 array with default values
2. Creates a 3x3 array with all elements set to 1
3. Creates a 3x3 array that uses data through a pointer
4. Creates a 3x3 array from a generator function
5. Creates a 3x3 array from an initializer list

The `{3, 3}` represents a `Point<N>` object that is created from an initializer list

### Accessing the Array
The following functions slice\_, range\_, and flip\_ have X, Y, Z, W, and N variants where X, Y, Z, W correspond to the dimensions 1, 2, 3, 4 while N allows a parameter specifying the dimension.

    NArray<int, 2> data({6, 6}, [](){ return rand() % 10; });

    data.at({3, 1});
    data[3][1];
    data.sliceX(1);
    data.rangeY(3, 2);
    data.subarray({0, 0}, {2, 3});

    data.flipX();
    data.t(0, 1);

1. Returns the element at the location
2. Identical to #1 (at() is preferable over [])
3. Returns an `NArray<int, 1>` corresponding to that slice
4. Returns an `NArray<int, 2>` corresponding to that range
5. Returns an `NArray<int, 2>` corresponding to that position and size
6. Returns the array with the X dimension reversed
7. Returns the array with the two dimensions swapped

### Modifying the Array
All of the above operations do not copy the data but actually access the same data and therfore can modify the data. Meaning that `data.subarray({1, 1}, {2, 2}).at({0, 1}) = 5` makes `data.at({1, 2}) == 5`. If you wish to create a copy to modify separately, just use `.clone()`

You can protect the data by specifying `const NArray<const int, 2>` or similar when passing your data into functions. 

There are many other functions that provide attributes, iterators, and other miscellaneous information related to the array that are well documented within the source. 

## Operations

### Arithmetic
Operators (+,-,*,/,%,&,|,^) and specific-return functions (add,sub,div,mul,mod,bit\_and,bit\_or,bit\_xor) are provided that apply array-to-array and array-to-single calculations. Comparisons operators (>,<,>=,<=,==,!=) work the same way except == and != for array-to-array returns a single bool. To calculate element-wise equality use `compare(arr1, arr2, CMP::EQ);`.

### Filters
Filters are functions that apply an operation using the surrounding elements. The filters available and the various border cases are below:

    Border::NONE            abc|de|fgh     border values are in the array, used for pre-padded data
    Border::REPLICATE    aaa|abcdefgh|hhh  border values are nearest edge value
    Border::REFLECT      cba|abcdefgh|hgf  border values are reflected around the border
    Border::REFLECT_101  dcb|abcdefgh|gfe  border values are reflected around the edge
    Border::WRAP         fgh|abcdefgh|abc  border values are copied from the opposite edge
    Border::IGNORE       ---|abcdefgh|---  border values are ignored, may not be valid for all functions
    Border::PADDED       iii|abcdefgh|iii  border values are a set value (created with Border::PAD())

    filterMax();     gets the maximum element in the area, can provide a mask
    filterMin();     gets the minimum element in the area, can provide a mask
    filterMean();    gets the mean of the surrounding elements
    filterMedian();  gets the median of the surrounding elements
    filterKernel();  gets the sum of kernel * area
    filter();        custom filter, pass in a function that accepts an NArray and returns a value

### Miscellaneous 
Various other functions are available such as max, min, mean, and median that act on the whole array. There are also singleOp, unaryOp, and binaryOp that apply an element-wise operation between arrays.

## Using the Library

You would normally initialize the library as such

    #include "wilt/core.hpp"
    using wilt::NArray;

This library was designed with C++11 features but you can disable some features (brace-list-initialization, inline namespaces) for environments without full support (namely VS 2012) by using special defines in core.hpp

### Cons
This library is highly templated and uses various lambda expressions for its internal operations. As a result, the library may compile slowly depending on its use and debugging may become confusing when something is used incorrectly. Also, all the functions (particularly filters) are generic solutions that are not as efficient as optimized versions.

## Final

### Future
Various optimizations can be made for literal typed arrays (particularly uchar). A few functions have already been made that utilize SIMD operations but aren't in the library quite yet. Currently the library is header-only and optimized functions will change that, but the core features will always be header only. There may be an image-processing specific portion of the library eventually but there are other libraries that do that far more effectively.

### Contact
If you have any questions, concerns, recommendations please feel free to e-mail me at kmdreko@gmail.com. If you notice a bug or something that seems unintentional, create an issue to report it.