#ifndef CONSOLE_UI_HPP
#define CONSOLE_UI_HPP

#include "../sequences/LazySequence.hpp"

class ConsoleUI {
private:
    static const int REG_COUNT = 5;
    LazySequence<int>* int_regs[REG_COUNT];
    LazySequence<double>* double_regs[REG_COUNT];

    // Вспомогательные методы
    void FreeAll();
    void ClearScreen() const;
    void WaitForKey() const;
    void PrintDashboard() const;
    void PrintMenu() const;

    // Обработчики действий
    void CreateLinearSequence();
    void CreateSmartSequence();
    void PrintElements() const;
    void GetElement() const;
    void AppendElement();
    void ConcatSequences();
    void InterleaveSequences();
    void MapSequence();
    void WhereSequence();
    void ReduceSequence() const;
    void StreamWrite();
    void StreamRead() const;
    void FreeRegister();

public:
    ConsoleUI();
    ~ConsoleUI();

    // Главный цикл приложения
    void Run();
};

#endif // CONSOLE_UI_HPP