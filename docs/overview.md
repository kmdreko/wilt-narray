# Overview ![logo](/docs/images/logo.png)

This is a more detailed overview of the inner workings of the library. It doesn't document any functions, but rather documents the class behaviors and design decisions. It is good to consider the implementation to know what is possible and what is optimal.

## Classes

The primary class of this library is `wilt::NArray<T, N>`. It is the class designed for manipulating N-dimensional data and contains the bulk of the functions used to do that. It is further detailed below.

The `wilt::Point<N>` class is basically a wrapper around `std::array<int, N>` with additional functions for manipulating it. It is used primarily for array size or positional arguments, though it is also used other places internally for non-point-like things.

The only other classes, `wilt::NArrayIterator<T, N, M>` and `wilt::Subarrays<T, N, M>`, aren't seen as much directly but are used for iteration.

## NArray Internal Structure

The `NArray` class is fairly simple. It consists of:

- A data pointer that points to the first accessed element.
- `N` dimension sizes which correspond to the bounds that are accessible.
- `N` dimension step values which hold the offsets from one value to another.

### Shared Data

The data pointer is a `std::shared_ptr<T>` that points at an element in an array of shared data. This is needed because array transformations do not copy or modify the data; they all share the same resource. The pointer always points to the first accessed element, but the first element may change between arrays even if they share. This is because many transformations, like `range()` and `flip()`, can change what the first element is.

Keeping the data shared between arrays can introduce some inefficiencies. However, there are a few reasons why this design was used:

- Separate "data" and "view" classes are annoying. In such designs, the "view" classes are the types most seen and used as variables and function parameters. But that means the "data" object, that you need somewhere, is an outlier in comparison and can cause lifetime issues. Using the same type for everything means there's no ambiguity or cognitive overhead needed when handling transformations.
- The arrays don't need to care about the data source beyond holding a pointer to it. This library was designed so that arrays an be constructed from different sources with different requirements. The `std::shared_ptr` class has a built-in abstraction between the element it points to and the shared data itself. The pointer could be referencing a plain array, a vector, or a container with a totally different type and it doesn't change how the array fundamentally accesses and manipulates the data. This abstraction works for data cleanup as well.
- The performance hit from using `std::shared_ptr` is negligable in comparison to the work done for data access and manipulation. The only concern would be from repeated use of `arr[x][y][z]` style access due to the temporary arrays created.

These reasons above make the library simpler to use and reason about, and also makes it easier to develop and maintain.

### Data Access

The array has to keep track of its dimension sizes. They are reported by the various size-related functions, they are needed for bounds-checking, and are required for proper iteration. 

The reason that a single `NArray` can express so many access patterns is due to keeping step values (also called strides). There's one for each dimension and it represents the offset between adjacent elements along that dimension. So a single element `at(x, y, z)` is retrieved like so:

```
data() + x * step(0) + y * step(1) + z * step(2);
```

With this information, there are a surprising number of options available for changing how the data is accessed. In fact, its through modifying these values that all the transformations are done:

- `range()`: increase data pointer and reduce size
- `flip()`: move data pointer to the end and negate step value
- `skip()`: divide size and multiply step value
- `transpose()`: swap size and step values between two dimensions
- etc...

The functions `slice()` and `window()` can make arrays with more or less dimensions and the more elaborate `reshape()` function can do it arbitrarily as long as dimensions align properly. Closer examination of the size and step values are also how arrangement functions like `isContiguous()` and `isAligned()` work.

To be clear, this is not a novel idea. OpenCV's `Mat` and Boost's `multi_array_view` classes both keep track of step/stride information and use it for certain operations, but they are much more limited. They don't provide the flexibility or simplicity available through this library.

## Performance Considerations

With any C++ library, performance is always a concern, especially in areas that this library is hoping to target. There aren't really any big things to avoid, most operations are cheap and those that aren't are obvious (hopefully). But there are some things to keep in mind along hot paths.

### Access Performance

While array transformations have some inefficiencies due to design, the performance of element iteration is critical since it is the most frequent operation in most workloads. There are a few ways to iterate over and access the elements:

- `arr[x][y][z]`: while the most familiar way, is the slowest way to access an element. Each `[]` call (except for the last) creates a temporary array which has costs that the compiler can't optimize away.
- `arr.at(x, y, z)`: is fast as it doesn't need to create temporaries and can get the element directly. It does do bounds-checking by default but there is the `atUnchecked()` variant that does not.
- `*(arr.data() + x * arr.step(0) + y * arr.step(1) + z * arr.step(2))`: (aka manual access) is pretty much identical to `at()` but can be slightly faster if the step calculations are stored and reused.
- `arr.foreach([](auto& element){...})`: is _the_ fastest way to iterate over all elements.
- `for (auto& element : arr){...}`: uses iterators and is fast but its performance degrades as the number of dimensions increases. Many attempts have been made to make it faster through rewrites, but the current version (which just keeps a N-dimensional point and uses `atUnchecked()`) is the best.

There are speeds reported for all these methods as part of the tests.

In addition to these methods, the access order of the array should be considered. Transformations like `flip()` or `transpose()` can cause data to be accessed in reverse-order or in a way that causes large gaps. Out-of-order memory access is not as fast as in-order memory access due to spatial and temporal caching. If you don't need to access elements in order, you can iterate over the `asAligned()` transformation, which will make the memory access as in-order as possible.

### Transformation Performance

As said above, transformations, and making new arrays in general, have a cost due to the use of `shared_ptr`. The individual cost isn't really that significant and the use of transformations is encouraged, but it can add up. Transformation chaining and `arr[x][y][z]` accesses could be made better by transfering the `shared_ptr` on temporaries, which would have negligible cost. However, since transformations use the "aliasing constructor" for making the new array, it can't transfer ownership. This is planned to be in C++20 though.

## Exception Policy

The current policy is that any invalid input will throw an exception. This covers bounds-checks, dimension-checks, empty-checks, and others. At one point, asserts were used instead, but that has problems in library useability and testability. There are some functions with checkless variants that are common on hot paths.

A macro could be added to allow customization on bad input (assert, check-and-throw, or no-checks). The _Contracts TS_ planned for C++20 could be another possibility in the future.

## Const Data

Because this library uses shared data, making an array `const` is not enough to protect the data from modification. A non-const copy can always be made that would be able to modify the data. To protect data, you must convert to a `NArray<const T, N>`. This follows the design of other shared data classes. The conversion is always available but it may be more convenient or clear to use `asConst()`.
