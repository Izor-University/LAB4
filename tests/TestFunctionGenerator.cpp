// =========================================================
// tests/TestFunctionGenerator.cpp
// =========================================================

#include <gtest/gtest.h>
#include "../core/Ordinal.hpp"
#include "../generators/FunctionGenerator.hpp"

namespace {
    int DoubleRule(const Ordinal& index) {
        return index.GetOffset() * 2;
    }

    int TransfiniteRule(const Ordinal& index) {
        return (index.GetOmegaCount() * 1000) + index.GetOffset();
    }
}

TEST(FunctionGeneratorTest, Generate_BasicRule) {
    FunctionGenerator<int> gen(DoubleRule);

    EXPECT_EQ(gen.Generate(Ordinal(0, 0)), 0);
    EXPECT_EQ(gen.Generate(Ordinal(0, 5)), 10);
    EXPECT_EQ(gen.Generate(Ordinal(0, 42)), 84);
}

TEST(FunctionGeneratorTest, Generate_TransfiniteIndex) {
    FunctionGenerator<int> gen(TransfiniteRule);

    EXPECT_EQ(gen.Generate(Ordinal(0, 5)), 5);
    EXPECT_EQ(gen.Generate(Ordinal::Omega()), 1000);
    EXPECT_EQ(gen.Generate(Ordinal(2, 15)), 2015);
}

TEST(FunctionGeneratorTest, Clone_Independence) {
    IGenerator<int>* originalGen = new FunctionGenerator<int>(DoubleRule);
    IGenerator<int>* clonedGen = originalGen->Clone();

    delete originalGen;

    EXPECT_EQ(clonedGen->Generate(Ordinal(0, 10)), 20);
    EXPECT_EQ(clonedGen->Generate(Ordinal(0, 50)), 100);

    delete clonedGen;
}