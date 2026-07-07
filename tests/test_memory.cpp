#include <gtest/gtest.h>
#include <stdx/memory.h>

#include <cstdint>
#include <limits>
#include <list>
#include <type_traits>
#include <vector>

namespace {
struct test_object {
    int value;

    explicit test_object(int v = 0) : value(v) {}
};

struct destructor_counter {
    int* count;

    explicit destructor_counter(int* c) : count(c) {}
    ~destructor_counter() { ++(*count); }
};
} // namespace

TEST(GrowthPolicy, ExactGrowthPolicy) {
    // Always returns required capacity
    stdx::exact_growth exactGrowth;

    EXPECT_EQ(0, exactGrowth(0, 0));
    EXPECT_EQ(1, exactGrowth(0, 1));
    EXPECT_EQ(2, exactGrowth(0, 2));
    EXPECT_EQ(100, exactGrowth(0, 100));
    EXPECT_EQ(1, exactGrowth(1, 1));
    EXPECT_EQ(1, exactGrowth(2, 1));
}

TEST(GrowthPolicy, DoublingGrowthPolicy) {
    // Doubles capacity
    stdx::doubling_growth doublingGrowth;

    EXPECT_EQ(0, doublingGrowth(0, 0));
    EXPECT_EQ(1, doublingGrowth(0, 1));
    EXPECT_EQ(2, doublingGrowth(0, 2));
    EXPECT_EQ(4, doublingGrowth(0, 3));
    EXPECT_EQ(4, doublingGrowth(0, 4));
    EXPECT_EQ(8, doublingGrowth(0, 5));
    EXPECT_EQ(12, doublingGrowth(3, 10));
    EXPECT_EQ(5, doublingGrowth(5, 5));
    EXPECT_EQ(101, doublingGrowth(101, 100));
    EXPECT_EQ(1, doublingGrowth(1, 1));
    EXPECT_EQ(2, doublingGrowth(2, 1));
}

//------------------------------------------------------------------------------
// Traits
//------------------------------------------------------------------------------

TEST(allocator_tests, value_type_is_correct) {
    static_assert(std::is_same<stdx::allocator<int>::value_type, int>::value, "value_type incorrect");
}

TEST(allocator_tests, allocator_traits_are_correct) {
    static_assert(std::is_same<stdx::allocator<int>::is_always_equal, std::true_type>::value,
                  "is_always_equal incorrect");

    static_assert(std::is_same<stdx::allocator<int>::propagate_on_container_copy_assignment, std::false_type>::value,
                  "copy propagation incorrect");

    static_assert(std::is_same<stdx::allocator<int>::propagate_on_container_move_assignment, std::false_type>::value,
                  "move propagation incorrect");

    static_assert(std::is_same<stdx::allocator<int>::propagate_on_container_swap, std::false_type>::value,
                  "swap propagation incorrect");
}

//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

TEST(allocator_tests, default_constructible) {
    stdx::allocator<int> alloc;
    (void)alloc;
}

TEST(allocator_tests, copy_constructible) {
    stdx::allocator<int> a;
    stdx::allocator<int> b(a);

    EXPECT_TRUE(a == b);
}

TEST(allocator_tests, move_constructible) {
    stdx::allocator<int> a;
    stdx::allocator<int> b(std::move(a));

    EXPECT_TRUE(b == stdx::allocator<int>());
}

TEST(allocator_tests, copy_assignable) {
    stdx::allocator<int> a;
    stdx::allocator<int> b;

    b = a;

    EXPECT_TRUE(a == b);
}

TEST(allocator_tests, move_assignable) {
    stdx::allocator<int> a;
    stdx::allocator<int> b;

    b = std::move(a);

    EXPECT_TRUE(b == stdx::allocator<int>());
}

TEST(allocator_tests, converting_constructor) {
    stdx::allocator<int> a;
    stdx::allocator<double> b(a);

    EXPECT_TRUE(a == b);
}

