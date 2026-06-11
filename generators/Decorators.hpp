#ifndef DECORATORS_HPP
#define DECORATORS_HPP

#include "IGenerator.hpp"
#include "../core/Exceptions.hpp"

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

    virtual Option<T> Generate(const Ordinal& index) const override {
        if (index < baseLength) return baseGen->Generate(index);
        if (index == baseLength) return Option<T>(item);
        return Option<T>(); // None
    }
    virtual IGenerator<T>* Clone() const override { return new AppendGenerator<T>(baseGen, item, baseLength); }
};

template <class T>
class PrependGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    T item;
public:
    PrependGenerator(const IGenerator<T>* base, const T& val) : item(val) { baseGen = base->Clone(); }
    virtual ~PrependGenerator() { delete baseGen; }

    virtual Option<T> Generate(const Ordinal& index) const override {
        if (index == Ordinal(0, 0)) return Option<T>(item);
        return baseGen->Generate(index - Ordinal(0, 1));
    }
    virtual IGenerator<T>* Clone() const override { return new PrependGenerator<T>(baseGen, item); }
};

template <class T>
class SubsequenceGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    Ordinal startOrdinal;
public:
    SubsequenceGenerator(const IGenerator<T>* base, const Ordinal& start) : startOrdinal(start) { baseGen = base->Clone(); }
    virtual ~SubsequenceGenerator() { delete baseGen; }

    virtual Option<T> Generate(const Ordinal& index) const override {
        return baseGen->Generate(startOrdinal + index);
    }
    virtual IGenerator<T>* Clone() const override { return new SubsequenceGenerator<T>(baseGen, startOrdinal); }
};

template <class T>
class InsertAtGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    T item;
    Ordinal insertIndex;
public:
    InsertAtGenerator(const IGenerator<T>* base, const T& val, const Ordinal& idx) : item(val), insertIndex(idx) { baseGen = base->Clone(); }
    virtual ~InsertAtGenerator() { delete baseGen; }

    virtual Option<T> Generate(const Ordinal& index) const override {
        if (index < insertIndex) return baseGen->Generate(index);
        if (index == insertIndex) return Option<T>(item);
        return baseGen->Generate(index - Ordinal(0, 1));
    }
    virtual IGenerator<T>* Clone() const override { return new InsertAtGenerator<T>(baseGen, item, insertIndex); }
};

template <class T>
class ConcatGenerator : public IGenerator<T> {
private:
    IGenerator<T>* leftGen;
    IGenerator<T>* rightGen;
    Ordinal leftLength;
public:
    ConcatGenerator(const IGenerator<T>* left, const IGenerator<T>* right, const Ordinal& leftLen) : leftLength(leftLen) {
        leftGen = left->Clone(); rightGen = right->Clone();
    }
    virtual ~ConcatGenerator() { delete leftGen; delete rightGen; }

    virtual Option<T> Generate(const Ordinal& index) const override {
        if (index < leftLength) return leftGen->Generate(index);
        return rightGen->Generate(index - leftLength);
    }
    virtual IGenerator<T>* Clone() const override { return new ConcatGenerator<T>(leftGen, rightGen, leftLength); }
};

template <class T>
class MapGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    T (*mapper)(const T&);
public:
    MapGenerator(const IGenerator<T>* base, T (*mapFunc)(const T&)) : mapper(mapFunc) { baseGen = base->Clone(); }
    virtual ~MapGenerator() { delete baseGen; }

    virtual Option<T> Generate(const Ordinal& index) const override {
        Option<T> opt = baseGen->Generate(index);
        if (opt.IsNone()) return Option<T>();
        return Option<T>(mapper(opt.GetValue()));
    }
    virtual IGenerator<T>* Clone() const override { return new MapGenerator<T>(baseGen, mapper); }
};

template <class T>
class WhereGenerator : public IGenerator<T> {
private:
    IGenerator<T>* baseGen;
    bool (*predicate)(const T&);
    Ordinal sourceLength;
public:
    WhereGenerator(const IGenerator<T>* base, bool (*pred)(const T&), const Ordinal& len) : predicate(pred), sourceLength(len) { baseGen = base->Clone(); }
    virtual ~WhereGenerator() { delete baseGen; }

    virtual Option<T> Generate(const Ordinal& index) const override {
        Ordinal curr(0, 0);
        Ordinal matchCount(0, 0);

        while (curr < sourceLength) {
            Option<T> optVal = baseGen->Generate(curr);

            // Если источник иссяк, мы просто возвращаем None
            if (optVal.IsNone()) return Option<T>();

            if (predicate(optVal.GetValue())) {
                if (matchCount == index) return optVal;
                matchCount = matchCount + Ordinal(0, 1);
            }
            curr = curr + Ordinal(0, 1);
        }
        return Option<T>();
    }
    virtual IGenerator<T>* Clone() const override { return new WhereGenerator<T>(baseGen, predicate, sourceLength); }
};


template <class T>
class InterleaveThreeGenerator : public IGenerator<T> {
private:
    IGenerator<T>* genA;
    IGenerator<T>* genB;
    IGenerator<T>* genC;

public:
    InterleaveThreeGenerator(const IGenerator<T>* a, const IGenerator<T>* b, const IGenerator<T>* c) {
        genA = a->Clone();
        genB = b->Clone();
        genC = c->Clone();
    }

    virtual ~InterleaveThreeGenerator() {
        delete genA;
        delete genB;
        delete genC;
    }

    virtual Option<T> Generate(const Ordinal& index) const override {
        int k = index.GetOmegaCount();
        int n = index.GetOffset();

        int source = n % 3;             // 0 -> A, 1 -> B, 2 -> C
        int realIndex = n / 3;

        Ordinal targetOrdinal(k, realIndex);

        if (source == 0) return genA->Generate(targetOrdinal);
        if (source == 1) return genB->Generate(targetOrdinal);
        return genC->Generate(targetOrdinal);
    }

    virtual IGenerator<T>* Clone() const override {
        return new InterleaveThreeGenerator<T>(genA, genB, genC);
    }
};

#endif // DECORATORS_HPP