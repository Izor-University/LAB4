#pragma once
#include "IGenerator.hpp"
#include "ISequence.hpp"
#include <functional>

template <typename T>
class RecurrentGenerator : public IGenerator<T> {
public:
    // Конструктор принимает функцию-правило генерации и указатель на историю элементов
    RecurrentGenerator(std::function<T(ISequence<T>*)> rule, ISequence<T>* history);
    
    virtual ~RecurrentGenerator();

    T GetNext() override;
    bool HasNext() const override;
    IGenerator<T>* Clone() const override;

private:
    std::function<T(ISequence<T>*)> rule_; // Правило порождения: f(history) -> T
    ISequence<T>* history_;                 // Ссылка на историю (не владеет ею)
};

#include "RecurrentGenerator.ipp"