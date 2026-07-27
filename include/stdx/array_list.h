#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <stdx/comparable.h>
#include <stdx/contiguous_container.h>
#include <stdx/equatable.h>
#include <stdx/memory.h>

#include <cstddef>
#include <initializer_list>
#include <utility>

namespace stdx {

/**
 * @brief Dynamic-size array. Elements are stored contiguously, giving O(1) random access and amortized O(1) append.
 * @tparam T            Element type.
 * @tparam Allocator    Allocator type. Defaults to stdx::allocator<T>.
 * @tparam GrowthPolicy Policy controlling how capacity grows on reallocation. Defaults to stdx::doubling_growth.
 */
template <typename T, typename Allocator = stdx::allocator<T>, typename GrowthPolicy = stdx::doubling_growth<>>
class array_list : public stdx::contiguous_container<array_list<T, Allocator, GrowthPolicy>, T>,
                   public stdx::equatable<array_list<T, Allocator, GrowthPolicy>>,
                   public stdx::comparable<array_list<T, Allocator, GrowthPolicy>> {
  public:
    using iterator = typename contiguous_container<array_list, T>::iterator;
    using const_iterator = typename contiguous_container<array_list, T>::const_iterator;
    using reverse_iterator = typename contiguous_container<array_list, T>::reverse_iterator;
    using const_reverse_iterator = typename contiguous_container<array_list, T>::const_reverse_iterator;

    using allocator_type = Allocator;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

    using growth_type = GrowthPolicy;

    /// @brief Default constructor. Constructs an empty container with no elements and no allocated storage.
    /// @details Time:  O(1)
    ///          Space: O(1)
    array_list();

    /// @brief Copy constructor. Constructs a container with a copy of each element in `other`, in the same order.
    /// @details Time:  O(n), where n = other.size()
    ///          Space: O(n)
    /// @param other Container to copy from.
    array_list(const array_list& other);

    /// @brief Move constructor. Acquires ownership of the elements in `other`; `other` is left in a valid empty state.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @param other Container to move from.
    array_list(array_list&& other) noexcept;

    /// @brief Constructs an array_list with contents of the range [`first`, `last`).
    /// @tparam InputIterator Iterator type for `first` and `last`
    /// @param first Iterator defining the start of the range of elements to copy
    /// @param last  Iterator defining the end of the range of elements to copy
    template <typename InputIterator>
    array_list(InputIterator first, InputIterator last);

    /// @brief Constructs an array_list from an initializer list of elements
    /// @details Time: O(n) where n is the size of the initializer list
    ///          Space: O(1)
    /// @param init Initializer list to initialize the elements of the container with
    array_list(std::initializer_list<T> init);

    /// @brief Destructor. Destroys all elements and releases allocated memory.
    /// @details Time:  O(n), where n = size()
    ///          Space: O(1)
    ~array_list();

    /// @brief Copy assignment operator. Replaces the contents with a copy of `other`.
    /// @details Time:  O(n), where n = max(size(), other.size())
    ///          Space: O(n) if reallocation is required; O(1) otherwise
    /// @param other Container to copy from.
    /// @return Reference to this container.
    array_list& operator=(const array_list& other);

    /// @brief Move assignment operator. Replaces the contents by acquiring the elements of `other`.
    /// @details Time:  O(1) when allocators are equal or POCMA is true; O(n) otherwise, where n = other.size()
    ///          Space: O(1)
    ///          noexcept when the buffer-steal path is guaranteed (POCMA, or an always-equal allocator type);
    ///          the element-wise fallback may allocate and move elements, either of which can throw.
    /// @param other Container to move from.
    /// @return Reference to this container.
    array_list& operator=(array_list&& other) noexcept(
        std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value ||
        internal::alloc_is_always_equal<Allocator>::type::value);

    /// @brief Replaces the contents of the container.
    /// @details Time: o(n) where n == count
    ///          Space: TODO
    /// @param count The new size of the container
    /// @param value The value to initialize elements of the container with
    void assign(std::size_t count, const T& value);

    /// @brief Replaces the contents with copies of those in the range [first, last).
    /// @details Time: O(n) where n is the distance between `first` and `last`
    ///          Space: TODO
    ///          If either argument is an iterator into *this, the behavior is undefined.
    /// @tparam InputIt Iterator type
    /// @param first first iterator defining the source range of elements to copy
    /// @param last  last iterator defining the source range of elements to copy
    template <class InputIt>
    void assign(InputIt first, InputIt last);

    /// @brief Returns the allocator associated with the container.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return The associated allocator.
    allocator_type get_allocator() const noexcept { return m_alloc; }

    /// @brief Returns the growth policy associated with the container.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return The associated growth policy.
    growth_type get_growth_policy() const noexcept { return m_growth; }

    // ===== Element Access =====

    /// @brief Returns a pointer to the underlying element storage. The range [data(), data() + size()) is always valid.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          If the container is empty, data() may be nullptr and is not dereferenceable.
    /// @return Pointer to the first element, or nullptr if empty.
    T* data() { return m_data; }
    const T* data() const { return m_data; }

    // ===== Capacity =====

    /// @brief Returns the number of elements in the container.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return The number of elements in the container.
    std::size_t size() const noexcept { return m_size; }

    /// @brief Returns the number of elements the container can hold without reallocating.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Current allocated capacity in number of elements.
    std::size_t capacity() const { return m_capacity; }

    /// @brief Requests that the array_list capacity be at least enough to contain `n` elements.
    /// @details Time:  O(n) — moves all existing elements if reallocation occurs
    ///          Space: O(n) — allocates a new buffer of size n when growing
    ///          If n is greater than the current array_list capacity, the function causes the container to reallocate
    ///          its storage increasing its capacity to n (or greater). In all other cases, the function call does not
    ///          cause a reallocation and the array_list capacity is not affected. Invalidates all iterators and
    ///          references if reallocation occurs.
    /// @param n Minimum required capacity.
    void reserve(std::size_t n);

    /// @brief Requests the removal of unused capacity by reallocating the buffer to exactly size() elements. No-op if
    /// capacity() == size().
    /// @details Time:  O(n) — moves all existing elements if reallocation occurs
    ///          Space: O(n) — allocates a new buffer of size size() when shrinking
    ///          Invalidates all iterators and references if reallocation occurs.
    void shrink_to_fit();

    // ==== Modifiers ====

    /// @brief Appends a copy of `val` to the end of the container. Reallocates if size() == capacity().
    /// @details Time:  O(1) amortized — O(n) on reallocation
    ///          Space: O(1) amortized — O(n) on reallocation
    ///          Invalidates all iterators and references if reallocation occurs.
    /// @param val Value to copy-construct into the new element.
    void push_back(const T& val) { return emplace_back(val); }

    /// @brief Appends `val` to the end of the container by moving it. Reallocates if size() == capacity().
    /// @details Time:  O(1) amortized — O(n) on reallocation
    ///          Space: O(1) amortized — O(n) on reallocation
    ///          Invalidates all iterators and references if reallocation occurs.
    /// @param val Value to move-construct into the new element.
    void push_back(T&& val) { return emplace_back(std::move(val)); }

    /// @brief Constructs an element in-place at the end of the container, forwarding `args` to T's constructor.
    /// @details Time:  O(1) amortized — O(n) on reallocation
    ///          Space: O(1) amortized — O(n) on reallocation
    ///          Invalidates all iterators and references if reallocation occurs.
    /// @param args Arguments to forward to the constructor of T.
    template <typename... Args>
    void emplace_back(Args&&... args);

    /// @brief Removes the last element. Behavior is undefined if the container is empty.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          Invalidates the end() iterator and any reference to the last element.
    void pop_back();

    /// @brief Inserts a copy of `value` before `pos`. Reallocates if size() == capacity().
    /// @details Time:  O(n) — shifts elements after pos to make room
    ///          Space: O(1) amortized — O(n) on reallocation
    ///          Invalidates all iterators and references if reallocation occurs; otherwise invalidates from pos onward.
    /// @param pos Iterator before which the element is inserted. May be end().
    /// @param value Value to copy-construct into the new element.
    /// @return Iterator to the inserted element.
    iterator insert(const_iterator pos, const T& value) { return emplace(pos, value); }

    /// @brief Inserts `value` before `pos` by moving it. Reallocates if size() == capacity().
    /// @details Time:  O(n) — shifts elements after pos to make room
    ///          Space: O(1) amortized — O(n) on reallocation
    ///          Invalidates all iterators and references if reallocation occurs; otherwise invalidates from pos onward.
    /// @param pos Iterator before which the element is inserted. May be end().
    /// @param value Value to move-construct into the new element.
    /// @return Iterator to the inserted element.
    iterator insert(const_iterator pos, T&& value) { return emplace(pos, std::move(value)); }

    /// @brief Constructs an element in-place before `pos`, forwarding `args` to T's constructor.
    /// @details Time:  O(n) — shifts elements after pos to make room
    ///          Space: O(1) amortized — O(n) on reallocation
    ///          Invalidates all iterators and references if reallocation occurs; otherwise invalidates from pos onward.
    /// @param pos Iterator before which the element is constructed. May be end().
    /// @param args Arguments to forward to the constructor of T.
    /// @return Iterator to the emplaced element.
    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    /// @brief Resizes the container to contain count elements.
    /// @details Time:  O(n), where n is the difference between the current size and count; O(n) where n is the current
    /// size on reallocation;
    ///          Space: O(1); O(n) on reallocation;
    ///          If the current size is equal to count, this function does nothing.
    ///          If the current size is greater than count, the container is reduced to its first count elements.
    ///          If the current size is less than count, then additional default-inserted elements are appended.
    ///          Invalidates all iterators and references if reallocation occurs; if count < size(), invalidates
    ///          iterators from count onward.
    /// @param count new size of the container
    void resize(std::size_t count) { resize_impl(count); }

    /// @brief Resizes the container to contain count elements.
    /// @details Time:  O(n), where n is the difference between the current size and count; O(n) where n is the current
    /// size on reallocation;
    ///          Space: O(1); O(n) on reallocation;
    ///          If the current size is equal to count, this function does nothing.
    ///          If the current size is greater than count, the container is reduced to its first count elements.
    ///          If the current size is less than count, then additional copies of value are appended.
    ///          Invalidates all iterators and references if reallocation occurs; if count < size(), invalidates
    ///          iterators from count onward.
    /// @param count new size of the container
    /// @param value the value to initialize the new elements with
    void resize(std::size_t count, const T& value) { resize_impl(count, value); }

    /// @brief Swaps the contents of this container with `other`. Iterators remain valid but now refer to the other
    /// container.
    /// @details Time:  O(1)
    ///          Space: O(1)
    ///          noexcept when POCS is true or the allocator type is always-equal; otherwise swapping containers
    ///          with unequal allocators is undefined behavior, matching std::vector.
    /// @param other Container to swap with.
    void swap(array_list& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
                                          internal::alloc_is_always_equal<Allocator>::type::value);

    /// @brief Destroys all elements. size() becomes zero; capacity() is unchanged.
    /// @details Time:  O(n) — destructs each element
    ///          Space: O(1)
    ///          Invalidates all iterators and references to elements.
    void clear();

  private:
    /// @brief Current number of elements stored in the container
    std::size_t m_size;

    /// @brief Pointer to underlying array of elements
    T* m_data;

    /// @brief Size of the allocated storage capacity
    std::size_t m_capacity;

    /// @brief Allocator for this array_list
    allocator_type m_alloc;

    /// @brief Growth policy for this data structure
    growth_type m_growth;

    /// @brief Constructs a T at `location` from `args`, routed through the allocator. Hook used by the
    ///        shared insert helpers in contiguous_container. Perfect-forwards, so it copy-, move-, or
    ///        emplace-constructs with no extra overhead.
    /// @param location Raw storage to construct into.
    /// @param args     Arguments forwarded to T's constructor.
    template <typename... Args>
    void construct(T* location, Args&&... args) {
        std::allocator_traits<Allocator>::construct(m_alloc, location, std::forward<Args>(args)...);
    }

    /// @brief Destroys the T at `location` through the allocator.
    /// @param location Pointer to the live object to destroy.
    void destroy(T* location) { std::allocator_traits<Allocator>::destroy(m_alloc, location); }

    /// @brief Allocates a buffer of `new_capacity` elements, moves all existing elements into it, and releases the old
    /// buffer.
    /// @details Time:  O(n) — moves all existing elements into the new buffer
    ///          Space: O(n) — allocates a new buffer of new_capacity elements
    ///          Uses std::move_if_noexcept, so elements are copied if T's move constructor may throw.
    ///          Updates m_data and m_capacity; m_size is unchanged.
    ///          Invalidates all iterators and references.
    /// @param newCapacity Target size of the new buffer. Must be >= size().
    void realloc(std::size_t newCapacity);

    /// @brief Reallocating insert: grows the buffer and places a new element at `index` in a single pass.
    /// @details Time:  O(n) — allocates a new buffer and moves all existing elements exactly once
    ///          Space: O(n) — allocates a new buffer sized by the growth policy
    ///          Each existing element is moved exactly once (no separate grow-then-shift). The new element is
    ///          constructed first, so a throwing element constructor leaves the container unchanged.
    ///          Invalidates all iterators and references.
    /// @param index Position at which the new element is constructed (0..m_size).
    /// @param args  Arguments forwarded to T's constructor for the new element.
    /// @return Iterator to the newly constructed element.
    template <typename... Args>
    iterator emplace_realloc(std::size_t index, Args&&... args);

    /// @brief Resizes the container to contain count elements.
    /// @details Time:  O(n), where n is the difference between the current size and count; O(n) where n is the current
    /// size on reallocation;
    ///          Space: O(1); O(n) on reallocation;
    ///          If the current size is equal to count, this function does nothing.
    ///          If the current size is greater than count, the container is reduced to its first count elements.
    ///          If the current size is less than count, then additional copies of value are appended.
    ///          Invalidates all iterators and references if reallocation occurs; if count < size(), invalidates
    ///          iterators from count onward.
    /// @param count new size of the container
    /// @param value the value to initialize the new elements with; leave blank for default initialization;
    template <typename... Args>
    void resize_impl(std::size_t count, Args&&... args);
};

// ===== Inline array_list Implementation =====

template <typename T, typename Allocator, typename GrowthPolicy>
inline array_list<T, Allocator, GrowthPolicy>::array_list()
    : contiguous_container<array_list, T>(), m_size(0), m_data(nullptr), m_capacity(0), m_alloc(), m_growth() {}

template <typename T, typename Allocator, typename GrowthPolicy>
inline array_list<T, Allocator, GrowthPolicy>::array_list(const array_list<T, Allocator, GrowthPolicy>& other)
    : contiguous_container<array_list, T>(other)
    , m_size(0)
    , m_data(nullptr)
    , m_capacity(0)
    , m_alloc(other.m_alloc)
    , m_growth(other.m_growth) {

    // Grow capacity to at least the size of other if needed
    if (m_capacity < other.m_size) {
        realloc(m_growth(m_capacity, other.m_size));
    }

    // Copy construct the all elements in other
    namespace icc = internal::contiguous_container;
    icc::construct_copy(m_alloc, m_data, icc::make_view(other.data(), other.m_size));
    m_size = other.m_size;
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline array_list<T, Allocator, GrowthPolicy>::array_list(array_list<T, Allocator, GrowthPolicy>&& other) noexcept
    : contiguous_container<array_list, T>(std::move(other))
    , m_size(other.m_size)
    , m_data(other.m_data)
    , m_capacity(other.m_capacity)
    , m_alloc(std::move(other.m_alloc))
    , m_growth(std::move(other.m_growth)) {

    other.m_size = 0;
    other.m_data = nullptr;
    other.m_capacity = 0;
}

template <typename T, typename Allocator, typename GrowthPolicy>
template <typename InputIterator>
inline array_list<T, Allocator, GrowthPolicy>::array_list(InputIterator first, InputIterator last)
    : contiguous_container<array_list, T>(), m_size(0), m_data(nullptr), m_capacity(0), m_alloc(), m_growth() {

    const std::size_t SIZE = std::distance(first, last);

    // Grow capacity to at least the size of the init list if needed
    if (m_capacity < SIZE) {
        realloc(m_growth(m_capacity, SIZE));
    }

    // Copy construct the all elements in other
    namespace icc = internal::contiguous_container;
    icc::construct_copy(m_alloc, m_data, icc::make_view(first, SIZE));
    m_size = SIZE;
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline array_list<T, Allocator, GrowthPolicy>::array_list(std::initializer_list<T> init)
    : contiguous_container<array_list, T>(), m_size(0), m_data(nullptr), m_capacity(0), m_alloc(), m_growth() {
    // Grow capacity to at least the size of the init list if needed
    if (m_capacity < init.size()) {
        realloc(m_growth(m_capacity, init.size()));
    }

    // Copy construct the all elements in other
    namespace icc = internal::contiguous_container;
    icc::construct_copy(m_alloc, m_data, icc::make_view(init.begin(), init.size()));
    m_size = init.size();
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline array_list<T, Allocator, GrowthPolicy>::~array_list() {
    clear();
    std::allocator_traits<Allocator>::deallocate(m_alloc, m_data, m_capacity);
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline array_list<T, Allocator, GrowthPolicy>&
array_list<T, Allocator, GrowthPolicy>::operator=(const array_list<T, Allocator, GrowthPolicy>& other) {
    if (this == &other) {
        return *this;
    }

    if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
        // Allocator should be copied. Need to deallocate our own memory and reallocate with a copy of other's
        // allocator.
        clear();
        std::allocator_traits<Allocator>::deallocate(m_alloc, m_data, m_capacity);
        m_capacity = 0;
        m_alloc = other.m_alloc;
    }

    // Grow capacity to at least the size of other if needed
    if (m_capacity < other.m_size) {
        realloc(m_growth(m_capacity, other.m_size));
    }

    // Assign elements from other
    namespace icc = internal::contiguous_container;
    icc::assign_copy(m_alloc, icc::make_view(m_data, m_size), icc::make_view(other.data(), other.m_size));

    m_size = other.m_size;
    return *this;
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline array_list<T, Allocator, GrowthPolicy>&
array_list<T, Allocator, GrowthPolicy>::operator=(array_list<T, Allocator, GrowthPolicy>&& other) noexcept(
    std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value ||
    internal::alloc_is_always_equal<Allocator>::type::value) {
    if (this == &other) {
        return *this;
    }

    constexpr bool POCMA = std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value;
    if (POCMA || m_alloc == other.m_alloc) {
        // Either our allocator and the memory our allocator allocates need to stay together (if POCMA) OR allocators
        // are equal Its safe to steal other's buffer and deallocate our own
        clear();
        std::allocator_traits<Allocator>::deallocate(m_alloc, m_data, m_capacity);

        if (POCMA) {
            m_alloc = std::move(other.m_alloc);
        }

        m_data = other.m_data;
        m_capacity = other.m_capacity;
        other.m_data = nullptr;
        other.m_capacity = 0;
    } else {
        // POCMA false AND allocators are not equal
        // We must perform element-wise move into own storage, O(n)
        if (m_capacity < other.m_size) {
            realloc(m_growth(m_capacity, other.m_size));
        }

        // Assign elements from other. Other keeps its (now empty) buffer: only its own
        // allocator may deallocate it, which happens in other's destructor.
        namespace icc = internal::contiguous_container;
        icc::assign_move(m_alloc, icc::make_view(m_data, m_size), other.m_alloc,
                         icc::make_view(other.m_data, other.m_size));
    }

    m_size = other.m_size;
    other.m_size = 0;
    return *this;
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline void array_list<T, Allocator, GrowthPolicy>::assign(std::size_t count, const T& value) {
    if (m_capacity < count) {
        // resize and construct elements in same loop

        clear();
        realloc(m_growth(m_capacity, count));
    }

    namespace icc = internal::contiguous_container;
    icc::assign_copy(m_alloc, icc::make_view(m_data, m_size), count, value);

    m_size = count;
}

template <typename T, typename Allocator, typename GrowthPolicy>
template <class InputIt>
inline void array_list<T, Allocator, GrowthPolicy>::assign(InputIt first, InputIt last) {
    const std::size_t COUNT = std::distance(first, last);
    if (m_capacity < COUNT) {
        clear();
        realloc(m_growth(m_capacity, COUNT));
    }

    namespace icc = internal::contiguous_container;
    icc::assign_copy(m_alloc, icc::make_view(m_data, m_size), first, COUNT);

    m_size = COUNT;
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline void array_list<T, Allocator, GrowthPolicy>::reserve(std::size_t n) {
    if (m_capacity < n) {
        realloc(m_growth(m_capacity, n));
    }
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline void array_list<T, Allocator, GrowthPolicy>::shrink_to_fit() {
    if (m_capacity > m_size) {
        realloc(m_size);
    }
}

template <typename T, typename Allocator, typename GrowthPolicy>
template <typename... Args>
inline void array_list<T, Allocator, GrowthPolicy>::emplace_back(Args&&... args) {
    if (m_size == m_capacity) {
        realloc(m_growth(m_capacity, m_size + 1));
    }

    construct(m_data + m_size, std::forward<Args>(args)...);
    ++m_size;
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline void array_list<T, Allocator, GrowthPolicy>::pop_back() {
    destroy(m_data + --m_size);
}

template <typename T, typename Allocator, typename GrowthPolicy>
template <typename... Args>
inline typename array_list<T, Allocator, GrowthPolicy>::iterator
array_list<T, Allocator, GrowthPolicy>::emplace(const_iterator pos, Args&&... args) {
    const std::size_t INDEX = static_cast<std::size_t>(pos - m_data);
    if (m_size == m_capacity) {
        return emplace_realloc(INDEX, std::forward<Args>(args)...);
    }

    namespace icc = internal::contiguous_container;
    return icc::emplace_at(m_alloc, icc::make_view(m_data, m_size++), INDEX, std::forward<Args>(args)...);
}

template <typename T, typename Allocator, typename GrowthPolicy>
template <typename... Args>
inline void array_list<T, Allocator, GrowthPolicy>::resize_impl(std::size_t count, Args&&... args) {
    if (count > m_capacity) {
        realloc(m_growth(m_capacity, count));
    }

    namespace icc = internal::contiguous_container;
    icc::resize(m_alloc, icc::make_view(m_data, m_size), count, std::forward<Args>(args)...);
    m_size = count;
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline void array_list<T, Allocator, GrowthPolicy>::swap(array_list<T, Allocator, GrowthPolicy>& other) noexcept(
    std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
    internal::alloc_is_always_equal<Allocator>::type::value) {
    // Always swap storage: each pointer/size/capacity pair must travel together.
    T* tempData = other.m_data;
    std::size_t tempSize = other.m_size;
    std::size_t tempCapacity = other.m_capacity;

    other.m_data = m_data;
    other.m_size = m_size;
    other.m_capacity = m_capacity;

    m_data = tempData;
    m_size = tempSize;
    m_capacity = tempCapacity;

    // Swap allocators when POCS is true so each allocator stays paired with the memory it owns.
    if (std::allocator_traits<Allocator>::propagate_on_container_swap::value) {
        using std::swap; // allows compiler to check allocator's own namespace via ADL first, then falls back to
                         // std::swap if needed
        swap(m_alloc, other.m_alloc);
    }
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline void array_list<T, Allocator, GrowthPolicy>::realloc(std::size_t newCapacity) {
    T* newData = std::allocator_traits<Allocator>::allocate(m_alloc, newCapacity);

    // Copy container contents to new memory (newData)
    for (size_t i = 0; i < m_size; ++i) {
        construct(newData + i, std::move_if_noexcept(m_data[i]));
        destroy(m_data + i);
    }

    // Deallocate old memory (m_data) and update members
    std::allocator_traits<Allocator>::deallocate(m_alloc, m_data, m_capacity);
    m_data = newData;
    m_capacity = newCapacity;
}

template <typename T, typename Allocator, typename GrowthPolicy>
template <typename... Args>
inline typename array_list<T, Allocator, GrowthPolicy>::iterator
array_list<T, Allocator, GrowthPolicy>::emplace_realloc(std::size_t index, Args&&... args) {
    const std::size_t NEW_CAPACITY = m_growth(m_capacity, m_size + 1);
    T* newData = std::allocator_traits<Allocator>::allocate(m_alloc, NEW_CAPACITY);

    // Construct the new element first so a throwing T constructor leaves *this unchanged.
    try {
        construct(newData + index, std::forward<Args>(args)...);
    } catch (...) {
        std::allocator_traits<Allocator>::deallocate(m_alloc, newData, NEW_CAPACITY);
        throw;
    }

    // Move existing elements into the new buffer around the gap; each element is moved exactly once.
    std::size_t i = 0;
    for (; i < index; ++i) {
        construct(newData + i, std::move_if_noexcept(m_data[i]));
        destroy(m_data + i);
    }
    for (; i < m_size; ++i) {
        construct(newData + i + 1, std::move_if_noexcept(m_data[i]));
        destroy(m_data + i);
    }

    // Deallocate old memory and update members
    std::allocator_traits<Allocator>::deallocate(m_alloc, m_data, m_capacity);
    m_data = newData;
    m_capacity = NEW_CAPACITY;
    ++m_size;
    return iterator(m_data + index);
}

template <typename T, typename Allocator, typename GrowthPolicy>
inline void array_list<T, Allocator, GrowthPolicy>::clear() {
    while (m_size > 0) {
        pop_back();
    }
}
} // namespace stdx

#endif
