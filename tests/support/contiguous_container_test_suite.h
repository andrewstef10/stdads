#ifndef STDX_TEST_CONTIGUOUS_CONTAINER_TEST_SUITE_H
#define STDX_TEST_CONTIGUOUS_CONTAINER_TEST_SUITE_H

#include <gtest/gtest.h>

#include <iterator>
#include <stdexcept>
#include <vector>

#include "container_factory.h"

/// @file
/// Shared type-parameterized suite for contiguous, fixed-content containers.
/// Functions tested are (begin/end/rbegin/rend, at/operator[]/, front/back/data).
/// None of these tests mutate a container's logical size
///
/// Every test below builds its fixtures from exactly 3 values via ContainerFactory<TypeParam>::MakeFromTestValues(),
/// which pulls the values from TestValues<T> (see ContainerFactory.h) instead of hardcoding `int` literals. That's
/// what lets this suite run against containers of any value_type -- specialize TestValues<T> for value types a raw
/// int literal can't construct (std::string already has one) and instantiate the suite with that container.
///
/// To reuse against a new type, include this header in a test_*.cpp and instantiate it:
///
///     using MyTypes = ::testing::Types<stdx::array_list<int>, stdx::fixed_array_list<int, 8>, stdx::array<int, 3>>;
///     namespace stdx_test { INSTANTIATE_TYPED_TEST_SUITE_P(MyTypes, ContiguousContainerTest, MyTypes); }
///
/// (The instantiation must live inside `namespace stdx_test { ... }`)

namespace StdxTest {

template <typename Container>
class ContiguousContainerTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(ContiguousContainerTest);

// ==========================
// ========= Size ===========
// ==========================

TYPED_TEST_P(ContiguousContainerTest, SizeMatchesElementCount) {
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_EQ(container.size(), 3U);
    EXPECT_FALSE(container.empty());
}

// ==========================
// ===== Element Access =====
// ==========================

TYPED_TEST_P(ContiguousContainerTest, FrontReturnsFirstElement) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_EQ(container.front(), TestValues<T>::VALUES[0]);
}

TYPED_TEST_P(ContiguousContainerTest, BackReturnsLastElement) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_EQ(container.back(), TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ContiguousContainerTest, OperatorIndexReadAndWrite) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_EQ(container[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(container[1], TestValues<T>::VALUES[1]);
    EXPECT_EQ(container[2], TestValues<T>::VALUES[2]);
    container[1] = TestValues<T>::UPDATED;
    EXPECT_EQ(container[1], TestValues<T>::UPDATED);

    const TypeParam& constContainer = container;
    EXPECT_EQ(constContainer[0], TestValues<T>::VALUES[0]);
    EXPECT_EQ(constContainer[1], TestValues<T>::UPDATED);
    EXPECT_EQ(constContainer[2], TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ContiguousContainerTest, AtReadAndWrite) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_EQ(container.at(0), TestValues<T>::VALUES[0]);
    EXPECT_EQ(container.at(1), TestValues<T>::VALUES[1]);
    EXPECT_EQ(container.at(2), TestValues<T>::VALUES[2]);
    container.at(1) = TestValues<T>::UPDATED;
    EXPECT_EQ(container.at(1), TestValues<T>::UPDATED);

    const TypeParam& constContainer = container;
    EXPECT_EQ(constContainer.at(0), TestValues<T>::VALUES[0]);
    EXPECT_EQ(constContainer.at(1), TestValues<T>::UPDATED);
    EXPECT_EQ(constContainer.at(2), TestValues<T>::VALUES[2]);
}

TYPED_TEST_P(ContiguousContainerTest, AtOutOfRangeThrows) {
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    const TypeParam& constContainer = container;
    EXPECT_THROW(container.at(3), std::out_of_range);
    EXPECT_THROW(constContainer.at(3), std::out_of_range);
}

TYPED_TEST_P(ContiguousContainerTest, DataPointerConsistentWithSubscript) {
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_EQ(*(container.data() + 0), container[0]);
    EXPECT_EQ(*(container.data() + 1), container[1]);
    EXPECT_EQ(*(container.data() + 2), container[2]);

    const TypeParam& constContainer = container;
    EXPECT_EQ(*(constContainer.data() + 0), constContainer[0]);
    EXPECT_EQ(*(constContainer.data() + 1), constContainer[1]);
    EXPECT_EQ(*(constContainer.data() + 2), constContainer[2]);

    EXPECT_EQ(constContainer.data(), container.data());
}

// ==================================
// ====== Forward Iteration =========
// ==================================

TYPED_TEST_P(ContiguousContainerTest, BeginPointsToFirstElement) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    typename TypeParam::iterator begin = container.begin();
    typename TypeParam::const_iterator cbegin = container.cbegin();
    EXPECT_EQ(*begin, TestValues<T>::VALUES[0]);
    EXPECT_EQ(*cbegin, TestValues<T>::VALUES[0]);
    EXPECT_EQ(begin, cbegin);
}

