#include <gtest/gtest.h>
#include <stdx/num_utils.h>

// ---------- WITHIN (Inclusive) ----------

TEST(WithinTests, ValueInsideRange) {
    EXPECT_TRUE(stdx::within(5, 1, 10));
    EXPECT_TRUE(stdx::within(-5, -10, -1));
}

TEST(WithinTests, ValueOnLowerBound) {
    EXPECT_TRUE(stdx::within(1, 1, 10));
    EXPECT_TRUE(stdx::within(-10, -10, -1));
}

TEST(WithinTests, ValueOnUpperBound) {
    EXPECT_TRUE(stdx::within(10, 1, 10));
    EXPECT_TRUE(stdx::within(-1, -10, -1));
}

TEST(WithinTests, ValueBelowRange) {
    EXPECT_FALSE(stdx::within(0, 1, 10));
    EXPECT_FALSE(stdx::within(-11, -10, -1));
}

TEST(WithinTests, ValueAboveRange) {
    EXPECT_FALSE(stdx::within(11, 1, 10));
    EXPECT_FALSE(stdx::within(0, -10, -1));
}

// ---------- WITHIN EXCLUSIVE ----------

TEST(WithinExclusiveTests, ValueInsideRange) {
    EXPECT_TRUE(stdx::within_exclusive(5, 1, 10));
    EXPECT_TRUE(stdx::within_exclusive(-5, -10, -1));
}

TEST(WithinExclusiveTests, ValueOnLowerBound) {
    EXPECT_FALSE(stdx::within_exclusive(1, 1, 10));
    EXPECT_FALSE(stdx::within_exclusive(-10, -10, -1));
}

TEST(WithinExclusiveTests, ValueOnUpperBound) {
    EXPECT_FALSE(stdx::within_exclusive(10, 1, 10));
    EXPECT_FALSE(stdx::within_exclusive(-1, -10, -1));
}

TEST(WithinExclusiveTests, ValueBelowRange) {
    EXPECT_FALSE(stdx::within_exclusive(0, 1, 10));
    EXPECT_FALSE(stdx::within_exclusive(-11, -10, -1));
}

TEST(WithinExclusiveTests, ValueAboveRange) {
    EXPECT_FALSE(stdx::within_exclusive(11, 1, 10));
    EXPECT_FALSE(stdx::within_exclusive(0, -10, -1));
}

// ---------- OUTSIDE (Exclusive) ----------

TEST(OutsideTests, ValueInsideRange) {
    EXPECT_FALSE(stdx::outside(5, 1, 10));
    EXPECT_FALSE(stdx::outside(-5, -10, -1));
}

TEST(OutsideTests, ValueOnLowerBound) {
    EXPECT_FALSE(stdx::outside(1, 1, 10));
    EXPECT_FALSE(stdx::outside(-10, -10, -1));
}

TEST(OutsideTests, ValueOnUpperBound) {
    EXPECT_FALSE(stdx::outside(10, 1, 10));
    EXPECT_FALSE(stdx::outside(-1, -10, -1));
}

TEST(OutsideTests, ValueBelowRange) {
    EXPECT_TRUE(stdx::outside(0, 1, 10));
    EXPECT_TRUE(stdx::outside(-11, -10, -1));
}

TEST(OutsideTests, ValueAboveRange) {
    EXPECT_TRUE(stdx::outside(11, 1, 10));
    EXPECT_TRUE(stdx::outside(0, -10, -1));
}

// ---------- OUTSIDE INCLUSIVE ----------

TEST(OutsideExclusiveTests, ValueInsideRange) {
    EXPECT_FALSE(stdx::outside_inclusive(5, 1, 10));
    EXPECT_FALSE(stdx::outside_inclusive(-5, -10, -1));
}

TEST(OutsideExclusiveTests, ValueOnLowerBound) {
    EXPECT_TRUE(stdx::outside_inclusive(1, 1, 10));
    EXPECT_TRUE(stdx::outside_inclusive(-11, -10, -1));
}

TEST(OutsideExclusiveTests, ValueOnUpperBound) {
    EXPECT_TRUE(stdx::outside_inclusive(10, 1, 10));
    EXPECT_TRUE(stdx::outside_inclusive(-1, -10, -1));
}

TEST(OutsideExclusiveTests, ValueBelowRange) {
    EXPECT_TRUE(stdx::outside_inclusive(0, 1, 10));
    EXPECT_TRUE(stdx::outside_inclusive(-11, -10, -1));
}

TEST(OutsideExclusiveTests, ValueAboveRange) {
    EXPECT_TRUE(stdx::outside_inclusive(11, 1, 10));
    EXPECT_TRUE(stdx::outside_inclusive(0, -10, -1));
}

// ---------- CLAMP ----------

TEST(ClampTests, ValueInsideRange) {
    int val = 5;
    int negVal = -5;
    EXPECT_FALSE(stdx::clamp(val, 1, 10));
    EXPECT_FALSE(stdx::clamp(negVal, -10, -1));
    EXPECT_EQ(5, val);
    EXPECT_EQ(-5, negVal);
}

TEST(ClampTests, ValueOnLowerBound) {
    int val = 1;
    int negVal = -10;
    EXPECT_FALSE(stdx::clamp(val, 1, 10));
    EXPECT_FALSE(stdx::clamp(negVal, -10, -1));
    EXPECT_EQ(1, val);
    EXPECT_EQ(-10, negVal);
}

TEST(ClampTests, ValueOnUpperBound) {
    int val = 10;
    int negVal = -1;
    EXPECT_FALSE(stdx::clamp(val, 1, 10));
    EXPECT_FALSE(stdx::clamp(negVal, -10, -1));
    EXPECT_EQ(10, val);
    EXPECT_EQ(-1, negVal);
}

TEST(ClampTests, ValueBelowRange) {
    int val = 0;
    int negVal = -11;
    EXPECT_TRUE(stdx::clamp(val, 1, 10));
    EXPECT_TRUE(stdx::clamp(negVal, -10, -1));
    EXPECT_EQ(1, val);
    EXPECT_EQ(-10, negVal);
}

TEST(ClampTests, ValueAboveRange) {
    int val = 11;
    int negVal = 0;
    EXPECT_TRUE(stdx::clamp(val, 1, 10));
    EXPECT_TRUE(stdx::clamp(negVal, -10, -1));
    EXPECT_EQ(10, val);
    EXPECT_EQ(-1, negVal);
}
