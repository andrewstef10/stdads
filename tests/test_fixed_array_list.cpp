#include <gtest/gtest.h>
#include <stdx/fixed_array_list.h>

#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// ===== Test Helpers =====

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

struct DestructorCounter {
    int* count;
    explicit DestructorCounter(int* c) : count(c) {}
    DestructorCounter(const DestructorCounter&) = default;
    DestructorCounter& operator=(const DestructorCounter&) = default;
    ~DestructorCounter() {
        if (count)
            ++(*count);
    }
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

// ===== Constructor / Assignment Tests =====

TEST(FixedArrayListConstructorTest, DefaultConstructorIsEmpty) {
    stdx::fixed_array_list<int, 5> v;
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(FixedArrayListConstructorTest, DefaultConstructorCapacityIsN) {
    stdx::fixed_array_list<int, 5> v;
    EXPECT_EQ(5u, v.capacity());
    EXPECT_EQ(5u, v.max_size());
}

TEST(FixedArrayListConstructorTest, CopyConstructorFromEmpty) {
    stdx::fixed_array_list<int, 4> src;
    stdx::fixed_array_list<int, 4> dst(src);
    EXPECT_EQ(0u, dst.size());
    EXPECT_TRUE(dst.empty());
}

TEST(FixedArrayListConstructorTest, CopyConstructorFromNonEmpty) {
    stdx::fixed_array_list<int, 4> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    stdx::fixed_array_list<int, 4> dst(src);
    ASSERT_EQ(3u, dst.size());
    EXPECT_EQ(1, dst[0]);
    EXPECT_EQ(2, dst[1]);
    EXPECT_EQ(3, dst[2]);
}

TEST(FixedArrayListConstructorTest, CopyConstructorIsDeepCopy) {
    stdx::fixed_array_list<int, 4> src;
    src.push_back(10);
    src.push_back(20);

    stdx::fixed_array_list<int, 4> dst(src);
    dst[0] = 99;

    EXPECT_EQ(10, src[0]);
    EXPECT_EQ(99, dst[0]);
}

TEST(FixedArrayListConstructorTest, CopyConstructorFromFull) {
    stdx::fixed_array_list<int, 3> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    stdx::fixed_array_list<int, 3> dst(src);
    ASSERT_EQ(3u, dst.size());
    EXPECT_EQ(1, dst[0]);
    EXPECT_EQ(2, dst[1]);
    EXPECT_EQ(3, dst[2]);
}

TEST(FixedArrayListConstructorTest, MoveConstructorFromEmpty) {
    stdx::fixed_array_list<int, 4> src;
    stdx::fixed_array_list<int, 4> dst(std::move(src));
    EXPECT_EQ(0u, dst.size());
    EXPECT_TRUE(dst.empty());
}

TEST(FixedArrayListConstructorTest, MoveConstructorFromNonEmpty) {
    stdx::fixed_array_list<int, 4> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    stdx::fixed_array_list<int, 4> dst(std::move(src));
    ASSERT_EQ(3u, dst.size());
    EXPECT_EQ(1, dst[0]);
    EXPECT_EQ(2, dst[1]);
    EXPECT_EQ(3, dst[2]);
}

TEST(FixedArrayListConstructorTest, MoveConstructorLeavesSourceEmpty) {
    stdx::fixed_array_list<int, 4> src;
    src.push_back(1);
    src.push_back(2);

    stdx::fixed_array_list<int, 4> dst(std::move(src));
    EXPECT_TRUE(src.empty());
    EXPECT_EQ(0u, src.size());
}

TEST(FixedArrayListConstructorTest, MoveConstructorWithMoveOnlyType) {
    stdx::fixed_array_list<MoveOnly, 3> src;
    src.emplace_back(42);
    src.emplace_back(99);

    stdx::fixed_array_list<MoveOnly, 3> dst(std::move(src));
    ASSERT_EQ(2u, dst.size());
    EXPECT_EQ(42, dst[0].value);
    EXPECT_EQ(99, dst[1].value);
}

TEST(FixedArrayListConstructorTest, CopyAssignmentFromEmpty) {
    stdx::fixed_array_list<int, 4> src;
    stdx::fixed_array_list<int, 4> dst;
    dst = src;
    EXPECT_EQ(0u, dst.size());
    EXPECT_TRUE(dst.empty());
}

TEST(FixedArrayListConstructorTest, CopyAssignmentFromNonEmpty) {
    stdx::fixed_array_list<int, 4> src;
    src.push_back(10);
    src.push_back(20);

    stdx::fixed_array_list<int, 4> dst;
    dst = src;
    ASSERT_EQ(2u, dst.size());
    EXPECT_EQ(10, dst[0]);
    EXPECT_EQ(20, dst[1]);
}

TEST(FixedArrayListConstructorTest, CopyAssignmentOverwritesExisting) {
    stdx::fixed_array_list<int, 5> src;
    src.push_back(10);
    src.push_back(20);

    stdx::fixed_array_list<int, 5> dst;
    dst.push_back(99);
    dst.push_back(88);
    dst.push_back(77);

    dst = src;
    ASSERT_EQ(2u, dst.size());
    EXPECT_EQ(10, dst[0]);
    EXPECT_EQ(20, dst[1]);
}

TEST(FixedArrayListConstructorTest, CopyAssignmentIsDeepCopy) {
    stdx::fixed_array_list<int, 4> src;
    src.push_back(5);

    stdx::fixed_array_list<int, 4> dst;
    dst = src;
    dst[0] = 42;

    EXPECT_EQ(5, src[0]);
    EXPECT_EQ(42, dst[0]);
}

TEST(FixedArrayListConstructorTest, CopyAssignmentSelfAssignment) {
    stdx::fixed_array_list<int, 4> v;
    v.push_back(1);
    v.push_back(2);
    stdx::fixed_array_list<int, 4> self = v;
    v = self;
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
}

TEST(FixedArrayListConstructorTest, MoveAssignmentFromEmpty) {
    stdx::fixed_array_list<int, 4> src;
    stdx::fixed_array_list<int, 4> dst;
    dst = std::move(src);
    EXPECT_EQ(0u, dst.size());
    EXPECT_TRUE(dst.empty());
}

TEST(FixedArrayListConstructorTest, MoveAssignmentFromNonEmpty) {
    stdx::fixed_array_list<int, 4> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    stdx::fixed_array_list<int, 4> dst;
    dst = std::move(src);
    ASSERT_EQ(3u, dst.size());
    EXPECT_EQ(1, dst[0]);
    EXPECT_EQ(2, dst[1]);
    EXPECT_EQ(3, dst[2]);
}

TEST(FixedArrayListConstructorTest, MoveAssignmentLeavesSourceEmpty) {
    stdx::fixed_array_list<int, 4> src;
    src.push_back(1);
    src.push_back(2);

    stdx::fixed_array_list<int, 4> dst;
    dst = std::move(src);

    EXPECT_TRUE(src.empty());
    EXPECT_EQ(0u, src.size());
}

TEST(FixedArrayListConstructorTest, MoveAssignmentOverwritesExisting) {
    stdx::fixed_array_list<int, 4> src;
    src.push_back(7);

    stdx::fixed_array_list<int, 4> dst;
    dst.push_back(1);
    dst.push_back(2);
    dst = std::move(src);

    ASSERT_EQ(1u, dst.size());
    EXPECT_EQ(7, dst[0]);
}

TEST(FixedArrayListConstructorTest, DestructorCallsElementDestructors) {
    int count = 0;
    {
        stdx::fixed_array_list<DestructorCounter, 3> v;
        v.emplace_back(&count);
        v.emplace_back(&count);
        EXPECT_EQ(0, count);
    }
    EXPECT_EQ(2, count);
}

TEST(FixedArrayListConstructorTest, MoveConstructorDestroysSourceElements) {
    int srcCount = 0;
    int dstCount = 0;
    {
        stdx::fixed_array_list<DestructorCounter, 3> src;
        src.emplace_back(&srcCount);
        src.emplace_back(&srcCount);

        stdx::fixed_array_list<DestructorCounter, 3> dst(std::move(src));
        // src's moved-from elements must be destroyed as part of the move, not leaked
        EXPECT_EQ(2, srcCount);

        // Repoint dst's elements so their destructors count separately at scope exit
        for (auto& e : dst) {
            e.count = &dstCount;
        }
    }
    EXPECT_EQ(2, dstCount);
}

TEST(FixedArrayListConstructorTest, MoveAssignmentDestroysSourceElements) {
    int srcCount = 0;
    {
        stdx::fixed_array_list<DestructorCounter, 3> src;
        src.emplace_back(&srcCount);
        src.emplace_back(&srcCount);

        stdx::fixed_array_list<DestructorCounter, 3> dst;
        dst = std::move(src);
        // src's moved-from elements must be destroyed as part of the move, not leaked
        EXPECT_EQ(2, srcCount);

        // Detach dst's elements so the outer scope exit doesn't add to srcCount
        for (auto& e : dst) {
            e.count = nullptr;
        }
    }
    EXPECT_EQ(2, srcCount);
}

// ===== Element Access Tests =====

TEST(FixedArrayListElementAccessTest, AtValidIndex) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    EXPECT_EQ(10, v.at(0));
    EXPECT_EQ(20, v.at(1));
    EXPECT_EQ(30, v.at(2));
}

TEST(FixedArrayListElementAccessTest, AtConstValidIndex) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(10);
    v.push_back(20);
    const stdx::fixed_array_list<int, 5>& cv = v;
    EXPECT_EQ(10, cv.at(0));
    EXPECT_EQ(20, cv.at(1));
}

