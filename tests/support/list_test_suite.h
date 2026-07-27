#ifndef STDX_TEST_LIST_TEST_SUITE_H
#define STDX_TEST_LIST_TEST_SUITE_H

#include <gtest/gtest.h>

#include <cstddef>
#include <utility>

#include "container_factory.h"

/// @file
/// Shared type-parameterized suite for "list-like" containers: types built on top of stdx::contiguous_container
/// that support push_back/pop_back/insert/erase/clear/swap/resize (currently stdx::array_list and
/// stdx::fixed_array_list).
///
/// This suite only covers behavior that mutates a container's logical size -- construction/assignment,
/// push_back/pop_back, insert, erase, clear, swap, comparison, and resize. Non-mutating accessors (at/operator[]/
/// front/back/data, begin/end/rbegin/rend iteration) on a fixed 3-element fixture are already covered by
/// tests/support/contiguous_container_test_suite.h and are deliberately not re-tested here.
///
/// Every fixture is built from TestValues<T> (see container_factory.h) instead of hardcoded literals, and a few
/// tests grow a container up to TestValues<T>::SIZE + 2 elements -- instantiate this suite with enough fixed
/// capacity to cover that (fixed_array_list<T, 8> is what the example below and the existing .cpp instantiations
/// use).
///
/// To reuse against a new type, include this header in a test_*.cpp and instantiate it:
///
///     using MyListTypes = ::testing::Types<stdx::array_list<int>, stdx::fixed_array_list<int, 8>>;
///     INSTANTIATE_TYPED_TEST_SUITE_P(MyListTypes, ListTest, MyListTypes);
///
/// Behavior that diverges between list types (dynamic capacity growth vs. fixed-capacity std::length_error,
/// reserve()/shrink_to_fit(), allocator propagation, etc.) does not belong here -- keep that in each type's own
/// test_<header>.cpp.

namespace StdxTest {

template <typename Container>
class ListTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(ListTest);

// ==================================
// === Construction / Assignment ===
// ==================================

TYPED_TEST_P(ListTest, DefaultConstructedIsEmpty) {
    TypeParam list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0U);
}

TYPED_TEST_P(ListTest, BeginEqualsEndWhenEmpty) {
    TypeParam list;
    EXPECT_EQ(list.begin(), list.end());
    EXPECT_EQ(list.cbegin(), list.cend());
    EXPECT_EQ(list.rbegin(), list.rend());
    EXPECT_EQ(list.crbegin(), list.crend());

    const TypeParam& constList = list;
    EXPECT_EQ(constList.begin(), constList.end());
    EXPECT_EQ(constList.rbegin(), constList.rend());
}

TYPED_TEST_P(ListTest, CopyConstructorFromEmpty) {
    TypeParam source;
    TypeParam copy(source);
    EXPECT_TRUE(copy.empty());
}

TYPED_TEST_P(ListTest, CopyConstructorFromNonEmpty) {
    TypeParam source = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam copy(source);
    EXPECT_TRUE(copy == source);
}

TYPED_TEST_P(ListTest, CopyConstructorIsDeepCopy) {
    using T = typename TypeParam::value_type;
    TypeParam source = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam copy(source);
    copy[0] = TestValues<T>::UPDATED;
    EXPECT_NE(copy[0], source[0]);
}

TYPED_TEST_P(ListTest, MoveConstructorFromEmpty) {
    TypeParam source;
    TypeParam moved(std::move(source));
    EXPECT_TRUE(moved.empty());
}

