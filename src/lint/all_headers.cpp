// Includes all header-only template headers so clang-tidy can reach them
// via the src/ compilation units without scanning test files.
// Only include header files that are not included by any other source file located in src/ to avoid duplicate warnings
#include <stdx/array.h>
#include <stdx/array_list.h>
#include <stdx/fixed_array_list.h>
