#ifndef COMPARABLE_H
#define COMPARABLE_H

namespace stdx {

/// @brief CRTP base class that provides full comparison operators.
///
/// This class automatically provides all relational
/// comparison operators (`<`, `>`, `<=`, `>=`) for a derived type by requiring
/// the implementation of a single member function:
///
///     bool less_than(const Derived& other) const;
///
/// All comparison operators are defined in terms of `less_than`, ensuring
/// consistent and minimal implementation effort for derived types.
///
/// Example usage:
/// @code
/// struct MyType : public comparable<MyType> {
///     int value;
///
///     bool less_than(const MyType& other) const {
///         return value < other.value;
///     }
/// };
/// @endcode
///
/// @note The less_than function should define a strict weak ordering:
/// - Irreflexive: a.less_than(a) == false
/// - Asymmetric: if a.less_than(b) then !b.less_than(a)
/// - Transitive: if a.less_than(b) and b.less_than(c), then a.less_than(c)
///
/// @tparam Derived The type that inherits from this class and implements:
/// - bool equals(const Derived&) const  (from equatable)
/// - bool less_than(const Derived&) const
template <typename Derived>
class comparable {
  public:
    /// @brief Less than operator
    /// @param lhs left hand sid
    /// @param rhs right had side
    /// @return True if lhs is less than rhs, false otherwise.
    friend constexpr bool operator<(const Derived& lhs, const Derived& rhs) { return lhs.less_than(rhs); }

    /// @brief Greater than operator
    /// @param lhs left hand side
    /// @param rhs right had side
    /// @return True if lhs is greater than rhs, false otherwise.
    friend constexpr bool operator>(const Derived& lhs, const Derived& rhs) { return rhs.less_than(lhs); }

    /// @brief Less than or equal to operator
    /// @param lhs left hand side
    /// @param rhs right had side
    /// @return True if lhs is less than or equal to rhs, false otherwise.
    friend constexpr bool operator<=(const Derived& lhs, const Derived& rhs) { return !rhs.less_than(lhs); }

    /// @brief Greater than or equal to operator
    /// @param lhs left hand side
    /// @param rhs right had side
    /// @return True if lhs is greater than or equal to rhs, false otherwise.
    friend constexpr bool operator>=(const Derived& lhs, const Derived& rhs) { return !lhs.less_than(rhs); }

  protected:
    // ==== comparable should not be constructed directly ====
    comparable() = default; // NOLINT(bugprone-crtp-constructor-accessibility)
};
} // namespace stdx

#endif
