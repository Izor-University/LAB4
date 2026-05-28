#ifndef IMMUTABLE_ARRAY_SEQUENCE_HPP
#define IMMUTABLE_ARRAY_SEQUENCE_HPP

#include "ArraySequence.hpp"

template <class T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
    // --- Фабричный метод инстанса ---
    virtual ArraySequence<T>* Instance() override {
        return new ImmutableArraySequence<T>(*this);
    }

public:
    // --- Внутренний билдер ---
    class Builder : public ISequenceBuilder<T> {
    private:
        ImmutableArraySequence<T>* seq;

    public:
        Builder() {
            seq = new ImmutableArraySequence<T>();
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
        return new ImmutableArraySequence<T>();
    }

    virtual Sequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }

    // --- Конструкторы ---
    ImmutableArraySequence() : ArraySequence<T>() {}
    ImmutableArraySequence(T* items, int count) : ArraySequence<T>(items, count) {}
    ImmutableArraySequence(const LinkedList<T>& list) : ArraySequence<T>(list) {}
    ImmutableArraySequence(const ArraySequence<T>& seq) : ArraySequence<T>(seq) {}

    // --- Деструктор ---
    virtual ~ImmutableArraySequence() {}
};

#endif // IMMUTABLE_ARRAY_SEQUENCE_HPP
