#ifndef FUNCTION_GENERATOR_HPP
#define FUNCTION_GENERATOR_HPP

#include "IGenerator.hpp"

template <class T>
class FunctionGenerator : public IGenerator<T> {
private:
    T (*rule)(const Ordinal&);

public:
    explicit FunctionGenerator(T (*ruleFunc)(const Ordinal&)) : rule(ruleFunc) {}

    virtual Option<T> Generate(const Ordinal& index) const override {
        return Option<T>(rule(index));
    }

    virtual IGenerator<T>* Clone() const override {
        return new FunctionGenerator<T>(rule);
    }
};

#endif // FUNCTION_GENERATOR_HPP