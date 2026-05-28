#ifndef SMART_DATA_GENERATOR_HPP
#define SMART_DATA_GENERATOR_HPP

#include "IGenerator.hpp"

// --- Типы поддерживаемых распределений ---
enum class DistributionType {
    Uniform, // Равномерное
    Normal   // Нормальное (Гауссовское)
};

class SmartDataGenerator : public IGenerator<double> {
private:
    unsigned int baseSeed;
    DistributionType distType;
    double param1; // min для Uniform, Mean (матожидание) для Normal
    double param2; // max для Uniform, Sigma (отклонение) для Normal

    // Внутренний LCG (Линейный конгруэнтный метод), возвращающий число в диапазоне [0.0; 1.0]
    double GetLcgFloat(int step, unsigned int localSeed) const {
        unsigned int current = localSeed;

        // Для соблюдения чистой функциональности генератора просто прокручиваем
        // состояние до нужного индекса. Это делает генератор детерминированным.
        for (int i = 0; i <= step; ++i) {
            current = (current * 1103515245 + 12345) % 2147483648;
        }

        return static_cast<double>(current) / 2147483648.0;
    }

public:
    // По умолчанию: равномерное распределение от 0.0 до 100.0
    explicit SmartDataGenerator(unsigned int seed = 42,
                                DistributionType type = DistributionType::Uniform,
                                double p1 = 0.0, double p2 = 100.0)
        : baseSeed(seed), distType(type), param1(p1), param2(p2) {}

    virtual double Generate(const Ordinal& index) const override {
        // Превращаем ординал в уникальный "шаг".
        // Учитываем бесконечности (k) и конечное смещение (n).
        int step = index.GetOffset() + index.GetOmegaCount() * 100000;

        if (distType == DistributionType::Uniform) {
            // Равномерное распределение: param1 = min, param2 = max
            double u = GetLcgFloat(step, baseSeed);
            return param1 + u * (param2 - param1);
        }
        else if (distType == DistributionType::Normal) {
            // Нормальное распределение: param1 = Mean, param2 = Sigma
            // Используем распределение Ирвина-Холла (сумма 12 равномерных чисел)
            double sum = 0.0;
            for (int i = 0; i < 12; ++i) {
                // Смещаем seed, чтобы 12 слагаемых были псевдонезависимы
                sum += GetLcgFloat(step, baseSeed + i * 9999);
            }

            // Сумма 12 чисел U(0,1) минус 6 дает приближение N(0, 1)
            double standardNormal = sum - 6.0;

            // Масштабируем до заданного матожидания и отклонения
            return param1 + standardNormal * param2;
        }

        return 0.0;
    }

    virtual IGenerator<double>* Clone() const override {
        return new SmartDataGenerator(baseSeed, distType, param1, param2);
    }
};

#endif // SMART_DATA_GENERATOR_HPP