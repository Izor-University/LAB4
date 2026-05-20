#pragma once
#include "ISequence.hpp"
#include "Cardinal.hpp"
#include "IGenerator.hpp"

template <typename T>
class LazySequence : public ISequence<T> {
public:
    // Конструкторы и деструктор
    LazySequence();
    
    // Конструктор, принимающий генератор и начальный размер (конечный или бесконечный)
    LazySequence(IGenerator<T>* generator, Cardinal length);
    
    // Конструкторы на основе готовых данных
    LazySequence(T* items, int count);
    LazySequence(ISequence<T>* seq);

    // Управление памятью (Правило трех)
    LazySequence(const LazySequence<T>& other);
    LazySequence<T>& operator=(const LazySequence<T>& other);
    virtual ~LazySequence();

    // Реализация методов интерфейса ISequence
    T GetFirst() override;
    T GetLast() override;
    T Get(int index) override;

    // Специфичные ленивые методы
    Cardinal GetLength() const;
    size_t GetMaterializedCount() const;

private:
    T* cache_;                  // Динамический массив для кэширования
    size_t cache_capacity_;     // Текущая физическая емкость массива
    size_t materialized_count_; // Количество реально вычисленных элементов
    
    IGenerator<T>* generator_;  // Указатель на генератор (LazySequence им владеет)
    Cardinal total_length_;     // Размер последовательности

    // Вспомогательные приватные методы
    void Cleanup();
    void CopyFrom(const LazySequence<T>& other);
    void EnsureCapacity(size_t capacity);
    void MaterializeTo(size_t index);
};

#include "LazySequence.ipp"