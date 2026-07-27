#ifndef ITERATOR_H
#define ITERATOR_H

#include <cstddef>
#include <iterator>
#include <type_traits>

#include <stdx/comparable.h>
#include <stdx/equatable.h>

namespace stdx {

// Forward declarations
template <typename Derived, typename T, typename Difference, typename Pointer, typename Reference, typename Category>
class forward_iterator;

template <typename Derived, typename T, typename Difference, typename Pointer, typename Reference, typename Category>
class bidirectional_iterator;

template <typename Derived, typename T, typename Difference, typename Pointer, typename Reference, typename Category>
class random_access_iterator;

/// @brief Iterator base class selector tags
///
/// Struct that provides the correct iterator base class to implement based on the provided iterator category.
///
/// @tparam Category Iterator category
/// @tparam Derived CRTP Derived class
/// @tparam T Iterator value type
/// @tparam Difference Iterator difference type
/// @tparam Pointer Iterator pointer type
/// @tparam Reference Iterator reference
template <typename Category, typename Derived, typename T, typename Difference = std::ptrdiff_t, typename Pointer = T*,
          typename Reference = T&>
struct iterator_base_selector;

template <typename Derived, typename T, typename Difference, typename Pointer, typename Reference>
struct iterator_base_selector<std::forward_iterator_tag, Derived, T, Difference, Pointer, Reference> {
    using type = forward_iterator<Derived, T, Difference, Pointer, Reference, std::forward_iterator_tag>;
};

template <typename Derived, typename T, typename Difference, typename Pointer, typename Reference>
struct iterator_base_selector<std::bidirectional_iterator_tag, Derived, T, Difference, Pointer, Reference> {
    using type = bidirectional_iterator<Derived, T, Difference, Pointer, Reference, std::bidirectional_iterator_tag>;
};

template <typename Derived, typename T, typename Difference, typename Pointer, typename Reference>
struct iterator_base_selector<std::random_access_iterator_tag, Derived, T, Difference, Pointer, Reference> {
    using type = random_access_iterator<Derived, T, Difference, Pointer, Reference, std::random_access_iterator_tag>;
};

/// @brief Base iterator class to define iterator traits.
/// @tparam Category The type of iterator
/// @tparam T The iterator's underlying value type
/// @tparam Difference The type to define the difference between two iterators
/// @tparam Pointer Pointer to the iterator's value type
/// @tparam Reference Reference to the iterators value type
template <typename Category, typename T, typename Difference = std::ptrdiff_t, typename Pointer = T*,
          typename Reference = T&>
struct iterator {
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Difference;
    using pointer = Pointer;
    using reference = Reference;
};

/// @brief Base CRTP forward iterator class.
///
/// This class implements all forward iterator requirements, and requires Derived to implement the following:
///     - Reference operator*() const
///     - Pointer operator->() const
///     - void increment()
///     - bool equals(const MyIterator& other) const
///
/// @tparam Derived CRTP Derived class
/// @tparam T The iterator's underlying value type
/// @tparam Difference The type to define the difference between two iterators
/// @tparam Pointer Pointer to the iterator's value type
/// @tparam Reference Reference to the iterators value type
/// @tparam Category Category The type of iterator
template <typename Derived, typename T, typename Difference = std::ptrdiff_t, typename Pointer = T*,
          typename Reference = T&, typename Category = std::forward_iterator_tag>
class forward_iterator : public iterator<Category, T, Difference, Pointer, Reference>, public equatable<Derived> {
  public:
    /// @brief Pre-increment operator
    /// @return A reference to this iterator after the increment
    Derived& operator++() {
        this->derived().increment();
        return this->derived();
    }

    /// @brief Post-increment operator
    /// @param
    /// @return Copy of this iterator before the increment
    Derived operator++(int) {
        Derived temp = this->derived();
        this->derived().increment();
        return temp;
    }

  protected:
    // ==== forward_iterator should not be constructed directly ====
    forward_iterator() = default; // NOLINT(bugprone-crtp-constructor-accessibility)

