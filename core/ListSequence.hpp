#ifndef LIST_SEQUENCE_HPP
#define LIST_SEQUENCE_HPP

#include "Sequence.hpp"
#include "LinkedList.hpp"

template <class T>
class ListSequence : public Sequence<T> {
protected:
    LinkedList<T>* items;
    
    // --- Внутренние методы ---
    virtual ListSequence<T>* Instance() = 0;

    Sequence<T>* AppendInternal(const T& item);
    Sequence<T>* PrependInternal(const T& item);
    Sequence<T>* InsertAtInternal(const T& item, int index);

public:
    // --- Конструкторы и Деструктор ---
    ListSequence();
    ListSequence(T* items, int count);
    ListSequence(const LinkedList<T>& list);
    ListSequence(const ListSequence<T>& seq);
    virtual ~ListSequence();

    // --- Фабрика итератора ---
    IEnumerator<T>* GetEnumerator() const override {
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
    ListSequence<T>& operator=(const ListSequence<T>& other);
};

#include "ListSequence.tpp"
#endif // LIST_SEQUENCE_HPP
