#include <gtest/gtest.h>
#include <stdx/ctype.h>

// ---------------- isAlpha ----------------

TEST(IsAlphaTests, LowercaseLetters) {
    EXPECT_TRUE(stdx::is_alpha('a'));
    EXPECT_TRUE(stdx::is_alpha('z'));
    EXPECT_TRUE(stdx::is_alpha('m'));
}

TEST(IsAlphaTests, UppercaseLetters) {
    EXPECT_TRUE(stdx::is_alpha('A'));
    EXPECT_TRUE(stdx::is_alpha('Z'));
    EXPECT_TRUE(stdx::is_alpha('M'));
}

TEST(IsAlphaTests, DigitsReturnFalse) {
    EXPECT_FALSE(stdx::is_alpha('0'));
    EXPECT_FALSE(stdx::is_alpha('9'));
}

TEST(IsAlphaTests, SymbolsReturnFalse) {
    EXPECT_FALSE(stdx::is_alpha('!'));
    EXPECT_FALSE(stdx::is_alpha('@'));
    EXPECT_FALSE(stdx::is_alpha(' '));
}

TEST(IsAlphaTests, BoundaryCharacters) {
    EXPECT_FALSE(stdx::is_alpha('`')); // before 'a'
    EXPECT_FALSE(stdx::is_alpha('{')); // after 'z'
    EXPECT_FALSE(stdx::is_alpha('@')); // before 'A'
    EXPECT_FALSE(stdx::is_alpha('[')); // after 'Z'
}

// ---------------- IsDigit ----------------

TEST(IsDigitTests, ValidDigits) {
    EXPECT_TRUE(stdx::is_digit('0'));
    EXPECT_TRUE(stdx::is_digit('5'));
    EXPECT_TRUE(stdx::is_digit('9'));
}

TEST(IsDigitTests, LettersReturnFalse) {
    EXPECT_FALSE(stdx::is_digit('a'));
    EXPECT_FALSE(stdx::is_digit('Z'));
}

TEST(IsDigitTests, SymbolsReturnFalse) {
    EXPECT_FALSE(stdx::is_digit('-'));
    EXPECT_FALSE(stdx::is_digit(' '));
}

// ---------------- IsDigitOfBase ----------------

TEST(IsDigitOfBaseTests, Base10Valid) {
    for (char c = '0'; c <= '9'; ++c) {
        EXPECT_TRUE(stdx::is_digit_of_base(c, 10));
    }
}

TEST(IsDigitOfBaseTests, Base10Invalid) {
    EXPECT_FALSE(stdx::is_digit_of_base('0' - 1, 10));
    EXPECT_FALSE(stdx::is_digit_of_base('9' + 1, 10));
    EXPECT_FALSE(stdx::is_digit_of_base('a', 10));
    EXPECT_FALSE(stdx::is_digit_of_base('A', 10));
    EXPECT_FALSE(stdx::is_digit_of_base('-', 10));
    EXPECT_FALSE(stdx::is_digit_of_base(' ', 10));
    EXPECT_FALSE(stdx::is_digit_of_base('\0', 10));
}

TEST(IsDigitOfBaseTests, Base2Valid) {
    EXPECT_TRUE(stdx::is_digit_of_base('0', 2));
    EXPECT_TRUE(stdx::is_digit_of_base('1', 2));
}

TEST(IsDigitOfBaseTests, Base2Invalid) {
    EXPECT_FALSE(stdx::is_digit_of_base('0' - 1, 2));
    EXPECT_FALSE(stdx::is_digit_of_base('2', 2));
    EXPECT_FALSE(stdx::is_digit_of_base('a', 2));
    EXPECT_FALSE(stdx::is_digit_of_base('A', 2));
    EXPECT_FALSE(stdx::is_digit_of_base('-', 2));
    EXPECT_FALSE(stdx::is_digit_of_base(' ', 2));
    EXPECT_FALSE(stdx::is_digit_of_base('\0', 2));
}

