#ifndef CTYPE_H
#define CTYPE_H

namespace stdx {

/**
 * @brief Checks whether c is an alphabetic letter (a-z) ignoreing case.
 *
 * @param c The character to check.
 * @return True if c is a letter a-z ignoring case, false otherwise.
 */
bool is_alpha(char c);

/**
 * @brief Checks whether c is a decimal digit.
 *
 * @param c The character to check.
 * @return True if c is 0 - 9, false otherwise.
 */
bool is_digit(char c);

/**
 * @brief Checks whether c is a digit of the provided base.
 *
 * @param c The character to check.
 * @param base The base of the character to check in range 1 - 36.
 * @return True if character c is a valid digit of base base, false otherwise.
 */
bool is_digit_of_base(char c, int base);

/**
 * @brief Convert a character c to a digit where c is a valid digit of the provided base.
 *
 * @param c The character to convert.
 * @param base The base of the character to convert (default 10 for decimal)
 * @return Digit representation of character c, or -1 if the character is not a valid character of base base.
 */
int char_to_digit(char c, int base = 10);

/**
 * @brief Convert a uppercase letter (A-Z) to lowercase.
 *
 * Converts c to its lowercase equivalent if c is an uppercase letter and has a lowercase equivalent.
 * If no such conversion is possible, the value returned is c unchanged.
 *
 * @param c Character to be converted.
 * @return The lowercase equivalent to c, if such value exists, or c (unchanged) otherwise.
 */
char to_lower(char c);

/**
 * @brief Convert a lowercase letter (a-z) to uppercase.
 *
 * Converts c to its uppercase equivalent if c is a lowercase letter and has a uppercase equivalent.
 * If no such conversion is possible, the value returned is c unchanged.
 *
 * @param c Character to be converted.
 * @return The uppercase equivalent to c, if such value exists, or c (unchanged) otherwise.
 */
char to_upper(char c);

} // namespace stdx

#endif
