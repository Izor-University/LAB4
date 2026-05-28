#pragma once

#include "Exceptions.hpp"
#include "Option.hpp"
#include "IEnumerable.hpp"
#include "ICollection.hpp"
#include "ISequenceBuilder.hpp"


template <class T>
class Sequence : public IEnumerable<T>, public ICollection<T> {
public:
    virtual ~Sequence() {}

    // --- Фабричный метод билдера ---
    virtual ISequenceBuilder<T>* CreateBuilder() const = 0;

    // --- Виртуальные конструкторы (ICollection) ---
    virtual Sequence<T>* CreateEmpty() const override = 0;
    virtual Sequence<T>* Clone() const override = 0;

    // --- Чисто виртуальные методы декомпозиции ---
    virtual const T& Get(int index) const override = 0;
    virtual int GetLength() const override = 0;

    // --- Чисто виртуальные методы модификации ---
    virtual Sequence<T>* Append(const T& item) = 0;
    virtual Sequence<T>* Prepend(const T& item) = 0;
    virtual Sequence<T>* InsertAt(const T& item, int index) = 0;
    virtual const T& operator[](int index) const = 0;

    // --- Декомпозиция ---
    virtual const T& GetFirst() const;
    virtual const T& GetLast() const;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;

    // --- Операции ---
    virtual Sequence<T>* Concat(Sequence<T>* list) const = 0;
    virtual Sequence<T>* Slice(int index, int count, Sequence<T>* insertSeq = nullptr) = 0;

    // --- Try-семантика ---
    virtual Option<T> TryGetFirst() const;
    virtual Option<T> TryGetLast() const;
    virtual Option<T> TryGet(int index) const;

    // --- Map, Where, Reduce ---
    virtual Sequence<T>* Map(T (*mapper)(const T& element)) const;
    virtual Sequence<T>* Where(bool (*predicate)(const T& element)) const;
    virtual T Reduce(T (*reduce_func)(const T& accumulator, const T& current), const T& start_element) const;
};



#include "Sequence.tpp"
