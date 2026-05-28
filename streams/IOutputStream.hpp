#ifndef I_OUTPUT_STREAM_HPP
#define I_OUTPUT_STREAM_HPP

#include "../core/Exceptions.hpp"
#include "../core/Ordinal.hpp"

template <class T>
class IOutputStream {
public:
    virtual ~IOutputStream() {}

    // Управление состоянием
    virtual void Open() = 0;
    virtual void Close() = 0;

    // Позиция записи (Ordinal)
    virtual Ordinal GetPosition() const = 0;

    // Запись
    virtual Ordinal Output(const T& item) = 0;
};

#endif // I_OUTPUT_STREAM_HPP