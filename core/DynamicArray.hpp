#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include "Exceptions.hpp"
#include "IEnumerable.hpp"

template <class T>
class DynamicArray : public IEnumerable<T> {
private:
    T* data;
    int size;
    int capacity;

public:
    // --- Внутренний итератор ---
    class Enumerator : public IEnumerator<T> {
    private:
        const DynamicArray<T>* array;
        int currentIndex;

    public:
        explicit Enumerator(const DynamicArray<T>* arr) : array(arr), currentIndex(-1) {}

        bool MoveNext() override {
            if (currentIndex + 1 < array->GetSize()) {
                currentIndex++;
                return true;
            }
            return false;
        }

        const T& GetCurrent() const override {
            if (currentIndex < 0 || currentIndex >= array->GetSize()) {
                throw IndexOutOfRange("Error: Iterator out of bounds!");
            }
            return array->data[currentIndex];
        }

        void Reset() override {
            currentIndex = -1;
        }
    };

    // --- Конструкторы ---
    DynamicArray();
    explicit DynamicArray(int size);
    DynamicArray(T* items, int count);
    DynamicArray(const DynamicArray<T>& other);
    DynamicArray(DynamicArray<T>&& other) noexcept;
    ~DynamicArray();

    // --- Фабрика итератора ---
    IEnumerator<T>* GetEnumerator() const override;

    // --- Операторы присваивания ---
    DynamicArray<T>& operator=(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(DynamicArray<T>&& other) noexcept;

    // --- Декомпозиция ---
    const T& Get(int index) const;
    int GetSize() const;
    int GetCapacity() const;

    // --- Операции ---
    void Set(int index, const T& value);
    void Resize(int newSize);
    void RemoveAt(int index);
};

#include "DynamicArray.tpp"
#endif // DYNAMIC_ARRAY_HPP
