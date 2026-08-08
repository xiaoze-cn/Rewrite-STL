#pragma once

#include <array>
#include <limits>
#include <stdexcept>
#include <utility>

#include <ds/common/error.hpp>

namespace ds {

template<typename T>
class SequentialList {
public:
    T* data;
    int capacity;
    int length;

    explicit SequentialList(int capacity) : data(nullptr), capacity(capacity), length(0) {
        if (capacity < 0) throw std::invalid_argument("capacity must not be negative");
        data = new T[static_cast<size_t>(capacity)];
    }

    ~SequentialList() { delete[] data; }

    SequentialList(const SequentialList&) = delete;
    SequentialList& operator=(const SequentialList&) = delete;

    SequentialList(SequentialList&& other) noexcept
        : data(std::exchange(other.data, nullptr)),
          capacity(std::exchange(other.capacity, 0)),
          length(std::exchange(other.length, 0)) {}

    SequentialList& operator=(SequentialList&& other) noexcept {
        if (this == &other) return *this;
        delete[] data;
        data = std::exchange(other.data, nullptr);
        capacity = std::exchange(other.capacity, 0);
        length = std::exchange(other.length, 0);
        return *this;
    }

    int getCapacity() const { return capacity; }
    
    int getLength() const { return length; }
    
    bool isEmpty() const { return length == 0; }

    bool isFull() const { return length == capacity; }

    std::expected<void, DataStructureError> insert(int index, const T& value) {
        if (index < 0 || index > length) return std::unexpected(DataStructureError::IndexOutOfRange);
        if (isFull()) return std::unexpected(DataStructureError::ContainerIsFull);
        for (int i = length; i > index; i--) data[i] = data[i - 1];
        data[index] = value;
        length++;
        return {};
    }
    
    std::expected<void, DataStructureError> pushFront(const T& value) {
        return insert(0, value);
    }

    std::expected<void, DataStructureError> pushBack(const T& value) {
        return insert(length, value);
    }

    std::expected<T, DataStructureError> erase(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = data[index];
        for (int i = index;i < length - 1;i++) data[i] = data[i + 1];
        length--;
        return value;
    }

    std::expected<T, DataStructureError> popFront() {
        return erase(0);
    }

    std::expected<T, DataStructureError> popBack() {
        return erase(length - 1);
    }

