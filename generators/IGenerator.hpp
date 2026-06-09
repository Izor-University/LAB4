#ifndef IGENERATOR_HPP
#define IGENERATOR_HPP

#include "../core/Ordinal.hpp"
#include "../core/Option.hpp"

template <class T>
class IGenerator {
public:
    virtual ~IGenerator() {}

    // Теперь возвращает Option<T>! Если элементов больше нет - вернет Option::None()
    virtual Option<T> Generate(const Ordinal& index) const = 0;

    virtual IGenerator<T>* Clone() const = 0;
};

#endif // IGENERATOR_HPP