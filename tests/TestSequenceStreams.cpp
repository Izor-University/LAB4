// =========================================================
// tests/TestSequenceStreams.cpp
// =========================================================

#include <gtest/gtest.h>
#include "../core/Ordinal.hpp"
#include "../core/Exceptions.hpp"
#include "../core/MutableArraySequence.hpp"
#include "../streams/SequenceInputStream.hpp"
#include "../streams/SequenceOutputStream.hpp"

// ---------------------------------------------------------
// 1. Тесты Потока Ввода (SequenceInputStream)
// ---------------------------------------------------------
TEST(SequenceStreamsTest, InputStream_Navigation) {
    // Подготавливаем базовую последовательность
    MutableArraySequence<int> seq;
    seq.Append(10);
    seq.Append(20);
    seq.Append(30);

    SequenceInputStream<int> stream(&seq);

    // Открываем поток
    stream.Open();

    // Проверяем начальное состояние
    EXPECT_FALSE(stream.IsEndOfStream());
    EXPECT_EQ(stream.GetPosition(), Ordinal(0, 0));

    // Последовательное чтение
    EXPECT_EQ(stream.Input(), 10);
    EXPECT_EQ(stream.GetPosition(), Ordinal(0, 1));

    EXPECT_EQ(stream.Input(), 20);
    EXPECT_EQ(stream.GetPosition(), Ordinal(0, 2));

    // Навигация (Seek) - Возвращаемся в начало
    stream.Seek(Ordinal(0, 0));
    EXPECT_EQ(stream.Input(), 10); // Снова прочитали первый элемент

    // Перемещаемся к последнему элементу
    stream.Seek(Ordinal(0, 2));
    EXPECT_EQ(stream.Input(), 30);

    // Поток должен закончиться
    EXPECT_TRUE(stream.IsEndOfStream());

    // Отрицательный тест: Попытка чтения за концом файла (EOF)
    EXPECT_THROW(stream.Input(), IndexOutOfRange);

    stream.Close();
}

TEST(SequenceStreamsTest, InputStream_ClosedThrows) {
    MutableArraySequence<int> seq;
    seq.Append(100);

    SequenceInputStream<int> stream(&seq);

    // До Open() любые операции должны выбрасывать исключение безопасности
    EXPECT_THROW(stream.Input(), Exception);
    EXPECT_THROW(stream.Seek(Ordinal(0, 0)), Exception);

    // Открываем и сразу закрываем
    stream.Open();
    stream.Close();

    // После Close() операции снова недоступны
    EXPECT_THROW(stream.Input(), Exception);
    EXPECT_THROW(stream.Seek(Ordinal(0, 0)), Exception);
}

// ---------------------------------------------------------
// 2. Тесты Потока Вывода (SequenceOutputStream)
// ---------------------------------------------------------
TEST(SequenceStreamsTest, OutputStream_Mutation) {
    // Важно: передаем динамически выделенный объект, так как поток будет
    // удалять старые версии при мутации (благодаря нашему исправлению утечек в Шаге 1)
    Sequence<int>* seq = new MutableArraySequence<int>();

    SequenceOutputStream<int> stream(seq);

    stream.Open();

    // Проверяем начальную позицию
    EXPECT_EQ(stream.GetPosition(), Ordinal(0, 0));

    // Пишем данные
    Ordinal pos1 = stream.Output(777);
    EXPECT_EQ(pos1, Ordinal(0, 1)); // Позиция сдвинулась на 1

    Ordinal pos2 = stream.Output(888);
    EXPECT_EQ(pos2, Ordinal(0, 2)); // Позиция сдвинулась на 2

    stream.Close();

    // Извлекаем итоговую последовательность из потока
    Sequence<int>* finalSeq = stream.GetSequence();

    // Проверяем, что данные реально записались
    EXPECT_EQ(finalSeq->GetLength(), 2);
    EXPECT_EQ(finalSeq->Get(0), 777);
    EXPECT_EQ(finalSeq->Get(1), 888);

    // Освобождаем итоговую последовательность (предыдущие версии удалил сам поток)
    delete finalSeq;
}

TEST(SequenceStreamsTest, OutputStream_ClosedThrows) {
    Sequence<int>* seq = new MutableArraySequence<int>();
    SequenceOutputStream<int> stream(seq);

    // Запись в неоткрытый поток запрещена
    EXPECT_THROW(stream.Output(555), Exception);

    stream.Open();
    stream.Close();

    // Запись в закрытый поток запрещена
    EXPECT_THROW(stream.Output(999), Exception);

    // Очищаем начальную последовательность, так как успешных Output не было
    delete stream.GetSequence();
}