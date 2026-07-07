#ifndef MEMORY_H
#define MEMORY_H

#include <cstddef>
#include <memory>
#include <new>
#include <stdexcept>

#include <stdx/equatable.h>

namespace stdx {

/// @brief Growth policy that returns the exact requested capacity.
/// Use this policy when you want the array_list to grow to exactly the size needed,
/// without any overallocation. This minimizes memory usage but may cause more
/// frequent reallocations as the array_list grows.
/// @tparam size_type The unsigned integer type used for capacity values.
///                   Defaults to std::size_t.
template <typename size_type = std::size_t>
struct exact_growth {
    /// @brief Returns the required capacity without any growth factor.
    /// @param current_capacity The current capacity (ignored by this policy).
    /// @param required_capacity The minimum capacity needed to accommodate new elements.
    /// @return The required_capacity unchanged, ensuring no overallocation.
    size_type operator()(size_type /*current_capacity*/, size_type required_capacity) const {
        return required_capacity;
    }
};

/// @brief Growth policy that doubles capacity each time it needs to grow
/// This policy provides good performance characteristics by reducing frequency
/// of reallocations while maintaining reasonable memory usage. It doubles the
/// current capacity until it meets or exceeds the required size.
/// @tparam size_type size_type The unsigned integer type used for capacity values.
///                   Defaults to std::size_t.
template <typename size_type = std::size_t>
struct doubling_growth {
    /// @brief Computes new capacity by doubling until it meets or exceeds required
    /// @param current_capacity The current allocated capacity
    /// @param required_capacity The minimum capacity needed for new elements
    /// @return size_type The new capacity which will always be >= required_capacity
    size_type operator()(size_type current_capacity, size_type required_capacity) const {
        if (current_capacity < required_capacity) {
            current_capacity = current_capacity == 0 ? 1 : current_capacity * 2;
            while (current_capacity < required_capacity) {
                current_capacity *= 2;
            }
        }

        return current_capacity;
    }
};

/// @brief Growth policy for fixed-capacity containers. Always allocates all N slots on the first growth; throws
/// std::length_error if the required capacity exceeds N.
/// @details Returning N even for small requests ensures the backing fixed_allocator's buffer is claimed in one shot,
///          which avoids the self-aliasing undefined behaviour that would occur if the same pointer were returned
///          by successive smaller allocations while elements are already present.
/// @tparam N           The maximum number of elements this container may ever hold.
/// @tparam size_type   The unsigned integer type used for capacity values. Defaults to std::size_t.
template <std::size_t N, typename size_type = std::size_t>
struct no_growth {
    /// @brief Returns N for any request that fits; throws std::length_error otherwise.
    /// @param current_capacity The current allocated capacity (unused).
    /// @param required_capacity The minimum capacity needed for the pending operation.
    /// @return N — the fixed capacity of the container.
    /// @exception std::length_error If required_capacity > N.
    size_type operator()(size_type /*currentCapacity*/, size_type requiredCapacity) const {
        if (requiredCapacity > static_cast<size_type>(N)) {
            throw std::length_error("container cannot grow beyond fixed capacity");
        }
        return static_cast<size_type>(N);
    }
};

template <typename T>
class allocator {
  public:
    using value_type = T;

    /// @brief True if any two instances of this allocator can free each other's memory.
    ///        If is_always_equal is true, then comparing any two containers of this type (operator==) should also
    ///        return true.
    /// @details True here because stdx::allocator is stateless and all instances are are considered equal.
    using is_always_equal = std::true_type;

    /// @brief True if this allocator object should itself be copy-assigned into the destination container during
    /// container copy assignment.
    /// @details False here because stdx::allocator is stateless. There is no internal state to transfer.
    using propagate_on_container_copy_assignment = std::false_type;

    /// @brief True if this allocator object should itself be move-assigned into the destination container during
    /// container move assignment.
    /// @details False here because stdx::allocator is stateless. There is no internal state to transfer.
    using propagate_on_container_move_assignment = std::false_type;

