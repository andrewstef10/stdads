#ifndef MATH_H
#define MATH_H

namespace stdx {

/**
 * @brief Gets the absolute value of a number
 *
 * @param value Value to get absolute value of
 * @return absolute value of the input
 */
int abs(int value);
long abs(long value);
long long abs(long long value);
float abs(float value);
double abs(double value);
long double abs(long double value);

/**
 * @brief Raises a number to a power
 *
 * This function raises the base to the exponent exp.
 *
 * @param base Base value
 * @param exp Exponent value
 * @return base raised to the power of exp
 */
float pow(float base, int exp);
double pow(double base, int exp);

} // namespace stdx

#endif
