#include <stdexcept>

template <typename T>
PrependGenerator<T>::PrependGenerator(T item, ISequence<T>* source)
    : item_(item), source_(source), first_returned_(false), source_index_(0) {
    if (source_ == nullptr) {
        throw std::invalid_argument("Source sequence cannot be null");
    }
}

template <typename T>
PrependGenerator<T>::~PrependGenerator() {
    // Не очищаем source_, так как генератор им не владеет
}

template <typename T>
T PrependGenerator<T>::GetNext() {
    if (!first_returned_) {
        first_returned_ = true;
        return item_;
    }
    T val = source_->Get(static_cast<int>(source_index_));
    source_index_++;
    return val;
}

template <typename T>
bool PrependGenerator<T>::HasNext() const {
    if (!first_returned_) return true;
    
    // Пытаемся проверить, есть ли элемент в источнике.
    // Если источник бесконечный или содержит элементы — возвращаем true.
    try {
        // Проверка осуществляется попыткой заглянуть вперед без сдвига состояния
        const_cast<ISequence<T>*>(source_)->Get(static_cast<int>(source_index_));
        return true;
    } catch (...) {
        return false;
    }
}

template <typename T>
IGenerator<T>* PrependGenerator<T>::Clone() const {
    PrependGenerator<T>* clone = new PrependGenerator<T>(item_, source_);
    clone->first_returned_ = this->first_returned_;
    clone->source_index_ = this->source_index_;
    return clone;
}