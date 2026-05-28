#ifndef ORDINAL_HPP
#define ORDINAL_HPP

#include "Exceptions.hpp"

class Ordinal {
private:
    int omegaCount; // k (количество бесконечностей)
    int offset;     // n (конечное смещение)

public:
    Ordinal() : omegaCount(0), offset(0) {}
    Ordinal(int n) : omegaCount(0), offset(n) {
        if (n < 0) throw Exception("Ordinal cannot be negative");
    }
    Ordinal(int k, int n) : omegaCount(k), offset(n) {
        if (k < 0 || n < 0) throw Exception("Ordinal components cannot be negative");
    }

    static Ordinal Omega() { return Ordinal(1, 0); }
    static Ordinal Finite(int n) { return Ordinal(0, n); }

    int GetOmegaCount() const { return omegaCount; }
    int GetOffset() const { return offset; }

    bool IsFinite() const { return omegaCount == 0; }
    bool IsInfinite() const { return omegaCount > 0; }

    // --- Сравнения ---
    bool operator==(const Ordinal& other) const {
        return omegaCount == other.omegaCount && offset == other.offset;
    }
    bool operator!=(const Ordinal& other) const { return !(*this == other); }

    bool operator<(const Ordinal& other) const {
        if (omegaCount < other.omegaCount) return true;
        if (omegaCount == other.omegaCount) return offset < other.offset;
        return false;
    }
    bool operator<=(const Ordinal& other) const { return (*this < other) || (*this == other); }
    bool operator>(const Ordinal& other) const { return !(*this <= other); }
    bool operator>=(const Ordinal& other) const { return !(*this < other); }

    // --- Алгебра Кантора ---

    // Сложение: конечный сдвиг слева поглощается бесконечностью справа
    // Пример: (w*1 + 5) + (w*1 + 3) = w*2 + 3
    Ordinal operator+(const Ordinal& other) const {
        if (other.omegaCount == 0) {
            return Ordinal(omegaCount, offset + other.offset);
        } else {
            return Ordinal(omegaCount + other.omegaCount, other.offset);
        }
    }

    // Вычитание Кантора (левое): a - b = x такое, что b + x = a
    // Пример: (w*2 + 5) - (w*1 + 3) = (w*1 + 5)
    Ordinal operator-(const Ordinal& other) const {
        if (*this < other) throw IndexOutOfRange("Resulting ordinal cannot be negative");
        if (omegaCount == other.omegaCount) {
            return Ordinal(0, offset - other.offset);
        }
        return Ordinal(omegaCount - other.omegaCount, offset);
    }
};

#endif // ORDINAL_HPP