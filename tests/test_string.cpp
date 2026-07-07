#include <cstring>
#include <gtest/gtest.h>
#include <stdx/string.h>

// memchar tests
TEST(MemchrTest, FindCharacter) {
    const char* str = "Hello, World!";
    const void* result = stdx::memchr(str, 'W', 13);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*(static_cast<const char*>(result)), 'W');
}

TEST(MemchrTest, FindsExistingByte) {
    const char buffer[] = "hello world";
    const void* result = stdx::memchr(buffer, 'w', sizeof(buffer));
    EXPECT_EQ(static_cast<const void*>(buffer + 6), result);
}

TEST(MemchrTest, FindsLastByte) {
    const char buffer[] = "hello";
    const void* result = stdx::memchr(buffer, 'o', sizeof(buffer));
    EXPECT_EQ(static_cast<const void*>(buffer + 4), result);
}

TEST(MemchrTest, FindsFirstOccurrenceOnly) {
    const char buffer[] = "banana";
    const void* result = stdx::memchr(buffer, 'a', sizeof(buffer));
    EXPECT_EQ(static_cast<const void*>(buffer + 1), result); // first 'a'
}

TEST(MemchrTest, ReturnsNullWhenNotFound) {
    const char buffer[] = "abcdef";
    const void* result = stdx::memchr(buffer, 'z', sizeof(buffer));
    EXPECT_EQ(nullptr, result);
}

TEST(MemchrTest, RespectsLengthLimit) {
    const char buffer[] = "hello world";

    // Search only first 5 bytes: "hello"
    const void* result = stdx::memchr(buffer, 'w', 5);
    EXPECT_EQ(nullptr, result); // 'w' is outside search range
}

TEST(MemchrTest, WorksWithBinaryData) {
    unsigned char buffer[] = {0x00, 0x10, 0xFF, 0x20, 0xFF};
    const void* result = stdx::memchr(buffer, 0xFF, sizeof(buffer));
    EXPECT_EQ(static_cast<const void*>(buffer + 2), result);
}

TEST(MemchrTest, FindsNullByte) {
    const char buffer[] = {'a', 'b', '\0', 'c', 'd'};
    const void* result = stdx::memchr(buffer, '\0', sizeof(buffer));
    EXPECT_EQ(static_cast<const void*>(buffer + 2), result);
}

TEST(MemchrTest, ZeroLengthReturnsNull) {
    const char buffer[] = "test";
    const void* result = stdx::memchr(buffer, 't', 0);
    EXPECT_EQ(nullptr, result);
}

TEST(MemchrTest, NullPointerWithZeroLength) {
    const void* result = stdx::memchr(nullptr, 'a', 0);
    EXPECT_EQ(nullptr, result);
}

// memcmp tests
TEST(MemcmpTest, EqualBuffers) {
    const char a[] = "hello";
    const char b[] = "hello";
    int result = stdx::memcmp(a, b, 5);
    EXPECT_EQ(0, result);
}

TEST(MemcmpTest, ReferenceEquals) {
    const char a[] = "hello";
    int result = stdx::memcmp(a, a, 5);
    EXPECT_EQ(0, result);
}

TEST(MemcmpTest, FirstLessThanSecond) {
    const char a[] = "abc";
    const char b[] = "abd";
    int result = stdx::memcmp(a, b, 3);
    EXPECT_LT(result, 0); // 'c' < 'd'
}

TEST(MemcmpTest, FirstGreaterThanSecond) {
    const char a[] = "abe";
    const char b[] = "abd";
    int result = stdx::memcmp(a, b, 3);
    EXPECT_GT(result, 0); // 'e' > 'd'
}

TEST(MemcmpTest, PartialCompareEqual) {
    const char a[] = "hello123";
    const char b[] = "hello999";
    int result = stdx::memcmp(a, b, 5); // only compare "hello"
    EXPECT_EQ(0, result);
}

TEST(MemcmpTest, RespectsLengthLimit) {
    const char a[] = "abcXXX";
    const char b[] = "abcYYY";
    int result = stdx::memcmp(a, b, 3); // compare only "abc"
    EXPECT_EQ(0, result);
}

TEST(MemcmpTest, BinaryDataComparison) {
    unsigned char a[] = {0x01, 0x02, 0xFF};
    unsigned char b[] = {0x01, 0x02, 0x10};
    int result = stdx::memcmp(a, b, 3);
    EXPECT_GT(result, 0); // 0xFF > 0x10
}

TEST(MemcmpTest, FindsDifferenceAtCorrectIndex) {
    unsigned char a[] = {0x10, 0x20, 0x30, 0x40};
    unsigned char b[] = {0x10, 0x20, 0x31, 0x40};
    int result = stdx::memcmp(a, b, 4);
    EXPECT_LT(result, 0); // 0x30 < 0x31
}

TEST(MemcmpTest, ZeroLengthReturnsEqual) {
    const char a[] = "abc";
    const char b[] = "xyz";
    int result = stdx::memcmp(a, b, 0);
    EXPECT_EQ(0, result);

    result = stdx::memcmp(a, a, 0);
    EXPECT_EQ(0, result);
}

TEST(MemcmpTest, NullptrWithZeroLength) {
    int result = stdx::memcmp(nullptr, nullptr, 0);
    EXPECT_EQ(0, result);
}

// memcpy tests
TEST(MemcpyTest, BasicCopy) {
    char src[] = "hello";
    char dst[6] = {};
    void* ret = stdx::memcpy(dst, src, 6);
    EXPECT_EQ(dst, ret);
    EXPECT_STREQ("hello", dst);
}

TEST(MemcpyTest, PartialCopy) {
    char src[] = "hello";
    char dst[6] = {};
    stdx::memcpy(dst, src, 3);
    EXPECT_EQ('h', dst[0]);
    EXPECT_EQ('e', dst[1]);
    EXPECT_EQ('l', dst[2]);
}

TEST(MemcpyTest, BinaryDataCopy) {
    unsigned char src[] = {0x01, 0xFF, 0x10, 0x20};
    unsigned char dst[4] = {};
    stdx::memcpy(dst, src, 4);
    EXPECT_EQ(0x01, dst[0]);
    EXPECT_EQ(0xFF, dst[1]);
    EXPECT_EQ(0x10, dst[2]);
    EXPECT_EQ(0x20, dst[3]);
}

TEST(MemcpyTest, ZeroLengthCopy) {
    char src[] = "hello";
    char dst[] = "world";
    stdx::memcpy(dst, src, 0);
    EXPECT_STREQ("world", dst); // unchanged
}

TEST(MemcpyTest, LargeCopy) {
    const std::size_t size = 1024;
    unsigned char src[size];
    unsigned char dst[size];

    for (std::size_t i = 0; i < size; ++i)
        src[i] = static_cast<unsigned char>(i % 256);

    stdx::memcpy(dst, src, size);

    for (std::size_t i = 0; i < size; ++i)
        EXPECT_EQ(src[i], dst[i]);
}

