#pragma once
#include "IGenerator.hpp"
#include "ISequence.hpp"

template <typename T>
class PrependGenerator : public IGenerator<T> {
public:
    PrependGenerator(T item, ISequence<T>* source);
    virtual ~PrependGenerator();

    T GetNext() override;
    bool HasNext() const override;
    IGenerator<T>* Clone() const override;

private:
    T item_;                    // Элемент для вставки в начало
    ISequence<T>* source_;      // Исходная последовательность
    bool first_returned_;       // Флаг: был ли возвращен первый элемент
    size_t source_index_;       // Текущий индекс чтения из исходной последовательности
};

#include "PrependGenerator.ipp"