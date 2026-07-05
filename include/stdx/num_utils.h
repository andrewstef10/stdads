#ifndef NUM_UTILS_H
#define NUM_UTILS_H

namespace stdx {

/**
 * @brief Get if a value val is within values low and high (inclusive).
 *
 * This function assumes that the type passed in has properly implemented <= and >= operators.
 *
 * @param val Value to check is within low and high (inclusive)
 * @param low The low value.
 * @param high The high value.
 * @return True if val is greater than or equal to low AND val is less than or equal to high.
 */
template <typename T>
bool within(const T& val, const T& low, const T& high);

/**
 * @brief Get if a value val is within values low and high (exclusive).
 *
 * This function assumes that the type passed in has properly implemented < and > operators.
 *
 * @param val Value to check is within low and high (exclusive)
 * @param low The low value.
 * @param high The high value.
 * @return True if val is greater than low AND val is less than high.
 */
template <typename T>
bool within_exclusive(const T& val, const T& low, const T& high);

/**
 * @brief Get if a value val is outside values low and high (exclusive).
 *
 * This function assumes that the type passed in has properly implemented < and > operators.
 *
 * @param val Value to check is outside low and high (exclusive)
 * @param low The low value.
 * @param high The high value.
 * @return True if val is less than low OR val is greater than high.
 */
template <typename T>
bool outside(const T& val, const T& low, const T& high);

/**
 * @brief Get if a value val is outside values low and high (inclusive).
 *
 * This function assumes that the type passed in has properly implemented <= and >= operators.
 *
 * @param val Value to check is outside low and high (inclusive)
 * @param low The low value.
 * @param high The high value.
 * @return True if val is less than or equal to low OR val is greater than or equal to high.
 */
template <typename T>
bool outside_inclusive(const T& val, const T& low, const T& high);

/**
 * @brief Clamp a value val so that it is within a range.
 *
 * This function assumes that the type passed in has properly implemented <, >, and = operators.
 *
 * @param val Value to clamp
 * @param low The lower limit.
 * @param high The upper limit.
 * @return True if the value val was clamped.
 */
template <typename T>
bool clamp(T& val, const T& low, const T& high);

// inline implementation //

template <typename T>
inline bool within(const T& val, const T& low, const T& high) {
    return val >= low && val <= high;
}

template <typename T>
inline bool within_exclusive(const T& val, const T& low, const T& high) {
    return val > low && val < high;
}

template <typename T>
inline bool outside(const T& val, const T& low, const T& high) {
    return val < low || val > high;
}

template <typename T>
inline bool outside_inclusive(const T& val, const T& low, const T& high) {
    return val <= low || val >= high;
}

template <typename T>
inline bool clamp(T& val, const T& low, const T& high) {
    bool limited = false;
    if (val < low) {
        val = low;
        limited = true;
    } else if (val > high) {
        val = high;
        limited = true;
    }
    return limited;
}

} // namespace stdx

#endif
