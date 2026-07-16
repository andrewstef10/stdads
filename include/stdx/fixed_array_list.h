#ifndef FIXED_ARRAY_LIST_H
#define FIXED_ARRAY_LIST_H

#include "stdx/internal/internal_contiguous_container.h"
#include <stdx/comparable.h>
#include <stdx/contiguous_container.h>
#include <stdx/equatable.h>
#include <stdx/memory.h>

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace stdx {

/**
 * @brief Fixed-capacity array list. Elements are stored contiguously in a stack-allocated buffer, giving O(1) random
 * access and O(1) append up to N elements.
 * @details The capacity is fixed at N elements at compile time. No heap memory is ever allocated or freed.
 *          All operations that would exceed the capacity throw std::length_error.
 * @tparam T Element type.
 * @tparam N Maximum number of elements the container can hold.
 */
template <typename T, std::size_t N>
class fixed_array_list : public stdx::contiguous_container<fixed_array_list<T, N>, T>,
                         public stdx::equatable<fixed_array_list<T, N>>,
                         public stdx::comparable<fixed_array_list<T, N>> {
  public:
    using size_type = typename container<fixed_array_list, T>::size_type;
    using reference = typename container<fixed_array_list, T>::reference;
    using const_reference = typename container<fixed_array_list, T>::const_reference;

    using iterator = typename contiguous_container<fixed_array_list, T>::iterator;
    using const_iterator = typename contiguous_container<fixed_array_list, T>::const_iterator;
    using reverse_iterator = typename contiguous_container<fixed_array_list, T>::reverse_iterator;
    using const_reverse_iterator = typename contiguous_container<fixed_array_list, T>::const_reverse_iterator;

    // ==== Constructors / Destructor ====

    /// @brief Default constructor. Constructs an empty container; no elements are live but the internal buffer is
    /// ready.
    /// @details Time:  O(1)
    ///          Space: O(1) — N * sizeof(T) bytes are always reserved on the stack regardless of element count
    fixed_array_list();

    /// @brief Copy constructor. Constructs a container with a copy of each element in `other`, in the same order.
    /// @details Time:  O(n), where n = other.size()
    ///          Space: O(1)
    /// @param other Container to copy from.
    fixed_array_list(const fixed_array_list& other);

    /// @brief Move constructor. Moves each element individually from `other`; the buffer cannot be transferred.
    ///        `other` is left in a valid empty state after the operation.
    /// @details Time:  O(n), where n = other.size() — elements are moved one-by-one since each object owns its own
    /// stack buffer
    ///          Space: O(1)
    ///          noexcept when T is nothrow move constructible, since every element is moved individually.
    /// @param other Container to move from.
    fixed_array_list(fixed_array_list&& other) noexcept(std::is_nothrow_move_constructible<T>::value);

    /// @brief Destructor. Destroys all live elements; the stack buffer is reclaimed with the object.
    /// @details Time:  O(n), where n = size()
    ///          Space: O(1)
    ~fixed_array_list();

    /// @brief Copy assignment operator. Replaces the contents with a copy of `other`.
    /// @details Time:  O(n), where n = max(size(), other.size())
    ///          Space: O(1) — no allocation is performed
    /// @param other Container to copy from.
    /// @return Reference to this container.
    fixed_array_list& operator=(const fixed_array_list& other);

    /// @brief Move assignment operator. Replaces the contents by moving each element from `other` into this container's
    /// buffer.
    ///        `other` is left in a valid empty state after the operation.
    /// @details Time:  O(n), where n = other.size() — elements are moved one-by-one since each object owns its own
    /// stack buffer
    ///          Space: O(1) — no allocation is performed
    ///          noexcept when T is nothrow move constructible and assignable: elements are move-assigned into
    ///          live slots and move-constructed into the remaining raw slots.
    /// @param other Container to move from.
    /// @return Reference to this container.
    fixed_array_list& operator=(fixed_array_list&& other) noexcept(std::is_nothrow_move_assignable<T>::value &&
                                                                   std::is_nothrow_move_constructible<T>::value);

    // ===== Element Access =====

    /// @brief Returns a pointer to the underlying element storage. The range [data(), data() + size()) is always valid.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          If the container is empty, data() may be nullptr and is not dereferenceable.
    /// @return Pointer to the first element, or nullptr if empty.
    T* data() { return reinterpret_cast<T*>(m_data); }
    const T* data() const { return reinterpret_cast<const T*>(m_data); }

    // ==== Capacity ====

    /// @brief Returns the number of elements in the container.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return The number of elements in the container.
    size_type size() const noexcept { return m_size; }

    /// @brief Returns the maximum number of elements this container can hold. Always equal to N.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          Overrides container::max_size() to reflect the compile-time fixed limit.
    /// @return N — the compile-time fixed capacity.
    size_type constexpr max_size() const noexcept { return static_cast<size_type>(N); }

    /// @brief Returns the number of elements this container can hold without throwing. Always equal to N.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return N — the compile-time fixed capacity.
    size_type constexpr capacity() const noexcept { return static_cast<size_type>(N); }

    // ==== Modifiers ====

    /// @brief Appends a copy of `val` to the end of the container.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          Invalidates the end() iterator.
    /// @param val Value to copy-construct into the new element.
    /// @exception std::length_error If size() == N.
    void push_back(const_reference val) { return emplace_back(val); }

    /// @brief Appends `val` to the end of the container by moving it.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          Invalidates the end() iterator.
    /// @param val Value to move-construct into the new element.
    /// @exception std::length_error If size() == N.
    void push_back(T&& val) { return emplace_back(std::move(val)); }

    /// @brief Constructs an element in-place at the end of the container, forwarding `args` to T's constructor.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          Invalidates the end() iterator.
    /// @param args Arguments to forward to the constructor of T.
    /// @exception std::length_error If size() == N.
    template <typename... Args>
    void emplace_back(Args&&... args);

    /// @brief Removes the last element.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          Invalidates the end() iterator and any reference to the last element.
    void pop_back();

    /// @brief Inserts a copy of `value` before `pos`, shifting elements after `pos` to the right.
    /// @details Time:  O(n) — shifts elements after pos to make room
    ///          Space: O(1) — no allocation is performed
    ///          Invalidates iterators and references from pos onward.
    /// @param pos Iterator before which the element is inserted. May be end().
    /// @param value Value to copy-construct into the new element.
    /// @return Iterator to the inserted element.
    /// @exception std::length_error If size() == N.
    iterator insert(const_iterator pos, const_reference value) { return emplace(pos, value); }

    /// @brief Inserts `value` before `pos` by moving it, shifting elements after `pos` to the right.
    /// @details Time:  O(n) — shifts elements after pos to make room
    ///          Space: O(1) — no allocation is performed
    ///          Invalidates iterators and references from pos onward.
    /// @param pos Iterator before which the element is inserted. May be end().
    /// @param value Value to move-construct into the new element.
    /// @return Iterator to the inserted element.
    /// @exception std::length_error If size() == N.
    iterator insert(const_iterator pos, T&& value) { return emplace(pos, std::move(value)); }

    /// @brief Constructs an element in-place before `pos`, forwarding `args` to T's constructor.
    /// @details Time:  O(n) — shifts elements after pos to make room
    ///          Space: O(1) — no allocation is performed
    ///          Invalidates iterators and references from pos onward.
    /// @param pos Iterator before which the element is constructed. May be end().
    /// @param args Arguments to forward to the constructor of T.
    /// @return Iterator to the emplaced element.
    /// @exception std::length_error If size() == N.
    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    /// @brief Resizes the container to contain count elements.
    /// @details Time:  O(n), where n is the difference between the current size and count
    ///          Space: O(1)
    ///          If the current size is equal to count, this function does nothing.
    ///          If the current size is greater than count, the container is reduced to its first count elements.
    ///          If the current size is less than count, then additional default-inserted elements are appended.
    ///          Invalidates all iterators and references if reallocation occurs; if count < size(), invalidates
    ///          iterators from count onward.
    /// @exception std::length_error if count > N
    /// @param count new size of the container
    void resize(size_type count) { resize_impl(count); }

    /// @brief Resizes the container to contain count elements.
    /// @details Time:  O(n), where n is the difference between the current size and count
    ///          Space: O(1)
    ///          If the current size is equal to count, this function does nothing.
    ///          If the current size is greater than count, the container is reduced to its first count elements.
    ///          If the current size is less than count, then additional default-inserted elements are appended.
    ///          Invalidates all iterators and references if reallocation occurs; if count < size(), invalidates
    ///          iterators from count onward.
    /// @exception std::length_error if count > N
    /// @param count new size of the container
    /// @param value the value to initialize the new elements with
    void resize(size_type count, const_reference value) { resize_impl(count, value); }

    /// @brief Swaps the contents of this container with `other` element-by-element.
    ///        Iterators remain valid but now refer to the other container's elements.
    /// @details Time:  O(n) — swaps elements one-by-one since each object owns its own stack buffer
    ///          Space: O(1)
    ///          noexcept when T is nothrow move constructible and assignable, since swapping is implemented with
    ///          the container's move operations.
    /// @param other Container to swap with.
    void swap(fixed_array_list& other) noexcept(std::is_nothrow_move_assignable<T>::value &&
                                                std::is_nothrow_move_constructible<T>::value);

    /// @brief Destroys all elements. size() becomes zero; capacity() is unchanged.
    /// @details Time:  O(n) — destructs each element
    ///          Space: O(1)
    ///          Invalidates all iterators and references to elements.
    void clear();

  private:
    /// @brief Current number of live elements in the container.
    size_type m_size;

    /// @brief Fixed sized storage for this container
    alignas(T) unsigned char m_data[sizeof(T) * (N == 0 ? 1 : N)];

    /// @brief Resizes the container to contain count elements.
    /// @details Time:  O(n), where n is the difference between the current size and count
    ///          Space: O(1)
    ///          If the current size is equal to count, this function does nothing.
    ///          If the current size is greater than count, the container is reduced to its first count elements.
    ///          If the current size is less than count, then additional default-inserted elements are appended.
    ///          Invalidates all iterators and references if reallocation occurs; if count < size(), invalidates
    ///          iterators from count onward.
    /// @exception std::length_error if count > N
    /// @param count new size of the container
    /// @param value the value to initialize the new elements with; leave blank for default initialization;
    template <typename... Args>
    void resize_impl(size_type count, Args&&... args);
};

// ===== Inline fixed_array_list Implementation =====

// m_data is raw storage: elements are placement-new'd into it on demand, so zero-initializing the bytes here
// would be pure overhead. NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
template <typename T, std::size_t N>
inline fixed_array_list<T, N>::fixed_array_list() : contiguous_container<fixed_array_list, T>(), m_size(0) {}

// m_data is raw storage: construct_copy below placement-news the live elements.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
template <typename T, std::size_t N>
inline fixed_array_list<T, N>::fixed_array_list(const fixed_array_list& other)
    : contiguous_container<fixed_array_list, T>(other), m_size(other.m_size) {
    // Copy construct the elements in other
    stdx::allocator<T> alloc;
    namespace icc = internal::contiguous_container;
    icc::construct_copy(alloc, data(), icc::make_view(other.data(), other.m_size));
}

// m_data is raw storage: construct_move below placement-news the live elements.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
template <typename T, std::size_t N>
inline fixed_array_list<T, N>::fixed_array_list(fixed_array_list&& other) noexcept(
    std::is_nothrow_move_constructible<T>::value)
    : contiguous_container<fixed_array_list, T>(std::move(other)), m_size(other.m_size) {
    // Move construct (steal) the elements in other
    stdx::allocator<T> alloc;
    namespace icc = internal::contiguous_container;
    icc::construct_move(alloc, data(), alloc, icc::make_view(other.data(), other.m_size));
    other.m_size = 0;
}

template <typename T, std::size_t N>
inline fixed_array_list<T, N>::~fixed_array_list() {
    clear();
}

template <typename T, std::size_t N>
inline fixed_array_list<T, N>& fixed_array_list<T, N>::operator=(const fixed_array_list& other) {
    if (this == &other) {
        return *this;
    }

    // Assign elements from other
    stdx::allocator<T> alloc;
    namespace icc = internal::contiguous_container;
    icc::assign_copy(alloc, icc::make_view(data(), m_size), icc::make_view(other.data(), other.m_size));

    m_size = other.m_size;
    return *this;
}

template <typename T, std::size_t N>
inline fixed_array_list<T, N>&
fixed_array_list<T, N>::operator=(fixed_array_list&& other) noexcept(std::is_nothrow_move_assignable<T>::value &&
                                                                     std::is_nothrow_move_constructible<T>::value) {
    if (this == &other) {
        return *this;
    }

    // Assign elements from other. Other keeps its (now empty) buffer
    stdx::allocator<T> alloc;
    namespace icc = internal::contiguous_container;
    icc::assign_move(alloc, icc::make_view(data(), m_size), alloc, icc::make_view(other.data(), other.m_size));

    m_size = other.m_size;
    other.m_size = 0;
    return *this;
}

template <typename T, std::size_t N>
template <typename... Args>
inline void fixed_array_list<T, N>::emplace_back(Args&&... args) {
    if (m_size == N) {
        throw std::length_error("Attempted to append to a full fixed_array_list");
    }
    stdx::allocator<T> alloc; // stateless allocator
    std::allocator_traits<stdx::allocator<T>>::construct(alloc, data() + m_size, std::forward<Args>(args)...);
    ++m_size;
}

template <typename T, std::size_t N>
inline void fixed_array_list<T, N>::pop_back() {
    stdx::allocator<T> alloc; // stateless allocator
    std::allocator_traits<stdx::allocator<T>>::destroy(alloc, data() + --m_size);
}

template <typename T, std::size_t N>
template <typename... Args>
inline typename fixed_array_list<T, N>::iterator fixed_array_list<T, N>::emplace(const_iterator pos, Args&&... args) {
    if (m_size == N) {
        throw std::length_error("Attempted to insert into a full fixed_array_list");
    }

    // Stateless allocator: allocator_traits routes construct through placement new, identical to construct().
    stdx::allocator<T> alloc;
    namespace icc = internal::contiguous_container;
    return icc::emplace_at(alloc, icc::make_view(data(), m_size++), static_cast<size_type>(pos - data()),
                           std::forward<Args>(args)...);
}

template <typename T, std::size_t N>
template <typename... Args>
inline void fixed_array_list<T, N>::resize_impl(size_type count, Args&&... args) {
    if (count > N) {
        throw std::length_error("Attempted to resize beyond the capacity of fixed_array_list");
    }

    stdx::allocator<T> alloc;
    namespace icc = internal::contiguous_container;
    icc::resize(alloc, icc::make_view(data(), m_size), count, std::forward<Args>(args)...);
    m_size = count;
}

template <typename T, std::size_t N>
inline void
fixed_array_list<T, N>::swap(fixed_array_list& other) noexcept(std::is_nothrow_move_assignable<T>::value &&
                                                               std::is_nothrow_move_constructible<T>::value) {
    // Move (not copy) through a temporary so swap works for move-only T and avoids a full copy
    fixed_array_list temp(std::move(*this));

    *this = std::move(other);
    other = std::move(temp);
}

template <typename T, std::size_t N>
inline void fixed_array_list<T, N>::clear() {
    while (m_size > 0) {
        pop_back();
    }
}
} // namespace stdx

#endif
