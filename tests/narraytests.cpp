////////////////////////////////////////////////////////////////////////////////
// FILE: narraytests.cpp
// DATE: 2018-11-14
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Tests for the NArray class

////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Trevor Wilson
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

#include <catch2/catch.hpp>

#include <cassert>
#include <algorithm>

#include "../wilt-narray/narray.hpp"

class NoDefault
{
public:
  NoDefault() = delete;
};

class Tracker
{
public:
  Tracker() { ++defaultConstructorCalls; }
  Tracker(const Tracker&) { ++copyConstructorCalls; }
  Tracker(Tracker&&) { ++moveConstructorCalls; }
  Tracker& operator=(const Tracker&) { ++copyAssignmentCalls; }
  Tracker& operator=(Tracker&&) { ++moveAssignmentCalls; }

  static int defaultConstructorCalls;
  static int copyConstructorCalls;
  static int moveConstructorCalls;
  static int copyAssignmentCalls;
  static int moveAssignmentCalls;

  static void reset()
  {
    defaultConstructorCalls = 0;
    copyConstructorCalls = 0;
    moveConstructorCalls = 0;
    copyAssignmentCalls = 0;
    moveAssignmentCalls = 0;
  }
};

int Tracker::defaultConstructorCalls = 0;
int Tracker::copyConstructorCalls = 0;
int Tracker::moveConstructorCalls = 0;
int Tracker::copyAssignmentCalls = 0;
int Tracker::moveAssignmentCalls = 0;

TEST_CASE("NArray<T, N>()", "[narray]")
{
  SECTION("creates empty 1-dimensional array")
  {
    // act
    wilt::NArray<int, 1> a;

    // assert
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.sizes() == wilt::Point<1>());
    REQUIRE(a.steps() == wilt::Point<1>());
  }

  SECTION("creates empty 2-dimensional array")
  {
    // act
    wilt::NArray<int, 2> a;

    // assert
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.sizes() == wilt::Point<2>());
    REQUIRE(a.steps() == wilt::Point<2>());
  }

  SECTION("creates empty 5-dimensional array")
  {
    // act
    wilt::NArray<int, 5> a;

    // assert
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.sizes() == wilt::Point<5>());
    REQUIRE(a.steps() == wilt::Point<5>());
  }

  SECTION("does not construct any elements")
  {
    // arrange
    Tracker::reset();

    // act
    wilt::NArray<Tracker, 1> a;

    // assert
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.sizes() == wilt::Point<1>());
    REQUIRE(a.steps() == wilt::Point<1>());
    REQUIRE(Tracker::defaultConstructorCalls == 0);
    REQUIRE(Tracker::copyConstructorCalls == 0);
    REQUIRE(Tracker::moveConstructorCalls == 0);
  }

  SECTION("does not require default constructor")
  {
    // act
    wilt::NArray<NoDefault, 1> a;

    // assert
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.sizes() == wilt::Point<1>());
    REQUIRE(a.steps() == wilt::Point<1>());
  }
}

TEST_CASE("NArray<T, N>(arr)", "[narray]")
{
  SECTION("creates an array of the correct size")
  {
    // arrange
    wilt::NArray<int, 2> a({ 3, 2 });

    // act
    wilt::NArray<int, 2> b(a);

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.shared());
    REQUIRE(b.size() == 6);
    REQUIRE(b.sizes() == wilt::Point<2>({ 3, 2 }));
    REQUIRE(b.steps() == wilt::Point<2>({ 2, 1 }));
  }

  SECTION("creates an array that uses the same shared data")
  {
    // arrange
    wilt::NArray<int, 2> a({ 3, 2 });

    // act
    wilt::NArray<int, 2> b(a);
    a[1][1] = 5;

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.shared());
    REQUIRE(b[1][1] == 5);
    REQUIRE(&a[0][0] == &b[0][0]);
    REQUIRE(&a[0][1] == &b[0][1]);
    REQUIRE(&a[1][0] == &b[1][0]);
    REQUIRE(&a[1][1] == &b[1][1]);
    REQUIRE(&a[2][0] == &b[2][0]);
    REQUIRE(&a[2][1] == &b[2][1]);
  }

  SECTION("creates an empty array if the original was empty")
  {
    // arrange
    wilt::NArray<int, 2> a;

    // act
    wilt::NArray<int, 2> b(a);

    // assert
    REQUIRE(b.empty());
    REQUIRE(!b.shared());
    REQUIRE(b.size() == 0);
    REQUIRE(b.sizes() == wilt::Point<2>());
    REQUIRE(b.steps() == wilt::Point<2>());
  }

  SECTION("does not copy any elements")
  {
    // arrange
    wilt::NArray<Tracker, 2> a({ 3, 2 });
    Tracker::reset();

    // act
    wilt::NArray<Tracker, 2> b(a);

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.shared());
    REQUIRE(Tracker::defaultConstructorCalls == 0);
    REQUIRE(Tracker::copyConstructorCalls == 0);
    REQUIRE(Tracker::moveConstructorCalls == 0);
  }
}

