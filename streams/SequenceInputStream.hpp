#ifndef SEQUENCE_INPUT_STREAM_HPP
#define SEQUENCE_INPUT_STREAM_HPP

#include "IInputStream.hpp"
#include "../core/Sequence.hpp"

template <class T>
class SequenceInputStream : public IInputStream<T> {
private:
    const Sequence<T>* sequence;
    int position;
    bool isOpen;

public:
    explicit SequenceInputStream(const Sequence<T>* seq)
        : sequence(seq), position(0), isOpen(false) {}

    virtual void Open() override {
        isOpen = true;
        position = 0;
    }

    virtual void Close() override {
        isOpen = false;
    }

    virtual bool IsEndOfStream() const override {
        if (!isOpen) return true;
        int len = sequence->GetLength();
        if (len == -1) return false; // Бесконечная ленивая последовательность никогда не заканчивается!
        return position >= len;
    }

    virtual int GetPosition() const override {
        return position;
    }

    virtual bool IsCanSeek() const override {
        return true; // В памяти мы всегда можем перемещаться
    }

    virtual bool IsCanGoBack() const override {
        return true;
    }

    virtual T Input() override {
        if (!isOpen) throw Exception("Stream is closed");
        if (IsEndOfStream()) throw IndexOutOfRange("End of stream reached");

        // Вызов Get() автоматически запустит вычисления в LazySequence, если нужно
        return sequence->Get(position++);
    }

    virtual int Seek(int index) override {
        if (!isOpen) throw Exception("Stream is closed");

        int len = sequence->GetLength();
        if (index < 0) {
            position = 0;
        } else if (len != -1 && index > len) {
            position = len;
        } else {
            position = index;
        }
        return position;
    }
};

#endif // SEQUENCE_INPUT_STREAM_HPP