TEST(IsDigitOfBaseTests, Base8Valid) {
    for (char c = '0'; c <= '7'; ++c) {
        EXPECT_TRUE(stdx::is_digit_of_base(c, 8));
    }
}

TEST(IsDigitOfBaseTests, Base8Invalid) {
    EXPECT_FALSE(stdx::is_digit_of_base('0' - 1, 8));
    EXPECT_FALSE(stdx::is_digit_of_base('8', 8));
    EXPECT_FALSE(stdx::is_digit_of_base('a', 8));
    EXPECT_FALSE(stdx::is_digit_of_base('A', 8));
    EXPECT_FALSE(stdx::is_digit_of_base('-', 8));
    EXPECT_FALSE(stdx::is_digit_of_base(' ', 8));
    EXPECT_FALSE(stdx::is_digit_of_base('\0', 8));
}

TEST(IsDigitOfBaseTests, Base1Valid) { EXPECT_TRUE(stdx::is_digit_of_base('1', 1)); }

TEST(IsDigitOfBaseTests, Base1Invalid) {
    EXPECT_FALSE(stdx::is_digit_of_base('0', 1));
    EXPECT_FALSE(stdx::is_digit_of_base('2', 1));
    EXPECT_FALSE(stdx::is_digit_of_base('a', 1));
    EXPECT_FALSE(stdx::is_digit_of_base('A', 1));
    EXPECT_FALSE(stdx::is_digit_of_base('-', 1));
    EXPECT_FALSE(stdx::is_digit_of_base(' ', 1));
    EXPECT_FALSE(stdx::is_digit_of_base('\0', 1));
}

TEST(IsDigitOfBaseTests, Base16Number) {
    for (char c = '0'; c <= '9'; ++c) {
        EXPECT_TRUE(stdx::is_digit_of_base(c, 16));
    }
}

TEST(IsDigitOfBaseTests, Base16Lowercase) {
    for (char c = 'a'; c <= 'f'; ++c) {
        EXPECT_TRUE(stdx::is_digit_of_base(c, 16));
    }
}

TEST(IsDigitOfBaseTests, Base16Uppercase) {
    for (char c = 'A'; c <= 'F'; ++c) {
        EXPECT_TRUE(stdx::is_digit_of_base(c, 16));
    }
}

TEST(IsDigitOfBaseTests, Base16Invalid) {
    EXPECT_FALSE(stdx::is_digit_of_base('0' - 1, 16));
    EXPECT_FALSE(stdx::is_digit_of_base('9' + 1, 16));
    EXPECT_FALSE(stdx::is_digit_of_base('a' - 1, 16));
    EXPECT_FALSE(stdx::is_digit_of_base('A' - 1, 16));
    EXPECT_FALSE(stdx::is_digit_of_base('g', 16));
    EXPECT_FALSE(stdx::is_digit_of_base('G', 16));
    EXPECT_FALSE(stdx::is_digit_of_base('-', 16));
    EXPECT_FALSE(stdx::is_digit_of_base(' ', 16));
    EXPECT_FALSE(stdx::is_digit_of_base('\0', 16));
}

TEST(IsDigitOfBaseTests, Base36Number) {
    for (char c = '0'; c <= '9'; ++c) {
        EXPECT_TRUE(stdx::is_digit_of_base(c, 36));
    }
}

TEST(IsDigitOfBaseTests, Base36Lowercase) {
    for (char c = 'a'; c <= 'z'; ++c) {
        EXPECT_TRUE(stdx::is_digit_of_base(c, 36));
    }
}

TEST(IsDigitOfBaseTests, Base36Uppercase) {
    for (char c = 'A'; c <= 'Z'; ++c) {
        EXPECT_TRUE(stdx::is_digit_of_base(c, 36));
    }
}