TYPED_TEST_P(ListTest, MoveConstructorFromNonEmpty) {
    using T = typename TypeParam::value_type;
    TypeParam source = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam moved(std::move(source));
    ASSERT_EQ(moved.size(), TestValues<T>::SIZE);
    EXPECT_EQ(moved[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(moved[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(moved[2], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, MoveConstructorLeavesSourceEmpty) {
    TypeParam source = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam moved(std::move(source));
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(source.size(), 0U);
}

TYPED_TEST_P(ListTest, CopyAssignmentFromEmpty) {
    TypeParam source;
    TypeParam dest = ContainerFactory<TypeParam>::MakeFromTestValues();
    dest = source;
    EXPECT_TRUE(dest.empty());
}

TYPED_TEST_P(ListTest, CopyAssignmentFromNonEmpty) {
    TypeParam source = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam dest;
    dest = source;
    EXPECT_TRUE(dest == source);
}

TYPED_TEST_P(ListTest, CopyAssignmentOverwritesExisting) {
    using T = typename TypeParam::value_type;
    TypeParam source = ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[2], TestValues<T>::VALUES[1]});
    TypeParam dest = ContainerFactory<TypeParam>::MakeFromTestValues();
    dest = source;
    ASSERT_EQ(dest.size(), 2U);
    EXPECT_EQ(dest[0], TestValues<T>::VALUES[2]);
    EXPECT_EQ(dest[1], TestValues<T>::VALUES[1]);
}

TYPED_TEST_P(ListTest, CopyAssignmentIsDeepCopy) {
    using T = typename TypeParam::value_type;
    TypeParam source = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam dest;
    dest = source;
    dest[0] = TestValues<T>::UPDATED;
    EXPECT_NE(dest[0], source[0]);
}

TYPED_TEST_P(ListTest, MoveAssignmentFromEmpty) {
    TypeParam source;
    TypeParam dest = ContainerFactory<TypeParam>::MakeFromTestValues();
    dest = std::move(source);
    EXPECT_TRUE(dest.empty());
}

TYPED_TEST_P(ListTest, MoveAssignmentFromNonEmpty) {
    using T = typename TypeParam::value_type;
    TypeParam source = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam dest;
    dest = std::move(source);
    ASSERT_EQ(dest.size(), TestValues<T>::SIZE);
    EXPECT_EQ(dest[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(dest[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(dest[2], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, MoveAssignmentLeavesSourceEmpty) {
    TypeParam source = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam dest;
    dest = std::move(source);
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(source.size(), 0U);
}

TYPED_TEST_P(ListTest, MoveAssignmentOverwritesExisting) {
    using T = typename TypeParam::value_type;
    TypeParam source = ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[0], TestValues<T>::VALUES[1]});
    TypeParam dest = ContainerFactory<TypeParam>::MakeFromTestValues();
    dest = std::move(source);
    ASSERT_EQ(dest.size(), 2U);
    EXPECT_EQ(dest[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(dest[1], TestValues<T>::VALUES[1]);
}

// ==========================
// ====== Push / Pop ========
// ==========================

TYPED_TEST_P(ListTest, PushBackCopyAppendsValue) {
    using T = typename TypeParam::value_type;
    TypeParam list;
    T value = TestValues<T>::VALUES[1];
    list.push_back(value);
    EXPECT_EQ(list.size(), 1U);
    EXPECT_EQ(list.back(), TestValues<T>::VALUES[1]);
    EXPECT_NE(&list.back(), &value); // a copy was created
}

TYPED_TEST_P(ListTest, PushBackMoveAppendsValue) {
    using T = typename TypeParam::value_type;
    TypeParam list;
    T value = TestValues<T>::VALUES[1];
    list.push_back(std::move(value));
    EXPECT_EQ(list.size(), 1U);
    EXPECT_EQ(list.back(), TestValues<T>::VALUES[1]);
}

TYPED_TEST_P(ListTest, PushBackMultiplePreservesOrder) {
    using T = typename TypeParam::value_type;
    TypeParam list;

    list.push_back(TestValues<T>::VALUES[0]);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);

    list.push_back(TestValues<T>::VALUES[1]);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[1]);

    list.push_back(TestValues<T>::VALUES[2]);
    ASSERT_EQ(list.size(), 3U);
    EXPECT_EQ(list[2], TestValues<T>::VALUES[2]);

    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(list[2], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, PopBackRemovesLastElement) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    ASSERT_EQ(list.size(), TestValues<T>::SIZE);

    EXPECT_EQ(list.back(), TestValues<T>::VALUES[2]);
    list.pop_back();
    EXPECT_EQ(list.size(), TestValues<T>::SIZE - 1);
    EXPECT_EQ(list.back(), TestValues<T>::VALUES[1]);
}

TYPED_TEST_P(ListTest, PopBackToEmpty) {
    using T = typename TypeParam::value_type;
    TypeParam list;
    list.push_back(TestValues<T>::VALUES[0]);
    list.pop_back();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0U);
}

// ==========================
// ========= Insert =========
// ==========================

TYPED_TEST_P(ListTest, InsertIntoEmptyContainer) {
    using T = typename TypeParam::value_type;
    TypeParam list;
    list.insert(list.begin(), TestValues<T>::VALUES[0]);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
}

TYPED_TEST_P(ListTest, InsertAtBeginShiftsExistingElements) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[1], TestValues<T>::VALUES[2]});
    list.insert(list.begin(), TestValues<T>::VALUES[0]);
    ASSERT_EQ(list.size(), 3U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(list[2], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, InsertAtEndAppendsValue) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[0], TestValues<T>::VALUES[1]});
    list.insert(list.end(), TestValues<T>::VALUES[2]);
    ASSERT_EQ(list.size(), 3U);
    EXPECT_EQ(list.back(), TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, InsertAtMiddleShiftsTail) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[0], TestValues<T>::VALUES[2]});
    list.insert(list.begin() + 1, TestValues<T>::VALUES[1]);
    ASSERT_EQ(list.size(), 3U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(list[2], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, InsertMoveAtBeginShiftsExistingElements) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[1], TestValues<T>::VALUES[2]});
    T value = TestValues<T>::VALUES[0];
    list.insert(list.begin(), std::move(value));
    ASSERT_EQ(list.size(), 3U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(list[2], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, InsertSelfReferenceValue) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.insert(list.begin(), list[2]);
    ASSERT_EQ(list.size(), 4U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[2]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[2], TestValues<T>::VALUES[1]);
    EXPECT_EQ(list[3], TestValues<T>::VALUES[2]);
}

