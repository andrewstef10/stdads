#ifndef STDXLIB_H
#define STDXLIB_H

namespace stdx {

/**
 * @brief Parses the C-string str interpreting its content as an integral number of the specified base, which is
 * returned as a long int value.
 *
 * The function first discards as many whitespace characters as necessary until the first non-whitespace character is
 * found. Then, starting from this character, takes as many characters as possible that are valid following a syntax
 * that depends on the base parameter, and interprets them as a numerical value. Finally, a pointer to the first
 * character following the integer representation in str is stored in the object pointed by endptr.
 *
 * If the value of base is zero, the syntax expected is similar to that of integer constants, which is formed by a
 * succession of:
 * - An optional sign character (+ or -)
 * - An optional prefix indicating octal or hexadecimal base ("0" or "0x"/"0X" respectively)
 * - A sequence of decimal digits (if no base prefix was specified) or either octal or hexadecimal digits if a specific
 * prefix is present
 *
 * If the base value is between 1 and 36, the format expected for the integral number is a succession of any of the
 * valid digits and/or letters needed to represent integers of the specified radix (starting from '0' and up to 'z'/'Z'
 * for radix 36). The sequence may optionally be preceded by a sign (either + or -) and, if base is 16, an optional "0x"
 * or "0X" prefix.
 *
 * @param str C-string beginning with the representation of an integral number.
 * @param base Numerical base (radix) that determines the valid characters and their interpretation.
 *             If this is 0, the base used is determined by the format in the sequence (see above).
 *             Default param option is base 10 (decimal).
 * @param endptr Reference to an object of type char*, whose value is set by the function to the next character in str
 * after the numerical value. This parameter can also be a null pointer (default param option), in which case it is not
 * used.
 * @return Integral number representation of str, or 0 if no valid conversion could be performed.
 */
long strtol(const char* str, int base = 10, char** endptr = nullptr);
} // namespace stdx

#endif
