#include <stdx/string.h>

namespace stdx {

const void* memchr(const void* ptr, int value, std::size_t num) {
    const unsigned char* p = static_cast<const unsigned char*>(ptr);
    for (; num > 0; --num) {
        if (*p == static_cast<unsigned char>(value)) {
            return p; // found value
        }
        ++p;
    }
    return nullptr; // value not found, return nullptr
}

int memcmp(const void* ptr1, const void* ptr2, std::size_t num) {
    if (ptr1 == ptr2) {
        return 0; // If both pointers are to the same location, they are equal
    }

    const unsigned char* p1 = static_cast<const unsigned char*>(ptr1);
    const unsigned char* p2 = static_cast<const unsigned char*>(ptr2);
    for (; num > 0; --num) {
        if (*p1 != *p2) {
            // If *p1 < *p2, return value < 0, otherwise > 0
            return static_cast<int>(*p1) - static_cast<int>(*p2);
        }
        ++p1;
        ++p2;
    }
    return 0; // memory blocks are equal
}

void* memcpy(void* destination, const void* source, std::size_t num) {
    unsigned char* dest = static_cast<unsigned char*>(destination);
    const unsigned char* src = static_cast<const unsigned char*>(source);
    for (; num > 0; --num) {
        *dest = *src;
        ++dest;
        ++src;
    }
    return destination;
}

void* memset(void* ptr, int value, std::size_t num) {
    unsigned char* dest = static_cast<unsigned char*>(ptr);
    for (; num > 0; --num) {
        *dest = static_cast<unsigned char>(value);
        ++dest;
    }
    return ptr;
}

char* strcat(char* destination, const char* source) {
    char* dest = destination; // save a copy so destination can be returned
    while (*dest != '\0') {
        ++dest;
    } // find the end of the dest String

    while (*source != '\0') {
        *dest = *source;
        ++source;
        ++dest;
    }
    *dest = '\0';

    return destination;
}

const char* strchr(const char* str, int character) {
    while (*str != '\0') {
        if (*str == static_cast<char>(character)) {
            return str;
        }
        ++str;
    }

    // check for null term
    if (*str == static_cast<char>(character)) {
        return str;
    }
    return nullptr; // char not found
}

int strcmp(const char* str1, const char* str2) {
    // If both pointers are to the same location, they are equal
    if (str1 == str2) {
        return 0;
    }

    while (*str1 != '\0' && *str2 != '\0' && *str1 == *str2) {
        ++str1;
        ++str2;
    }
    return static_cast<int>(*str1) - static_cast<int>(*str2);
}

char* strcpy(char* destination, const char* source) {
    // If both pointers are to the same location, they are equal
    if (destination == source) {
        return destination;
    }

    char* dest = destination; // save of copy of dest to iterate
    while (*source != '\0') {
        *dest = *source;
        ++dest;
        ++source;
    }

    *dest = '\0'; // copy null term
    return destination;
}

std::size_t strlen(const char* str) {
    std::size_t len = 0;
    while (*str != '\0') {
        ++len;
        ++str;
    }
    return len;
}

bool contains(const char* str, char c) {
    bool result = false;
    for (; *str != '\0'; ++str) {
        if (*str == c) {
            result = true;
            break;
        }
    }
    return result;
}

bool contains_any(const char* str, const char* chars) {
    bool result = false;
    for (; *chars != '\0'; ++chars) {
        if (contains(str, *chars)) {
            result = true;
            break;
        }
    }
    return result;
}

namespace helper {
template <typename CharT>
CharT* trim_front_impl(CharT* str, const char* trimChars) {
    while (*str != '\0' && contains(trimChars, *str)) {
        ++str;
    }
    return str;
}
} // namespace helper

const char* trim_front(const char* str, const char* trimChars) { return helper::trim_front_impl(str, trimChars); }

char* trim_front(char* str, const char* trimChars) { return helper::trim_front_impl(str, trimChars); }

char* trim_back(char* str, std::size_t len, const char* trimChars) {
    if (len == 0) {
        return str;
    }

    char* strEnd = str + len - 1;
    while (strEnd > str && contains(trimChars, *strEnd)) {
        *strEnd = '\0';
        --strEnd;
    }

    // check first character in str if all other trailing characters were trimmed
    if (str == strEnd && contains(trimChars, *strEnd)) {
        *strEnd = '\0';
    }
    return str;
}

char* trim(char* str, std::size_t len, const char* trimChars) {
    trim_back(str, len, trimChars);
    return trim_front(str, trimChars);
}
} // namespace stdx
