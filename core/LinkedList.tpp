// --- Вспомогательный метод очистки ---
template <class T>
void LinkedList<T>::Clear() {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

// --- Конструкторы и Деструктор ---
template <class T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), size(0) {}

template <class T>
LinkedList<T>::LinkedList(T* items, int count) : LinkedList() {
    if (count < 0) {
        throw IndexOutOfRange("Error: Count cannot be negative!");
    }
    for (int i = 0; i < count; ++i) {
        Append(items[i]);
    }
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T>& other) : LinkedList() {
    Node* current = other.head;
    while (current != nullptr) {
        Append(current->data);
        current = current->next;
    }
}

template <class T>
LinkedList<T>::LinkedList(LinkedList<T>&& other) noexcept
    : head(other.head), tail(other.tail), size(other.size) {
    other.head = nullptr;
    other.tail = nullptr;
    other.size = 0;
}

template <class T>
LinkedList<T>::~LinkedList() {
    Clear();
}

// --- Операторы присваивания ---
template <class T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& other) {
    if (this != &other) {
        LinkedList<T> temp(other);

        Node* tempHead = head;
        head = temp.head;
        temp.head = tempHead;

        Node* tempTail = tail;
        tail = temp.tail;
        temp.tail = tempTail;

        int tempSize = size;
        size = temp.size;
        temp.size = tempSize;
    }
    return *this;
}

template <class T>
LinkedList<T>& LinkedList<T>::operator=(LinkedList<T>&& other) noexcept {
    if (this != &other) {
        Clear();

        head = other.head;
        tail = other.tail;
        size = other.size;

        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }
    return *this;
}

// --- Декомпозиция ---
template <class T>
const T& LinkedList<T>::GetFirst() const {
    if (size == 0) {
        throw EmptyCollectionError("Error: List is empty in GetFirst()!");
    }
    return head->data;
}

template <class T>
const T& LinkedList<T>::GetLast() const {
    if (size == 0) {
        throw EmptyCollectionError("Error: List is empty in GetLast()!");
    }
    return tail->data;
}

template <class T>
const T& LinkedList<T>::Get(int index) const {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in Get()!");
    }

    if (index < size / 2) {
        Node* current = head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    } else {
        Node* current = tail;
        for (int i = size - 1; i > index; --i) {
            current = current->prev;
        }
        return current->data;
    }
}

template <class T>
int LinkedList<T>::GetLength() const {
    return size;
}

template <class T>
LinkedList<T>* LinkedList<T>::GetSubList(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= size || startIndex > endIndex) {
        throw IndexOutOfRange("Error: Invalid indices in GetSubList()!");
    }

    LinkedList<T>* subList = new LinkedList<T>();

    Node* current = head;
    for (int i = 0; i < startIndex; ++i) {
        current = current->next;
    }

    for (int i = startIndex; i <= endIndex; ++i) {
        subList->Append(current->data);
        current = current->next;
    }

    return subList;
}

// --- Операции ---
template <class T>
void LinkedList<T>::Append(const T& item) {
    Node* newNode = new Node(item);
    if (size == 0) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    size++;
}

template <class T>
void LinkedList<T>::Prepend(const T& item) {
    Node* newNode = new Node(item);
    if (size == 0) {
        head = tail = newNode;
    } else {
        head->prev = newNode;
        newNode->next = head;
        head = newNode;
    }
    size++;
}

template <class T>
void LinkedList<T>::InsertAt(const T& item, int index) {
    if (index < 0 || index > size) {
        throw IndexOutOfRange("Error: Index out of range in InsertAt()!");
    }

    if (index == 0) {
        Prepend(item);
    } else if (index == size) {
        Append(item);
    } else {
        Node* current;
        if (index < size / 2) {
            current = head;
            for (int i = 0; i < index; ++i) {
                current = current->next;
            }
        } else {
            current = tail;
            for (int i = size - 1; i > index; --i) {
                current = current->prev;
            }
        }

        Node* newNode = new Node(item);

        newNode->prev = current->prev;
        newNode->next = current;
        current->prev->next = newNode;
        current->prev = newNode;

        size++;
    }
}

template <class T>
void LinkedList<T>::RemoveAt(int index) {
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("Error: Index out of range in RemoveAt()!");
    }

    if (index == 0) {
        Node* temp = head;
        head = head->next;
        if (head != nullptr) {
            head->prev = nullptr;
        } else {
            tail = nullptr;
        }
        delete temp;
    } else if (index == size - 1) {
        Node* temp = tail;
        tail = tail->prev;
        if (tail != nullptr) {
            tail->next = nullptr;
        } else {
            head = nullptr;
        }
        delete temp;
    } else {
        Node* current;
        if (index < size / 2) {
            current = head;
            for (int i = 0; i < index; ++i) {
                current = current->next;
            }
        } else {
            current = tail;
            for (int i = size - 1; i > index; --i) {
                current = current->prev;
            }
        }

        current->prev->next = current->next;
        current->next->prev = current->prev;

        delete current;
    }
    size--;
}

template <class T>
LinkedList<T>* LinkedList<T>::Concat(LinkedList<T>* list) const {
    LinkedList<T>* newList = new LinkedList<T>(*this);
    if (list != nullptr) {
        Node* current = list->head;
        while (current != nullptr) {
            newList->Append(current->data);
            current = current->next;
        }
    }
    return newList;
}

// --- Фабрика итератора ---
template <class T>
IEnumerator<T>* LinkedList<T>::GetEnumerator() const {
    return new Enumerator(head);
}
