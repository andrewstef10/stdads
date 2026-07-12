#ifndef INTERNAL_CONTIGUOUS_CONTAINER_H
#define INTERNAL_CONTIGUOUS_CONTAINER_H

#include <stdexcept>
#include <stdx/memory.h>

namespace stdx { namespace internal { namespace contiguous_container {

/// @brief Returns a reference to the element at position `index` with bounds checking.
/// @details Time:  O(1)
///          Space: O(1)
/// @param data  Pointer to the first element of the buffer.
/// @param size  Number of live elements in the buffer.
/// @param index Position of the element to return.
/// @return Reference to the requested element.
/// @exception std::out_of_range If index >= size().
template <typename T, typename size_type>
T& at(T* data, size_type size, size_type index) {
    if (index >= size) {
        throw std::out_of_range("Index is outside the bounds of the container");
    }
    return data[index];
}

/// @brief Compares two sequences of elements element-by-element using operator==, stopping at the first mismatch.
/// @details Time:  O(n)
///          Space: O(1)
///          Requires T to be equality-comparable (operator== defined).
///          Assumes that both buffers are the same size.
/// @param data1  Pointer to the first element of a buffer.
/// @param data2  Pointer to the first element of another buffer.
/// @param size   Number of live elements in both buffers.
/// @return True if the elements of both buffers compare equal.
template <typename T, typename size_type>
bool equals(T* data1, T* data2, size_type size) {
    for (size_type i = 0; i < size; ++i) {
        if (!(data1[i] == data2[i])) {
            return false;
        }
    }
    return true;
}

/// @brief Performs a lexicographic less-than comparison using operator<.
/// @details Time:  O(n)
///          Space: O(1)
///          Requires T to be less-than-comparable (operator< defined).
/// @param data1  Pointer to the first element of a buffer.
/// @param data2  Pointer to the first element of another buffer.
/// @param size1  Number of live elements in the first buffer.
/// @param size2  Number of live elements in the second buffer.
/// @return True if `data1` buffer is lexicographically less than `data2`.
template <typename T, typename size_type>
bool less_than(const T* data1, const T* data2, size_type size1, size_type size2) {
    for (std::size_t i = 0; i < size1 && i < size2; ++i) {
        if (data1[i] < data2[i]) {
            return true;
        }
        if (data2[i] < data1[i]) {
            return false;
        }
    }
    return size1 < size2;
}

/// @brief Opens a one-slot gap at `index` by moving the elements [index, size) up by one.
/// @details Time:  O(n), where n = size - index
///          Space: O(1)
///          Precondition: index < size, and the buffer holds raw storage for at least size + 1 elements.
///          The slot data[size] is raw storage, so the last element is move-constructed into it through the
///          allocator; the elements in (index, size - 1] are then move-assigned up by one. Afterwards
///          data[index] holds a live, moved-from object ready to be assigned to. The caller remains
///          responsible for tracking the new element count.
/// @param alloc Allocator used to construct into the raw slot past the last element.
/// @param data  Pointer to the first element of the buffer.
/// @param size  Number of live elements in the buffer.
/// @param index Position of the gap to open.
template <typename Allocator, typename T, typename size_type>
void shift_right(Allocator& alloc, T* data, size_type size, size_type index) {
    const size_type LAST = size - 1;
    std::allocator_traits<Allocator>::construct(alloc, data + size, std::move(data[LAST]));
    for (size_type i = LAST; i > index; --i) {
        data[i] = std::move(data[i - 1]);
    }
}

/// @brief Resizes the buffer to contain count elements.
/// @details Time:  O(n), where n is the difference between the current size and count
///          Space: O(1)
///          If the current size is equal to count, this function does nothing.
///          If the current size is greater than count, the buffer is reduced to its first count elements.
///          If the current size is less than count, then additional default-inserted elements are appended.
/// @param alloc Allocator used to construct default elements or destroy elements
/// @param data  Pointer to the first element of the buffer.
/// @param size  Number of live elements in the buffer.
/// @param count Number of desired elements in the buffer.
/// @param args  Arguments forwarded to T's constructor if additional T constructions are needed.
template <typename Allocator, typename T, typename size_type, typename... Args>
void resize(Allocator alloc, T* data, size_type size, size_type count, Args&&... args) {
    if (size < count) {
        for (; size < count; ++size) {
            std::allocator_traits<Allocator>::construct(alloc, data + size, std::forward<Args>(args)...);
        }
    } else {
        while (size > count) {
            std::allocator_traits<Allocator>::destroy(alloc, data + --size);
        }
    }
}

/// @brief Constructs an element at `index`, opening a one-slot gap in [index, size) if needed.
/// @details Time:  O(n), where n = size - index
///          Space: O(1)
///          Precondition: index <= size, and the buffer holds raw storage for at least size + 1 elements.
///          For a middle insert the value is built into a temporary before shifting, so it remains correct
///          even when `args` alias an element already in the buffer. For index == size the destination is
///          raw storage and the element is constructed directly in place. The caller remains responsible
///          for tracking the new element count.
/// @param alloc Allocator used to construct the new element.
/// @param data  Pointer to the first element of the buffer.
/// @param size  Number of live elements in the buffer.
/// @param index Position at which the new element is constructed (0..size).
/// @param args  Arguments forwarded to T's constructor.
/// @return Pointer to the newly constructed element.
template <typename Allocator, typename T, typename size_type, typename... Args>
T* emplace_at(Allocator& alloc, T* data, size_type size, size_type index, Args&&... args) {
    if (index < size) {
        // Middle: build the value first so it stays correct even when args alias an existing element,
        // open a one-slot gap, then move the value into the now-live slot.
        T value(std::forward<Args>(args)...);
        shift_right(alloc, data, size, index);
        data[index] = std::move(value);
    } else {
        // End: the destination is raw storage, so construct the element directly in place.
        std::allocator_traits<Allocator>::construct(alloc, data + index, std::forward<Args>(args)...);
    }
    return data + index;
}

/// @brief Copy-constructs the `srcSize` elements of `src` into the raw storage at `dest`, in order.
/// @details Time:  O(n), where n = srcSize
///          Space: O(1)
///          Precondition: dest is raw storage for at least srcSize elements and does not overlap src.
///          The caller remains responsible for tracking the new element count.
/// @param alloc   Allocator used to construct the new elements.
/// @param dest    Pointer to the raw storage receiving the copies.
/// @param src     Pointer to the first element to copy from.
/// @param srcSize Number of elements to copy.
template <typename Allocator, typename T, typename size_type>
void construct_copy(Allocator& alloc, T* dest, const T* src, size_type srcSize) {
    for (size_type i = 0; i < srcSize; ++i) {
        std::allocator_traits<Allocator>::construct(alloc, dest + i, src[i]);
    }
}

/// @brief Replaces the `destSize` live elements at `dest` with copies of the `srcSize` elements at `src`.
/// @details Time:  O(n), where n = max(destSize, srcSize)
///          Space: O(1)
///          Precondition: dest holds destSize live elements backed by raw storage for at least srcSize
///          elements, and does not overlap src.
///          Copy-assigns over the live elements both ranges share; when src is larger, the remainder is
///          copy-constructed into raw storage, and when dest is larger, the surplus elements are destroyed.
///          The caller remains responsible for tracking the new element count.
/// @param alloc    Allocator used to construct and destroy elements.
/// @param dest     Pointer to the first live destination element.
/// @param destSize Number of live elements at dest.
/// @param src      Pointer to the first element to copy from.
/// @param srcSize  Number of elements to copy.
template <typename Allocator, typename T, typename size_type>
void assign_copy(Allocator& alloc, T* dest, size_type destSize, const T* src, size_type srcSize) {
    if (destSize < srcSize) { // if dest contains less elements than src
        // Copy assign all elements from other up to current size
        size_type i = 0U;
        for (; i < destSize; ++i) {
            dest[i] = src[i];
        }

        // Copy construct remaining elements from other
        for (; i < srcSize; ++i) {
            std::allocator_traits<Allocator>::construct(alloc, dest + i, src[i]);
        }
    } else {
        // Copy assign all elements from other
        size_type i = 0U;
        for (; i < srcSize; ++i) {
            dest[i] = src[i];
        }

        // Remove remaining elements in this
        for (; i < destSize; ++i) {
            std::allocator_traits<Allocator>::destroy(alloc, dest + i);
        }
    }
}

/// @brief Move-constructs the `srcSize` elements of `src` into the raw storage at `dest`, destroying each
///        source element as it is consumed.
/// @details Time:  O(n), where n = srcSize
///          Space: O(1)
///          Precondition: dest is raw storage for at least srcSize elements and does not overlap src.
///          After the call src holds no live elements. The caller remains responsible for tracking both
///          containers' element counts.
/// @param destAlloc Allocator used to construct the destination elements.
/// @param dest      Pointer to the raw storage receiving the moved elements.
/// @param srcAlloc  Allocator used to destroy the source elements.
/// @param src       Pointer to the first element to move from.
/// @param srcSize   Number of elements to move.
template <typename DestAllocator, typename SrcAllocator, typename T, typename size_type>
void construct_move(DestAllocator& destAlloc, T* dest, SrcAllocator& srcAlloc, T* src, size_type srcSize) {
    for (size_type i = 0; i < srcSize; ++i) {
        std::allocator_traits<DestAllocator>::construct(destAlloc, dest + i, std::move(src[i]));
        std::allocator_traits<SrcAllocator>::destroy(srcAlloc, src + i); // Destroy other's moved-from elements
    }
}

/// @brief Replaces the `destSize` live elements at `dest` by moving the `srcSize` elements from `src` into
///        them, destroying each source element as it is consumed.
/// @details Time:  O(n), where n = max(destSize, srcSize)
///          Space: O(1)
///          Precondition: dest holds destSize live elements backed by raw storage for at least srcSize
///          elements, and does not overlap src.
///          Move-assigns over the live elements both ranges share; when src is larger, the remainder is
///          move-constructed into raw storage, and when dest is larger, the surplus elements are destroyed.
///          After the call src holds no live elements. The caller remains responsible for tracking both
///          containers' element counts.
/// @param destAlloc Allocator used to construct and destroy destination elements.
/// @param dest      Pointer to the first live destination element.
/// @param destSize  Number of live elements at dest.
/// @param srcAlloc  Allocator used to destroy the source elements.
/// @param src       Pointer to the first element to move from.
/// @param srcSize   Number of elements to move.
template <typename DestAllocator, typename SrcAllocator, typename T, typename size_type>
void assign_move(DestAllocator& destAlloc, T* dest, size_type destSize, //
                 SrcAllocator& srcAlloc, T* src, size_type srcSize)     //
{
    if (destSize < srcSize) // if dest container contains less elements than src
    {
        // Move assign all elements from other up to current size
        size_type i = 0U;
        for (; i < destSize; ++i) {
            dest[i] = std::move(src[i]);
            std::allocator_traits<SrcAllocator>::destroy(srcAlloc, src + i); // Destroy other's moved-from elements
        }

        // Move construct remaining elements from other
        for (; i < srcSize; ++i) {
            std::allocator_traits<DestAllocator>::construct(destAlloc, dest + i, std::move(src[i]));
            std::allocator_traits<SrcAllocator>::destroy(srcAlloc, src + i); // Destroy other's moved-from elements
        }
    } else {
        // Move assign all elements from other
        size_type i = 0U;
        for (; i < srcSize; ++i) {
            dest[i] = std::move(src[i]);
            std::allocator_traits<SrcAllocator>::destroy(srcAlloc, src + i); // Destroy other's moved-from elements
        }

        // Remove remaining elements in this
        for (; i < destSize; ++i) {
            std::allocator_traits<DestAllocator>::destroy(destAlloc, dest + i);
        }
    }
}
}}} // namespace stdx::internal::contiguous_container

#endif
