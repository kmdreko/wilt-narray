# Examples ![logo](/docs/images/logo.png)

These are some examples/samples/scenarios where I've used this library.

## Sudoku Solver

During a painfully boring weekend, I'd written up a fairly simple sudoku solver. I used the transformations in this library as an aid to help me iterate over the different parts of the grid in a uniform fashion.

```C++
auto grid = wilt::NArray<SudokuCell, 2>({ 9, 9 });

// iterate over columns
for (auto col : grid.subarrays<1>()) {}

// iterate over rows
for (auto row : grid.transpose().subarrays<1>()) {}

// iterate over 3x3 grids
for (auto tri : grid.reshape<4>({ 3, 3, 3, 3 }).transpose(1, 2).subarrays<2>()) {}
```

The solver wasn't that complex, it made some simple deductions and just made a guess if it couldn't make any progress. However, it worked flawlessly, quickly, and was only a couple hundred lines to write.

I have also written a Picross solver using this library as well.

## Data Remapping

I was once troubled by a situation where API #1 was producing data in the order "AAA...BBB...CCC..." and I needed to give it to API #2 in the order "ABC...ABC...ABC...". Fortunately, I could adapt an `NArray` to the first set of data, apply the transformations, and use `clone()` to rewrite it in the correct order.

```C++
auto size = API_1.getSize();
char* src = API_1.getData();

auto src_array = wilt::NArray<char, 2>(size, src, wilt::REFERENCE);
auto dst_array = src_array.transpose().clone();

API_2.setData(dst_array.data());
```

This is such a small use-case, but it did save me the time to write the loops and remap it manually.

## Element Iteration and Math

There are many times where it's nice to be able to iterate over a member of elements in a vector, or even add them together, and this library makes that easy.

```C++
struct Element { int field_1; float field_2; }

auto elements = std::vector<Element>{...};
auto element_array = wilt::make_narray(elements);

for (auto field_2 : element_array.byMember(&Element::field_2))
{
  // iterate over all 'field_2's
}

auto calculations = element_array.byMember(&Element::field_1)
                  + element_array.byMember(&Element::field_2);
```

Even more helpful than a simple loop, these `NArray`s can of course be iterated over using standard algorithms and other iterator-friendly libraries. And this isn't a math focused library, others do it better, but it's nice to have the simple stuff.