TEST_CASE("NArray<const T, N>(arr)", "[narray]")
{
  SECTION("creates an array of the correct size")
  {
    // arrange
    wilt::NArray<int, 2> a({ 3, 2 }, 1);

    // act
    wilt::NArray<const int, 2> b(a);

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.shared());
    REQUIRE(b.size() == 6);
    REQUIRE(b.sizes() == wilt::Point<2>({ 3, 2 }));
    REQUIRE(b.steps() == wilt::Point<2>({ 2, 1 }));
  }

  SECTION("creates an array that uses the same shared data")
  {
    // arrange
    wilt::NArray<int, 2> a({ 3, 2 }, 1);

    // act
    wilt::NArray<const int, 2> b(a);
    a[1][1] = 5;

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.shared());
    REQUIRE(b[1][1] == 5);
    REQUIRE(&a[0][0] == &b[0][0]);
    REQUIRE(&a[0][1] == &b[0][1]);
    REQUIRE(&a[1][0] == &b[1][0]);
    REQUIRE(&a[1][1] == &b[1][1]);
    REQUIRE(&a[2][0] == &b[2][0]);
    REQUIRE(&a[2][1] == &b[2][1]);
  }

  SECTION("creates an empty array if the original was empty")
  {
    // arrange
    wilt::NArray<int, 2> a;

    // act
    wilt::NArray<const int, 2> b(a);

    // assert
    REQUIRE(b.empty());
    REQUIRE(!b.shared());
    REQUIRE(b.size() == 0);
    REQUIRE(b.sizes() == wilt::Point<2>());
    REQUIRE(b.steps() == wilt::Point<2>());
  }

  SECTION("does not copy any elements")
  {
    // arrange
    wilt::NArray<Tracker, 2> a({ 3, 2 });
    Tracker::reset();

    // act
    wilt::NArray<const Tracker, 2> b(a);

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.shared());
    REQUIRE(Tracker::defaultConstructorCalls == 0);
    REQUIRE(Tracker::copyConstructorCalls == 0);
    REQUIRE(Tracker::moveConstructorCalls == 0);
  }
}

TEST_CASE("NArray<T, N>(size)", "[narray]")
{
  SECTION("creates a sized 1-dimensional array")
  {
    // act
    wilt::NArray<int, 1> a({ 3 });

    // assert
    REQUIRE(!a.empty());
    REQUIRE(a.size() == 3);
    REQUIRE(a.sizes() == wilt::Point<1>({ 3 }));
    REQUIRE(a.steps() == wilt::Point<1>({ 1 }));
  }

  SECTION("creates a sized 2-dimensional array")
  {
    // act
    wilt::NArray<int, 2> a({ 3, 2 });

    // assert
    REQUIRE(!a.empty());
    REQUIRE(a.size() == 6);
    REQUIRE(a.sizes() == wilt::Point<2>({ 3, 2 }));
    REQUIRE(a.steps() == wilt::Point<2>({ 2, 1 }));
  }

  SECTION("creates a sized 5-dimensional array")
  {
    // act
    wilt::NArray<int, 5> a({ 3, 2, 5, 1, 7 });

    // assert
    REQUIRE(!a.empty());
    REQUIRE(a.size() == 210);
    REQUIRE(a.sizes() == wilt::Point<5>({ 3, 2, 5, 1, 7 }));
    REQUIRE(a.steps() == wilt::Point<5>({ 70, 35, 7, 7, 1 }));
  }

  SECTION("default constructs elements equal to the constructed size")
  {
    // arrange
    Tracker::reset();

    // act
    wilt::NArray<Tracker, 5> a({ 3, 2, 5, 1, 7 });

    // assert
    REQUIRE(!a.empty());
    REQUIRE(a.size() == 210);
    REQUIRE(a.sizes() == wilt::Point<5>({ 3, 2, 5, 1, 7 }));
    REQUIRE(Tracker::defaultConstructorCalls == 210);
    REQUIRE(Tracker::copyConstructorCalls == 0);
    REQUIRE(Tracker::moveConstructorCalls == 0);
  }

  SECTION("creates empty array when given size with a 0-sized dimension")
  {
    // act
    wilt::NArray<int, 2> a({ 3, 0 });

    // assert
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.sizes() == wilt::Point<2>());
    REQUIRE(a.steps() == wilt::Point<2>());
  }

  SECTION("creates empty array when given size with a negative-sized dimension")
  {
    // act
    wilt::NArray<int, 2> a({ 3, -2 });

    // assert
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.sizes() == wilt::Point<2>());
    REQUIRE(a.steps() == wilt::Point<2>());
  }
}

TEST_CASE("NArray<T, N>(size, val)", "[narray]")
{
  SECTION("creates a sized 1-dimensional array with the copied values")
  {
    // act
    wilt::NArray<int, 1> a({ 3 }, 1);

    // assert
    REQUIRE(!a.empty());
    REQUIRE(a.size() == 3);
    REQUIRE(a.sizes() == wilt::Point<1>({ 3 }));
    REQUIRE(a.steps() == wilt::Point<1>({ 1 }));
    REQUIRE(a[0] == 1);
    REQUIRE(a[1] == 1);
    REQUIRE(a[2] == 1);
  }

  SECTION("creates a sized 2-dimensional array with the copied values")
  {
    // act
    wilt::NArray<int, 2> a({ 3, 2 }, 1);

    // assert
    REQUIRE(!a.empty());
    REQUIRE(a.size() == 6);
    REQUIRE(a.sizes() == wilt::Point<2>({ 3, 2 }));
    REQUIRE(a.steps() == wilt::Point<2>({ 2, 1 }));
    REQUIRE(a[0][0] == 1);
    REQUIRE(a[0][1] == 1);
    REQUIRE(a[1][0] == 1);
    REQUIRE(a[1][1] == 1);
    REQUIRE(a[2][0] == 1);
    REQUIRE(a[2][1] == 1);
  }

  SECTION("copy constructs elements equal to the constructed size")
  {
    // arrange
    Tracker value;
    Tracker::reset();

    // act
    wilt::NArray<Tracker, 1> a({ 3 }, value);

    // assert
    REQUIRE(!a.empty());
    REQUIRE(a.size() == 3);
    REQUIRE(a.sizes() == wilt::Point<1>({ 3 }));
    REQUIRE(Tracker::defaultConstructorCalls == 0);
    REQUIRE(Tracker::copyConstructorCalls == 3);
    REQUIRE(Tracker::moveConstructorCalls == 0);
  }

  SECTION("creates empty array when given size with a 0-sized dimension")
  {
    // act
    wilt::NArray<int, 2> a({ 3, 0 }, 1);

    // assert
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.sizes() == wilt::Point<2>());
    REQUIRE(a.steps() == wilt::Point<2>());
  }

  SECTION("creates empty array when given size with a negative-sized dimension")
  {
    // act
    wilt::NArray<int, 2> a({ 3, -2 }, 1);

    // assert
    REQUIRE(a.empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.sizes() == wilt::Point<2>());
    REQUIRE(a.steps() == wilt::Point<2>());
  }
}