    /// @brief Helper function to cast this to a Derived reference
    /// @return Derived reference
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

/// @brief Base CRTP bidirectional iterator class.
///
/// This class implements all bidirectional iterator requirements, and requires Derived to implement the following:
///     - Reference operator*() const
///     - Pointer operator->() const
///     - void increment()
///     - bool equals(const MyIterator& other) const
///     - void decrement()
///
/// @tparam Derived CRTP Derived class
/// @tparam T The iterator's underlying value type
/// @tparam Difference The type to define the difference between two iterators
/// @tparam Pointer Pointer to the iterator's value type
/// @tparam Reference Reference to the iterators value type
/// @tparam Category Category The type of iterator
template <typename Derived, typename T, typename Difference = std::ptrdiff_t, typename Pointer = T*,
          typename Reference = T&, typename Category = std::bidirectional_iterator_tag>
class bidirectional_iterator : public forward_iterator<Derived, T, Difference, Pointer, Reference, Category> {
  public:
    /// @brief Pre-decrement operator
    /// @return A reference to this iterator after the decrement
    Derived& operator--() {
        this->derived().decrement();
        return this->derived();
    }

    /// @brief Post-decrement operator
    /// @param
    /// @return Copy of this iterator before the decrement
    Derived operator--(int) {
        Derived temp = this->derived();
        this->derived().decrement();
        return temp;
    }