TEST(allocator_tests, size_of_allocator) {
    EXPECT_EQ(1, sizeof(stdx::allocator<int>));
    EXPECT_EQ(1, sizeof(stdx::allocator<long>));
    EXPECT_EQ(1, sizeof(stdx::allocator<double>));
}

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

TEST(allocator_tests, equals_same_type) {
    stdx::allocator<int> a;
    stdx::allocator<int> b;

    EXPECT_TRUE(a.equals(b));
}

TEST(allocator_tests, equals_different_type) {
    stdx::allocator<int> a;
    stdx::allocator<double> b;

    EXPECT_TRUE(a.equals(b));
}

TEST(allocator_tests, operator_equal_same_type) {
    stdx::allocator<int> a;
    stdx::allocator<int> b;

    EXPECT_TRUE(a == b);
}

TEST(allocator_tests, operator_equal_different_type) {
    stdx::allocator<int> a;
    stdx::allocator<double> b;

    EXPECT_TRUE(a == b);
}

TEST(allocator_tests, operator_not_equal_same_type) {
    stdx::allocator<int> a;
    stdx::allocator<int> b;

    EXPECT_FALSE(a != b);
}

TEST(allocator_tests, operator_not_equal_different_type) {
    stdx::allocator<int> a;
    stdx::allocator<double> b;

    EXPECT_FALSE(a != b);
}

//------------------------------------------------------------------------------
// Allocation
//------------------------------------------------------------------------------

TEST(allocator_tests, allocate_and_deallocate_single_object) {
    stdx::allocator<int> alloc;

    int* ptr = alloc.allocate(1);

    ASSERT_NE(ptr, nullptr);

    *ptr = 42;
    EXPECT_EQ(*ptr, 42);

    alloc.deallocate(ptr, 1);
}

TEST(allocator_tests, allocate_and_deallocate_multiple_objects) {
    stdx::allocator<int> alloc;

    int* ptr = alloc.allocate(10);

    ASSERT_NE(ptr, nullptr);

    for (int i = 0; i < 10; ++i) {
        ptr[i] = i;
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(ptr[i], i);
    }

    alloc.deallocate(ptr, 10);
}

TEST(allocator_tests, max_size_is_non_zero) {
    stdx::allocator<int> alloc;

    EXPECT_GT(alloc.max_size(), 0u);
}

//------------------------------------------------------------------------------
// allocator_traits integration
//------------------------------------------------------------------------------

TEST(allocator_tests, allocator_traits_construct_destroy) {
    stdx::allocator<test_object> alloc;

    test_object* ptr = alloc.allocate(1);

    std::allocator_traits<stdx::allocator<test_object>>::construct(alloc, ptr, 123);

    EXPECT_EQ(ptr->value, 123);

    std::allocator_traits<stdx::allocator<test_object>>::destroy(alloc, ptr);

    alloc.deallocate(ptr, 1);
}

//------------------------------------------------------------------------------
// STL compatibility
//------------------------------------------------------------------------------

TEST(allocator_tests, works_with_vector) {
    std::vector<int, stdx::allocator<int>> vec;

    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }

    ASSERT_EQ(vec.size(), 100u);

    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(vec[i], i);
    }
}

TEST(allocator_tests, works_with_list) {
    std::list<int, stdx::allocator<int>> lst;

    lst.push_back(1);
    lst.push_back(2);
    lst.push_back(3);

    ASSERT_EQ(lst.size(), 3u);

    auto it = lst.begin();

    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
}

TEST(allocator_tests, vector_copy_move_operations) {
    std::vector<int, stdx::allocator<int>> v1;

    for (int i = 0; i < 50; ++i) {
        v1.push_back(i);
    }

    std::vector<int, stdx::allocator<int>> v2(v1);

    EXPECT_EQ(v1, v2);

    std::vector<int, stdx::allocator<int>> v3(std::move(v2));

    EXPECT_EQ(v3.size(), 50u);
}