TEST(FixedArrayListElementAccessTest, AtReturnsReference) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(5);
    v.at(0) = 99;
    EXPECT_EQ(99, v.at(0));
}

TEST(FixedArrayListElementAccessTest, AtBoundaryLastElement) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(3, v.at(2));
    EXPECT_THROW(v.at(3), std::out_of_range);
}

TEST(FixedArrayListElementAccessTest, AtOutOfRange) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    EXPECT_THROW(v.at(1), std::out_of_range);
    EXPECT_THROW(v.at(100), std::out_of_range);
}

TEST(FixedArrayListElementAccessTest, AtConstOutOfRange) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    const stdx::fixed_array_list<int, 5>& cv = v;
    EXPECT_THROW(cv.at(1), std::out_of_range);
    EXPECT_THROW(cv.at(99), std::out_of_range);
}

TEST(FixedArrayListElementAccessTest, AtOnEmptyContainerThrows) {
    stdx::fixed_array_list<int, 5> v;
    EXPECT_THROW(v.at(0), std::out_of_range);
}

TEST(FixedArrayListElementAccessTest, SubscriptOperator) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(FixedArrayListElementAccessTest, SubscriptOperatorConst) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(10);
    v.push_back(20);
    const stdx::fixed_array_list<int, 5>& cv = v;
    EXPECT_EQ(10, cv[0]);
    EXPECT_EQ(20, cv[1]);
}

TEST(FixedArrayListElementAccessTest, SubscriptOperatorReturnsReference) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(5);
    v[0] = 42;
    EXPECT_EQ(42, v[0]);
}

TEST(FixedArrayListElementAccessTest, DataPointerNonNullAfterPushBack) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    EXPECT_NE(nullptr, v.data());
}

TEST(FixedArrayListElementAccessTest, DataPointerConsistentWithSubscript) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(10);
    v.push_back(20);
    EXPECT_EQ(&v[0], v.data());
    EXPECT_EQ(10, v.data()[0]);
    EXPECT_EQ(20, v.data()[1]);
}