  protected:
    // ==== bidirectional_iterator should not be constructed directly ====
    bidirectional_iterator() = default; // NOLINT(bugprone-crtp-constructor-accessibility)
};

/// @brief Base CRTP random access iterator class.
///
/// This class implements all random access iterator requirements, and requires Derived to implement the following:
///     - Reference operator*() const
///     - Pointer operator->() const
///     - void increment()
///     - bool equals(const MyIterator& other) const
///     - void decrement()
///     - void advance(Difference n)
///     - Difference distance_to(const MyIterator& other) const
///     - bool less_than(const MyIterator& other) const
///
/// @tparam Derived CRTP Derived class
/// @tparam T The iterator's underlying value type
/// @tparam Difference The type to define the difference between two iterators
/// @tparam Pointer Pointer to the iterator's value type
/// @tparam Reference Reference to the iterators value type
/// @tparam Category Category The type of iterator
template <typename Derived, typename T, typename Difference = std::ptrdiff_t, typename Pointer = T*,
          typename Reference = T&, typename Category = std::random_access_iterator_tag>
class random_access_iterator : public bidirectional_iterator<Derived, T, Difference, Pointer, Reference, Category>,
                               public comparable<Derived> {
  public:
    /// @brief Addition assignment operator. Advances this iterator by n.
    /// @param n Amount to advance this iterator
    /// @return A reference to this iterator
    Derived& operator+=(Difference n) {
        this->derived().advance(n);
        return this->derived();
    }

    /// @brief Subtraction assignment operator. Retreats this iterator by n.
    /// @param n Amount to retreats this iterator
    /// @return A reference to this iterator
    Derived& operator-=(Difference n) {
        this->derived().advance(-n);
        return this->derived();
    }

    /// @brief Addition operator
    /// @param n Amount to add to this iterator
    /// @return Iterator advanced by n
    Derived operator+(Difference n) const {
        Derived temp = this->derived();
        temp += n;
        return temp;
    }

    /// @brief Addition operator
    /// @param n Amount to add to this iterator
    /// @param it The iterator to add to
    /// @return Iterator advanced by n
    friend Derived operator+(Difference n, const Derived& it) { return it + n; }

    /// @brief Subtraction operator
    /// @param n Amount to subtract to this iterator
    /// @return An Iterator retreated by n
    Derived operator-(Difference n) const {
        Derived temp = this->derived();
        temp -= n;
        return temp;
    }

    /// @brief Subtraction operator
    /// @param lhs left hand side iterator
    /// @param rhs right had side iterator
    /// @return The difference between two iterators
    friend Difference operator-(const Derived& lhs, const Derived& rhs) { return lhs.distance_to(rhs); }

    /// @brief Subscript operator
    /// @param n Number of elements to offset from the current iterator position
    /// @return A reference to the iterator at *this + n
    Reference operator[](Difference n) const { return *(this->derived() + n); }

  protected:
    // ==== random_access_iterator should not be constructed directly ====
    random_access_iterator() = default; // NOLINT(bugprone-crtp-constructor-accessibility)
};

/// @brief Class to reverse an iterator
/// @tparam Iterator The base iterator class to reverse
template <typename Iterator>
class reverse_iterator
    : public iterator_base_selector<
          typename std::iterator_traits<Iterator>::iterator_category, reverse_iterator<Iterator>,
          typename std::iterator_traits<Iterator>::value_type, typename std::iterator_traits<Iterator>::difference_type,
          typename std::iterator_traits<Iterator>::pointer, typename std::iterator_traits<Iterator>::reference>::type {
  public:
    using traits = std::iterator_traits<Iterator>;

    static_assert(std::is_base_of<std::bidirectional_iterator_tag, typename traits::iterator_category>::value,
                  "reverse_iterator requires bidirectional or better iterator");

    /// @brief Default constructor
    /// Reverses Iterator calling Iterator's default constructor
    /// NOTE: Constructor is only enabled for iterators that are at least bidirectional iterators
    reverse_iterator() : m_base() {}

    /// @brief Iterator constructor. Reverses Iterator it
    /// @param it Iterator to reverse
    reverse_iterator(Iterator it) : m_base(it) {}

    /// @brief Implicitly defined Copy constructor.
    reverse_iterator(const reverse_iterator&) = default;

    /// @brief Implicitly defined Move constructor.
    reverse_iterator(reverse_iterator&&) = default;

    /// @brief Implicitly defined destructor.
    ~reverse_iterator() = default;

    /// @brief Implicitly defined assignment operator
    /// @return Reference to this object
    reverse_iterator& operator=(const reverse_iterator&) = default;

    /// @brief Implicitly defined move assignment operator
    /// @return Reference to this object
    reverse_iterator& operator=(reverse_iterator&&) = default;

    /// @brief Const conversion constructor
    /// @tparam OtherIterator The const version this reverse_iterator
    /// NOTE: This constructor is only enabled if OtherIterator::pointer is convertible to this::pointer
    /// @param other The other iterator
    template <typename OtherIterator,
              typename = std::enable_if_t<std::is_convertible_v<typename std::iterator_traits<OtherIterator>::pointer,
                                                                typename traits::pointer>>>
    reverse_iterator(const reverse_iterator<OtherIterator>& other) : m_base(other.base()) {}

    /// @brief Get the reverse iterator's base iterator
    /// @return Base iterator object
    Iterator base() const { return m_base; }

    /// @brief Dereference operator
    /// @return A reference to this reverse iterator object
    typename traits::reference operator*() const {
        Iterator tmp = m_base;
        return *--tmp;
    }

    /// @brief Class member access (arrow) operator
    /// @return A pointer to this reverse iterator object
    typename traits::pointer operator->() const {
        Iterator tmp = m_base;
        --tmp;
        return tmp.operator->();
    }

    /// @brief Increments the reverse iterator
    void increment() { --m_base; }

    /// @brief Determines if reverse_iterator other is equal to this
    /// @param other Other reverse_iterator
    /// @return True if other is equal to this, false otherwise
    bool equals(const reverse_iterator& other) const { return m_base == other.m_base; }

    /// @brief Decrements the reverse iterator
    void decrement() { ++m_base; };

    /// @brief Advances the reverse iterator
    /// NOTE: Function is only available for iterators that are at least random access iterators
    /// @param n Number of elements to advance
    void advance(typename traits::difference_type n) { m_base -= n; };

    /// @brief Gets the distance from this iterator to other
    /// NOTE: Function is only available for iterators that are at least random access iterators
    /// @param other Other reverse iterator
    /// @return The distance between this and other
    typename traits::difference_type distance_to(const reverse_iterator& other) const { return other.m_base - m_base; }

    /// @brief Determines if reverse_iterator other is less than this
    /// NOTE: Function is only available for iterators that are at least random access iterators
    /// @param other Other reverse_iterator
    /// @return True if other is less than this, false otherwise
    bool less_than(const reverse_iterator& other) const { return other.m_base < m_base; }

  private:
    Iterator m_base;
};

} // namespace stdx

#endif