    /// @brief True if this allocator object should itself be swapped when two containers are swapped.
    /// @details False here because stdx::allocator is stateless. There is no internal state to swap.
    ///          If false and allocators are not equal, swapping containers is undefined behavior because
    ///          each container would hold memory it cannot legally free with its own allocator.
    using propagate_on_container_swap = std::false_type;

    // No longer required c++11+
    // pointer
    // const_pointer
    // reference
    // const_reference
    // size_type
    // difference_type

    // construct()
    // destroy()

    // rebind
    // address()
    // max_size()

    /// @brief Default constructor
    allocator() = default;

    /// @brief Copy constructor
    /// @param other allocator to copy.
    allocator(const allocator& other) = default;

    /// @brief Move constructor
    /// @param other allocator to move.
    allocator(allocator&& other) = default;

    /// @brief Destructor.
    ~allocator() = default;

    /// @brief Copy assignment operator
    /// @param other allocator to copy
    /// @return Reference to this object
    allocator& operator=(const allocator& other) = default;

    /// @brief Move assignment operator
    /// @param other allocator to move
    /// @return Reference to this object
    allocator& operator=(allocator&& other) = default;

    /// @brief Copy constructor
    /// @tparam U Other allocator's type
    /// @param
    template <typename U>
    allocator(const allocator<U>&) noexcept {}

    /// @brief Allocates n * sizeof(T) bytes of uninitialized storage by calling ::operator new(std::size_t) or
    /// ::operator new(std::size_t, std::align_val_t)(since C++17), but it is unspecified when and how this function is
    /// called. Then, this function creates an array of type T[n] in the storage and starts its lifetime, but does not
    /// start lifetime of any of its elements. Use of this function is ill-formed if T is an incomplete type.
    /// @param n The number of objects to allocate storage for
    /// @return Pointer to the first element of an array of n objects of type T whose elements have not been constructed
    /// yet.
    T* allocate(std::size_t n);

    /// @brief Deallocates the storage referenced by the pointer p, which must be a pointer obtained by an earlier call
    /// to allocate() or allocate_at_least(). Calls ::operator delete(void*) or ::operator delete(void*,
    /// std::align_val_t)(since C++17), but it is unspecified when and how it is called.
    /// @param ptr
    /// @param n
    void deallocate(T* ptr, std::size_t n) noexcept;

    /// @brief Returns the maximum theoretically possible value of n, for which the call allocate(n) could succeed.
    /// @return The maximum supported allocation size.
    constexpr std::size_t max_size() const noexcept;

    /// @brief Compares two allocators
    /// @tparam U Other allocator's type
    /// @param
    /// @return true always because allocators of any type T are equal
    template <typename U>
    bool constexpr equals(const allocator<U>&) const noexcept {
        return true;
    }

    /// @brief Equality operator
    /// @tparam U Other allocator's type
    /// @param lhs left hand side
    /// @param rhs right hand side
    /// @return true always because allocators of any type T are equal
    template <typename U>
    friend constexpr bool operator==(const allocator<T>& lhs, const allocator<U>& rhs) noexcept {
        return lhs.equals(rhs);
    }

