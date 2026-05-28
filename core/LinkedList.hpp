#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "Exceptions.hpp"
#include "IEnumerable.hpp"

template <class T>
class LinkedList : public IEnumerable<T> {
private:
    // --- Внутренняя структура узла ---
    struct Node {
        T data;
        Node* next;
        Node* prev;

        explicit Node(const T& data) : data(data), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;

    // --- Вспомогательные методы ---
    void Clear();

public:
    // --- Быстрый внутренний итератор ---
    class Enumerator : public IEnumerator<T> {
    private:
        const Node* head;
        const Node* current;
        bool started;

    public:
        explicit Enumerator(const Node* headNode) : head(headNode), current(nullptr), started(false) {}

        bool MoveNext() override {
            if (!started) {
                current = head;
                started = true;
            } else if (current != nullptr) {
                current = current->next;
            }
            return current != nullptr;
        }

        const T& GetCurrent() const override {
            if (current == nullptr) {
                throw IndexOutOfRange("Error: Iterator out of bounds!");
            }
            return current->data;
        }

        void Reset() override {
            current = nullptr;
            started = false;
        }
    };

    // --- Конструкторы ---
    LinkedList();
    LinkedList(T* items, int count);
    LinkedList(const LinkedList<T>& other);
    LinkedList(LinkedList<T>&& other) noexcept;

    // --- Деструктор ---
    ~LinkedList();

    // --- Фабрика итератора ---
    IEnumerator<T>* GetEnumerator() const override;

    // --- Операторы присваивания ---
    LinkedList<T>& operator=(const LinkedList<T>& other);
    LinkedList<T>& operator=(LinkedList<T>&& other) noexcept;

    // --- Декомпозиция ---
    const T& GetFirst() const;
    const T& GetLast() const;
    const T& Get(int index) const;
    int GetLength() const;

    LinkedList<T>* GetSubList(int startIndex, int endIndex) const;

    // --- Операции ---
    void Append(const T& item);
    void Prepend(const T& item);
    void InsertAt(const T& item, int index);
    void RemoveAt(int index);

    LinkedList<T>* Concat(LinkedList<T>* list) const;
};

#include "LinkedList.tpp"
#endif // LINKED_LIST_HPP
