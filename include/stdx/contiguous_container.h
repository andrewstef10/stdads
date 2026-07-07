#ifndef CONTIGUOUS_CONTAINER_H
#define CONTIGUOUS_CONTAINER_H

#include <stdexcept>

#include <stdx/container.h>
#include <stdx/iterator.h>

namespace stdx {

/// @brief CRTP base class for contiguous-storage containers (e.g. array_list, fixed_array_list).
///        Provides element access, iterators, and element-order comparison on top of container<>.
/// @tparam Derived The concrete container type inheriting this base.
/// @tparam T       The element type stored in the container.
template <typename Derived, typename T>
class contiguous_container : public stdx::container<Derived, T> {
  public:
    using size_type = typename container<Derived, T>::size_type;
    using reference = typename container<Derived, T>::reference;
    using const_reference = typename container<Derived, T>::const_reference;

    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = stdx::reverse_iterator<iterator>;
    using const_reverse_iterator = stdx::reverse_iterator<const_iterator>;

    // ===== Element Access =====

    /// @brief Returns a reference to the element at position `index` with bounds checking.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @param index Position of the element to return.
    /// @return Reference to the requested element.
    /// @exception std::out_of_range If index >= size().
    reference at(size_type index);
    const_reference at(size_type index) const;

    /// @brief Returns a reference to the element at position `index` without bounds checking.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @param index Position of the element to return.
    /// @return Reference to the requested element.
    reference operator[](size_type index) { return derived().data()[index]; }
    const_reference operator[](size_type index) const { return derived().data()[index]; }

    /// @brief Returns a reference to the last element. Behavior is undefined if the container is empty.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reference to the last element.
    reference back() { return derived().data()[derived().m_size - 1]; }
    const_reference back() const { return derived().data()[derived().m_size - 1]; }

    /// @brief Returns a reference to the first element. Behavior is undefined if the container is empty.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reference to the first element.
    reference front() { return derived().data()[0]; }
    const_reference front() const { return derived().data()[0]; }

    // ==== Iterators ====

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
    iterator end() { return iterator(derived().data() + derived().m_size); }
    const_iterator end() const { return const_iterator(derived().data() + derived().m_size); }
    const_iterator cend() const { return const_iterator(derived().data() + derived().m_size); }

    /// @brief Returns a reverse iterator to the last element (first element of the reversed range). Equals rend() if
    /// empty.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reverse iterator to the last element.
    reverse_iterator rbegin() { return reverse_iterator(derived().data() + derived().m_size); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(derived().data() + derived().m_size); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(derived().data() + derived().m_size); }

