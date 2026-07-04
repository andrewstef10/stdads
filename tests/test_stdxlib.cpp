#include <gtest/gtest.h>
#include <stdx/stdxlib.h>

TEST(StrtolTest, ParsesSimpleDecimal) { EXPECT_EQ(123, stdx::strtol("123")); }

TEST(StrtolTest, ParsesNegativeDecimal) { EXPECT_EQ(-456, stdx::strtol("-456")); }

TEST(StrtolTest, ParsesPositiveSign) { EXPECT_EQ(789, stdx::strtol("+789")); }

TEST(StrtolTest, ParsesDecimalWithLeadingZeros) { EXPECT_EQ(123, stdx::strtol("00123")); }

//
// WHITESPACE
//

TEST(StrtolTest, SkipsLeadingWhitespace) { EXPECT_EQ(42, stdx::strtol("    42")); }

TEST(StrtolTest, SkipsWhitespaceWithSign) { EXPECT_EQ(-99, stdx::strtol("   -99")); }

//
// BASE SPECIFIED
//

TEST(StrtolTest, ParsesBinaryBase2) {
    EXPECT_EQ(10, stdx::strtol("1010", 2));
    EXPECT_EQ(10, stdx::strtol("0001010", 2));
}

TEST(StrtolTest, ParsesHexBase16) { EXPECT_EQ(255, stdx::strtol("FF", 16)); }

TEST(StrtolTest, ParsesLowercaseHex) { EXPECT_EQ(255, stdx::strtol("ff", 16)); }

TEST(StrtolTest, ParsesBase36) {
    EXPECT_EQ(35, stdx::strtol("z", 36));
    EXPECT_EQ(35, stdx::strtol("Z", 36));
    EXPECT_EQ(1223, stdx::strtol("xZ", 36));
    EXPECT_EQ(1223, stdx::strtol("0xZ", 36));
}

//
// BASE ZERO AUTO DETECTION
//

TEST(StrtolTest, BaseZeroDecimal) { EXPECT_EQ(123, stdx::strtol("123", 0)); }

TEST(StrtolTest, BaseZeroOctal) {
    EXPECT_EQ(8, stdx::strtol("010", 0)); // octal
}

TEST(StrtolTest, BaseZeroHex) {
    EXPECT_EQ(26, stdx::strtol("0x1A", 0));
    EXPECT_EQ(26, stdx::strtol("0X1A", 0));
}

//
// HEX PREFIX WHEN BASE 16
//

TEST(StrtolTest, HexPrefixWithBase16) { EXPECT_EQ(26, stdx::strtol("0x1A", 16)); }

TEST(StrtolTest, HexPrefixUppercase) { EXPECT_EQ(26, stdx::strtol("0X1A", 16)); }

TEST(StrtolTest, HexPrefixInvalidForCoverage) {
    EXPECT_EQ(10, stdx::strtol("0a", 16));
    EXPECT_EQ(0, stdx::strtol("X1A", 16));
    EXPECT_EQ(0, stdx::strtol("x1A", 16));
}

//
// PARTIAL PARSING
//

TEST(StrtolTest, StopsAtInvalidCharacter) {
    char* end = nullptr;
    long result = stdx::strtol("123abc", 10, &end);

    EXPECT_EQ(123, result);
    EXPECT_STREQ("abc", end);
}

TEST(StrtolTest, StopsImmediatelyIfNoDigits) {
    char* end = nullptr;
    const char* str = "abc";
    long result = stdx::strtol(str, 10, &end);

    EXPECT_EQ(0, result);
    EXPECT_EQ(end, str);
}

TEST(StrtolTest, ParsesThenStopsOnSpace) {
    char* end = nullptr;
    long result = stdx::strtol("42 99", 10, &end);

    EXPECT_EQ(42, result);
    EXPECT_STREQ(" 99", end);
}

//
// INVALID BASE
//

TEST(StrtolTest, InvalidBaseReturnsZero) {
    EXPECT_EQ(0, stdx::strtol("123", -1));
    EXPECT_EQ(0, stdx::strtol("123", 37));
}

//
// NULL ENDPTR
//

TEST(StrtolTest, WorksWithNullEndPtr) { EXPECT_EQ(123, stdx::strtol("123", 10, nullptr)); }

//
// SIGN WITH BASE 2
//

TEST(StrtolTest, NegativeBinary) { EXPECT_EQ(-5, stdx::strtol("-101", 2)); }

//
// LEADING ZERO WITHOUT BASE ZERO
//

TEST(StrtolTest, LeadingZeroBase10) {
    EXPECT_EQ(10, stdx::strtol("010", 10)); // decimal, not octal
}

//
// EMPTY STRING
//

TEST(StrtolTest, EmptyString) {
    char* end = nullptr;
    const char* emptyStr = "";
    EXPECT_EQ(0, stdx::strtol(emptyStr, 10, &end));
    EXPECT_EQ(end, emptyStr);
}

//
// ONLY SIGN
//

TEST(StrtolTest, OnlySign) {
    char* end = nullptr;
    const char* onlySign = "-";
    EXPECT_EQ(0, stdx::strtol(onlySign, 10, &end));
    EXPECT_EQ(end, onlySign);

    onlySign = "+";
    EXPECT_EQ(0, stdx::strtol(onlySign, 10, &end));
    EXPECT_EQ(end, onlySign);
}
