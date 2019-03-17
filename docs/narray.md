# NArray<T, N>

Represents an N-dimensional array of `T` elements.

Template parameters:
- `class T`: the type of elements that are contained
- `std::size_t N`: the number of dimensions

Notes:
- `T` has to be an object type
- `T` has no class requirements (some functions will impose requirements independently)
- `N` must be greater than `0` (any function that would theoretically return an `NArray<T, 0>` will instead return `T&`)

# Constructors

## `NArray()` (_default constructor_)

Creates an empty array (`empty()` is true, `size()` is 0, `data()` is null)


## `NArray(const NArray<T, N>&)` (_copy constructor_)

Copies the data pointer, size, and step values from another array.

Parameters:
- `const NArray<T, N>& arr`: the array to copy from

Notes:
- does not modify `arr`
- does not copy any elements


## `NArray(NArray<T, N>&&)` (_move constructor_)

Moves the data pointer, size, and step values from another array.

Parameters:
- `NArray<T, N>&& arr`: the array to move from

Notes:
- `arr` is empty after this call
- does not move any elements


## `NArray(const NArray<U, N>&)` (_non-const copy constructor_)

Copies the data pointer, size, and step values from another array. This constructor is for converting a `T` array into a `const T` array.

Parameters:
- `const NArray<U, N>& arr`: the array to copy from

Example:

```
wilt::NArray<int, 2> arr1({ 2, 4 }, { 0, 1, 2, 3, 4, 5, 6, 7 });
wilt::NArray<const int, 2> arr2(arr1);
```

Notes:
- this constructor only exists when `T` is const and `U` is a non-const `T`.
- does not modify `arr`
- does not copy any elements


## `NArray(NArray<U, N>&&)` (_non-const move constructor_)

Moves the data pointer, size, and step values from another array. This constructor is for converting a `T` array into a `const T` array.

Parameters:
- `NArray<U, N>&& arr`: the array to move from

Notes:
- this constructor only exists when `T` is const and `U` is a non-const `T`.
- `arr` is empty after this call
- does not move any elements


## `NArray(const Point<N>&)`

Creates an array of the given size with elements default-constructed.

Parameters:
- `const Point<N>& size`: the size of the array to construct

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _DefaultConstructible_


## `NArray(const Point<N>&, const T&)`

Creates an array of the given size with elements copy-constructed.

Parameters:
- `const Point<N>& size`: the size of the array to construct
- `const T& val`: the value to construct the elements with

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _CopyConstructible_


## `NArray<T, N>::NArray(const Point<N>&, T*, NArrayDataAcquireType)`

Creates an array of the given size using data from an existing contiguous source.

Parameters:
- `const Point<N>& size`: the size of the array to construct
- `T* ptr`: a pointer to existing data
- `NArrayDataAcquireType atype`: how the existing data should be handled

The behavior is different depending on the `atype` parameter:
- `ASSUME`: the array will assume ownership over the data and will delete it when there are no references remaining.
- `COPY`: the array will allocate space and copy from the data
- `REFERENCE`: the array will reference the data but will not delete it when there are no references remaineing.
- otherwise the behavior undefined

Example:

```
int data[] = { 0, 1, 2, 3, 4, 5, 6, 7 }

wilt::NArray<int, 2> arr({ 2, 4 }, data, wilt::REFERENCE);
```

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _CopyConstructible_
- `ptr` must have enough elements for the constructed size


## `NArray(const Point<N>&, std::initializer_list<T>)`

Creates an array of the given size using data from an initializer list.

Parameters:
- `const Point<N>& size`: the size of the array to construct
- `std::initializer_list<T> list`: the list to construct elements with

Example:

```
wilt::NArray<int, 2> arr({ 2, 4 }, { 0, 1, 2, 3, 4, 5, 6, 7 });
```

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _DefaultConstructible_ and _CopyConstructible_
- if there are not enough elements for the constructed size, the rest will be default-constructed


## `NArray(const Point<N>&, Generator)`

Creates an array of the given size with elements constructed from values from a generator function.

Parameters:
- `const Point<N>& size`: the size of the array to construct
- `Generator gen`: a functor with the signature `T gen()`

Example:

```
wilt::NArray<int, 2> arr({ 100, 100 }, [](){ return rand() % 100; });
```

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _CopyConstructible_
- `gen` is called once per each element constructed


## `NArray(const Point<N>&, Iterator, Iterator)`

Creates an array of the given size with elements constructed from values from the given range.

Parameters:
- `const Point<N>& size`: the size of the array to construct
- `Iterator first`: the beginning of the range
- `Iterator last`: the end of the range

Example:

```
std::vector<int> data = { 0, 1, 2, 3, 4, 5, 6, 7 };

wilt::NArray<int, 2> arr({ 2, 4 }, data.begin(), data.end());
```

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _DefaultConstructible_ and _CopyConstructible_
- if there are not enough elements for the constructed size, the rest will be default-constructed


## `NArray(std::shared_ptr<T>, const Point<N>&)`

Creates an array of the given size using the data provided.

Parameters:
- `std::shared_ptr<T> data`: the data to reference
- `const Point<N>& size`: the size of the array to construct

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- keeps a strong reference to `data`


## `NArray(std::shared_ptr<T>, const Point<N>&, const Point<N>&)`

Creates an array of the given size and step values using the data provided.

Parameters:
- `std::shared_ptr<T> data`: the data to reference
- `const Point<N>& size`: the size of the array to construct
- `const Point<N>& steps`: the step values for the dimensions

Notes:
- does not check that `size` is valid
- keeps a strong reference to `data`


# Assignment Operators