TEST(IsDigitOfBaseTests, Base36Invalid) {
    EXPECT_FALSE(stdx::is_digit_of_base('0' - 1, 36));
    EXPECT_FALSE(stdx::is_digit_of_base('9' + 1, 36));
    EXPECT_FALSE(stdx::is_digit_of_base('a' - 1, 36));
    EXPECT_FALSE(stdx::is_digit_of_base('A' - 1, 36));
    EXPECT_FALSE(stdx::is_digit_of_base('z' + 1, 36));
    EXPECT_FALSE(stdx::is_digit_of_base('Z' + 1, 36));
    EXPECT_FALSE(stdx::is_digit_of_base('-', 36));
    EXPECT_FALSE(stdx::is_digit_of_base(' ', 36));
    EXPECT_FALSE(stdx::is_digit_of_base('\0', 36));
}

TEST(IsDigitOfBaseTests, Base35CharOutOfRange) { EXPECT_FALSE(stdx::is_digit_of_base('z', 35)); }

TEST(IsDigitOfBaseTests, InvalidBaseLow) {
    EXPECT_FALSE(stdx::is_digit_of_base('1', 0));
    EXPECT_FALSE(stdx::is_digit_of_base('1', -1));
}

TEST(IsDigitOfBaseTests, InvalidBaseHigh) {
    EXPECT_FALSE(stdx::is_digit_of_base('1', 37));
    EXPECT_FALSE(stdx::is_digit_of_base('z', 100));
}

// ---------------- CharToDigit ----------------

TEST(CharToDigitTests, Base10Valid) {
    int i = 0;
    for (char c = '0'; c <= '9'; ++c) {
        EXPECT_EQ(i, stdx::char_to_digit(c, 10));
        ++i;
    }
}

TEST(CharToDigitTests, Base10Invalid) {
    EXPECT_EQ(-1, stdx::char_to_digit('0' - 1, 10));
    EXPECT_EQ(-1, stdx::char_to_digit('9' + 1, 10));
    EXPECT_EQ(-1, stdx::char_to_digit('a', 10));
    EXPECT_EQ(-1, stdx::char_to_digit('A', 10));
    EXPECT_EQ(-1, stdx::char_to_digit('-', 10));
    EXPECT_EQ(-1, stdx::char_to_digit(' ', 10));
    EXPECT_EQ(-1, stdx::char_to_digit('\0', 10));
}

TEST(CharToDigitTests, Base2Valid) {
    EXPECT_EQ(0, stdx::char_to_digit('0', 2));
    EXPECT_EQ(1, stdx::char_to_digit('1', 2));
}

TEST(CharToDigitTests, Base2Invalid) {
    EXPECT_EQ(-1, stdx::char_to_digit('0' - 1, 2));
    EXPECT_EQ(-1, stdx::char_to_digit('2', 2));
    EXPECT_EQ(-1, stdx::char_to_digit('a', 2));
    EXPECT_EQ(-1, stdx::char_to_digit('A', 2));
    EXPECT_EQ(-1, stdx::char_to_digit('-', 2));
    EXPECT_EQ(-1, stdx::char_to_digit(' ', 2));
    EXPECT_EQ(-1, stdx::char_to_digit('\0', 2));
}

TEST(CharToDigitTests, Base8Valid) {
    int i = 0;
    for (char c = '0'; c <= '7'; ++c) {
        EXPECT_EQ(i, stdx::char_to_digit(c, 8));
        ++i;
    }
}

TEST(CharToDigitTests, Base8Invalid) {
    EXPECT_EQ(-1, stdx::char_to_digit('0' - 1, 8));
    EXPECT_EQ(-1, stdx::char_to_digit('8', 8));
    EXPECT_EQ(-1, stdx::char_to_digit('a', 8));
    EXPECT_EQ(-1, stdx::char_to_digit('A', 8));
    EXPECT_EQ(-1, stdx::char_to_digit('-', 8));
    EXPECT_EQ(-1, stdx::char_to_digit(' ', 8));
    EXPECT_EQ(-1, stdx::char_to_digit('\0', 8));
}

TEST(CharToDigitTests, Base1Valid) { EXPECT_EQ(1, stdx::char_to_digit('1', 1)); }

