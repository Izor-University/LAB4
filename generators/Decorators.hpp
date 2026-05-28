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
    Ordinal baseLength;
public:
    AppendGenerator(const IGenerator<T>* base, const T& val, const Ordinal& len)
        : item(val), baseLength(len) { baseGen = base->Clone(); }
    virtual ~AppendGenerator() { delete baseGen; }

    virtual T Generate(const Ordinal& index) const override {
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

    virtual T Generate(const Ordinal& index) const override {
        if (index == Ordinal(0, 0)) return item;
        // Смещаем индекс на -1 для предка
        return baseGen->Generate(index - Ordinal(0, 1));
    }
    virtual IGenerator<T>* Clone() const override {
        return new PrependGenerator<T>(baseGen, item);
    }
};

// --- Декоратор подпоследовательности ---
template <class T>
class SubsequenceGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    Ordinal startOrdinal;
public:
    SubsequenceGenerator(const IGenerator<T>* base, const Ordinal& start)
        : startOrdinal(start) { baseGen = base->Clone(); }
    virtual ~SubsequenceGenerator() { delete baseGen; }

    virtual T Generate(const Ordinal& index) const override {
        return baseGen->Generate(startOrdinal + index);
    }
    virtual IGenerator<T>* Clone() const override {
        return new SubsequenceGenerator<T>(baseGen, startOrdinal);
    }
};

// --- Декоратор вставки в заданную позицию (ШАГ 2) ---
template <class T>
class InsertAtGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    T item;
    Ordinal insertIndex;

public:
    InsertAtGenerator(const IGenerator<T>* base, const T& val, const Ordinal& idx)
        : item(val), insertIndex(idx) {
        baseGen = base->Clone();
    }

    virtual ~InsertAtGenerator() { delete baseGen; }

    virtual T Generate(const Ordinal& index) const override {
        if (index < insertIndex) return baseGen->Generate(index);
        if (index == insertIndex) return item;
        return baseGen->Generate(index - Ordinal(0, 1));
    }

    virtual IGenerator<T>* Clone() const override {
        return new InsertAtGenerator<T>(baseGen, item, insertIndex);
    }
};

// --- Декоратор Конкатенации (ШАГ 2) ---
template <class T>
class ConcatGenerator : public IGenerator<T> {
private:
    IGenerator<T>* leftGen;
    IGenerator<T>* rightGen;
    Ordinal leftLength;

public:
    ConcatGenerator(const IGenerator<T>* left, const IGenerator<T>* right, const Ordinal& leftLen)
        : leftLength(leftLen) {
        leftGen = left->Clone();
        rightGen = right->Clone();
    }

    virtual ~ConcatGenerator() {
        delete leftGen;
        delete rightGen;
    }

    virtual T Generate(const Ordinal& index) const override {
        if (index < leftLength) {
            return leftGen->Generate(index);
        } else {
            return rightGen->Generate(index - leftLength);
        }
    }

    virtual IGenerator<T>* Clone() const override {
        return new ConcatGenerator<T>(leftGen, rightGen, leftLength);
    }
};

// --- Декоратор Map (Отображение) (ШАГ 3) ---
template <class T>
class MapGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    T (*mapper)(const T&);

public:
    MapGenerator(const IGenerator<T>* base, T (*mapFunc)(const T&))
        : mapper(mapFunc) {
        baseGen = base->Clone();
    }

    virtual ~MapGenerator() { delete baseGen; }

    virtual T Generate(const Ordinal& index) const override {
        // Запрашиваем элемент у предка и сразу применяем к нему функцию
        return mapper(baseGen->Generate(index));
    }

    virtual IGenerator<T>* Clone() const override {
        return new MapGenerator<T>(baseGen, mapper);
    }
};

// --- Декоратор Where (Фильтрация) (ШАГ 3) ---
template <class T>
class WhereGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    bool (*predicate)(const T&);
    Ordinal sourceLength;

public:
    WhereGenerator(const IGenerator<T>* base, bool (*pred)(const T&), const Ordinal& len)
        : predicate(pred), sourceLength(len) {
        baseGen = base->Clone();
    }

    virtual ~WhereGenerator() { delete baseGen; }

    virtual T Generate(const Ordinal& index) const override {
        Ordinal curr(0, 0);
        Ordinal matchCount(0, 0);

        // Идем по базовой последовательности и считаем элементы, проходящие фильтр
        while (curr < sourceLength) {
            T val = baseGen->Generate(curr);
            if (predicate(val)) {
                if (matchCount == index) {
                    return val;
                }
                matchCount = matchCount + Ordinal(0, 1);
            }
            curr = curr + Ordinal(0, 1);
        }
        throw IndexOutOfRange("WhereGenerator: Element not found or index out of bounds");
    }

    virtual IGenerator<T>* Clone() const override {
        return new WhereGenerator<T>(baseGen, predicate, sourceLength);
    }
};

#endif // DECORATORS_HPP