TYPED_TEST_P(ContiguousContainerTest, EndIsOnePastLastElement) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_EQ(std::distance(container.begin(), container.end()), static_cast<std::ptrdiff_t>(container.size()));

    const typename TypeParam::iterator begin = container.begin();
    typename TypeParam::iterator end = container.end();
    typename TypeParam::const_iterator cend = container.cend();
    EXPECT_EQ(end - begin, 3);
    EXPECT_EQ(begin - end, -3);
    EXPECT_EQ(cend - begin, 3);
    EXPECT_EQ(begin - cend, -3);

    // end is not dereferenceable. Ensure decrementing gives us the last element
    EXPECT_EQ(*--end, TestValues<T>::VALUES[2]);
    EXPECT_EQ(*--cend, TestValues<T>::VALUES[2]);
    EXPECT_EQ(end, cend);
}

TYPED_TEST_P(ContiguousContainerTest, ForwardIterationVisitsElementsInOrder) {
    using T = typename TypeParam::value_type;
    const T expected[] = {TestValues<T>::VALUES[0], TestValues<T>::VALUES[1], TestValues<T>::VALUES[2]};
    const std::vector<T> expectedVec(expected, expected + 3);
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    const TypeParam& constContainer = container;

    int i = 0;
    for (typename TypeParam::iterator it = container.begin(); it != container.end(); ++it) {
        EXPECT_EQ(*it, expected[i++]);
    }
    EXPECT_EQ(3, i);

    i = 0;
    for (typename TypeParam::const_iterator cit = container.cbegin(); cit != container.cend(); ++cit) {
        EXPECT_EQ(*cit, expected[i++]);
    }
    EXPECT_EQ(3, i);

    i = 0;
    for (typename TypeParam::const_iterator cit = constContainer.begin(); cit != constContainer.end(); ++cit) {
        EXPECT_EQ(*cit, expected[i++]);
    }
    EXPECT_EQ(3, i);

    std::vector<T> visited(container.begin(), container.end());
    EXPECT_EQ(visited, expectedVec);

    std::vector<T> visited1(container.cbegin(), container.cend());
    EXPECT_EQ(visited1, expectedVec);

    std::vector<T> visited2(constContainer.begin(), constContainer.end());
    EXPECT_EQ(visited2, expectedVec);
}

TYPED_TEST_P(ContiguousContainerTest, IterateForwardRangeBasedForLoop) {
    using T = typename TypeParam::value_type;
    const T expected[] = {TestValues<T>::VALUES[0], TestValues<T>::VALUES[1], TestValues<T>::VALUES[2]};
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    const TypeParam& constContainer = container;

    int i = 0;
    for (T x : container) {
        EXPECT_EQ(x, expected[i++]);
    }
    EXPECT_EQ(3, i);

    i = 0;
    for (T x : constContainer) {
        EXPECT_EQ(x, expected[i++]);
    }
    EXPECT_EQ(3, i);

    i = 0;
    for (T& x : container) {
        EXPECT_EQ(x, expected[i++]);
    }
    EXPECT_EQ(3, i);

    i = 0;
    for (const T& x : container) {
        EXPECT_EQ(x, expected[i++]);
    }
    EXPECT_EQ(3, i);

    i = 0;
    for (const T& x : constContainer) {
        EXPECT_EQ(x, expected[i++]);
    }
    EXPECT_EQ(3, i);
}

TYPED_TEST_P(ContiguousContainerTest, ConstIterationMatchesNonConst) {
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();

    int i = 0;
    typename TypeParam::iterator it = container.begin();
    typename TypeParam::const_iterator cit = container.cbegin();
    for (; it != container.end() && cit != container.cend(); ++it, ++cit) {
        EXPECT_EQ(*it, *cit);
        EXPECT_EQ(it, cit);
        ++i;
    }
    EXPECT_EQ(3, i);
}

TYPED_TEST_P(ContiguousContainerTest, MutableIteratorAllowsWrite) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    *container.begin() = TestValues<T>::UPDATED;
    EXPECT_EQ(container.front(), TestValues<T>::UPDATED);
}

TYPED_TEST_P(ContiguousContainerTest, ConvertIteratorToConstIterator) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    typename TypeParam::iterator it = container.begin();
    typename TypeParam::const_iterator cit = it;
    EXPECT_EQ(*it, TestValues<T>::VALUES[0]);
    EXPECT_EQ(*cit, TestValues<T>::VALUES[0]);
    EXPECT_EQ(it, cit);
}

// ==================================
// ====== Reverse Iteration =========
// ==================================

TYPED_TEST_P(ContiguousContainerTest, RBeginPointsToFirstElement) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    typename TypeParam::reverse_iterator rbegin = container.rbegin();
    typename TypeParam::const_reverse_iterator crbegin = container.crbegin();
    EXPECT_EQ(*rbegin, TestValues<T>::VALUES[2]);
    EXPECT_EQ(*crbegin, TestValues<T>::VALUES[2]);
    EXPECT_EQ(rbegin, crbegin);
}