TEST(CharToDigitTests, Base1Invalid) {
    EXPECT_EQ(-1, stdx::char_to_digit('0', 1));
    EXPECT_EQ(-1, stdx::char_to_digit('2', 1));
    EXPECT_EQ(-1, stdx::char_to_digit('a', 1));
    EXPECT_EQ(-1, stdx::char_to_digit('A', 1));
    EXPECT_EQ(-1, stdx::char_to_digit('-', 1));
    EXPECT_EQ(-1, stdx::char_to_digit(' ', 1));
    EXPECT_EQ(-1, stdx::char_to_digit('\0', 1));
}

TEST(CharToDigitTests, Base16Number) {
    int i = 0;
    for (char c = '0'; c <= '9'; ++c) {
        EXPECT_EQ(i, stdx::char_to_digit(c, 16));
        ++i;
    }
}

TEST(CharToDigitTests, Base16Lowercase) {
    int i = 10;
    for (char c = 'a'; c <= 'f'; ++c) {
        EXPECT_EQ(i, stdx::char_to_digit(c, 16));
        ++i;
    }
}

TEST(CharToDigitTests, Base16Uppercase) {
    int i = 10;
    for (char c = 'A'; c <= 'F'; ++c) {
        EXPECT_EQ(i, stdx::char_to_digit(c, 16));
        ++i;
    }
}

TEST(CharToDigitTests, Base16Invalid) {
    EXPECT_EQ(-1, stdx::char_to_digit('0' - 1, 16));
    EXPECT_EQ(-1, stdx::char_to_digit('9' + 1, 16));
    EXPECT_EQ(-1, stdx::char_to_digit('a' - 1, 16));
    EXPECT_EQ(-1, stdx::char_to_digit('A' - 1, 16));
    EXPECT_EQ(-1, stdx::char_to_digit('g', 16));
    EXPECT_EQ(-1, stdx::char_to_digit('G', 16));
    EXPECT_EQ(-1, stdx::char_to_digit('-', 16));
    EXPECT_EQ(-1, stdx::char_to_digit(' ', 16));
    EXPECT_EQ(-1, stdx::char_to_digit('\0', 16));
}

TEST(CharToDigitTests, Base36Number) {
    int i = 0;
    for (char c = '0'; c <= '9'; ++c) {
        EXPECT_EQ(i, stdx::char_to_digit(c, 36));
        ++i;
    }
}

TEST(CharToDigitTests, Base36Lowercase) {
    int i = 10;
    for (char c = 'a'; c <= 'z'; ++c) {
        EXPECT_EQ(i, stdx::char_to_digit(c, 36));
        ++i;
    }
}

TEST(CharToDigitTests, Base36Uppercase) {
    int i = 10;
    for (char c = 'A'; c <= 'Z'; ++c) {
        EXPECT_EQ(i, stdx::char_to_digit(c, 36));
        ++i;
    }
}

TEST(CharToDigitTests, Base36Invalid) {
    EXPECT_EQ(-1, stdx::char_to_digit('0' - 1, 36));
    EXPECT_EQ(-1, stdx::char_to_digit('9' + 1, 36));
    EXPECT_EQ(-1, stdx::char_to_digit('a' - 1, 36));
    EXPECT_EQ(-1, stdx::char_to_digit('A' - 1, 36));
    EXPECT_EQ(-1, stdx::char_to_digit('z' + 1, 36));
    EXPECT_EQ(-1, stdx::char_to_digit('Z' + 1, 36));
    EXPECT_EQ(-1, stdx::char_to_digit('-', 36));
    EXPECT_EQ(-1, stdx::char_to_digit(' ', 36));
    EXPECT_EQ(-1, stdx::char_to_digit('\0', 36));
}

TEST(CharToDigitTests, Base35CharOutOfRange) { EXPECT_EQ(-1, stdx::char_to_digit('z', 35)); }

TEST(CharToDigitTests, InvalidBaseLow) {
    EXPECT_EQ(-1, stdx::char_to_digit('1', 0));
    EXPECT_EQ(-1, stdx::char_to_digit('1', -1));
}

