// =========================================================
// tests/TestSmartDataGenerator.cpp
// =========================================================

#include <gtest/gtest.h>
#include "../core/Ordinal.hpp"
#include "../generators/SmartDataGenerator.hpp"

// ---------------------------------------------------------
// 1. Тесты Равномерного распределения (Uniform)
// ---------------------------------------------------------
TEST(SmartDataGeneratorTest, UniformDistribution_Bounds) {
    double minVal = -50.0;
    double maxVal = 50.0;
    SmartDataGenerator gen(12345, DistributionType::Uniform, minVal, maxVal);

    // Генерируем 1000 элементов, чтобы убедиться, что ни один не вылетает за границы
    for (int i = 0; i < 1000; ++i) {
        double val = gen.Generate(Ordinal(0, i));
        EXPECT_GE(val, minVal);
        EXPECT_LE(val, maxVal);
    }
}

TEST(SmartDataGeneratorTest, UniformDistribution_Determinism) {
    SmartDataGenerator gen(42, DistributionType::Uniform, 0.0, 1.0);

    Ordinal targetIdx(1, 5); // w + 5

    // Запрос одного и того же индекса должен ВСЕГДА возвращать одинаковый результат
    double val1 = gen.Generate(targetIdx);
    double val2 = gen.Generate(targetIdx);
    EXPECT_DOUBLE_EQ(val1, val2);

    // Запрос другого индекса должен дать другое значение (с огромной долей вероятности)
    double val3 = gen.Generate(Ordinal(1, 6));
    EXPECT_NE(val1, val3);
}

// ---------------------------------------------------------
// 2. Тесты Нормального распределения (Irwin-Hall)
// ---------------------------------------------------------
TEST(SmartDataGeneratorTest, NormalDistribution_Statistics) {
    double mean = 100.0;
    double sigma = 15.0;
    SmartDataGenerator gen(999, DistributionType::Normal, mean, sigma);

    double sum = 0.0;
    int samples = 1000;

    for (int i = 0; i < samples; ++i) {
        double val = gen.Generate(Ordinal(0, i));
        sum += val;

        // По правилу "трех сигм" 99.7% значений лежат в пределах [mean - 3*sigma, mean + 3*sigma].
        // Для 1000 элементов LCG берем запас в 5 сигм, чтобы тест 100% не был плавающим (flaky test).
        EXPECT_GE(val, mean - 6.0 * sigma);
        EXPECT_LE(val, mean + 6.0 * sigma);
    }

    double calcMean = sum / samples;

    // Среднее значение выборки (calcMean) должно быть очень близко к заданному (mean).
    // Допускаем погрешность в 2.0 единицы (так как выборка конечна и используется LCG).
    EXPECT_NEAR(calcMean, mean, 2.0);
}

// ---------------------------------------------------------
// 3. Тест клонирования (Паттерн Prototype)
// ---------------------------------------------------------
TEST(SmartDataGeneratorTest, Clone_StatePreservation) {
    // Создаем генератор в куче
    SmartDataGenerator* originalGen = new SmartDataGenerator(1337, DistributionType::Normal, 10.0, 2.0);

    // Запоминаем значение для индекса 42
    double originalVal = originalGen->Generate(Ordinal(0, 42));

    // Клонируем
    IGenerator<double>* clonedGen = originalGen->Clone();

    // Удаляем оригинал, проверяем жизнеспособность клона
    delete originalGen;

    // Клон должен помнить все настройки (включая seed, mean, sigma)
    // и для индекса 42 обязан выдать ровно то же самое число.
    double clonedVal = clonedGen->Generate(Ordinal(0, 42));
    EXPECT_DOUBLE_EQ(originalVal, clonedVal);

    delete clonedGen;
}