//------------------------------------------------------------------------------
// Statelessness (is_always_equal)
//------------------------------------------------------------------------------

TEST(allocator_tests, memory_from_one_instance_freeable_by_another) {
    // is_always_equal = true means any two instances are interchangeable:
    // memory allocated by `a` must be safely deallocatable by `b`.
    stdx::allocator<int> a;
    stdx::allocator<int> b;

    int* ptr = a.allocate(5);
    ASSERT_NE(ptr, nullptr);

    for (int i = 0; i < 5; ++i)
        ptr[i] = i;

    b.deallocate(ptr, 5); // must not crash or corrupt
}

TEST(allocator_tests, memory_from_converting_instance_freeable_by_rebound) {
    // Rebinding to a different type still yields an equal allocator.
    stdx::allocator<int> int_alloc;
    stdx::allocator<double> double_alloc(int_alloc);

    double* ptr = double_alloc.allocate(3);
    ASSERT_NE(ptr, nullptr);

    stdx::allocator<double> other_double_alloc;
    other_double_alloc.deallocate(ptr, 3); // cross-instance deallocation
}

//------------------------------------------------------------------------------
// Pointer alignment
//------------------------------------------------------------------------------

TEST(allocator_tests, allocated_pointer_is_aligned_for_type_int) {
    stdx::allocator<int> alloc;
    int* ptr = alloc.allocate(1);

    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % alignof(int), 0u);

    alloc.deallocate(ptr, 1);
}

TEST(allocator_tests, allocated_pointer_is_aligned_for_type_double) {
    stdx::allocator<double> alloc;
    double* ptr = alloc.allocate(4);

    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % alignof(double), 0u);

    alloc.deallocate(ptr, 4);
}

TEST(allocator_tests, allocated_pointer_is_aligned_for_type_long_double) {
    stdx::allocator<long double> alloc;
    long double* ptr = alloc.allocate(1);

    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % alignof(long double), 0u);

    alloc.deallocate(ptr, 1);
}

//------------------------------------------------------------------------------
// allocate(0)
//------------------------------------------------------------------------------

TEST(allocator_tests, allocate_zero_returns_non_null_and_is_deallocatable) {
    // ::operator new(0) guarantees a non-null unique pointer;
    // the returned pointer must be safely passable to deallocate.
    stdx::allocator<int> alloc;
    int* ptr = alloc.allocate(0);

    EXPECT_NE(ptr, nullptr);

    alloc.deallocate(ptr, 0); // must not crash
}

//------------------------------------------------------------------------------
// max_size correctness
//------------------------------------------------------------------------------

TEST(allocator_tests, max_size_equals_size_t_max_divided_by_sizeof_T) {
    stdx::allocator<char> char_alloc;
    stdx::allocator<int> int_alloc;
    stdx::allocator<double> double_alloc;

    EXPECT_EQ(char_alloc.max_size(), std::numeric_limits<std::size_t>::max() / sizeof(char));
    EXPECT_EQ(int_alloc.max_size(), std::numeric_limits<std::size_t>::max() / sizeof(int));
    EXPECT_EQ(double_alloc.max_size(), std::numeric_limits<std::size_t>::max() / sizeof(double));
}

TEST(allocator_tests, max_size_decreases_with_larger_element_type) {
    stdx::allocator<char> char_alloc;
    stdx::allocator<int> int_alloc;

    EXPECT_GT(char_alloc.max_size(), int_alloc.max_size());
}

//------------------------------------------------------------------------------
// Destructor is actually invoked by allocator_traits::destroy
//------------------------------------------------------------------------------

TEST(allocator_tests, destroy_actually_calls_destructor) {
    stdx::allocator<destructor_counter> alloc;
    destructor_counter* ptr = alloc.allocate(1);

    int count = 0;
    std::allocator_traits<stdx::allocator<destructor_counter>>::construct(alloc, ptr, &count);
    EXPECT_EQ(count, 0);

    std::allocator_traits<stdx::allocator<destructor_counter>>::destroy(alloc, ptr);
    EXPECT_EQ(count, 1); // destructor must have run exactly once

    alloc.deallocate(ptr, 1);
}