TEST(MemcpyTest, ReturnsDestinationPointer) {
    char src[] = "abc";
    char dst[4];
    void* ret = stdx::memcpy(dst, src, 4);
    EXPECT_EQ(dst, ret);
}

TEST(MemcpyTest, NullptrWithZeroLength) {
    void* ret = stdx::memcpy(nullptr, nullptr, 0);
    EXPECT_EQ(nullptr, ret);
}

// memset tests
TEST(MemsetTest, BasicSet) {
    char buffer[10];
    void* ret = stdx::memset(buffer, 'A', 10);

    EXPECT_EQ(buffer, ret);
    for (int i = 0; i < 10; ++i)
        EXPECT_EQ('A', buffer[i]);
}

TEST(MemsetTest, PartialSet) {
    char buffer[10] = {};
    stdx::memset(buffer, 'B', 5);

    for (int i = 0; i < 5; ++i)
        EXPECT_EQ('B', buffer[i]);
    for (int i = 5; i < 10; ++i)
        EXPECT_EQ(0, buffer[i]);
}

TEST(MemsetTest, BinaryValueSet) {
    unsigned char buffer[4];
    stdx::memset(buffer, 0xFF, 4);

    for (int i = 0; i < 4; ++i)
        EXPECT_EQ(0xFF, buffer[i]);
}

TEST(MemsetTest, ZeroValueSet) {
    char buffer[8];
    stdx::memset(buffer, 0, 8);

    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(0, buffer[i]);
}

TEST(MemsetTest, ZeroLengthDoesNothing) {
    char buffer[5] = {'a', 'b', 'c', 'd', 'e'};
    stdx::memset(buffer, 'X', 0);
    EXPECT_EQ('a', buffer[0]);
    EXPECT_EQ('b', buffer[1]);
    EXPECT_EQ('c', buffer[2]);
    EXPECT_EQ('d', buffer[3]);
    EXPECT_EQ('e', buffer[4]);
}

TEST(MemsetTest, LargeBufferSet) {
    const std::size_t size = 1024;
    unsigned char buffer[size];
    stdx::memset(buffer, 0xAB, size);

    for (std::size_t i = 0; i < size; ++i)
        EXPECT_EQ(0xAB, buffer[i]);
}

TEST(MemsetTest, ReturnsPointer) {
    char buffer[4];
    void* ret = stdx::memset(buffer, 1, 4);
    EXPECT_EQ(buffer, ret);
}

TEST(MemsetTest, NullptrWithZeroLength) {
    void* ret = stdx::memset(nullptr, 0, 0);
    EXPECT_EQ(nullptr, ret);
}

// strcat tests
TEST(StrCatTest, AppendToEmptyString) {
    char dest[32] = "";
    const char src[] = "hello";
    char* result = stdx::strcat(dest, src);
    EXPECT_EQ(dest, result);
    EXPECT_STREQ("hello", dest);
}

TEST(StrCatTest, AppendNonEmptyStrings) {
    char dest[32] = "hello";
    const char src[] = " world";
    stdx::strcat(dest, src);
    EXPECT_STREQ("hello world", dest);
}

TEST(StrCatTest, AppendSingleCharacter) {
    char dest[8] = "a";
    const char src[] = "b";
    stdx::strcat(dest, src);
    EXPECT_STREQ("ab", dest);
}

TEST(StrCatTest, AppendEmptySource) {
    char dest[16] = "hello";
    const char src[] = "";
    stdx::strcat(dest, src);
    EXPECT_STREQ("hello", dest);
}

TEST(StrCatTest, AppendToLongerString) {
    char dest[64] = "The quick ";
    const char src[] = "brown fox";
    stdx::strcat(dest, src);
    EXPECT_STREQ("The quick brown fox", dest);
}

// strchar tests
TEST(StrChrTest, FindsCharacterAtBeginning) {
    const char str[] = "hello";
    const char* result = stdx::strchr(str, 'h');
    ASSERT_NE(nullptr, result);
    EXPECT_EQ('h', *result);
    EXPECT_EQ(str, result);
}

TEST(StrChrTest, FindsCharacterInMiddle) {
    const char str[] = "hello";
    const char* result = stdx::strchr(str, 'l');
    ASSERT_NE(nullptr, result);
    EXPECT_EQ('l', *result);
    EXPECT_EQ(&str[2], result);
}

TEST(StrChrTest, FindsCharacterAtEnd) {
    const char str[] = "hello";
    const char* result = stdx::strchr(str, 'o');
    ASSERT_NE(nullptr, result);
    EXPECT_EQ('o', *result);
    EXPECT_EQ(&str[4], result);
}

TEST(StrChrTest, ReturnsFirstOccurrence) {
    const char str[] = "banana";
    const char* result = stdx::strchr(str, 'a');
    ASSERT_NE(nullptr, result);
    EXPECT_EQ(&str[1], result);
}

TEST(StrChrTest, CharacterNotFoundReturnsNullptr) {
    const char str[] = "hello";
    const char* result = stdx::strchr(str, 'z');
    EXPECT_EQ(nullptr, result);
}

TEST(StrChrTest, FindsNullTerminator) {
    const char str[] = "hello";
    const char* result = stdx::strchr(str, '\0');
    ASSERT_NE(nullptr, result);
    EXPECT_EQ('\0', *result);
    EXPECT_EQ(str + 5, result);
}

TEST(StrChrTest, EmptyString) {
    const char str[] = "";
    const char* result = stdx::strchr(str, 'a');
    EXPECT_EQ(nullptr, result);
}

TEST(StrChrTest, EmptyStringFindNullTerminator) {
    const char str[] = "";
    const char* result = stdx::strchr(str, '\0');
    ASSERT_NE(nullptr, result);
    EXPECT_EQ(str, result);
}

// strcmp tests
TEST(StrcmpTest, comparison) {
    EXPECT_EQ(0, stdx::strcmp("", ""));
    EXPECT_LT(stdx::strcmp("a", "b"), 0);
    EXPECT_GT(stdx::strcmp("b", "a"), 0);
    EXPECT_EQ(0, stdx::strcmp("test", "test"));
    EXPECT_LT(stdx::strcmp("abc", "abcd"), 0);
    EXPECT_GT(stdx::strcmp("abcd", "abc"), 0);
}

TEST(StrcmpTest, BothEmpty) {
    const char* a = "";
    const char* b = "";
    int result = stdx::strcmp(a, b);
    EXPECT_EQ(0, result);
}

TEST(StrcmpTest, EqualStrings) {
    const char* a = "hello";
    const char* b = "hello";
    int result = stdx::strcmp(a, b);
    EXPECT_EQ(0, result);
}

TEST(StrcmpTest, FirstLessThanSecond) {
    const char* a = "abc";
    const char* b = "abd";
    int result = stdx::strcmp(a, b);
    EXPECT_LT(result, 0);
}

TEST(StrcmpTest, FirstGreaterThanSecond) {
    const char* a = "abe";
    const char* b = "abd";
    int result = stdx::strcmp(a, b);
    EXPECT_GT(result, 0);
}