TEST_CASE("NArray<T, N>(size, first, last)", "[narray]")
{
  SECTION("creates array with the correct size")
  {
    // arrange
    int data[] = { 1, 2, 3, 4 };

    // act
    wilt::NArray<int, 2> a({ 2, 2 }, std::begin(data), std::end(data));

    // assert
    REQUIRE(!a.empty());
    REQUIRE(a.size() == 4);
    REQUIRE(a.sizes() == wilt::Point<2>({ 2, 2 }));
    REQUIRE(a.steps() == wilt::Point<2>({ 2, 1 }));
  }

  SECTION("creates array with values from the iterator even when range is larger")
  {
    // arrange
    int data[] = { 1, 2, 3, 4, 5 };
    Tracker tracker_data[5];
    Tracker::reset();

    // act
    wilt::NArray<int, 2> a({ 2, 2 }, std::begin(data), std::end(data));
    wilt::NArray<Tracker, 2> b({ 2, 2 }, std::begin(tracker_data), std::end(tracker_data));

    // assert
    REQUIRE(a[0][0] == 1);
    REQUIRE(a[0][1] == 2);
    REQUIRE(a[1][0] == 3);
    REQUIRE(a[1][1] == 4);
    REQUIRE(Tracker::defaultConstructorCalls == 0);
    REQUIRE(Tracker::copyConstructorCalls == 4);
    REQUIRE(Tracker::moveConstructorCalls == 0);
  }

  SECTION("creates array with default values if range is too small")
  {
    // arrange
    int data[] = { 1, 2, 3 };
    Tracker tracker_data[3];
    Tracker::reset();

    // act
    wilt::NArray<int, 2> a({ 2, 2 }, std::begin(data), std::end(data));
    wilt::NArray<Tracker, 2> b({ 2, 2 }, std::begin(tracker_data), std::end(tracker_data));

    // assert
    REQUIRE(a[0][0] == 1);
    REQUIRE(a[0][1] == 2);
    REQUIRE(a[1][0] == 3);
    REQUIRE(a[1][1] == 0);
    REQUIRE(Tracker::defaultConstructorCalls == 1);
    REQUIRE(Tracker::copyConstructorCalls == 3);
    REQUIRE(Tracker::moveConstructorCalls == 0);
  }
}

