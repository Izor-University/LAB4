// =========================================================
// tests/TestLazyStreams.cpp
// =========================================================

#include <gtest/gtest.h>
#include "../core/Ordinal.hpp"
#include "../core/Exceptions.hpp"
#include "../generators/FunctionGenerator.hpp"
#include "../sequences/LazySequence.hpp"
#include "../streams/LazyInputStream.hpp"
#include "../streams/LazyOutputStream.hpp"

namespace {
    int IdentityRule(const Ordinal& index) {
        return index.GetOffset();
    }
}

TEST(LazyStreamsTest, InputStream_InfinityJump) {
    // ИСПРАВЛЕНИЕ: передаем new FunctionGenerator
    LazySequence<int>* infSeq = new LazySequence<int>(new FunctionGenerator<int>(IdentityRule), Ordinal::Omega());

    Sequence<int>* transfiniteSeqBase = infSeq->Append(999);
    LazySequence<int>* transfiniteSeq = static_cast<LazySequence<int>*>(transfiniteSeqBase);

    // Удаляем матрешку, деструктор теперь сделает безопасный delete для генератора в куче
    delete infSeq;

    LazyInputStream<int> stream(transfiniteSeq);
    stream.Open();

    EXPECT_EQ(stream.Input(), 0);
    EXPECT_EQ(stream.Input(), 1);

    stream.Seek(Ordinal::Omega());

    EXPECT_EQ(stream.Input(), 999);
    EXPECT_TRUE(stream.IsEndOfStream());
    EXPECT_THROW(stream.Input(), IndexOutOfRange);

    stream.Close();
    delete transfiniteSeq;
}

TEST(LazyStreamsTest, OutputStream_InfinityAppend) {
    // ИСПРАВЛЕНИЕ: передаем new FunctionGenerator
    LazySequence<int>* seq = new LazySequence<int>(new FunctionGenerator<int>(IdentityRule), Ordinal::Omega());

    LazyOutputStream<int> stream(seq);
    stream.Open();

    EXPECT_EQ(stream.GetPosition(), Ordinal::Omega());

    Ordinal pos1 = stream.Output(777);
    EXPECT_EQ(pos1, Ordinal(1, 1));

    Ordinal pos2 = stream.Output(888);
    EXPECT_EQ(pos2, Ordinal(1, 2));

    stream.Close();

    LazySequence<int>* finalSeq = stream.GetSequence();

    EXPECT_EQ(finalSeq->GetOrdinalLength(), Ordinal(1, 2));
    EXPECT_EQ(finalSeq->GetByOrdinal(Ordinal(1, 0)), 777);
    EXPECT_EQ(finalSeq->GetByOrdinal(Ordinal(1, 1)), 888);

    delete finalSeq;
}

TEST(LazyStreamsTest, Streams_ClosedSafety) {
    // ИСПРАВЛЕНИЕ: передаем new FunctionGenerator
    LazySequence<int>* seq = new LazySequence<int>(new FunctionGenerator<int>(IdentityRule), Ordinal(0, 5));

    LazyOutputStream<int> outStream(seq);
    LazyInputStream<int> inStream(seq);

    EXPECT_THROW(outStream.Output(100), Exception);
    EXPECT_THROW(inStream.Input(), Exception);
    EXPECT_THROW(inStream.Seek(Ordinal(0, 2)), Exception);

    outStream.Open();
    outStream.Close();

    inStream.Open();
    inStream.Close();

    EXPECT_THROW(outStream.Output(100), Exception);
    EXPECT_THROW(inStream.Input(), Exception);

    delete outStream.GetSequence();
}