TEST(StrcmpTest, PrefixShorterIsLess) {
    const char* a = "abc";
    const char* b = "abcd";
    int result = stdx::strcmp(a, b);
    EXPECT_LT(result, 0);
}

TEST(StrcmpTest, PrefixLongerIsGreater) {
    const char* a = "abcd";
    const char* b = "abc";
    int result = stdx::strcmp(a, b);
    EXPECT_GT(result, 0);
}

TEST(StrcmpTest, CaseSensitivity) {
    const char* a = "abc";
    const char* b = "Abc";
    int result = stdx::strcmp(a, b);
    EXPECT_GT(result, 0); // 'a' > 'A' in ASCII
}

TEST(StrcmpTest, SpecialCharacters) {
    const char* a = "a\nb";
    const char* b = "a\tb";
    int result = stdx::strcmp(a, b);
    EXPECT_GT(result, 0); // '\n' (10) > '\t' (9)
}

TEST(StrcmpTest, EmbeddedNullTerminates) {
    const char a[] = {'a', 'b', '\0', 'c'};
    const char b[] = {'a', 'b', '\0', 'd'};
    int result = stdx::strcmp(a, b);
    EXPECT_EQ(0, result); // must stop at first '\0'
}

TEST(StrcmpTest, NullptrBoth) {
    const char* a = nullptr;
    const char* b = nullptr;
    int result = stdx::strcmp(a, b);
    EXPECT_EQ(0, result);
}

// strcpy tests
TEST(StrcpyTest, BasicCopy) {
    char src[] = "hello";
    char dst[6] = {};
    char* ret = stdx::strcpy(dst, src);
    EXPECT_EQ(dst, ret);
    EXPECT_STREQ("hello", dst);
}

TEST(StrcpyTest, EmptyStringCopy) {
    char src[] = "";
    char dst[1] = {'X'};
    stdx::strcpy(dst, src);
    EXPECT_EQ('\0', dst[0]);
}

TEST(StrcpyTest, SingleCharacterCopy) {
    char src[] = "A";
    char dst[2];
    stdx::strcpy(dst, src);
    EXPECT_EQ('A', dst[0]);
    EXPECT_EQ('\0', dst[1]);
}

TEST(StrcpyTest, IncludesNullTerminator) {
    char src[] = "abc";
    char dst[4];
    stdx::strcpy(dst, src);
    EXPECT_EQ('a', dst[0]);
    EXPECT_EQ('b', dst[1]);
    EXPECT_EQ('c', dst[2]);
    EXPECT_EQ('\0', dst[3]); // must copy terminator
}

TEST(StrcpyTest, ReferenceEqual) {
    char src[] = "abc";
    stdx::strcpy(src, src);
    EXPECT_EQ('a', src[0]);
    EXPECT_EQ('b', src[1]);
    EXPECT_EQ('c', src[2]);
    EXPECT_EQ('\0', src[3]); // must copy terminator
}

TEST(StrcpyTest, BinarySafeCopy) {
    const char src[] = {'a', 'b', '\0', 'c', 'd', '\0'};
    char dst[6];
    stdx::strcpy(dst, src);
    EXPECT_EQ('a', dst[0]);
    EXPECT_EQ('b', dst[1]);
    EXPECT_EQ('\0', dst[2]); // stop at first null
}

TEST(StrcpyTest, LargeStringCopy) {
    const std::size_t size = 1024;
    char src[size];
    char dst[size];

    for (std::size_t i = 0; i < size - 1; ++i)
        src[i] = 'A';
    src[size - 1] = '\0';

    stdx::strcpy(dst, src);
    EXPECT_STREQ(src, dst);
}

TEST(StrcpyTest, ReturnsDestinationPointer) {
    char src[] = "abc";
    char dst[4];
    char* ret = stdx::strcpy(dst, src);
    EXPECT_EQ(dst, ret);
}

// strlen tests
TEST(StrlenTest, EmptyString) {
    const char* s = "";
    std::size_t result = stdx::strlen(s);
    EXPECT_EQ(0u, result);
}

TEST(StrlenTest, SingleCharacter) {
    const char* s = "a";
    std::size_t result = stdx::strlen(s);
    EXPECT_EQ(1u, result);
}

TEST(StrlenTest, NormalString) {
    const char* s = "hello";
    std::size_t result = stdx::strlen(s);
    EXPECT_EQ(5u, result);
}

TEST(StrlenTest, StringWithSpaces) {
    const char* s = "hello world";
    std::size_t result = stdx::strlen(s);
    EXPECT_EQ(11u, result);
}

TEST(StrlenTest, StringWithSpecialCharacters) {
    const char* s = "a\tb\nc";
    std::size_t result = stdx::strlen(s);
    EXPECT_EQ(5u, result); // '\t' and '\n' count as characters
}

TEST(StrlenTest, StopsAtNullTerminator) {
    const char s[] = {'a', 'b', '\0', 'c', 'd'};
    std::size_t result = stdx::strlen(s);
    EXPECT_EQ(2u, result); // must stop at first '\0'
}

TEST(StrlenTest, LongString) {
    const char* s = "abcdefghijklmnopqrstuvwxyz";
    std::size_t result = stdx::strlen(s);
    EXPECT_EQ(26u, result);
}

// -------------------------
// contains
// -------------------------

TEST(ContainsTest, FindsExistingChar) {
    const char* str = "hello";
    EXPECT_TRUE(stdx::contains(str, 'e'));
    EXPECT_TRUE(stdx::contains(str, 'h'));
    EXPECT_TRUE(stdx::contains(str, 'l'));
    EXPECT_TRUE(stdx::contains(str, 'o'));
}

TEST(ContainsTest, ReturnsFalseIfMissing) {
    const char* str = "hello";
    EXPECT_FALSE(stdx::contains(str, 'z'));
    EXPECT_FALSE(stdx::contains(str, '\0'));
}

TEST(ContainsTest, EmptyString) {
    const char* str = "";
    EXPECT_FALSE(stdx::contains(str, 'a'));
    EXPECT_FALSE(stdx::contains(str, '\0'));
}

// -------------------------
// contains_any
// -------------------------

TEST(ContainsAnyTest, FindsAnyMatch) {
    const char* str = "hello";
    EXPECT_TRUE(stdx::contains_any(str, "xyzle"));
    EXPECT_TRUE(stdx::contains_any(str, "h"));
    EXPECT_TRUE(stdx::contains_any(str, "l"));
    EXPECT_TRUE(stdx::contains_any(str, "o"));
    EXPECT_TRUE(stdx::contains_any(str, "lh"));
}

TEST(ContainsAnyTest, NoMatch) {
    const char* str = "hello";
    EXPECT_FALSE(stdx::contains_any(str, "xyz"));
    EXPECT_FALSE(stdx::contains_any(str, ""));
    EXPECT_FALSE(stdx::contains_any(str, "\0"));
}

