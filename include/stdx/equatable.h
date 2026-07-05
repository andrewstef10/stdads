#ifndef EQUATABLE_H
#define EQUATABLE_H

namespace stdx {

/// @brief CRTP base class that provides equality and inequality operators.
///
/// This class enables a derived type to automatically gain `operator==` and
/// `operator!=` by implementing a single member function:
///
///     bool equals(const Derived& other) const;
///
/// The comparison operators are defined in terms of this `equals` function,
/// ensuring consistent equality semantics across all derived types.
///
/// Example usage:
/// @code
/// struct MyType : public equatable<MyType> {
///     int value;
///
///     bool equals(const MyType& other) const {
///         return value == other.value;
///     }
/// };
/// @endcode
///
/// @tparam Derived The type that inherits from this class and implements
/// `bool equals(const Derived&) const`.
template <typename Derived>
class equatable {
  public:
    /// @brief Equality operator
    /// @param lhs left hand side
    /// @param rhs right had side
    /// @return True if lhs is equal to rhs, false otherwise.
    friend bool operator==(const Derived& lhs, const Derived& rhs) { return lhs.equals(rhs); }

    /// @brief Inequality operator
    /// @param lhs left hand side iterator
    /// @param rhs right had side iterator
    /// @return True if lhs is not equal to rhs, false otherwise.
    friend bool operator!=(const Derived& lhs, const Derived& rhs) { return !lhs.equals(rhs); }

  protected:
    // ==== equatable should not be constructed directly ====
    equatable() = default; // NOLINT(bugprone-crtp-constructor-accessibility)
};
} // namespace stdx

#endif