TEST(FixedArrayListElementAccessTest, DataPointerConst) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(5);
    const stdx::fixed_array_list<int, 5>& cv = v;
    EXPECT_NE(nullptr, cv.data());
    EXPECT_EQ(5, *cv.data());
}

TEST(FixedArrayListElementAccessTest, FrontElement) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(1, v.front());
}

TEST(FixedArrayListElementAccessTest, FrontElementConst) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(42);
    v.push_back(99);
    const stdx::fixed_array_list<int, 5>& cv = v;
    EXPECT_EQ(42, cv.front());
}

TEST(FixedArrayListElementAccessTest, FrontReturnsReference) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(5);
    v.front() = 99;
    EXPECT_EQ(99, v.front());
}

TEST(FixedArrayListElementAccessTest, BackElement) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(3, v.back());
}

TEST(FixedArrayListElementAccessTest, BackElementConst) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(100);
    const stdx::fixed_array_list<int, 5>& cv = v;
    EXPECT_EQ(100, cv.back());
}

TEST(FixedArrayListElementAccessTest, BackReturnsReference) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(5);
    v.back() = 77;
    EXPECT_EQ(77, v.back());
}

TEST(FixedArrayListElementAccessTest, FrontAndBackSameWhenSizeOne) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(7);
    EXPECT_EQ(v.front(), v.back());
    EXPECT_EQ(&v.front(), &v.back());
}

TEST(FixedArrayListElementAccessTest, FrontAndBackDistinctWhenSizeTwo) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    EXPECT_EQ(1, v.front());
    EXPECT_EQ(2, v.back());
    EXPECT_NE(&v.front(), &v.back());
}

// ===== Capacity Tests =====

TEST(FixedArrayListCapacityTest, EmptyOnDefault) {
    stdx::fixed_array_list<int, 5> v;
    EXPECT_TRUE(v.empty());
}

TEST(FixedArrayListCapacityTest, NotEmptyAfterPushBack) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    EXPECT_FALSE(v.empty());
}

TEST(FixedArrayListCapacityTest, EmptyAfterClear) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.clear();
    EXPECT_TRUE(v.empty());
}

TEST(FixedArrayListCapacityTest, SizeDefault) {
    stdx::fixed_array_list<int, 5> v;
    EXPECT_EQ(0u, v.size());
}

TEST(FixedArrayListCapacityTest, SizeAfterPushBack) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    EXPECT_EQ(1u, v.size());
    v.push_back(2);
    EXPECT_EQ(2u, v.size());
    v.push_back(3);
    EXPECT_EQ(3u, v.size());
}

TEST(FixedArrayListCapacityTest, SizeAfterPopBack) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.pop_back();
    EXPECT_EQ(1u, v.size());
    v.pop_back();
    EXPECT_EQ(0u, v.size());
}

TEST(FixedArrayListCapacityTest, CapacityAlwaysN) {
    stdx::fixed_array_list<int, 5> v;
    EXPECT_EQ(5u, v.capacity());
    v.push_back(1);
    EXPECT_EQ(5u, v.capacity());
    v.push_back(2);
    EXPECT_EQ(5u, v.capacity());
}

TEST(FixedArrayListCapacityTest, CapacityUnchangedAfterClear) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.clear();
    EXPECT_EQ(5u, v.capacity());
}

TEST(FixedArrayListCapacityTest, MaxSizeAlwaysN) {
    stdx::fixed_array_list<int, 7> v;
    EXPECT_EQ(7u, v.max_size());
    v.push_back(1);
    EXPECT_EQ(7u, v.max_size());
}

TEST(FixedArrayListCapacityTest, MaxSizeEqualsCapacity) {
    stdx::fixed_array_list<int, 4> v;
    EXPECT_EQ(v.max_size(), v.capacity());
}

TEST(FixedArrayListCapacityTest, DifferentNValues) {
    stdx::fixed_array_list<int, 1> v1;
    stdx::fixed_array_list<int, 10> v10;
    stdx::fixed_array_list<int, 100> v100;
    EXPECT_EQ(1u, v1.capacity());
    EXPECT_EQ(10u, v10.capacity());
    EXPECT_EQ(100u, v100.capacity());
}

// ===== Modifier Tests =====

TEST(FixedArrayListModifierTest, PushBackCopyIncreasesSize) {
    stdx::fixed_array_list<int, 5> v;
    int val = 42;
    v.push_back(val);
    EXPECT_EQ(1u, v.size());
    EXPECT_EQ(42, v[0]);
    EXPECT_EQ(42, val);
}

TEST(FixedArrayListModifierTest, PushBackCopyMultiple) {
    stdx::fixed_array_list<int, 5> v;
    for (int i = 0; i < 5; ++i) {
        v.push_back(i);
    }
    ASSERT_EQ(5u, v.size());
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(i, v[i]);
    }
}

TEST(FixedArrayListModifierTest, PushBackMove) {
    stdx::fixed_array_list<std::string, 5> v;
    std::string s = "hello";
    v.push_back(std::move(s));
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ("hello", v[0]);
}

TEST(FixedArrayListModifierTest, PushBackMoveSourceEmptied) {
    stdx::fixed_array_list<std::string, 5> v;
    std::string s = "hello";
    v.push_back(std::move(s));
    EXPECT_TRUE(s.empty());
}

TEST(FixedArrayListModifierTest, PushBackCopyAtCapacityThrows) {
    stdx::fixed_array_list<int, 3> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    int val = 4;
    EXPECT_THROW(v.push_back(val), std::length_error);
}

TEST(FixedArrayListModifierTest, PushBackMoveAtCapacityThrows) {
    stdx::fixed_array_list<std::string, 2> v;
    v.push_back(std::string("a"));
    v.push_back(std::string("b"));
    EXPECT_THROW(v.push_back(std::string("c")), std::length_error);
}

