#ifndef IGENERATOR_HPP
#define IGENERATOR_HPP

#include "../core/Ordinal.hpp"
#include "../core/Option.hpp"

template <class T>
class IGenerator {
public:
    virtual ~IGenerator() {}

    virtual Option<T> Generate(const Ordinal& index) const = 0;

    virtual IGenerator<T>* Clone() const = 0;
};

#endif // IGENERATOR_HPP