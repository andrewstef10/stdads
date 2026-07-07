#include <algorithm>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

#include <stdx/iterator.h>

/// @brief Test class used to test creating a custom forward iterator
/// @tparam T Iterator value type
template <typename T>
class TestForwardIterator : public stdx::forward_iterator<TestForwardIterator<T>, T> {
  public:
    // ===== Constructors =====
    TestForwardIterator() : m_ptr(0) {}
    TestForwardIterator(T* ptr) : m_ptr(ptr) {}
    TestForwardIterator(const TestForwardIterator&) = default;
    TestForwardIterator(TestForwardIterator&&) = default;
    ~TestForwardIterator() = default;
    TestForwardIterator& operator=(const TestForwardIterator&) = default;
    TestForwardIterator& operator=(TestForwardIterator&&) = default;

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TestForwardIterator(const TestForwardIterator<U>& other) : m_ptr(&(*other)) {} // Const conversion constructor

    // ==== Required by ForwardIterator ====
    void increment() { ++m_ptr; }
    bool equals(const TestForwardIterator& other) const { return m_ptr == other.m_ptr; }
    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

  private:
    T* m_ptr;
};

/// @brief Test class used to test creating a custom bidirectional iterator
/// @tparam T Iterator value type
template <typename T>
class TestBidirectionalIterator : public stdx::bidirectional_iterator<TestBidirectionalIterator<T>, T> {
  public:
    // ===== Constructors =====
    TestBidirectionalIterator() : m_ptr(0) {}
    TestBidirectionalIterator(T* ptr) : m_ptr(ptr) {}
    TestBidirectionalIterator(const TestBidirectionalIterator&) = default;
    TestBidirectionalIterator(TestBidirectionalIterator&&) = default;
    ~TestBidirectionalIterator() = default;
    TestBidirectionalIterator& operator=(const TestBidirectionalIterator&) = default;
    TestBidirectionalIterator& operator=(TestBidirectionalIterator&&) = default;

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TestBidirectionalIterator(const TestBidirectionalIterator<U>& other)
        : m_ptr(&(*other)) {} // Const conversion constructor

    // ==== Required by ForwardIterator ====
    void increment() { ++m_ptr; }
    bool equals(const TestBidirectionalIterator& other) const { return m_ptr == other.m_ptr; }
    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

    // ==== Required by Bidirectional Iterator ====
    void decrement() { --m_ptr; }

  private:
    T* m_ptr;
};

/// @brief Test class used to test creating a custom random access iterator
/// @tparam T Iterator value type
template <typename T>
class TestRandomAccessIterator : public stdx::random_access_iterator<TestRandomAccessIterator<T>, T> {
  public:
    // ===== Constructors =====
    TestRandomAccessIterator() : m_ptr(0) {}
    TestRandomAccessIterator(T* ptr) : m_ptr(ptr) {}
    TestRandomAccessIterator(const TestRandomAccessIterator&) = default;
    TestRandomAccessIterator(TestRandomAccessIterator&&) = default;
    ~TestRandomAccessIterator() = default;
    TestRandomAccessIterator& operator=(const TestRandomAccessIterator&) = default;
    TestRandomAccessIterator& operator=(TestRandomAccessIterator&&) = default;

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TestRandomAccessIterator(const TestRandomAccessIterator<U>& other)
        : m_ptr(&(*other)) {} // Const conversion constructor

    // ==== Required by ForwardIterator ====
    void increment() { ++m_ptr; }
    bool equals(const TestRandomAccessIterator& other) const { return m_ptr == other.m_ptr; }
    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

    // ==== Required by Bidirectional Iterator ====
    void decrement() { --m_ptr; }

    // ==== Required by Random Access Iterator ====
    void advance(std::ptrdiff_t n) { m_ptr += n; }
    std::ptrdiff_t distance_to(const TestRandomAccessIterator& other) const { return m_ptr - other.m_ptr; }
    bool less_than(const TestRandomAccessIterator& other) const { return m_ptr < other.m_ptr; }

  private:
    T* m_ptr;
};

// ============================================================
// Custom ForwardIterator Tests
// ============================================================

TEST(CustomForwardIteratorTest, IteratorTraitsTest) {
    using It = TestForwardIterator<int>;
    static_assert(std::is_same_v<std::iterator_traits<It>::value_type, int>);
    static_assert(std::is_same_v<std::iterator_traits<It>::reference, int&>);
    static_assert(std::is_same_v<std::iterator_traits<It>::pointer, int*>);
    static_assert(std::is_same_v<std::iterator_traits<It>::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<std::iterator_traits<It>::iterator_category, std::forward_iterator_tag>);
}

