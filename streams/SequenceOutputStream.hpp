#ifndef SEQUENCE_OUTPUT_STREAM_HPP
#define SEQUENCE_OUTPUT_STREAM_HPP

#include "IOutputStream.hpp"
#include "../core/Sequence.hpp"

template <class T>
class SequenceOutputStream : public IOutputStream<T> {
private:
    Sequence<T>* sequence;
    int position;
    bool isOpen;

public:
    explicit SequenceOutputStream(Sequence<T>* seq)
        : sequence(seq), position(0), isOpen(false) {
        if (seq != nullptr && seq->GetLength() != -1) {
            position = seq->GetLength();
        }
    }

    virtual void Open() override { isOpen = true; }
    virtual void Close() override { isOpen = false; }

    virtual Ordinal GetPosition() const override { return Ordinal(0, position); }

    virtual Ordinal Output(const T& item) override {
        if (!isOpen) throw Exception("Stream is closed");
        if (sequence->GetLength() == -1) {
            throw Exception("Cannot output to an infinite sequence");
        }

        Sequence<T>* newSeq = sequence->Append(item);

        // ИСПРАВЛЕНИЕ: Удаляем старую версию только если Append вернул НОВЫЙ объект.
        // (Для Immutable возвращается новый указатель, а для Mutable возвращается this)
        if (newSeq != sequence) {
            delete sequence;
        }

        sequence = newSeq;
        position++;
        return Ordinal(0, position);
    }

    Sequence<T>* GetSequence() const { return sequence; }
};

#endif // SEQUENCE_OUTPUT_STREAM_HPP