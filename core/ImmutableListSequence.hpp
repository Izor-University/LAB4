#ifndef IMMUTABLE_LIST_SEQUENCE_HPP
#define IMMUTABLE_LIST_SEQUENCE_HPP

#include "ListSequence.hpp"

template <class T>
class ImmutableListSequence : public ListSequence<T> {
protected:
    // --- Фабричный метод инстанса ---
    virtual ListSequence<T>* Instance() override {
        return new ImmutableListSequence<T>(*this);
    }

public:
    // --- Внутренний билдер ---
    class Builder : public ISequenceBuilder<T> {
    private:
        ImmutableListSequence<T>* seq;

    public:
        Builder() {
            seq = new ImmutableListSequence<T>();
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
        return new ImmutableListSequence<T>();
    }

    virtual Sequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }

    // --- Конструкторы ---
    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(T* items, int count) : ListSequence<T>(items, count) {}
    ImmutableListSequence(const LinkedList<T>& list) : ListSequence<T>(list) {}
    ImmutableListSequence(const ListSequence<T>& seq) : ListSequence<T>(seq) {}

    // --- Деструктор ---
    virtual ~ImmutableListSequence() {}
};

#endif // IMMUTABLE_LIST_SEQUENCE_HPP