TEST(CustomForwardIteratorTest, PointerConstructorAndDereferenceAndArrow) {
    int arr[] = {1, 2, 3};
    TestForwardIterator<int> it(arr);

    EXPECT_EQ(*it, 1);
    EXPECT_EQ(it.operator->(), &arr[0]);
}

TEST(CustomForwardIteratorTest, DefaultConstructor) {
    TestForwardIterator<int> it;

    // Cannot safely dereference, but should be constructible
    SUCCEED();
}

TEST(CustomForwardIteratorTest, CopyConstructor) {
    int arr[] = {1, 2};

    TestForwardIterator<int> it(arr);
    TestForwardIterator<int> copy(it);
    EXPECT_EQ(*copy, 1);
}

TEST(CustomForwardIteratorTest, CopyAssignment) {
    int arr[] = {1, 2};

    TestForwardIterator<int> a(arr);
    TestForwardIterator<int> b;
    b = a;

    EXPECT_EQ(*b, 1);
}

TEST(CustomForwardIteratorTest, MoveConstructor) {
    int arr[] = {9};

    TestForwardIterator<int> it(arr);
    TestForwardIterator<int> moved(std::move(it));

    EXPECT_EQ(*moved, 9);
}

TEST(CustomForwardIteratorTest, MoveAssignment) {
    int arr[] = {7};

    TestForwardIterator<int> it(arr);
    TestForwardIterator<int> target;

    target = std::move(it);

    EXPECT_EQ(*target, 7);
}

TEST(CustomForwardIteratorTest, Destructor) {
    int arr[] = {1, 2, 3};
    TestForwardIterator<int>* it = new TestForwardIterator<int>(arr);
    delete it;
}

TEST(CustomForwardIteratorTest, PreIncrement) {
    int arr[] = {1, 2, 3};
    TestForwardIterator<int> it(arr);

    ++it;
    EXPECT_EQ(*it, 2);
}

TEST(CustomForwardIteratorTest, PostIncrement) {
    int arr[] = {1, 2, 3};
    TestForwardIterator<int> it(arr);

    TestForwardIterator<int> old = it++;
    EXPECT_EQ(*old, 1);
    EXPECT_EQ(*it, 2);
}

TEST(CustomForwardIteratorTest, IterateForwardThroughArray) {
    int arr[] = {1, 2, 3};

    TestForwardIterator<int> it = arr;
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(*it, arr[i]);
        ++it;
    }
}

// ============================================================
// Custom Bidirectional Iterator Tests
// ============================================================

TEST(CustomBidirectionalIteratorTest, IteratorTraitsTest) {
    using It = TestBidirectionalIterator<int>;
    static_assert(std::is_same_v<std::iterator_traits<It>::value_type, int>);
    static_assert(std::is_same_v<std::iterator_traits<It>::reference, int&>);
    static_assert(std::is_same_v<std::iterator_traits<It>::pointer, int*>);
    static_assert(std::is_same_v<std::iterator_traits<It>::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<std::iterator_traits<It>::iterator_category, std::bidirectional_iterator_tag>);
}

TEST(CustomBidirectionalIteratorTest, PointerConstructorAndDereferenceAndArrow) {
    int arr[] = {1, 2, 3};
    TestBidirectionalIterator<int> it(arr);

    EXPECT_EQ(*it, 1);
    EXPECT_EQ(it.operator->(), &arr[0]);
}

TEST(CustomBidirectionalIteratorTest, DefaultConstructor) {
    TestBidirectionalIterator<int> it;

    // Cannot safely dereference, but should be constructible
    SUCCEED();
}

TEST(CustomBidirectionalIteratorTest, CopyConstructor) {
    int arr[] = {1, 2};

    TestBidirectionalIterator<int> it(arr);
    TestBidirectionalIterator<int> copy(it);
    EXPECT_EQ(*copy, 1);
}

TEST(CustomBidirectionalIteratorTest, CopyAssignment) {
    int arr[] = {1, 2};

    TestBidirectionalIterator<int> a(arr);
    TestBidirectionalIterator<int> b;
    b = a;

    EXPECT_EQ(*b, 1);
}

TEST(CustomBidirectionalIteratorTest, MoveConstructor) {
    int arr[] = {9};

    TestBidirectionalIterator<int> it(arr);
    TestBidirectionalIterator<int> moved(std::move(it));

    EXPECT_EQ(*moved, 9);
}

TEST(CustomBidirectionalIteratorTest, MoveAssignment) {
    int arr[] = {7};

    TestBidirectionalIterator<int> it(arr);
    TestBidirectionalIterator<int> target;

    target = std::move(it);

    EXPECT_EQ(*target, 7);
}

