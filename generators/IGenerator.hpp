#ifndef IGENERATOR_HPP
#define IGENERATOR_HPP

#include "../core/Ordinal.hpp"

template <class T>
class IGenerator {
public:
    virtual ~IGenerator() {}

    // Теперь запрашиваем элемент по Ординалу!
    virtual T Generate(const Ordinal& index) const = 0;

    virtual IGenerator<T>* Clone() const = 0;
};

#endif // IGENERATOR_HPP