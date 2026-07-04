#include <gtest/gtest.h>
#include <stdx/math.h>

TEST(AbsTest, Int) {
  EXPECT_EQ(10, stdx::abs(-10));
  EXPECT_EQ(10, stdx::abs(10));
  EXPECT_EQ(0, stdx::abs(0));
}

TEST(AbsTest, Long) {
  long neg = -123456L;
  long pos = 123456L;

  EXPECT_EQ(pos, stdx::abs(neg));
  EXPECT_EQ(pos, stdx::abs(pos));
  EXPECT_EQ(0L, stdx::abs(0L));
}

TEST(AbsTest, LongLong) {
  long long neg = -9876543210LL;
  long long pos = 9876543210LL;

  EXPECT_EQ(pos, stdx::abs(neg));
  EXPECT_EQ(pos, stdx::abs(pos));
  EXPECT_EQ(0LL, stdx::abs(0LL));
}

TEST(AbsTest, Float) {
  float neg = -3.5f;
  float pos = 3.5f;

  EXPECT_FLOAT_EQ(pos, stdx::abs(neg));
  EXPECT_FLOAT_EQ(pos, stdx::abs(pos));
  EXPECT_FLOAT_EQ(0.0f, stdx::abs(0.0f));
}

TEST(AbsTest, Double) {
  double neg = -3.1415926535;
  double pos = 3.1415926535;

  EXPECT_DOUBLE_EQ(pos, stdx::abs(neg));
  EXPECT_DOUBLE_EQ(pos, stdx::abs(pos));
  EXPECT_DOUBLE_EQ(0.0, stdx::abs(0.0));
}

TEST(AbsTest, LongDouble) {
  long double neg = -1.23456789012345L;
  long double pos = 1.23456789012345L;

  EXPECT_EQ(pos, stdx::abs(neg));
  EXPECT_EQ(pos, stdx::abs(pos));
  EXPECT_EQ(0.0L, stdx::abs(0.0L));
}

TEST(PowTest, FloatRaisedToInt) {
  float base = 2.0f;
  EXPECT_FLOAT_EQ(1.0f, stdx::pow(base, 0));
  EXPECT_FLOAT_EQ(2.0f, stdx::pow(base, 1));
  EXPECT_FLOAT_EQ(4.0f, stdx::pow(base, 2));
  EXPECT_FLOAT_EQ(8.0f, stdx::pow(base, 3));

  EXPECT_FLOAT_EQ(1.0f, stdx::pow(base, -0));
  EXPECT_FLOAT_EQ(0.5f, stdx::pow(base, -1));
  EXPECT_FLOAT_EQ(0.25f, stdx::pow(base, -2));
  EXPECT_FLOAT_EQ(0.125f, stdx::pow(base, -3));

  base = 1.5f;
  EXPECT_FLOAT_EQ(1.0f, stdx::pow(base, 0));
  EXPECT_FLOAT_EQ(1.5f, stdx::pow(base, 1));
  EXPECT_FLOAT_EQ(2.25f, stdx::pow(base, 2));
  EXPECT_FLOAT_EQ(3.375f, stdx::pow(base, 3));

  EXPECT_FLOAT_EQ(1.0f, stdx::pow(base, -0));
  EXPECT_FLOAT_EQ(1.0f / 1.5f, stdx::pow(base, -1));
  EXPECT_FLOAT_EQ(1.0f / 2.25f, stdx::pow(base, -2));
  EXPECT_FLOAT_EQ(1.0f / 3.375f, stdx::pow(base, -3));

  base = -1.5f;
  EXPECT_FLOAT_EQ(1.0f, stdx::pow(base, 0));
  EXPECT_FLOAT_EQ(-1.5f, stdx::pow(base, 1));
  EXPECT_FLOAT_EQ(2.25f, stdx::pow(base, 2));
  EXPECT_FLOAT_EQ(-3.375f, stdx::pow(base, 3));

  EXPECT_FLOAT_EQ(1.0f, stdx::pow(base, -0));
  EXPECT_FLOAT_EQ(-1.0f / 1.5f, stdx::pow(base, -1));
  EXPECT_FLOAT_EQ(1.0f / 2.25f, stdx::pow(base, -2));
  EXPECT_FLOAT_EQ(-1.0f / 3.375f, stdx::pow(base, -3));

  base = 0.0f;
  EXPECT_FLOAT_EQ(0.0f, stdx::pow(base, 5));
  EXPECT_FLOAT_EQ(1.0f, stdx::pow(base, 0));
}

TEST(PowTest, DoubleRaisedToInt) {
  double base = 2.0;
  EXPECT_DOUBLE_EQ(1.0, stdx::pow(base, 0));
  EXPECT_DOUBLE_EQ(2.0, stdx::pow(base, 1));
  EXPECT_DOUBLE_EQ(4.0, stdx::pow(base, 2));
  EXPECT_DOUBLE_EQ(8.0, stdx::pow(base, 3));

  EXPECT_DOUBLE_EQ(1.0f, stdx::pow(base, -0));
  EXPECT_DOUBLE_EQ(0.5, stdx::pow(base, -1));
  EXPECT_DOUBLE_EQ(0.25, stdx::pow(base, -2));
  EXPECT_DOUBLE_EQ(0.125, stdx::pow(base, -3));

  base = 1.5;
  EXPECT_DOUBLE_EQ(1.0, stdx::pow(base, 0));
  EXPECT_DOUBLE_EQ(1.5, stdx::pow(base, 1));
  EXPECT_DOUBLE_EQ(2.25, stdx::pow(base, 2));
  EXPECT_DOUBLE_EQ(3.375, stdx::pow(base, 3));

  EXPECT_DOUBLE_EQ(1.0, stdx::pow(base, -0));
  EXPECT_DOUBLE_EQ(1.0 / 1.5, stdx::pow(base, -1));
  EXPECT_DOUBLE_EQ(1.0 / 2.25, stdx::pow(base, -2));
  EXPECT_DOUBLE_EQ(1.0 / 3.375, stdx::pow(base, -3));

  base = -1.5;
  EXPECT_DOUBLE_EQ(1.0, stdx::pow(base, 0));
  EXPECT_DOUBLE_EQ(-1.5, stdx::pow(base, 1));
  EXPECT_DOUBLE_EQ(2.25, stdx::pow(base, 2));
  EXPECT_DOUBLE_EQ(-3.375, stdx::pow(base, 3));

  EXPECT_DOUBLE_EQ(1.0, stdx::pow(base, -0));
  EXPECT_DOUBLE_EQ(-1.0 / 1.5, stdx::pow(base, -1));
  EXPECT_DOUBLE_EQ(1.0 / 2.25, stdx::pow(base, -2));
  EXPECT_DOUBLE_EQ(-1.0 / 3.375, stdx::pow(base, -3));

  base = 0.0;
  EXPECT_DOUBLE_EQ(0.0, stdx::pow(base, 5));
  EXPECT_DOUBLE_EQ(1.0, stdx::pow(base, 0));
}
