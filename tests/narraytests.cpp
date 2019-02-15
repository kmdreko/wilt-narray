////////////////////////////////////////////////////////////////////////////////
// FILE: narraytests.cpp
// DATE: 2018-11-14
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Tests for the NArray class

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

#include <cassert>

#include "../wilt-narray/narray.hpp"
#include "tests.h"

class NoDefault
{
public:
  NoDefault() = delete;
};

class TrackDefault
{
public:
  TrackDefault() { ++count; }
  static int count;
  static void reset() { count = 0; }
};
int TrackDefault::count = 0;

class TrackCopy
{
public:
  TrackCopy() {}
  TrackCopy(const TrackCopy&) { ++count; }
  static int count;
  static void reset() { count = 0; }
};
int TrackCopy::count = 0;

void testDefaultConstructorHasSizeZero()
{
  { // T=int N=1
    wilt::NArray<int, 1> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<1>({ 0 }));
  }

  { // T=int N=2
    wilt::NArray<int, 2> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<2>({ 0, 0 }));
  }

  { // T=int N=5
    wilt::NArray<int, 5> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<5>({ 0, 0, 0, 0, 0 }));
  }

  { // T=NoDefault N=1
    wilt::NArray<NoDefault, 1> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<1>({ 0 }));
  }

  { // T=TrackDefault N=1
    TrackDefault::reset();

    wilt::NArray<TrackDefault, 1> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<1>({ 0 }));
    assert(TrackDefault::count == 0);
  }
}

void testSizedConstructorHasConstructedSize()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 3 });

    assert(a.size() == 3);
    assert(a.sizes() == wilt::Point<1>({ 3 }));
  }

  { // T=int N=2
    wilt::NArray<int, 2> a({ 3, 2 });

    assert(a.size() == 6);
    assert(a.sizes() == wilt::Point<2>({ 3, 2 }));
  }

  { // T=int N=5
    wilt::NArray<int, 5> a({ 3, 2, 5, 1, 7 });

    assert(a.size() == 210);
    assert(a.sizes() == wilt::Point<5>({ 3, 2, 5, 1, 7 }));
  }

  { // T=TrackDefault N=1
    TrackDefault::reset();

    wilt::NArray<TrackDefault, 1> a({ 3 });

    assert(a.size() == 3);
    assert(a.sizes() == wilt::Point<1>({ 3 }));
    assert(TrackDefault::count == 3);
  }
}

void testSizedWithDefaultConstructorHasConstructedSize()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 3 }, 1);

    assert(a.size() == 3);
    assert(a.sizes() == wilt::Point<1>({ 3 }));
  }

  { // T=int N=2
    wilt::NArray<int, 2> a({ 3, 2 }, 1);

    assert(a.size() == 6);
    assert(a.sizes() == wilt::Point<2>({ 3, 2 }));
  }

  { // T=int N=5
    wilt::NArray<int, 5> a({ 3, 2, 5, 1, 7 }, 1);

    assert(a.size() == 210);
    assert(a.sizes() == wilt::Point<5>({ 3, 2, 5, 1, 7 }));
  }

  { // T=TrackCopy N=1
    TrackCopy::reset();

    wilt::NArray<TrackCopy, 1> a({ 3 }, TrackCopy{});

    assert(a.size() == 3);
    assert(a.sizes() == wilt::Point<1>({ 3 }));
    assert(TrackCopy::count == 3);
  }
}

void testCopyConstructorSharesData()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 3 }, 0);
    wilt::NArray<int, 1> b(a);
    b[1] = 1;

    assert(a.size() == b.size());
    assert(a.sizes() == b.sizes());
    assert(a[1] == 1);
    assert(&a[0] == &b[0]);
  }
}