TEST(FixedArrayListModifierTest, PushBackThrowDoesNotChangeSize) {
    stdx::fixed_array_list<int, 2> v;
    v.push_back(1);
    v.push_back(2);
    int val = 3;
    EXPECT_THROW(v.push_back(val), std::length_error);
    EXPECT_EQ(2u, v.size());
}

TEST(FixedArrayListModifierTest, PopBackReducesSize) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.pop_back();
    EXPECT_EQ(1u, v.size());
}

TEST(FixedArrayListModifierTest, PopBackRemovesLastElement) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.pop_back();
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
}

TEST(FixedArrayListModifierTest, PopBackToEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(42);
    v.pop_back();
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(FixedArrayListModifierTest, PopBackCallsDestructor) {
    int count = 0;
    stdx::fixed_array_list<DestructorCounter, 3> v;
    v.emplace_back(&count);
    v.emplace_back(&count);
    EXPECT_EQ(0, count);
    v.pop_back();
    EXPECT_EQ(1, count);
    v.pop_back();
    EXPECT_EQ(2, count);
}

TEST(FixedArrayListModifierTest, PopBackThenPushBack) {
    stdx::fixed_array_list<int, 3> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.pop_back();
    v.push_back(99);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(99, v[2]);
}

TEST(FixedArrayListModifierTest, EmplaceBackIntoEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.emplace_back(42);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(42, v[0]);
}

TEST(FixedArrayListModifierTest, EmplaceBackIncreasesSize) {
    stdx::fixed_array_list<int, 5> v;
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(FixedArrayListModifierTest, EmplaceBackWithMultipleArgs) {
    stdx::fixed_array_list<Point, 5> v;
    v.emplace_back(3, 7);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(3, v[0].x);
    EXPECT_EQ(7, v[0].y);
}

TEST(FixedArrayListModifierTest, EmplaceBackConstructsInPlace) {
    stdx::fixed_array_list<MoveOnly, 3> v;
    v.emplace_back(99);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(99, v[0].value);
}

TEST(FixedArrayListModifierTest, EmplaceBackNoCopyOrMove) {
    ConstructionCounter::direct_constructions = 0;
    ConstructionCounter::copy_constructions = 0;
    ConstructionCounter::move_constructions = 0;

    stdx::fixed_array_list<ConstructionCounter, 5> v;
    v.emplace_back(5);

    EXPECT_EQ(1, ConstructionCounter::direct_constructions);
    EXPECT_EQ(0, ConstructionCounter::copy_constructions);
    EXPECT_EQ(0, ConstructionCounter::move_constructions);
}

TEST(FixedArrayListModifierTest, EmplaceBackAtCapacityThrows) {
    stdx::fixed_array_list<int, 2> v;
    v.emplace_back(1);
    v.emplace_back(2);
    EXPECT_THROW(v.emplace_back(3), std::length_error);
}

TEST(FixedArrayListModifierTest, EmplaceBackWithStringArgs) {
    stdx::fixed_array_list<std::string, 3> v;
    v.emplace_back(3, 'z');
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ("zzz", v[0]);
}

TEST(FixedArrayListModifierTest, InsertAtBeginCopy) {
    stdx::fixed_array_list<int, 5> v;
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

TEST(FixedArrayListModifierTest, InsertAtEndCopy) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    auto it = v.insert(v.end(), 3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(3, *it);
}

TEST(FixedArrayListModifierTest, InsertAtMiddleCopy) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(3);
    auto it = v.insert(v.begin() + 1, 2);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(2, *it);
}

TEST(FixedArrayListModifierTest, InsertIntoEmptyCopy) {
    stdx::fixed_array_list<int, 5> v;
    auto it = v.insert(v.end(), 42);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(42, v[0]);
    EXPECT_EQ(42, *it);
}

TEST(FixedArrayListModifierTest, InsertAtBeginMove) {
    stdx::fixed_array_list<std::string, 5> v;
    v.push_back("world");
    std::string s = "hello";
    auto it = v.insert(v.begin(), std::move(s));
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ("hello", v[0]);
    EXPECT_EQ("world", v[1]);
    EXPECT_EQ("hello", *it);
}

TEST(FixedArrayListModifierTest, InsertAtEndMove) {
    stdx::fixed_array_list<std::string, 5> v;
    v.push_back("hello");
    std::string s = "world";
    auto it = v.insert(v.end(), std::move(s));
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ("hello", v[0]);
    EXPECT_EQ("world", v[1]);
    EXPECT_EQ("world", *it);
}

TEST(FixedArrayListModifierTest, InsertAtMiddleMove) {
    stdx::fixed_array_list<std::string, 5> v;
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

TEST(FixedArrayListModifierTest, InsertCopyAtCapacityThrows) {
    stdx::fixed_array_list<int, 3> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    int val = 4;
    EXPECT_THROW(v.insert(v.begin(), val), std::length_error);
}

TEST(FixedArrayListModifierTest, InsertMoveAtCapacityThrows) {
    stdx::fixed_array_list<std::string, 2> v;
    v.push_back("a");
    v.push_back("b");
    EXPECT_THROW(v.insert(v.begin(), std::string("c")), std::length_error);
}

TEST(FixedArrayListModifierTest, InsertSelfReferenceWithSpareCapacity) {
    stdx::fixed_array_list<int, 5> v;
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

TEST(FixedArrayListModifierTest, EmplaceIntoEmpty) {
    stdx::fixed_array_list<int, 5> v;
    auto it = v.emplace(v.end(), 42);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(42, v[0]);
    EXPECT_EQ(42, *it);
}

TEST(FixedArrayListModifierTest, EmplaceAtBegin) {
    stdx::fixed_array_list<int, 5> v;
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

TEST(FixedArrayListModifierTest, EmplaceAtEnd) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    auto it = v.emplace(v.end(), 3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(3, *it);
}

TEST(FixedArrayListModifierTest, EmplaceAtMiddle) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(3);
    auto it = v.emplace(v.begin() + 1, 2);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    EXPECT_EQ(2, *it);
}

TEST(FixedArrayListModifierTest, EmplaceWithMultipleArgs) {
    stdx::fixed_array_list<Point, 5> v;
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

TEST(FixedArrayListModifierTest, EmplaceAtCapacityThrows) {
    stdx::fixed_array_list<int, 2> v;
    v.emplace_back(1);
    v.emplace_back(2);
    EXPECT_THROW(v.emplace(v.begin(), 3), std::length_error);
}

TEST(FixedArrayListModifierTest, EraseFirst) {
    stdx::fixed_array_list<int, 5> v;
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

TEST(FixedArrayListModifierTest, EraseLast) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    auto it = v.erase(v.end() - 1);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(v.end(), it);
}

TEST(FixedArrayListModifierTest, EraseMiddle) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    auto it = v.erase(v.begin() + 1);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(3, v[1]);
    EXPECT_EQ(3, *it);
}

TEST(FixedArrayListModifierTest, EraseOnlyElement) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(42);
    auto it = v.erase(v.begin());
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.end(), it);
}

TEST(FixedArrayListModifierTest, EraseCallsDestructor) {
    int count = 0;
    stdx::fixed_array_list<DestructorCounter, 3> v;
    v.emplace_back(&count);
    v.emplace_back(&count);
    v.emplace_back(&count);
    v.erase(v.begin());
    EXPECT_EQ(1, count);
}

TEST(FixedArrayListModifierTest, EraseRangeEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    auto it = v.erase(v.begin() + 1, v.begin() + 1);
    EXPECT_EQ(2u, v.size());
    EXPECT_EQ(v.begin() + 1, it);
}

TEST(FixedArrayListModifierTest, EraseRangeAll) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    auto it = v.erase(v.begin(), v.end());
    EXPECT_EQ(0u, v.size());
    EXPECT_EQ(v.end(), it);
}

TEST(FixedArrayListModifierTest, EraseRangePartial) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.push_back(5);
    auto it = v.erase(v.begin() + 1, v.begin() + 3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(4, v[1]);
    EXPECT_EQ(5, v[2]);
    EXPECT_EQ(4, *it);
}

TEST(FixedArrayListModifierTest, EraseRangeFromBegin) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    auto it = v.erase(v.begin(), v.begin() + 2);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(3, v[0]);
    EXPECT_EQ(4, v[1]);
    EXPECT_EQ(v.begin(), it);
    EXPECT_EQ(3, *it);
}

