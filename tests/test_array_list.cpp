#include <cstddef>
#include <gtest/gtest.h>
#include <stdx/array_list.h>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// ===== Constructor / Assignment Tests =====

TEST(ArrayListConstructorTest, DefaultConstructorEmpty) {
    stdx::array_list<int> v;
    EXPECT_EQ(0u, v.size());
    EXPECT_EQ(0u, v.capacity());
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(nullptr, v.data());
}

TEST(ArrayListConstructorTest, CopyConstructorFromEmpty) {
    stdx::array_list<int> src;
    stdx::array_list<int> dst(src);
    EXPECT_EQ(0u, dst.size());
    EXPECT_TRUE(dst.empty());
}

TEST(ArrayListConstructorTest, CopyConstructorFromNonEmpty) {
    stdx::array_list<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    stdx::array_list<int> dst(src);
    ASSERT_EQ(3u, dst.size());
    EXPECT_EQ(1, dst[0]);
    EXPECT_EQ(2, dst[1]);
    EXPECT_EQ(3, dst[2]);
}

TEST(ArrayListConstructorTest, CopyConstructorIsDeepCopy) {
    stdx::array_list<int> src;
    src.push_back(10);
    src.push_back(20);

    stdx::array_list<int> dst(src);
    ASSERT_EQ(2u, dst.size());
    dst[0] = 99;

    EXPECT_EQ(10, src[0]);
    EXPECT_EQ(99, dst[0]);
}

TEST(ArrayListConstructorTest, MoveConstructorFromEmpty) {
    stdx::array_list<int> src;
    stdx::array_list<int> dst(std::move(src));
    EXPECT_EQ(0u, dst.size());
    EXPECT_TRUE(dst.empty());
}

TEST(ArrayListConstructorTest, MoveConstructorFromNonEmpty) {
    stdx::array_list<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    stdx::array_list<int> dst(std::move(src));
    ASSERT_EQ(3u, dst.size());
    EXPECT_EQ(1, dst[0]);
    EXPECT_EQ(2, dst[1]);
    EXPECT_EQ(3, dst[2]);
}

TEST(ArrayListConstructorTest, MoveConstructorSourceBecomesEmpty) {
    stdx::array_list<int> src;
    src.push_back(1);
    src.push_back(2);

    stdx::array_list<int> dst(std::move(src));
    EXPECT_TRUE(src.empty());
    EXPECT_EQ(0u, src.size());
    EXPECT_EQ(nullptr, src.data());
}

TEST(ArrayListConstructorTest, CopyAssignmentEmpty) {
    stdx::array_list<int> src;
    stdx::array_list<int> dst;
    dst = src;
    EXPECT_EQ(0u, dst.size());
    EXPECT_TRUE(dst.empty());
}

TEST(ArrayListConstructorTest, CopyAssignmentNonEmpty) {
    stdx::array_list<int> src;
    src.push_back(10);
    src.push_back(20);

    stdx::array_list<int> dst;
    dst = src;
    ASSERT_EQ(2u, dst.size());
    EXPECT_EQ(10, dst[0]);
    EXPECT_EQ(20, dst[1]);
}

TEST(ArrayListConstructorTest, CopyAssignmentOverwritesExisting) {
    stdx::array_list<int> src;
    src.push_back(10);
    src.push_back(20);

    stdx::array_list<int> dst;
    dst.push_back(99);
    dst.push_back(88);
    dst.push_back(77);

    dst = src;
    ASSERT_EQ(2u, dst.size());
    EXPECT_EQ(10, dst[0]);
    EXPECT_EQ(20, dst[1]);
}

TEST(ArrayListConstructorTest, CopyAssignmentIsDeepCopy) {
    stdx::array_list<int> src;
    src.push_back(5);

    stdx::array_list<int> dst;
    dst = src;
    ASSERT_EQ(1u, dst.size());
    dst[0] = 42;

    EXPECT_EQ(5, src[0]);
    EXPECT_EQ(42, dst[0]);
}

TEST(ArrayListConstructorTest, MoveAssignmentEmpty) {
    stdx::array_list<int> src;
    stdx::array_list<int> dst;
    dst = std::move(src);
    EXPECT_EQ(0u, dst.size());
    EXPECT_TRUE(dst.empty());
}

TEST(ArrayListConstructorTest, MoveAssignmentNonEmpty) {
    stdx::array_list<int> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    stdx::array_list<int> dst;
    dst = std::move(src);
    ASSERT_EQ(3u, dst.size());
    EXPECT_EQ(1, dst[0]);
    EXPECT_EQ(2, dst[1]);
    EXPECT_EQ(3, dst[2]);
}

TEST(ArrayListConstructorTest, MoveAssignmentSourceBecomesValid) {
    stdx::array_list<int> src;
    src.push_back(1);
    src.push_back(2);

    stdx::array_list<int> dst;
    dst = std::move(src);

    EXPECT_TRUE(src.empty());
    EXPECT_EQ(0u, src.size());
    EXPECT_EQ(nullptr, src.data());
}

TEST(ArrayListConstructorTest, MoveAssignmentOverwritesExisting) {
    stdx::array_list<int> src;
    src.push_back(7);

    stdx::array_list<int> dst;
    dst.push_back(1);
    dst.push_back(2);
    dst = std::move(src);

    ASSERT_EQ(1u, dst.size());
    EXPECT_EQ(7, dst[0]);
}

// ===== Element Access Tests =====

TEST(ArrayListElementAccessTest, AtValidIndex) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    EXPECT_EQ(10, v.at(0));
    EXPECT_EQ(20, v.at(1));
    EXPECT_EQ(30, v.at(2));
}

TEST(ArrayListElementAccessTest, AtConstValidIndex) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    const stdx::array_list<int>& cv = v;
    EXPECT_EQ(10, cv.at(0));
    EXPECT_EQ(20, cv.at(1));
}

TEST(ArrayListElementAccessTest, AtReturnsReference) {
    stdx::array_list<int> v;
    v.push_back(5);
    v.at(0) = 99;
    EXPECT_EQ(99, v.at(0));
}

TEST(ArrayListElementAccessTest, AtBoundaryLastElement) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(3, v.at(2));                    // last valid index
    EXPECT_THROW(v.at(3), std::out_of_range); // one past end
}

TEST(ArrayListElementAccessTest, AtOutOfRange) {
    stdx::array_list<int> v;
    v.push_back(1);
    EXPECT_THROW(v.at(1), std::out_of_range);
    EXPECT_THROW(v.at(100), std::out_of_range);
}

TEST(ArrayListElementAccessTest, AtConstOutOfRange) {
    stdx::array_list<int> v;
    v.push_back(1);
    const stdx::array_list<int>& cv = v;
    EXPECT_THROW(cv.at(1), std::out_of_range);
    EXPECT_THROW(cv.at(99), std::out_of_range);
}

TEST(ArrayListElementAccessTest, AtOnEmptyContainerThrows) {
    stdx::array_list<int> v;
    EXPECT_THROW(v.at(0), std::out_of_range);
}

TEST(ArrayListElementAccessTest, AtConstOnEmptyContainerThrows) {
    const stdx::array_list<int> v;
    EXPECT_THROW(v.at(0), std::out_of_range);
}

TEST(ArrayListElementAccessTest, SubscriptOperator) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(ArrayListElementAccessTest, SubscriptOperatorConst) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    const stdx::array_list<int>& cv = v;
    EXPECT_EQ(10, cv[0]);
    EXPECT_EQ(20, cv[1]);
}

