#ifndef LAZY_OUTPUT_STREAM_HPP
#define LAZY_OUTPUT_STREAM_HPP

#include "IOutputStream.hpp"
#include "../sequences/LazySequence.hpp"

template <class T>
class LazyOutputStream : public IOutputStream<T> {
private:
    LazySequence<T>* sequence;
    Ordinal position;
    bool isOpen;

public:
    explicit LazyOutputStream(LazySequence<T>* seq)
        : sequence(seq), isOpen(false) {
        if (seq != nullptr) {
            position = seq->GetOrdinalLength();
        } else {
            position = Ordinal(0, 0);
        }
    }

    virtual void Open() override {
        isOpen = true;
    }

    virtual void Close() override {
        isOpen = false;
    }

    virtual Ordinal GetPosition() const override {
        return position;
    }

    virtual Ordinal Output(const T& item) override {
        if (!isOpen) throw Exception("Stream is closed");

        Sequence<T>* newSeqBase = sequence->Append(item);
        LazySequence<T>* newLazySeq = static_cast<LazySequence<T>*>(newSeqBase);

        // ИСПРАВЛЕНИЕ: Защита от удаления себя (на будущее)
        if (newLazySeq != sequence) {
            delete sequence;
        }

        sequence = newLazySeq;
        position = sequence->GetOrdinalLength();

        return position;
    }

    LazySequence<T>* GetSequence() const {
        return sequence;
    }
};

#endif // LAZY_OUTPUT_STREAM_HPP