TEST_CASE("skip(dim, n, start)", "[narray]")
{
  SECTION("creates an array of the correct size")
  {
    // arrange
    wilt::NArray<int, 1> a({ 5 });

    // act
    wilt::NArray<int, 1> b = a.skip(0, 2);
    wilt::NArray<int, 1> c = a.skip(0, 2, 1);
    wilt::NArray<int, 1> d = a.skip(0, 1);
    wilt::NArray<int, 1> e = a.skip(0, 1, 2);

    // assert
    REQUIRE(b.sizes() == wilt::Point<1>({ 3 }));
    REQUIRE(b.steps() == wilt::Point<1>({ 2 }));
    REQUIRE(c.sizes() == wilt::Point<1>({ 2 }));
    REQUIRE(c.steps() == wilt::Point<1>({ 2 }));
    REQUIRE(d.sizes() == wilt::Point<1>({ 5 }));
    REQUIRE(d.steps() == wilt::Point<1>({ 1 }));
    REQUIRE(e.sizes() == wilt::Point<1>({ 3 }));
    REQUIRE(e.steps() == wilt::Point<1>({ 1 }));
  }

  SECTION("creates an array that shares data")
  {
    // arrange
    wilt::NArray<int, 1> a({ 5 });

    // act
    wilt::NArray<int, 1> b = a.skip(0, 2);
    wilt::NArray<int, 1> c = a.skip(0, 2, 1);
    wilt::NArray<int, 1> d = a.skip(0, 1);
    wilt::NArray<int, 1> e = a.skip(0, 1, 2);

    // assert
    REQUIRE(&b[0] == &a[0]);
    REQUIRE(&b[1] == &a[2]);
    REQUIRE(&c[0] == &a[1]);
    REQUIRE(&c[1] == &a[3]);
    REQUIRE(&d[0] == &a[0]);
    REQUIRE(&d[1] == &a[1]);
    REQUIRE(&e[0] == &a[2]);
    REQUIRE(&e[1] == &a[3]);
  }

  SECTION("with dim=0 is identical to skipX()")
  {
    // arrange
    wilt::NArray<int, 5> a({ 3, 4, 5, 6, 7 });

    // assert
    REQUIRE(a.skip(0, 2) == a.skipX(2));
    REQUIRE(a.skip(0, 2, 1) == a.skipX(2, 1));
    REQUIRE(a.skip(0, 1) == a.skipX(1));
    REQUIRE(a.skip(0, 1, 2) == a.skipX(1, 2));
  }

  SECTION("with dim=1 is identical to skipY()")
  {
    // arrange
    wilt::NArray<int, 5> a({ 3, 4, 5, 6, 7 });

    // assert
    REQUIRE(a.skip(1, 2) == a.skipY(2));
    REQUIRE(a.skip(1, 2, 1) == a.skipY(2, 1));
    REQUIRE(a.skip(1, 1) == a.skipY(1));
    REQUIRE(a.skip(1, 1, 1) == a.skipY(1, 1));
  }

  SECTION("with dim=2 is identical to skipZ()")
  {
    // arrange
    wilt::NArray<int, 5> a({ 3, 4, 5, 6, 7 });

    // assert
    REQUIRE(a.skip(2, 2) == a.skipZ(2));
    REQUIRE(a.skip(2, 2, 1) == a.skipZ(2, 1));
    REQUIRE(a.skip(2, 1) == a.skipZ(1));
    REQUIRE(a.skip(2, 1, 1) == a.skipZ(1, 1));
  }

  SECTION("with dim=3 is identical to skipW()")
  {
    // arrange
    wilt::NArray<int, 5> a({ 3, 4, 5, 6, 7 });

    // assert
    REQUIRE(a.skip(3, 2) == a.skipW(2));
    REQUIRE(a.skip(3, 2, 1) == a.skipW(2, 1));
    REQUIRE(a.skip(3, 1) == a.skipW(1));
    REQUIRE(a.skip(3, 1, 1) == a.skipW(1, 1));
  }

  SECTION("throws when skip amount is less than 1")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });

    // assert
    REQUIRE_NOTHROW(a.skip(0, 1));
    REQUIRE_THROWS(a.skip(0, 0));
    REQUIRE_THROWS(a.skip(0, -1));

    REQUIRE_NOTHROW(a.skip(1, 1));
    REQUIRE_THROWS(a.skip(1, 0));
    REQUIRE_THROWS(a.skip(1, -1));
  }

  SECTION("throws when called with a starting position greater than the dimension")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });

    // assert
    REQUIRE_NOTHROW(a.skip(0, 1, 0));
    REQUIRE_NOTHROW(a.skip(0, 1, 1));
    REQUIRE_THROWS(a.skip(0, 1, 2));
    REQUIRE_THROWS(a.skip(0, 1, 100));

    REQUIRE_NOTHROW(a.skip(1, 1, 2));
    REQUIRE_THROWS(a.skip(1, 1, 3));
    REQUIRE_THROWS(a.skip(1, 1, 100));
  }

  SECTION("throws when called with a starting position less than 0")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });

    // assert
    REQUIRE_THROWS(a.skip(0, 1, -1));
    REQUIRE_THROWS(a.skip(1, 1, -2));
    REQUIRE_THROWS(a.skip(2, 1, -100));
  }

  SECTION("throws if dimension is larger than N")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });

    // assert
    REQUIRE_NOTHROW(a.skip(0, 1));
    REQUIRE_NOTHROW(a.skip(1, 1));
    REQUIRE_NOTHROW(a.skip(2, 1));
    REQUIRE_THROWS(a.skip(3, 1));
    REQUIRE_THROWS(a.skip(100, 1));
  }

  SECTION("throws when called on an empty array")
  {
    // there are no acceptable parameters that can be given to skip() when the
    // array is empty

    // arrange
    wilt::NArray<int, 3> empty;

    // assert
    REQUIRE_THROWS(empty.skip(0, 0));
    REQUIRE_THROWS(empty.skip(0, 1));
    REQUIRE_THROWS(empty.skip(1, 1));
    REQUIRE_THROWS(empty.skip(2, 1));
  }
}

TEST_CASE("subarrays()", "[narray]")
{
  SECTION("can iterate over elements")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 }, 5);

    // act
    auto subarrays = a.subarrays<0>();

    // assert
    for (auto arr : subarrays) {
      REQUIRE((std::is_same<decltype(arr), int>::value));
      REQUIRE(arr == 5);
    }
    REQUIRE(std::distance(subarrays.begin(), subarrays.end()) == 24);
  }

  SECTION("can iterate over subarrays")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 }, 5);

    // act
    auto subarrays = a.subarrays<1>();

    // assert
    for (auto arr : subarrays) {
      REQUIRE((std::is_same<decltype(arr), wilt::NArray<int, 1>>::value));
      REQUIRE(arr.size() == 4);
      REQUIRE(arr.sizes() == wilt::Point<1>({ 4 }));
    }
    REQUIRE(std::distance(subarrays.begin(), subarrays.end()) == 6);
  }

  SECTION("can iterate over an empty array")
  {
    // arrange
    wilt::NArray<int, 3> a;

    // act
    auto subarrays = a.subarrays<1>();
    for (auto arr : subarrays) {}

    // assert
    REQUIRE(std::distance(subarrays.begin(), subarrays.end()) == 0);
  }
}

