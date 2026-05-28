#ifndef SMART_DATA_GENERATOR_HPP
#define SMART_DATA_GENERATOR_HPP

#include "IGenerator.hpp"

class SmartDataGenerator : public IGenerator<int> {
private:
    unsigned int baseSeed;

    // Вспомогательная функция для прокрутки LCG до нужного шага
    unsigned int GetLcgValue(int step) const {
        unsigned int current = baseSeed;
        // Константы из стандарта POSIX
        for (int i = 0; i <= step; ++i) {
            current = (current * 1103515245 + 12345) % 2147483648;
        }
        return current;
    }

public:
    explicit SmartDataGenerator(unsigned int seed = 42) : baseSeed(seed) {}

    virtual int Generate(int index) const override {
        // Мы гарантируем, что вызов Get(5) всегда вернет одно и то же число,
        // что необходимо для корректной работы LazySequence, если элемент еще не в кэше.
        return static_cast<int>(GetLcgValue(index) % 1000); // Ограничим диапазон для тестов (0-999)
    }

    virtual IGenerator<int>* Clone() const override {
        return new SmartDataGenerator(baseSeed);
    }
};

#endif // SMART_DATA_GENERATOR_HPP