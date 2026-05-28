// --- Конструкторы и деструктор ---
template <class T>
ArraySequence<T>::ArraySequence() {
    items = new DynamicArray<T>();
}

template <class T>
ArraySequence<T>::ArraySequence(T* arr, int count) {
    items = new DynamicArray<T>(arr, count);
}

template <class T>
ArraySequence<T>::ArraySequence(const LinkedList<T>& list) {
    int length = list.GetLength();
    items = new DynamicArray<T>(length);
    for (int i = 0; i < length; ++i) {
        items->Set(i, list.Get(i));
    }
}

template <class T>
ArraySequence<T>::ArraySequence(const ArraySequence<T>& seq) {
    items = new DynamicArray<T>(*(seq.items));
}

template <class T>
ArraySequence<T>::~ArraySequence() {
    delete items;
}

// --- Декомпозиция ---
template <class T>
const T& ArraySequence<T>::Get(int index) const {
    return items->Get(index);
}

template <class T>
int ArraySequence<T>::GetLength() const {
    return items->GetSize();
}

// --- Операции ---
template <class T>
Sequence<T>* ArraySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    int len = this->GetLength();
    if (startIndex < 0 || startIndex >= len || endIndex < 0 || endIndex >= len || startIndex > endIndex) {
        throw IndexOutOfRange("GetSubsequence: Invalid indices");
    }

    int count = endIndex - startIndex + 1;

    // Создаем пустую оболочку и сразу выделяем память
    ArraySequence<T>* result = static_cast<ArraySequence<T>*>(this->CreateEmpty());
    result->items->Resize(count);

    // Прямое копирование
    for (int i = 0; i < count; ++i) {
        result->items->Set(i, this->items->Get(startIndex + i));
    }

    return result;
}

template <class T>
Sequence<T>* ArraySequence<T>::Concat(Sequence<T>* list) const {
    ArraySequence<T>* result = static_cast<ArraySequence<T>*>(this->CreateEmpty());

    int otherLen = (list != nullptr) ? list->GetLength() : 0;
    result->items->Resize(this->GetLength() + otherLen);

    int destIndex = 0;
    // Копируем себя
    for (int i = 0; i < this->GetLength(); ++i) {
        result->items->Set(destIndex++, this->items->Get(i));
    }

    // Копируем other полиморфно
    if (list != nullptr) {
        IEnumerator<T>* en = list->GetEnumerator();
        try {
            while (en->MoveNext()) {
                result->items->Set(destIndex++, en->GetCurrent());
            }
        } catch (...) {
            delete en;
            delete result;
            throw;
        }
        delete en;
    }

    return result;
}

template <class T>
Sequence<T>* ArraySequence<T>::Slice(int index, int count, Sequence<T>* insertSeq) {
    int len = this->GetLength();
    int start = (index < 0) ? (len + index) : index;
    if (start < 0 || start > len) throw IndexOutOfRange("Slice: Out of bounds");
    if (count < 0) count = 0;
    if (start + count > len) count = len - start;

    ArraySequence<T>* result = static_cast<ArraySequence<T>*>(this->CreateEmpty());
    int insertLen = (insertSeq != nullptr) ? insertSeq->GetLength() : 0;

    // ОДИН раз выделяем финальный размер
    result->items->Resize(len - count + insertLen);

    int destIndex = 0;

    // 1. До вырезанного куска
    for (int i = 0; i < start; ++i) {
        result->items->Set(destIndex++, this->items->Get(i));
    }

    // 2. Вставка
    if (insertSeq != nullptr) {
        IEnumerator<T>* en = insertSeq->GetEnumerator();
        try {
            while (en->MoveNext()) {
                result->items->Set(destIndex++, en->GetCurrent());
            }
        } catch (...) {
            delete en;
            delete result;
            throw;
        }
        delete en;
    }

    // 3. После вырезанного куска
    for (int i = start + count; i < len; ++i) {
        result->items->Set(destIndex++, this->items->Get(i));
    }

    return result;
}

// --- Внутренние методы модификации ---
template <class T>
Sequence<T>* ArraySequence<T>::AppendInternal(const T& item) {
    int size = items->GetSize();
    items->Resize(size + 1);
    items->Set(size, item);
    return this;
}

template <class T>
Sequence<T>* ArraySequence<T>::PrependInternal(const T& item) {
    int size = items->GetSize();
    items->Resize(size + 1);
    for (int i = size; i > 0; --i) {
        items->Set(i, items->Get(i - 1));
    }
    items->Set(0, item);
    return this;
}

template <class T>
Sequence<T>* ArraySequence<T>::InsertAtInternal(const T& item, int index) {
    int size = items->GetSize();
    if (index < 0 || index > size) {
        throw IndexOutOfRange("Invalid index in InsertAt");
    }
    items->Resize(size + 1);
    for (int i = size; i > index; --i) {
        items->Set(i, items->Get(i - 1));
    }
    items->Set(index, item);
    return this;
}

// --- Публичные методы модификации ---
template <class T>
Sequence<T>* ArraySequence<T>::Append(const T& item) {
    return this->Instance()->AppendInternal(item);
}

template <class T>
Sequence<T>* ArraySequence<T>::Prepend(const T& item) {
    return this->Instance()->PrependInternal(item);
}

template <class T>
Sequence<T>* ArraySequence<T>::InsertAt(const T& item, int index) {
    return this->Instance()->InsertAtInternal(item, index);
}

template <class T>
const T& ArraySequence<T>::operator[](int index) const {
    return this->Get(index);
}

template <class T>
ArraySequence<T>& ArraySequence<T>::operator=(const ArraySequence<T>& other) {
    if (this != &other) {
        *(this->items) = *(other.items);
    }
    return *this;
}