TEST(ArrayListElementAccessTest, SubscriptOperatorReturnsReference) {
    stdx::array_list<int> v;
    v.push_back(5);
    v[0] = 42;
    EXPECT_EQ(42, v[0]);
}

TEST(ArrayListElementAccessTest, DataPointerNullWhenEmpty) {
    stdx::array_list<int> v;
    EXPECT_EQ(nullptr, v.data());
}

TEST(ArrayListElementAccessTest, DataPointerNullWhenEmptyConst) {
    const stdx::array_list<int> v;
    EXPECT_EQ(nullptr, v.data());
}

TEST(ArrayListElementAccessTest, DataPointerNonNull) {
    stdx::array_list<int> v;
    v.push_back(1);
    EXPECT_NE(nullptr, v.data());
}

TEST(ArrayListElementAccessTest, DataPointerConsistentWithSubscript) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    EXPECT_EQ(&v[0], v.data());
    EXPECT_EQ(10, v.data()[0]);
    EXPECT_EQ(20, v.data()[1]);
}

TEST(ArrayListElementAccessTest, DataPointerConst) {
    stdx::array_list<int> v;
    v.push_back(5);
    const stdx::array_list<int>& cv = v;
    EXPECT_NE(nullptr, cv.data());
    EXPECT_EQ(5, *cv.data());
}

TEST(ArrayListElementAccessTest, FrontElement) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(1, v.front());
}

TEST(ArrayListElementAccessTest, FrontElementConst) {
    stdx::array_list<int> v;
    v.push_back(42);
    v.push_back(99);
    const stdx::array_list<int>& cv = v;
    EXPECT_EQ(42, cv.front());
}

TEST(ArrayListElementAccessTest, FrontReturnsReference) {
    stdx::array_list<int> v;
    v.push_back(5);
    v.front() = 99;
    EXPECT_EQ(99, v.front());
}

TEST(ArrayListElementAccessTest, BackElement) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(3, v.back());
}

TEST(ArrayListElementAccessTest, BackElementConst) {
    stdx::array_list<int> v;
    v.push_back(100);
    const stdx::array_list<int>& cv = v;
    EXPECT_EQ(100, cv.back());
}

TEST(ArrayListElementAccessTest, BackReturnsReference) {
    stdx::array_list<int> v;
    v.push_back(5);
    v.back() = 77;
    EXPECT_EQ(77, v.back());
}

TEST(ArrayListElementAccessTest, FrontAndBackSameWhenSizeOne) {
    stdx::array_list<int> v;
    v.push_back(7);
    EXPECT_EQ(v.front(), v.back());
    EXPECT_EQ(&v.front(), &v.back());
}

TEST(ArrayListElementAccessTest, FrontAndBackDistinctWhenSizeTwo) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    EXPECT_EQ(1, v.front());
    EXPECT_EQ(2, v.back());
    EXPECT_NE(&v.front(), &v.back());
}

// ===== Iterator Tests =====

TEST(ArrayListIteratorTest, BeginEqualsEndWhenEmpty) {
    stdx::array_list<int> v;
    EXPECT_EQ(v.begin(), v.end());
}

TEST(ArrayListIteratorTest, BeginNotEqualsEndWhenNonEmpty) {
    stdx::array_list<int> v;
    v.push_back(1);
    EXPECT_NE(v.begin(), v.end());
}

TEST(ArrayListIteratorTest, BeginPointsToFirstElement) {
    stdx::array_list<int> v;
    v.push_back(42);
    v.push_back(7);
    EXPECT_EQ(42, *v.begin());
}

TEST(ArrayListIteratorTest, EndIsOnePastLast) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    EXPECT_EQ(2, *(v.end() - 1));
}

TEST(ArrayListIteratorTest, ForwardIteration) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    int expected = 1;
    for (auto it = v.begin(); it != v.end(); ++it) {
        EXPECT_EQ(expected++, *it);
    }
    EXPECT_EQ(4, expected);
}

TEST(ArrayListIteratorTest, RangeBasedForLoop) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    int sum = 0;
    for (int x : v) {
        sum += x;
    }
    EXPECT_EQ(60, sum);
}

TEST(ArrayListIteratorTest, ConstBeginEnd) {
    stdx::array_list<int> v;
    v.push_back(5);
    const stdx::array_list<int>& cv = v;
    EXPECT_NE(cv.begin(), cv.end());
    EXPECT_EQ(5, *cv.begin());
}

TEST(ArrayListIteratorTest, ConstBeginEqualsEndWhenEmpty) {
    const stdx::array_list<int> v;
    EXPECT_EQ(v.begin(), v.end());
}

TEST(ArrayListIteratorTest, CBeginCEnd) {
    stdx::array_list<int> v;
    v.push_back(3);
    v.push_back(4);
    EXPECT_EQ(3, *v.cbegin());
    EXPECT_NE(v.cbegin(), v.cend());
}

TEST(ArrayListIteratorTest, CBeginEqualsCBeginConst) {
    stdx::array_list<int> v;
    v.push_back(1);
    const stdx::array_list<int>& cv = v;
    EXPECT_EQ(v.cbegin(), cv.begin());
    EXPECT_EQ(v.cend(), cv.end());
}

TEST(ArrayListIteratorTest, ReverseIterationEmpty) {
    stdx::array_list<int> v;
    EXPECT_EQ(v.rbegin(), v.rend());
}

TEST(ArrayListIteratorTest, ReverseIteration) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    std::vector<int> result;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        result.push_back(*it);
    }
    ASSERT_EQ(3u, result.size());
    EXPECT_EQ(3, result[0]);
    EXPECT_EQ(2, result[1]);
    EXPECT_EQ(1, result[2]);
}

TEST(ArrayListIteratorTest, ConstReverseIteration) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    const stdx::array_list<int>& cv = v;

    auto it = cv.rbegin();
    EXPECT_EQ(20, *it);
    ++it;
    EXPECT_EQ(10, *it);
    ++it;
    EXPECT_EQ(cv.rend(), it);
}

TEST(ArrayListIteratorTest, CRBeginCREnd) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);

    std::vector<int> result;
    for (auto it = v.crbegin(); it != v.crend(); ++it) {
        result.push_back(*it);
    }
    ASSERT_EQ(2u, result.size());
    EXPECT_EQ(2, result[0]);
    EXPECT_EQ(1, result[1]);
}

TEST(ArrayListIteratorTest, MutableIteratorAllowsWrite) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    for (auto it = v.begin(); it != v.end(); ++it) {
        *it *= 2;
    }
    EXPECT_EQ(2, v[0]);
    EXPECT_EQ(4, v[1]);
    EXPECT_EQ(6, v[2]);
}

// ===== Capacity Tests =====

TEST(ArrayListCapacityTest, EmptyOnDefault) {
    stdx::array_list<int> v;
    EXPECT_TRUE(v.empty());
}

TEST(ArrayListCapacityTest, NotEmptyAfterPushBack) {
    stdx::array_list<int> v;
    v.push_back(1);
    EXPECT_FALSE(v.empty());
}

TEST(ArrayListCapacityTest, EmptyAfterClear) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.clear();
    EXPECT_TRUE(v.empty());
}

TEST(ArrayListCapacityTest, SizeDefault) {
    stdx::array_list<int> v;
    EXPECT_EQ(0u, v.size());
}