TEST(CustomBidirectionalIteratorTest, Destructor) {
    int arr[] = {1, 2, 3};
    TestBidirectionalIterator<int>* it = new TestBidirectionalIterator<int>(arr);
    delete it;
}

TEST(CustomBidirectionalIteratorTest, PreIncrement) {
    int arr[] = {1, 2, 3};
    TestBidirectionalIterator<int> it(arr);

    ++it;
    EXPECT_EQ(*it, 2);
}

TEST(CustomBidirectionalIteratorTest, PostIncrement) {
    int arr[] = {1, 2, 3};
    TestBidirectionalIterator<int> it(arr);

    TestBidirectionalIterator<int> old = it++;
    EXPECT_EQ(*old, 1);
    EXPECT_EQ(*it, 2);
}

TEST(CustomBidirectionalIteratorTest, IterateForwardThroughArray) {
    int arr[] = {1, 2, 3};

    TestBidirectionalIterator<int> it = arr;
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(*it, arr[i]);
        ++it;
    }
}

TEST(CustomBidirectionalIteratorTest, PreDecrement) {
    int arr[] = {1, 2, 3};
    TestBidirectionalIterator<int> it(arr + 1);

    --it;
    EXPECT_EQ(*it, 1);
}

TEST(CustomBidirectionalIteratorTest, PostDecrement) {
    int arr[] = {1, 2, 3};
    TestBidirectionalIterator<int> it(arr + 1);

    TestBidirectionalIterator<int> old = it--;
    EXPECT_EQ(*old, 2);
    EXPECT_EQ(*it, 1);
}

TEST(CustomBidirectionalIteratorTest, IterateBackwardThroughArray) {
    int arr[] = {1, 2, 3};

    TestBidirectionalIterator<int> it = arr + 2;
    for (int i = 2; i >= 0; --i) {
        EXPECT_EQ(*it, arr[i]);
        --it;
    }
}

// ============================================================
// TestRandomAccessIterator Tests
// ============================================================

TEST(CustomRandomAccessIteratorTest, IteratorTraitsTest) {
    using It = TestRandomAccessIterator<int>;
    static_assert(std::is_same_v<std::iterator_traits<It>::value_type, int>);
    static_assert(std::is_same_v<std::iterator_traits<It>::reference, int&>);
    static_assert(std::is_same_v<std::iterator_traits<It>::pointer, int*>);
    static_assert(std::is_same_v<std::iterator_traits<It>::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>);
}

TEST(CustomRandomAccessIteratorTest, PointerConstructorAndDereferenceAndArrow) {
    int arr[] = {1, 2, 3};
    TestRandomAccessIterator<int> it(arr);

    EXPECT_EQ(*it, 1);
    EXPECT_EQ(it.operator->(), &arr[0]);
}

TEST(CustomRandomAccessIteratorTest, DefaultConstructor) {
    TestRandomAccessIterator<int> it;

    // Cannot safely dereference, but should be constructible
    SUCCEED();
}

TEST(CustomRandomAccessIteratorTest, CopyConstructor) {
    int arr[] = {1, 2};

    TestRandomAccessIterator<int> it(arr);
    TestRandomAccessIterator<int> copy(it);
    EXPECT_EQ(*copy, 1);
}

TEST(CustomRandomAccessIteratorTest, CopyAssignment) {
    int arr[] = {1, 2};

    TestRandomAccessIterator<int> a(arr);
    TestRandomAccessIterator<int> b;
    b = a;

    EXPECT_EQ(*b, 1);
}

TEST(CustomRandomAccessIteratorTest, MoveConstructor) {
    int arr[] = {9};

    TestRandomAccessIterator<int> it(arr);
    TestRandomAccessIterator<int> moved(std::move(it));

    EXPECT_EQ(*moved, 9);
}

TEST(CustomRandomAccessIteratorTest, MoveAssignment) {
    int arr[] = {7};

    TestRandomAccessIterator<int> it(arr);
    TestRandomAccessIterator<int> target;

    target = std::move(it);

    EXPECT_EQ(*target, 7);
}

TEST(CustomRandomAccessIteratorTest, Destructor) {
    int arr[] = {1, 2, 3};
    TestRandomAccessIterator<int>* it = new TestRandomAccessIterator<int>(arr);
    delete it;
}

TEST(CustomRandomAccessIteratorTest, PreIncrement) {
    int arr[] = {1, 2, 3};
    TestRandomAccessIterator<int> it(arr);

    ++it;
    EXPECT_EQ(*it, 2);
}