TEST(ContainsAnyTest, EmptyStr) {
    const char* str = "";
    EXPECT_FALSE(stdx::contains_any(str, "abc"));
    EXPECT_FALSE(stdx::contains_any(str, ""));
    EXPECT_FALSE(stdx::contains_any(str, "\0"));
}

// -------------------------
// trim_front
// -------------------------

TEST(TrimFrontConstTest, TrimsSpaces) {
    const char* cstr = "   hello";
    char buffer[] = "   hello";

    const char* cresult = stdx::trim_front(cstr, " ");
    const char* cresultDefaultParams = stdx::trim_front(cstr);
    char* result = stdx::trim_front(buffer, " ");
    char* resultDefaultParams = stdx::trim_front(buffer);

    EXPECT_STREQ(cresult, "hello");
    EXPECT_EQ(cresult, cstr + 3);
    EXPECT_STREQ(cresultDefaultParams, "hello");
    EXPECT_EQ(cresultDefaultParams, cstr + 3);

    EXPECT_STREQ(result, "hello");
    EXPECT_EQ(result, buffer + 3);
    EXPECT_STREQ(resultDefaultParams, "hello");
    EXPECT_EQ(resultDefaultParams, buffer + 3);

    EXPECT_STREQ(buffer, "   hello");
}

TEST(TrimFrontConstTest, NoTrimNeeded) {
    const char* cstr = "hello";
    char buffer[] = "hello";
    const char* cresult = stdx::trim_front(cstr, " ");
    char* result = stdx::trim_front(buffer, " ");

    EXPECT_STREQ(cresult, "hello");
    EXPECT_EQ(cresult, cstr);
    EXPECT_STREQ(result, "hello");
    EXPECT_EQ(result, buffer);
}

TEST(TrimFrontConstTest, TrimEmptyString) {
    const char* cstr = " hello";
    char buffer[] = " hello";
    const char* cresult = stdx::trim_front(cstr, "");
    const char* result = stdx::trim_front(buffer, "");

    EXPECT_STREQ(cresult, " hello");
    EXPECT_EQ(cresult, cstr);
    EXPECT_STREQ(result, " hello");
    EXPECT_EQ(result, buffer);
}

TEST(TrimFrontConstTest, TrimsMultipleChars) {
    const char* cstr = "!!!hello";
    char buffer[] = "!!!hello";
    const char* cresult = stdx::trim_front(cstr, "!");
    char* result = stdx::trim_front(buffer, "!");

    EXPECT_STREQ(cresult, "hello");
    EXPECT_STREQ(result, "hello");
}

TEST(TrimFrontConstTest, AllTrimmed) {
    const char* cstr = "aaaa";
    char buffer[] = "aaaa";
    const char* cresult = stdx::trim_front(cstr, "a");
    char* result = stdx::trim_front(buffer, "a");

    EXPECT_STREQ(cresult, "");
    EXPECT_STREQ(cresult, cstr + 4);
    EXPECT_STREQ(result, "");
    EXPECT_STREQ(result, buffer + 4);
}

TEST(TrimFrontConstTest, EmptyString) {
    const char* cstr = "";
    char buffer[] = "";

    EXPECT_STREQ(stdx::trim_front(cstr, "a"), "");
    EXPECT_STREQ(stdx::trim_front(cstr, ""), "");

    EXPECT_STREQ(stdx::trim_front(buffer, "a"), "");
    EXPECT_STREQ(stdx::trim_front(buffer, ""), "");
}

// -------------------------
// trim_back
// -------------------------

TEST(TrimBackTest, TrimsSpaces) {
    char buffer[] = "hello   ";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim_back(buffer, len, " ");

    EXPECT_STREQ(result, "hello");
    EXPECT_EQ(result, buffer);
}

TEST(TrimBackTest, NoTrimNeeded) {
    char buffer[] = "hello";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim_back(buffer, len, " ");
    char* result2 = stdx::trim_back(buffer, len, "");

    EXPECT_STREQ(result, "hello");
    EXPECT_EQ(result, buffer);

    EXPECT_STREQ(result2, "hello");
    EXPECT_EQ(result2, buffer);
}

TEST(TrimBackTest, TrimsExclamation) {
    char buffer[] = "hello!!!";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim_back(buffer, len, "!");

    EXPECT_STREQ(result, "hello");
    EXPECT_EQ(result, buffer);
}

TEST(TrimBackTest, AllTrimmed) {
    char buffer[] = "aaaa";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim_back(buffer, len, "a");

    EXPECT_STREQ(result, "");
    EXPECT_EQ(result[0], '\0');
    EXPECT_EQ(result, buffer);
}

TEST(TrimBackTest, TrimAllButFirst) {
    char buffer[] = "baaa";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim_back(buffer, len, "a");

    EXPECT_STREQ(result, "b");
    EXPECT_EQ(result[0], 'b');
    EXPECT_EQ(result[1], '\0');
    EXPECT_EQ(result, buffer);
}

TEST(TrimBackTest, TrimsEmptyString) {
    char buffer[] = "";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim_back(buffer, len, "!");

    EXPECT_STREQ(result, "");
    EXPECT_EQ(result, buffer);
}

// -------------------------
// trim (front + back)
// -------------------------

TEST(TrimTest, TrimsBothSides) {
    char buffer[] = "   hello   ";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim(buffer, len, " ");

    EXPECT_STREQ(result, "hello");
    EXPECT_EQ(result, buffer + 3);
}

TEST(TrimTest, OnlyFront) {
    char buffer[] = "   hello";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim(buffer, len, " ");

    EXPECT_STREQ(result, "hello");
    EXPECT_EQ(result, buffer + 3);
}

TEST(TrimTest, OnlyBack) {
    char buffer[] = "hello   ";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim(buffer, len, " ");

    EXPECT_STREQ(result, "hello");
    EXPECT_EQ(result, buffer);
}

TEST(TrimTest, AllTrimmed) {
    char buffer[] = "aaaa";
    std::size_t len = std::strlen(buffer);

    char* result = stdx::trim(buffer, len, "a");

    EXPECT_STREQ(result, "");
}

// String class constructors
TEST(StringConstructorTest, DefaultConstructorCreatesEmptyString) {
    stdx::string s;
    EXPECT_EQ(0u, s.size());
    EXPECT_STREQ("", s.c_str());

    stdx::string<100> largeStr;
    EXPECT_EQ(0u, largeStr.size());
    EXPECT_STREQ("", largeStr.c_str());

    stdx::string<10> smallStr;
    EXPECT_EQ(0u, smallStr.size());
    EXPECT_STREQ("", smallStr.c_str());

    stdx::string<0> zeroCapacityStr;
    EXPECT_EQ(0u, zeroCapacityStr.size());
    EXPECT_STREQ("", zeroCapacityStr.c_str());
}

TEST(StringConstructorTest, DefaultConstructorCapacity) {
    stdx::string s;
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, s.capacity());

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES - 10> smallStr;
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES - 10, smallStr.capacity());

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES + 10> largeStr;
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES + 10, largeStr.capacity());

    stdx::string<0> zeroCapacityStr;
    EXPECT_EQ(0, zeroCapacityStr.capacity());
}

