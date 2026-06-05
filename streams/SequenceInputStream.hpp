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

    virtual void Open() override { isOpen = true; position = 0; }
    virtual void Close() override { isOpen = false; }

    virtual bool IsEndOfStream() const override {
        if (!isOpen) return true;
        int len = sequence->GetLength();
        if (len == -1) return false;
        return position >= len;
    }

    virtual Ordinal GetPosition() const override { return Ordinal(0, position); }
    virtual bool IsCanSeek() const override { return true; }
    virtual bool IsCanGoBack() const override { return true; }

    virtual Ordinal Seek(const Ordinal& index) override {
        if (!isOpen) throw Exception("Stream is closed");
        int len = sequence->GetLength();
        int reqIndex = index.GetOffset(); // Извлекаем int

        if (reqIndex < 0) {
            position = 0;
        } else if (len != -1 && reqIndex > len) {
            position = len;
        } else {
            position = reqIndex;
        }
        return Ordinal(0, position);
    }

    virtual T Input() override {
        if (!isOpen) throw Exception("Stream is closed");
        if (IsEndOfStream()) throw IndexOutOfRange("End of stream reached");
        return sequence->Get(position++);
    }
};

#endif // SEQUENCE_INPUT_STREAM_HPP