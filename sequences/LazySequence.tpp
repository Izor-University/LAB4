// =========================================================
// sequences/LazySequence.tpp
// =========================================================

// =========================================================
// ИТЕРАТОР
// =========================================================
template <class T>
bool LazySequence<T>::LazyEnumerator::MoveNext() {
    if (!started) {
        started = true;
        currentIndex = Ordinal(0, 0);
    } else {
        currentIndex = currentIndex + Ordinal(0, 1);
    }

    if (seq->GetOrdinalLength().IsFinite()) {
        return currentIndex < seq->GetOrdinalLength();
    }

    // Для бесконечных генераторов итератор перебирает только натуральные индексы первой бесконечности
    return currentIndex.GetOmegaCount() == 0;
}

template <class T>
const T& LazySequence<T>::LazyEnumerator::GetCurrent() const {
    if (!started) throw IndexOutOfRange("Enumerator has not started");
    return seq->GetByOrdinal(currentIndex);
}

template <class T>
void LazySequence<T>::LazyEnumerator::Reset() {
    started = false;
    currentIndex = Ordinal(0, 0);
}

// =========================================================
// КОНСТРУКТОРЫ И ПАМЯТЬ
// =========================================================
template <class T>
LazySequence<T>::LazySequence(IGenerator<T>* gen, const Ordinal& length)
    : generator(gen), virtualLength(length) {
    caches = new DynamicArray<MutableArraySequence<T>*>();
    caches->Append(new MutableArraySequence<T>());
}

template <class T>
LazySequence<T>::LazySequence(const LazySequence<T>& other)
    : virtualLength(other.virtualLength) {
    generator = other.generator->Clone();
    caches = new DynamicArray<MutableArraySequence<T>*>();
    for (int i = 0; i < other.caches->GetLength(); ++i) {
        caches->Append(static_cast<MutableArraySequence<T>*>(other.caches->Get(i)->Clone()));
    }
}

template <class T>
LazySequence<T>::~LazySequence() {
    for (int i = 0; i < caches->GetLength(); ++i) {
        delete caches->Get(i);
    }
    delete caches;
    delete generator;
}

// =========================================================
// ФАБРИЧНЫЕ МЕТОДЫ (Интеграция с core)
// =========================================================
template <class T>
ISequenceBuilder<T>* LazySequence<T>::CreateBuilder() const {
    return (new MutableArraySequence<T>())->CreateBuilder();
}

template <class T>
Sequence<T>* LazySequence<T>::CreateEmpty() const {
    return new MutableArraySequence<T>();
}

template <class T>
Sequence<T>* LazySequence<T>::Clone() const {
    return new LazySequence<T>(*this);
}

template <class T>
IEnumerator<T>* LazySequence<T>::GetEnumerator() const {
    return new LazyEnumerator(this);
}

// =========================================================
// ДВУМЕРНАЯ МЕМОИЗАЦИЯ И ДЕКОМПОЗИЦИЯ
// =========================================================
template <class T>
void LazySequence<T>::EnsureCacheLevel(int omegaCount) const {
    while (caches->GetLength() <= omegaCount) {
        caches->Append(new MutableArraySequence<T>());
    }
}

template <class T>
const T& LazySequence<T>::GetByOrdinal(const Ordinal& index) const {
    if (index >= virtualLength) {
        throw IndexOutOfRange("LazySequence: Ordinal index out of bounds");
    }

    int k = index.GetOmegaCount();
    int n = index.GetOffset();

    EnsureCacheLevel(k);
    MutableArraySequence<T>* currentCache = caches->Get(k);

    while (currentCache->GetLength() <= n) {
        Ordinal target(k, currentCache->GetLength());
        T computedItem = generator->Generate(target);
        currentCache->Append(computedItem);
    }

    return currentCache->Get(n);
}

template <class T>
const T& LazySequence<T>::Get(int index) const {
    return GetByOrdinal(Ordinal(0, index));
}

template <class T>
int LazySequence<T>::GetLength() const {
    if (virtualLength.IsFinite()) {
        return virtualLength.GetOffset();
    }
    return -1;
}

template <class T>
Ordinal LazySequence<T>::GetOrdinalLength() const {
    return virtualLength;
}

template <class T>
const T& LazySequence<T>::operator[](int index) const {
    return this->GetByOrdinal(Ordinal(0, index));
}

// =========================================================
// АЛГЕБРА ЛЕНИВЫХ СПИСКОВ И ОРДИНАЛОВ
// =========================================================
template <class T>
Sequence<T>* LazySequence<T>::Append(const T& item) {
    IGenerator<T>* dec = new AppendGenerator<T>(this->generator, item, this->virtualLength);
    Ordinal newLen = this->virtualLength + Ordinal(0, 1);
    return new LazySequence<T>(dec, newLen);
}

template <class T>
Sequence<T>* LazySequence<T>::Prepend(const T& item) {
    IGenerator<T>* dec = new PrependGenerator<T>(this->generator, item);
    Ordinal newLen = virtualLength.IsInfinite() ? virtualLength : virtualLength + Ordinal(0, 1);
    return new LazySequence<T>(dec, newLen);
}

template <class T>
Sequence<T>* LazySequence<T>::InsertAt(const T& item, int index) {
    throw Exception("InsertAt demands materialization or split-generators; unsupported in pure ordinal mode without adapter.");
}

template <class T>
Sequence<T>* LazySequence<T>::GetSubsequence(const Ordinal& startIndex, const Ordinal& endIndex) const {
    if (endIndex < startIndex) throw IndexOutOfRange("Invalid Subsequence ordinal bounds");
    if (endIndex >= virtualLength) throw IndexOutOfRange("Out of bounds");

    Ordinal newLen = endIndex - startIndex + Ordinal(0, 1);
    IGenerator<T>* dec = new SubsequenceGenerator<T>(this->generator, startIndex);
    return new LazySequence<T>(dec, newLen);
}

template <class T>
Sequence<T>* LazySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    return GetSubsequence(Ordinal(0, startIndex), Ordinal(0, endIndex));
}

template <class T>
Sequence<T>* LazySequence<T>::Concat(Sequence<T>* list) const {
    throw Exception("Concat requires external sequence adapter generator; materialize to fuse.");
}

template <class T>
Sequence<T>* LazySequence<T>::Slice(int index, int count, Sequence<T>* insertSeq) {
    throw Exception("Slice operations are fundamentally mutating; materialize to alter.");
}