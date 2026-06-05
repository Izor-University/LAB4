#include "ConsoleUI.hpp"
#include <iostream>
#include "../core/Ordinal.hpp"
#include "../core/Exceptions.hpp"
#include "../generators/FunctionGenerator.hpp"
#include "../generators/SmartDataGenerator.hpp"
#include "../streams/LazyInputStream.hpp"
#include "../streams/LazyOutputStream.hpp"

// --- Локальные функции и железобетонный ввод ---
namespace {
    int LinearRule(const Ordinal& idx) { return idx.GetOffset(); }
    int Mul10(const int& v) { return v * 10; }
    bool IsEven(const int& v) { return v % 2 == 0; }
    int Sum(const int& acc, const int& v) { return acc + v; }

    // Агрессивная очистка буфера до самого конца строки
    void ConsumeLine() {
        while (true) {
            int c = std::cin.get();
            if (c == '\n' || c == EOF) break;
        }
    }

    // Умное чтение: читает число и сразу чистит за собой мусор
    int ReadInt() {
        int val;
        if (!(std::cin >> val)) {
            std::cin.clear();
            ConsumeLine();
            throw Exception("Invalid input format. Expected a number!");
        }
        ConsumeLine(); // Съедаем '\n' и любой мусор после числа
        return val;
    }
}

// --- Конструктор и Деструктор ---
ConsoleUI::ConsoleUI() {
    for (int i = 0; i < REG_COUNT; ++i) {
        int_regs[i] = nullptr;
        double_regs[i] = nullptr;
    }
}

ConsoleUI::~ConsoleUI() {
    FreeAll();
}

// --- Вспомогательные методы UI ---
void ConsoleUI::FreeAll() {
    for (int i = 0; i < REG_COUNT; ++i) {
        if (int_regs[i]) { delete int_regs[i]; int_regs[i] = nullptr; }
        if (double_regs[i]) { delete double_regs[i]; double_regs[i] = nullptr; }
    }
}

void ConsoleUI::ClearScreen() const {
    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
}

void ConsoleUI::WaitForKey() const {
    std::cout << "\nPress Enter to return to menu...";
    ConsumeLine();
}

void ConsoleUI::PrintDashboard() const {
    std::cout << "============================================================\n"
              << "                      REGISTERS DASHBOARD                   \n"
              << "============================================================\n";
    for (int i = 0; i < REG_COUNT; ++i) {
        if (int_regs[i]) {
            Ordinal len = int_regs[i]->GetOrdinalLength();

            // ИСПРАВЛЕНИЕ: Выводим множитель бесконечности (Omega * K)
            std::cout << " [Int " << i << "] (Len: ";
            if (len.IsInfinite()) {
                std::cout << "Omega*" << len.GetOmegaCount() << " + ";
            }
            std::cout << len.GetOffset() << ") -> [ ";

            int previewCount = 7;
            if (len.IsFinite() && len.GetOffset() < 7) previewCount = len.GetOffset();

            for (int j = 0; j < previewCount; ++j) {
                try { std::cout << int_regs[i]->GetByOrdinal(Ordinal(0, j)) << " "; }
                catch (...) { break; }
            }
            if (len.IsInfinite() || len.GetOffset() > 7) std::cout << "... ";
            std::cout << "]\n";
        }
        else if (double_regs[i]) {
            Ordinal len = double_regs[i]->GetOrdinalLength();

            // ИСПРАВЛЕНИЕ: Выводим множитель бесконечности (Omega * K)
            std::cout << " [Dbl " << i << "] (Len: ";
            if (len.IsInfinite()) {
                std::cout << "Omega*" << len.GetOmegaCount() << " + ";
            }
            std::cout << len.GetOffset() << ") -> [ ";

            int previewCount = 5;
            if (len.IsFinite() && len.GetOffset() < 5) previewCount = len.GetOffset();

            for (int j = 0; j < previewCount; ++j) {
                try { std::cout << double_regs[i]->GetByOrdinal(Ordinal(0, j)) << " "; }
                catch (...) { break; }
            }
            if (len.IsInfinite() || len.GetOffset() > 5) std::cout << "... ";
            std::cout << "]\n";
        }
        else {
            std::cout << " [Reg " << i << "] EMPTY\n";
        }
    }
    std::cout << "============================================================\n";
}

void ConsoleUI::PrintMenu() const {
    std::cout << " 1. Create Linear Sequence (Int)\n"
              << " 2. Create Smart Sequence (Double / Var 14)\n"
              << " 3. Print N Elements from Ordinal (Take)\n"   // Обновлено
              << " 4. Get Element by Ordinal\n"                 // Новое
              << " 5. Append Element\n"
              << " 6. Concat Two Sequences\n"
              << " 7. Map (Multiply by 10)\n"
              << " 8. Where (Filter Even numbers)\n"
              << " 9. Reduce (Sum of elements)\n"
              << "10. Stream Write (Append via Stream)\n"
              << "11. Stream Read (Seek & Input)\n"
              << "12. Free Register\n"
              << " 0. Exit\n"
              << "------------------------------------------------------------\n"
              << "Select action: ";
}

