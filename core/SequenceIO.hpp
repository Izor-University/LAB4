#ifndef SEQUENCE_IO_HPP
#define SEQUENCE_IO_HPP

#include <ostream>
#include "Sequence.hpp"
#include "BitSequence.hpp"

// --- Универсальный вывод для любой коллекции Sequence<T> ---
template <class T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq) {
    os << "[";
    IEnumerator<T>* en = seq.GetEnumerator();
    bool isFirst = true;

    while (en->MoveNext()) {
        if (!isFirst) {
            os << ", ";
        }
        os << en->GetCurrent();
        isFirst = false;
    }
    delete en;
    os << "]";
    return os;
}

// --- Специфичный вывод для компактной BitSequence ---
inline std::ostream& operator<<(std::ostream& os, const BitSequence& b) {
    for (int i = 0; i < b.GetLength(); ++i) {
        os << (b.GetBit(i) ? "1" : "0");
    }
    return os;
}

#endif // SEQUENCE_IO_HPP