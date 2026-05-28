// =========================================================
// sequences/LazySequence.hpp
// =========================================================
#ifndef LAZY_SEQUENCE_HPP
#define LAZY_SEQUENCE_HPP

#include "../core/Sequence.hpp"
#include "../core/MutableArraySequence.hpp"
#include "../core/DynamicArray.hpp"
#include "../core/Ordinal.hpp"
#include "../generators/IGenerator.hpp"
#include "../generators/Decorators.hpp"

template <class T>
class LazySequence : public Sequence<T> {
protected:
    mutable DynamicArray<MutableArraySequence<T>*>* caches;
    IGenerator<T>* generator;
    Ordinal virtualLength;

    void EnsureCacheLevel(int omegaCount) const;

public:
    class LazyEnumerator : public IEnumerator<T> {
    private:
        const LazySequence<T>* seq;
        Ordinal currentIndex;
        bool started;
    public:
        explicit LazyEnumerator(const LazySequence<T>* s) : seq(s), currentIndex(0, 0), started(false) {}
        virtual bool MoveNext() override;
        virtual const T& GetCurrent() const override;
        virtual void Reset() override;
    };

    LazySequence(IGenerator<T>* gen, const Ordinal& length);
    LazySequence(const LazySequence<T>& other);
    virtual ~LazySequence();

    virtual ISequenceBuilder<T>* CreateBuilder() const override;
    virtual Sequence<T>* CreateEmpty() const override;
    virtual Sequence<T>* Clone() const override;
    virtual IEnumerator<T>* GetEnumerator() const override;

    virtual const T& Get(int index) const override;
    virtual int GetLength() const override;
    virtual const T& operator[](int index) const override;

    virtual const T& GetByOrdinal(const Ordinal& index) const;
    virtual Ordinal GetOrdinalLength() const;

    virtual Sequence<T>* Append(const T& item) override;
    virtual Sequence<T>* Prepend(const T& item) override;
    virtual Sequence<T>* InsertAt(const T& item, int index) override;

    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    virtual Sequence<T>* GetSubsequence(const Ordinal& startIndex, const Ordinal& endIndex) const;

    virtual Sequence<T>* Concat(Sequence<T>* list) const override;
    virtual Sequence<T>* Slice(int index, int count, Sequence<T>* insertSeq = nullptr) override;
};

#include "LazySequence.tpp"
#endif // LAZY_SEQUENCE_HPP