TEST(ArrayListCapacityTest, SizeAfterPushBack) {
    stdx::array_list<int> v;
    v.push_back(1);
    EXPECT_EQ(1u, v.size());
    v.push_back(2);
    EXPECT_EQ(2u, v.size());
    v.push_back(3);
    EXPECT_EQ(3u, v.size());
}

TEST(ArrayListCapacityTest, SizeAfterPopBack) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.pop_back();
    EXPECT_EQ(1u, v.size());
    v.pop_back();
    EXPECT_EQ(0u, v.size());
}

TEST(ArrayListCapacityTest, CapacityDefault) {
    stdx::array_list<int> v;
    EXPECT_EQ(0u, v.capacity());
}

TEST(ArrayListCapacityTest, CapacityAtLeastSize) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_GE(v.capacity(), v.size());
}

TEST(ArrayListCapacityTest, ReserveIncreasesCapacity) {
    stdx::array_list<int> v;
    v.reserve(10);
    EXPECT_GE(v.capacity(), 10u);
}

TEST(ArrayListCapacityTest, ReserveDoesNotChangeSize) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.reserve(100);
    EXPECT_EQ(2u, v.size());
}

TEST(ArrayListCapacityTest, ReserveNoopWhenSufficient) {
    stdx::array_list<int> v;
    v.reserve(10);
    std::size_t cap = v.capacity();
    v.reserve(5);
    EXPECT_EQ(cap, v.capacity());
}

TEST(ArrayListCapacityTest, ReserveZeroOnEmptyIsNoop) {
    stdx::array_list<int> v;
    v.reserve(0);
    EXPECT_EQ(0u, v.size());
    EXPECT_EQ(0u, v.capacity());
}

TEST(ArrayListCapacityTest, ReservePreservesElements) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.reserve(100);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(ArrayListCapacityTest, ShrinkToFitReducesCapacityToSize) {
    stdx::array_list<int, stdx::allocator<int>, stdx::doubling_growth<>> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3); // capacity doubles to 4 with doubling_growth
    ASSERT_GE(v.capacity(), v.size());
    v.shrink_to_fit();
    EXPECT_EQ(v.size(), v.capacity());
}

TEST(ArrayListCapacityTest, ShrinkToFitPreservesElements) {
    stdx::array_list<int, stdx::allocator<int>, stdx::doubling_growth<>> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.shrink_to_fit();
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(10, v[0]);
    EXPECT_EQ(20, v[1]);
    EXPECT_EQ(30, v[2]);
}

TEST(ArrayListCapacityTest, ShrinkToFitOnAlreadyFitContainer) {
    stdx::array_list<int> v; // exact_growth: capacity always equals size
    v.push_back(1);
    v.push_back(2);
    ASSERT_EQ(v.size(), v.capacity());
    v.shrink_to_fit();
    EXPECT_EQ(v.size(), v.capacity());
}

TEST(ArrayListCapacityTest, ShrinkToFitOnEmpty) {
    stdx::array_list<int, stdx::allocator<int>, stdx::doubling_growth<>> v;
    v.push_back(1);
    v.push_back(2);
    v.pop_back();
    v.pop_back();
    EXPECT_EQ(0u, v.size());
    v.shrink_to_fit();
    EXPECT_EQ(0u, v.capacity());
}

// ===== Modifier Tests =====

TEST(ArrayListModifierTest, PushBackCopyIncreasesSize) {
    stdx::array_list<int> v;
    int val = 42;
    v.push_back(val);
    EXPECT_EQ(1u, v.size());
    EXPECT_EQ(42, v[0]);
    EXPECT_EQ(42, val); // source unchanged
}

TEST(ArrayListModifierTest, PushBackCopyMultiple) {
    stdx::array_list<int> v;
    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
    }
    ASSERT_EQ(10u, v.size());
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(i, v[i]);
    }
}

TEST(ArrayListModifierTest, PushBackMove) {
    stdx::array_list<std::string> v;
    std::string s = "hello";
    v.push_back(std::move(s));
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ("hello", v[0]);
}

TEST(ArrayListModifierTest, PushBackMoveSourceEmptied) {
    stdx::array_list<std::string> v;
    std::string s = "hello";
    v.push_back(std::move(s));
    EXPECT_TRUE(s.empty());
}

TEST(ArrayListModifierTest, PushBackTriggersGrow) {
    // Verifies elements survive a grow reallocation
    stdx::array_list<int> v;
    for (int i = 0; i < 5; ++i) {
        v.push_back(i * 10);
    }
    ASSERT_EQ(5u, v.size());
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(i * 10, v[i]);
    }
}

TEST(ArrayListModifierTest, PopBackReducesSize) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.pop_back();
    EXPECT_EQ(1u, v.size());
}

TEST(ArrayListModifierTest, PopBackRemovesLastElement) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.pop_back();
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
}

TEST(ArrayListModifierTest, PopBackToEmpty) {
    stdx::array_list<int> v;
    v.push_back(42);
    v.pop_back();
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(ArrayListModifierTest, InsertAtBeginCopy) {
    stdx::array_list<int> v;
    v.push_back(2);
    v.push_back(3);
    auto it = v.insert(v.begin(), 1);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(1, *it);
    EXPECT_EQ(v.begin(), it);
}

TEST(ArrayListModifierTest, InsertAtEndCopy) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    auto it = v.insert(v.end(), 3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(3, *it);
}

TEST(ArrayListModifierTest, InsertAtMiddleCopy) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(3);
    auto it = v.insert(v.begin() + 1, 2);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(2, *it);
}

TEST(ArrayListModifierTest, InsertIntoEmptyCopy) {
    stdx::array_list<int> v;
    auto it = v.insert(v.end(), 42);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(42, v[0]);
    EXPECT_EQ(42, *it);
}

TEST(ArrayListModifierTest, InsertAtBeginMove) {
    stdx::array_list<std::string> v;
    v.push_back("world");
    std::string s = "hello";
    auto it = v.insert(v.begin(), std::move(s));
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ("hello", v[0]);
    EXPECT_EQ("world", v[1]);
    EXPECT_EQ("hello", *it);
}

TEST(ArrayListModifierTest, InsertAtEndMove) {
    stdx::array_list<std::string> v;
    v.push_back("hello");
    std::string s = "world";
    auto it = v.insert(v.end(), std::move(s));
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ("hello", v[0]);
    EXPECT_EQ("world", v[1]);
    EXPECT_EQ("world", *it);
}

TEST(ArrayListModifierTest, InsertAtMiddleMove) {
    stdx::array_list<std::string> v;
    v.push_back("a");
    v.push_back("c");
    std::string s = "b";
    auto it = v.insert(v.begin() + 1, std::move(s));
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ("a", v[0]);
    EXPECT_EQ("b", v[1]);
    EXPECT_EQ("c", v[2]);
    EXPECT_EQ("b", *it);
}

TEST(ArrayListModifierTest, InsertSelfReferenceWithSpareCapacity) {
    stdx::array_list<int> v;
    v.reserve(8); // ensure the in-place (non-reallocating) path is taken
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    // value aliases an element already in the container
    auto it = v.insert(v.begin(), v[2]);
    ASSERT_EQ(4u, v.size());
    EXPECT_EQ(30, v[0]); // the original v[2] value, not a shifted-over one
    EXPECT_EQ(10, v[1]);
    EXPECT_EQ(20, v[2]);
    EXPECT_EQ(30, v[3]);
    EXPECT_EQ(30, *it);

    it = v.insert(v.begin(), std::move(v[2]));
    ASSERT_EQ(5u, v.size());
    EXPECT_EQ(20, v[0]);
    EXPECT_EQ(30, v[1]); // the original v[2] value, not a shifted-over one
    EXPECT_EQ(10, v[2]);
    EXPECT_EQ(20, v[3]);
    EXPECT_EQ(30, v[4]);
    EXPECT_EQ(20, *it);
}