TEST(CustomRandomAccessIteratorTest, PostIncrement) {
    int arr[] = {1, 2, 3};
    TestRandomAccessIterator<int> it(arr);

    TestRandomAccessIterator<int> old = it++;
    EXPECT_EQ(*old, 1);
    EXPECT_EQ(*it, 2);
}

TEST(CustomRandomAccessIteratorTest, IterateForwardThroughArray) {
    int arr[] = {1, 2, 3};

    TestRandomAccessIterator<int> it = arr;
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(*it, arr[i]);
        ++it;
    }
}

TEST(CustomRandomAccessIteratorTest, PreDecrement) {
    int arr[] = {1, 2, 3};
    TestRandomAccessIterator<int> it(arr + 1);

    --it;
    EXPECT_EQ(*it, 1);
}

TEST(CustomRandomAccessIteratorTest, PostDecrement) {
    int arr[] = {1, 2, 3};
    TestRandomAccessIterator<int> it(arr + 1);

    TestRandomAccessIterator<int> old = it--;
    EXPECT_EQ(*old, 2);
    EXPECT_EQ(*it, 1);
}

TEST(CustomRandomAccessIteratorTest, IterateBackwardThroughArray) {
    int arr[] = {1, 2, 3};

    TestRandomAccessIterator<int> it = arr + 2;
    for (int i = 2; i >= 0; --i) {
        EXPECT_EQ(*it, arr[i]);
        --it;
    }
}

TEST(CustomRandomAccessIteratorTest, AdvanceAndArithmetic) {
    int arr[] = {10, 20, 30, 40};
    TestRandomAccessIterator<int> it(arr);

    it += 2;
    EXPECT_EQ(*it, 30);

    it -= 1;
    EXPECT_EQ(*it, 20);

    TestRandomAccessIterator<int> it2 = it + 2;
    EXPECT_EQ(*it2, 40);

    TestRandomAccessIterator<int> it3 = it2 - 3;
    EXPECT_EQ(*it3, 10);
}

TEST(CustomRandomAccessIteratorTest, AdditionSymmetry) {
    int arr[] = {1, 2, 3, 4};

    TestRandomAccessIterator<int> it(arr);

    TestRandomAccessIterator<int> a = it + 2;
    TestRandomAccessIterator<int> b = 2 + it;

    EXPECT_EQ(*a, *b);
    EXPECT_EQ(a, b);
}

TEST(CustomRandomAccessIteratorTest, Distance) {
    int arr[] = {1, 2, 3, 4};
    TestRandomAccessIterator<int> a(arr);
    TestRandomAccessIterator<int> b(arr + 3);

    EXPECT_EQ(b - a, 3);
    EXPECT_EQ(a - b, -3);

    TestRandomAccessIterator<const int> cit(arr + 2);
    EXPECT_EQ(cit - a, 2);
    EXPECT_EQ(a - cit, -2);
}

TEST(CustomRandomAccessIteratorTest, IndexOperator) {
    int arr[] = {5, 6, 7, 8};
    TestRandomAccessIterator<int> it(arr);

    EXPECT_EQ(it[0], 5);
    EXPECT_EQ(it[2], 7);
}

TEST(CustomRandomAccessIteratorTest, EqualityAndInequality) {
    int arr[] = {1, 2};
    TestRandomAccessIterator<int> a(arr);
    TestRandomAccessIterator<int> b(arr);
    TestRandomAccessIterator<int> c(arr + 1);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);

    EXPECT_TRUE(a != c);
    EXPECT_FALSE(a == c);
}

TEST(CustomRandomAccessIteratorTest, RelationalOperators) {
    int arr[] = {1, 2, 3};
    TestRandomAccessIterator<int> a(arr);
    TestRandomAccessIterator<int> aCopy(arr);
    TestRandomAccessIterator<int> b(arr + 1);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b >= a);
    EXPECT_TRUE(a <= aCopy);
    EXPECT_TRUE(a >= aCopy);

    EXPECT_FALSE(b < a);
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(b <= a);
    EXPECT_FALSE(a >= b);
}

TEST(CustomRandomAccessIteratorTest, ComparisonOperatorsBetweenConstAndNormal) {
    int arr[] = {1, 2, 3};
    TestRandomAccessIterator<int> a(arr);
    TestRandomAccessIterator<const int> caCopy(arr);
    TestRandomAccessIterator<const int> b(arr + 1);

    EXPECT_TRUE(a == caCopy);
    EXPECT_FALSE(a != caCopy);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == b);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b >= a);
    EXPECT_TRUE(a <= caCopy);
    EXPECT_TRUE(a >= caCopy);

    EXPECT_FALSE(b < a);
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(b <= a);
    EXPECT_FALSE(a >= b);
}

