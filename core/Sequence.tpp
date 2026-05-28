// --- Функциональные методы: Map, Where, Reduce ---
template <class T>
Sequence<T>* Sequence<T>::Map(T (*mapper)(const T& element)) const {
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    IEnumerator<T>* en = this->GetEnumerator();
    
    try {
        while (en->MoveNext()) {
            builder->Append(mapper(en->GetCurrent()));
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
Sequence<T>* Sequence<T>::Where(bool (*predicate)(const T& element)) const {
    ISequenceBuilder<T>* builder = this->CreateBuilder();
    IEnumerator<T>* en = this->GetEnumerator();
    
    try {
        while (en->MoveNext()) {
            if (predicate(en->GetCurrent())) {
                builder->Append(en->GetCurrent());
            }
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
T Sequence<T>::Reduce(T (*reduce_func)(const T& accumulator, const T& current), const T& start_element) const {
    T result = start_element;
    IEnumerator<T>* en = this->GetEnumerator();
    
    try {
        while (en->MoveNext()) {
            result = reduce_func(result, en->GetCurrent());
        }
    } catch (...) {
        delete en;
        throw;
    }
    
    delete en;
    return result;
}

template <class T>
const T& Sequence<T>::GetFirst() const {
    if (this->GetLength() == 0) {
        throw EmptyCollectionError("Sequence is empty");
    }
    return this->Get(0);
}

template <class T>
const T& Sequence<T>::GetLast() const {
    if (this->GetLength() == 0) {
        throw EmptyCollectionError("Sequence is empty");
    }
    return this->Get(this->GetLength() - 1);
}

// --- Try-семантика ---
template <class T>
Option<T> Sequence<T>::TryGet(int index) const {
    if (index < 0 || index >= this->GetLength()) {
        return Option<T>();
    }
    return Option<T>(this->Get(index));
}

template <class T>
Option<T> Sequence<T>::TryGetFirst() const {
    return this->TryGet(0);
}

template <class T>
Option<T> Sequence<T>::TryGetLast() const {
    return this->TryGet(this->GetLength() - 1);
}

