#ifndef ARRAY_SEQUENCE_HPP
#define ARRAY_SEQUENCE_HPP

#include "Sequence.hpp"
#include "DynamicArray.hpp"
#include "LinkedList.hpp"

template <class T>
class ArraySequence : public Sequence<T> {
protected:
    DynamicArray<T>* items;
    
    // --- Внутренние методы ---
    virtual ArraySequence<T>* Instance() = 0;

    Sequence<T>* AppendInternal(const T& item);
    Sequence<T>* PrependInternal(const T& item);
    Sequence<T>* InsertAtInternal(const T& item, int index);

public:
    // --- Конструкторы и Деструктор ---
    ArraySequence();
    ArraySequence(T* items, int count);
    ArraySequence(const LinkedList<T>& list);
    ArraySequence(const ArraySequence<T>& seq);
    virtual ~ArraySequence();

    // --- Фабрика итератора ---
    IEnumerator<T>* GetEnumerator() const override {
        // Делегируем создание итератора самому массиву!
        return items->GetEnumerator();
    }

    // --- Декомпозиция ---
    virtual const T& Get(int index) const override;
    virtual int GetLength() const override;

    // --- Операции ---
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    virtual Sequence<T>* Concat(Sequence<T>* list) const override;
    virtual Sequence<T>* Slice(int index, int count, Sequence<T>* insertSeq = nullptr) override;

    // --- Модификация ---
    virtual Sequence<T>* Append(const T& item) override;
    virtual Sequence<T>* Prepend(const T& item) override;
    virtual Sequence<T>* InsertAt(const T& item, int index) override;

    virtual const T& operator[](int index) const override;
    ArraySequence<T>& operator=(const ArraySequence<T>& other);
};

#include "ArraySequence.tpp"
#endif // ARRAY_SEQUENCE_HPP
