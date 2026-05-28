// --- Конструкторы ---
template <class T>
DynamicArray<T>::DynamicArray() : data(nullptr), size(0), capacity(0) {}

template <class T>
DynamicArray<T>::DynamicArray(int size) : size(size), capacity(size) {
    if (size < 0) {
        throw IndexOutOfRange("Error: Size cannot be negative!");
    }
    data = (this->capacity > 0) ? new T[this->capacity]() : nullptr;
}

template <class T>
DynamicArray<T>::DynamicArray(T* items, int count) : size(count), capacity(count) {
    if (count < 0) {
        throw IndexOutOfRange("Error: Count cannot be negative!");
    }
    data = (this->capacity > 0) ? new T[this->capacity]() : nullptr;
    for (int i = 0; i < this->size; ++i) {
        data[i] = items[i];
    }
}

// --- Копирующий и Move конструкторы ---
template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other) : size(other.size), capacity(other.capacity) {
    data = (this->capacity > 0) ? new T[this->capacity]() : nullptr;
    for (int i = 0; i < this->size; ++i) {
        data[i] = other.data[i];
    }
}

template <class T>
DynamicArray<T>::DynamicArray(DynamicArray<T>&& other) noexcept
    : data(other.data), size(other.size), capacity(other.capacity) {
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
}

// --- Деструктор ---
template <class T>
DynamicArray<T>::~DynamicArray() {
    delete[] data;
}

// --- Операторы присваивания ---
template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this != &other) {
        DynamicArray<T> temp(other);

        T* tempData = data;
        data = temp.data;
        temp.data = tempData;

        int tempSize = size;
        size = temp.size;
        temp.size = tempSize;

        int tempCapacity = capacity;
        capacity = temp.capacity;
        temp.capacity = tempCapacity;
    }
    return *this;
}

template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray<T>&& other) noexcept {
    if (this != &other) {
        delete[] data;

        data = other.data;
        size = other.size;
        capacity = other.capacity;

        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }
    return *this;
}

// --- Декомпозиция ---
template <class T>
const T& DynamicArray<T>::Get(int index) const {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in Get()!");
    }
    return data[index];
}

template <class T>
int DynamicArray<T>::GetSize() const {
    return size;
}

template <class T>
int DynamicArray<T>::GetCapacity() const {
    return capacity;
}

// --- Операции ---
template <class T>
void DynamicArray<T>::Set(int index, const T& value) {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in Set()!");
    }
    data[index] = value;
}

template <class T>
void DynamicArray<T>::RemoveAt(int index) {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in RemoveAt()!");
    }
    for (int i = index; i < size - 1; ++i) {
        data[i] = data[i + 1];
    }
    size--;
}

template <class T>
void DynamicArray<T>::Resize(int newSize) {
    if (newSize < 0) {
        throw IndexOutOfRange("Error: New size cannot be negative!");
    }

    if (newSize <= capacity) {
        if (newSize > size) {
            for (int i = size; i < newSize; ++i) {
                data[i] = T();
            }
        }
        size = newSize;
        return;
    }

    int newCapacity = (capacity == 0) ? 1 : capacity * 2;
    if (newCapacity < newSize) {
        newCapacity = newSize;
    }

    T* newData = new T[newCapacity]();
    for (int i = 0; i < size; ++i) {
        newData[i] = data[i];
    }

    delete[] data;
    data = newData;
    size = newSize;
    capacity = newCapacity;
}

// --- Фабрика итератора ---
template <class T>
IEnumerator<T>* DynamicArray<T>::GetEnumerator() const {
    return new Enumerator(this);
}