// ==========================
// ========== Erase =========
// ==========================

TYPED_TEST_P(ListTest, EraseFirstElementShiftsRemaining) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.erase(list.begin());
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[1]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, EraseLastElement) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.erase(list.end() - 1);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[1]);
}

TYPED_TEST_P(ListTest, EraseRemovesElementAndShiftsTail) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.erase(list.begin() + 1);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, EraseOnlyElementLeavesContainerEmpty) {
    using T = typename TypeParam::value_type;
    TypeParam list;
    list.push_back(TestValues<T>::VALUES[0]);
    list.erase(list.begin());
    EXPECT_TRUE(list.empty());
}

TYPED_TEST_P(ListTest, EraseFullRangeEmptiesContainer) {
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.erase(list.begin(), list.end());
    EXPECT_TRUE(list.empty());
}

TYPED_TEST_P(ListTest, EraseRangeRemovesAllElementsInRange) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::Make(
        {TestValues<T>::VALUES[0], TestValues<T>::VALUES[1], TestValues<T>::VALUES[2], TestValues<T>::UPDATED});
    list.erase(list.begin() + 1, list.begin() + 3);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::UPDATED);
}

// ==========================
// ========== Clear =========
// ==========================

TYPED_TEST_P(ListTest, ClearOnEmptyContainerIsNoop) {
    TypeParam list;
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0U);
}

TYPED_TEST_P(ListTest, ClearRemovesAllElements) {
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0U);
}

TYPED_TEST_P(ListTest, ClearPreservesCapacity) {
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    const std::size_t capacityBefore = list.capacity();
    list.clear();
    EXPECT_EQ(list.capacity(), capacityBefore);
}

TYPED_TEST_P(ListTest, ClearThenPushBackWorks) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.clear();
    list.push_back(TestValues<T>::UPDATED);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list.front(), TestValues<T>::UPDATED);
}

// ==========================
// ========== Swap ==========
// ==========================

