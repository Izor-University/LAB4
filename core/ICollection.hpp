#ifndef ICOLLECTION_HPP
#define ICOLLECTION_HPP

template <class T>
class ICollection {
public:
    virtual ~ICollection() {}

    // --- Декомпозиция ---
    virtual const T& Get(int index) const = 0;
    virtual int GetLength() const = 0;

    // --- Виртуальные конструкторы ---
    virtual ICollection<T>* CreateEmpty() const = 0;
    virtual ICollection<T>* Clone() const = 0;
};

#endif // ICOLLECTION_HPP
