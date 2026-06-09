// =========================================================
// tests/TestLazySequence.cpp
// =========================================================

#include <gtest/gtest.h>
#include "../core/Ordinal.hpp"
#include "../core/Exceptions.hpp"
#include "../generators/FunctionGenerator.hpp"
#include "../sequences/LazySequence.hpp"

namespace {
    int IdentityRule(const Ordinal& index) { return index.GetOffset(); }
    int MultiplyByTwo(const int& val) { return val * 2; }
    bool IsEvenVal(const int& val) { return val % 2 == 0; }
    int SumReducer(const int& acc, const int& val) { return acc + val; }
}

class CallCountingGenerator : public IGenerator<int> {
private:
    int* callCount;
public:
    explicit CallCountingGenerator(int* counter) : callCount(counter) {}

    // ИСПРАВЛЕНИЕ: Возвращаем Option<int>
    virtual Option<int> Generate(const Ordinal& index) const override {
        (*callCount)++;
        return Option<int>(index.GetOffset() * 10);
    }

    virtual IGenerator<int>* Clone() const override {
        return new CallCountingGenerator(callCount);
    }
};

TEST(LazySequenceTest, Creation_EmptyAndInfinite) {
    LazySequence<int> emptySeq(new FunctionGenerator<int>(IdentityRule), Ordinal(0, 0));
    EXPECT_THROW(emptySeq.GetFirst(), EmptyCollectionError);
    EXPECT_THROW(emptySeq.GetLast(), EmptyCollectionError);
    EXPECT_THROW(emptySeq.Get(0), IndexOutOfRange);

    LazySequence<int> infSeq(new FunctionGenerator<int>(IdentityRule), Ordinal::Omega());
    EXPECT_EQ(infSeq.GetFirst(), 0);
    EXPECT_THROW(infSeq.GetLast(), Exception);
}

TEST(LazySequenceTest, Memoization_Efficiency) {
    int count = 0;
    LazySequence<int> seq(new CallCountingGenerator(&count), Ordinal(0, 10));

    EXPECT_EQ(count, 0);
    EXPECT_EQ(seq.Get(5), 50);
    EXPECT_EQ(count, 6);
    EXPECT_EQ(seq.Get(5), 50);
    EXPECT_EQ(count, 6); // Кэш сработал!
    EXPECT_EQ(seq.Get(3), 30);
    EXPECT_EQ(count, 6);
    EXPECT_EQ(seq.Get(7), 70);
    EXPECT_EQ(count, 8);
}

TEST(LazySequenceTest, Memory_RuleOfThree) {
    LazySequence<int> seq1(new FunctionGenerator<int>(IdentityRule), Ordinal(0, 5));
    seq1.Get(2);

    LazySequence<int> seq2(seq1);
    EXPECT_EQ(seq2.Get(2), 2);

    LazySequence<int> seq3(new FunctionGenerator<int>(IdentityRule), Ordinal(0, 1));
    seq3 = seq1;
    EXPECT_EQ(seq3.Get(2), 2);
}

TEST(LazySequenceTest, Transfinite_Algebra) {
    LazySequence<int> infSeq(new FunctionGenerator<int>(IdentityRule), Ordinal::Omega());

    Sequence<int>* appBase = infSeq.Append(999);
    LazySequence<int>* appSeq = static_cast<LazySequence<int>*>(appBase);

    EXPECT_EQ(appSeq->GetOrdinalLength(), Ordinal(1, 1));
    EXPECT_EQ(appSeq->GetByOrdinal(Ordinal(0, 5)), 5);
    EXPECT_EQ(appSeq->GetByOrdinal(Ordinal(1, 0)), 999);

    Sequence<int>* insBase = infSeq.InsertAt(888, 3);
    LazySequence<int>* insSeq = static_cast<LazySequence<int>*>(insBase);

    EXPECT_EQ(insSeq->GetOrdinalLength(), Ordinal::Omega());
    EXPECT_EQ(insSeq->Get(2), 2);
    EXPECT_EQ(insSeq->Get(3), 888);
    EXPECT_EQ(insSeq->Get(4), 3);

    LazySequence<int> finSeq(new FunctionGenerator<int>(IdentityRule), Ordinal(0, 5));
    Sequence<int>* concatBase = finSeq.Concat(&infSeq);
    LazySequence<int>* concatSeq = static_cast<LazySequence<int>*>(concatBase);

    EXPECT_EQ(concatSeq->GetOrdinalLength(), Ordinal::Omega());

    delete appBase;
    delete insBase;
    delete concatBase;
}

TEST(LazySequenceTest, Functional_MapWhere) {
    LazySequence<int> seq(new FunctionGenerator<int>(IdentityRule), Ordinal(0, 5));

    Sequence<int>* mapped = seq.Map(MultiplyByTwo);
    EXPECT_EQ(mapped->Get(3), 6);
    delete mapped;

    Sequence<int>* filtered = seq.Where(IsEvenVal);
    EXPECT_EQ(filtered->Get(0), 0);
    EXPECT_EQ(filtered->Get(1), 2);
    EXPECT_EQ(filtered->Get(2), 4);

    // Внутри Get() вызывается TryGet(). Он получит None и выбросит IndexOutOfRange.
    // Это именно то поведение, которое должен видеть конечный пользователь!
    EXPECT_THROW(filtered->Get(3), IndexOutOfRange);
    delete filtered;
}

TEST(LazySequenceTest, Functional_ReduceFinite) {
    LazySequence<int> seq(new FunctionGenerator<int>(IdentityRule), Ordinal(0, 4));
    int sum = seq.Reduce(SumReducer, 0);
    EXPECT_EQ(sum, 6);
}

TEST(LazySequenceTest, Functional_ReduceInfiniteThrows) {
    LazySequence<int> infSeq(new FunctionGenerator<int>(IdentityRule), Ordinal::Omega());
    EXPECT_THROW(infSeq.Reduce(SumReducer, 0), Exception);
}

TEST(LazySequenceTest, Iterator_Traversal) {
    LazySequence<int> seq(new FunctionGenerator<int>(IdentityRule), Ordinal(0, 3));
    IEnumerator<int>* it = seq.GetEnumerator();

    EXPECT_TRUE(it->MoveNext());
    EXPECT_EQ(it->GetCurrent(), 0);
    EXPECT_TRUE(it->MoveNext());
    EXPECT_EQ(it->GetCurrent(), 1);
    EXPECT_TRUE(it->MoveNext());
    EXPECT_EQ(it->GetCurrent(), 2);
    EXPECT_FALSE(it->MoveNext());

    it->Reset();
    EXPECT_TRUE(it->MoveNext());
    EXPECT_EQ(it->GetCurrent(), 0);

    delete it;
}