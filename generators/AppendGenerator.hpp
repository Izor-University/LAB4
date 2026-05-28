#ifndef APPEND_GENERATOR_HPP
#define APPEND_GENERATOR_HPP

#include "IGenerator.hpp"

template <class T>
class AppendGenerator : public IGenerator<T> {
private:
    IGenerator<T>* previousGenerator;
    T appendedItem;
    Ordinal baseLength; // Теперь длина - это Ординал (например, w)

public:
    AppendGenerator(const IGenerator<T>* prevGen, const T& item, const Ordinal& prevLength)
        : appendedItem(item), baseLength(prevLength) {
        previousGenerator = prevGen->Clone();
    }

    virtual ~AppendGenerator() {
        delete previousGenerator;
    }

    // Правило трех... (Конструктор копирования и оператор= как раньше)

    virtual T Generate(const Ordinal& index) const override {
        // Если индекс ДО нашей вставки (даже если он где-то в глубине бесконечности)
        if (index < baseLength) {
            return previousGenerator->Generate(index);
        }
        // Если индекс указывает ровно на добавленный элемент
        else if (index == baseLength) {
            return appendedItem;
        }

        throw IndexOutOfRange("AppendGenerator: Index is beyond the appended element");
    }

    virtual IGenerator<T>* Clone() const override {
        // ...
    }
};

#endif // APPEND_GENERATOR_HPP