TEST(FixedArrayListModifierTest, EraseRangeToEnd) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    auto it = v.erase(v.begin() + 2, v.end());
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(v.end(), it);
}

TEST(FixedArrayListModifierTest, EraseRangeAllowsSubsequentInsert) {
    stdx::fixed_array_list<int, 3> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.erase(v.begin(), v.end());
    EXPECT_TRUE(v.empty());
    v.push_back(99);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(99, v[0]);
}

TEST(FixedArrayListModifierTest, ClearEmptyContainer) {
    stdx::fixed_array_list<int, 5> v;
    v.clear();
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(FixedArrayListModifierTest, ClearNonEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.clear();
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(FixedArrayListModifierTest, ClearPreservesCapacity) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.clear();
    EXPECT_EQ(5u, v.capacity());
}

TEST(FixedArrayListModifierTest, ClearCallsDestructors) {
    int count = 0;
    stdx::fixed_array_list<DestructorCounter, 3> v;
    v.emplace_back(&count);
    v.emplace_back(&count);
    v.emplace_back(&count);
    EXPECT_EQ(0, count);
    v.clear();
    EXPECT_EQ(3, count);
}

TEST(FixedArrayListModifierTest, ClearThenPushBack) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.clear();
    v.push_back(99);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ(99, v[0]);
}

TEST(FixedArrayListModifierTest, ClearAllowsRefillToCapacity) {
    stdx::fixed_array_list<int, 3> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.clear();
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(10, v[0]);
    EXPECT_EQ(20, v[1]);
    EXPECT_EQ(30, v[2]);
}

TEST(FixedArrayListModifierTest, SwapTwoNonEmpty) {
    stdx::fixed_array_list<int, 5> a;
    a.push_back(1);
    a.push_back(2);

    stdx::fixed_array_list<int, 5> b;
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

TEST(FixedArrayListModifierTest, SwapWithEmpty) {
    stdx::fixed_array_list<int, 5> a;
    a.push_back(1);
    a.push_back(2);

    stdx::fixed_array_list<int, 5> b;

    a.swap(b);

    EXPECT_TRUE(a.empty());
    ASSERT_EQ(2u, b.size());
    EXPECT_EQ(1, b[0]);
    EXPECT_EQ(2, b[1]);
}

TEST(FixedArrayListModifierTest, SwapTwoEmptyContainers) {
    stdx::fixed_array_list<int, 5> a;
    stdx::fixed_array_list<int, 5> b;
    a.swap(b);
    EXPECT_TRUE(a.empty());
    EXPECT_TRUE(b.empty());
}

TEST(FixedArrayListModifierTest, SwapBothContainersUsableAfterSwap) {
    stdx::fixed_array_list<int, 5> a, b;
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

TEST(FixedArrayListModifierTest, SwapMoveOnlyType) {
    stdx::fixed_array_list<MoveOnly, 3> a;
    a.emplace_back(1);
    a.emplace_back(2);

    stdx::fixed_array_list<MoveOnly, 3> b;
    b.emplace_back(10);

    a.swap(b);

    ASSERT_EQ(1u, a.size());
    EXPECT_EQ(10, a[0].value);
    ASSERT_EQ(2u, b.size());
    EXPECT_EQ(1, b[0].value);
    EXPECT_EQ(2, b[1].value);
}

// ===== Iterator Tests =====

TEST(FixedArrayListIteratorTest, BeginEqualsEndWhenEmpty) {
    stdx::fixed_array_list<int, 5> v;
    EXPECT_EQ(v.begin(), v.end());
}

TEST(FixedArrayListIteratorTest, BeginNotEqualsEndWhenNonEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    EXPECT_NE(v.begin(), v.end());
}

TEST(FixedArrayListIteratorTest, BeginPointsToFirstElement) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(42);
    v.push_back(7);
    EXPECT_EQ(42, *v.begin());
}

TEST(FixedArrayListIteratorTest, EndIsOnePastLast) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    EXPECT_EQ(2, *(v.end() - 1));
}