TEST(ArrayListModifierTest, EraseFirst) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    auto it = v.erase(v.begin());
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(2, v[0]);
    EXPECT_EQ(3, v[1]);
    EXPECT_EQ(v.begin(), it);
    EXPECT_EQ(2, *it);
}

TEST(ArrayListModifierTest, EraseLast) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    auto it = v.erase(v.end() - 1);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(v.end(), it);
}

TEST(ArrayListModifierTest, EraseMiddle) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    auto it = v.erase(v.begin() + 1);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(3, v[1]);
    EXPECT_EQ(3, *it);
}

TEST(ArrayListModifierTest, EraseOnlyElement) {
    stdx::array_list<int> v;
    v.push_back(42);
    auto it = v.erase(v.begin());
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.end(), it);
}

TEST(ArrayListModifierTest, EraseConstIteratorOverload) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    stdx::array_list<int>::const_iterator pos = v.cbegin() + 1;
    auto it = v.erase(pos);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(3, v[1]);
    EXPECT_EQ(3, *it);
}

TEST(ArrayListModifierTest, EraseConstIteratorFirst) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    auto it = v.erase(v.cbegin());
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(20, v[0]);
    EXPECT_EQ(v.begin(), it);
}

TEST(ArrayListModifierTest, EraseRangeEmpty) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    // Empty range [begin+1, begin+1)
    auto it = v.erase(v.begin() + 1, v.begin() + 1);
    EXPECT_EQ(2u, v.size());
    EXPECT_EQ(v.begin() + 1, it);
}

TEST(ArrayListModifierTest, EraseRangeAll) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    auto it = v.erase(v.begin(), v.end());
    EXPECT_EQ(0u, v.size());
    EXPECT_EQ(v.end(), it);
}

TEST(ArrayListModifierTest, EraseRangePartial) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.push_back(5);
    auto it = v.erase(v.begin() + 1, v.begin() + 3); // removes 2 and 3
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(4, v[1]);
    EXPECT_EQ(5, v[2]);
    EXPECT_EQ(4, *it);
}

TEST(ArrayListModifierTest, EraseRangeFromBegin) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    auto it = v.erase(v.begin(), v.begin() + 2); // removes 1 and 2
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(3, v[0]);
    EXPECT_EQ(4, v[1]);
    EXPECT_EQ(v.begin(), it);
    EXPECT_EQ(3, *it);
}

TEST(ArrayListModifierTest, EraseRangeToEnd) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    auto it = v.erase(v.begin() + 2, v.end()); // removes 3 and 4
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(v.end(), it);
}

TEST(ArrayListModifierTest, EraseRangeConstIteratorOverload) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    stdx::array_list<int>::const_iterator first = v.cbegin();
    stdx::array_list<int>::const_iterator last = v.cbegin() + 2;
    auto it = v.erase(first, last);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(30, v[0]);
    EXPECT_EQ(v.begin(), it);
}

TEST(ArrayListModifierTest, EraseRangeConstIteratorEmpty) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    auto it = v.erase(v.cbegin() + 1, v.cbegin() + 1);
    EXPECT_EQ(2u, v.size());
    EXPECT_EQ(v.begin() + 1, it);
}

TEST(ArrayListModifierTest, ClearEmptyContainer) {
    stdx::array_list<int> v;
    v.clear();
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(ArrayListModifierTest, ClearNonEmpty) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.clear();
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(ArrayListModifierTest, ClearPreservesCapacity) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    std::size_t cap = v.capacity();
    v.clear();
    EXPECT_EQ(cap, v.capacity());
}

TEST(ArrayListModifierTest, ClearThenPushBack) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.clear();
    v.push_back(99);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(99, v[0]);
}

TEST(ArrayListModifierTest, SwapTwoNonEmpty) {
    stdx::array_list<int> a;
    a.push_back(1);
    a.push_back(2);

    stdx::array_list<int> b;
    b.push_back(10);
    b.push_back(20);
    b.push_back(30);

    a.swap(b);

    ASSERT_EQ(3u, a.size());
    EXPECT_EQ(10, a[0]);
    EXPECT_EQ(20, a[1]);
    EXPECT_EQ(30, a[2]);

    ASSERT_EQ(2u, b.size());
    EXPECT_EQ(1, b[0]);
    EXPECT_EQ(2, b[1]);
}

TEST(ArrayListModifierTest, SwapWithEmpty) {
    stdx::array_list<int> a;
    a.push_back(1);
    a.push_back(2);

    stdx::array_list<int> b;

    a.swap(b);

    EXPECT_TRUE(a.empty());
    ASSERT_EQ(2u, b.size());
    EXPECT_EQ(1, b[0]);
    EXPECT_EQ(2, b[1]);
}

TEST(ArrayListModifierTest, SwapTwoEmptyContainers) {
    stdx::array_list<int> a;
    stdx::array_list<int> b;
    a.swap(b);
    EXPECT_TRUE(a.empty());
    EXPECT_TRUE(b.empty());
}

TEST(ArrayListModifierTest, SwapPreservesCapacity) {
    stdx::array_list<int, stdx::allocator<int>, stdx::doubling_growth<>> a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3); // cap=4

    stdx::array_list<int, stdx::allocator<int>, stdx::doubling_growth<>> b;
    b.push_back(10); // cap=1

    std::size_t capA = a.capacity();
    std::size_t capB = b.capacity();

    a.swap(b);

    EXPECT_EQ(capB, a.capacity());
    EXPECT_EQ(capA, b.capacity());
}

TEST(ArrayListModifierTest, SwapBothContainersUsableAfterSwap) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    b.push_back(2);
    a.swap(b);
    a.push_back(3);
    b.push_back(4);
    ASSERT_EQ(2u, a.size());
    ASSERT_EQ(2u, b.size());
    EXPECT_EQ(2, a[0]);
    EXPECT_EQ(3, a[1]);
    EXPECT_EQ(1, b[0]);
    EXPECT_EQ(4, b[1]);
}

// ===== Comparison Tests =====

TEST(ArrayListComparisonTest, EqualsTwoEmpty) {
    stdx::array_list<int> a, b;
    EXPECT_TRUE(a.equals(b));
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(ArrayListComparisonTest, EqualsSameElements) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    EXPECT_TRUE(a.equals(b));
    EXPECT_TRUE(a == b);
}

TEST(ArrayListComparisonTest, EqualsNotEqualElements) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(1);
    b.push_back(3);
    EXPECT_FALSE(a.equals(b));
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(ArrayListComparisonTest, EqualsNotEqualSizesASmaller) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    EXPECT_FALSE(a.equals(b));
    EXPECT_FALSE(a == b);
}

TEST(ArrayListComparisonTest, EqualsNotEqualSizesALarger) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    b.push_back(1);
    b.push_back(2);
    EXPECT_FALSE(a.equals(b));
    EXPECT_FALSE(a == b);
}

TEST(ArrayListComparisonTest, EqualsEmptyVsNonEmpty) {
    stdx::array_list<int> a, b;
    b.push_back(1);
    EXPECT_FALSE(a.equals(b));
    EXPECT_FALSE(b.equals(a));
}