TEST_CASE("reshape(size)", "[narray]")
{
  SECTION("creates an array with the correct size")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });

    // act
    wilt::NArray<int, 2> b = a.reshape<2>({ 98, 2 });

    // assert
    REQUIRE(b.sizes() == wilt::Point<2>({ 98, 2 }));
    REQUIRE(b.steps() == wilt::Point<2>({ 2, 1 }));
  }

  SECTION("can split single-dimensions into segments")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });
    wilt::NArray<int, 2> b = a.subarray({ 1, 1 }, { 12, 12 });

    // act
    wilt::NArray<int, 4> c = b.reshape<4>({ 4, 3, 4, 3 });

    // assert
    REQUIRE(c.sizes() == wilt::Point<4>({ 4, 3, 4, 3 }));
    REQUIRE(c.steps() == wilt::Point<4>({ 42, 14, 3, 1 }));
  }

  SECTION("can always add dimensions of size 1")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });

    // act
    wilt::NArray<int, 5> b = a.reshape<5>({ 1, 98, 1, 2, 1 });

    // assert
    REQUIRE(b.sizes() == wilt::Point<5>({ 1, 98, 1, 2, 1 }));
    REQUIRE(b.steps() == wilt::Point<5>({ 196, 2, 2, 1, 1 }));
  }

  SECTION("can combine dimensions if underlying access is uniform")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });
    wilt::NArray<int, 2> b = a.flipX().flipY().skipY(2);

    // act
    wilt::NArray<int, 1> c = a.reshape<1>({ 196 });
    wilt::NArray<int, 2> d = b.reshape<2>({ 49, 2 });

    // assert
    REQUIRE(c.sizes() == wilt::Point<1>({ 196 }));
    REQUIRE(c.steps() == wilt::Point<1>({ 1 }));
    REQUIRE(d.sizes() == wilt::Point<2>({ 49, 2 }));
    REQUIRE(d.steps() == wilt::Point<2>({ -4, -2 }));
  }

  SECTION("can combine repeated dimensions")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });
    wilt::NArray<int, 4> b = a.repeat(7).repeat(2);

    // act
    wilt::NArray<int, 3> c = b.reshape<3>({ 14, 14, 14 });

    // assert
    REQUIRE(c.sizes() == wilt::Point<3>({ 14, 14, 14 }));
    REQUIRE(c.steps() == wilt::Point<3>({ 14, 1, 0 }));
  }

  SECTION("can split a repeated dimension")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });
    wilt::NArray<int, 3> b = a.repeat(14);

    // act
    wilt::NArray<int, 4> c = b.reshape<4>({ 14, 14, 7, 2 });

    // assert
    REQUIRE(c.sizes() == wilt::Point<4>({ 14, 14, 7, 2 }));
    REQUIRE(c.steps() == wilt::Point<4>({ 14, 1, 0, 0 }));
  }

  SECTION("creates an array that shares data")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });

    // act
    wilt::NArray<int, 1> b = a.reshape<1>({ 196 });
    wilt::NArray<int, 2> c = a.flipX().flipY().skipY(2).reshape<2>({ 49, 2 });
    wilt::NArray<int, 4> d = a.subarray({ 1, 1 }, { 12, 12 }).reshape<4>({ 4, 3, 4, 3 });
    wilt::NArray<int, 5> e = a.reshape<5>({ 1, 98, 1, 2, 1 });

    // assert
    REQUIRE(&b[0] == &a[0][0]);
    REQUIRE(&b[1] == &a[0][1]);
    REQUIRE(&c[0][0] == &a[13][13]);
    REQUIRE(&c[1][1] == &a[13][7]);
    REQUIRE(&d[0][0][0][0] == &a[1][1]);
    REQUIRE(&d[1][1][1][1] == &a[5][5]);
    REQUIRE(&e[0][0][0][0][0] == &a[0][0]);
    REQUIRE(&e[0][1][0][1][0] == &a[0][3]);
  }

  SECTION("throws if sizes don't match")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });

    // assert
    REQUIRE_THROWS(a.reshape<2>({ 14, 13 }));
    REQUIRE_THROWS(a.reshape<2>({ 14, 15 }));
    REQUIRE_THROWS(a.reshape<2>({ 13, 14 }));
    REQUIRE_THROWS(a.reshape<2>({ 15, 14 }));
  }

  SECTION("throws if dimension sizes are not positive")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });

    // assert
    REQUIRE_THROWS(a.reshape<2>({ 98, -2 }));
    REQUIRE_THROWS(a.reshape<2>({ -7, 28 }));
    REQUIRE_THROWS(a.reshape<2>({ -14, -14 }));
  }

  SECTION("throws if attempting to combine dimensions that aren't uniform")
  {
    // arrange
    wilt::NArray<int, 2> a({ 14, 14 });
    wilt::NArray<int, 2> b = a.subarray({ 1, 1 }, { 12, 12 });

    // assert
    REQUIRE_THROWS(b.reshape<1>({ 144 }));
    REQUIRE_THROWS(b.reshape<2>({ 3, 48 }));
  }

  SECTION("throws when called on an empty array")
  {
    // arrange
    wilt::NArray<int, 2> empty;

    // assert
    REQUIRE_THROWS(empty.reshape<1>({ 0 }));
    REQUIRE_THROWS(empty.reshape<1>({ 5 }));
    REQUIRE_THROWS(empty.reshape<2>({ 92, 2 }));
    REQUIRE_THROWS(empty.reshape<3>({ 2, 0, 4 }));
  }
}

TEST_CASE("repeat(n)", "[narray]")
{
  SECTION("creates an array with the correct size")
  {
    // arrange
    wilt::NArray<int, 2> a({ 2, 3 });

    // act
    wilt::NArray<int, 3> b = a.repeat(4);

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.sizes() == wilt::Point<3>({ 2, 3, 4 }));
    REQUIRE(b.steps() == wilt::Point<3>({ 3, 1, 0 }));
  }

  SECTION("creates an array that shares data")
  {
    // arrange
    wilt::NArray<int, 2> a({ 2, 3 });

    // act
    wilt::NArray<int, 3> b = a.repeat(4);

    // assert
    REQUIRE(b.shared());
    REQUIRE(&b[0][0][0] == &a[0][0]);
    REQUIRE(&b[0][1][0] == &a[0][1]);
    REQUIRE(&b[1][0][0] == &a[1][0]);
  }

  SECTION("creates an array where each element of the repeated dimension refer to the same element")
  {
    // arrange
    wilt::NArray<int, 2> a({ 2, 3 });

    // act
    wilt::NArray<int, 3> b = a.repeat(4);

    // assert
    REQUIRE(&b[0][0][0] == &b[0][0][1]);
    REQUIRE(&b[0][0][0] == &b[0][0][2]);
    REQUIRE(&b[0][0][0] == &b[0][0][3]);
  }

  SECTION("throws when count is not positive")
  {
    // arrange
    wilt::NArray<int, 2> a({ 2, 3 });

    // assert
    REQUIRE_THROWS(a.repeat(0));
    REQUIRE_THROWS(a.repeat(-1));
    REQUIRE_THROWS(a.repeat(-100));
  }

  SECTION("throws when called on an empty array")
  {
    // if the array is empty, there's nothing to repeat

    // arrange
    wilt::NArray<int, 2> empty;

    // assert
    REQUIRE_THROWS(empty.repeat(5));
  }
}