TEST(FixedArrayListIteratorTest, ForwardIteration) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    int expected = 1;
    for (auto it = v.begin(); it != v.end(); ++it) {
        EXPECT_EQ(expected++, *it);
    }
    EXPECT_EQ(4, expected);
}

TEST(FixedArrayListIteratorTest, RangeBasedForLoop) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    int sum = 0;
    for (int x : v) {
        sum += x;
    }
    EXPECT_EQ(60, sum);
}

TEST(FixedArrayListIteratorTest, ConstBeginEnd) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(5);
    const stdx::fixed_array_list<int, 5>& cv = v;
    EXPECT_NE(cv.begin(), cv.end());
    EXPECT_EQ(5, *cv.begin());
}

TEST(FixedArrayListIteratorTest, ConstBeginEqualsEndWhenEmpty) {
    const stdx::fixed_array_list<int, 5> v;
    EXPECT_EQ(v.begin(), v.end());
}

TEST(FixedArrayListIteratorTest, CBeginCEnd) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(3);
    v.push_back(4);
    EXPECT_EQ(3, *v.cbegin());
    EXPECT_NE(v.cbegin(), v.cend());
}

TEST(FixedArrayListIteratorTest, CBeginEqualsCBeginConst) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    const stdx::fixed_array_list<int, 5>& cv = v;
    EXPECT_EQ(v.cbegin(), cv.begin());
    EXPECT_EQ(v.cend(), cv.end());
}

TEST(FixedArrayListIteratorTest, ReverseIterationEmpty) {
    stdx::fixed_array_list<int, 5> v;
    EXPECT_EQ(v.rbegin(), v.rend());
}

TEST(FixedArrayListIteratorTest, ReverseIteration) {
    stdx::fixed_array_list<int, 5> v;
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

TEST(FixedArrayListIteratorTest, ConstReverseIteration) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(10);
    v.push_back(20);
    const stdx::fixed_array_list<int, 5>& cv = v;

    auto it = cv.rbegin();
    EXPECT_EQ(20, *it);
    ++it;
    EXPECT_EQ(10, *it);
    ++it;
    EXPECT_EQ(cv.rend(), it);
}

TEST(FixedArrayListIteratorTest, CRBeginCREnd) {
    stdx::fixed_array_list<int, 5> v;
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

TEST(FixedArrayListIteratorTest, MutableIteratorAllowsWrite) {
    stdx::fixed_array_list<int, 5> v;
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

// ===== Comparison Tests =====

TEST(FixedArrayListComparisonTest, EqualsTwoEmpty) {
    stdx::fixed_array_list<int, 5> a, b;
    EXPECT_TRUE(a.equals(b));
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(FixedArrayListComparisonTest, EqualsSameElements) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    EXPECT_TRUE(a.equals(b));
    EXPECT_TRUE(a == b);
}

TEST(FixedArrayListComparisonTest, EqualsNotEqualElements) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(1);
    b.push_back(3);
    EXPECT_FALSE(a.equals(b));
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(FixedArrayListComparisonTest, EqualsNotEqualSizesASmaller) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    EXPECT_FALSE(a.equals(b));
    EXPECT_FALSE(a == b);
}

TEST(FixedArrayListComparisonTest, EqualsNotEqualSizesALarger) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    b.push_back(1);
    b.push_back(2);
    EXPECT_FALSE(a.equals(b));
    EXPECT_FALSE(a == b);
}

TEST(FixedArrayListComparisonTest, EqualsEmptyVsNonEmpty) {
    stdx::fixed_array_list<int, 5> a, b;
    b.push_back(1);
    EXPECT_FALSE(a.equals(b));
    EXPECT_FALSE(b.equals(a));
}

TEST(FixedArrayListComparisonTest, LessThanLexicographic) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    b.push_back(1);
    b.push_back(2);
    b.push_back(4);
    EXPECT_TRUE(a.less_than(b));
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(FixedArrayListComparisonTest, LessThanBySize) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    EXPECT_TRUE(a.less_than(b));
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(FixedArrayListComparisonTest, LessThanEqualIsNotLess) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(1);
    b.push_back(2);
    EXPECT_FALSE(a.less_than(b));
    EXPECT_FALSE(a < b);
}

TEST(FixedArrayListComparisonTest, LessThanBothEmpty) {
    stdx::fixed_array_list<int, 5> a, b;
    EXPECT_FALSE(a.less_than(b));
    EXPECT_FALSE(a < b);
}

TEST(FixedArrayListComparisonTest, LessThanFirstElementDecides) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    b.push_back(2);
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(FixedArrayListComparisonTest, GreaterThan) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(5);
    b.push_back(3);
    EXPECT_TRUE(a > b);
    EXPECT_FALSE(b > a);
}

TEST(FixedArrayListComparisonTest, GreaterThanEqualNotGreater) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    b.push_back(1);
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(b > a);
}

