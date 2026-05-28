// --- Конструкторы и деструктор ---
template <class T>
ListSequence<T>::ListSequence() {
    items = new LinkedList<T>();
}

template <class T>
ListSequence<T>::ListSequence(T* arr, int count) {
    items = new LinkedList<T>(arr, count);
}

template <class T>
ListSequence<T>::ListSequence(const LinkedList<T>& list) {
    items = new LinkedList<T>(list);
}

template <class T>
ListSequence<T>::ListSequence(const ListSequence<T>& seq) {
    items = new LinkedList<T>(*(seq.items));
}

template <class T>
ListSequence<T>::~ListSequence() {
    delete items;
}

// --- Декомпозиция ---
template <class T>
const T& ListSequence<T>::Get(int index) const {
    return items->Get(index);
}

template <class T>
int ListSequence<T>::GetLength() const {
    return items->GetLength();
}

// --- Внутренние методы модификации ---
template <class T>
Sequence<T>* ListSequence<T>::AppendInternal(const T& item) {
    items->Append(item);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::PrependInternal(const T& item) {
    items->Prepend(item);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::InsertAtInternal(const T& item, int index) {
    items->InsertAt(item, index);
    return this;
}

// --- Операции ---
template <class T>
Sequence<T>* ListSequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    int len = this->GetLength();
    if (startIndex < 0 || startIndex >= len || endIndex < 0 || endIndex >= len || startIndex > endIndex) {
        throw IndexOutOfRange("GetSubsequence: Invalid indices");
    }

    ISequenceBuilder<T>* builder = this->CreateBuilder();
    IEnumerator<T>* en = this->GetEnumerator();
    try {
        int currentIndex = 0;
        while (en->MoveNext()) {
            if (currentIndex >= startIndex && currentIndex <= endIndex) {
                builder->Append(en->GetCurrent());
            }
            if (currentIndex > endIndex) {
                break;
            }
            currentIndex++;
        }
    } catch (...) {
        delete en;
        delete builder;
        throw;
    }

    Sequence<T>* result = builder->Build();
    delete en;
    delete builder;
    return result;
}

template <class T>
Sequence<T>* ListSequence<T>::Concat(Sequence<T>* list) const {
    if (list == nullptr) {
        return this->Clone();
    }

    ISequenceBuilder<T>* builder = this->CreateBuilder();
    IEnumerator<T>* en1 = this->GetEnumerator();
    IEnumerator<T>* en2 = list->GetEnumerator();
    try {
        while (en1->MoveNext()) {
            builder->Append(en1->GetCurrent());
        }
        while (en2->MoveNext()) {
            builder->Append(en2->GetCurrent());
        }
    } catch (...) {
        delete en1;
        delete en2;
        delete builder;
        throw;
    }

    Sequence<T>* result = builder->Build();
    delete en1;
    delete en2;
    delete builder;
    return result;
}

template <class T>
Sequence<T>* ListSequence<T>::Slice(int index, int count, Sequence<T>* insertSeq) {
    int len = this->GetLength();
    int start = (index < 0) ? (len + index) : index;
    if (start < 0 || start > len) throw IndexOutOfRange("Slice: Out of bounds");
    if (count < 0) count = 0;
    if (start + count > len) count = len - start;

    ISequenceBuilder<T>* builder = this->CreateBuilder();
    IEnumerator<T>* en = this->GetEnumerator();
    try {
        int currentIndex = 0;
        while (en->MoveNext()) {
            // Вставка
            if (currentIndex == start && insertSeq != nullptr) {
                IEnumerator<T>* insertEn = insertSeq->GetEnumerator();
                try {
                    while (insertEn->MoveNext()) {
                        builder->Append(insertEn->GetCurrent());
                    }
                } catch (...) {
                    delete insertEn;
                    throw;
                }
                delete insertEn;
            }

            // Копирование (если мы не в зоне вырезания)
            if (currentIndex < start || currentIndex >= start + count) {
                builder->Append(en->GetCurrent());
            }
            currentIndex++;
        }

        // Вставка в самый конец (если вырезание было до конца)
        if (start == len && insertSeq != nullptr) {
            IEnumerator<T>* insertEn = insertSeq->GetEnumerator();
            try {
                while (insertEn->MoveNext()) {
                    builder->Append(insertEn->GetCurrent());
                }
            } catch (...) {
                delete insertEn;
                throw;
            }
            delete insertEn;
        }
    } catch (...) {
        delete en;
        delete builder;
        throw;
    }

    Sequence<T>* result = builder->Build();
    delete en;
    delete builder;
    return result;
}

// --- Основные операции интерфейса ---
template <class T>
Sequence<T>* ListSequence<T>::Append(const T& item) {
    return this->Instance()->AppendInternal(item);
}

template <class T>
Sequence<T>* ListSequence<T>::Prepend(const T& item) {
    return this->Instance()->PrependInternal(item);
}

template <class T>
Sequence<T>* ListSequence<T>::InsertAt(const T& item, int index) {
    return this->Instance()->InsertAtInternal(item, index);
}

template <class T>
const T& ListSequence<T>::operator[](int index) const {
    return this->Get(index);
}

template <class T>
ListSequence<T>& ListSequence<T>::operator=(const ListSequence<T>& other) {
    if (this != &other) {
        *(this->items) = *(other.items);
    }
    return *this;
}