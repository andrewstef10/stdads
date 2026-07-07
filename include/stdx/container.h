#ifndef CONTAINER_H
#define CONTAINER_H

#include <cstddef>

namespace stdx {

/// @brief A container provides a CRTP base class for all data structures
/// @tparam Derived The derived class implementing this base class
/// @tparam T The element type stored in the container
template <typename Derived, typename T>
class container {
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;

    /// @brief Returns the number of elements in the container.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return The number of elements in the container.
    size_type size() const noexcept { return derived().m_size; }

    /// @brief Checks if the container has no elements.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return true if the container is empty, false otherwise.
    bool empty() const noexcept { return size() == 0; }

    /// @brief Returns the maximum number of elements the container is able to hold due to system or implementation
    /// limitations.
    /// @details Time:  O(1)
    ///          Space: O(1)
    /// @return Maximum number of elements.
    constexpr size_type max_size() const noexcept { return static_cast<size_type>(-1) / sizeof(value_type); }

  protected:
    container() = default;
    ~container() = default;
    container(const container&) = default;
    container(container&&) noexcept = default;
    container& operator=(const container&) = default;
    container& operator=(container&&) noexcept = default;

  private:
    /// @brief Helper function to cast this to a Derived reference
    /// @return Derived reference
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};
} // namespace stdx

#endif
