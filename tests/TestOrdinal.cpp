// =========================================================
// tests/TestOrdinal.cpp
// =========================================================

#include <gtest/gtest.h>
#include "../core/Ordinal.hpp"
#include "../core/Exceptions.hpp"

TEST(OrdinalTest, Creation_FiniteAndInfinite) {
    Ordinal zero;
    Ordinal five(5);
    EXPECT_TRUE(zero.IsFinite());
    EXPECT_TRUE(five.IsFinite());
    EXPECT_FALSE(five.IsInfinite());
    EXPECT_EQ(five.GetOmegaCount(), 0);
    EXPECT_EQ(five.GetOffset(), 5);

    Ordinal omega = Ordinal::Omega();
    EXPECT_FALSE(omega.IsFinite());
    EXPECT_TRUE(omega.IsInfinite());
    EXPECT_EQ(omega.GetOmegaCount(), 1);
    EXPECT_EQ(omega.GetOffset(), 0);
}

TEST(OrdinalTest, Creation_NegativeThrows) {
    EXPECT_THROW(Ordinal(-1), Exception);
    EXPECT_THROW(Ordinal(-5, 10), Exception);
    EXPECT_THROW(Ordinal(1, -5), Exception);
}

TEST(OrdinalTest, Comparison_Finite) {
    Ordinal a(5), b(10), c(5);
    EXPECT_EQ(a, c);
    EXPECT_NE(a, b);
    EXPECT_LT(a, b);
    EXPECT_GT(b, a);
}

TEST(OrdinalTest, Comparison_Transfinite) {
    Ordinal ten(10);
    Ordinal omega = Ordinal::Omega();
    Ordinal w_plus_5(1, 5);
    Ordinal w2_plus_3(2, 3);

    EXPECT_LT(ten, omega);
    EXPECT_LT(omega, w_plus_5);
    EXPECT_LT(w_plus_5, w2_plus_3);
    EXPECT_GT(w2_plus_3, w_plus_5);
}

TEST(OrdinalTest, Addition_Absorption) {
    Ordinal five(5);
    Ordinal omega = Ordinal::Omega();
    EXPECT_EQ(five + omega, omega); // 5 + w = w

    Ordinal w_plus_5(1, 5);
    EXPECT_EQ(w_plus_5 + omega, Ordinal(2, 0)); // (w + 5) + w = w*2
}

TEST(OrdinalTest, Addition_Transfinite) {
    EXPECT_EQ(Ordinal(5) + Ordinal(3), Ordinal(8));
    EXPECT_EQ(Ordinal::Omega() + Ordinal(5), Ordinal(1, 5));
    EXPECT_EQ(Ordinal(1, 5) + Ordinal(1, 3), Ordinal(2, 3));
    EXPECT_EQ(Ordinal(5, 10) + Ordinal(2, 4), Ordinal(7, 4));
}

TEST(OrdinalTest, Subtraction_Valid) {
    EXPECT_EQ(Ordinal(10) - Ordinal(3), Ordinal(7));
    EXPECT_EQ(Ordinal(1, 5) - Ordinal::Omega(), Ordinal(5));
    EXPECT_EQ(Ordinal(1, 10) - Ordinal(1, 3), Ordinal(7));
    EXPECT_EQ(Ordinal(3, 5) - Ordinal(1, 100), Ordinal(2, 5));
}

TEST(OrdinalTest, Subtraction_InvalidThrows) {
    EXPECT_THROW(Ordinal(5) - Ordinal(10), IndexOutOfRange);
    EXPECT_THROW(Ordinal(5) - Ordinal::Omega(), IndexOutOfRange);
    EXPECT_THROW(Ordinal(1, 5) - Ordinal(2, 0), IndexOutOfRange);
    EXPECT_THROW(Ordinal(1, 5) - Ordinal(1, 10), IndexOutOfRange);
}