TEST(ArrayListComparisonTest, EqualsSingleElement) {
    stdx::array_list<int> a, b;
    a.push_back(5);
    b.push_back(5);
    EXPECT_TRUE(a.equals(b));

    stdx::array_list<int> c;
    c.push_back(6);
    EXPECT_FALSE(a.equals(c));
}

TEST(ArrayListComparisonTest, LessThanLexicographic) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    b.push_back(1);
    b.push_back(2);
    b.push_back(4);
    EXPECT_TRUE(a.less_than(b));
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
    EXPECT_FALSE(b.less_than(a));
}

TEST(ArrayListComparisonTest, LessThanBySize) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    EXPECT_TRUE(a.less_than(b));
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(ArrayListComparisonTest, LessThanEqualIsNotLess) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(1);
    b.push_back(2);
    EXPECT_FALSE(a.less_than(b));
    EXPECT_FALSE(a < b);
}

TEST(ArrayListComparisonTest, LessThanBothEmpty) {
    stdx::array_list<int> a, b;
    EXPECT_FALSE(a.less_than(b));
    EXPECT_FALSE(a < b);
}

TEST(ArrayListComparisonTest, LessThanFirstElementDecides) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    b.push_back(2);
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(ArrayListComparisonTest, GreaterThan) {
    stdx::array_list<int> a, b;
    a.push_back(5);
    b.push_back(3);
    EXPECT_TRUE(a > b);
    EXPECT_FALSE(b > a);
}

TEST(ArrayListComparisonTest, GreaterThanEqualNotGreater) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    b.push_back(1);
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(b > a);
}

TEST(ArrayListComparisonTest, LessOrEqualWhenEqual) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    b.push_back(1);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b <= a);
}

TEST(ArrayListComparisonTest, LessOrEqualWhenLess) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    b.push_back(2);
    EXPECT_TRUE(a <= b);
    EXPECT_FALSE(b <= a);
}

TEST(ArrayListComparisonTest, GreaterOrEqualWhenEqual) {
    stdx::array_list<int> a, b;
    a.push_back(1);
    b.push_back(1);
    EXPECT_TRUE(a >= b);
    EXPECT_TRUE(b >= a);
}

TEST(ArrayListComparisonTest, GreaterOrEqualWhenGreater) {
    stdx::array_list<int> a, b;
    a.push_back(3);
    b.push_back(2);
    EXPECT_TRUE(a >= b);
    EXPECT_FALSE(b >= a);
}

TEST(ArrayListComparisonTest, AllOperatorsEmptyContainers) {
    stdx::array_list<int> a, b;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
}

// ===== Growth Policy Tests =====

TEST(ArrayListGrowthPolicyTest, ExactGrowthCapacityEqualsSize) {
    stdx::array_list<int, stdx::allocator<int>, stdx::exact_growth<>> v;
    v.push_back(1);
    EXPECT_EQ(1u, v.capacity());
    v.push_back(2);
    EXPECT_EQ(2u, v.capacity());
    v.push_back(3);
    EXPECT_EQ(3u, v.capacity());
}

TEST(ArrayListGrowthPolicyTest, DoublingGrowthCapacitySequence) {
    stdx::array_list<int, stdx::allocator<int>, stdx::doubling_growth<>> v;
    v.push_back(1); // req=1, cap: 0->1 = 1
    EXPECT_EQ(1u, v.capacity());
    v.push_back(2); // req=2, cap: 1->2 = 2
    EXPECT_EQ(2u, v.capacity());
    v.push_back(3); // req=3, cap: 2->4 = 4
    EXPECT_EQ(4u, v.capacity());
    v.push_back(4); // req=4, fits in 4
    EXPECT_EQ(4u, v.capacity());
    v.push_back(5); // req=5, cap: 4->8 = 8
    EXPECT_EQ(8u, v.capacity());
}

TEST(ArrayListGrowthPolicyTest, DoublingGrowthPreservesElements) {
    stdx::array_list<int, stdx::allocator<int>, stdx::doubling_growth<>> v;
    for (int i = 0; i < 8; ++i) {
        v.push_back(i * 3);
    }
    ASSERT_EQ(8u, v.size());
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(i * 3, v[i]);
    }
}

// ===== emplace_back / emplace Tests =====

namespace {

struct Point {
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

struct MoveOnly {
    int value;
    explicit MoveOnly(int v) : value(v) {}
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;
};

struct ConstructionCounter {
    static int direct_constructions;
    static int copy_constructions;
    static int move_constructions;

    int value;

    explicit ConstructionCounter(int v) : value(v) { ++direct_constructions; }
    ConstructionCounter(const ConstructionCounter& o) : value(o.value) { ++copy_constructions; }
    ConstructionCounter(ConstructionCounter&& o) noexcept : value(o.value) { ++move_constructions; }
    ConstructionCounter& operator=(const ConstructionCounter&) = default;
    ConstructionCounter& operator=(ConstructionCounter&&) = default;
};

int ConstructionCounter::direct_constructions = 0;
int ConstructionCounter::copy_constructions = 0;
int ConstructionCounter::move_constructions = 0;

} // namespace

TEST(ArrayListModifierTest, EmplaceBackIntoEmpty) {
    stdx::array_list<int> v;
    v.emplace_back(42);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(42, v[0]);
}

TEST(ArrayListModifierTest, EmplaceBackIncreasesSize) {
    stdx::array_list<int> v;
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(ArrayListModifierTest, EmplaceBackWithMultipleArgs) {
    stdx::array_list<Point> v;
    v.emplace_back(3, 7);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(3, v[0].x);
    EXPECT_EQ(7, v[0].y);
}

TEST(ArrayListModifierTest, EmplaceBackConstructsInPlace) {
    stdx::array_list<MoveOnly> v;
    v.emplace_back(99);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(99, v[0].value);
}

TEST(ArrayListModifierTest, EmplaceBackNoCopyOrMove) {
    ConstructionCounter::direct_constructions = 0;
    ConstructionCounter::copy_constructions = 0;
    ConstructionCounter::move_constructions = 0;

    stdx::array_list<ConstructionCounter> v;
    v.reserve(1);
    v.emplace_back(5);

    EXPECT_EQ(1, ConstructionCounter::direct_constructions);
    EXPECT_EQ(0, ConstructionCounter::copy_constructions);
    EXPECT_EQ(0, ConstructionCounter::move_constructions);
}

TEST(ArrayListModifierTest, EmplaceBackWithStringArgs) {
    stdx::array_list<std::string> v;
    v.emplace_back(3, 'z');
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ("zzz", v[0]);
}

TEST(ArrayListModifierTest, EmplaceBackTriggersGrow) {
    stdx::array_list<int> v;
    for (int i = 0; i < 10; ++i) {
        v.emplace_back(i);
    }
    ASSERT_EQ(10u, v.size());
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(i, v[i]);
    }
}

TEST(ArrayListModifierTest, EmplaceIntoEmpty) {
    stdx::array_list<int> v;
    auto it = v.emplace(v.end(), 42);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(42, v[0]);
    EXPECT_EQ(42, *it);
}

TEST(ArrayListModifierTest, EmplaceAtBegin) {
    stdx::array_list<int> v;
    v.push_back(2);
    v.push_back(3);
    auto it = v.emplace(v.begin(), 1);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(1, *it);
    EXPECT_EQ(v.begin(), it);
}

TEST(ArrayListModifierTest, EmplaceAtEnd) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    auto it = v.emplace(v.end(), 3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(3, *it);
}

TEST(ArrayListModifierTest, EmplaceAtMiddle) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(3);
    auto it = v.emplace(v.begin() + 1, 2);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(2, *it);
}

