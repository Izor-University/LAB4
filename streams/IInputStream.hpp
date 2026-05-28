#ifndef I_INPUT_STREAM_HPP
#define I_INPUT_STREAM_HPP

#include "../core/Exceptions.hpp"
#include "../core/Ordinal.hpp"

template <class T>
class IInputStream {
public:
    virtual ~IInputStream() {}

    // Управление состоянием
    virtual void Open() = 0;
    virtual void Close() = 0;

    // Навигация и состояние (теперь через Ordinal)
    virtual bool IsEndOfStream() const = 0;
    virtual Ordinal GetPosition() const = 0;

    virtual bool IsCanSeek() const = 0;
    virtual bool IsCanGoBack() const = 0;
    virtual Ordinal Seek(const Ordinal& index) = 0;

    // Чтение
    virtual T Input() = 0;
};

#endif // I_INPUT_STREAM_HPP