TYPED_TEST_P(ContiguousContainerTest, REndIsOnePastLastElement) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    EXPECT_EQ(std::distance(container.rbegin(), container.rend()), static_cast<std::ptrdiff_t>(container.size()));

    const typename TypeParam::reverse_iterator rbegin = container.rbegin();
    typename TypeParam::reverse_iterator rend = container.rend();
    typename TypeParam::const_reverse_iterator crend = container.crend();
    EXPECT_EQ(rend - rbegin, 3);
    EXPECT_EQ(rbegin - rend, -3);
    EXPECT_EQ(crend - rbegin, 3);
    EXPECT_EQ(rbegin - crend, -3);

    // rend is not dereferenceable. Ensure decrementing gives us the first element
    EXPECT_EQ(*--rend, TestValues<T>::VALUES[0]);
    EXPECT_EQ(*--crend, TestValues<T>::VALUES[0]);
    EXPECT_EQ(rend, crend);
}

TYPED_TEST_P(ContiguousContainerTest, ReverseIterationVisitsElementsInOrder) {
    using T = typename TypeParam::value_type;
    const T expected[] = {TestValues<T>::VALUES[2], TestValues<T>::VALUES[1], TestValues<T>::VALUES[0]};
    const std::vector<T> expectedVec(expected, expected + 3);
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    const TypeParam& constContainer = container;

    int i = 0;
    for (typename TypeParam::reverse_iterator it = container.rbegin(); it != container.rend(); ++it) {
        EXPECT_EQ(*it, expected[i++]);
    }
    EXPECT_EQ(3, i);

    i = 0;
    for (typename TypeParam::const_reverse_iterator cit = container.crbegin(); cit != container.crend(); ++cit) {
        EXPECT_EQ(*cit, expected[i++]);
    }
    EXPECT_EQ(3, i);

    i = 0;
    for (typename TypeParam::const_reverse_iterator cit = constContainer.rbegin(); cit != constContainer.rend();
         ++cit) {
        EXPECT_EQ(*cit, expected[i++]);
    }
    EXPECT_EQ(3, i);

    std::vector<T> visited(container.rbegin(), container.rend());
    EXPECT_EQ(visited, expectedVec);

    std::vector<T> visited1(container.crbegin(), container.crend());
    EXPECT_EQ(visited1, expectedVec);

    std::vector<T> visited2(constContainer.rbegin(), constContainer.rend());
    EXPECT_EQ(visited2, expectedVec);
}

TYPED_TEST_P(ContiguousContainerTest, ConstReverseIterationMatchesNonConst) {
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();

    int i = 0;
    typename TypeParam::reverse_iterator it = container.rbegin();
    typename TypeParam::const_reverse_iterator cit = container.crbegin();
    for (; it != container.rend() && cit != container.crend(); ++it, ++cit) {
        EXPECT_EQ(*it, *cit);
        EXPECT_EQ(it, cit);
        ++i;
    }
    EXPECT_EQ(3, i);
}

TYPED_TEST_P(ContiguousContainerTest, MutableReverseIteratorAllowsWrite) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    *container.rbegin() = TestValues<T>::UPDATED;
    EXPECT_EQ(container.back(), TestValues<T>::UPDATED);
}

TYPED_TEST_P(ContiguousContainerTest, ConvertReverseIteratorToConstReverseIterator) {
    using T = typename TypeParam::value_type;
    TypeParam container = ContainerFactory<TypeParam>::MakeFromTestValues();
    typename TypeParam::reverse_iterator it = container.rbegin();
    typename TypeParam::const_reverse_iterator cit = it;
    EXPECT_EQ(*it, TestValues<T>::VALUES[2]);
    EXPECT_EQ(*cit, TestValues<T>::VALUES[2]);
    EXPECT_EQ(it, cit);
}

REGISTER_TYPED_TEST_SUITE_P(ContiguousContainerTest, SizeMatchesElementCount, OperatorIndexReadAndWrite,
                            AtOutOfRangeThrows, FrontReturnsFirstElement, BackReturnsLastElement,
                            DataPointerConsistentWithSubscript, BeginPointsToFirstElement, EndIsOnePastLastElement,
                            ForwardIterationVisitsElementsInOrder, ConstIterationMatchesNonConst,
                            MutableIteratorAllowsWrite, ConvertIteratorToConstIterator, AtReadAndWrite,
                            IterateForwardRangeBasedForLoop, RBeginPointsToFirstElement, REndIsOnePastLastElement,
                            ReverseIterationVisitsElementsInOrder, ConstReverseIterationMatchesNonConst,
                            MutableReverseIteratorAllowsWrite, ConvertReverseIteratorToConstReverseIterator);

} // namespace StdxTest

#endif
