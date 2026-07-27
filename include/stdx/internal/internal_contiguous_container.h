#ifndef INTERNAL_CONTIGUOUS_CONTAINER_H
#define INTERNAL_CONTIGUOUS_CONTAINER_H

#include <cstddef>
#include <stdexcept>
#include <stdx/memory.h>
#include <utility>

namespace stdx { namespace internal { namespace contiguous_container {

/// @brief Non-owning view of a contiguous buffer: a pointer to its first element plus a caller-asserted count of live
///        elements.
/// @tparam T Element type. Use view<const T> for buffers that are only read from.
template <typename T>
struct view {
    /// @brief Pointer to the first element of the buffer.
    T* data;

    /// @brief Number of live elements in the buffer.
    std::size_t size;
};

/// @brief Creates a view over the `size` elements starting at `data`.
/// @details A pointer-to-const produces a view<const T>.
/// @param data Pointer to the first element of the buffer.
/// @param size Number of live elements in the buffer.
/// @return View over the described buffer.
template <typename T>
view<T> make_view(T* data, std::size_t size) {
    return view<T>{data, size};
}

/// @brief Returns a reference to the element in `container` at position `index` with bounds checking.
/// @details Time:  O(1)
///          Space: O(1)
/// @tparam Container The class type of `container` with public data() and size() functions, and size_type type trait.
/// @param container Contiguous container to access
/// @param index Position of the element to return
/// @return Reference to the requested element
/// @exception std::out_of_range If index >= container.size().
template <typename Container>
auto at(Container& container, typename Container::size_type index) -> decltype(container.data()[index]) {
    if (index >= container.size()) {
        throw std::out_of_range("Index is outside the bounds of the container");
    }
    return container.data()[index];
}

/// @brief Compares two contiguous containers element-by-element using operator==, stopping at the first mismatch.
/// @details Time:  O(n)
///          Space: O(1)
///          Requires Container::value_type to be equality-comparable (operator== defined).
/// @tparam Container The class type of `container` with public data() and size() functions, and size_type type trait.
/// @param lhs lhs contiguous container to compare
/// @param rhs rhs contiguous container to compare
/// @return True if the elements of both containers compare equal.
template <typename Container>
bool equals(const Container& lhs, const Container& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (typename Container::size_type i = 0; i < lhs.size(); ++i) {
        if (!(lhs.data()[i] == rhs.data()[i])) {
            return false;
        }
    }
    return true;
}

/// @brief Performs a lexicographic less-than comparison using operator<.
/// @details Time:  O(n)
///          Space: O(1)
///          Requires Container::value_type to be less-than-comparable (operator< defined).
/// @tparam Container The class type of `container` with public data() and size() functions, and size_type type trait.
/// @param lhs Reference to the lhs contiguous container to compare
/// @param rhs Reference to the rhs contiguous container to compare
/// @return True if `lhs` is lexicographically less than `rhs`
template <typename Container>
bool less_than(const Container& lhs, const Container& rhs) {
    for (typename Container::size_type i = 0; i < lhs.size() && i < rhs.size(); ++i) {
        if (lhs.data()[i] < rhs.data()[i]) {
            return true;
        }
        if (rhs.data()[i] < lhs.data()[i]) {
            return false;
        }
    }
    return lhs.size() < rhs.size();
}

/// @brief Opens a one-slot gap at `index` by moving the elements [index, buffer.size) up by one.
/// @details Time:  O(n), where n = buffer.size - index
///          Space: O(1)
///          Precondition: index < buffer.size, and the buffer holds raw storage for at least buffer.size + 1 elements.
///          The slot buffer.data[buffer.size] is raw storage, so the last element is move-constructed into it through
///          the allocator.
/// @param alloc  Allocator used to construct into the raw slot past the last element.
/// @param buffer View of the live elements of the buffer.
/// @param index  Position of the gap to open.
template <typename Allocator, typename T>
void shift_right(Allocator& alloc, view<T> buffer, std::size_t index) {
    const std::size_t LAST = buffer.size - 1;
    std::allocator_traits<Allocator>::construct(alloc, buffer.data + buffer.size, std::move(buffer.data[LAST]));
    for (std::size_t i = LAST; i > index; --i) {
        buffer.data[i] = std::move(buffer.data[i - 1]);
    }
}

/// @brief Closes the gap [first, last) by moving the elements [last, buffer.size) down onto it.
/// @details Time:  O(n), where n = buffer.size - last
///          Space: O(1)
///          Precondition: first <= last <= buffer.size.
///          The elements [last, buffer.size) are move-assigned into [first, first + buffer.size - last). The tail
///          behind the returned count is left holding live, moved-from objects that the caller remains responsible
///          for destroying. Does nothing if the range is empty.
/// @param buffer View of the live elements of the buffer.
/// @param first Position of the first element of the gap to close.
/// @param last  Position one past the last element of the gap to close.
/// @return The buffer's new logical element count (the position where the moved-from tail begins).
template <typename T>
std::size_t shift_left(view<T> buffer, std::size_t first, std::size_t last) {
    if (first == last) {
        return buffer.size;
    }

    std::size_t left = first;
    for (std::size_t right = last; right < buffer.size; ++left, ++right) {
        buffer.data[left] = std::move(buffer.data[right]);
    }
    return left;
}

/// @brief Resizes the buffer to contain count elements.
/// @details Time:  O(n), where n is the difference between buffer.size and count
///          Space: O(1)
///          Precondition: the buffer holds raw storage for at least count elements.
///          If buffer.size is equal to count, this function does nothing.
///          If buffer.size is greater than count, the buffer is reduced to its first count elements.
///          If buffer.size is less than count, additional elements constructed from `args` are appended.
///          The caller remains responsible for tracking the new element count.
/// @param alloc  Allocator used to construct appended elements or destroy surplus elements.
/// @param buffer View of the live elements of the buffer.
/// @param count  Number of desired elements in the buffer.
/// @param args   Arguments forwarded to T's constructor if additional T constructions are needed.
template <typename Allocator, typename T, typename... Args>
void resize(Allocator& alloc, view<T> buffer, std::size_t count, Args&&... args) {
    if (buffer.size < count) {
        for (std::size_t i = buffer.size; i < count; ++i) {
            std::allocator_traits<Allocator>::construct(alloc, buffer.data + i, std::forward<Args>(args)...);
        }
    } else {
        for (std::size_t i = buffer.size; i > count; --i) {
            std::allocator_traits<Allocator>::destroy(alloc, buffer.data + (i - 1));
        }
    }
}

/// @brief Constructs an element at `index`, opening a one-slot gap in [index, buffer.size) if needed.
/// @details Time:  O(n), where n = buffer.size - index
///          Space: O(1)
///          Precondition: index <= buffer.size, and the buffer holds raw storage for at least buffer.size + 1 elements.
///          For a middle insert the value is built into a temporary before shifting, so it remains correct even when
///          `args` alias an element already in the buffer. For index == buffer.size the destination is raw storage and
///          the element is constructed directly in place. The caller remains responsible for tracking the new element
///          count.
/// @param alloc  Allocator used to construct the new element.
/// @param buffer View of the live elements of the buffer.
/// @param index  Position at which the new element is constructed (0..buffer.size).
/// @param args   Arguments forwarded to T's constructor.
/// @return Pointer to the newly constructed element.
template <typename Allocator, typename T, typename... Args>
T* emplace_at(Allocator& alloc, view<T> buffer, std::size_t index, Args&&... args) {
    if (index < buffer.size) {
        // Middle: build the value first so it stays correct even when args alias an existing element,
        // open a one-slot gap, then move the value into the now-live slot.
        T value(std::forward<Args>(args)...);
        shift_right(alloc, buffer, index);
        buffer.data[index] = std::move(value);
    } else {
        // End: the destination is raw storage, so construct the element directly in place.
        std::allocator_traits<Allocator>::construct(alloc, buffer.data + index, std::forward<Args>(args)...);
    }
    return buffer.data + index;
}

/// @brief Copy-constructs the elements of `src` into the raw storage at `dest`, in order.
/// @details Time:  O(n), where n = src.size
///          Space: O(1)
///          Precondition: dest is raw storage for at least src.size elements and does not overlap src.
///          The caller remains responsible for tracking the new element count.
/// @param alloc Allocator used to construct the new elements.
/// @param dest  Pointer to the raw storage receiving the copies.
/// @param src   View of the elements to copy from.
template <typename Allocator, typename T>
void construct_copy(Allocator& alloc, T* dest, view<const T> src) {
    for (std::size_t i = 0; i < src.size; ++i) {
        std::allocator_traits<Allocator>::construct(alloc, dest + i, src.data[i]);
    }
}

/// @brief Replaces the live elements of `dest` with `count` copies of `value`.
/// @details Time:  O(n), where n = max(dest.size, count)
///          Space: O(1)
///          Precondition: dest holds dest.size live elements backed by raw storage for at least count elements, and
///          `value` is not an element of dest.
///          The caller remains responsible for tracking the new element count.
/// @param alloc Allocator used to construct and destroy elements.
/// @param dest  View of the live destination elements.
/// @param count Number of copies of `value` the buffer holds afterwards.
/// @param value Value to fill the buffer with.
template <typename Allocator, typename T>
void assign_copy(Allocator& alloc, view<T> dest, std::size_t count, const T& value) {
    if (dest.size < count) {
        // Copy assign over the live elements
        std::size_t i = 0;
        for (; i < dest.size; ++i) {
            dest.data[i] = value;
        }

        // Copy construct the remaining elements into raw storage
        for (; i < count; ++i) {
            std::allocator_traits<Allocator>::construct(alloc, dest.data + i, value);
        }
    } else {
        // Copy assign over the first count live elements
        std::size_t i = 0;
        for (; i < count; ++i) {
            dest.data[i] = value;
        }

        // Destroy the surplus elements
        for (; i < dest.size; ++i) {
            std::allocator_traits<Allocator>::destroy(alloc, dest.data + i);
        }
    }
}

/// @brief Replaces the live elements of `dest` with copies of the elements of `src`.
/// @details Time:  O(n), where n = max(dest.size, src.size)
///          Space: O(1)
///          Precondition: dest holds dest.size live elements backed by raw storage for at least src.size elements, and
///          does not overlap src. The caller remains responsible for tracking the new element count.
/// @param alloc Allocator used to construct and destroy elements.
/// @param dest  View of the live destination elements.
/// @param src   View of the elements to copy from.
template <typename Allocator, typename T>
void assign_copy(Allocator& alloc, view<T> dest, view<const T> src) {
    assign_copy(alloc, dest, src.data, src.size);
}

/// @brief Replaces the live elements of `dest` with copies of the elements of the container defined by the forward
///        iterator `first` and `count` number of elements past `first`.
/// @details Time:  O(n), where n = max(dest.size, src.size)
///          Space: O(1)
///          Precondition: dest holds dest.size live elements backed by raw storage for at least src.size elements, and
///          does not overlap src. The caller remains responsible for tracking the new element count.
/// @param alloc Allocator used to construct and destroy elements.
/// @param dest  View of the live destination elements.
/// @param first Forward iterator pointing to the first element to copy assign to dest
/// @param count Number of elements to copy starting with `first`
template <typename Allocator, typename T, typename ForwardIt>
void assign_copy(Allocator& alloc, view<T> dest, ForwardIt first, std::size_t count) {
    std::size_t i = 0;
    ForwardIt src = first;
    if (dest.size < count) { // if dest contains less elements than src
        // Copy assign all elements from src up to dest's live count
        for (; i < dest.size; ++i, ++src) {
            dest.data[i] = *src;
        }

        // Copy construct the remaining elements from src
        for (; i < count; ++i, ++src) {
            std::allocator_traits<Allocator>::construct(alloc, dest.data + i, *src);
        }
    } else {
        // Copy assign all elements from src
        for (; i < count; ++i, ++src) {
            dest.data[i] = *src;
        }

        // Destroy the surplus elements in dest
        for (; i < dest.size; ++i) {
            std::allocator_traits<Allocator>::destroy(alloc, dest.data + i);
        }
    }
}

/// @brief Move-constructs the elements of `src` into the raw storage at `dest`, destroying each source element as it is
///        consumed.
/// @details Time:  O(n), where n = src.size
///          Space: O(1)
///          Precondition: dest is raw storage for at least src.size elements and does not overlap src.
///          After the call src holds no live elements. The caller remains responsible for tracking both
///          containers' element counts.
/// @param destAlloc Allocator used to construct the destination elements.
/// @param dest      Pointer to the raw storage receiving the moved elements.
/// @param srcAlloc  Allocator used to destroy the source elements.
/// @param src       View of the elements to move from.
template <typename DestAllocator, typename SrcAllocator, typename T>
void construct_move(DestAllocator& destAlloc, T* dest, SrcAllocator& srcAlloc, view<T> src) {
    for (std::size_t i = 0; i < src.size; ++i) {
        std::allocator_traits<DestAllocator>::construct(destAlloc, dest + i, std::move(src.data[i]));
        std::allocator_traits<SrcAllocator>::destroy(srcAlloc, src.data + i); // Destroy src's moved-from elements
    }
}

/// @brief Replaces the live elements of `dest` by moving the elements of `src` into them, destroying each
///        source element as it is consumed.
/// @details Time:  O(n), where n = max(dest.size, src.size)
///          Space: O(1)
///          Precondition: dest holds dest.size live elements backed by raw storage for at least src.size elements, and
///          does not overlap src. After the call src holds no live elements. The caller remains responsible for
///          tracking both containers' element counts.
/// @param destAlloc Allocator used to construct and destroy destination elements.
/// @param dest      View of the live destination elements.
/// @param srcAlloc  Allocator used to destroy the source elements.
/// @param src       View of the elements to move from.
template <typename DestAllocator, typename SrcAllocator, typename T>
void assign_move(DestAllocator& destAlloc, view<T> dest, SrcAllocator& srcAlloc, view<T> src) {
    if (dest.size < src.size) // if dest contains less elements than src
    {
        // Move assign all elements from src up to dest's live count
        std::size_t i = 0;
        for (; i < dest.size; ++i) {
            dest.data[i] = std::move(src.data[i]);
            std::allocator_traits<SrcAllocator>::destroy(srcAlloc, src.data + i); // Destroy src's moved-from elements
        }

        // Move construct the remaining elements from src
        for (; i < src.size; ++i) {
            std::allocator_traits<DestAllocator>::construct(destAlloc, dest.data + i, std::move(src.data[i]));
            std::allocator_traits<SrcAllocator>::destroy(srcAlloc, src.data + i); // Destroy src's moved-from elements
        }
    } else {
        // Move assign all elements from src
        std::size_t i = 0;
        for (; i < src.size; ++i) {
            dest.data[i] = std::move(src.data[i]);
            std::allocator_traits<SrcAllocator>::destroy(srcAlloc, src.data + i); // Destroy src's moved-from elements
        }

        // Destroy the surplus elements in dest
        for (; i < dest.size; ++i) {
            std::allocator_traits<DestAllocator>::destroy(destAlloc, dest.data + i);
        }
    }
}
}}} // namespace stdx::internal::contiguous_container

#endif
