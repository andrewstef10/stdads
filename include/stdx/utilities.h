#ifndef UTILITIES_H
#define UTILITIES_H

namespace stdx {

/// @brief Converts a reference of type T to const T
/// @tparam T Type to convert to const
/// @param value T reference to convert to const T
/// @return Constant T reference
template <typename T>
constexpr const T& as_const(T& value) noexcept {
    return value;
}

} // namespace stdx

#endif