TEST(CharToDigitTests, InvalidBaseHigh) {
    EXPECT_EQ(-1, stdx::char_to_digit('1', 37));
    EXPECT_EQ(-1, stdx::char_to_digit('z', 100));
}

// ---------------- ToLower ----------------

TEST(ToLowerTests, ConvertsUppercaseLetters) {
    EXPECT_EQ(stdx::to_lower('A'), 'a');
    EXPECT_EQ(stdx::to_lower('Z'), 'z');
    EXPECT_EQ(stdx::to_lower('M'), 'm');
}

TEST(ToLowerTests, BoundaryCharacters) {
    EXPECT_EQ(stdx::to_lower('A'), 'a');
    EXPECT_EQ(stdx::to_lower('Z'), 'z');
}

TEST(ToLowerTests, LowercaseUnchanged) {
    EXPECT_EQ(stdx::to_lower('a'), 'a');
    EXPECT_EQ(stdx::to_lower('z'), 'z');
    EXPECT_EQ(stdx::to_lower('m'), 'm');
}

TEST(ToLowerTests, DigitsUnchanged) {
    EXPECT_EQ(stdx::to_lower('0'), '0');
    EXPECT_EQ(stdx::to_lower('9'), '9');
}

TEST(ToLowerTests, SymbolsUnchanged) {
    EXPECT_EQ(stdx::to_lower('!'), '!');
    EXPECT_EQ(stdx::to_lower('@'), '@');
    EXPECT_EQ(stdx::to_lower('['), '[');
}

TEST(ToLowerTests, WhitespaceUnchanged) {
    EXPECT_EQ(stdx::to_lower(' '), ' ');
    EXPECT_EQ(stdx::to_lower('\n'), '\n');
    EXPECT_EQ(stdx::to_lower('\t'), '\t');
}

TEST(ToLowerTests, AdjacentAsciiCharacters) {
    EXPECT_EQ(stdx::to_lower('@'), '@'); // before 'A'
    EXPECT_EQ(stdx::to_lower('['), '['); // after 'Z'
}

// ---------------- ToUpper ----------------

TEST(ToUpperTests, ConvertsLowercaseLetters) {
    EXPECT_EQ(stdx::to_upper('a'), 'A');
    EXPECT_EQ(stdx::to_upper('z'), 'Z');
    EXPECT_EQ(stdx::to_upper('m'), 'M');
}

TEST(ToUpperTests, BoundaryCharacters) {
    EXPECT_EQ(stdx::to_upper('a'), 'A');
    EXPECT_EQ(stdx::to_upper('z'), 'Z');
}

TEST(ToUpperTests, UppercaseUnchanged) {
    EXPECT_EQ(stdx::to_upper('A'), 'A');
    EXPECT_EQ(stdx::to_upper('Z'), 'Z');
    EXPECT_EQ(stdx::to_upper('M'), 'M');
}

TEST(ToUpperTests, DigitsUnchanged) {
    EXPECT_EQ(stdx::to_upper('0'), '0');
    EXPECT_EQ(stdx::to_upper('9'), '9');
}

TEST(ToUpperTests, SymbolsUnchanged) {
    EXPECT_EQ(stdx::to_upper('!'), '!');
    EXPECT_EQ(stdx::to_upper('@'), '@');
    EXPECT_EQ(stdx::to_upper('{'), '{');
}

TEST(ToUpperTests, WhitespaceUnchanged) {
    EXPECT_EQ(stdx::to_upper(' '), ' ');
    EXPECT_EQ(stdx::to_upper('\n'), '\n');
    EXPECT_EQ(stdx::to_upper('\t'), '\t');
}

TEST(ToUpperTests, AdjacentAsciiCharacters) {
    EXPECT_EQ(stdx::to_upper('`'), '`'); // before 'a'
    EXPECT_EQ(stdx::to_upper('{'), '{'); // after 'z'
}
