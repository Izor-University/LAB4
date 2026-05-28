#ifndef LAZY_INPUT_STREAM_HPP
#define LAZY_INPUT_STREAM_HPP

#include "IInputStream.hpp"
#include "../sequences/LazySequence.hpp"

template <class T>
class LazyInputStream : public IInputStream<T> {
private:
    const LazySequence<T>* sequence;
    Ordinal position;
    bool isOpen;

public:
    explicit LazyInputStream(const LazySequence<T>* seq)
        : sequence(seq), position(0, 0), isOpen(false) {}

    virtual void Open() override {
        isOpen = true;
        position = Ordinal(0, 0); // Начинаем с самого начала (0-й элемент 0-й бесконечности)
    }

    virtual void Close() override {
        isOpen = false;
    }

    virtual bool IsEndOfStream() const override {
        if (!isOpen) return true;
        // Благодаря перегруженным операторам сравнения в Ordinal,
        // это корректно работает для любой длины (конечной, w, w+5 и т.д.)
        return position >= sequence->GetOrdinalLength();
    }

    virtual Ordinal GetPosition() const override {
        return position;
    }

    virtual bool IsCanSeek() const override {
        return true;
    }

    virtual bool IsCanGoBack() const override {
        return true;
    }

    virtual Ordinal Seek(const Ordinal& index) override {
        if (!isOpen) throw Exception("Stream is closed");

        Ordinal len = sequence->GetOrdinalLength();
        if (index >= len) {
            position = len; // Нельзя прыгнуть дальше длины
        } else {
            position = index;
        }
        return position;
    }

    virtual T Input() override {
        if (!isOpen) throw Exception("Stream is closed");
        if (IsEndOfStream()) throw IndexOutOfRange("End of stream reached");

        // Получаем элемент по ординалу. Это триггерит кэширование на нужном уровне!
        T item = sequence->GetByOrdinal(position);

        // Сдвигаем позицию на 1 шаг вперед (арифметика Кантора обработает это безопасно)
        position = position + Ordinal(0, 1);

        return item;
    }
};

#endif // LAZY_INPUT_STREAM_HPP