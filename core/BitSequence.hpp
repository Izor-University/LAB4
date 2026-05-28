#ifndef BIT_SEQUENCE_HPP
#define BIT_SEQUENCE_HPP

#include <cstdint>
#include "Exceptions.hpp"
#include "IEnumerable.hpp"

class BitSequence : public IEnumerable<bool> {
private:
    uint8_t* data;
    int bitCount;
    int capacityBytes;

    void Resize(int newCapacityBytes);

public:
    // --- Внутренний итератор ---
    class Enumerator : public IEnumerator<bool> {
    private:
        const BitSequence* sequence;
        int currentIndex;
        bool currentBit; // Буфер для возврата по ссылке

    public:
        explicit Enumerator(const BitSequence* seq);
        bool MoveNext() override;
        const bool& GetCurrent() const override;
        void Reset() override;
    };

    // --- Конструкторы и деструктор ---
    BitSequence();
    BitSequence(const BitSequence& other);
    BitSequence& operator=(const BitSequence& other);
    ~BitSequence();

    // --- Доступ и модификация ---
    int GetLength() const;
    bool GetBit(int index) const;
    void AppendBit(bool value);

    // --- Фабричный метод итератора ---
    IEnumerator<bool>* GetEnumerator() const override;

    // --- Перегрузка побитовых операторов ---
    BitSequence operator&(const BitSequence& other) const;
    BitSequence operator|(const BitSequence& other) const;
    BitSequence operator^(const BitSequence& other) const;
    BitSequence operator~() const;
};

#endif // BIT_SEQUENCE_HPP