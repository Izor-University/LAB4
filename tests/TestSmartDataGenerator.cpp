// =========================================================
// tests/TestSmartDataGenerator.cpp
// =========================================================

#include <gtest/gtest.h>
#include "../core/Ordinal.hpp"
#include "../generators/SmartDataGenerator.hpp"

TEST(SmartDataGeneratorTest, UniformDistribution_Bounds) {
    double minVal = -50.0;
    double maxVal = 50.0;
    SmartDataGenerator gen(12345, DistributionType::Uniform, minVal, maxVal);

    for (int i = 0; i < 1000; ++i) {
        double val = gen.Generate(Ordinal(0, i)).GetValue();
        EXPECT_GE(val, minVal);
        EXPECT_LE(val, maxVal);
    }
}

TEST(SmartDataGeneratorTest, UniformDistribution_Determinism) {
    SmartDataGenerator gen(42, DistributionType::Uniform, 0.0, 1.0);
    Ordinal targetIdx(1, 5);

    double val1 = gen.Generate(targetIdx).GetValue();
    double val2 = gen.Generate(targetIdx).GetValue();
    EXPECT_DOUBLE_EQ(val1, val2);

    double val3 = gen.Generate(Ordinal(1, 6)).GetValue();
    EXPECT_NE(val1, val3);
}

TEST(SmartDataGeneratorTest, NormalDistribution_Statistics) {
    double mean = 100.0;
    double sigma = 15.0;
    SmartDataGenerator gen(999, DistributionType::Normal, mean, sigma);

    double sum = 0.0;
    int samples = 1000;

    for (int i = 0; i < samples; ++i) {
        double val = gen.Generate(Ordinal(0, i)).GetValue();
        sum += val;
        // Ирвин-Холл строго ограничен +- 6 сигмами
        EXPECT_GE(val, mean - 6.0 * sigma);
        EXPECT_LE(val, mean + 6.0 * sigma);
    }

    double calcMean = sum / samples;
    EXPECT_NEAR(calcMean, mean, 2.0);
}

TEST(SmartDataGeneratorTest, Clone_StatePreservation) {
    SmartDataGenerator* originalGen = new SmartDataGenerator(1337, DistributionType::Normal, 10.0, 2.0);
    double originalVal = originalGen->Generate(Ordinal(0, 42)).GetValue();

    IGenerator<double>* clonedGen = originalGen->Clone();
    delete originalGen;

    double clonedVal = clonedGen->Generate(Ordinal(0, 42)).GetValue();
    EXPECT_DOUBLE_EQ(originalVal, clonedVal);

    delete clonedGen;
}