TEST(StringConstructorTest, CStringConstructorCopiesString) {
    stdx::string s("hello");
    EXPECT_EQ(5u, s.size());
    EXPECT_STREQ("hello", s.c_str());
}

TEST(StringConstructorTest, CStringConstructorWithEmptyString) {
    stdx::string s("");
    EXPECT_EQ(0u, s.size());
    EXPECT_STREQ("", s.c_str());
}

TEST(StringConstructorTest, CStringConstructorCreatesDeepCopy) {
    const char* cstr = "hello";
    stdx::string str(cstr);
    EXPECT_NE(cstr, str.c_str()); // Addresses must differ (deep copy)
}

TEST(StringConstructorTest, CStringConstructorCapacity) {
    stdx::string s("hello");
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, s.capacity());

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES - 10> smallStr("hello");
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES - 10, smallStr.capacity());

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES + 10> largeStr("hello");
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES + 10, largeStr.capacity());

    stdx::string<0> zeroCapacityStr("");
    EXPECT_EQ(0, zeroCapacityStr.capacity());
}

TEST(StringConstructorTest, CStringConstructorGrows) {
    const char* cstr = "abcdefghijklmnopqrstuvwxyz"; // 26 chars > than default 24 characters;

    stdx::string s(cstr);
    EXPECT_EQ(26u, s.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES * 2, s.capacity()); // capacity always doubles when growing
    EXPECT_STREQ(cstr, s.c_str());

    stdx::string<10> smallStr(cstr);
    EXPECT_EQ(26u, smallStr.size());
    EXPECT_EQ(40u, smallStr.capacity()); // needs to grow twice
    EXPECT_STREQ(cstr, smallStr.c_str());
}

TEST(StringConstructorTest, CharConstructorCopiesChar) {
    stdx::string s('a');
    EXPECT_EQ(1u, s.size());
    EXPECT_STREQ("a", s.c_str());
}

TEST(StringConstructorTest, CharConstructorCreatesDeepCopy) {
    const char c = 'a';
    stdx::string str(c);
    EXPECT_NE(&c, str.c_str()); // Addresses must differ (deep copy)
}

TEST(StringConstructorTest, CharConstructorCapacity) {
    stdx::string s('a');
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, s.capacity());

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES - 10> smallStr('a');
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES - 10, smallStr.capacity());

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES + 10> largeStr('a');
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES + 10, largeStr.capacity());
}

TEST(StringConstructorTest, CharConstructorGrows) {
    stdx::string<0> s('!');
    EXPECT_EQ(1u, s.size());
    EXPECT_EQ(1u, s.capacity()); // 0 capacity edge case. Should grow to 1.
    EXPECT_STREQ("!", s.c_str());
}

TEST(StringConstructorTest, CopyConstructorCopiesContent) {
    stdx::string original("hello");
    stdx::string copy(original);
    EXPECT_EQ(original.size(), copy.size());
    EXPECT_STREQ(original.c_str(), copy.c_str());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, copy.capacity());

    stdx::string<100> largeStr("hello");
    stdx::string<100> largeCopy(largeStr);
    EXPECT_EQ(largeStr.size(), largeCopy.size());
    EXPECT_STREQ(largeStr.c_str(), largeCopy.c_str());
    EXPECT_EQ(100u, largeCopy.capacity());
}

TEST(StringConstructorTest, CopyConstructorCreatesDeepCopy) {
    stdx::string original("hello");
    stdx::string copy(original);
    EXPECT_NE(original.c_str(), copy.c_str()); // Addresses must differ (deep copy)
}

TEST(StringConstructorTest, CopyConstructorDifferentCapacities) {
    stdx::string<10> original("hello");
    stdx::string<15> copy(original);
    EXPECT_EQ(original.size(), copy.size());
    EXPECT_STREQ(original.c_str(), copy.c_str());
    EXPECT_NE(original.c_str(), copy.c_str()); // Addresses must differ (deep copy)
    EXPECT_EQ(15u, copy.capacity());
}

TEST(StringConstructorTest, CopyConstructorDifferentCapacitiesGrows) {
    stdx::string<10> original("hello");
    stdx::string<4> copy(original);
    EXPECT_EQ(original.size(), copy.size());
    EXPECT_STREQ(original.c_str(), copy.c_str());
    EXPECT_NE(original.c_str(), copy.c_str()); // Addresses must differ (deep copy)
    EXPECT_EQ(8u, copy.capacity());
}

// String class assignment operators
TEST(StringAssignmentTest, AssignFromAnotherString) {
    stdx::string a("hello");
    stdx::string b("world");
    a = b;

    EXPECT_EQ(5u, a.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, a.capacity());
    EXPECT_STREQ("world", a.c_str());
    EXPECT_NE(a.c_str(), b.c_str()); // deep copy
    EXPECT_NE(&a, &b);
}

TEST(StringAssignmentTest, AssignSelf) {
    stdx::string a("hello");
    const char* before = a.c_str();
    auto& self = a;
    a = self;

    EXPECT_EQ(5u, a.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, a.capacity());
    EXPECT_STREQ("hello", a.c_str());
    EXPECT_EQ(before, a.c_str()); // no reallocation required
}

TEST(StringAssignmentTest, AssignFromEmptyString) {
    stdx::string a("hello");
    stdx::string empty;
    a = empty;

    EXPECT_EQ(0u, a.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, a.capacity());
    EXPECT_STREQ("", a.c_str());
    EXPECT_NE(a.c_str(), empty.c_str()); // deep copy
    EXPECT_NE(&a, &empty);
}

TEST(StringAssignmentTest, AssignFromAnotherStringGrowsCapacity) {
    stdx::string a("hello");
    stdx::string b("abcdefghijklmnopqrstuvwxyz");
    a = b;

    EXPECT_EQ(26u, a.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES * 2, a.capacity());
    EXPECT_STREQ("abcdefghijklmnopqrstuvwxyz", a.c_str());
    EXPECT_NE(a.c_str(), b.c_str()); // deep copy
    EXPECT_NE(&a, &b);

    stdx::string<10> c("goodbye");
    stdx::string<10> d("abcdefghijklmnopqrstuvwxyz");
    c = d;

    EXPECT_EQ(26u, c.size());
    EXPECT_EQ(40u, c.capacity());
    EXPECT_STREQ("abcdefghijklmnopqrstuvwxyz", c.c_str());
    EXPECT_NE(c.c_str(), d.c_str()); // deep copy
    EXPECT_NE(&c, &d);
}

TEST(StringAssignmentTest, AssignFromDifferentCapacity) {
    stdx::string a("hello");
    stdx::string<50> b("abcdefghijklmnopqrstuvwxyz");
    a = b;

    EXPECT_EQ(26u, a.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES * 2, a.capacity());
    EXPECT_STREQ("abcdefghijklmnopqrstuvwxyz", a.c_str());
    EXPECT_NE(a.c_str(), b.c_str()); // deep copy
    EXPECT_NE(static_cast<void*>(&a), static_cast<void*>(&b));
}