TEST_CASE("window(dim, n)", "[narray]")
{
  SECTION("creates an array with the correct size")
  {
    // arrange
    wilt::NArray<int, 1> a({ 10 });

    // act
    wilt::NArray<int, 2> b = a.window(0, 3);

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.sizes() == wilt::Point<2>({ 8, 3 }));
    REQUIRE(b.steps() == wilt::Point<2>({ 1, 1 }));
  }

  SECTION("creates an array that shares data")
  {
    // arrange
    wilt::NArray<int, 1> a({ 10 });

    // act
    wilt::NArray<int, 2> b = a.window(0, 3);

    // assert
    REQUIRE(b.shared());
    REQUIRE(&b[0][0] == &a[0]);
    REQUIRE(&b[0][1] == &a[1]);
    REQUIRE(&b[1][0] == &a[1]);
    REQUIRE(&b[7][2] == &a[9]);
  }

  { // T=int N=2
    wilt::NArray<int, 2> a({ 10, 10 }, 5);

    wilt::NArray<int, 4> b = a.windowX(3).windowY(3);

    REQUIRE(b.size() == 576);
    REQUIRE(b.sizes() == wilt::Point<4>({ 8, 8, 3, 3 }));
    REQUIRE(b.steps() == wilt::Point<4>({ 10, 1, 10, 1 }));
    REQUIRE(&b.at({ 2, 0, 0, 0 }) == &b.at({ 0, 0, 2, 0 }));
    REQUIRE(&b.at({ 0, 2, 0, 0 }) == &b.at({ 0, 0, 0, 2 }));
    REQUIRE(&b.at({ 7, 7, 0, 0 }) == &b.at({ 5, 5, 2, 2 }));
  }

  SECTION("with dim=0 is identical to windowX()")
  {
    // arrange
    wilt::NArray<int, 5> a({ 3, 4, 5, 6, 7 });

    // assert
    REQUIRE(a.window(0, 2) == a.windowX(2));
    REQUIRE(a.window(0, 1) == a.windowX(1));
  }

  SECTION("with dim=1 is identical to windowY()")
  {
    // arrange
    wilt::NArray<int, 5> a({ 3, 4, 5, 6, 7 });

    // assert
    REQUIRE(a.window(1, 2) == a.windowY(2));
    REQUIRE(a.window(1, 1) == a.windowY(1));
  }

  SECTION("with dim=2 is identical to windowZ()")
  {
    // arrange
    wilt::NArray<int, 5> a({ 3, 4, 5, 6, 7 });

    // assert
    REQUIRE(a.window(2, 2) == a.windowZ(2));
    REQUIRE(a.window(2, 1) == a.windowZ(1));
  }

  SECTION("with dim=3 is identical to windowW()")
  {
    // arrange
    wilt::NArray<int, 5> a({ 3, 4, 5, 6, 7 });

    // assert
    REQUIRE(a.window(3, 2) == a.windowW(2));
    REQUIRE(a.window(3, 1) == a.windowW(1));
  }

  SECTION("throws if size is larger than dimension")
  {
    // arrange
    wilt::NArray<int, 1> a({ 10 });

    // assert
    REQUIRE_NOTHROW(a.window(0, 2));
    REQUIRE_NOTHROW(a.window(0, 10));
    REQUIRE_THROWS(a.window(0, 11));
    REQUIRE_THROWS(a.window(0, 100));
  }

  SECTION("throws if size is not positive")
  {
    // arrange
    wilt::NArray<int, 1> a({ 10 });

    // assert
    REQUIRE_NOTHROW(a.window(0, 1));
    REQUIRE_THROWS(a.window(0, 0));
    REQUIRE_THROWS(a.window(0, -1));
    REQUIRE_THROWS(a.window(0, -100));
  }

  SECTION("throws if dimension is larger than N")
  {
    // arrange
    wilt::NArray<int, 2> a({ 10, 10 });

    // assert
    REQUIRE_NOTHROW(a.window(0, 3));
    REQUIRE_NOTHROW(a.window(1, 3));
    REQUIRE_THROWS(a.window(2, 3));
    REQUIRE_THROWS(a.window(100, 3));
  }

  SECTION("throws when called on an empty array")
  {
    // there are no acceptable parameters that can be given to window() when
    // the array is empty

    // arrange
    wilt::NArray<int, 3> empty;

    // assert
    REQUIRE_THROWS(empty.window(0, 1));
    REQUIRE_THROWS(empty.window(0, 0));
    REQUIRE_THROWS(empty.window(1, 0));
  }
}

