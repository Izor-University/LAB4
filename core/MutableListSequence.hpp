#ifndef MUTABLE_LIST_SEQUENCE_HPP
#define MUTABLE_LIST_SEQUENCE_HPP

#include "ListSequence.hpp"

template <class T>
class MutableListSequence : public ListSequence<T> {
protected:
    // --- Фабричный метод инстанса ---
    virtual ListSequence<T>* Instance() override {
        return this;
    }

public:
    // --- Внутренний билдер ---
    class Builder : public ISequenceBuilder<T> {
    private:
        MutableListSequence<T>* seq;

    public:
        Builder() {
            seq = new MutableListSequence<T>();
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
        return new MutableListSequence<T>();
    }

    virtual Sequence<T>* Clone() const override {
        return new MutableListSequence<T>(*this);
    }

    // --- Конструкторы ---
    MutableListSequence() : ListSequence<T>() {}
    MutableListSequence(T* items, int count) : ListSequence<T>(items, count) {}
    MutableListSequence(const LinkedList<T>& list) : ListSequence<T>(list) {}
    MutableListSequence(const ListSequence<T>& seq) : ListSequence<T>(seq) {}

    // --- Деструктор ---
    virtual ~MutableListSequence() {}
};

#endif // MUTABLE_LIST_SEQUENCE_HPP