    /// @brief Returns a reverse past-the-end sentinel iterator. The iterator is not dereferenceable.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Reverse iterator past the first element (in reverse order).
    reverse_iterator rend() { return reverse_iterator(derived().data()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(derived().data()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(derived().data()); }

    // ==== Comparison ====

    /// @brief Compares two containers element-by-element using operator==, stopping at the first mismatch.
    /// @details Time:  O(n) — compares up to min(size(), other.size()) elements
    ///          Space: O(1)
    ///          Requires T to be equality-comparable (operator== defined).
    /// @param other Container to compare against.
    /// @return True if both containers have equal size and all elements compare equal.
    bool equals(const Derived& other) const;

    /// @brief Performs a lexicographic less-than comparison using operator<.
    /// @details Time:  O(n) — compares up to min(size(), other.size()) elements
    ///          Space: O(1)
    ///          Requires T to be less-than-comparable (operator< defined).
    /// @param other Container to compare against.
    /// @return True if this container is lexicographically less than `other`.
    bool less_than(const Derived& other) const;

    // ==== Modifiers ====

    /// @brief Removes the element at `pos`, shifting elements after `pos` to the left.
    /// @details Time:  O(n) — shifts elements after pos forward by one
    ///          Space: O(1)
    ///          Invalidates the iterator at pos and all iterators/references after it.
    /// @param pos Iterator to the element to remove.
    /// @return Iterator to the element that followed the removed one, or end() if pos was the last element.
    iterator erase(const_iterator pos);

    /// @brief Removes all elements in the range [first, last), shifting elements after `last` to the left.
    /// @details Time:  O(n) — shifts elements after last forward
    ///          Space: O(1)
    ///          Invalidates iterators from first onward. Returns last unchanged if the range is empty.
    /// @param first Iterator to the first element to remove.
    /// @param last  Iterator past the last element to remove.
    /// @return Iterator to the element that followed the last removed element.
    iterator erase(const_iterator first, const_iterator last);

    /// @brief Destroys all elements. size() becomes zero; capacity() is unchanged.
    /// @details Time:  O(n) — destructs each element
    ///          Space: O(1)
    ///          Invalidates all iterators and references to elements.
    void clear();

  protected:
    contiguous_container() = default;
    ~contiguous_container() = default;
    contiguous_container(const contiguous_container&) = default;
    contiguous_container(contiguous_container&&) noexcept = default;
    contiguous_container& operator=(const contiguous_container&) = default;
    contiguous_container& operator=(contiguous_container&&) noexcept = default;

    /// @brief In-place insert: opens a one-slot gap at `index` and places the new element.
    /// @details Precondition: the derived container has spare capacity for one more element.
    ///          For a middle insert the new value is built into a temporary before shifting, so it is correct
    ///          even when `args` alias an element already in the container. Relies on the derived class
    ///          providing `construct(T*, Args&&...)`, `operator[]`, `data()`, `size()`, and member `m_size`.
    /// @param index Position at which the new element is constructed (0..size()).
    /// @param args  Arguments forwarded to T's constructor for the new element.
    /// @return Iterator to the newly constructed element.
    template <typename... Args>
    iterator emplace_inplace(size_type index, Args&&... args);

    /// @brief Opens a one-slot gap at `index` by moving elements [index, size()) up by one.
    /// @details Precondition: index < size() and the derived container has spare capacity for one more element.
    ///          The slot just past the last element is raw storage, so it is filled via Derived::construct;
    ///          the remaining elements are move-assigned up. Afterwards the slot at `index` holds a live,
    ///          moved-from object ready to be assigned to.
    /// @param index Position of the gap to open.
    void shift_right(size_type index);

  private:
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

template <typename Derived, typename T>
inline contiguous_container<Derived, T>::reference contiguous_container<Derived, T>::at(size_type index) {
    if (index >= this->size()) {
        throw std::out_of_range("Index outside the bounds of the container");
    }
    return derived()[index];
}

template <typename Derived, typename T>
inline contiguous_container<Derived, T>::const_reference contiguous_container<Derived, T>::at(size_type index) const {
    if (index >= this->size()) {
        throw std::out_of_range("Index outside the bounds of the container");
    }
    return derived()[index];
}

template <typename Derived, typename T>
inline bool contiguous_container<Derived, T>::equals(const Derived& other) const {
    if (this->size() != other.size()) {
        return false;
    }

    for (size_type i = 0; i < this->size(); ++i) {
        if (!(derived()[i] == other[i])) {
            return false;
        }
    }
    return true;
}

template <typename Derived, typename T>
inline bool contiguous_container<Derived, T>::less_than(const Derived& other) const {
    for (size_type i = 0; i < this->size() && i < other.size(); ++i) {
        if (derived()[i] < other[i]) {
            return true;
        }
        if (other[i] < derived()[i]) {
            return false;
        }
    }
    return this->size() < other.size();
}

template <typename Derived, typename T>
inline typename contiguous_container<Derived, T>::iterator contiguous_container<Derived, T>::erase(const_iterator pos) {
    const size_type INDEX = static_cast<size_type>(pos - derived().data());

    // Move elements [INDEX + 1, m_size) left by one slot, replacing the element at INDEX
    for (size_type i = INDEX; i < this->size() - 1; ++i) {
        derived()[i] = std::move(derived()[i + 1]);
    }

    // Destroy the last element
    derived().pop_back();

    return iterator(derived().data() + INDEX);
}

template <typename Derived, typename T>
inline typename contiguous_container<Derived, T>::iterator
contiguous_container<Derived, T>::erase(const_iterator first, const_iterator last) {
    const size_type IFIRST = static_cast<size_type>(first - derived().data());

    // remove values in range [first, last)
    size_type left = IFIRST;
    size_type right = static_cast<size_type>(last - derived().data());
    for (; right < this->size(); ++left, ++right) {
        derived()[left] = std::move(derived()[right]);
    }

    // destroy the remaining elements
    while (this->size() > left) {
        derived().pop_back();
    }

    return iterator(derived().data() + IFIRST);
}

template <typename Derived, typename T>
inline void contiguous_container<Derived, T>::clear() {
    while (this->size() > 0) {
        derived().pop_back();
    }
}

template <typename Derived, typename T>
template <typename... Args>
inline typename contiguous_container<Derived, T>::iterator
contiguous_container<Derived, T>::emplace_inplace(size_type index, Args&&... args) {
    if (index < this->size()) {
        // Middle: build the value first so it stays correct even if args alias an existing element,
        // open a one-slot gap, then move the value into the now-live slot.
        T value(std::forward<Args>(args)...);
        shift_right(index);
        derived()[index] = std::move(value);
    } else {
        // End: the destination is raw storage, so construct the element directly in place.
        derived().construct(derived().data() + index, std::forward<Args>(args)...);
    }

    ++derived().m_size;
    return iterator(derived().data() + index);
}

template <typename Derived, typename T>
inline void contiguous_container<Derived, T>::shift_right(size_type index) {
    // The slot just past the last element is raw storage -> construct into it; the elements in
    // (index, size()) are live -> move-assign them up by one, leaving slot[index] live & moved-from.
    const size_type LAST = this->size() - 1;
    derived().construct(derived().data() + this->size(), std::move(derived()[LAST]));
    for (size_type i = LAST; i > index; --i) {
        derived()[i] = std::move(derived()[i - 1]);
    }
}
} // namespace stdx

#endif
