#ifndef UTILS_HPP
#define UTILS_HPP

#include "Sequence.hpp"
#include "MutableArraySequence.hpp"

// --- Вспомогательная структура пары ---
template <typename T1, typename T2>
struct Pair {
    T1 first;
    T2 second;

    // Конструкторы
    Pair() : first(), second() {}
    Pair(T1 f, T2 s) : first(f), second(s) {}

    // Операторы
    bool operator==(const Pair& other) const {
        return first == other.first && second == other.second;
    }
};

// --- Утилиты для работы с последовательностями ---
class SequenceUtils {
public:
    // --- Разделение по элементу (Split) ---
    template <typename T>
    static Sequence<Sequence<T>*>* Split(const Sequence<T>& sequence, const T& splitElement) {
        Sequence<Sequence<T>*>* resultSequence = new MutableArraySequence<Sequence<T>*>();
        Sequence<T>* currentChunk = new MutableArraySequence<T>();

        try {
            for (int i = 0; i < sequence.GetLength(); i++) {
                if (sequence[i] == splitElement) {
                    resultSequence->Append(currentChunk);
                    currentChunk = new MutableArraySequence<T>();
                } else {
                    currentChunk->Append(sequence[i]);
                }
            }
            resultSequence->Append(currentChunk);
            return resultSequence;
        } catch (...) {
            delete currentChunk;
            for (int i = 0; i < resultSequence->GetLength(); i++) {
                delete (*resultSequence)[i];
            }
            delete resultSequence;
            throw;
        }
    }

    // --- Сцепление пар (Zip) ---
    template <typename T1, typename T2>
    static Sequence<Pair<T1, T2>>* Zip(const Sequence<T1>& sequence1, const Sequence<T2>& sequence2) {
        Sequence<Pair<T1, T2>>* resultSequence = new MutableArraySequence<Pair<T1, T2>>();

        int length1 = sequence1.GetLength();
        int length2 = sequence2.GetLength();
        int minLength = length1;

        if (length2 < length1) {
            minLength = length2;
        }

        try {
            for (int i = 0; i < minLength; i++) {
                resultSequence->Append(Pair<T1, T2>(sequence1[i], sequence2[i]));
            }
            return resultSequence;
        } catch (...) {
            delete resultSequence;
            throw;
        }
    }

    // --- Расцепление пар (Unzip) ---
    template <typename T1, typename T2>
    static Pair<Sequence<T1>*, Sequence<T2>*> Unzip(const Sequence<Pair<T1, T2>>& pairsSequence) {
        Sequence<T1>* result1 = new MutableArraySequence<T1>();
        Sequence<T2>* result2 = new MutableArraySequence<T2>();

        try {
            for (int i = 0; i < pairsSequence.GetLength(); i++) {
                result1->Append(pairsSequence[i].first);
                result2->Append(pairsSequence[i].second);
            }
            return Pair<Sequence<T1>*, Sequence<T2>*>(result1, result2);
        } catch (...) {
            delete result1;
            delete result2;
            throw;
        }
    }
};

#endif // UTILS_HPP
