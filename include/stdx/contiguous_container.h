#ifndef CONTIGUOUS_CONTAINER_H
#define CONTIGUOUS_CONTAINER_H

#include <cstddef>

#include <stdx/container.h>
#include <stdx/internal/internal_contiguous_container.h>
#include <stdx/iterator.h>

namespace stdx {

/// @brief CRTP base class for contiguous-storage containers (e.g. array_list, fixed_array_list).
///        Provides element access, iterators, and element-order comparison on top of container<>.
/// @tparam Derived The concrete container type inheriting this base.
/// @tparam T       The element type stored in the container.
template <typename Derived, typename T>
class contiguous_container : public stdx::container<Derived, T> {
  public:
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = stdx::reverse_iterator<iterator>;
    using const_reverse_iterator = stdx::reverse_iterator<const_iterator>;

    // ==========================
    // ===== Element Access =====
    // ==========================

    /// @brief Returns a reference to the first element. Behavior is undefined if the container is empty.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reference to the first element.
    T& front() { return derived().data()[0]; }
    const T& front() const { return derived().data()[0]; }

    /// @brief Returns a reference to the last element. Behavior is undefined if the container is empty.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reference to the last element.
    T& back() { return derived().data()[derived().size() - 1]; }
    const T& back() const { return derived().data()[derived().size() - 1]; }

    /// @brief Returns a reference to the element at position `index` with bounds checking.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @param index Position of the element to return.
    /// @return Reference to the requested element.
    /// @exception std::out_of_range If index >= size().
    T& at(std::size_t index) { return internal::contiguous_container::at(derived(), index); }
    const T& at(std::size_t index) const { return internal::contiguous_container::at(derived(), index); }

    /// @brief Returns a reference to the element at position `index` without bounds checking.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @param index Position of the element to return.
    /// @return Reference to the requested element.
    T& operator[](std::size_t index) { return derived().data()[index]; }
    const T& operator[](std::size_t index) const { return derived().data()[index]; }

    // ===================
    // ==== Iterators ====
    // ===================

    /// @brief Returns an iterator to the first element. If the container is empty, the iterator equals end().
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Iterator to the first element.
    iterator begin() { return iterator(derived().data()); }
    const_iterator begin() const { return const_iterator(derived().data()); }
    const_iterator cbegin() const { return const_iterator(derived().data()); }

    /// @brief Returns a past-the-end sentinel iterator. The iterator is not dereferenceable.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Iterator past the last element.
    iterator end() { return iterator(derived().data() + derived().size()); }
    const_iterator end() const { return const_iterator(derived().data() + derived().size()); }
    const_iterator cend() const { return const_iterator(derived().data() + derived().size()); }

    /// @brief Returns a reverse iterator to the last element (first element of the reversed range). Equals rend() if
    /// empty.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reverse iterator to the last element.
    reverse_iterator rbegin() { return reverse_iterator(derived().data() + derived().size()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(derived().data() + derived().size()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(derived().data() + derived().size()); }

    /// @brief Returns a reverse past-the-end sentinel iterator. The iterator is not dereferenceable.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reverse iterator past the first element (in reverse order).
    reverse_iterator rend() { return reverse_iterator(derived().data()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(derived().data()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(derived().data()); }

    // ====================
    // ==== Comparison ====
    // ====================

    /// @brief Compares two containers element-by-element using operator==, stopping at the first mismatch.
    /// @details Time:  O(n) — compares up to min(size(), other.size()) elements
    ///          Space: O(1)
    ///          Requires T to be equality-comparable (operator== defined).
    /// @param other Container to compare against.
    /// @return True if both containers have equal size and all elements compare equal.
    bool equals(const Derived& other) const { return internal::contiguous_container::equals(derived(), other); }

    /// @brief Performs a lexicographic less-than comparison using operator<.
    /// @details Time:  O(n) — compares up to min(size(), other.size()) elements
    ///          Space: O(1)
    ///          Requires T to be less-than-comparable (operator< defined).
    /// @param other Container to compare against.
    /// @return True if this container is lexicographically less than `other`.
    bool less_than(const Derived& other) const { return internal::contiguous_container::less_than(derived(), other); }

    // ===================
    // ==== Modifiers ====
    // ===================

    /// @brief Removes the element at `pos`, shifting elements after `pos` to the left.
    /// @details Time:  O(n) — shifts elements after pos forward by one
    ///          Space: O(1)
    ///          Invalidates the iterator at pos and all iterators/references after it.
    /// @param pos Iterator to the element to remove.
    /// @return Iterator to the element that followed the removed one, or end() if pos was the last element.
    iterator erase(const_iterator pos) { return erase(pos, pos + 1); }

    /// @brief Removes all elements in the range [first, last), shifting elements after `last` to the left.
    /// @details Time:  O(n) — shifts elements after last forward
    ///          Space: O(1)
    ///          Invalidates iterators from first onward. Returns last unchanged if the range is empty.
    /// @param first Iterator to the first element to remove.
    /// @param last  Iterator past the last element to remove.
    /// @return Iterator to the element that followed the last removed element.
    iterator erase(const_iterator first, const_iterator last);

  protected:
    contiguous_container() = default;
    ~contiguous_container() = default;
    contiguous_container(const contiguous_container&) = default;
    contiguous_container(contiguous_container&&) noexcept = default;
    contiguous_container& operator=(const contiguous_container&) = default;
    contiguous_container& operator=(contiguous_container&&) noexcept = default;

  private:
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

template <typename Derived, typename T>
inline typename contiguous_container<Derived, T>::iterator
contiguous_container<Derived, T>::erase(const_iterator first, const_iterator last) {
    const std::size_t FIRST = static_cast<std::size_t>(first - derived().data());
    const std::size_t LAST = static_cast<std::size_t>(last - derived().data());

    // Close the gap by moving the tail elements down onto it
    namespace icc = internal::contiguous_container;
    const std::size_t NEW_SIZE = icc::shift_left(icc::make_view(derived().data(), derived().size()), FIRST, LAST);

    // Destroy the moved-from tail elements
    while (derived().size() > NEW_SIZE) {
        derived().pop_back();
    }

    return iterator(derived().data() + FIRST);
}

} // namespace stdx

#endif