TEST(FixedArrayListComparisonTest, LessOrEqualWhenEqual) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    b.push_back(1);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b <= a);
}

TEST(FixedArrayListComparisonTest, LessOrEqualWhenLess) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    b.push_back(2);
    EXPECT_TRUE(a <= b);
    EXPECT_FALSE(b <= a);
}

TEST(FixedArrayListComparisonTest, GreaterOrEqualWhenEqual) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(1);
    b.push_back(1);
    EXPECT_TRUE(a >= b);
    EXPECT_TRUE(b >= a);
}

TEST(FixedArrayListComparisonTest, GreaterOrEqualWhenGreater) {
    stdx::fixed_array_list<int, 5> a, b;
    a.push_back(3);
    b.push_back(2);
    EXPECT_TRUE(a >= b);
    EXPECT_FALSE(b >= a);
}

TEST(FixedArrayListComparisonTest, AllOperatorsEmptyContainers) {
    stdx::fixed_array_list<int, 5> a, b;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
}

// ===== Member Types Tests =====

TEST(FixedArrayListMemberTypesTest, SizeType) {
    static_assert(std::is_same<stdx::fixed_array_list<int, 5>::size_type, std::size_t>::value, "");
}

TEST(FixedArrayListMemberTypesTest, Reference) {
    static_assert(std::is_same<stdx::fixed_array_list<int, 5>::reference, int&>::value, "");
    static_assert(std::is_same<stdx::fixed_array_list<double, 5>::reference, double&>::value, "");
}

TEST(FixedArrayListMemberTypesTest, ConstReference) {
    static_assert(std::is_same<stdx::fixed_array_list<int, 5>::const_reference, const int&>::value, "");
}

TEST(FixedArrayListMemberTypesTest, ReferenceIsActuallyMutable) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    stdx::fixed_array_list<int, 5>::reference ref = v[0];
    ref = 42;
    EXPECT_EQ(42, v[0]);
}

TEST(FixedArrayListMemberTypesTest, ConstReferenceFromConstContainer) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(7);
    const stdx::fixed_array_list<int, 5>& cv = v;
    stdx::fixed_array_list<int, 5>::const_reference cref = cv[0];
    EXPECT_EQ(7, cref);
}

// ===== Non-Default-Constructible Type Tests =====

TEST(FixedArrayListNonDefaultConstructibleTest, EmplaceBackPoint) {
    stdx::fixed_array_list<Point, 5> v;
    v.emplace_back(1, 2);
    v.emplace_back(3, 4);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0].x);
    EXPECT_EQ(2, v[0].y);
    EXPECT_EQ(3, v[1].x);
    EXPECT_EQ(4, v[1].y);
}

TEST(FixedArrayListNonDefaultConstructibleTest, MoveOnlyPushBack) {
    stdx::fixed_array_list<MoveOnly, 3> v;
    v.push_back(MoveOnly(1));
    v.push_back(MoveOnly(2));
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0].value);
    EXPECT_EQ(2, v[1].value);
}

TEST(FixedArrayListNonDefaultConstructibleTest, MoveOnlyCopyConstructorIsDeleted) {
    static_assert(!std::is_copy_constructible<MoveOnly>::value, "");
}

// ===== Resize Tests =====

namespace {

struct ResizeDestructorCounter {
    static int destructions;
    int value;
    ResizeDestructorCounter() : value(0) {}
    explicit ResizeDestructorCounter(int v) : value(v) {}
    ~ResizeDestructorCounter() { ++destructions; }
    ResizeDestructorCounter(const ResizeDestructorCounter&) = default;
    ResizeDestructorCounter& operator=(const ResizeDestructorCounter&) = default;
    ResizeDestructorCounter(ResizeDestructorCounter&&) noexcept = default;
    ResizeDestructorCounter& operator=(ResizeDestructorCounter&&) noexcept = default;
};
int ResizeDestructorCounter::destructions = 0;

} // namespace

// --- resize(count) ---

TEST(FixedArrayListResizeTest, ResizeSameSizeIsNoop) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.resize(3);
    EXPECT_EQ(3u, v.size());
    EXPECT_EQ(5u, v.capacity());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(FixedArrayListResizeTest, ResizeToZeroFromEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.resize(0);
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(FixedArrayListResizeTest, ResizeToZeroFromNonEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.resize(0);
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(FixedArrayListResizeTest, ResizeLargerFromEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.resize(3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(0, v[0]);
    EXPECT_EQ(0, v[1]);
    EXPECT_EQ(0, v[2]);
}

TEST(FixedArrayListResizeTest, ResizeLargerPreservesExistingElements) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(10);
    v.push_back(20);
    v.resize(4);
    ASSERT_EQ(4u, v.size());
    EXPECT_EQ(10, v[0]);
    EXPECT_EQ(20, v[1]);
}

TEST(FixedArrayListResizeTest, ResizeLargerDefaultInitializesNewElements) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(10);
    v.push_back(20);
    v.resize(5);
    ASSERT_EQ(5u, v.size());
    EXPECT_EQ(0, v[2]);
    EXPECT_EQ(0, v[3]);
    EXPECT_EQ(0, v[4]);
}

TEST(FixedArrayListResizeTest, ResizeSmallerReducesSize) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.push_back(5);
    v.resize(2);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
}

TEST(FixedArrayListResizeTest, ResizeSmallerCapacityRemainsN) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.resize(1);
    EXPECT_EQ(1u, v.size());
    EXPECT_EQ(5u, v.capacity());
}

TEST(FixedArrayListResizeTest, ResizeSmallerDestroysRemovedElements) {
    ResizeDestructorCounter::destructions = 0;
    {
        stdx::fixed_array_list<ResizeDestructorCounter, 5> v;
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace_back(3);
        ResizeDestructorCounter::destructions = 0;
        v.resize(1);
        EXPECT_EQ(2, ResizeDestructorCounter::destructions);
        EXPECT_EQ(1u, v.size());
        EXPECT_EQ(1, v[0].value);
    }
}