TEST(CustomRandomAccessIteratorTest, NonConstToConstConversion) {
    int arr[] = {1, 2, 3};

    TestRandomAccessIterator<int> it(arr);
    TestRandomAccessIterator<const int> cit = it; // implicit conversion

    EXPECT_EQ(*cit, 1);
}

TEST(CustomRandomAccessIteratorTest, ConvertedIteratorTracksSameLocation) {
    int arr[] = {10, 20, 30};

    TestRandomAccessIterator<int> it(arr + 1);
    TestRandomAccessIterator<const int> cit = it;

    EXPECT_EQ(*cit, 20);

    ++it;
    ++cit;

    EXPECT_EQ(*it, 30);
    EXPECT_EQ(*cit, 30);
}

// Uncomment to test compile failure
// TEST(TestRandomAccessIteratorTest, ConstToNonConstShouldNotCompile)
// {
//     const int arr[] = {1, 2, 3};

//     TestRandomAccessIterator<const int> cit(arr);
//     TestRandomAccessIterator<int> it = cit;  // should NOT compile
// }

// ============================================================
// reverse_iterator Tests
// ============================================================

TEST(ReverseIteratorTest, ReverseIteratorTraitsTest) {
    using BIIt = stdx::reverse_iterator<TestBidirectionalIterator<int>>;
    static_assert(std::is_same_v<std::iterator_traits<BIIt>::value_type, int>);
    static_assert(std::is_same_v<std::iterator_traits<BIIt>::reference, int&>);
    static_assert(std::is_same_v<std::iterator_traits<BIIt>::pointer, int*>);
    static_assert(std::is_same_v<std::iterator_traits<BIIt>::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<std::iterator_traits<BIIt>::iterator_category, std::bidirectional_iterator_tag>);

    using RAIt = stdx::reverse_iterator<TestRandomAccessIterator<int>>;
    static_assert(std::is_same_v<std::iterator_traits<RAIt>::value_type, int>);
    static_assert(std::is_same_v<std::iterator_traits<RAIt>::reference, int&>);
    static_assert(std::is_same_v<std::iterator_traits<RAIt>::pointer, int*>);
    static_assert(std::is_same_v<std::iterator_traits<RAIt>::difference_type, std::ptrdiff_t>);
    static_assert(std::is_same_v<std::iterator_traits<RAIt>::iterator_category, std::random_access_iterator_tag>);
}

TEST(ReverseIteratorTest, BaseConstructorAndDereference) {
    int arr[] = {1, 2, 3};
    stdx::reverse_iterator<TestBidirectionalIterator<int>> birit(TestBidirectionalIterator<int>(arr + 3));
    stdx::reverse_iterator<TestRandomAccessIterator<int>> rarit(TestRandomAccessIterator<int>(arr + 3));

    EXPECT_EQ(*birit, 3);
    EXPECT_EQ(*rarit, 3);
}

TEST(ReverseIteratorTest, DefaultConstructor) {
    stdx::reverse_iterator<TestBidirectionalIterator<int>> birit;
    stdx::reverse_iterator<TestRandomAccessIterator<int>> rarit;
    SUCCEED();
}

TEST(ReverseIteratorTest, CopyConstructor) {
    int arr[] = {1, 2, 3};

    stdx::reverse_iterator<TestRandomAccessIterator<int>> a(TestRandomAccessIterator<int>(arr + 3));
    stdx::reverse_iterator<TestRandomAccessIterator<int>> b(a);
    EXPECT_EQ(*b, 3);
}

TEST(ReverseIteratorTest, CopyAssignment) {
    int arr[] = {1, 2, 3};

    stdx::reverse_iterator<TestRandomAccessIterator<int>> a(TestRandomAccessIterator<int>(arr + 3));
    stdx::reverse_iterator<TestRandomAccessIterator<int>> b;
    b = a;

    EXPECT_EQ(*b, 3);
}

TEST(ReverseIteratorTest, MoveConstructor) {
    int arr[] = {1, 2, 3};

    stdx::reverse_iterator<TestRandomAccessIterator<int>> a(TestRandomAccessIterator<int>(arr + 3));
    stdx::reverse_iterator<TestRandomAccessIterator<int>> b(std::move(a));
    EXPECT_EQ(*b, 3);
}

TEST(ReverseIteratorTest, MoveAssignment) {
    int arr[] = {1, 2, 3};

    stdx::reverse_iterator<TestRandomAccessIterator<int>> a(TestRandomAccessIterator<int>(arr + 3));
    stdx::reverse_iterator<TestRandomAccessIterator<int>> b;
    b = std::move(a);

    EXPECT_EQ(*b, 3);
}

