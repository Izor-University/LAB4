#pragma once

template <typename T>
class ISequence {
public:
    virtual ~ISequence() {}

    // Получение элементов по позиции
    virtual T GetFirst() = 0;
    virtual T GetLast() = 0;
    virtual T Get(int index) = 0;
};