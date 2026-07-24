#ifndef ARRAY_H
#define ARRAY_H

#include <cstddef>
#include <utility>

#include <stdx/internal/internal_contiguous_container.h>
#include <stdx/iterator.h>
#include <stdx/span.h>

namespace stdx {

/**
 * @brief Fixed size contiguous array class.
 */
template <typename T, std::size_t N>
struct array {

    using value_type = T;
    using size_type = std::size_t;

    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = stdx::reverse_iterator<iterator>;
    using const_reverse_iterator = stdx::reverse_iterator<const_iterator>;

    // ==== Implicitly-defined member functions ====

    // Constructor:
    // Initializes the array following the rules of aggregate initialization (note that default initialization may
    // result in indeterminate values for non-class T)

    // Destructor:
    // Destroys every element of the array

    // operator=:
    // Overwrites every element of the array with the corresponding element of another array

    // ===== Element Access =====

    /// @brief Returns a reference to the element at specified location pos, with bounds checking.
    /// If pos is not within the range of the container, an exception of type std::out_of_range is thrown.
    /// @param pos Position of the element to return.
    /// @return Reference to the requested element.
    /// @exception std::out_of_range if pos is out of range (pos >= size()).
    T& at(std::size_t pos) { return internal::contiguous_container::at(*this, pos); }
    const T& at(std::size_t pos) const { return internal::contiguous_container::at(*this, pos); }

    /// @brief Returns a reference to the element at specified location pos, without bounds checking.
    /// @param index Position of the element to return.
    /// @return Reference to the requested element.
    T& operator[](std::size_t index) { return elems[index]; }
    const T& operator[](std::size_t index) const { return elems[index]; }

    /// @brief Returns a pointer to the underlying array serving as element storage.
    /// The pointer is such that range [data(), data() + size()) is always a valid range.
    /// If *this is empty, data() is not dereferenceable.
    /// @return Pointer to the underlying element storage. For non-empty containers, the returned pointer compares equal
    /// to the address of the first element.
    T* data() { return elems; }
    const T* data() const { return elems; }

    /// @brief Returns a reference to the last element in the container.
    /// @return Reference to the last element.
    T& back() { return elems[N - 1]; }
    const T& back() const { return elems[N - 1]; }

    /// @brief Returns a reference to the first element in the container.
    /// @return Reference to the first element.
    T& front() { return elems[0]; }
    const T& front() const { return elems[0]; }

    // ==== Iterators ====

    /// @brief Returns a contiguous iterator to the first element of *this.
    /// If *this is empty, the returned iterator will be equal to end().
    /// @return contiguous iterator to the first element.
    iterator begin() { return iterator(elems); }
    const_iterator begin() const { return const_iterator(elems); }
    const_iterator cbegin() const { return const_iterator(elems); }

    /// @brief Returns a contiguous iterator past the last element of *this.
    /// This returned iterator only acts as a sentinel. It is not guaranteed to be dereferenceable.
    /// @return contiguous iterator past the last element.
    iterator end() { return iterator(elems + N); }
    const_iterator end() const { return const_iterator(elems + N); }
    const_iterator cend() const { return const_iterator(elems + N); }

    /// @brief Returns a reverse contiguous iterator to the first element of the reversed *this. It corresponds to the
    /// last element of the non-reversed *this. If *this is empty, the returned iterator will be equal to end().
    /// @return Reverse contiguous iterator to the first element.
    reverse_iterator rbegin() { return reverse_iterator(elems + N); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(elems + N); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(elems + N); }

    /// @brief Returns a reverse contiguous iterator past the last element of the reversed *this. It corresponds to the
    /// element preceding the first element of the non-reversed *this. This returned iterator only acts as a sentinel.
    /// It is not guaranteed to be dereferenceable.
    /// @return Reverse contiguous iterator to the element following the last element.
    reverse_iterator rend() { return reverse_iterator(elems); }
    const_reverse_iterator rend() const { return const_reverse_iterator(elems); }
    const_reverse_iterator crend() const { return const_reverse_iterator(elems); }

    // ===== Capacity =====

    /// @brief Checks if the container has no elements.
    /// @return true if the container is empty, false otherwise.
    bool empty() const { return size() == 0; }

    /// @brief Returns the number of elements in the container.
    /// @return The number of elements in the container.
    std::size_t size() const { return N; }

    // ==== Operations ====

    /// @brief Assigns the value to all elements in the container.
    /// @param value The value to assign to the elements.
    void fill(const T& value);

    /// @brief Exchanges the contents of the container with those of other.
    /// Does not cause iterators and references to associate with the other container.
    /// @param other container to exchange the contents with
    void swap(array& other) noexcept(noexcept(std::swap(std::declval<T&>(), std::declval<T&>())));

    // ==== Comparison ====