TEST(ReverseIteratorTest, Destructor) {
    int arr[] = {1, 2, 3};
    stdx::reverse_iterator<TestRandomAccessIterator<int>>* it =
        new stdx::reverse_iterator<TestRandomAccessIterator<int>>(TestRandomAccessIterator<int>(arr + 3));
    delete it;
}

TEST(ReverseIteratorTest, ArrowOperator) {
    int arr[] = {10, 20};
    stdx::reverse_iterator<TestRandomAccessIterator<int>> rit(TestRandomAccessIterator<int>(arr + 2));

    EXPECT_EQ(rit.operator->(), &arr[1]);
}

TEST(ReverseIteratorTest, PreIncrement) {
    int arr[] = {1, 2, 3};
    stdx::reverse_iterator<TestRandomAccessIterator<int>> rit(TestRandomAccessIterator<int>(arr + 3));

    ++rit;
    EXPECT_EQ(*rit, 2);
}

TEST(ReverseIteratorTest, PostIncrement) {
    int arr[] = {1, 2, 3};
    stdx::reverse_iterator<TestRandomAccessIterator<int>> rit(TestRandomAccessIterator<int>(arr + 3));

    stdx::reverse_iterator<TestRandomAccessIterator<int>> old = rit++;
    EXPECT_EQ(*old, 3);
    EXPECT_EQ(*rit, 2);
}

TEST(ReverseIteratorTest, PreDecrement) {
    int arr[] = {1, 2, 3};
    stdx::reverse_iterator<TestRandomAccessIterator<int>> rit(TestRandomAccessIterator<int>(arr + 2));

    --rit;
    EXPECT_EQ(*rit, 3);
}

TEST(ReverseIteratorTest, PostDecrement) {
    int arr[] = {1, 2, 3};
    stdx::reverse_iterator<TestRandomAccessIterator<int>> rit(TestRandomAccessIterator<int>(arr + 2));

    stdx::reverse_iterator<TestRandomAccessIterator<int>> old = rit--;
    EXPECT_EQ(*old, 2);
    EXPECT_EQ(*rit, 3);
}

TEST(ReverseIteratorTest, Arithmetic) {
    int arr[] = {1, 2, 3, 4};
    stdx::reverse_iterator<TestRandomAccessIterator<int>> rit(TestRandomAccessIterator<int>(arr + 4));

    rit += 2;
    EXPECT_EQ(*rit, 2);

    rit -= 1;
    EXPECT_EQ(*rit, 3);

    stdx::reverse_iterator<TestRandomAccessIterator<int>> r2 = rit + 1;
    EXPECT_EQ(*r2, 2);

    stdx::reverse_iterator<TestRandomAccessIterator<int>> r3 = r2 - 2;
    EXPECT_EQ(*r3, 4);
}

TEST(ReverseIteratorTest, AdditionSymmetry) {
    int arr[] = {1, 2, 3, 4};

    using It = TestRandomAccessIterator<int>;
    stdx::reverse_iterator<It> rit(It(arr + 4));

    stdx::reverse_iterator<It> a = rit + 2;
    stdx::reverse_iterator<It> b = 2 + rit;

    EXPECT_EQ(*a, *b);
    EXPECT_EQ(a, b);
}

TEST(ReverseIteratorTest, Distance) {
    int arr[] = {1, 2, 3, 4};

    stdx::reverse_iterator<TestRandomAccessIterator<int>> a(TestRandomAccessIterator<int>(arr + 4));
    stdx::reverse_iterator<TestRandomAccessIterator<int>> b(TestRandomAccessIterator<int>(arr + 1));

    EXPECT_EQ(a - b, -3);
    EXPECT_EQ(b - a, 3);

    stdx::reverse_iterator<TestRandomAccessIterator<const int>> crit(TestRandomAccessIterator<const int>(arr + 2));
    EXPECT_EQ(a - crit, -2);
    EXPECT_EQ(crit - a, 2);
}

TEST(ReverseIteratorTest, IndexOperator) {
    int arr[] = {10, 20, 30, 40};
    stdx::reverse_iterator<TestRandomAccessIterator<int>> rit(TestRandomAccessIterator<int>(arr + 4));

    EXPECT_EQ(rit[0], 40);
    EXPECT_EQ(rit[2], 20);
}