// --- Главный цикл ---
void ConsoleUI::Run() {
    int choice = -1;

    while (true) {
        ClearScreen();
        PrintDashboard();
        PrintMenu();

        if (!(std::cin >> choice)) {
            std::cin.clear();
            ConsumeLine();
            std::cout << "[ERROR] Invalid input. Please enter a number.\n";
            WaitForKey();
            continue;
        }
        ConsumeLine(); // Съедаем '\n' после ввода choice

        if (choice == 0) {
            std::cout << "Cleaning up memory and exiting...\n";
            break;
        }

        try {
            switch (choice) {
                case 1: CreateLinearSequence(); break;
                case 2: CreateSmartSequence(); break;
                case 3: PrintElements(); break;
                case 4: GetElement(); break;          // <--- Новое
                case 5: AppendElement(); break;
                case 6: ConcatSequences(); break;
                case 7: MapSequence(); break;
                case 8: WhereSequence(); break;
                case 9: ReduceSequence(); break;
                case 10: StreamWrite(); break;
                case 11: StreamRead(); break;
                case 12: FreeRegister(); break;
                default: std::cout << "[ERROR] Unknown action.\n"; break;
            }
        }
        catch (const Exception& e) {
            std::cout << "\n[RUNTIME ERROR] " << e.GetMessage() << "\n";
        }
        catch (...) {
            std::cout << "\n[FATAL ERROR] An unexpected exception occurred.\n";
        }

        WaitForKey();
    }
}

// --- Обработчики команд ---
void ConsoleUI::CreateLinearSequence() {
    std::cout << "Enter register ID (0-" << REG_COUNT - 1 << "): ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT) throw Exception("Invalid register");

    std::cout << "Enter length (-1 for Infinity/Omega): ";
    int len = ReadInt();

    Ordinal ordLen = (len == -1) ? Ordinal::Omega() : Ordinal(0, len);

    if (int_regs[reg]) delete int_regs[reg];
    int_regs[reg] = new LazySequence<int>(new FunctionGenerator<int>(LinearRule), ordLen);
    std::cout << "[SUCCESS] Created Linear Sequence.\n";
}

void ConsoleUI::CreateSmartSequence() {
    std::cout << "Enter register ID (0-" << REG_COUNT - 1 << "): ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT) throw Exception("Invalid register");

    std::cout << "Select distribution (1 - Uniform, 2 - Normal): ";
    int distChoice = ReadInt();
    DistributionType dist = (distChoice == 2) ? DistributionType::Normal : DistributionType::Uniform;

    std::cout << "Enter length (-1 for Infinity/Omega): ";
    int len = ReadInt();

    Ordinal ordLen = (len == -1) ? Ordinal::Omega() : Ordinal(0, len);

    if (double_regs[reg]) delete double_regs[reg];
    double_regs[reg] = new LazySequence<double>(new SmartDataGenerator(42, dist, 50.0, 10.0), ordLen);
    std::cout << "[SUCCESS] Created Smart Data Sequence.\n";
}

// ОБНОВЛЕННАЯ: Умеет печатать элементы начиная из-за бесконечности
void ConsoleUI::PrintElements() const {
    std::cout << "Enter register ID: ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT || (!int_regs[reg] && !double_regs[reg]))
        throw Exception("Register is invalid or empty");

    std::cout << "Start from Omega count? (0 for standard, 1 for Omega): ";
    int omega = ReadInt();

    std::cout << "Start from Offset? (0-based, e.g. 0 is the first element): ";
    int offset = ReadInt();

    std::cout << "How many elements to print? ";
    int n = ReadInt();

    if (int_regs[reg]) {
        std::cout << "Int Output: [ ";
        for (int i = 0; i < n; ++i) {
            try { std::cout << int_regs[reg]->GetByOrdinal(Ordinal(omega, offset + i)) << " "; }
            catch (...) { std::cout << "<END> "; break; }
        }
        std::cout << "]\n";
    } else {
        std::cout << "Double Output: [ ";
        for (int i = 0; i < n; ++i) {
            try { std::cout << double_regs[reg]->GetByOrdinal(Ordinal(omega, offset + i)) << " "; }
            catch (...) { std::cout << "<END> "; break; }
        }
        std::cout << "]\n";
    }
}