    /// @brief Inequality operator
    /// @tparam U Other allocator's type
    /// @param lhs left hand side iterator
    /// @param rhs right hand side iterator
    /// @return true always because allocators of any type T are equal
    template <typename U>
    friend constexpr bool operator!=(const allocator<T>& lhs, const allocator<U>& rhs) noexcept {
        return !lhs.equals(rhs);
    }
};

// ===== Inline allocator Implementation =====

template <typename T>
inline T* allocator<T>::allocate(std::size_t n) {
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

template <typename T>
inline void allocator<T>::deallocate(T* ptr, std::size_t) noexcept {
    // #if __cpp_sized_deallocation
    //         ::operator delete(ptr, n * sizeof(T));
    // #else
    //         ::operator delete(ptr);
    // #endif
    ::operator delete(ptr);
}

template <typename T>
inline constexpr std::size_t allocator<T>::max_size() const noexcept {
    return static_cast<std::size_t>(-1) / sizeof(T);
}

// template<typename T, std::size_t N>
// class fixed_allocator : stdx::equatable<fixed_allocator<T, N>>
// {
// public:

//     using value_type = T;

//     /// @brief True if any two instances of this allocator can free each other's memory.
//     ///        If is_always_equal is true, then comparing any two containers of this type (operator==) should also
//     return true.
//     /// @details False here because stdx::fixed_allocator is statefull. Each stdx::fixed_allocator owns its own fixed
//     amount of memory for the life of the object. using is_always_equal = std::false_type;

//     /// @brief True if this allocator object should itself be copy-assigned into the destination container during
//     container copy assignment.
//     /// @details True here because stdx::fixed_allocator is statefull. Each stdx::fixed_allocator owns its own fixed
//     amount of memory for the life of the object. using propagate_on_container_copy_assignment = std::true_type;

//     /// @brief True if this allocator object should itself be move-assigned into the destination container during
//     container move assignment.
//     /// @details True here because stdx::fixed_allocator is statefull. Each stdx::fixed_allocator owns its own fixed
//     amount of memory for the life of the object. using propagate_on_container_move_assignment = std::true_type;

//     /// @brief True if this allocator object should itself be swapped when two containers are swapped.
//     /// @details True here because stdx::fixed_allocator is statefull. Each stdx::fixed_allocator owns its own fixed
//     amount of memory for the life of the object.
//     ///          If false and allocators are not equal, swapping containers is undefined behavior because
//     ///          each container would hold memory it cannot legally free with its own allocator.
//     using propagate_on_container_swap = std::true_type;

//     // No longer required c++11+
//     // pointer
//     // const_pointer
//     // reference
//     // const_reference
//     // size_type
//     // difference_type

//     // construct()
//     // destroy()

//     // rebind
//     // address()
//     // max_size()

//     /// @brief Default constructor
//     fixed_allocator() = default;

//     /// @brief Copy constructor
//     /// @param other allocator to copy.
//     fixed_allocator(const fixed_allocator& other) = default;

//     /// @brief Move constructor
//     /// @param other allocator to move.
//     fixed_allocator(fixed_allocator&& other) = default;

//     /// @brief Destructor.
//     ~fixed_allocator() = default;

//     /// @brief Copy assignment operator
//     /// @param other allocator to copy
//     /// @return Reference to this object
//     fixed_allocator& operator=(const fixed_allocator& other) = default;

//     /// @brief Move assignment operator
//     /// @param other allocator to move
//     /// @return Reference to this object
//     fixed_allocator& operator=(fixed_allocator&& other) = default;

//     /// @brief Returns a pointer to the internal stack buffer. Never allocates heap memory.
//     /// @details Time:  O(1)
//     ///          Space: O(1)
//     ///          Element lifetime is managed externally by allocator_traits::construct / ::destroy.
//     /// @param n The number of objects to allocate storage for. Must be <= N.
//     /// @return Pointer to the start of the raw storage buffer.
//     /// @exception std::bad_alloc If n > N.
//     T* allocate(std::size_t n);

//     /// @brief No-op. The stack buffer has no heap memory to release.
//     /// @details Time:  O(1)
//     ///          Space: O(1)
//     void deallocate(T*, std::size_t) noexcept {}

//     /// @brief Two fixed_allocators are equal only if they are the same object (share the same buffer).
//     /// @details Time:  O(1)
//     ///          Space: O(1)
//     /// @param other Other fixed_allocator.
//     /// @return True if both allocators refer to the same buffer.
//     bool equals(const fixed_allocator& other) const noexcept { return m_data == other.m_data; }

// private:
//     // Raw uninitialized storage. alignas ensures T objects can be correctly placed here via
//     // placement new. Using unsigned char avoids default-constructing T on allocator creation
//     // and avoids UB when the default copy/move operates on slots whose T lifetime has ended.
//     alignas(alignof(T)) unsigned char m_data[(N == 0 ? 1 : N) * sizeof(T)];
// };

// // ===== Inline fixed_allocator Implementation =====

// template<typename T, std::size_t N>
// inline T* fixed_allocator<T, N>::allocate(std::size_t n)
// {
//     if (n > N)
//     {
//         throw std::bad_alloc();
//     }
//     return reinterpret_cast<T*>(m_data);
// }

} // namespace stdx

#endif
