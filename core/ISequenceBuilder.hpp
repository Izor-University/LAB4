#ifndef ISEQUENCE_BUILDER_HPP
#define ISEQUENCE_BUILDER_HPP

// Предварительное объявление для избежания циклических зависимостей
template <class T>
class Sequence;

template <class T>
class ISequenceBuilder {
public:
    virtual ~ISequenceBuilder() {}

    // --- Модификация ---
    virtual ISequenceBuilder<T>* Append(const T& item) = 0;

    // --- Построение ---
    virtual Sequence<T>* Build() = 0;
};

#endif // ISEQUENCE_BUILDER_HPP
