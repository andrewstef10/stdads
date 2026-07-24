#include <gtest/gtest.h>
#include <span>
#include <stdx/array.h>
#include <stdx/span.h>
#include <type_traits>
#include <vector>

TEST(SpanTests, DefaultConstruction) {
    stdx::span<int> defaultSpan;
    std::span<int> stlDefault;

    EXPECT_EQ(defaultSpan.data(), nullptr);
    EXPECT_EQ(defaultSpan.size(), 0U);
    EXPECT_EQ(defaultSpan.size(), stlDefault.size());

    stdx::span<int, 0> defaultSpanWithFixedSize;
    std::span<int, 0> stlDefaultWithFixedSize;

    EXPECT_EQ(defaultSpanWithFixedSize.data(), nullptr);
    static_assert(defaultSpanWithFixedSize.size() == 0U);
    static_assert(defaultSpanWithFixedSize.size() == stlDefaultWithFixedSize.size());
}

TEST(SpanTests, PtrSizeConstruction) {
    int integer = 5;
    int* intPtr = &integer;

    stdx::span<int> myImpl(intPtr, 1);
    std::span<int> stl(intPtr, 1);

    EXPECT_EQ(myImpl.data(), intPtr);
    EXPECT_EQ(myImpl.size(), 1U);
}

TEST(SpanTests, PtrConstruction) {
    int integer = 5;
    int* intPtr = &integer;

    stdx::span<int, 1> myImpl(intPtr);
    // std::span<int, 1> stl(intPtr);

    EXPECT_EQ(myImpl.data(), intPtr);
    EXPECT_EQ(myImpl.size(), 1U);
}

TEST(SpanTests, CArrayConstructionFixedSize) {
    int array[5] = {1, 2, 3, 4, 5};

    stdx::span<int, 5> fixedView = stdx::make_span(array);
    std::span<int, 5> stlView = array;

    EXPECT_EQ(fixedView.data(), array);
    EXPECT_EQ(fixedView.size(), 5U);
    EXPECT_EQ(fixedView.size_bytes(), 5 * sizeof(int));
    EXPECT_EQ(fixedView.size(), stlView.size());
    EXPECT_EQ(fixedView.data(), stlView.data());

    // A span views the array, it does not copy it
    array[2] = 42;
    EXPECT_EQ(fixedView.data()[2], 42);
}

TEST(SpanTests, CArrayConstructionConstView) {
    int array[5] = {1, 2, 3, 4, 5};

    stdx::span<const int, 5> fixedView = stdx::make_span<const int>(array);
    std::span<const int, 5> stlFixedView = array;

    EXPECT_EQ(fixedView.data(), array);
    EXPECT_EQ(fixedView.size(), 5U);
}

TEST(SpanTests, CArrayConstructionRequiresCorrectConstT) {
    // A fixed size span only binds to an array of exactly N elements, matching std::span
    static_assert(std::is_constructible<stdx::span<int, 5>, int (&)[5]>::value, "matching bound must be constructible");
    // static_assert(!std::is_constructible<stdx::span<int, 3>, int (&)[5]>::value, "smaller N must not bind");
    // static_assert(!std::is_constructible<stdx::span<int, 9>, int (&)[5]>::value, "larger N must not bind");
    // static_assert(!std::is_constructible<std::span<int, 3>, int (&)[5]>::value, "std::span rejects smaller N too");

    // A mutable span must not bind to a const array
    static_assert(!std::is_constructible<stdx::span<int, 5>, const int (&)[5]>::value,
                  "mutable span must not view const elements");
    static_assert(std::is_constructible<stdx::span<const int, 5>, const int (&)[5]>::value,
                  "read-only span views const elements");
}

TEST(SpanTests, CArrayConstructionFixedSizeSpanIsPointerSized) {
    // A fixed size span's count lives in its type, so the object holds only the data pointer (empty base optimization)
    static_assert(sizeof(stdx::span<int, 5>) == sizeof(int*), "fixed size span must store only the data pointer");
    static_assert(sizeof(std::span<int, 5>) == sizeof(int*), "std::span has the same layout");

    // A dynamic span additionally stores its count
    static_assert(sizeof(stdx::span<int>) == sizeof(int*) + sizeof(std::size_t),
                  "dynamic span must store the data pointer and the count");
}

TEST(SpanTests, FixedSizeSpanDeterminsSizeAtCompileTime) {
    int array[5] = {1, 2, 3, 4, 5};

    stdx::span<int, 5> fixedView = stdx::make_span(array);
    std::span<int, 5> stlView = array;

    static_assert(fixedView.size() == 5);
    static_assert(stlView.size() == 5);
    static_assert(fixedView.size_bytes() == sizeof(int) * 5);
    static_assert(stlView.size_bytes() == sizeof(int) * 5);
}

TEST(SpanTests, VectorConstruction) {
    std::vector<int> vector = {1, 2, 3, 4, 5};

    std::span stlVectorSpan = vector;
    std::span<int> stlVectorSpan2 = vector;
}
