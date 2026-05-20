#pragma once
#include <cstddef>

class Cardinal {
public:
    enum Type { 
        Finite, 
        Infinity 
    };

    // Конструкторы
    Cardinal();
    Cardinal(size_t val);

    // Статический фабричный метод для создания бесконечной длины
    static Cardinal MakeInfinity();

    // Методы опроса состояния
    bool IsInfinity() const;
    size_t GetValue() const;

private:
    Type type_;
    size_t value_;

    // Приватный конструктор для фабричного метода
    Cardinal(Type type, size_t value);
};