    std::expected<T, DataStructureError> remove(const T& value) {
        for (int i = 0;i < length;i++) {
            if (data[i] == value) {
                for (int j = i;j < length - 1;j++) data[j] = data[j + 1];
                length--;
                return value;
            }
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> get(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data[index];
    }

    std::expected<int, DataStructureError> find(const T& value) {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        for (int i = 0;i < length;i++) {
            if (data[i] == value) return i;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> update(int index, const T& newValue) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = data[index];
        data[index] = newValue;
        return value;
    }

    int reverse(int addCapacity) {
        if (addCapacity < 0 || capacity > std::numeric_limits<int>::max() - addCapacity) {
            throw std::invalid_argument("capacity increment is invalid");
        }
        T* newData = new T[capacity + addCapacity];
        for (int i = 0;i < length;i++) newData[i] = data[i];
        delete[] data;
        data = newData;
        capacity += addCapacity;
        return capacity;
    }

    void clear() {
        length = 0;
    }
};

template<typename T>
class LinkList {
public:
    struct Node {
        T data;
        Node* next;
    };
    int length;

protected:
    Node* head;

public:
    LinkList(): length(0) { head = new Node{ T(), nullptr }; }

    ~LinkList() {
        clear();
        delete head;
    }

    LinkList(const LinkList&) = delete;
    LinkList& operator=(const LinkList&) = delete;
    LinkList(LinkList&&) = delete;
    LinkList& operator=(LinkList&&) = delete;

    int getLength() const { return length; }

    bool isEmpty() const { return length == 0; }

    std::expected<void, DataStructureError> insert(int index, const T& value) {
        if (index < 0 || index > length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* prevNode = head;
        for (int i = 0;i < index;i++) prevNode = prevNode->next;
        Node* newNode = new Node{ value, prevNode->next };
        prevNode->next = newNode;
        length++;
        return {};
    }

    std::expected<void, DataStructureError> pushFront(const T& value) {
        return insert(0, value);
    }

    std::expected<void, DataStructureError> pushBack(const T& value) {
        return insert(length, value);
    }

    std::expected<T, DataStructureError> erase(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* prevNode = head;
        for (int i = 0;i < index;i++) prevNode = prevNode->next;
        Node* deleteNode = prevNode->next;
        prevNode->next = deleteNode->next;
        T value = deleteNode->data;
        delete deleteNode;
        length--;
        return value;
    }

    std::expected<T, DataStructureError> popFront() {
        return erase(0);
    }

    std::expected<T, DataStructureError> popBack() {
        return erase(length - 1);
    }

    std::expected<void, DataStructureError> remove(const T& value) {
        Node* prevNode = head;
        for (int i = 0;i < length;i++) {
            if (prevNode->next->data == value) {
                Node* deleteNode = prevNode->next;
                prevNode->next = deleteNode->next;
                delete deleteNode;
                length--;
                return {};
            }
            prevNode = prevNode->next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> get(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* p = head->next;
        for (int i = 0;i < index;i++) p = p->next;
        return p->data;
    }

    std::expected<int, DataStructureError> find(const T& value) {
        Node* p = head->next;
        for (int i = 0;i < length;i++) {
            if (p->data == value) return i;
            p = p->next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> update(int index, const T& newValue) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* p = head->next;
        for (int i = 0;i < index;i++) p = p->next;
        T oldValue = p->data;
        p->data = newValue;
        return oldValue;
    }

    void clear() {
        Node* node = head->next;
        for (int i = 0;i < length;i++) {
            Node* deleteNode = node;
            node = node->next;
            delete deleteNode;
        }
        head->next = nullptr;
        length = 0;
    }
};

template<typename T>
class DoubleLinkList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
    };
    int length;

protected:
    Node* head;

public:
    DoubleLinkList(): length(0) {
        head = new Node{ T(), nullptr, nullptr };
    }

    ~DoubleLinkList() {
        clear();
        delete head;
    }

    DoubleLinkList(const DoubleLinkList&) = delete;
    DoubleLinkList& operator=(const DoubleLinkList&) = delete;
    DoubleLinkList(DoubleLinkList&&) = delete;
    DoubleLinkList& operator=(DoubleLinkList&&) = delete;

    int getLength() const { return length; }

    bool isEmpty() const { return length == 0; }

    std::expected<void, DataStructureError> insert(int index, const T& value) {
        if (index < 0 || index > length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* prevNode = head;
        for (int i = 0;i < index;i++) prevNode = prevNode->next;
        Node* newNode = new Node{ value, prevNode, prevNode->next };
        if (prevNode->next != nullptr) {
            prevNode->next->prev = newNode;
        }
        prevNode->next = newNode;
        length++;
        return {};
    }
    
    std::expected<void, DataStructureError> pushFront(const T& value) {
        return insert(0, value);
    }

    std::expected<void, DataStructureError> pushBack(const T& value) {
        return insert(length, value);
    }

    std::expected<T, DataStructureError> erase(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* deleteNode = head->next;
        for (int i = 0;i < index;i++) deleteNode = deleteNode->next;
        T value = deleteNode->data;
        deleteNode->prev->next = deleteNode->next;
        if (deleteNode->next != nullptr) deleteNode->next->prev = deleteNode->prev;
        delete deleteNode;
        length--;
        return value;
    }
    
    std::expected<T, DataStructureError> popFront() {
        return erase(0);
    }

    std::expected<T, DataStructureError> popBack() {
        return erase(length - 1);
    }

    std::expected<void, DataStructureError> remove(const T& value) {
        Node* deleteNode = head->next;
        for (int i = 0;i < length;i++) {
            if (deleteNode->data == value) {
                deleteNode->prev->next = deleteNode->next;
                if (deleteNode->next != nullptr) deleteNode->next->prev = deleteNode->prev;
                delete deleteNode;
                length--;
                return {};
            }
            deleteNode = deleteNode->next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> get(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* node = head->next;
        for (int i = 0;i < index;i++) node = node->next;
        return node->data;
    }

    std::expected<int, DataStructureError> find(const T& value) {
        Node* node = head->next;
        for (int i = 0;i < length;i++) {
            if (node->data == value) return i;
            node = node->next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> update(int index, const T& newValue) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* node = head->next;
        for (int i = 0;i < index;i++) node = node->next;
        T oldValue = node->data;
        node->data = newValue;
        return oldValue;
    }

    void clear() {
        Node* node = head->next;
        for (int i = 0;i < length;i++) {
            Node* nextNode = node->next;
            delete node;
            node = nextNode;
        }
        head->next = nullptr;
        length = 0;
    }
};

template<typename T>
class CircularLinkList {
public:
    struct Node {
        T data;
        Node* next;
    };
    int length;

protected:
    Node* head;
    Node* tail;

public: 
    CircularLinkList(): length(0) {
        head = new Node{ T(), nullptr };
        head->next = head;
        tail = head;
    }

    ~CircularLinkList() {
        clear();
        delete head;
    }

    CircularLinkList(const CircularLinkList&) = delete;
    CircularLinkList& operator=(const CircularLinkList&) = delete;
    CircularLinkList(CircularLinkList&&) = delete;
    CircularLinkList& operator=(CircularLinkList&&) = delete;

    int getLength() const { return length; }

    bool isEmpty() const { return length == 0; }

    std::expected<void, DataStructureError> insert(int index, const T& value) {
        if (index < 0 || index > length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* prevNode = head;
        for (int i = 0;i < index;i++) prevNode = prevNode->next;
        Node* newNode = new Node{ value, prevNode->next };
        prevNode->next = newNode;
        if (index == length) {
            tail = newNode;
            tail->next = head;
        }
        length++;
        return {};
    }

    std::expected<void, DataStructureError> pushFront(const T& value) {
        return insert(0, value);
    }

    std::expected<void, DataStructureError> pushBack(const T& value) {
        return insert(length, value);
    }

    std::expected<T, DataStructureError> erase(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* prevNode = head;
        for (int i = 0;i < index;i++) prevNode = prevNode->next;
        Node* deleteNode = prevNode->next;
        prevNode->next = deleteNode->next;
        T value = deleteNode->data;
        delete deleteNode;
        if (index == length - 1) {
            tail = prevNode;
            tail->next = head;
        }
        length--;
        return value;
    }

    std::expected<T, DataStructureError> popFront() {
        return erase(0);
    }

    std::expected<T, DataStructureError> popBack() {
        return erase(length - 1);
    }

    std::expected<void, DataStructureError> remove(const T& value) {
        Node* prevNode = head;
        for (int i = 0;i < length;i++) {
            if (prevNode->next->data == value) {
                Node* deleteNode = prevNode->next;
                prevNode->next = deleteNode->next;
                delete deleteNode;
                if (i == length - 1) {
                    tail = prevNode;
                    tail->next = head;
                }
                length--;
                return {};
            }
            prevNode = prevNode->next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> get(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* p = head->next;
        for (int i = 0;i < index;i++) p = p->next;
        return p->data;
    }

    std::expected<int, DataStructureError> find(const T& value) {
        Node* p = head->next;
        for (int i = 0;i < length;i++) {
            if (p->data == value) return i;
            p = p->next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> update(int index, const T& newValue) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* p = head->next;
        for (int i = 0;i < index;i++) p = p->next;
        T oldValue = p->data;
        p->data = newValue;
        return oldValue;
    }

    void clear() {
        Node* node = head->next;
        for (int i = 0;i < length;i++) {
            Node* deleteNode = node;
            node = node->next;
            delete deleteNode;
        }
        head->next = head;
        tail = head;
        length = 0;
    }
};

template<typename T>
class CircularDoubleLinkList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
    };
    int length;

protected:
    Node* head;
    Node* tail;

public:
    CircularDoubleLinkList() : length(0) {
        head = new Node{ T(), nullptr, nullptr };
        head->next = head;
        head->prev = head;
        tail = head;
    }

    ~CircularDoubleLinkList() {
        clear();
        delete head;
    }

    CircularDoubleLinkList(const CircularDoubleLinkList&) = delete;
    CircularDoubleLinkList& operator=(const CircularDoubleLinkList&) = delete;
    CircularDoubleLinkList(CircularDoubleLinkList&&) = delete;
    CircularDoubleLinkList& operator=(CircularDoubleLinkList&&) = delete;

    int getLength() const { return length; }

    bool isEmpty() const { return length == 0; }

    std::expected<void, DataStructureError> insert(int index, const T& value) {
        if (index < 0 || index > length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* prevNode = head;
        for (int i = 0;i < index;i++) prevNode = prevNode->next;
        Node* newNode = new Node{ value, prevNode, prevNode->next };
        prevNode->next->prev = newNode;
        prevNode->next = newNode;
        if (index == length) {
            tail = newNode;
            tail->next = head;
            head->prev = tail;
        }
        length++;
        return {};
    }

    std::expected<void, DataStructureError> pushFront(const T& value) {
        return insert(0, value);
    }

    std::expected<void, DataStructureError> pushBack(const T& value) {
        return insert(length, value);
    }

    std::expected<T, DataStructureError> erase(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* deleteNode = head->next;
        for (int i = 0;i < index;i++) deleteNode = deleteNode->next;
        T value = deleteNode->data;
        deleteNode->prev->next = deleteNode->next;
        deleteNode->next->prev = deleteNode->prev;
        if (index == length - 1) {
            tail = deleteNode->prev;
            tail->next = head;
            head->prev = tail;
        }
        delete deleteNode;
        length--;
        return value;
    }

    std::expected<T, DataStructureError> popFront() {
        return erase(0);
    }

    std::expected<T, DataStructureError> popBack() {
        return erase(length - 1);
    }

    std::expected<void, DataStructureError> remove(const T& value) {
        Node* deleteNode = head->next;
        for (int i = 0;i < length;i++) {
            if (deleteNode->data == value) {
                deleteNode->prev->next = deleteNode->next;
                deleteNode->next->prev = deleteNode->prev;
                if (i == length - 1) {
                    tail = deleteNode->prev;
                    tail->next = head;
                    head->prev = tail;
                }
                delete deleteNode;
                length--;
                return {};
            }
            deleteNode = deleteNode->next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> get(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* node = head->next;
        for (int i = 0;i < index;i++) node = node->next;
        return node->data;
    }

    std::expected<int, DataStructureError> find(const T& value) {
        Node* node = head->next;
        for (int i = 0;i < length;i++) {
            if (node->data == value) return i;
            node = node->next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> update(int index, const T& newValue) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node* node = head->next;
        for (int i = 0;i < index;i++) node = node->next;
        T oldValue = node->data;
        node->data = newValue;
        return oldValue;
    }

    void clear() {
        Node* node = head->next;
        for (int i = 0;i < length;i++) {
            Node* nextNode = node->next;
            delete node;
            node = nextNode;
        }
        head->next = head;
        head->prev = head;
        tail = head;
        length = 0;
    }
};

template<typename T, int MAXSIZE>
class StaticLinkList {
public:
    struct Node {
        T data;
        int next;
        bool used;
    };

    std::array<Node, static_cast<size_t>(MAXSIZE)> nodes;
    int length;

protected:
    int head;
    int freeHead;

    int nodeAt(int index) const {
        int current = head;
        for (int i = 0; i < index; i++) current = nodes[static_cast<size_t>(current)].next;
        return current;
    }

    int allocateNode(const T& value) {
        int index = freeHead;
        freeHead = nodes[static_cast<size_t>(index)].next;
        nodes[static_cast<size_t>(index)] = Node{value, -1, true};
        return index;
    }

    void releaseNode(int index) {
        nodes[static_cast<size_t>(index)].used = false;
        nodes[static_cast<size_t>(index)].next = freeHead;
        freeHead = index;
    }

public:
    static_assert(MAXSIZE > 0, "maximum size must be positive");

    StaticLinkList() : nodes{}, length(0), head(-1), freeHead(0) {
        for (int i = 0; i < MAXSIZE; i++) {
            nodes[static_cast<size_t>(i)].next = i + 1 < MAXSIZE ? i + 1 : -1;
            nodes[static_cast<size_t>(i)].used = false;
        }
    }

    int getCapacity() const { return MAXSIZE; }

    int getLength() const { return length; }

    bool isEmpty() const { return length == 0; }

    bool isFull() const { return length == MAXSIZE; }

    std::expected<void, DataStructureError> insert(int index, const T& value) {
        if (index < 0 || index > length) return std::unexpected(DataStructureError::IndexOutOfRange);
        if (isFull()) return std::unexpected(DataStructureError::ContainerIsFull);
        int newIndex = allocateNode(value);
        if (index == 0) {
            nodes[static_cast<size_t>(newIndex)].next = head;
            head = newIndex;
        }
        else {
            int previous = nodeAt(index - 1);
            nodes[static_cast<size_t>(newIndex)].next = nodes[static_cast<size_t>(previous)].next;
            nodes[static_cast<size_t>(previous)].next = newIndex;
        }
        length++;
        return {};
    }

    std::expected<void, DataStructureError> pushFront(const T& value) {
        return insert(0, value);
    }

    std::expected<void, DataStructureError> pushBack(const T& value) {
        return insert(length, value);
    }

    std::expected<T, DataStructureError> erase(int index) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        int deleteIndex;
        if (index == 0) {
            deleteIndex = head;
            head = nodes[static_cast<size_t>(head)].next;
        }
        else {
            int previous = nodeAt(index - 1);
            deleteIndex = nodes[static_cast<size_t>(previous)].next;
            nodes[static_cast<size_t>(previous)].next = nodes[static_cast<size_t>(deleteIndex)].next;
        }
        T value = nodes[static_cast<size_t>(deleteIndex)].data;
        releaseNode(deleteIndex);
        length--;
        return value;
    }

    std::expected<T, DataStructureError> popFront() {
        return erase(0);
    }

    std::expected<T, DataStructureError> popBack() {
        return erase(length - 1);
    }

    std::expected<void, DataStructureError> remove(const T& value) {
        int current = head;
        int previous = -1;
        while (current != -1) {
            if (nodes[static_cast<size_t>(current)].data == value) {
                if (previous == -1) head = nodes[static_cast<size_t>(current)].next;
                else nodes[static_cast<size_t>(previous)].next = nodes[static_cast<size_t>(current)].next;
                releaseNode(current);
                length--;
                return {};
            }
            previous = current;
            current = nodes[static_cast<size_t>(current)].next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> get(int index) const {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        return nodes[static_cast<size_t>(nodeAt(index))].data;
    }

    std::expected<int, DataStructureError> find(const T& value) const {
        int current = head;
        for (int i = 0; i < length; i++) {
            if (nodes[static_cast<size_t>(current)].data == value) return i;
            current = nodes[static_cast<size_t>(current)].next;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::expected<T, DataStructureError> update(int index, const T& newValue) {
        if (index < 0 || index >= length) return std::unexpected(DataStructureError::IndexOutOfRange);
        Node& node = nodes[static_cast<size_t>(nodeAt(index))];
        T oldValue = node.data;
        node.data = newValue;
        return oldValue;
    }

    void clear() {
        head = -1;
        freeHead = 0;
        length = 0;
        for (int i = 0; i < MAXSIZE; i++) {
            nodes[static_cast<size_t>(i)].next = i + 1 < MAXSIZE ? i + 1 : -1;
            nodes[static_cast<size_t>(i)].used = false;
        }
    }

};

} // namespace ds
