#include <stdexcept>

template <typename T>
RecurrentGenerator<T>::RecurrentGenerator(std::function<T(ISequence<T>*)> rule, ISequence<T>* history)
    : rule_(rule), history_(history) {
    if (!rule_) {
        throw std::invalid_argument("Generation rule cannot be empty");
    }
    if (history_ == nullptr) {
        throw std::invalid_argument("History sequence pointer cannot be null");
    }
}

template <typename T>
RecurrentGenerator<T>::~RecurrentGenerator() {
    // history_ не удаляется здесь, так как генератор им не владеет.
    // Последовательность сама управляет своей историей.
}

template <typename T>
T RecurrentGenerator<T>::GetNext() {
    return rule_(history_);
}

template <typename T>
bool RecurrentGenerator<T>::HasNext() const {
    // Рекуррентные математические генераторы по умолчанию бесконечны
    return true;
}

template <typename T>
IGenerator<T>* RecurrentGenerator<T>::Clone() const {
    // Создаем новую копию генератора с теми же параметрами
    return new RecurrentGenerator<T>(rule_, history_);
}