TYPED_TEST_P(ListTest, SwapExchangesContents) {
    using T = typename TypeParam::value_type;
    TypeParam a = ContainerFactory<TypeParam>::Make({TestValues<T>::UPDATED, TestValues<T>::VALUES[1]});
    TypeParam b = ContainerFactory<TypeParam>::MakeFromTestValues();
    a.swap(b);
    ASSERT_EQ(a.size(), 3U);
    ASSERT_EQ(b.size(), 2U);
    EXPECT_EQ(a[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(b[0], TestValues<T>::UPDATED);
}

TYPED_TEST_P(ListTest, SwapWithEmptyContainer) {
    using T = typename TypeParam::value_type;
    TypeParam a = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam b;
    a.swap(b);
    EXPECT_TRUE(a.empty());
    ASSERT_EQ(b.size(), TestValues<T>::SIZE);
    EXPECT_EQ(b[0], TestValues<T>::VALUES[0]);
}

TYPED_TEST_P(ListTest, SwapTwoEmptyContainersRemainsEmpty) {
    TypeParam a;
    TypeParam b;
    a.swap(b);
    EXPECT_TRUE(a.empty());
    EXPECT_TRUE(b.empty());
}

TYPED_TEST_P(ListTest, SwapBothContainersUsableAfterSwap) {
    using T = typename TypeParam::value_type;
    TypeParam a = ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[0], TestValues<T>::VALUES[1]});
    TypeParam b = ContainerFactory<TypeParam>::MakeFromTestValues();
    a.swap(b);
    a.push_back(TestValues<T>::UPDATED);
    b.push_back(TestValues<T>::UPDATED);
    EXPECT_EQ(a.back(), TestValues<T>::UPDATED);
    EXPECT_EQ(b.back(), TestValues<T>::UPDATED);
}

// ============================
// ====== Comparision =========
// ============================

TYPED_TEST_P(ListTest, EqualsTrueForSameElements) {
    TypeParam a = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam b = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a.equals(b));
}

TYPED_TEST_P(ListTest, EqualsFalseForDifferentElements) {
    using T = typename TypeParam::value_type;
    TypeParam a = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam b =
        ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[0], TestValues<T>::UPDATED, TestValues<T>::VALUES[2]});
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a.equals(b));
}

TYPED_TEST_P(ListTest, EqualsFalseForDifferentSizes) {
    using T = typename TypeParam::value_type;
    TypeParam a = ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[0], TestValues<T>::VALUES[1]});
    TypeParam b = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a.equals(b));
}

TYPED_TEST_P(ListTest, LessThanLexicographicallyOrders) {
    using T = typename TypeParam::value_type;
    TypeParam a = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam b =
        ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[0], TestValues<T>::VALUES[1], TestValues<T>::UPDATED});
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
}

TYPED_TEST_P(ListTest, LessThanTrueWhenSizeSmallerAndPrefixEqual) {
    using T = typename TypeParam::value_type;
    TypeParam a = ContainerFactory<TypeParam>::Make({TestValues<T>::VALUES[0], TestValues<T>::VALUES[1]});
    TypeParam b = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
}

TYPED_TEST_P(ListTest, LessOrEqualAndGreaterOrEqualTrueWhenEqual) {
    TypeParam a = ContainerFactory<TypeParam>::MakeFromTestValues();
    TypeParam b = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
}

TYPED_TEST_P(ListTest, AllComparisonOperatorsTrueForTwoEmptyContainers) {
    TypeParam a;
    TypeParam b;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
}

// ==========================
// ========= Resize =========
// ==========================

TYPED_TEST_P(ListTest, ResizeSameSizeIsNoop) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.resize(TestValues<T>::SIZE);
    ASSERT_EQ(list.size(), TestValues<T>::SIZE);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(list[2], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ListTest, ResizeToZeroFromNonEmpty) {
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.resize(0);
    EXPECT_TRUE(list.empty());
}

TYPED_TEST_P(ListTest, ResizeLargerDefaultInsertsNewElements) {
    using T = typename TypeParam::value_type;
    TypeParam list;
    list.resize(2);
    ASSERT_EQ(list.size(), 2U);
    EXPECT_EQ(list[0], T{});
    EXPECT_EQ(list[1], T{});
}