TEST(ArrayListModifierTest, EmplaceWithMultipleArgs) {
    stdx::array_list<Point> v;
    v.emplace_back(1, 2);
    auto it = v.emplace(v.begin(), 10, 20);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(10, v[0].x);
    EXPECT_EQ(20, v[0].y);
    EXPECT_EQ(1, v[1].x);
    EXPECT_EQ(2, v[1].y);
    EXPECT_EQ(10, it->x);
    EXPECT_EQ(20, it->y);
}

TEST(ArrayListModifierTest, EmplaceConstructsInPlace) {
    stdx::array_list<MoveOnly> v;
    v.emplace_back(1);
    auto it = v.emplace(v.begin(), 99);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(99, v[0].value);
    EXPECT_EQ(1, v[1].value);
    EXPECT_EQ(99, it->value);
}

TEST(ArrayListModifierTest, EmplaceNoCopyOrMoveIntoReservedSlot) {
    ConstructionCounter::direct_constructions = 0;
    ConstructionCounter::copy_constructions = 0;
    ConstructionCounter::move_constructions = 0;

    stdx::array_list<ConstructionCounter> v;
    v.reserve(1);
    v.emplace(v.end(), 7);

    EXPECT_EQ(1, ConstructionCounter::direct_constructions);
    EXPECT_EQ(0, ConstructionCounter::copy_constructions);
    EXPECT_EQ(0, ConstructionCounter::move_constructions);
}

// ===== Member Types =====

TEST(ArrayListMemberTypesTest, ValueType) {
    static_assert(std::is_same<stdx::array_list<int>::value_type, int>::value, "");
    static_assert(std::is_same<stdx::array_list<std::string>::value_type, std::string>::value, "");
}

TEST(ArrayListMemberTypesTest, SizeType) {
    static_assert(std::is_same<stdx::array_list<int>::size_type, std::size_t>::value, "");
}

TEST(ArrayListMemberTypesTest, DifferenceType) {
    static_assert(std::is_same<stdx::array_list<int>::difference_type, std::ptrdiff_t>::value, "");
}

TEST(ArrayListMemberTypesTest, Reference) {
    static_assert(std::is_same<stdx::array_list<int>::reference, int&>::value, "");
    static_assert(std::is_same<stdx::array_list<double>::reference, double&>::value, "");
}

TEST(ArrayListMemberTypesTest, ConstReference) {
    static_assert(std::is_same<stdx::array_list<int>::const_reference, const int&>::value, "");
    static_assert(std::is_same<stdx::array_list<double>::const_reference, const double&>::value, "");
}

TEST(ArrayListMemberTypesTest, AllocatorType) {
    static_assert(std::is_same<stdx::array_list<int>::allocator_type, stdx::allocator<int>>::value, "");
}

TEST(ArrayListMemberTypesTest, Pointer) {
    static_assert(std::is_same<stdx::array_list<int>::pointer, int*>::value, "");
}

TEST(ArrayListMemberTypesTest, ConstPointer) {
    static_assert(std::is_same<stdx::array_list<int>::const_pointer, const int*>::value, "");
}

TEST(ArrayListMemberTypesTest, Iterator) {
    static_assert(std::is_same<stdx::array_list<int>::iterator, int*>::value, "");
}

TEST(ArrayListMemberTypesTest, ConstIterator) {
    static_assert(std::is_same<stdx::array_list<int>::const_iterator, const int*>::value, "");
}

TEST(ArrayListMemberTypesTest, ReverseIterator) {
    using expected = stdx::reverse_iterator<int*>;
    static_assert(std::is_same<stdx::array_list<int>::reverse_iterator, expected>::value, "");
}

TEST(ArrayListMemberTypesTest, ConstReverseIterator) {
    using expected = stdx::reverse_iterator<const int*>;
    static_assert(std::is_same<stdx::array_list<int>::const_reverse_iterator, expected>::value, "");
}

TEST(ArrayListMemberTypesTest, ReferenceIsActuallyMutable) {
    stdx::array_list<int> v;
    v.push_back(1);
    stdx::array_list<int>::reference ref = v[0];
    ref = 42;
    EXPECT_EQ(42, v[0]);
}

TEST(ArrayListMemberTypesTest, ConstReferenceFromConstContainer) {
    stdx::array_list<int> v;
    v.push_back(7);
    const stdx::array_list<int>& cv = v;
    stdx::array_list<int>::const_reference cref = cv[0];
    EXPECT_EQ(7, cref);
}

TEST(ArrayListMemberTypesTest, PointerPointsToElement) {
    stdx::array_list<int> v;
    v.push_back(5);
    stdx::array_list<int>::pointer p = v.data();
    ASSERT_NE(nullptr, p);
    EXPECT_EQ(5, *p);
}

TEST(ArrayListMemberTypesTest, ConstPointerFromConstData) {
    stdx::array_list<int> v;
    v.push_back(9);
    const stdx::array_list<int>& cv = v;
    stdx::array_list<int>::const_pointer p = cv.data();
    ASSERT_NE(nullptr, p);
    EXPECT_EQ(9, *p);
}

// ===== max_size =====

TEST(ArrayListCapacityTest, MaxSizeIsMaxSizeT) {
    stdx::array_list<int> v;
    std::vector<int> vec;
    EXPECT_EQ(vec.max_size(), v.max_size());
}

TEST(ArrayListCapacityTest, MaxSizeIsConstant) {
    stdx::array_list<int> empty;
    stdx::array_list<int> nonempty;
    nonempty.push_back(1);
    nonempty.push_back(2);
    EXPECT_EQ(empty.max_size(), nonempty.max_size());
}

TEST(ArrayListCapacityTest, MaxSizeGreaterThanSize) {
    stdx::array_list<int> v;
    v.push_back(1);
    EXPECT_GT(v.max_size(), v.size());
}

TEST(ArrayListCapacityTest, MaxSizeGreaterThanCapacity) {
    stdx::array_list<int> v;
    v.reserve(1000);
    EXPECT_GT(v.max_size(), v.capacity());
}

// ===== Resize Tests =====

namespace {

struct DestructorCounter {
    static int destructions;
    int value;
    DestructorCounter() : value(0) {}
    explicit DestructorCounter(int v) : value(v) {}
    ~DestructorCounter() { ++destructions; }
    DestructorCounter(const DestructorCounter&) = default;
    DestructorCounter& operator=(const DestructorCounter&) = default;
    DestructorCounter(DestructorCounter&&) noexcept = default;
    DestructorCounter& operator=(DestructorCounter&&) noexcept = default;
};
int DestructorCounter::destructions = 0;

} // namespace

// --- resize(count) ---

TEST(ArrayListResizeTest, ResizeSameSizeIsNoop) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    std::size_t capBefore = v.capacity();
    v.resize(3);
    EXPECT_EQ(3u, v.size());
    EXPECT_EQ(capBefore, v.capacity());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(ArrayListResizeTest, ResizeToZeroFromEmpty) {
    stdx::array_list<int> v;
    v.resize(0);
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(ArrayListResizeTest, ResizeToZeroFromNonEmpty) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.resize(0);
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(ArrayListResizeTest, ResizeLargerFromEmpty) {
    stdx::array_list<int> v;
    v.resize(3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(0, v[0]);
    EXPECT_EQ(0, v[1]);
    EXPECT_EQ(0, v[2]);
}

TEST(ArrayListResizeTest, ResizeLargerPreservesExistingElements) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.resize(5);
    ASSERT_EQ(5u, v.size());
    EXPECT_EQ(10, v[0]);
    EXPECT_EQ(20, v[1]);
    EXPECT_EQ(30, v[2]);
}

TEST(ArrayListResizeTest, ResizeLargerDefaultInitializesNewElements) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    v.resize(5);
    ASSERT_EQ(5u, v.size());
    EXPECT_EQ(0, v[2]);
    EXPECT_EQ(0, v[3]);
    EXPECT_EQ(0, v[4]);
}

