#ifndef IENUMERATOR_HPP
#define IENUMERATOR_HPP

template <class T>
class IEnumerator {
public:
    virtual ~IEnumerator() {}

    // --- Перемещение ---
    virtual bool MoveNext() = 0;

    // --- Декомпозиция ---
    virtual const T& GetCurrent() const = 0;

    // --- Управление ---
    virtual void Reset() = 0;
};

#endif // IENUMERATOR_HPP