TEST(StringAssignmentTest, AssignFromCString) {
    stdx::string s("hello");
    const char* cstr = "world";
    s = cstr;

    EXPECT_EQ(5u, s.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, s.capacity());
    EXPECT_STREQ("world", s.c_str());
    EXPECT_NE(s.c_str(), cstr); // deep copy
}

TEST(StringAssignmentTest, AssignFromEmptyCString) {
    stdx::string s("hello");
    s = "";

    EXPECT_EQ(0u, s.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, s.capacity());
    EXPECT_STREQ("", s.c_str());
}

TEST(StringAssignmentTest, AssignSameCStringContent) {
    stdx::string s("hello");
    const char* before = s.c_str();
    s = before;

    EXPECT_EQ(5u, s.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, s.capacity());
    EXPECT_STREQ("hello", s.c_str());
    EXPECT_EQ(s.c_str(), before); // buffer should be reused
}

TEST(StringAssignmentTest, AssignSameCStringOverlappingMemory) {
    stdx::string s("hello");
    const char* before = s.c_str();
    s = before + 1;

    EXPECT_EQ(4u, s.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, s.capacity());
    EXPECT_STREQ("ello", s.c_str());
    EXPECT_EQ(s.c_str(), before); // buffer should be reused
}

TEST(StringAssignmentTest, AssignFromCStringGrowsCapacity) {
    stdx::string a("hello");
    const char* b = "abcdefghijklmnopqrstuvwxyz";
    a = b;

    EXPECT_EQ(26u, a.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES * 2, a.capacity());
    EXPECT_STREQ("abcdefghijklmnopqrstuvwxyz", a.c_str());
    EXPECT_NE(a.c_str(), b); // deep copy

    stdx::string<10> c("goodbye");
    const char* d = "abcdefghijklmnopqrstuvwxyz";
    c = d;

    EXPECT_EQ(26u, c.size());
    EXPECT_EQ(40u, c.capacity());
    EXPECT_STREQ("abcdefghijklmnopqrstuvwxyz", c.c_str());
    EXPECT_NE(c.c_str(), d); // deep copy
}

