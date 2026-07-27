#include <gtest/gtest.h>
#include <stdx/array.h>

#include "support/contiguous_container_test_suite.h"

namespace StdxTest {

// Instantiate general contiguous container test cases for array.
// Covers indexing and iteration as well as basic size tests.
using ContiguousContainerTypes = ::testing::Types<stdx::array<int, 3>>;
INSTANTIATE_TYPED_TEST_SUITE_P(ArrayContiguousContainerTests, ContiguousContainerTest, ContiguousContainerTypes);

// ===== Helper Type =====

struct test_object {
    static int constructed;
    static int destructed;
    int value;

    test_object() : value(42) { ++constructed; }

    test_object(int val) : value(val) { ++constructed; }

    ~test_object() { ++destructed; }
};

int test_object::constructed = 0;
int test_object::destructed = 0;

// ===== Default Constructor =====

TEST(ArrayTest, DefaultConstructor_ClassType_CallsConstructors) {
    test_object::constructed = 0;
    stdx::array<test_object, 5> arr;
    EXPECT_EQ(test_object::constructed, 5);
    EXPECT_EQ(5, arr.size());
}

TEST(ArrayTest, DefaultConstructor_PrimitiveType_NoCrash) {
    // We cannot reliably test "garbage values"
    // but we can ensure construction succeeds
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    stdx::array<int, 5> arr;
    EXPECT_EQ(5, arr.size());
    SUCCEED();
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

// ===== Destructor =====

TEST(ArrayTest, Destructor_DestructsAllElements) {
    test_object::constructed = 0;
    test_object::destructed = 0;
    stdx::array<test_object, 5>* arr = new stdx::array<test_object, 5>();
    EXPECT_EQ(test_object::constructed, 5);
    EXPECT_EQ(test_object::destructed, 0);

    delete arr;
    EXPECT_EQ(test_object::destructed, 5);

    stdx::array<int, 5>* arr2 = new stdx::array<int, 5>();
    delete arr2;
}

// ===== Aggregate initialization constructor =====

TEST(ArrayTest, AggregateInitialization_Zeros) {
    stdx::array<int, 1000> arr{};
    for (std::size_t i = 0; i < 1000; ++i) {
        EXPECT_EQ(0, arr[i]);
    }

    test_object::constructed = 0;
    stdx::array<test_object, 1000> arr2{};
    EXPECT_EQ(1000, test_object::constructed);
    for (std::size_t i = 0; i < 1000; ++i) {
        EXPECT_EQ(42, arr2[i].value);
    }
}

TEST(ArrayTest, AggregateInitialization_Partial) {
    stdx::array<int, 3> arr1{1};
    EXPECT_EQ(1, arr1[0]);
    EXPECT_EQ(0, arr1[1]);
    EXPECT_EQ(0, arr1[2]);

    stdx::array<int, 3> arr2{1, 2};
    EXPECT_EQ(1, arr2[0]);
    EXPECT_EQ(2, arr2[1]);
    EXPECT_EQ(0, arr2[2]);

    test_object::constructed = 0;
    stdx::array<test_object, 3> arr3{test_object(10)};
    EXPECT_EQ(3, test_object::constructed);
    EXPECT_EQ(10, arr3[0].value);
    EXPECT_EQ(42, arr3[1].value);
    EXPECT_EQ(42, arr3[2].value);

    stdx::array<test_object, 3> arr4{test_object(10), test_object(11)};
    EXPECT_EQ(6, test_object::constructed);
    EXPECT_EQ(10, arr4[0].value);
    EXPECT_EQ(11, arr4[1].value);
    EXPECT_EQ(42, arr4[2].value);
}

TEST(ArrayTest, AggregateInitialization_Full) {
    stdx::array<int, 3> arr{1, 2, 3};
    EXPECT_EQ(1, arr[0]);
    EXPECT_EQ(2, arr[1]);
    EXPECT_EQ(3, arr[2]);
}

// ==== Other Implicitly defined functions ====

TEST(ArrayTest, CopyConstructor) {
    stdx::array<int, 5> a{11};
    stdx::array<int, 5> b(a);
    EXPECT_EQ(b[0], 11);
}

TEST(ArrayTest, CopyAssignment) {
    stdx::array<int, 5> a{11};
    stdx::array<int, 5> b;
    b = a;
    EXPECT_EQ(b[0], 11);
}

TEST(ArrayTest, MoveConstructor) {
    stdx::array<int, 5> a{11};
    stdx::array<int, 5> b(std::move(a));
    EXPECT_EQ(b[0], 11);
}

TEST(ArrayTest, MoveAssignment) {
    stdx::array<int, 5> a{11};
    stdx::array<int, 5> b;
    b = std::move(a);
    EXPECT_EQ(b[0], 11);
}

// ===== Size / Empty =====

TEST(ArrayTest, Size_ReturnsCorrectValue) {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    stdx::array<int, 7> arr;
    stdx::array<int, 0> emptyArr;
    EXPECT_EQ(arr.size(), 7);
    EXPECT_EQ(emptyArr.size(), 0);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

TEST(ArrayTest, Empty_ReturnsFalseForNonZeroSize) {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    stdx::array<int, 3> arr;
    EXPECT_FALSE(arr.empty());
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

TEST(ArrayTest, Empty_ReturnsTrueForZeroSize) {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    stdx::array<int, 0> arr;
    stdx::array<test_object, 0> arr1;
    EXPECT_TRUE(arr.empty());
    EXPECT_TRUE(arr1.empty());
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

// ===== operator[] =====

TEST(ArrayTest, OperatorIndex_ReadWrite) {
    stdx::array<int, 3> arr;
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;

    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[1], 20);
    EXPECT_EQ(arr[2], 30);
}

TEST(ArrayTest, OperatorIndex_ConstAccess) {
    stdx::array<int, 2> arr;
    arr[0] = 5;
    arr[1] = 6;

    const stdx::array<int, 2>& carr = arr;

    EXPECT_EQ(carr[0], 5);
    EXPECT_EQ(carr[1], 6);
}

TEST(ArrayTest, OperatorIndex_ConstAccessOnZeroSize) {
    // array<T, 0> stores T elems[1] internally (N==0 ? 1 : N), so index 0 is valid
    const stdx::array<int, 0> arr{};
    (void)arr[0];
    SUCCEED();
}

// ===== at() =====

TEST(ArrayTest, At_ValidAccess) {
    stdx::array<int, 3> arr;
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    EXPECT_EQ(arr.at(0), 1);
    EXPECT_EQ(arr.at(1), 2);
    EXPECT_EQ(arr.at(2), 3);
}

TEST(ArrayTest, At_ConstAccess) {
    stdx::array<int, 3> arr;
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;

    const stdx::array<int, 3>& carr = arr;

    EXPECT_EQ(carr.at(0), 10);
    EXPECT_EQ(carr.at(1), 20);
    EXPECT_EQ(carr.at(2), 30);
}

TEST(ArrayTest, At_OutOfRange_Throws) {
    stdx::array<int, 3> arr;
    const stdx::array<int, 3>& carr = arr;

    EXPECT_THROW(arr.at(3), std::out_of_range);
    EXPECT_THROW(arr.at(100), std::out_of_range);
    EXPECT_THROW(carr.at(3), std::out_of_range);
    EXPECT_THROW(carr.at(100), std::out_of_range);

    try {
        arr.at(3);
    } catch (const std::out_of_range&) {
        SUCCEED();
    } catch (...) {
        FAIL();
    }

    try {
        arr.at(100);
    } catch (const std::out_of_range&) {
        SUCCEED();
    } catch (...) {
        FAIL();
    }

    try {
        carr.at(3);
    } catch (const std::out_of_range&) {
        SUCCEED();
    } catch (...) {
        FAIL();
    }

    try {
        carr.at(100);
    } catch (const std::out_of_range&) {
        SUCCEED();
    } catch (...) {
        FAIL();
    }
}

// ===== data() =====

TEST(ArrayTest, Data_ReturnsPointerToFirstElement) {
    stdx::array<int, 3> arr;

    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;

    int* ptr = arr.data();

    EXPECT_EQ(ptr[0], 10);
    EXPECT_EQ(ptr[1], 20);
    EXPECT_EQ(ptr[2], 30);
}

TEST(ArrayTest, Data_ConstVersion) {
    stdx::array<int, 2> arr;
    arr[0] = 7;
    arr[1] = 8;

    const stdx::array<int, 2>& carr = arr;

    const int* ptr = carr.data();

    EXPECT_EQ(ptr[0], 7);
    EXPECT_EQ(ptr[1], 8);
}

// ===== back() =====

TEST(ArrayTest, FrontBack) {
    stdx::array<int, 3> arr{1, 2, 3};
    const stdx::array<int, 3> constArr{1, 2, 3};

    EXPECT_EQ(arr.front(), 1);
    EXPECT_EQ(arr.back(), 3);
    EXPECT_EQ(constArr.front(), 1);
    EXPECT_EQ(constArr.back(), 3);
}

// ===== Copy Constructor =====

TEST(ArrayTest, CopyConstructor_CopiesElements) {
    stdx::array<int, 3> arr;
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    stdx::array<int, 3> copy(arr);
    EXPECT_EQ(copy[0], 1);
    EXPECT_EQ(copy[1], 2);
    EXPECT_EQ(copy[2], 3);

    stdx::array<int, 3> copy2 = arr;
    EXPECT_EQ(copy2[0], 1);
    EXPECT_EQ(copy2[1], 2);
    EXPECT_EQ(copy2[2], 3);
}

// ===== Assignment Operator =====

TEST(ArrayTest, AssignmentOperator_CopiesElements) {
    stdx::array<int, 3> arr1;
    arr1[0] = 10;
    arr1[1] = 20;
    arr1[2] = 30;

    stdx::array<int, 3> arr2;
    arr2 = arr1;
    EXPECT_EQ(arr2[0], 10);
    EXPECT_EQ(arr2[1], 20);
    EXPECT_EQ(arr2[2], 30);

    EXPECT_NE(arr1.data(), arr2.data());
}

TEST(ArrayTest, AssignmentOperator_SelfAssignment) {
    stdx::array<int, 3> arr;
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    auto& self = arr;
    arr = self;

    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

// ===== begin() / end() =====

TEST(ArrayIteratorTest, BeginPointsToFirstElement) {
    stdx::array<int, 3> arr{1, 2, 3};
    stdx::array<int, 3>::iterator it = arr.begin();
    stdx::array<int, 3>::const_iterator cit = arr.cbegin();
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(*cit, 1);
}

TEST(ArrayIteratorTest, EndPointsPastLastElement) {
    stdx::array<int, 3> arr{1, 2, 3};
    const stdx::array<int, 3>::iterator begin = arr.begin();
    stdx::array<int, 3>::iterator end = arr.end();
    stdx::array<int, 3>::const_iterator cend = arr.cend();
    EXPECT_EQ(end - begin, 3);
    EXPECT_EQ(begin - end, -3);
    EXPECT_EQ(cend - begin, 3);
    EXPECT_EQ(begin - cend, -3);

    // end is not dereferenceable. Ensure decrementing gives us the last element
    EXPECT_EQ(*--end, 3);
    EXPECT_EQ(*--cend, 3);
}

TEST(ArrayIteratorTest, IterateForwardFromBeginToEnd) {
    stdx::array<int, 3> arr{1, 2, 3};

    int expected[] = {1, 2, 3};
    int i = 0;
    int j = 0;

    for (stdx::array<int, 3>::iterator it = arr.begin(); it != arr.end(); ++it) {
        EXPECT_EQ(*it, expected[i++]);
    }
    for (stdx::array<int, 3>::const_iterator cit = arr.cbegin(); cit != arr.cend(); ++cit) {
        EXPECT_EQ(*cit, expected[j++]);
    }

    EXPECT_EQ(3, i);
    EXPECT_EQ(3, j);
}

TEST(ArrayIteratorTest, IterateForwardRangeBasedForLoop) {
    stdx::array<int, 3> arr{1, 2, 3};

    int expected[] = {1, 2, 3};
    int i = 0;

    for (auto& x : arr) {
        EXPECT_EQ(x, expected[i++]);
    }

    EXPECT_EQ(3, i);
}

TEST(ArrayIteratorTest, ConstIterationMatchesNonConst) {
    stdx::array<int, 3> arr{1, 2, 3};

    int i = 0;
    stdx::array<int, 3>::iterator it = arr.begin();
    stdx::array<int, 3>::const_iterator cit = arr.cbegin();
    for (; it != arr.end() && cit != arr.cend(); ++it, ++cit) {
        EXPECT_EQ(*it, *cit);
        ++i;
    }
    EXPECT_EQ(3, i);
}

TEST(ArrayIteratorTest, BeginEqualsEndForEmptyArray) {
    stdx::array<int, 0> arr;
    EXPECT_EQ(arr.begin(), arr.end());
    EXPECT_EQ(arr.cbegin(), arr.cend());
}

TEST(ArrayIteratorTest, ConvertIteratorToConstIterator) {
    stdx::array<int, 3> arr{1, 2, 3};

    stdx::array<int, 3>::iterator it = arr.begin();
    stdx::array<int, 3>::const_iterator cit = it;
    (void)cit;

    const stdx::array<int, 3> carr{1, 2, 3};
    stdx::array<int, 3>::const_iterator cbegin = carr.begin(); // must be const iterator
    stdx::array<int, 3>::const_iterator cend = carr.end();     // must be const iterator
    (void)cbegin;
    (void)cend;
}

TEST(ArrayIteratorTest, SingleElementForwardIteration) {
    stdx::array<int, 1> arr{99};

    stdx::array<int, 1>::iterator it = arr.begin();
    stdx::array<int, 1>::const_iterator cit = arr.cbegin();
    EXPECT_EQ(*it, 99);
    EXPECT_EQ(*cit, 99);

    ++it;
    ++cit;
    EXPECT_EQ(it, arr.end());
    EXPECT_EQ(cit, arr.cend());
}

TEST(ArrayIteratorTest, SizeZeroArray) {
    stdx::array<int, 0> arr;
    const stdx::array<int, 0> constArr{};

    EXPECT_EQ(arr.begin(), arr.end());
    EXPECT_EQ(constArr.begin(), constArr.end());
    EXPECT_EQ(arr.cbegin(), arr.cend());

    EXPECT_EQ(arr.rbegin(), arr.rend());
    EXPECT_EQ(constArr.rbegin(), constArr.rend());
    EXPECT_EQ(arr.crbegin(), arr.crend());
}

// ===== rbegin() / rend() =====

TEST(ArrayIteratorTest, RBeginPointsToLastElement) {
    stdx::array<int, 3> arr{1, 2, 3};
    stdx::array<int, 3>::reverse_iterator rit = arr.rbegin();
    stdx::array<int, 3>::const_reverse_iterator crit = arr.crbegin();
    EXPECT_EQ(*rit, 3);
    EXPECT_EQ(*crit, 3);
}

TEST(ArrayIteratorTest, REndPointsBeforeFirstElement) {
    stdx::array<int, 3> arr{1, 2, 3};

    const stdx::array<int, 3>::reverse_iterator rbegin = arr.rbegin();
    stdx::array<int, 3>::reverse_iterator rend = arr.rend();
    stdx::array<int, 3>::const_reverse_iterator crend = arr.crend();
    EXPECT_EQ(rend - rbegin, 3);
    EXPECT_EQ(rbegin - rend, -3);
    EXPECT_EQ(crend - rbegin, 3);
    EXPECT_EQ(rbegin - crend, -3);

    // end is not dereferenceable. Ensure decrementing gives us the last element
    EXPECT_EQ(*--rend, 1);
    EXPECT_EQ(*--crend, 1);
}

TEST(ArrayIteratorTest, IterateReverseFromRBeginToREnd) {
    stdx::array<int, 3> arr{1, 2, 3};

    int expected[] = {3, 2, 1};
    int i = 0;
    int j = 0;

    for (stdx::array<int, 3>::reverse_iterator rit = arr.rbegin(); rit != arr.rend(); ++rit) {
        EXPECT_EQ(*rit, expected[i++]);
    }
    for (stdx::array<int, 3>::const_reverse_iterator crit = arr.crbegin(); crit != arr.crend(); ++crit) {
        EXPECT_EQ(*crit, expected[j++]);
    }

    EXPECT_EQ(3, i);
    EXPECT_EQ(3, j);
}

TEST(ArrayIteratorTest, ConstReverseIterationMatchesNonConst) {
    stdx::array<int, 3> arr{1, 2, 3};

    int i = 0;
    stdx::array<int, 3>::reverse_iterator rit = arr.rbegin();
    stdx::array<int, 3>::const_reverse_iterator crit = arr.crbegin();
    for (; rit != arr.rend() && crit != arr.crend(); ++rit, ++crit) {
        EXPECT_EQ(*rit, *crit);
        ++i;
    }
    EXPECT_EQ(3, i);
}

TEST(ArrayIteratorTest, RBeginEqualsREndForEmptyArray) {
    stdx::array<int, 0> arr;
    EXPECT_EQ(arr.rbegin(), arr.rend());
    EXPECT_EQ(arr.crbegin(), arr.crend());
}

TEST(ArrayIteratorTest, ConvertReverseIteratorToConstReverseIterator) {
    stdx::array<int, 3> arr{1, 2, 3};

    stdx::array<int, 3>::reverse_iterator rit = arr.rbegin();
    stdx::array<int, 3>::const_reverse_iterator crit = rit;
    (void)crit;

    const stdx::array<int, 3> carr{1, 2, 3};
    stdx::array<int, 3>::const_reverse_iterator crbegin = carr.rbegin(); // must be const iterator
    stdx::array<int, 3>::const_reverse_iterator crend = carr.rend();     // must be const iterator
    (void)crbegin;
    (void)crend;
}

TEST(ArrayIteratorTest, ReverseIteratorBaseRelationship) {
    stdx::array<int, 3> arr{1, 2, 3};

    stdx::array<int, 3>::reverse_iterator rit = arr.rbegin();
    stdx::array<int, 3>::iterator base = rit.base();

    // base() should point to end()
    EXPECT_EQ(base, arr.end());
}

TEST(ArrayIteratorTest, SingleElementReverseIteration) {
    stdx::array<int, 1> arr{99};

    stdx::array<int, 1>::reverse_iterator rit = arr.rbegin();
    stdx::array<int, 1>::const_reverse_iterator crit = arr.crbegin();
    EXPECT_EQ(*rit, 99);
    EXPECT_EQ(*crit, 99);

    ++rit;
    ++crit;
    EXPECT_EQ(rit, arr.rend());
    EXPECT_EQ(crit, arr.crend());
}

// ==== fill() ====
TEST(ArrayFillTest, FillsEmptyArray) {
    stdx::array<int, 5> arr;
    arr.fill(42);

    for (std::size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(arr[i], 42);
    }
}

TEST(ArrayFillTest, FillsEmptyArrayClassType) {
    stdx::array<test_object, 5> arr;
    const test_object testObj(10);
    arr.fill(testObj);

    for (std::size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(arr[i].value, 10);
    }
}

TEST(ArrayFillTest, FillsAllElementsOverrides) {
    stdx::array<int, 5> arr{1, 2, 3, 4, 5};
    for (std::size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(arr[i], i + 1);
    }

    arr.fill(42);

    for (std::size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(arr[i], 42);
    }
}

// ==== swap() ====

TEST(ArraySwapTest, Swaps) {
    stdx::array<int, 3> a = {0, 1, 2};
    stdx::array<int, 3> b = {3, 4, 5};
    a.swap(b);

    for (std::size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(a[i], 3 + i);
        EXPECT_EQ(b[i], i);
    }
}

TEST(ArraySwapTest, SwapDoesNotAlias) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{4, 5, 6};

    int* a_ptr = &a[0];
    int* b_ptr = &b[0];

    a.swap(b);

    // After swap, pointers should still refer to their own storage
    EXPECT_EQ(&a[0], a_ptr);
    EXPECT_EQ(&b[0], b_ptr);
}

// ==== operator+ ====

// ==============================
// Basic Functionality
// ==============================

TEST(ArrayConcatTest, BasicConcatenation) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 2> b{4, 5};

    auto result = a + b;

    static_assert(std::is_same<decltype(result), stdx::array<int, 5>>::value, "Wrong return type");

    EXPECT_EQ(1, result[0]);
    EXPECT_EQ(2, result[1]);
    EXPECT_EQ(3, result[2]);
    EXPECT_EQ(4, result[3]);
    EXPECT_EQ(5, result[4]);
}

// ==============================
// Different Sizes
// ==============================

TEST(ArrayConcatTest, DifferentSizes) {
    stdx::array<int, 1> a{42};
    stdx::array<int, 4> b{1, 2, 3, 4};

    auto result = a + b;

    EXPECT_EQ(42, result[0]);
    EXPECT_EQ(1, result[1]);
    EXPECT_EQ(2, result[2]);
    EXPECT_EQ(3, result[3]);
    EXPECT_EQ(4, result[4]);
}

// ==============================
// Empty Arrays
// ==============================

TEST(ArrayConcatTest, LeftEmpty) {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    stdx::array<int, 0> a;
    stdx::array<int, 3> b{1, 2, 3};
    stdx::array<int, 3> result = a + b;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

    EXPECT_EQ(1, result[0]);
    EXPECT_EQ(2, result[1]);
    EXPECT_EQ(3, result[2]);
}

TEST(ArrayConcatTest, RightEmpty) {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 0> b;
    stdx::array<int, 3> result = a + b;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

    EXPECT_EQ(1, result[0]);
    EXPECT_EQ(2, result[1]);
    EXPECT_EQ(3, result[2]);
}

// Uncomment to confirm compile error
// TEST(ArrayConcatTest, BothEmpty)
// {
//     stdx::array<int, 0> a;
//     stdx::array<int, 0> b;
//     auto result = a + b;
// }

// ==============================
// Order Preservation
// ==============================

TEST(ArrayConcatTest, OrderPreserved) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{4, 5, 6};

    auto result = a + b;

    for (int i = 0; i < 3; ++i)
        EXPECT_EQ(a[i], result[i]);

    for (int i = 0; i < 3; ++i)
        EXPECT_EQ(b[i], result[i + 3]);
}

// ==============================
// Chaining
// ==============================

TEST(ArrayConcatTest, Chaining) {
    stdx::array<int, 1> a{1};
    stdx::array<int, 1> b{2};
    stdx::array<int, 1> c{3};

    auto result = a + b + c;

    EXPECT_EQ(1, result[0]);
    EXPECT_EQ(2, result[1]);
    EXPECT_EQ(3, result[2]);
}

// ==============================
// Non-Trivial Type
// ==============================

TEST(ArrayConcatTest, NonTrivialType) {
    stdx::array<test_object, 2> a{test_object(1), test_object(2)};
    stdx::array<test_object, 2> b{test_object(3), test_object(4)};

    test_object::constructed = 0;
    auto result = a + b;

    EXPECT_EQ(4, test_object::constructed);
    EXPECT_EQ(1, result[0].value);
    EXPECT_EQ(2, result[1].value);
    EXPECT_EQ(3, result[2].value);
    EXPECT_EQ(4, result[3].value);
}

// === operator== / operator!= ====

TEST(ArrayRelationalTest, EqualitySameValues) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{1, 2, 3};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(ArrayRelationalTest, InequalityDifferentValues) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{1, 2, 4};

    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

// ==============================
// Less Than (Lexicographical)
// ==============================

TEST(ArrayRelationalTest, LessThanBasic) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{1, 2, 4};

    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(ArrayRelationalTest, LessThanFirstElement) {
    stdx::array<int, 3> a{0, 9, 9};
    stdx::array<int, 3> b{1, 0, 0};

    EXPECT_TRUE(a < b);
}

TEST(ArrayRelationalTest, LessThanMiddleElement) {
    stdx::array<int, 3> a{1, 2, 9};
    stdx::array<int, 3> b{1, 3, 0};

    EXPECT_TRUE(a < b);
}

// ==============================
// Greater Than
// ==============================

TEST(ArrayRelationalTest, GreaterThanBasic) {
    stdx::array<int, 3> a{1, 2, 4};
    stdx::array<int, 3> b{1, 2, 3};

    EXPECT_TRUE(a > b);
    EXPECT_FALSE(b > a);
}

// ==============================
// Less Than or Equal
// ==============================

TEST(ArrayRelationalTest, LessEqualEqualCase) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{1, 2, 3};

    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b <= a);
}

