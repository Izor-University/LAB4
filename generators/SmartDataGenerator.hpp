#ifndef SMART_DATA_GENERATOR_HPP
#define SMART_DATA_GENERATOR_HPP

#include "IGenerator.hpp"

enum class DistributionType {
    Uniform,
    Normal
};

class SmartDataGenerator : public IGenerator<double> {
private:
    unsigned int baseSeed;
    DistributionType distType;
    double param1;
    double param2;

    double GetLcgFloat(int step, unsigned int localSeed) const {
        unsigned int current = localSeed;
        for (int i = 0; i <= step; ++i) {
            current = (current * 1103515245 + 12345) % 2147483648;
        }
        return static_cast<double>(current) / 2147483648.0;
    }

public:
    explicit SmartDataGenerator(unsigned int seed = 42,
                                DistributionType type = DistributionType::Uniform,
                                double p1 = 0.0, double p2 = 100.0)
        : baseSeed(seed), distType(type), param1(p1), param2(p2) {}

    virtual Option<double> Generate(const Ordinal& index) const override {
        int step = index.GetOffset() + index.GetOmegaCount() * 100000;

        if (distType == DistributionType::Uniform) {
            double u = GetLcgFloat(step, baseSeed);
            return Option<double>(param1 + u * (param2 - param1));
        }
        else if (distType == DistributionType::Normal) {
            double sum = 0.0;
            for (int i = 0; i < 12; ++i) {
                sum += GetLcgFloat(step, baseSeed + i * 9999);
            }
            double standardNormal = sum - 6.0;
            return Option<double>(param1 + standardNormal * param2);
        }

        return Option<double>(); // Возвращаем None
    }

    virtual IGenerator<double>* Clone() const override {
        return new SmartDataGenerator(baseSeed, distType, param1, param2);
    }
};

#endif // SMART_DATA_GENERATOR_HPP