TYPED_TEST_P(ListTest, ResizeLargerPreservesExistingElements) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.resize(TestValues<T>::SIZE + 2);
    ASSERT_EQ(list.size(), TestValues<T>::SIZE + 2);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(list[2], TestValues<T>::VALUES[2]);
    EXPECT_EQ(list[3], T{});
    EXPECT_EQ(list[4], T{});
}

TYPED_TEST_P(ListTest, ResizeSmallerReducesSizeKeepsFrontElements) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.resize(1);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
}

TYPED_TEST_P(ListTest, ResizeSmallerPreservesCapacity) {
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    const std::size_t capacityBefore = list.capacity();
    list.resize(1);
    EXPECT_EQ(list.capacity(), capacityBefore);
}

TYPED_TEST_P(ListTest, ResizeWithValueLargerFillsNewElementsWithValue) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.resize(TestValues<T>::SIZE + 2, TestValues<T>::UPDATED);
    ASSERT_EQ(list.size(), TestValues<T>::SIZE + 2);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(list[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(list[2], TestValues<T>::VALUES[2]);
    EXPECT_EQ(list[3], TestValues<T>::UPDATED);
    EXPECT_EQ(list[4], TestValues<T>::UPDATED);
}

TYPED_TEST_P(ListTest, ResizeWithValueSmallerReducesSize) {
    using T = typename TypeParam::value_type;
    TypeParam list = ContainerFactory<TypeParam>::MakeFromTestValues();
    list.resize(1, TestValues<T>::UPDATED);
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0], TestValues<T>::VALUES[0]);
}

REGISTER_TYPED_TEST_SUITE_P(
    ListTest, DefaultConstructedIsEmpty, BeginEqualsEndWhenEmpty, CopyConstructorFromEmpty, CopyConstructorFromNonEmpty,
    CopyConstructorIsDeepCopy, MoveConstructorFromEmpty, MoveConstructorFromNonEmpty, MoveConstructorLeavesSourceEmpty,
    CopyAssignmentFromEmpty, CopyAssignmentFromNonEmpty, CopyAssignmentOverwritesExisting, CopyAssignmentIsDeepCopy,
    MoveAssignmentFromEmpty, MoveAssignmentFromNonEmpty, MoveAssignmentLeavesSourceEmpty,
    MoveAssignmentOverwritesExisting, PushBackCopyAppendsValue, PushBackMoveAppendsValue,
    PushBackMultiplePreservesOrder, PopBackRemovesLastElement, PopBackToEmpty, InsertIntoEmptyContainer,
    InsertAtBeginShiftsExistingElements, InsertAtEndAppendsValue, InsertAtMiddleShiftsTail,
    InsertMoveAtBeginShiftsExistingElements, InsertSelfReferenceValue, EraseFirstElementShiftsRemaining,
    EraseLastElement, EraseRemovesElementAndShiftsTail, EraseOnlyElementLeavesContainerEmpty,
    EraseFullRangeEmptiesContainer, EraseRangeRemovesAllElementsInRange, ClearOnEmptyContainerIsNoop,
    ClearRemovesAllElements, ClearPreservesCapacity, ClearThenPushBackWorks, SwapExchangesContents,
    SwapWithEmptyContainer, SwapTwoEmptyContainersRemainsEmpty, SwapBothContainersUsableAfterSwap,
    EqualsTrueForSameElements, EqualsFalseForDifferentElements, EqualsFalseForDifferentSizes,
    LessThanLexicographicallyOrders, LessThanTrueWhenSizeSmallerAndPrefixEqual,
    LessOrEqualAndGreaterOrEqualTrueWhenEqual, AllComparisonOperatorsTrueForTwoEmptyContainers, ResizeSameSizeIsNoop,
    ResizeToZeroFromNonEmpty, ResizeLargerDefaultInsertsNewElements, ResizeLargerPreservesExistingElements,
    ResizeSmallerReducesSizeKeepsFrontElements, ResizeSmallerPreservesCapacity,
    ResizeWithValueLargerFillsNewElementsWithValue, ResizeWithValueSmallerReducesSize);

} // namespace StdxTest

#endif