TEST(ReverseIteratorTest, Comparisons) {
    int arr[] = {1, 2, 3};

    stdx::reverse_iterator<TestRandomAccessIterator<int>> a(TestRandomAccessIterator<int>(arr + 3));
    stdx::reverse_iterator<TestRandomAccessIterator<int>> aCopy(TestRandomAccessIterator<int>(arr + 3));
    stdx::reverse_iterator<TestRandomAccessIterator<int>> b(TestRandomAccessIterator<int>(arr + 2));

    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == b);
    EXPECT_FALSE(a != aCopy);
    EXPECT_TRUE(a == aCopy);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b >= a);
    EXPECT_TRUE(a <= aCopy);
    EXPECT_TRUE(a >= aCopy);

    EXPECT_FALSE(b < a);
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(b <= a);
    EXPECT_FALSE(a >= b);
}

TEST(ReverseIteratorTest, ComparisonsBetweenConstAndNormal) {
    int arr[] = {1, 2, 3};

    stdx::reverse_iterator<TestRandomAccessIterator<int>> a(TestRandomAccessIterator<int>(arr + 3));
    stdx::reverse_iterator<TestRandomAccessIterator<const int>> caCopy(TestRandomAccessIterator<int>(arr + 3));
    stdx::reverse_iterator<TestRandomAccessIterator<const int>> b(TestRandomAccessIterator<int>(arr + 2));

    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a != caCopy);
    EXPECT_TRUE(a == caCopy);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b >= a);
    EXPECT_TRUE(a <= caCopy);
    EXPECT_TRUE(a >= caCopy);

    EXPECT_FALSE(b < a);
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(b <= a);
    EXPECT_FALSE(a >= b);

    EXPECT_FALSE(a < caCopy);
    EXPECT_FALSE(caCopy < a);
}

TEST(ReverseIteratorTest, BaseFunction) {
    int arr[] = {1, 2, 3};
    TestRandomAccessIterator<int> it(arr + 2);

    stdx::reverse_iterator<TestRandomAccessIterator<int>> rit(it);

    EXPECT_EQ(rit.base(), it);
}

TEST(ReverseIteratorTest, NonConstToConstConversion) {
    int arr[] = {1, 2, 3};

    using It = TestRandomAccessIterator<int>;
    using CIt = TestRandomAccessIterator<const int>;

    stdx::reverse_iterator<It> rit(It(arr + 3));
    stdx::reverse_iterator<CIt> crit = rit;

    EXPECT_EQ(*crit, 3);
}

TEST(ReverseIteratorTest, ConvertedIteratorTracksSamePosition) {
    int arr[] = {5, 6, 7};

    using It = TestRandomAccessIterator<int>;
    using CIt = TestRandomAccessIterator<const int>;

    stdx::reverse_iterator<It> rit(It(arr + 3));
    stdx::reverse_iterator<CIt> crit = rit;

    ++rit;
    ++crit;

    EXPECT_EQ(*rit, 6);
    EXPECT_EQ(*crit, 6);
}

// Uncomment to test compile failure
// TEST(TestRandomAccessIteratorTest, ConstToNonConstShouldNotCompile)
// {
//     const int arr[] = {1, 2, 3};

//     using It  = TestRandomAccessIterator<int>;
//     using CIt = TestRandomAccessIterator<const int>;

//     stdx::reverse_iterator<CIt> crit(CIt(arr + 3));
//     stdx::reverse_iterator<It> rit = crit; // should NOT compile
// }

// ============================================================
// std::sort (requires RandomAccessIterator)
// ============================================================

TEST(STLCompatibilityTest, SortWithTestRandomAccessIterator) {
    int arr[] = {4, 1, 3, 2};

    TestRandomAccessIterator<int> begin(arr);
    TestRandomAccessIterator<int> end(arr + 4);

    std::sort(begin, end);

    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[3], 4);
}

TEST(STLCompatibilityTest, SortWithReverseIterator) {
    int arr[] = {1, 2, 3, 4};

    using It = TestRandomAccessIterator<int>;
    It begin(arr);
    It end(arr + 4);

    stdx::reverse_iterator<It> rbegin(end);
    stdx::reverse_iterator<It> rend(begin);

    std::sort(rbegin, rend);

    // Reverse sort should produce descending in original array
    EXPECT_EQ(arr[0], 4);
    EXPECT_EQ(arr[1], 3);
    EXPECT_EQ(arr[2], 2);
    EXPECT_EQ(arr[3], 1);
}

// ============================================================
// std::accumulate
// ============================================================

TEST(STLCompatibilityTest, Accumulate) {
    int arr[] = {1, 2, 3, 4};

    TestRandomAccessIterator<int> begin(arr);
    TestRandomAccessIterator<int> end(arr + 4);

    int sum = std::accumulate(begin, end, 0);

    EXPECT_EQ(sum, 10);
}

