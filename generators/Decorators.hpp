#ifndef DECORATORS_HPP
#define DECORATORS_HPP

#include "IGenerator.hpp"
#include "../core/Exceptions.hpp"

// --- Декоратор добавления в конец ---
template <class T>
class AppendGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    T item;
    int baseLength;
public:
    AppendGenerator(const IGenerator<T>* base, const T& val, int len)
        : item(val), baseLength(len) { baseGen = base->Clone(); }
    virtual ~AppendGenerator() { delete baseGen; }
    virtual T Generate(int index) const override {
        if (index < baseLength) return baseGen->Generate(index);
        if (index == baseLength) return item;
        throw IndexOutOfRange("AppendGenerator out of bounds");
    }
    virtual IGenerator<T>* Clone() const override {
        return new AppendGenerator<T>(baseGen, item, baseLength);
    }
};

// --- Декоратор добавления в начало ---
template <class T>
class PrependGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    T item;
public:
    PrependGenerator(const IGenerator<T>* base, const T& val)
        : item(val) { baseGen = base->Clone(); }
    virtual ~PrependGenerator() { delete baseGen; }
    virtual T Generate(int index) const override {
        if (index == 0) return item;
        return baseGen->Generate(index - 1); // Смещаем индекс для базы
    }
    virtual IGenerator<T>* Clone() const override {
        return new PrependGenerator<T>(baseGen, item);
    }
};

// --- Декоратор конкатенации ---
template <class T>
class ConcatGenerator : public IGenerator<T> {
private:
    IGenerator<T>* leftGen;
    IGenerator<T>* rightGen;
    int leftLength;
public:
    ConcatGenerator(const IGenerator<T>* left, int leftLen, const IGenerator<T>* right)
        : leftLength(leftLen) {
        leftGen = left->Clone();
        rightGen = right->Clone();
    }
    virtual ~ConcatGenerator() { delete leftGen; delete rightGen; }
    virtual T Generate(int index) const override {
        if (index < leftLength) return leftGen->Generate(index);
        return rightGen->Generate(index - leftLength);
    }
    virtual IGenerator<T>* Clone() const override {
        return new ConcatGenerator<T>(leftGen, leftLength, rightGen);
    }
};

// --- Декоратор подпоследовательности (Ленивый срез) ---
template <class T>
class SubsequenceGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    Ordinal startOrdinal;

public:
    SubsequenceGenerator(const IGenerator<T>* base, const Ordinal& start)
        : startOrdinal(start) {
        baseGen = base->Clone();
    }

    virtual ~SubsequenceGenerator() {
        delete baseGen;
    }

    virtual T Generate(const Ordinal& index) const override {
        // Мы просто сдвигаем запрос на startOrdinal вперед
        // Благодаря правильной арифметике Кантора в Ordinal,
        // это работает даже за пределами бесконечности!
        return baseGen->Generate(startOrdinal + index);
    }

    virtual IGenerator<T>* Clone() const override {
        return new SubsequenceGenerator<T>(baseGen, startOrdinal);
    }
};

#endif // DECORATORS_HPP