TEST(StringAssignmentTest, AssignFromCStringDoesNotShrinksCapacity) {
    stdx::string a("abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES * 2, a.capacity());

    const char* b = "hello";
    a = b;

    EXPECT_EQ(5u, a.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES * 2, a.capacity());
    EXPECT_STREQ(a.c_str(), b);
    EXPECT_NE(a.c_str(), b); // deep copy
}

TEST(StringAssignmentTest, AssignFromChar) {
    stdx::string s("hello");
    s = '!';

    EXPECT_EQ(1u, s.size());
    EXPECT_EQ(stdx::DEFAULT_STRING_CAPACITY_BYTES, s.capacity());
    EXPECT_STREQ("!", s.c_str());
}

TEST(StringAssignmentTest, AssignFromCharGrowsCapacity) {
    const char c = '!';
    stdx::string<0> s("");
    s = c;
    EXPECT_EQ(1u, s.size());
    EXPECT_EQ(1u, s.capacity()); // 0 capacity edge case. Should grow to 1.
    EXPECT_STREQ("!", s.c_str());
    EXPECT_NE(&c, s.c_str());
}

TEST(StringAssignmentTest, AssignFromCharDoesNotShrinksCapacity) {
    const char c = '!';
    stdx::string<1> s("hello");
    EXPECT_EQ(8u, s.capacity());

    s = c;

    EXPECT_EQ(1u, s.size());
    EXPECT_EQ(8u, s.capacity());
    EXPECT_STREQ("!", s.c_str());
    EXPECT_NE(&c, s.c_str());
}

// String class equality operators
TEST(StringEqualityTest, EqualStringsSameContent) {
    stdx::string a("hello");
    stdx::string b("hello");
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(StringEqualityTest, NotEqualDifferentContent) {
    stdx::string a("hello");
    stdx::string b("world");
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(StringEqualityTest, NotEqualDifferentLength) {
    stdx::string a("hello");
    stdx::string b("hello!");
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(StringEqualityTest, EqualEmptyStrings) {
    stdx::string a;
    stdx::string b;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(StringEqualityTest, EmptyAndNonEmptyNotEqual) {
    stdx::string a;
    stdx::string b("hello");
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(StringEqualityTest, SelfEquality) {
    stdx::string a("hello");
    EXPECT_TRUE(a == a);
    EXPECT_FALSE(a != a);
}

TEST(StringEqualityTest, CaseSensitive) {
    stdx::string a("hello");
    stdx::string b("Hello");
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(StringEqualityTest, DifferentCapacities) {
    stdx::string a("hello");
    stdx::string<5> b("hello");
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(b == a);
    EXPECT_FALSE(b != a);

    // grow b
    b = "helloworld";
    EXPECT_EQ(10u, b.capacity());
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(b == a);
    EXPECT_TRUE(b != a);
}

TEST(StringEqualityTest, EqualCStringsSameContent) {
    stdx::string a("hello");
    const char* b = "hello";
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(b == a);
    EXPECT_FALSE(b != a);

    EXPECT_TRUE("hello" == a);
    EXPECT_FALSE("hello" != a);
}

TEST(StringEqualityTest, CStringNotEqualDifferentContent) {
    stdx::string a("hello");
    const char* b = "world";
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(b == a);
    EXPECT_TRUE(b != a);
}

TEST(StringEqualityTest, CStringNotEqualDifferentLength) {
    stdx::string a("hello");
    const char* b = "hello!";
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(b == a);
    EXPECT_TRUE(b != a);
}

TEST(StringEqualityTest, EqualEmptyCStrings) {
    stdx::string a;
    const char* b = "";
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(b == a);
    EXPECT_FALSE(b != a);
}

TEST(StringEqualityTest, EmptyAndNonEmptyCStringNotEqual) {
    stdx::string a;
    const char* b = "hello";
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(b == a);
    EXPECT_TRUE(b != a);
}

TEST(StringEqualityTest, EqualCharSameContent) {
    stdx::string a('!');
    char b = '!';
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(b == a);
    EXPECT_FALSE(b != a);

    EXPECT_TRUE('!' == a);
    EXPECT_FALSE('!' != a);
}

TEST(StringEqualityTest, CharNotEqualDifferentContent) {
    stdx::string a('a');
    char b = 'b';
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(b == a);
    EXPECT_TRUE(b != a);
}

TEST(StringEqualityTest, EmptyStringAndCharNotEqual) {
    stdx::string a;
    char b = 'b';
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(b == a);
    EXPECT_TRUE(b != a);
}

// String class isEmpty
TEST(StringEmptyTest, IsEmpty) {
    stdx::string emptyStr;
    EXPECT_TRUE(emptyStr.empty());

    stdx::string nonEmptyStr("Not Empty");
    EXPECT_FALSE(nonEmptyStr.empty());

    stdx::string<100> largeEmptyStr;
    EXPECT_TRUE(largeEmptyStr.empty());

    stdx::string<100> largeNonEmptyStr("Not Empty");
    EXPECT_FALSE(largeNonEmptyStr.empty());
}

// String class addition assignment operators
TEST(StringAppendAssignTest, AppendAnotherString) {
    stdx::string a("hello");
    stdx::string b(" world");
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>& result = (a += b);

    EXPECT_EQ(&a, &result);
    EXPECT_EQ(11u, a.size());
    EXPECT_STREQ("hello world", a.c_str());
}

TEST(StringAppendAssignTest, AppendEmptyString) {
    stdx::string a("hello");
    stdx::string empty;
    a += empty;

    EXPECT_EQ(5u, a.size());
    EXPECT_STREQ("hello", a.c_str());
}

TEST(StringAppendAssignTest, AppendToEmptyString) {
    stdx::string a;
    stdx::string b("hello");
    a += b;

    EXPECT_EQ(5u, a.size());
    EXPECT_STREQ("hello", a.c_str());
}

TEST(StringAppendAssignTest, AppendSelf) {
    stdx::string a("abc");
    a += a;

    EXPECT_EQ(6u, a.size());
    EXPECT_STREQ("abcabc", a.c_str());
}

TEST(StringAppendAssignTest, AppendDifferentCapacityString) {
    stdx::string a;
    stdx::string<10> b("hello");
    a += b;

    EXPECT_EQ(5u, a.size());
    EXPECT_STREQ("hello", a.c_str());
}

TEST(StringAppendAssignTest, AppendStringGrowsCapacity) {
    stdx::string<0> a;
    stdx::string<10> b("hello");
    a += b;

    EXPECT_EQ(5u, a.size());
    EXPECT_EQ(8u, a.capacity()); // Should grow to 8.
    EXPECT_STREQ("hello", a.c_str());
}

TEST(StringAppendAssignTest, AppendCString) {
    stdx::string s("hello");
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>& result = (s += " world");

    EXPECT_EQ(&s, &result);
    EXPECT_EQ(11u, s.size());
    EXPECT_STREQ("hello world", s.c_str());
}

TEST(StringAppendAssignTest, AppendEmptyCString) {
    stdx::string s("hello");
    s += "";
    EXPECT_EQ(5u, s.size());
    EXPECT_STREQ("hello", s.c_str());

    stdx::string<0> s2;
    s2 += "";
    EXPECT_EQ(0u, s2.size());
    EXPECT_EQ(0u, s2.capacity());
    EXPECT_STREQ("", s2.c_str());
}

TEST(StringAppendAssignTest, AppendCStringToEmptyString) {
    stdx::string s;
    s += "hello";

    EXPECT_EQ(5u, s.size());
    EXPECT_STREQ("hello", s.c_str());
}

TEST(StringAppendAssignTest, AppendCStringGrowsCapacity) {
    stdx::string<0> a;
    const char* b = "hello";
    a += b;

    EXPECT_EQ(5u, a.size());
    EXPECT_EQ(8u, a.capacity()); // Should grow to 8.
    EXPECT_STREQ("hello", a.c_str());
}

// String class clear
TEST(StringClearTest, ClearNonEmptyString) {
    stdx::string s("hello");
    s.clear();

    EXPECT_EQ(0u, s.size());
    EXPECT_STREQ("", s.c_str());
}

TEST(StringClearTest, ClearEmptyString) {
    stdx::string s;
    s.clear();

    EXPECT_EQ(0u, s.size());
    EXPECT_STREQ("", s.c_str());
}

TEST(StringClearTest, ClearThenAppendString) {
    stdx::string s("hello");

    s.clear();
    s += "world";

    EXPECT_EQ(5u, s.size());
    EXPECT_STREQ("world", s.c_str());
}

TEST(StringClearTest, ClearThenAppendChar) {
    stdx::string s("hello");
    s.clear();
    s += 'a';

    EXPECT_EQ(1u, s.size());
    EXPECT_STREQ("a", s.c_str());
}

TEST(StringClearTest, ClearThenAssignCString) {
    stdx::string s("hello");
    s.clear();
    s = "test";

    EXPECT_EQ(4u, s.size());
    EXPECT_STREQ("test", s.c_str());
}

// String class operator[]
TEST(StringIndexTests, AccessElements) {
    stdx::string s("hello");
    EXPECT_EQ('h', s[0]);
    EXPECT_EQ('e', s[1]);
    EXPECT_EQ('o', s[4]);
    EXPECT_EQ('o', s[s.size() - 1]);
}

TEST(StringIndexTests, AccessNullTerminator) {
    stdx::string s("hello");
    EXPECT_EQ('\0', s[5]);
    EXPECT_EQ('\0', s[s.size()]);
}

TEST(StringIndexTests, ModifyCharacter) {
    stdx::string s("hello");
    s[0] = 'H';
    EXPECT_EQ('H', s[0]);
    s[2] = 'X';
    EXPECT_EQ('X', s[2]);
    s[4] = 'Z';
    EXPECT_EQ('Z', s[4]);
}

TEST(StringIndexTests, ConstObjectAccess) {
    const stdx::string s("hello");
    EXPECT_EQ('e', s[1]);
    EXPECT_EQ('l', s[3]);
    EXPECT_EQ('o', s[4]);
    EXPECT_EQ('\0', s[5]);
}

// ===== begin() / end() =====

TEST(StringIteratorTest, BeginPointsToFirstElement) {
    stdx::string s = "hello";
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::iterator it = s.begin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_iterator cit = s.cbegin();
    EXPECT_EQ(*it, 'h');
    EXPECT_EQ(*cit, 'h');
}

TEST(StringIteratorTest, EndPointsPastLastElement) {
    stdx::string s = "hello";
    const stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::iterator begin = s.begin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::iterator end = s.end();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_iterator cend = s.cend();
    EXPECT_EQ(end - begin, 5);
    EXPECT_EQ(begin - end, -5);
    EXPECT_EQ(cend - begin, 5);
    EXPECT_EQ(begin - cend, -5);

    // end is not dereferenceable. Ensure decrementing gives us the last element
    EXPECT_EQ(*--end, 'o');
    EXPECT_EQ(*--cend, 'o');
}

TEST(StringIteratorTest, IterateForwardFromBeginToEnd) {
    stdx::string s = "hello";

    const char expected[] = {'h', 'e', 'l', 'l', 'o'};
    int i = 0;
    int j = 0;

    for (stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::iterator it = s.begin(); it != s.end(); ++it) {
        EXPECT_EQ(*it, expected[i++]);
    }
    for (stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_iterator cit = s.cbegin(); cit != s.cend(); ++cit) {
        EXPECT_EQ(*cit, expected[j++]);
    }

    EXPECT_EQ(5, i);
    EXPECT_EQ(5, j);
}

TEST(StringIteratorTest, IterateForwardRangeBasedForLoop) {
    stdx::string s = "hello";

    const char expected[] = {'h', 'e', 'l', 'l', 'o'};
    int i = 0;

    for (auto& x : s) {
        EXPECT_EQ(x, expected[i++]);
    }

    EXPECT_EQ(5, i);
}

TEST(StringIteratorTest, ConstIterationMatchesNonConst) {
    stdx::string s = "hello";

    int i = 0;
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::iterator it = s.begin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_iterator cit = s.cbegin();
    for (; it != s.end() && cit != s.cend(); ++it, ++cit) {
        EXPECT_EQ(*it, *cit);
        ++i;
    }
    EXPECT_EQ(5, i);
}

TEST(StringIteratorTest, BeginEqualsEndForEmptyString) {
    stdx::string s = "";
    EXPECT_EQ(s.begin(), s.end());
    EXPECT_EQ(s.cbegin(), s.cend());
}

TEST(StringIteratorTest, ConvertIteratorToConstIterator) {
    stdx::string s = "hello";

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::iterator it = s.begin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_iterator cit = it;
    EXPECT_EQ('h', *it);
    EXPECT_EQ('h', *cit);

    const stdx::string cs = "hello";
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_iterator cbegin = cs.begin(); // must be const iterator
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_iterator cend = cs.end();     // must be const iterator
    EXPECT_EQ('h', *cbegin);
    EXPECT_EQ('o', *(cend - 1));
}

TEST(StringIteratorTest, SingleElementForwardIteration) {
    stdx::string s = "h";

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::iterator it = s.begin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_iterator cit = s.cbegin();
    EXPECT_EQ(*it, 'h');
    EXPECT_EQ(*cit, 'h');

    ++it;
    ++cit;
    EXPECT_EQ(it, s.end());
    EXPECT_EQ(cit, s.cend());
}

TEST(StringIteratorTest, SizeZeroString) {
    stdx::string s = "";
    const stdx::string consts = s;

    EXPECT_EQ(s.begin(), s.end());
    EXPECT_EQ(consts.begin(), consts.end());
    EXPECT_EQ(s.cbegin(), s.cend());

    EXPECT_EQ(s.rbegin(), s.rend());
    EXPECT_EQ(consts.rbegin(), consts.rend());
    EXPECT_EQ(s.crbegin(), s.crend());
}

// ===== rbegin() / rend() =====

TEST(StringIteratorTest, RBeginPointsToLastElement) {
    stdx::string s = "hello";
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::reverse_iterator rit = s.rbegin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_reverse_iterator crit = s.crbegin();
    EXPECT_EQ(*rit, 'o');
    EXPECT_EQ(*crit, 'o');
}

TEST(StringIteratorTest, REndPointsBeforeFirstElement) {
    stdx::string s = "hello";

    const stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::reverse_iterator rbegin = s.rbegin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::reverse_iterator rend = s.rend();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_reverse_iterator crend = s.crend();
    EXPECT_EQ(rend - rbegin, 5);
    EXPECT_EQ(rbegin - rend, -5);
    EXPECT_EQ(crend - rbegin, 5);
    EXPECT_EQ(rbegin - crend, -5);

    // end is not dereferenceable. Ensure decrementing gives us the last element
    EXPECT_EQ(*--rend, 'h');
    EXPECT_EQ(*--crend, 'h');
}

TEST(StringIteratorTest, IterateReverseFromRBeginToREnd) {
    stdx::string s = "hello";

    const char expected[] = {'o', 'l', 'l', 'e', 'h'};
    int i = 0;
    int j = 0;

    for (stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::reverse_iterator rit = s.rbegin(); rit != s.rend(); ++rit) {
        EXPECT_EQ(*rit, expected[i++]);
    }
    for (stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_reverse_iterator crit = s.crbegin();
         crit != s.crend(); ++crit) {
        EXPECT_EQ(*crit, expected[j++]);
    }

    EXPECT_EQ(5, i);
    EXPECT_EQ(5, j);
}

TEST(StringIteratorTest, ConstReverseIterationMatchesNonConst) {
    stdx::string s = "hello";

    int i = 0;
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::reverse_iterator rit = s.rbegin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_reverse_iterator crit = s.crbegin();
    for (; rit != s.rend() && crit != s.crend(); ++rit, ++crit) {
        EXPECT_EQ(*rit, *crit);
        ++i;
    }
    EXPECT_EQ(5, i);
}

TEST(StringIteratorTest, RBeginEqualsREndForEmptyString) {
    stdx::string s = "";
    EXPECT_EQ(s.rbegin(), s.rend());
    EXPECT_EQ(s.crbegin(), s.crend());
}

TEST(StringIteratorTest, ConvertReverseIteratorToConstReverseIterator) {
    stdx::string s = "hello";

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::reverse_iterator rit = s.rbegin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_reverse_iterator crit = rit;
    EXPECT_EQ('o', *rit);
    EXPECT_EQ('o', *crit);

    const stdx::string cs = "hello";
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_reverse_iterator crbegin =
        cs.rbegin(); // must be const iterator
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_reverse_iterator crend =
        cs.rend(); // must be const iterator
    EXPECT_EQ('o', *crbegin);
    EXPECT_EQ('h', *(crend - 1));
}

TEST(StringIteratorTest, ReverseIteratorBaseRelationship) {
    stdx::string s = "hello";

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::reverse_iterator rit = s.rbegin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::iterator base = rit.base();

    // base() should point to end()
    EXPECT_EQ(base, s.end());
}

TEST(StringIteratorTest, SingleElementReverseIteration) {
    stdx::string s = "h";

    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::reverse_iterator rit = s.rbegin();
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES>::const_reverse_iterator crit = s.crbegin();
    EXPECT_EQ(*rit, 'h');
    EXPECT_EQ(*crit, 'h');

    ++rit;
    ++crit;
    EXPECT_EQ(rit, s.rend());
    EXPECT_EQ(crit, s.crend());
}

// Instantiation coverage: force calculate_new_capacity for string<N> sizes that have
// never had content assigned beyond their stack capacity in any other test.

TEST(StringGrowthTest, GrowsCapacity_Size14) {
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES - 10> s; // string<14>
    s = "aaaaaaaaaaaaaaa";                                    // 15 chars > 14
    EXPECT_EQ(15u, s.size());
}

TEST(StringGrowthTest, GrowsCapacity_Size15) {
    stdx::string<15> s;
    s = "aaaaaaaaaaaaaaaa"; // 16 chars > 15
    EXPECT_EQ(16u, s.size());
}

TEST(StringGrowthTest, GrowsCapacity_Size34) {
    stdx::string<stdx::DEFAULT_STRING_CAPACITY_BYTES + 10> s; // string<34>
    s = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";                // 35 chars > 34
    EXPECT_EQ(35u, s.size());
}

TEST(StringGrowthTest, GrowsCapacity_Size50) {
    stdx::string<50> s;
    s = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"; // 51 chars > 50
    EXPECT_EQ(51u, s.size());
}

TEST(StringGrowthTest, GrowsCapacity_Size100) {
    stdx::string<100> s;
    s = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "a"; // 101 chars > 100
    EXPECT_EQ(101u, s.size());
}
