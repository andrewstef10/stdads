#ifndef SPAN_H
#define SPAN_H

#include <cstddef>
#include <stdexcept>

namespace stdx {

/// @brief Compile time constant indicating that size should be determined dynamically at runtime.
///        Actual value is the maximum std::size_t value.
constexpr std::size_t DYNAMIC_SIZE = static_cast<std::size_t>(-1);

namespace internal {

/// @brief Holds span's element count when the size is known at compile time: stores nothing, the count lives in the
///        type. The constructor's count argument is a caller-asserted precondition (count == N) and is discarded.
template <std::size_t N>
class size_storage {
  public:
    constexpr explicit size_storage(std::size_t /*count*/) noexcept {}

    constexpr std::size_t size() const noexcept { return N; }
};

/// @brief Holds span's element count when the extent is dynamic: stores the count per object.
template <>
class size_storage<DYNAMIC_SIZE> {
  public:
    constexpr explicit size_storage(std::size_t count) noexcept : m_size(count) {}

    constexpr std::size_t size() const noexcept { return m_size; }

  private:
    std::size_t m_size;
};

} // namespace internal

/// @brief Non-owning view over a contiguous sequence of objects.
/// @tparam T Element type. Use span<const T> for read-only access.
/// @tparam N Number of elements in the sequence if known at compile time, otherwise default DYNAMIC_SIZE allows the
///           size to be determined at construction time.
template <typename T, std::size_t N = DYNAMIC_SIZE>
class span : private internal::size_storage<N> {
    using size_storage = internal::size_storage<N>;

  public:
    /// @brief Default Constructor. Constructs an empty span (data() == nullptr, size() == 0)
    /// @details This constructor is only enabled when template parameter N == DYNAMIC_SIZE or N == 0.
    ///          Default construction of a view where N > 0 is not allowed (size() must be 0).
    // The conditions below must depend on E, a parameter of the constructor template itself, rather than on N directly:
    // N is already known when the class is instantiated, so an unsatisfied condition on N alone is a hard error instead
    // of removing the constructor from overload resolution (SFINAE).
    template <std::size_t E = N, typename std::enable_if<E == DYNAMIC_SIZE || E == 0, int>::type = 0>
    constexpr span() noexcept : size_storage(0), m_data(nullptr) {}

    /// @brief Constructs a span viewing the first `count` elements of the buffer starting at `data`.
    /// @details `count` is a runtime value. As a result, a member variable for this view's size will be stored and
    ///          size() cannot be determined at compile time.
    ///          This constructor is only enabled when template parameter N == DYNAMIC_SIZE.
    /// @param data Pointer to the first element to view.
    /// @param count Number of elements in the buffer starting at `data`.
    template <std::size_t E = N, typename std::enable_if<E == DYNAMIC_SIZE, int>::type = 0>
    constexpr span(T* data, std::size_t count) noexcept : size_storage(count), m_data(data) {}

    /// @brief Constructs a span viewing the first `N` elements of the buffer starting at `data`.
    /// @details `N` is a compile time constant. As a result, no member variable for this view's size is needed and
    ///          size() can be determined at compile time. Results in sizeof(span) == sizeof(T*).
    ///          This constructor is only enabled when template parameter N != DYNAMIC_SIZE.
    /// @param data Pointer to the first element to view.
    template <std::size_t E = N, typename std::enable_if<E != DYNAMIC_SIZE, int>::type = 0>
    constexpr span(T* data) noexcept : size_storage(N), m_data(data) {}

    /// @brief Default copy constructor
    /// @param other Object to copy
    constexpr span(const span& other) noexcept = default;

    /// @brief Default move constructor
    /// @param other Object to move
    constexpr span(span&& other) noexcept = default;

    /// @brief Default destructor
    constexpr ~span() = default;

    /// @brief Default copy assignment operator
    /// @param other object to copy
    /// @return Reference to this
    constexpr span& operator=(const span& other) noexcept = default;

    /// @brief Default move assignment operator
    /// @param other object to move
    /// @return Reference to this
    constexpr span& operator=(span&& other) noexcept = default;

    // ==========================
    // ===== Element Access =====
    // ==========================

    /// @brief Returns a reference to the first element. Behavior is undefined if the span is empty.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reference to the first element.
    constexpr T& front() const { return m_data[0]; }

    /// @brief Returns a reference to the last element. Behavior is undefined if the span is empty.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reference to the last element.
    constexpr T& back() const { return m_data[size() - 1]; }

    /// @brief Returns a reference to the element at position `index` without bounds checking.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @param index Position of the element to return.
    /// @return Reference to the requested element.
    constexpr T& operator[](std::size_t index) const { return m_data[index]; }

    /// @brief Returns a reference to the element at position `index` with bounds checking.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @param index Position of the element to return.
    /// @return Reference to the requested element.
    /// @exception std::out_of_range If index >= size().
    constexpr T& at(std::size_t index) const;

    /// @brief Returns a pointer to the underlying element storage. The range [data(), data() + size()) is always valid.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          If the span is empty, data() may be nullptr and is not dereferenceable.
    /// @return Pointer to the first element.
    constexpr T* data() const noexcept { return m_data; }

    // ====================
    // ======= Size =======
    // ====================

    /// @brief Returns the number of elements in the container.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return The number of elements in the container.
    constexpr std::size_t size() const noexcept { return size_storage::size(); }

    /// @brief Returns the size of the sequence in bytes.
    /// @return size() * sizeof(element_type)
    constexpr std::size_t size_bytes() const noexcept { return size() * sizeof(T); }

    /// @brief Checks if the span has no elements. Equivalent to size() == 0.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return true if the span is empty, false otherwise.
    constexpr bool empty() const noexcept { return size() == 0; }

  private:
    /// @brief Pointer to a contiguous span of elements
    T* m_data;
};

/// @brief Constructs a span viewing all N elements of the c style array `array`.
/// @details Because the size of the array (`N`) is known at compile time, the view returned will have a fixed size
///          which provides the following optimizations:
///          - The returned view's size() function can be determined at compile time (returns N)
///          - The returned view will have a sizeof equal to sizeof(T*)
/// @tparam T The class type stored in `array`
/// @tparam N The size of `array`
/// @param array C style array to view.
/// @return A view viewing the contents of `array` with static size N
template <typename T, std::size_t N>
constexpr span<T, N> make_span(T (&array)[N]) noexcept {
    return span<T, N>(array);
}

/// region - Span Inline Implementation ///
template <typename T, std::size_t N>
constexpr T& span<T, N>::at(std::size_t index) const {
    if (index >= size()) {
        throw std::out_of_range("Index is outside the bounds of the container");
    }
    return m_data[index];
}
/// endregion - Span Inline Implementation ///

} // namespace stdx

#endif