// НОВАЯ: Получить конкретный элемент по Ординалу
void ConsoleUI::GetElement() const {
    std::cout << "Enter register ID: ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT || (!int_regs[reg] && !double_regs[reg]))
        throw Exception("Register is invalid or empty");

    std::cout << "Target Omega count (0 for finite, 1 for Omega): ";
    int omega = ReadInt();

    std::cout << "Target Offset (0-based, e.g. w+0 is the 1st element after Omega): ";
    int offset = ReadInt();

    Ordinal target(omega, offset);

    if (int_regs[reg]) {
        int val = int_regs[reg]->GetByOrdinal(target);
        std::cout << "[SUCCESS] Element at Omega*" << omega << " + " << offset << " is: " << val << "\n";
    } else {
        double val = double_regs[reg]->GetByOrdinal(target);
        std::cout << "[SUCCESS] Element at Omega*" << omega << " + " << offset << " is: " << val << "\n";
    }
}

void ConsoleUI::AppendElement() {
    std::cout << "Enter Int register ID: ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT || !int_regs[reg]) throw Exception("Register is invalid or empty");

    std::cout << "Enter value to append: ";
    int val = ReadInt();

    Sequence<int>* newSeq = int_regs[reg]->Append(val);
    delete int_regs[reg];
    int_regs[reg] = static_cast<LazySequence<int>*>(newSeq);
    std::cout << "[SUCCESS] Value appended.\n";
}

void ConsoleUI::ConcatSequences() {
    std::cout << "Enter First Int register ID: ";
    int r1 = ReadInt();
    std::cout << "Enter Second Int register ID: ";
    int r2 = ReadInt();
    std::cout << "Enter Destination register ID: ";
    int rout = ReadInt();

    if (r1 < 0 || r2 < 0 || rout < 0 || r1 >= REG_COUNT || r2 >= REG_COUNT || rout >= REG_COUNT)
        throw Exception("Invalid register bounds");
    if (!int_regs[r1] || !int_regs[r2]) throw Exception("Source register is empty");

    Sequence<int>* newSeq = int_regs[r1]->Concat(int_regs[r2]);
    if (int_regs[rout]) delete int_regs[rout];
    int_regs[rout] = static_cast<LazySequence<int>*>(newSeq);
    std::cout << "[SUCCESS] Sequences concatenated.\n";
}

void ConsoleUI::MapSequence() {
    std::cout << "Enter Int register ID: ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT || !int_regs[reg]) throw Exception("Register is invalid or empty");

    Sequence<int>* newSeq = int_regs[reg]->Map(Mul10);
    delete int_regs[reg];
    int_regs[reg] = static_cast<LazySequence<int>*>(newSeq);
    std::cout << "[SUCCESS] Map (x * 10) applied.\n";
}

void ConsoleUI::WhereSequence() {
    std::cout << "Enter Int register ID: ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT || !int_regs[reg]) throw Exception("Register is invalid or empty");

    Sequence<int>* newSeq = int_regs[reg]->Where(IsEven);
    delete int_regs[reg];
    int_regs[reg] = static_cast<LazySequence<int>*>(newSeq);
    std::cout << "[SUCCESS] Where (keep even numbers) applied.\n";
}

void ConsoleUI::ReduceSequence() const {
    std::cout << "Enter Int register ID: ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT || !int_regs[reg]) throw Exception("Register is invalid or empty");

    int sum = int_regs[reg]->Reduce(Sum, 0);
    std::cout << "[SUCCESS] Reduce result (Sum) = " << sum << "\n";
}

void ConsoleUI::StreamWrite() {
    std::cout << "Enter Int register ID: ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT || !int_regs[reg]) throw Exception("Register is invalid or empty");

    std::cout << "Enter value to write via Stream: ";
    int val = ReadInt();

    LazyOutputStream<int> stream(int_regs[reg]);
    stream.Open();
    Ordinal pos = stream.Output(val);
    stream.Close();

    int_regs[reg] = stream.GetSequence();
    std::cout << "[SUCCESS] Value " << val << " written via Stream at position Omega*"
              << pos.GetOmegaCount() << " + " << pos.GetOffset() << ".\n";
}

void ConsoleUI::StreamRead() const {
    std::cout << "Enter Int register ID: ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT || !int_regs[reg]) throw Exception("Register is invalid or empty");

    std::cout << "Target Omega count for Stream Seek (0 for finite, 1 for Omega): ";
    int omega = ReadInt();
    std::cout << "Target Offset for Stream Seek: ";
    int offset = ReadInt();

    LazyInputStream<int> stream(int_regs[reg]);
    stream.Open();
    stream.Seek(Ordinal(omega, offset));
    int val = stream.Input();
    stream.Close();

    std::cout << "[SUCCESS] Stream read value: " << val << " at Omega*" << omega << " + " << offset << ".\n";
}

void ConsoleUI::FreeRegister() {
    std::cout << "Enter register ID to free: ";
    int reg = ReadInt();
    if (reg < 0 || reg >= REG_COUNT) throw Exception("Invalid register");
    
    if (int_regs[reg]) { delete int_regs[reg]; int_regs[reg] = nullptr; }
    if (double_regs[reg]) { delete double_regs[reg]; double_regs[reg] = nullptr; }
    std::cout << "[SUCCESS] Register cleared.\n";
}