#pragma once

template <typename T>
class IGenerator {
public:
    virtual ~IGenerator() {}
    
    // Получить следующий элемент
    virtual T GetNext() = 0;
    
    // Проверить, может ли генератор выдать следующий элемент
    virtual bool HasNext() const = 0;
    
    // Фабричный метод для глубокого копирования (для сырых указателей)
    virtual IGenerator<T>* Clone() const = 0;
};