TEST(ArrayListResizeTest, ResizeLargerCapacityAtLeastCount) {
    stdx::array_list<int> v;
    v.resize(10);
    EXPECT_GE(v.capacity(), 10u);
}

TEST(ArrayListResizeTest, ResizeLargerWhenCapacityAlreadySufficient) {
    stdx::array_list<int> v;
    v.reserve(10);
    v.push_back(1);
    v.push_back(2);
    std::size_t capBefore = v.capacity();
    v.resize(5);
    EXPECT_EQ(5u, v.size());
    EXPECT_EQ(capBefore, v.capacity());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(0, v[2]);
    EXPECT_EQ(0, v[3]);
    EXPECT_EQ(0, v[4]);
}

TEST(ArrayListResizeTest, ResizeLargerToExactCapacityDoesNotReallocate) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2); // size=2, capacity=2
    v.pop_back();   // size=1, capacity=2
    const int* dataBefore = v.data();
    v.resize(2); // count == capacity: must NOT reallocate
    EXPECT_EQ(dataBefore, v.data());
    EXPECT_EQ(2u, v.size());
}

TEST(ArrayListResizeTest, ResizeSmallerReducesSize) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.push_back(5);
    v.resize(3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(ArrayListResizeTest, ResizeSmallerPreservesCapacity) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    std::size_t capBefore = v.capacity();
    v.resize(1);
    EXPECT_EQ(1u, v.size());
    EXPECT_EQ(capBefore, v.capacity());
}

TEST(ArrayListResizeTest, ResizeSmallerDestroysRemovedElements) {
    DestructorCounter::destructions = 0;
    {
        stdx::array_list<DestructorCounter> v;
        v.reserve(3);
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace_back(3);
        DestructorCounter::destructions = 0;
        v.resize(1);
        EXPECT_EQ(2, DestructorCounter::destructions);
        EXPECT_EQ(1u, v.size());
        EXPECT_EQ(1, v[0].value);
    }
}

TEST(ArrayListResizeTest, ResizeLargerWithNonTrivialTypeDefaultInserted) {
    stdx::array_list<std::string> v;
    v.push_back("hello");
    v.resize(3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ("hello", v[0]);
    EXPECT_EQ("", v[1]);
    EXPECT_EQ("", v[2]);
}

TEST(ArrayListResizeTest, ResizeSmallerWithNonTrivialType) {
    stdx::array_list<std::string> v;
    v.push_back("a");
    v.push_back("b");
    v.push_back("c");
    v.resize(1);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ("a", v[0]);
}

TEST(ArrayListResizeTest, ResizeThenPushBackWorks) {
    stdx::array_list<int> v;
    v.resize(3);
    v.push_back(99);
    ASSERT_EQ(4u, v.size());
    EXPECT_EQ(0, v[0]);
    EXPECT_EQ(0, v[1]);
    EXPECT_EQ(0, v[2]);
    EXPECT_EQ(99, v[3]);
}

TEST(ArrayListResizeTest, ResizeSmallerThenLargerDefaultInitializesNewElements) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.resize(1);
    v.resize(3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(10, v[0]);
    EXPECT_EQ(0, v[1]);
    EXPECT_EQ(0, v[2]);
}

// --- resize(count, value) ---

TEST(ArrayListResizeTest, ResizeWithValueSameSizeIsNoop) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    std::size_t capBefore = v.capacity();
    v.resize(3, 99);
    EXPECT_EQ(3u, v.size());
    EXPECT_EQ(capBefore, v.capacity());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(ArrayListResizeTest, ResizeWithValueToZeroFromNonEmpty) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.resize(0, 99);
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(ArrayListResizeTest, ResizeWithValueLargerFromEmpty) {
    stdx::array_list<int> v;
    v.resize(3, 42);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(42, v[0]);
    EXPECT_EQ(42, v[1]);
    EXPECT_EQ(42, v[2]);
}

TEST(ArrayListResizeTest, ResizeWithValueLargerPreservesExistingElements) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    v.resize(5, 99);
    ASSERT_EQ(5u, v.size());
    EXPECT_EQ(10, v[0]);
    EXPECT_EQ(20, v[1]);
    EXPECT_EQ(99, v[2]);
    EXPECT_EQ(99, v[3]);
    EXPECT_EQ(99, v[4]);
}

TEST(ArrayListResizeTest, ResizeWithValueSmallerReducesSize) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.resize(2, 99);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
}

TEST(ArrayListResizeTest, ResizeWithValueSmallerPreservesCapacity) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    std::size_t capBefore = v.capacity();
    v.resize(1, 99);
    EXPECT_EQ(1u, v.size());
    EXPECT_EQ(capBefore, v.capacity());
}

TEST(ArrayListResizeTest, ResizeWithValueLargerCapacityAtLeastCount) {
    stdx::array_list<int> v;
    v.resize(10, 5);
    EXPECT_GE(v.capacity(), 10u);
}

TEST(ArrayListResizeTest, ResizeWithValueLargerWhenCapacityAlreadySufficient) {
    stdx::array_list<int> v;
    v.reserve(10);
    v.push_back(1);
    std::size_t capBefore = v.capacity();
    v.resize(4, 7);
    EXPECT_EQ(4u, v.size());
    EXPECT_EQ(capBefore, v.capacity());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(7, v[1]);
    EXPECT_EQ(7, v[2]);
    EXPECT_EQ(7, v[3]);
}

TEST(ArrayListResizeTest, ResizeWithValueLargerToExactCapacityDoesNotReallocate) {
    stdx::array_list<int> v;
    v.push_back(1);
    v.push_back(2); // size=2, capacity=2
    v.pop_back();   // size=1, capacity=2
    const int* dataBefore = v.data();
    v.resize(2, 99); // count == capacity: must NOT reallocate
    EXPECT_EQ(dataBefore, v.data());
    EXPECT_EQ(2u, v.size());
    EXPECT_EQ(99, v[1]);
}

TEST(ArrayListResizeTest, ResizeWithValueSmallerDestroysRemovedElements) {
    DestructorCounter::destructions = 0;
    {
        stdx::array_list<DestructorCounter> v;
        v.reserve(3);
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace_back(3);
        DestructorCounter filler{0};
        DestructorCounter::destructions = 0;
        v.resize(1, filler);
        EXPECT_EQ(2, DestructorCounter::destructions);
        EXPECT_EQ(1u, v.size());
        EXPECT_EQ(1, v[0].value);
    }
}

TEST(ArrayListResizeTest, ResizeWithValueWithString) {
    stdx::array_list<std::string> v;
    v.push_back("hello");
    v.resize(4, "world");
    ASSERT_EQ(4u, v.size());
    EXPECT_EQ("hello", v[0]);
    EXPECT_EQ("world", v[1]);
    EXPECT_EQ("world", v[2]);
    EXPECT_EQ("world", v[3]);
}

