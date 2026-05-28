#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

// --- Базовый класс для всех исключений ---
class Exception {
protected:
    const char* message;

public:
    // Конструкторы и Деструктор
    explicit Exception(const char* msg) : message(msg) {}
    virtual ~Exception() {}

    // Декомпозиция
    virtual const char* GetMessage() const {
        return message;
    }
};

// --- Исключение для неверных индексов ---
class IndexOutOfRange : public Exception {
public:
    explicit IndexOutOfRange(const char* msg = "Index out of range") : Exception(msg) {}
};

// --- Исключение для пустых коллекций ---
class EmptyCollectionError : public Exception {
public:
    explicit EmptyCollectionError(const char* msg = "Collection is empty") : Exception(msg) {}
};

#endif // EXCEPTIONS_HPP