    /// @brief Compare two arrays for equality.
    /// The equality comparison is performed by comparing the elements sequentially using operator==, stopping at the
    /// first mismatch. NOTE: T must be equality comparable and implement operator==
    /// @param other Other array
    /// @return True if this is equal to other, false otherwise.
    bool equals(const array& other) const;

    /// @brief Get if this array is less than other
    /// NOTE: T must be less than comparable and implement operator<
    /// @param other Other array
    /// @return True if this array is less thn other, false otherwise
    bool less_than(const array& other) const;

    /// @brief Equality operator
    /// NOTE: T must be equality comparable and implement operator==
    /// @param lhs left hand side
    /// @param rhs right had side
    /// @return True if lhs is equal to rhs, false otherwise.
    friend bool operator==(const array& lhs, const array& rhs) { return lhs.equals(rhs); }

    /// @brief Inequality operator
    /// NOTE: T must be equality comparable and implement operator==
    /// @param lhs left hand side iterator
    /// @param rhs right had side iterator
    /// @return True if lhs is not equal to rhs, false otherwise.
    friend bool operator!=(const array& lhs, const array& rhs) { return !lhs.equals(rhs); }

    /// @brief Less than operator
    /// NOTE: T must be less than comparable and implement operator<
    /// @param lhs left hand sid
    /// @param rhs right had side
    /// @return True if lhs is less than rhs, false otherwise.
    friend bool operator<(const array& lhs, const array& rhs) { return lhs.less_than(rhs); }

    /// @brief Greater than operator
    /// NOTE: T must be less than comparable and implement operator<
    /// @param lhs left hand side
    /// @param rhs right had side
    /// @return True if lhs is greater than rhs, false otherwise.
    friend bool operator>(const array& lhs, const array& rhs) { return rhs.less_than(lhs); }

    /// @brief Less than or equal to operator
    /// NOTE: T must be less than comparable and implement operator<
    /// @param lhs left hand side
    /// @param rhs right had side
    /// @return True if lhs is less than or equal to rhs, false otherwise.
    friend bool operator<=(const array& lhs, const array& rhs) { return !rhs.less_than(lhs); }

    /// @brief Greater than or equal to operator
    /// NOTE: T must be less than comparable and implement operator<
    /// @param lhs left hand side
    /// @param rhs right had side
    /// @return True if lhs is greater than or equal to rhs, false otherwise.
    friend bool operator>=(const array& lhs, const array& rhs) { return !lhs.less_than(rhs); }

    /// @brief Underlying array of type T
    T elems[N == 0 ? 1 : N];
};

// ==== Array Non Member functions ====

/// @brief Combines two arrays
/// @tparam M Size of the fhs Array
/// @param lhs Left hand side Array
/// @param rhs Right hand side Array
/// @return An Array size N + M with contents of lhs and then rhs
template <typename T, std::size_t N, std::size_t M>
array<T, N + M> operator+(const array<T, N>& lhs, const array<T, M>& rhs);

/// @brief Constructs a span viewing all N elements of the array `array`.
/// @details Because the size of the array (`N`) is known at compile time, the view returned will have a fixed size
///          which provides the following optimizations:
///          - The returned view's size() function can be determined at compile time (returns N)
///          - The returned view will have a sizeof equal to sizeof(T*)
/// @tparam T The class type stored in `array`
/// @tparam N The size of `array`
/// @param array array to view.
/// @return A view viewing the contents of `array` with static size N
template <typename T, std::size_t N>
span<T, N> make_span(array<T, N> array) noexcept {
    return span<T, N>(array.data());
}

// ===== Inline Array Implementation =====

template <typename T, std::size_t N>
inline void array<T, N>::fill(const T& value) {
    for (std::size_t i = 0; i < N; ++i) {
        elems[i] = value;
    }
}

template <typename T, std::size_t N>
inline void
array<T, N>::swap(array<T, N>& other) noexcept(noexcept(std::swap(std::declval<T&>(), std::declval<T&>()))) {
    for (std::size_t i = 0; i < N; ++i) {
        std::swap(elems[i], other.elems[i]);
    }
}

template <typename T, std::size_t N>
inline bool array<T, N>::equals(const array& other) const {
    return internal::contiguous_container::equals(*this, other);
}

template <typename T, std::size_t N>
inline bool array<T, N>::less_than(const array& other) const {
    return internal::contiguous_container::less_than(*this, other);
}

template <typename T, std::size_t N, std::size_t M>
inline array<T, N + M> operator+(const array<T, N>& lhs, const array<T, M>& rhs) {
    static_assert(N + M > 0, "Adding two arrays of size 0 is not allowed");
    array<T, N + M> combinedArray;
    for (std::size_t i = 0; i < N; ++i) {
        combinedArray[i] = lhs[i];
    }
    for (std::size_t i = 0; i < M; ++i) {
        combinedArray[i + N] = rhs[i];
    }

    return combinedArray;
}

} // namespace stdx

#endif