TEST(ArrayListResizeTest, ResizeWithValueSmallerThenLargerUsesValue) {
    stdx::array_list<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.resize(1, 0);
    v.resize(3, 55);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(10, v[0]);
    EXPECT_EQ(55, v[1]);
    EXPECT_EQ(55, v[2]);
}

// ===== Move Assignment Leak Tests =====

namespace {

/// Stateful allocator for exercising the element-wise move assignment path. Each default-constructed
/// instance gets a unique id, so two default-constructed containers compare unequal and (with POCMA
/// false) move assignment cannot steal the buffer. Tracks outstanding allocations across all
/// instances of an instantiation to expose leaked buffers.
template <typename T, bool POCMA>
struct tracking_allocator {
    using value_type = T;
    using is_always_equal = std::false_type;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::integral_constant<bool, POCMA>;
    using propagate_on_container_swap = std::false_type;

    template <typename U>
    struct rebind {
        using other = tracking_allocator<U, POCMA>;
    };

    /// Allocations not yet deallocated, across all instances of this instantiation
    static int live_allocations;

    int id;

    tracking_allocator() : id(next_id()++) {}
    tracking_allocator(const tracking_allocator&) = default;
    tracking_allocator& operator=(const tracking_allocator&) = default;
    template <typename U>
    tracking_allocator(const tracking_allocator<U, POCMA>& other) : id(other.id) {}

    T* allocate(std::size_t n) {
        ++live_allocations;
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* ptr, std::size_t) noexcept {
        if (ptr != nullptr) {
            --live_allocations;
        }
        ::operator delete(ptr);
    }

    friend bool operator==(const tracking_allocator& lhs, const tracking_allocator& rhs) noexcept {
        return lhs.id == rhs.id;
    }
    friend bool operator!=(const tracking_allocator& lhs, const tracking_allocator& rhs) noexcept {
        return lhs.id != rhs.id;
    }

  private:
    static int& next_id() {
        static int id = 0;
        return id;
    }
};

template <typename T, bool POCMA>
int tracking_allocator<T, POCMA>::live_allocations = 0;

/// Element that bumps a caller-owned counter on destruction. Moved-from objects still count:
/// every construction must be balanced by exactly one destruction.
struct CountedElement {
    int* count;
    explicit CountedElement(int* c) : count(c) {}
    CountedElement(const CountedElement&) = default;
    CountedElement& operator=(const CountedElement&) = default;
    ~CountedElement() {
        if (count)
            ++(*count);
    }
};

} // namespace

TEST(ArrayListMoveAssignmentTest, UnequalAllocatorsDoesNotLeakBuffer) {
    using alloc = tracking_allocator<int, false>;
    using list = stdx::array_list<int, alloc>;
    alloc::live_allocations = 0;
    {
        list src;
        src.push_back(1);
        src.push_back(2);
        src.push_back(3);

        list dst;
        dst.push_back(9);

        // Unequal allocators + POCMA false force the element-wise path
        ASSERT_TRUE(src.get_allocator() != dst.get_allocator());

        dst = std::move(src);
        ASSERT_EQ(3u, dst.size());
        EXPECT_EQ(1, dst[0]);
        EXPECT_EQ(2, dst[1]);
        EXPECT_EQ(3, dst[2]);
        EXPECT_TRUE(src.empty());
    }
    // Every buffer allocated by either container must have been deallocated
    EXPECT_EQ(0, alloc::live_allocations);
}

TEST(ArrayListMoveAssignmentTest, UnequalAllocatorsDestroysSourceElements) {
    using list = stdx::array_list<CountedElement, tracking_allocator<CountedElement, false>>;
    int srcCount = 0;
    int dstCount = 0;
    {
        list src;
        src.reserve(4); // avoid growth reallocs, which destroy elements and would inflate srcCount
        src.emplace_back(&srcCount);
        src.emplace_back(&srcCount);
        src.emplace_back(&srcCount);

        list dst;
        dst.reserve(4); // avoid a realloc during assignment so dstCount stays untouched
        dst.emplace_back(&dstCount);

        ASSERT_TRUE(src.get_allocator() != dst.get_allocator());
        ASSERT_EQ(0, srcCount);
        ASSERT_EQ(0, dstCount);

        dst = std::move(src);
        // src's moved-from elements must be destroyed as part of the move, not leaked
        EXPECT_EQ(3, srcCount);
        // dst's element was assigned over, not destroyed
        EXPECT_EQ(0, dstCount);
        ASSERT_EQ(3u, dst.size());

        // Detach dst's elements so the scope exit doesn't add to srcCount
        for (auto& e : dst) {
            e.count = nullptr;
        }
    }
    EXPECT_EQ(3, srcCount);
    EXPECT_EQ(0, dstCount);
}

TEST(ArrayListMoveAssignmentTest, UnequalAllocatorsDestroysSourceAndExcessDestinationElements) {
    using list = stdx::array_list<CountedElement, tracking_allocator<CountedElement, false>>;
    int srcCount = 0;
    int dstCount = 0;
    {
        list src;
        src.reserve(4); // avoid growth reallocs, which destroy elements and would inflate srcCount
        src.emplace_back(&srcCount);

        list dst;
        dst.reserve(4);
        dst.emplace_back(&dstCount);
        dst.emplace_back(&dstCount);
        dst.emplace_back(&dstCount);

        ASSERT_TRUE(src.get_allocator() != dst.get_allocator());
        ASSERT_EQ(0, srcCount);
        ASSERT_EQ(0, dstCount);

        dst = std::move(src);
        // src's moved-from element must be destroyed as part of the move
        EXPECT_EQ(1, srcCount);
        // dst's two excess elements beyond src's size must be destroyed
        EXPECT_EQ(2, dstCount);
        ASSERT_EQ(1u, dst.size());

        for (auto& e : dst) {
            e.count = nullptr;
        }
    }
    EXPECT_EQ(1, srcCount);
    EXPECT_EQ(2, dstCount);
}

TEST(ArrayListMoveAssignmentTest, EqualAllocatorsStealBufferWithoutLeak) {
    using alloc = tracking_allocator<int, false>;
    using list = stdx::array_list<int, alloc>;
    alloc::live_allocations = 0;
    {
        list src;
        src.push_back(1);
        src.push_back(2);
        src.push_back(3);

        list dst(src); // copy constructor copies src's allocator, so allocators compare equal
        dst.push_back(4);

        ASSERT_TRUE(src.get_allocator() == dst.get_allocator());

        dst = std::move(src);
        ASSERT_EQ(3u, dst.size());
        EXPECT_EQ(1, dst[0]);
        EXPECT_EQ(2, dst[1]);
        EXPECT_EQ(3, dst[2]);
    }
    EXPECT_EQ(0, alloc::live_allocations);
}

TEST(ArrayListMoveAssignmentTest, PocmaPropagatesAllocatorWithoutLeak) {
    using alloc = tracking_allocator<int, true>;
    using list = stdx::array_list<int, alloc>;
    alloc::live_allocations = 0;
    {
        list src;
        src.push_back(1);
        src.push_back(2);

        list dst;
        dst.push_back(9);

        const int SRC_ID = src.get_allocator().id;
        ASSERT_TRUE(src.get_allocator() != dst.get_allocator());

        dst = std::move(src);
        // POCMA true: dst must adopt src's allocator along with its buffer
        EXPECT_EQ(SRC_ID, dst.get_allocator().id);
        ASSERT_EQ(2u, dst.size());
        EXPECT_EQ(1, dst[0]);
        EXPECT_EQ(2, dst[1]);
    }
    EXPECT_EQ(0, alloc::live_allocations);
}
