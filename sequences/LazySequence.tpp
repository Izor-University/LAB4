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
    caches = new MutableArraySequence<MutableArraySequence<T>*>();
    caches->Append(new MutableArraySequence<T>());
}

template <class T>
LazySequence<T>::LazySequence(const LazySequence<T>& other)
    : virtualLength(other.virtualLength) {
    generator = other.generator->Clone();
    caches = new MutableArraySequence<MutableArraySequence<T>*>();
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

template <class T>
LazySequence<T>& LazySequence<T>::operator=(const LazySequence<T>& other) {
    if (this == &other) return *this;

    for (int i = 0; i < caches->GetLength(); ++i) {
        delete caches->Get(i);
    }
    delete caches;
    delete generator;

    virtualLength = other.virtualLength;
    generator = other.generator->Clone();

    caches = new MutableArraySequence<MutableArraySequence<T>*>();
    for (int i = 0; i < other.caches->GetLength(); ++i) {
        caches->Append(static_cast<MutableArraySequence<T>*>(other.caches->Get(i)->Clone()));
    }

    return *this;
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
// ДВУМЕРНАЯ МЕМОИЗАЦИЯ И ДЕКОМПОЗИЦИЯ С TRY-СЕМАНТИКОЙ
// =========================================================
template <class T>
void LazySequence<T>::EnsureCacheLevel(int omegaCount) const {
    while (caches->GetLength() <= omegaCount) {
        caches->Append(new MutableArraySequence<T>());
    }
}

template <class T>
Option<T> LazySequence<T>::TryGet(int index) const {
    return TryGetByOrdinal(Ordinal(0, index));
}

template <class T>
Option<T> LazySequence<T>::TryGetByOrdinal(const Ordinal& index) const {
    if (index >= virtualLength) {
        return Option<T>(); // None
    }

    int k = index.GetOmegaCount();
    int n = index.GetOffset();

    EnsureCacheLevel(k);
    MutableArraySequence<T>* currentCache = caches->Get(k);

    while (currentCache->GetLength() <= n) {
        Ordinal target(k, currentCache->GetLength());
        Option<T> optItem = generator->Generate(target);

        if (optItem.IsNone()) {
            const_cast<LazySequence<T>*>(this)->virtualLength = target;
            return Option<T>();
        }

        currentCache->Append(optItem.GetValue());
    }

    return Option<T>(currentCache->Get(n));
}

template <class T>
const T& LazySequence<T>::GetByOrdinal(const Ordinal& index) const {
    Option<T> opt = TryGetByOrdinal(index);
    if (opt.IsNone()) {
        throw IndexOutOfRange("LazySequence: Element not found (Stream ended or index out of bounds)");
    }
    int k = index.GetOmegaCount();
    int n = index.GetOffset();
    return caches->Get(k)->Get(n);
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

template <class T>
const T& LazySequence<T>::GetFirst() const {
    if (virtualLength == Ordinal(0, 0)) throw EmptyCollectionError();
    return GetByOrdinal(Ordinal(0, 0));
}

template <class T>
const T& LazySequence<T>::GetLast() const {
    if (virtualLength == Ordinal(0, 0)) throw EmptyCollectionError();
    if (virtualLength.IsInfinite()) throw Exception("Cannot get the last element of an infinite lazy sequence");
    return GetByOrdinal(virtualLength - Ordinal(0, 1));
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
Sequence<T>* LazySequence<T>::InsertAt(const T& item, int index) {
    Ordinal targetIndex(0, index);
    if (targetIndex > this->virtualLength) throw IndexOutOfRange("InsertAt: Index out of bounds");

    IGenerator<T>* dec = new InsertAtGenerator<T>(this->generator, item, targetIndex);
    Ordinal newLen = this->virtualLength.IsInfinite() ? this->virtualLength : this->virtualLength + Ordinal(0, 1);

    return new LazySequence<T>(dec, newLen);
}

template <class T>
Sequence<T>* LazySequence<T>::Concat(Sequence<T>* list) const {
    LazySequence<T>* lazyList = dynamic_cast<LazySequence<T>*>(list);

    if (lazyList != nullptr) {
        Ordinal newLen = this->GetOrdinalLength() + lazyList->GetOrdinalLength();
        IGenerator<T>* dec = new ConcatGenerator<T>(this->generator, lazyList->generator, this->GetOrdinalLength());

        return new LazySequence<T>(dec, newLen);
    } else {
        throw Exception("LazySequence Concat with non-lazy Sequence is not supported natively without adapter.");
    }
}

template <class T>
LazySequence<T>* LazySequence<T>::InterleaveWith(Sequence<T>* seq2, Sequence<T>* seq3) const {
    LazySequence<T>* lazy2 = dynamic_cast<LazySequence<T>*>(seq2);
    LazySequence<T>* lazy3 = dynamic_cast<LazySequence<T>*>(seq3);

    if (lazy2 != nullptr && lazy3 != nullptr) {
        IGenerator<T>* dec = new InterleaveThreeGenerator<T>(this->generator, lazy2->generator, lazy3->generator);

        return new LazySequence<T>(dec, Ordinal::Omega());
    } else {
        throw Exception("Interleave is only supported for 3 LazySequences natively.");
    }
}

// =========================================================
// ФУНКЦИОНАЛЬНЫЕ МЕТОДЫ
// =========================================================

template <class T>
Sequence<T>* LazySequence<T>::Map(T (*mapper)(const T& element)) const {
    IGenerator<T>* dec = new MapGenerator<T>(this->generator, mapper);
    // При Map длина последовательности не меняется
    return new LazySequence<T>(dec, this->virtualLength);
}

template <class T>
Sequence<T>* LazySequence<T>::Where(bool (*predicate)(const T& element)) const {
    IGenerator<T>* dec = new WhereGenerator<T>(this->generator, predicate, this->virtualLength);
    // Длина результата Where неизвестна без полной материализации.
    // Возвращаем исходную длину как максимально возможный "потолок" (Virtual Bound).
    return new LazySequence<T>(dec, this->virtualLength);
}

template <class T>
T LazySequence<T>::Reduce(T (*reduce_func)(const T& accumulator, const T& current), const T& start_element) const {
    if (this->virtualLength.IsInfinite()) {
        // Сворачивать бесконечность — значит запустить бесконечный цикл. Предотвращаем это.
        throw Exception("Cannot Reduce an infinite lazy sequence. Infinite time required.");
    }

    T accumulator = start_element;
    int len = this->virtualLength.GetOffset(); // Извлекаем конечное число элементов

    // Ленивая выгрузка элементов (срабатывает кэширование)
    for (int i = 0; i < len; ++i) {
        accumulator = reduce_func(accumulator, this->Get(i));
    }

    return accumulator;
}

// =========================================================
// ОСТАВШИЕСЯ ЗАГЛУШКИ
// =========================================================

template <class T>
Sequence<T>* LazySequence<T>::Slice(int index, int count, Sequence<T>* insertSeq) {
    throw Exception("Slice is inherently mutational and is not supported in pure LazySequence.");
}