void testAsCondensedCreatesCorrectNArray()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 12 });

    wilt::NArray<int, 1> b = a.asCondensed();
    assert(b.sizes() == wilt::Point<1>({ 12 }));
    assert(b.steps() == wilt::Point<1>({ 1 }));

    wilt::NArray<int, 1> c = a.flipX().asCondensed();
    assert(c.sizes() == wilt::Point<1>({ 12 }));
    assert(c.steps() == wilt::Point<1>({ -1 }));
  }

  { // T=int N=1
    wilt::NArray<int, 3> a;

    wilt::NArray<int, 3> b = a.asCondensed();
    assert(b.sizes() == wilt::Point<3>({ 0, 0, 0 }));
    assert(b.steps() == wilt::Point<3>({ 0, 0, 0 }));
  }

  { // T=int N=3
    wilt::NArray<int, 3> a({ 2, 3, 4 });

    wilt::NArray<int, 3> b = a.asCondensed();
    assert(b.sizes() == wilt::Point<3>({ 1, 1, 24 }));
    assert(b.steps() == wilt::Point<3>({ 24, 24, 1 }));

    wilt::NArray<int, 3> e = a.flipX().flipY().flipZ().asCondensed();
    assert(e.sizes() == wilt::Point<3>({ 1, 1, 24 }));
    assert(e.steps() == wilt::Point<3>({ 24, 24, -1 }));

    wilt::NArray<int, 3> c = a.flipX().asCondensed();
    assert(c.sizes() == wilt::Point<3>({ 1, 2, 12 }));
    assert(c.steps() == wilt::Point<3>({ 24, -12, 1 }));

    wilt::NArray<int, 3> d = a.subarray({ 0, 0, 0 }, { 1, 2, 3 }).asCondensed();
    assert(d.sizes() == wilt::Point<3>({ 1, 2, 3 }));
    assert(d.steps() == wilt::Point<3>({ 12, 4, 1 }));
  }
}

void testSkipNCreatesCorrectNArray()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 5 });

    wilt::NArray<int, 1> b = a.skipX(2);
    assert(b.sizes() == wilt::Point<1>({ 3 }));
    assert(b.steps() == wilt::Point<1>({ 2 }));

    wilt::NArray<int, 1> c = a.skipX(2, 1);
    assert(c.sizes() == wilt::Point<1>({ 2 }));
    assert(c.steps() == wilt::Point<1>({ 2 }));

    wilt::NArray<int, 1> d = a.skipX(1);
    assert(d.sizes() == wilt::Point<1>({ 5 }));
    assert(d.steps() == wilt::Point<1>({ 1 }));

    wilt::NArray<int, 1> e = a.skipX(1, 2);
    assert(e.sizes() == wilt::Point<1>({ 3 }));
    assert(e.steps() == wilt::Point<1>({ 1 }));
  }

  { // T=int N=3
    wilt::NArray<int, 3> a({ 3, 3, 3 });

    wilt::NArray<int, 3> b = a.skipY(2).skipZ(2);
    assert(b.sizes() == wilt::Point<3>({ 3, 2, 2 }));
    assert(b.steps() == wilt::Point<3>({ 9, 6, 2 }));
  }
}

void testReshapeCreatesCorrectNArray()
{
  { // T=int N=2
    wilt::NArray<int, 2> a({ 14, 14 });

    wilt::NArray<int, 4> b = a.subarray({ 1, 1 }, { 12, 12 }).reshape<4>({ 4, 3, 4, 3 });
    assert(b.sizes() == wilt::Point<4>({ 4, 3, 4, 3 }));
    assert(b.steps() == wilt::Point<4>({ 42, 14, 3, 1 }));

    wilt::NArray<int, 2> c = a.reshape<2>({ 98, 2 });
    assert(c.sizes() == wilt::Point<2>({ 98, 2 }));
    assert(c.steps() == wilt::Point<2>({ 2, 1 }));

    wilt::NArray<int, 5> d = a.reshape<5>({ 1, 98, 1, 2, 1 });
    assert(d.sizes() == wilt::Point<5>({ 1, 98, 1, 2, 1 }));
    assert(d.steps() == wilt::Point<5>({ 196, 2, 2, 1, 1 }));

    wilt::NArray<int, 2> e = a.flipX().flipY().reshape<2>({ 98, 2 });
    assert(e.sizes() == wilt::Point<2>({ 98, 2 }));
    assert(e.steps() == wilt::Point<2>({ -2, -1 }));
  }
}

void testNArray()
{
  testDefaultConstructorHasSizeZero();
  testSizedConstructorHasConstructedSize();
  testSizedWithDefaultConstructorHasConstructedSize();
  testCopyConstructorSharesData();
  testAsCondensedCreatesCorrectNArray();
  testSkipNCreatesCorrectNArray();
  testReshapeCreatesCorrectNArray();
}
