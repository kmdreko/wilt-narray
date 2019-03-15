# NArray<T, N>

Represents an N-dimensional array of `T` elements.

Template parameters:
- `class T`: the type of elements that are contained
- `std::size_t N`: the number of dimensions

Notes:
- `T` has to be an object type
- `T` has no class requirements (some functions will impose requirements independently)
- `N` must be greater than `0` (any function that would theoretically return an `NArray<T, 0>` will instead return `T&`)

## Constructors

### `NArray()` (_default constructor_)

Creates an empty array (`empty()` is true, `size()` is 0, `data()` is null)

### `NArray(const NArray<T, N>&)` (_copy constructor_)

Copies the data pointer, size, and step values from another array.

Parameters:
- `const NArray<T, N>& arr`: the array to copy from

Notes:
- does not modify `arr`
- does not copy any elements

### `NArray(NArray<T, N>&&)` (_move constructor_)

Moves the data pointer, size, and step values from another array.

Parameters:
- `NArray<T, N>&& arr`: the array to move from

Notes:
- `arr` is empty after this call
- does not move any elements

### `NArray(const NArray<U, N>&)` (_non-const copy constructor_)

Copies the data pointer, size, and step values from another array. This constructor is for converting a `T` array into a `const T` array.

Parameters:
- `const NArray<U, N>& arr`: the array to copy from

Notes:
- this constructor only exists when `T` is const and `U` is a non-const `T`.
- does not modify `arr`
- does not copy any elements

### `NArray(NArray<U, N>&&)` (_non-const move constructor_)

Moves the data pointer, size, and step values from another array. This constructor is for converting a `T` array into a `const T` array.

Parameters:
- `NArray<U, N>&& arr`: the array to move from

Notes:
- this constructor only exists when `T` is const and `U` is a non-const `T`.
- `arr` is empty after this call
- does not move any elements

### `NArray(const Point<N>&)`

Creates an array of the given size with elements default-constructed.

Parameters:
- `const Point<N>& size`: the size of the array to construct

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _DefaultConstructible_

### `NArray(const Point<N>&, const T&)`

Creates an array of the given size with elements copy-constructed.

Parameters:
- `const Point<N>& size`: the size of the array to construct
- `const T& val`: the value to construct the elements with

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _CopyConstructible_

### `NArray<T, N>::NArray(const Point<N>&, T*, NArrayDataAcquireType)`

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

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _CopyConstructible_
- `ptr` must have enough elements for the constructed size

### `NArray(const Point<N>&, std::initializer_list<T>)`

Creates an array of the given size using data from an initializer list.

Parameters:
- `const Point<N>& size`: the size of the array to construct
- `std::initializer_list<T> list`: the list to construct elements with

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _DefaultConstructible_ and _CopyConstructible_
- if there are not enough elements for the constructed size, the rest will be default-constructed

### `NArray(const Point<N>&, Generator)`

Creates an array of the given size with elements constructed from values from a generator function.

Parameters:
- `const Point<N>& size`: the size of the array to construct
- `Generator gen`: a functor with the signature `T gen()`

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _CopyConstructible_
- `gen` is called once per each element constructed

### `NArray(const Point<N>&, Iterator, Iterator)`

Creates an array of the given size with elements constructed from values from the given range.

Parameters:
- `const Point<N>& size`: the size of the array to construct
- `Iterator first`: the beginning of the range
- `Iterator last`: the end of the range

Notes:
- `size` must have all positive (non-zero) values, otherwise an exception will be thrown
- `T` must be _DefaultConstructible_ and _CopyConstructible_
- if there are not enough elements for the constructed size, the rest will be default-constructed

### `NArray(std::shared_ptr<T>, const Point<N>&, const Point<N>&)`

Creates an array of the given size and step values using the data provided.

Parameters:
- `std::shared_ptr<T> data`: the data to reference
- `const Point<N>& size`: the size of the array to construct
- `const Point<N>& steps`: the step values for the dimensions

Notes:
- keeps a strong reference to `data`

## Assignment Operators
