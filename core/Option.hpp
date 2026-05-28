#ifndef OPTION_HPP
#define OPTION_HPP

#include "Exceptions.hpp"

template <class T>
class Option {
private:
    T value;
    bool hasValue;

public:
    // --- Конструкторы ---
    Option() : hasValue(false) {}
    Option(const T& val) : value(val), hasValue(true) {}

    // --- Декомпозиция ---
    bool IsSome() const {
        return hasValue;
    }

    bool IsNone() const {
        return !hasValue;
    }

    const T& GetValue() const {
        if (!hasValue) {
            throw Exception("Trying to unwrap Option::None!");
        }
        return value;
    }
};

#endif // OPTION_HPP
