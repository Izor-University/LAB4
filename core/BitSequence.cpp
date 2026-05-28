#include "BitSequence.hpp"

// --- Приватный метод для расширения памяти ---
void BitSequence::Resize(int newCapacityBytes) {
    uint8_t* newData = new uint8_t[newCapacityBytes];

    for (int i = 0; i < capacityBytes; ++i) {
        newData[i] = data[i];
    }
    for (int i = capacityBytes; i < newCapacityBytes; ++i) {
        newData[i] = 0;
    }

    delete[] data;
    data = newData;
    capacityBytes = newCapacityBytes;
}

// --- Конструкторы и деструктор ---
BitSequence::BitSequence() : bitCount(0), capacityBytes(1) {
    data = new uint8_t[capacityBytes];
    data[0] = 0;
}

BitSequence::BitSequence(const BitSequence& other) : bitCount(other.bitCount), capacityBytes(other.capacityBytes) {
    data = new uint8_t[capacityBytes];
    for (int i = 0; i < capacityBytes; ++i) {
        data[i] = other.data[i];
    }
}

BitSequence& BitSequence::operator=(const BitSequence& other) {
    if (this == &other) {
        return *this;
    }

    delete[] data;

    bitCount = other.bitCount;
    capacityBytes = other.capacityBytes;
    data = new uint8_t[capacityBytes];
    for (int i = 0; i < capacityBytes; ++i) {
        data[i] = other.data[i];
    }

    return *this;
}

BitSequence::~BitSequence() {
    delete[] data;
}

// --- Доступ и модификация ---
int BitSequence::GetLength() const {
    return bitCount;
}

bool BitSequence::GetBit(int index) const {
    if (index < 0 || index >= bitCount) {
        throw IndexOutOfRange("Index out of range in BitSequence!");
    }
    int byteIndex = index / 8;
    int bitIndex = index % 8;

    return (data[byteIndex] >> bitIndex) & 1;
}

void BitSequence::AppendBit(bool value) {
    if (bitCount >= capacityBytes * 8) {
        Resize(capacityBytes * 2);
    }

    int byteIndex = bitCount / 8;
    int bitIndex = bitCount % 8;

    if (value) {
        data[byteIndex] |= (1 << bitIndex);
    } else {
        data[byteIndex] &= ~(1 << bitIndex);
    }

    bitCount++;
}

// --- Перегрузка побитовых операторов ---
BitSequence BitSequence::operator&(const BitSequence& other) const {
    int len1 = this->GetLength();
    int len2 = other.GetLength();
    int maxLength = (len1 > len2) ? len1 : len2;

    int offsetThis = maxLength - len1;
    int offsetOther = maxLength - len2;

    BitSequence resultSequence;

    for (int i = 0; i < maxLength; ++i) {
        bool bit1 = (i >= offsetThis) ? this->GetBit(i - offsetThis) : false;
        bool bit2 = (i >= offsetOther) ? other.GetBit(i - offsetOther) : false;

        resultSequence.AppendBit(bit1 && bit2);
    }

    return resultSequence;
}

BitSequence BitSequence::operator|(const BitSequence& other) const {
    int len1 = this->GetLength();
    int len2 = other.GetLength();
    int maxLength = (len1 > len2) ? len1 : len2;

    int offsetThis = maxLength - len1;
    int offsetOther = maxLength - len2;

    BitSequence resultSequence;

    for (int i = 0; i < maxLength; ++i) {
        bool bit1 = (i >= offsetThis) ? this->GetBit(i - offsetThis) : false;
        bool bit2 = (i >= offsetOther) ? other.GetBit(i - offsetOther) : false;

        resultSequence.AppendBit(bit1 || bit2);
    }

    return resultSequence;
}

BitSequence BitSequence::operator^(const BitSequence& other) const {
    int len1 = this->GetLength();
    int len2 = other.GetLength();
    int maxLength = (len1 > len2) ? len1 : len2;

    int offsetThis = maxLength - len1;
    int offsetOther = maxLength - len2;

    BitSequence resultSequence;

    for (int i = 0; i < maxLength; ++i) {
        bool bit1 = (i >= offsetThis) ? this->GetBit(i - offsetThis) : false;
        bool bit2 = (i >= offsetOther) ? other.GetBit(i - offsetOther) : false;

        resultSequence.AppendBit(bit1 != bit2);
    }

    return resultSequence;
}

BitSequence BitSequence::operator~() const {
    BitSequence resultSequence;

    for (int i = 0; i < this->GetLength(); ++i) {
        resultSequence.AppendBit(!this->GetBit(i));
    }

    return resultSequence;
}

// ============================================================================
// РЕАЛИЗАЦИЯ ИТЕРАТОРА
// ============================================================================
BitSequence::Enumerator::Enumerator(const BitSequence* seq)
    : sequence(seq), currentIndex(-1), currentBit(false) {}

bool BitSequence::Enumerator::MoveNext() {
    if (currentIndex + 1 < sequence->GetLength()) {
        currentIndex++;
        currentBit = sequence->GetBit(currentIndex);
        return true;
    }
    return false;
}

const bool& BitSequence::Enumerator::GetCurrent() const {
    if (currentIndex < 0 || currentIndex >= sequence->GetLength()) {
        throw IndexOutOfRange("Iterator out of bounds in BitSequence!");
    }
    return currentBit;
}

void BitSequence::Enumerator::Reset() {
    currentIndex = -1;
    currentBit = false;
}

IEnumerator<bool>* BitSequence::GetEnumerator() const {
    return new Enumerator(this);
}