TEST(allocator_tests, destroy_array_calls_each_destructor) {
    stdx::allocator<destructor_counter> alloc;
    constexpr int N = 5;
    destructor_counter* ptr = alloc.allocate(N);

    int count = 0;
    for (int i = 0; i < N; ++i)
        std::allocator_traits<stdx::allocator<destructor_counter>>::construct(alloc, ptr + i, &count);

    EXPECT_EQ(count, 0);

    for (int i = 0; i < N; ++i)
        std::allocator_traits<stdx::allocator<destructor_counter>>::destroy(alloc, ptr + i);

    EXPECT_EQ(count, N);

    alloc.deallocate(ptr, N);
}

//------------------------------------------------------------------------------
// Unique pointers
//------------------------------------------------------------------------------

TEST(allocator_tests, separate_allocations_return_distinct_pointers) {
    stdx::allocator<int> alloc;

    int* p1 = alloc.allocate(1);
    int* p2 = alloc.allocate(1);

    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    EXPECT_NE(p1, p2);

    alloc.deallocate(p1, 1);
    alloc.deallocate(p2, 1);
}

//------------------------------------------------------------------------------
// allocator_traits::rebind_alloc
//------------------------------------------------------------------------------

TEST(allocator_tests, rebind_alloc_gives_correct_type) {
    static_assert(
        std::is_same<std::allocator_traits<stdx::allocator<int>>::rebind_alloc<double>, stdx::allocator<double>>::value,
        "rebind_alloc<double> should yield stdx::allocator<double>");
}

//------------------------------------------------------------------------------
// Trivially copyable / default constructible
//------------------------------------------------------------------------------

TEST(allocator_tests, is_trivially_default_constructible) {
    static_assert(std::is_trivially_default_constructible<stdx::allocator<int>>::value,
                  "stdx::allocator must be trivially default constructible");
}

TEST(allocator_tests, is_trivially_copy_constructible) {
    static_assert(std::is_trivially_copy_constructible<stdx::allocator<int>>::value,
                  "stdx::allocator must be trivially copy constructible");
}

TEST(allocator_tests, is_trivially_copy_assignable) {
    static_assert(std::is_trivially_copy_assignable<stdx::allocator<int>>::value,
                  "stdx::allocator must be trivially copy assignable");
}

//------------------------------------------------------------------------------
// select_on_container_copy_construction
//------------------------------------------------------------------------------

TEST(allocator_tests, select_on_container_copy_construction_returns_equal_allocator) {
    stdx::allocator<int> alloc;
    stdx::allocator<int> selected =
        std::allocator_traits<stdx::allocator<int>>::select_on_container_copy_construction(alloc);

    EXPECT_TRUE(alloc == selected);
}

//------------------------------------------------------------------------------
// Repeated allocate/deallocate cycles
//------------------------------------------------------------------------------

TEST(allocator_tests, repeated_allocate_deallocate_does_not_corrupt) {
    stdx::allocator<int> alloc;

    for (int cycle = 0; cycle < 1000; ++cycle) {
        int* ptr = alloc.allocate(1);
        ASSERT_NE(ptr, nullptr);
        *ptr = cycle;
        EXPECT_EQ(*ptr, cycle);
        alloc.deallocate(ptr, 1);
    }
}

TEST(allocator_tests, large_allocation) {
    stdx::allocator<int> alloc;
    constexpr std::size_t N = 10000;

    int* ptr = alloc.allocate(N);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % alignof(int), 0u);

    for (std::size_t i = 0; i < N; ++i)
        ptr[i] = static_cast<int>(i);

    for (std::size_t i = 0; i < N; ++i)
        EXPECT_EQ(ptr[i], static_cast<int>(i));

    alloc.deallocate(ptr, N);
}
