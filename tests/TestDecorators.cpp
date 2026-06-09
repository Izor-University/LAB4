// =========================================================
// tests/TestDecorators.cpp
// =========================================================

#include <gtest/gtest.h>
#include "../core/Ordinal.hpp"
#include "../core/Exceptions.hpp"
#include "../generators/FunctionGenerator.hpp"
#include "../generators/Decorators.hpp"

namespace {
    int IdentityRule(const Ordinal& index) { return index.GetOffset(); }
    int MultiplyByTen(const int& val) { return val * 10; }
    bool IsEven(const int& val) { return val % 2 == 0; }
}

TEST(DecoratorsTest, AppendPrepend_Routing) {
    FunctionGenerator<int> baseGen(IdentityRule);

    PrependGenerator<int> prepGen(&baseGen, 999);
    EXPECT_EQ(prepGen.Generate(Ordinal(0, 0)).GetValue(), 999);
    EXPECT_EQ(prepGen.Generate(Ordinal(0, 1)).GetValue(), 0);
    EXPECT_EQ(prepGen.Generate(Ordinal(0, 5)).GetValue(), 4);

    Ordinal baseLen(0, 3);
    AppendGenerator<int> appGen(&baseGen, 888, baseLen);

    EXPECT_EQ(appGen.Generate(Ordinal(0, 0)).GetValue(), 0);
    EXPECT_EQ(appGen.Generate(Ordinal(0, 2)).GetValue(), 2);
    EXPECT_EQ(appGen.Generate(Ordinal(0, 3)).GetValue(), 888);
}

TEST(DecoratorsTest, Decorators_OutOfBoundsReturnsNone) {
    FunctionGenerator<int> baseGen(IdentityRule);
    Ordinal baseLen(0, 3);
    AppendGenerator<int> appGen(&baseGen, 888, baseLen);

    // Длина стала 4 (индексы 0..3). Индекс 4 должен вернуть None (а не Exception)
    EXPECT_TRUE(appGen.Generate(Ordinal(0, 4)).IsNone());
    EXPECT_TRUE(appGen.Generate(Ordinal::Omega()).IsNone());
}

TEST(DecoratorsTest, InsertAt_Shift) {
    FunctionGenerator<int> baseGen(IdentityRule);
    Ordinal insertPos(0, 2);
    InsertAtGenerator<int> insGen(&baseGen, 777, insertPos);

    EXPECT_EQ(insGen.Generate(Ordinal(0, 0)).GetValue(), 0);
    EXPECT_EQ(insGen.Generate(Ordinal(0, 1)).GetValue(), 1);
    EXPECT_EQ(insGen.Generate(Ordinal(0, 2)).GetValue(), 777);
    EXPECT_EQ(insGen.Generate(Ordinal(0, 3)).GetValue(), 2);
}

TEST(DecoratorsTest, Concat_CantorSubtraction) {
    FunctionGenerator<int> baseGen(IdentityRule);
    Ordinal leftLen(0, 3);
    ConcatGenerator<int> concatGen(&baseGen, &baseGen, leftLen);

    EXPECT_EQ(concatGen.Generate(Ordinal(0, 0)).GetValue(), 0);
    EXPECT_EQ(concatGen.Generate(Ordinal(0, 2)).GetValue(), 2);
    EXPECT_EQ(concatGen.Generate(Ordinal(0, 3)).GetValue(), 0);
    EXPECT_EQ(concatGen.Generate(Ordinal(0, 5)).GetValue(), 2);
}

TEST(DecoratorsTest, Map_Application) {
    FunctionGenerator<int> baseGen(IdentityRule);
    MapGenerator<int> mapGen(&baseGen, MultiplyByTen);

    EXPECT_EQ(mapGen.Generate(Ordinal(0, 0)).GetValue(), 0);
    EXPECT_EQ(mapGen.Generate(Ordinal(0, 3)).GetValue(), 30);
}

TEST(DecoratorsTest, Where_FilteringAndBounds) {
    FunctionGenerator<int> baseGen(IdentityRule);
    Ordinal sourceLen(0, 6);
    WhereGenerator<int> whereGen(&baseGen, IsEven, sourceLen);

    EXPECT_EQ(whereGen.Generate(Ordinal(0, 0)).GetValue(), 0);
    EXPECT_EQ(whereGen.Generate(Ordinal(0, 1)).GetValue(), 2);
    EXPECT_EQ(whereGen.Generate(Ordinal(0, 2)).GetValue(), 4);

    // Запрашиваем 3-й подходящий (его нет). Должен вернуть None
    EXPECT_TRUE(whereGen.Generate(Ordinal(0, 3)).IsNone());
}