TEST(ArrayRelationalTest, LessEqualLessCase) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{1, 2, 4};

    EXPECT_TRUE(a <= b);
    EXPECT_FALSE(b <= a);
}

// ==============================
// Greater Than or Equal
// ==============================

TEST(ArrayRelationalTest, GreaterEqualEqualCase) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{1, 2, 3};

    EXPECT_TRUE(a >= b);
    EXPECT_TRUE(b >= a);
}

TEST(ArrayRelationalTest, GreaterEqualGreaterCase) {
    stdx::array<int, 3> a{1, 2, 4};
    stdx::array<int, 3> b{1, 2, 3};

    EXPECT_TRUE(a >= b);
    EXPECT_FALSE(b >= a);
}

// ==============================
// All Operators Consistency
// ==============================

TEST(ArrayRelationalTest, OperatorConsistency) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{1, 2, 4};

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a <= b);
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(a >= b);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

// ==============================
// Self Comparison
// ==============================

TEST(ArrayRelationalTest, SelfComparison) {
    stdx::array<int, 3> a{1, 2, 3};

    EXPECT_TRUE(a == a);
    EXPECT_FALSE(a != a);
    EXPECT_FALSE(a < a);
    EXPECT_FALSE(a > a);
    EXPECT_TRUE(a <= a);
    EXPECT_TRUE(a >= a);
}

// ==============================
// Edge Case: All Equal Except Last
// ==============================

TEST(ArrayRelationalTest, LastElementDifference) {
    stdx::array<int, 3> a{1, 2, 3};
    stdx::array<int, 3> b{1, 2, 4};

    EXPECT_TRUE(a < b);
}

// ==============================
// Edge Case: Completely Different
// ==============================

TEST(ArrayRelationalTest, CompletelyDifferent) {
    stdx::array<int, 3> a{9, 9, 9};
    stdx::array<int, 3> b{1, 1, 1};

    EXPECT_TRUE(a > b);
}

} // namespace StdxTest