TEST_CASE("asCondensed()", "[narray]")
{
  SECTION("creates fully-condensed array from uniform array")
  {
    // act
    wilt::NArray<int, 3> a({ 2, 3, 4 });
    wilt::NArray<int, 3> b = a.asCondensed();

    // assert
    REQUIRE(b.sizes() == wilt::Point<3>({ 1, 1, 24 }));
    REQUIRE(b.steps() == wilt::Point<3>({ 24, 24, 1 }));
  }

  SECTION("creates fully-condensed array from reverse-uniform array")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });
    wilt::NArray<int, 3> transformed = a.flipX().flipY().flipZ();

    // act
    wilt::NArray<int, 3> b = transformed.asCondensed();

    // assert
    REQUIRE(b.sizes() == wilt::Point<3>({ 1, 1, 24 }));
    REQUIRE(b.steps() == wilt::Point<3>({ 24, 24, -1 }));
  }

  SECTION("creates fully-condensed array from skipped array")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });
    wilt::NArray<int, 3> transformed = a.skipZ(2);

    // act
    wilt::NArray<int, 3> b = transformed.asCondensed();

    // assert
    REQUIRE(b.sizes() == wilt::Point<3>({ 1, 1, 12 }));
    REQUIRE(b.steps() == wilt::Point<3>({ 24, 24, 2 }));
  }

  SECTION("creates partially-condensed array due to a flipped end dimension")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });
    wilt::NArray<int, 3> transformed = a.flipX();

    // act
    wilt::NArray<int, 3> b = transformed.asCondensed();

    // assert
    REQUIRE(b.sizes() == wilt::Point<3>({ 1, 2, 12 }));
    REQUIRE(b.steps() == wilt::Point<3>({ 24, -12, 1 }));
  }

  SECTION("creates non-condensed array due to a flipped middle dimension")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });
    wilt::NArray<int, 3> transformed = a.flipY();

    // act
    wilt::NArray<int, 3> b = transformed.asCondensed();

    // assert
    REQUIRE(b.sizes() == wilt::Point<3>({ 2, 3, 4 }));
    REQUIRE(b.steps() == wilt::Point<3>({ 12, -4, 1 }));
  }

  SECTION("creates non-condensed array due to a sub-ranges")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });
    wilt::NArray<int, 3> transformed = a.subarray({ 0, 0, 0 }, { 1, 2, 3 });

    // act
    wilt::NArray<int, 3> b = transformed.asCondensed();

    // assert
    REQUIRE(b.sizes() == wilt::Point<3>({ 1, 2, 3 }));
    REQUIRE(b.steps() == wilt::Point<3>({ 12, 4, 1 }));
  }

  SECTION("creates partially-condensed array with repeated elements at the end")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 });
    wilt::NArray<int, 5> transformed = a.repeat(5).repeat(6);

    // act
    wilt::NArray<int, 5> b = transformed.asCondensed();

    // assert
    REQUIRE(b.sizes() == wilt::Point<5>({ 1, 1, 1, 24, 30 }));
    REQUIRE(b.steps() == wilt::Point<5>({ 24, 24, 24, 1, 0 }));
  }

  SECTION("creates an empty array when called on an empty array")
  {
    // arrange
    wilt::NArray<int, 3> empty;

    // act
    wilt::NArray<int, 3> b = empty.asCondensed();

    // assert
    REQUIRE(b.empty());
    REQUIRE(b.size() == 0);
    REQUIRE(b.sizes() == wilt::Point<3>());
    REQUIRE(b.steps() == wilt::Point<3>());
  }
}

TEST_CASE("clone()", "[narray]")
{
  SECTION("creates an array of the correct size")
  {
    // arrange
    wilt::NArray<int, 2> a({ 3, 2 }, 1);

    // act
    wilt::NArray<int, 2> b = a.clone();

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.size() == 6);
    REQUIRE(b.sizes() == wilt::Point<2>({ 3, 2 }));
    REQUIRE(b.steps() == wilt::Point<2>({ 2, 1 }));
  }

  SECTION("creates an array that does not share elements")
  {
    // arrange
    wilt::NArray<int, 2> a({ 3, 2 }, 1);

    // act
    wilt::NArray<int, 2> b = a.clone();
    a[1][1] = 5;

    // assert
    REQUIRE(!b.empty());
    REQUIRE(!b.shared());
    REQUIRE(b[0][0] == 1);
    REQUIRE(b[0][1] == 1);
    REQUIRE(b[1][0] == 1);
    REQUIRE(b[1][1] == 1); // not 5
    REQUIRE(b[2][0] == 1);
    REQUIRE(b[2][1] == 1);
  }

  SECTION("calls the copy constructor for each element")
  {
    // arrange
    wilt::NArray<Tracker, 2> a({ 3, 2 });
    Tracker::reset();

    // act
    wilt::NArray<Tracker, 2> b = a.clone();

    // assert
    REQUIRE(Tracker::defaultConstructorCalls == 0);
    REQUIRE(Tracker::copyConstructorCalls == 6);
    REQUIRE(Tracker::moveConstructorCalls == 0);
  }

  SECTION("creates an empty array when called on an empty array")
  {
    // arrange
    wilt::NArray<int, 2> a;

    // act
    wilt::NArray<int, 2> b = a.clone();

    // assert
    REQUIRE(b.empty());
    REQUIRE(b.size() == 0);
  }

  SECTION("creates a non-const array from a const array")
  {
    // arrange
    wilt::NArray<const int, 2> a({ 3, 2 }, 1);

    // act
    wilt::NArray<int, 2> b = a.clone();

    // assert
    REQUIRE(!b.empty());
    REQUIRE(b.size() == 6);
    REQUIRE(b.sizes() == wilt::Point<2>({ 3, 2 }));
    REQUIRE(b.steps() == wilt::Point<2>({ 2, 1 }));
  }
}