TEST(FixedArrayListResizeTest, ResizeLargerWithNonTrivialTypeDefaultInserted) {
    stdx::fixed_array_list<std::string, 5> v;
    v.push_back("hello");
    v.resize(3);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ("hello", v[0]);
    EXPECT_EQ("", v[1]);
    EXPECT_EQ("", v[2]);
}

TEST(FixedArrayListResizeTest, ResizeSmallerWithNonTrivialType) {
    stdx::fixed_array_list<std::string, 5> v;
    v.push_back("a");
    v.push_back("b");
    v.push_back("c");
    v.resize(1);
    ASSERT_EQ(1u, v.size());
    EXPECT_EQ("a", v[0]);
}

TEST(FixedArrayListResizeTest, ResizeThenPushBackWorks) {
    stdx::fixed_array_list<int, 5> v;
    v.resize(3);
    v.push_back(99);
    ASSERT_EQ(4u, v.size());
    EXPECT_EQ(0, v[0]);
    EXPECT_EQ(0, v[1]);
    EXPECT_EQ(0, v[2]);
    EXPECT_EQ(99, v[3]);
}

TEST(FixedArrayListResizeTest, ResizeSmallerThenLargerDefaultInitializesNewElements) {
    stdx::fixed_array_list<int, 5> v;
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

TEST(FixedArrayListResizeTest, ResizeToExactCapacitySucceeds) {
    stdx::fixed_array_list<int, 4> v;
    v.push_back(1);
    v.resize(4);
    EXPECT_EQ(4u, v.size());
    EXPECT_EQ(4u, v.capacity());
}

TEST(FixedArrayListResizeTest, ResizeExceedsCapacityThrows) {
    stdx::fixed_array_list<int, 3> v;
    EXPECT_THROW(v.resize(4), std::length_error);
}

TEST(FixedArrayListResizeTest, ResizeExceedsCapacityDoesNotChangeSize) {
    stdx::fixed_array_list<int, 3> v;
    v.push_back(1);
    v.push_back(2);
    EXPECT_THROW(v.resize(4), std::length_error);
    EXPECT_EQ(2u, v.size());
}

// --- resize(count, value) ---

TEST(FixedArrayListResizeTest, ResizeWithValueSameSizeIsNoop) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.resize(3, 99);
    EXPECT_EQ(3u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
}

TEST(FixedArrayListResizeTest, ResizeWithValueToZeroFromNonEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.resize(0, 99);
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
}

TEST(FixedArrayListResizeTest, ResizeWithValueLargerFromEmpty) {
    stdx::fixed_array_list<int, 5> v;
    v.resize(3, 42);
    ASSERT_EQ(3u, v.size());
    EXPECT_EQ(42, v[0]);
    EXPECT_EQ(42, v[1]);
    EXPECT_EQ(42, v[2]);
}

TEST(FixedArrayListResizeTest, ResizeWithValueLargerPreservesExistingElements) {
    stdx::fixed_array_list<int, 5> v;
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

TEST(FixedArrayListResizeTest, ResizeWithValueSmallerReducesSize) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.resize(2, 99);
    ASSERT_EQ(2u, v.size());
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
}

TEST(FixedArrayListResizeTest, ResizeWithValueSmallerCapacityRemainsN) {
    stdx::fixed_array_list<int, 5> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.resize(1, 99);
    EXPECT_EQ(1u, v.size());
    EXPECT_EQ(5u, v.capacity());
}

TEST(FixedArrayListResizeTest, ResizeWithValueSmallerDestroysRemovedElements) {
    ResizeDestructorCounter::destructions = 0;
    {
        stdx::fixed_array_list<ResizeDestructorCounter, 5> v;
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace_back(3);
        ResizeDestructorCounter filler{0};
        ResizeDestructorCounter::destructions = 0;
        v.resize(1, filler);
        EXPECT_EQ(2, ResizeDestructorCounter::destructions);
        EXPECT_EQ(1u, v.size());
        EXPECT_EQ(1, v[0].value);
    }
}

TEST(FixedArrayListResizeTest, ResizeWithValueWithString) {
    stdx::fixed_array_list<std::string, 5> v;
    v.push_back("hello");
    v.resize(4, "world");
    ASSERT_EQ(4u, v.size());
    EXPECT_EQ("hello", v[0]);
    EXPECT_EQ("world", v[1]);
    EXPECT_EQ("world", v[2]);
    EXPECT_EQ("world", v[3]);
}

TEST(FixedArrayListResizeTest, ResizeWithValueToExactCapacitySucceeds) {
    stdx::fixed_array_list<int, 4> v;
    v.push_back(1);
    v.resize(4, 7);
    EXPECT_EQ(4u, v.size());
    EXPECT_EQ(7, v[1]);
    EXPECT_EQ(7, v[2]);
    EXPECT_EQ(7, v[3]);
}

TEST(FixedArrayListResizeTest, ResizeWithValueExceedsCapacityThrows) {
    stdx::fixed_array_list<int, 3> v;
    EXPECT_THROW(v.resize(4, 99), std::length_error);
}

TEST(FixedArrayListResizeTest, ResizeWithValueExceedsCapacityDoesNotChangeSize) {
    stdx::fixed_array_list<int, 3> v;
    v.push_back(1);
    v.push_back(2);
    EXPECT_THROW(v.resize(4, 99), std::length_error);
    EXPECT_EQ(2u, v.size());
}

TEST(FixedArrayListResizeTest, ResizeWithValueSmallerThenLargerUsesValue) {
    stdx::fixed_array_list<int, 5> v;
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