## `NArray<T, N>& operator= (const NArray<T, N>&)` (_copy assignment operator_)

Copies the data pointer, size, and step values from another array. This array releases its reference to the data, if it had one, and destroys it, if it was the last.

Parameters:
- `const NArray<T, N>& arr`: the array to copy from

Returns this array.

Notes:
- does not copy any elements


## `NArray<T, N>& operator= (NArray<T, N>&&)` (_move assignment operator_)

Moves the data pointer, size, and step values from another array. This array releases its reference to the data, if it had one, and destroys it, if it was the last.

Parameters:
- `NArray<T, N>&& arr`: the array to move from

Returns this array.

Notes:
- `arr` is empty after this call
- does not move any elements


## `NArray<T, N>& operator= (const NArray<U, N>&)` (_non-const copy assignment operator_)

Copies the data pointer, size, and step values from another array. This operator is for assigning a `T` array into a `const T` array. This array releases its reference to the data, if it had one, and destroys it, if it was the last.

Parameters:
- `const NArray<U, N>& arr`: the array to copy from

Returns this array.

Example:

```
wilt::NArray<int, 2> arr1({ 2, 4 }, { 0, 1, 2, 3, 4, 5, 6, 7 });
wilt::NArray<const int, 2> arr2;

arr2 = arr1;
```

Notes:
- this constructor only exists when `T` is const and `U` is a non-const `T`.
- does not modify `arr`
- does not copy any elements


## `NArray<T, N>& operator= (NArray<U, N>&&)` (_non-const move assignment operator_)

Moves the data pointer, size, and step values from another array. This operator is for assigning a `T` array into a `const T` array. This array releases its reference to the data, if it had one, and destroys it, if it was the last.

Parameters:
- `NArray<U, N>&& arr`: the array to move from

Returns this array.

Notes:
- this constructor only exists when `T` is const and `U` is a non-const `T`.
- `arr` is empty after this call
- does not move any elements


## Element-wise Assigment Operators

Modifies the referenced elements with elements from another array. 

Variants:
- `NArray<T, N>& operator+= (const NArray<const T, N>&)`
- `NArray<T, N>& operator-= (const NArray<const T, N>&)`

Parameters:
- `const NArray<const T, N>& arr`: the array to assign with

Returns this array.

Example:

```
wilt::NArray<int, 2> a({ 100, 100 }, 1);
wilt::NArray<int, 2> b({ 100, 100 }, 2);

a += b; // all elements of a are 3
```

Notes:
- the sizes of this array and `arr` must match, otherwise an exception is thrown
- calls the operator (`+=`, `-=`) once per element
- not available if `T` is const


## Per-element Assigment Operators

Modifies the referenced elements with a given value.

Parameters:
- `const T& val`: the value to assign with

Variants:
- `NArray<T, N>& operator+= (const T&)`
- `NArray<T, N>& operator-= (const T&)`
- `NArray<T, N>& operator*= (const T&)`
- `NArray<T, N>& operator/= (const T&)`

Returns this array.

```
wilt::NArray<int, 2> a({ 100, 100 }, 1);

a += 2; // all elements of a are 3
```

Notes:
- calls the operator (`+=`, `-=`, `*=`, `/=`) once per element
- not available if `T` is const


# Query Functions

Functions that return the state of the array.

## `const Point<N>& sizes()`

Gets a `Point` storing the array's dimension sizes.

Returns a const-reference to the point stored in the array.

Notes:
- if the array is empty, all values will be `0`
- if the array is not empty, all values will be positive


## `std::size_t size()`

Returns the number of elements accessed in the array.

Notes:
- equal to the compoud of `sizes()`
- equal to the values yeilded from iterators
- not necessarily the number of unique elements (because of `window()` and `repeat()`)


## `std::size_t size(std::size_t)`

Gets the size of the given dimension.

Parameters:
- `std::size_t dim`: the dimension to get the size of

Variants:
- `std::size_t width()`: with `dim` equal to `0`
- `std::size_t height()`: with `dim` equal to `1`
- `std::size_t depth()`: with `dim` equal to `2`

Returns the size of that dimension

Notes:
- `dim` must be less than `N`, otherwise an exception is thrown
- equal to `sizes()[dim]`
- variants are only available if that dimension exists


## `const Point<N>& steps()`

Gets a `Point` storing the array's step values.

Returns a const-reference to the point stored in the array.

Notes:
- if the array is empty, all values will be `0`


## `std::size_t step(std::size_t)`

Gets the step value for the given dimension.

Parameters:
- `std::size_t dim`: the dimension to get the step value for

Returns the step value of that dimension

Notes:
- `dim` must be less than `N`, otherwise an exception is thrown
- equal to `steps()[dim]`


## `bool empty()`

Returns true if no data is referenced.

Notes:
- if true, `data()` is null, `sizes()` and `steps()` are all zero
- if false, `data()` is not null, `sizes()` are all positive


## `bool unique()`

Returns true if this array holds the only reference to the shared data.

Notes:
- only one of `empty()`, `unique()`, and `shared()` is true at any time


## `bool shared()`

Returns true if this array does not hold the only reference to the shared data.

Notes:
- only one of `empty()`, `unique()`, and `shared()` is true at any time


## `bool isContiguous()`

Returns true if the elements accessed by this array have no gaps in memory.

Notes:
- if `isContiguous()` and `isAligned()`, the elements can be accessed linearly starting from `data()`


## `bool isAligned()`

Returns true if the elements accessed by this array are in order in memory.

Notes:
- repeated elements are still considered aligned as long as in-memory access order never reverses
- if `isContiguous()` and `isAligned()`, the elements can be accessed linearly starting from `data()`