TEST_CASE("compress(compressor)", "[narray]")
{
  SECTION("creates smaller array using a function")
  {
    // arrange
    wilt::NArray<int, 3> a({ 2, 3, 4 }, 5);

    // act
    wilt::NArray<int, 1> b = a.compress<1>([](wilt::NArray<int, 2> m) { return m[0][0] + 1; });
    wilt::NArray<int, 2> c = a.compress<2>([](wilt::NArray<int, 1> m) { return m[0] + 1; });

    // assert
    REQUIRE(b.size() == 2);
    REQUIRE(b.sizes() == wilt::Point<1>({ 2 }));
    REQUIRE(c.size() == 6);
    REQUIRE(c.sizes() == wilt::Point<2>({ 2, 3 }));
    REQUIRE(std::all_of(b.begin(), b.end(), [](int v) { return v == 6; }));
    REQUIRE(std::all_of(c.begin(), c.end(), [](int v) { return v == 6; }));
  }

  SECTION("creates an empty array when called on an empty array")
  {
    // arrange
    wilt::NArray<int, 3> a;

    // act
    wilt::NArray<int, 2> b = a.compress<2>([](wilt::NArray<int, 1> m) { return m[0] + 1; });
    wilt::NArray<int, 1> c = a.compress<1>([](wilt::NArray<int, 2> m) { return m[0][0] + 1; });

    // assert
    REQUIRE(b.empty());
    REQUIRE(c.empty());
  }
}

TEST_CASE("operator+(arr, arr)", "[narray]")
{
  SECTION("can add array to array element-wise")
  {
    // arrange
    wilt::NArray<int, 2> a({ 5, 5 }, 1);
    wilt::NArray<int, 2> b({ 5, 5 }, 2);
    wilt::NArray<double, 2> c({ 5, 5 }, 2.25);

    // act
    auto d = a + b;
    auto e = a + c;

    // assert
    REQUIRE((std::is_same<decltype(d), wilt::NArray<int, 2>>::value));
    REQUIRE(d.sizes() == wilt::Point<2>({ 5, 5 }));
    REQUIRE(std::all_of(d.begin(), d.end(), [](int v) { return v == 3; }));
    REQUIRE((std::is_same<decltype(e), wilt::NArray<double, 2>>::value));
    REQUIRE(e.sizes() == wilt::Point<2>({ 5, 5 }));
    REQUIRE(std::all_of(e.begin(), e.end(), [](double v) { return v == 3.25; }));
  }

  SECTION("creates empty array when called with empty arrays")
  {
    // arrange
    wilt::NArray<int, 2> a;
    wilt::NArray<int, 2> b;

    // act
    auto c = a + b;

    // assert
    REQUIRE(c.empty());
  }

  SECTION("throws if array dimensions don't match")
  {
    // arrange
    wilt::NArray<int, 2> a({ 5, 5 }, 1);
    wilt::NArray<int, 2> b({ 5, 4 }, 2);
    wilt::NArray<int, 2> c({ 4, 5 }, 2);
    wilt::NArray<int, 2> empty;

    // assert
    REQUIRE_THROWS(a + b);
    REQUIRE_THROWS(a + c);
    REQUIRE_THROWS(b + c);
    REQUIRE_THROWS(a + empty);
  }
}

TEST_CASE("operator+(arr, value)", "[narray]")
{
  SECTION("can add array to value")
  {
    // arrange
    wilt::NArray<int, 2> a({ 5, 5 }, 1);

    // act
    auto d = a + 2;
    auto e = a + 2.25;

    // assert
    REQUIRE((std::is_same<decltype(d), wilt::NArray<int, 2>>::value));
    REQUIRE(d.sizes() == wilt::Point<2>({ 5, 5 }));
    REQUIRE(std::all_of(d.begin(), d.end(), [](int v) { return v == 3; }));
    REQUIRE((std::is_same<decltype(e), wilt::NArray<double, 2>>::value));
    REQUIRE(e.sizes() == wilt::Point<2>({ 5, 5 }));
    REQUIRE(std::all_of(e.begin(), e.end(), [](double v) { return v == 3.25; }));
  }

  SECTION("creates empty array when called with an empty array")
  {
    // arrange
    wilt::NArray<int, 2> a;

    // act
    auto d = a + 2;
    auto e = a + 2.25;

    // assert
    REQUIRE(d.empty());
    REQUIRE(e.empty());
  }
}

TEST_CASE("operator+(value, arr)", "[narray]")
{
  SECTION("can add value to array")
  {
    // arrange
    wilt::NArray<int, 2> a({ 5, 5 }, 1);

    // act
    auto d = 2 + a;
    auto e = 2.25 + a;

    // assert
    REQUIRE((std::is_same<decltype(d), wilt::NArray<int, 2>>::value));
    REQUIRE(d.sizes() == wilt::Point<2>({ 5, 5 }));
    REQUIRE(std::all_of(d.begin(), d.end(), [](int v) { return v == 3; }));
    REQUIRE((std::is_same<decltype(e), wilt::NArray<double, 2>>::value));
    REQUIRE(e.sizes() == wilt::Point<2>({ 5, 5 }));
    REQUIRE(std::all_of(e.begin(), e.end(), [](double v) { return v == 3.25; }));
  }

  SECTION("creates empty array when called with an empty array")
  {
    // arrange
    wilt::NArray<int, 2> a;

    // act
    auto d = 2 + a;
    auto e = 2.25 + a;

    // assert
    REQUIRE(d.empty());
    REQUIRE(e.empty());
  }
}

TEST_CASE("window()+skip() can give the same result as a reshape()+transpose()", "[narray]")
{
  // arrange
  auto arr = wilt::NArray<int, 2>({ 9, 16 }, 1);

  // act
  auto a = arr.reshape<4>({ 3, 3, 4, 4 }).transpose(1, 2);
  auto b = arr.windowX(3).windowY(4).skipX(3).skipY(4);

  // assert
  REQUIRE(b.sizes() == a.sizes());
  REQUIRE(b.steps() == a.steps());
  REQUIRE(b == a);
}