TEST(STLCompatibilityTest, WorksWithSTLAfterConstConversion) {
    int arr[] = {3, 1, 2};

    TestRandomAccessIterator<int> it(arr);
    TestRandomAccessIterator<const int> cit = it;

    int sum = std::accumulate(cit, cit + 3, 0);

    EXPECT_EQ(sum, 6);
}

// ============================================================
// std::copy
// ============================================================

TEST(STLCompatibilityTest, Copy) {
    int src[] = {1, 2, 3};
    int dst[] = {0, 0, 0};

    std::copy(TestRandomAccessIterator<int>(src), TestRandomAccessIterator<int>(src + 3),
              TestRandomAccessIterator<int>(dst));

    EXPECT_EQ(dst[0], 1);
    EXPECT_EQ(dst[1], 2);
    EXPECT_EQ(dst[2], 3);
}

// ============================================================
// std::distance
// ============================================================

TEST(STLCompatibilityTest, Distance) {
    int arr[] = {1, 2, 3, 4, 5};

    TestRandomAccessIterator<int> begin = TestRandomAccessIterator<int>(arr);
    TestRandomAccessIterator<int> end = TestRandomAccessIterator<int>(arr + 5);

    EXPECT_EQ(std::distance(begin, end), 5);
}

// ============================================================
// std::advance
// ============================================================

TEST(STLCompatibilityTest, Advance) {
    int arr[] = {10, 20, 30};

    TestRandomAccessIterator<int> it = TestRandomAccessIterator<int>(arr);

    std::advance(it, 2);

    EXPECT_EQ(*it, 30);
}

// ============================================================
// std::find
// ============================================================

TEST(STLCompatibilityTest, Find) {
    int arr[] = {5, 10, 15};

    TestRandomAccessIterator<int> begin = TestRandomAccessIterator<int>(arr);
    TestRandomAccessIterator<int> end = TestRandomAccessIterator<int>(arr + 3);

    TestRandomAccessIterator<int> it = std::find(begin, end, 10);

    EXPECT_NE(it, end);
    EXPECT_EQ(*it, 10);
}

// ============================================================
// std::reverse
// ============================================================

TEST(STLCompatibilityTest, Reverse) {
    int arr[] = {1, 2, 3, 4};

    std::reverse(TestRandomAccessIterator<int>(arr), TestRandomAccessIterator<int>(arr + 4));

    EXPECT_EQ(arr[0], 4);
    EXPECT_EQ(arr[1], 3);
    EXPECT_EQ(arr[2], 2);
    EXPECT_EQ(arr[3], 1);
}

// ============================================================
// std::binary_search (requires sorted + random access)
// ============================================================

TEST(STLCompatibilityTest, BinarySearch) {
    int arr[] = {1, 2, 3, 4, 5};

    bool found = std::binary_search(TestRandomAccessIterator<int>(arr), TestRandomAccessIterator<int>(arr + 5), 3);

    EXPECT_TRUE(found);
}

// ============================================================
// std::lower_bound
// ============================================================

TEST(STLCompatibilityTest, LowerBound) {
    int arr[] = {1, 2, 4, 5};

    TestRandomAccessIterator<int> it =
        std::lower_bound(TestRandomAccessIterator<int>(arr), TestRandomAccessIterator<int>(arr + 4), 3);

    EXPECT_EQ(*it, 4);
}

// ============================================================
// std::nth_element
// ============================================================

TEST(STLCompatibilityTest, NthElement) {
    int arr[] = {4, 1, 3, 2};

    TestRandomAccessIterator<int> begin = TestRandomAccessIterator<int>(arr);
    TestRandomAccessIterator<int> nth = TestRandomAccessIterator<int>(arr + 2);
    TestRandomAccessIterator<int> end = TestRandomAccessIterator<int>(arr + 4);

    std::nth_element(begin, nth, end);

    EXPECT_EQ(arr[2], 3); // third smallest element
}

// ============================================================
// std::is_sorted
// ============================================================

TEST(STLCompatibilityTest, IsSorted) {
    int sorted[] = {1, 2, 3};
    int unsorted[] = {3, 1, 2};

    EXPECT_TRUE(std::is_sorted(TestRandomAccessIterator<int>(sorted), TestRandomAccessIterator<int>(sorted + 3)));

    EXPECT_FALSE(std::is_sorted(TestRandomAccessIterator<int>(unsorted), TestRandomAccessIterator<int>(unsorted + 3)));
}

// ============================================================
// Interoperability with std::vector (copy in/out)
// ============================================================

TEST(STLCompatibilityTest, InteropWithVector) {
    int arr[] = {1, 2, 3};

    std::vector<int> v(3);

    std::copy(TestRandomAccessIterator<int>(arr), TestRandomAccessIterator<int>(arr + 3), v.begin());

    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}
