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

    virtual void Open() override {
        isOpen = true;
    }

    virtual void Close() override {
        isOpen = false;
    }

    virtual int Output(const T& item) override {
        if (!isOpen) throw Exception("Stream is closed");
        if (sequence->GetLength() == -1) {
            throw Exception("Cannot output to an infinite sequence");
        }

        // В core->Append может возвращаться как текущий объект (mutable), так и новый (immutable)
        Sequence<T>* newSeq = sequence->Append(item);

        // Если вернулся новый объект, переключаем указатель
        // (Очисткой памяти старого занимается внешний код или умная фабрика,
        // чтобы не нарушить владение)
        sequence = newSeq;

        position++;
        return position;
    }

    virtual int GetPosition() const override {
        return position;
    }

    // Вспомогательный метод для получения итоговых данных
    Sequence<T>* GetSequence() const {
        return sequence;
    }
};

#endif // SEQUENCE_OUTPUT_STREAM_HPP