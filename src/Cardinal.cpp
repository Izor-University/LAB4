#include "Cardinal.hpp"
#include <stdexcept>

// Конструктор по умолчанию (размер равен 0)
Cardinal::Cardinal() : type_(Finite), value_(0) {}

// Конструктор конечного размера
Cardinal::Cardinal(size_t val) : type_(Finite), value_(val) {}

// Приватный конструктор
Cardinal::Cardinal(Type type, size_t value) : type_(type), value_(value) {}

// Создание бесконечности
Cardinal Cardinal::MakeInfinity() {
    return Cardinal(Infinity, 0);
}

bool Cardinal::IsInfinity() const {
    return type_ == Infinity;
}

// Получение значения конечной длины
size_t Cardinal::GetValue() const {
    if (type_ == Infinity) {
        throw std::runtime_error("Attempted to read integer value of an infinite Cardinal");
    }
    return value_;
}