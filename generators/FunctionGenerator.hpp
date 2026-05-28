#ifndef FUNCTION_GENERATOR_HPP
#define FUNCTION_GENERATOR_HPP

#include "IGenerator.hpp"

template <class T>
class FunctionGenerator : public IGenerator<T> {
private:
    T (*rule)(const Ordinal&); // Теперь принимает Ordinal

public:
    explicit FunctionGenerator(T (*ruleFunc)(const Ordinal&)) : rule(ruleFunc) {}

    virtual T Generate(const Ordinal& index) const override {
        return rule(index);
    }

    virtual IGenerator<T>* Clone() const override {
        return new FunctionGenerator<T>(rule);
    }
};

#endif // FUNCTION_GENERATOR_HPP