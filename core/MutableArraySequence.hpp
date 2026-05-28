#ifndef MUTABLE_ARRAY_SEQUENCE_HPP
#define MUTABLE_ARRAY_SEQUENCE_HPP

#include "ArraySequence.hpp"

template <class T>
class MutableArraySequence : public ArraySequence<T> {
protected:
    // --- Фабричный метод инстанса ---
    virtual ArraySequence<T>* Instance() override {
        return this;
    }

public:
    // --- Внутренний билдер ---
    class Builder : public ISequenceBuilder<T> {
    private:
        MutableArraySequence<T>* seq;

    public:
        Builder() {
            seq = new MutableArraySequence<T>();
        }

        virtual ~Builder() {
            if (seq) {
                delete seq;
            }
        }

        virtual ISequenceBuilder<T>* Append(const T& item) override {
            seq->AppendInternal(item);
            return this;
        }

        virtual Sequence<T>* Build() override {
            Sequence<T>* res = seq;
            seq = nullptr;
            return res;
        }
    };

    // --- Методы создания объектов ---
    virtual ISequenceBuilder<T>* CreateBuilder() const override {
        return new Builder();
    }

    virtual Sequence<T>* CreateEmpty() const override {
        return new MutableArraySequence<T>();
    }

    virtual Sequence<T>* Clone() const override {
        return new MutableArraySequence<T>(*this);
    }

    // --- Конструкторы ---
    MutableArraySequence() : ArraySequence<T>() {}
    MutableArraySequence(T* items, int count) : ArraySequence<T>(items, count) {}
    MutableArraySequence(const LinkedList<T>& list) : ArraySequence<T>(list) {}
    MutableArraySequence(const ArraySequence<T>& seq) : ArraySequence<T>(seq) {}

    // --- Деструктор ---
    virtual ~MutableArraySequence() {}
};

#endif // MUTABLE_ARRAY_SEQUENCE_HPP
