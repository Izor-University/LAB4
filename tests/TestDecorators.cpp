// =========================================================
// tests/TestDecorators.cpp
// =========================================================

#include <gtest/gtest.h>
#include "../core/Ordinal.hpp"
#include "../core/Exceptions.hpp"
#include "../generators/FunctionGenerator.hpp"
#include "../generators/Decorators.hpp"

// ---------------------------------------------------------
// Вспомогательные функции для тестов
// ---------------------------------------------------------
namespace {
    int IdentityRule(const Ordinal& index) { return index.GetOffset(); }
    int MultiplyByTen(const int& val) { return val * 10; }
    bool IsEven(const int& val) { return val % 2 == 0; }
}
// ---------------------------------------------------------
// 1. Тесты Append и Prepend (Вставка по краям)
// ---------------------------------------------------------
TEST(DecoratorsTest, AppendPrepend_Routing) {
    FunctionGenerator<int> baseGen(IdentityRule); // 0, 1, 2, 3...

    // Prepend (Вставка в начало)
    PrependGenerator<int> prepGen(&baseGen, 999);
    EXPECT_EQ(prepGen.Generate(Ordinal(0, 0)), 999); // Перехват
    EXPECT_EQ(prepGen.Generate(Ordinal(0, 1)), 0);   // Сдвиг (base[0])
    EXPECT_EQ(prepGen.Generate(Ordinal(0, 5)), 4);   // Сдвиг (base[4])

    // Append (Вставка в конец)
    Ordinal baseLen(0, 3); // Длина базы 3 (индексы 0, 1, 2)
    AppendGenerator<int> appGen(&baseGen, 888, baseLen);

    EXPECT_EQ(appGen.Generate(Ordinal(0, 0)), 0);   // Пропуск (base[0])
    EXPECT_EQ(appGen.Generate(Ordinal(0, 2)), 2);   // Пропуск (base[2])
    EXPECT_EQ(appGen.Generate(Ordinal(0, 3)), 888); // Перехват
}

// Отрицательный тест: Запрос за границами Append
TEST(DecoratorsTest, Decorators_OutOfBoundsThrows) {
    FunctionGenerator<int> baseGen(IdentityRule);
    Ordinal baseLen(0, 3);
    AppendGenerator<int> appGen(&baseGen, 888, baseLen);

    // Длина стала 4 (индексы 0, 1, 2, 3). Индекс 4 должен вызвать исключение.
    EXPECT_THROW(appGen.Generate(Ordinal(0, 4)), IndexOutOfRange);
    EXPECT_THROW(appGen.Generate(Ordinal::Omega()), IndexOutOfRange);
}

// ---------------------------------------------------------
// 2. Тест InsertAt (Вставка в середину со сдвигом Кантора)
// ---------------------------------------------------------
TEST(DecoratorsTest, InsertAt_Shift) {
    FunctionGenerator<int> baseGen(IdentityRule); // 0, 1, 2, 3...

    // Вставляем 777 на позицию 2
    Ordinal insertPos(0, 2);
    InsertAtGenerator<int> insGen(&baseGen, 777, insertPos);

    EXPECT_EQ(insGen.Generate(Ordinal(0, 0)), 0);   // До: base[0]
    EXPECT_EQ(insGen.Generate(Ordinal(0, 1)), 1);   // До: base[1]
    EXPECT_EQ(insGen.Generate(Ordinal(0, 2)), 777); // Точка вставки
    EXPECT_EQ(insGen.Generate(Ordinal(0, 3)), 2);   // После: сдвиг -1 (base[2])
    EXPECT_EQ(insGen.Generate(Ordinal(0, 4)), 3);   // После: сдвиг -1 (base[3])
}

// ---------------------------------------------------------
// 3. Тест Concat (Сцепление и вычитание Ординалов)
// ---------------------------------------------------------
TEST(DecoratorsTest, Concat_CantorSubtraction) {
    FunctionGenerator<int> baseGen(IdentityRule); // 0, 1, 2, 3...

    // Будем использовать один и тот же генератор для левой и правой части,
    // но смещение индексов должно работать правильно.
    Ordinal leftLen(0, 3); // Индексы левой части: 0, 1, 2
    ConcatGenerator<int> concatGen(&baseGen, &baseGen, leftLen);

    // Первые 3 элемента должны идти из левого генератора
    EXPECT_EQ(concatGen.Generate(Ordinal(0, 0)), 0);
    EXPECT_EQ(concatGen.Generate(Ordinal(0, 2)), 2);

    // Индекс 3 - это 0-й элемент правого генератора ((3) - (3) = 0)
    EXPECT_EQ(concatGen.Generate(Ordinal(0, 3)), 0);
    EXPECT_EQ(concatGen.Generate(Ordinal(0, 5)), 2);
}

// ---------------------------------------------------------
// 4. Тесты Функциональных декораторов (Map, Where)
// ---------------------------------------------------------
TEST(DecoratorsTest, Map_Application) {
    FunctionGenerator<int> baseGen(IdentityRule); // 0, 1, 2, 3...

    // Применяем x * 10
    MapGenerator<int> mapGen(&baseGen, MultiplyByTen);

    EXPECT_EQ(mapGen.Generate(Ordinal(0, 0)), 0);
    EXPECT_EQ(mapGen.Generate(Ordinal(0, 3)), 30);
    EXPECT_EQ(mapGen.Generate(Ordinal(0, 10)), 100);
}

TEST(DecoratorsTest, Where_FilteringAndBounds) {
    FunctionGenerator<int> baseGen(IdentityRule); // 0, 1, 2, 3, 4, 5...
    Ordinal sourceLen(0, 6); // Длина базы 6 (элементы: 0, 1, 2, 3, 4, 5)

    // Оставляем только четные: 0, 2, 4 (всего 3 элемента, индексы 0, 1, 2)
    WhereGenerator<int> whereGen(&baseGen, IsEven, sourceLen);

    // Запрашиваем 0-й подходящий (это 0)
    EXPECT_EQ(whereGen.Generate(Ordinal(0, 0)), 0);
    // Запрашиваем 1-й подходящий (это 2)
    EXPECT_EQ(whereGen.Generate(Ordinal(0, 1)), 2);
    // Запрашиваем 2-й подходящий (это 4)
    EXPECT_EQ(whereGen.Generate(Ordinal(0, 2)), 4);

    // Отрицательный тест: Запрашиваем 3-й подходящий (его нет, предел достигнут)
    EXPECT_THROW(whereGen.Generate(Ordinal(0, 3)), IndexOutOfRange);
}

// ---------------------------------------------------------
// 5. Тест Subsequence (Срез)
// ---------------------------------------------------------
TEST(DecoratorsTest, Subsequence_Offset) {
    FunctionGenerator<int> baseGen(IdentityRule); // 0, 1, 2, 3, 4, 5...

    // Срез начиная с индекса 4
    Ordinal start(0, 4);
    SubsequenceGenerator<int> subGen(&baseGen, start);

    EXPECT_EQ(subGen.Generate(Ordinal(0, 0)), 4); // base[4 + 0]
    EXPECT_EQ(subGen.Generate(Ordinal(0, 5)), 9); // base[4 + 5]
}