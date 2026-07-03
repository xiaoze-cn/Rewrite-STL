#pragma once

#include <list>
#include <vector>
#include <containerium/core/error.hpp>

namespace containerium {

template<typename T>
class CircularDeque {
public:
    std::vector<T> data;
    int front;
    int rear;
    int capacity;

    CircularDeque() = default;
    explicit CircularDeque(int capacity) : capacity(capacity + 1), front(0), rear(0) { data.resize(capacity); }

    ~CircularDeque() = default;

    bool isEmpty() const { return front == rear; }

    bool isFull() const { return front == (rear + 1) % capacity; }

    int getCapacity() const { return capacity - 1; }

    int getLength() const { return (rear - front + capacity) % capacity; }

    std::expected<void, DataStructureError> push(const T& value) {
        if (isFull()) return std::unexpected(DataStructureError::ContainerIsFull);
        data[rear] = value;
        rear = (rear + 1) % capacity;
        return {};
    }

    std::expected<T, DataStructureError> pop() {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = data[front];
        front = (front + 1) % capacity;
        return value;
    }

    std::expected<T, DataStructureError> getFront() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data[front];
    }

    std::expected<T, DataStructureError> getRear() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data[(rear - 1 + capacity) % capacity];
    }

    void clear() {
        front = rear = 0;
    }
};

template<typename T>
class CircularDequeue {
public:
    std::vector<T> data;
    int front;
    int rear;
    int capacity;
    int length;

    CircularDequeue() = default;
    explicit CircularDequeue(int capacity) : capacity(capacity + 1), front(0), rear(0), length(0) { data.resize(capacity); }

    ~CircularDequeue() = default;

    bool isEmpty() const { return length == 0; }

    bool isFull() const { return length == capacity - 1; }

    int getCapacity() const { return capacity - 1; }

    int getLength() const { return length; }

    std::expected<void, DataStructureError> pushFront(const T& value) {
        if (isFull()) return std::unexpected(DataStructureError::ContainerIsFull);
        front = (front - 1 + capacity) % capacity;
        data[front] = value;
        length++;
        return {};
    }

    std::expected<void, DataStructureError> pushRear(const T& value) {
        if (isFull()) return std::unexpected(DataStructureError::ContainerIsFull);
        data[rear] = value;
        rear = (rear + 1) % capacity;
        length++;
        return {};
    }

    std::expected<T, DataStructureError> popFront() {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = data[front];
        front = (front + 1) % capacity;
        length--;
        return value;
    }

    std::expected<T, DataStructureError> popRear() {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        rear = (rear - 1 + capacity) % capacity;
        T value = data[rear];
        length--;
        return value;
    }

    std::expected<T, DataStructureError> getFront() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data[front];
    }

    std::expected<T, DataStructureError> getRear() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data[(rear - 1 + capacity) % capacity];
    }

    void clear() {
        front = rear = length = 0;
    }
};

template<typename T>
class LinkQueue {
public:
    std::list<T> data;

    LinkQueue() = default;

    ~LinkQueue() = default;

    bool isEmpty() const { return data.empty(); }

    int getLength() const { return data.size(); }

    void push(const T& value) {
        data.push_back(value);
    }

    std::expected<T, DataStructureError> pop() {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = data.front();
        data.pop_front();
        return value;
    }

    std::expected<T, DataStructureError> getFront() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data.front();
    }

    std::expected<T, DataStructureError> getRear() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data.back();
    }

    void clear() {
        data.clear();
    }
};

template<typename T>
class ListDequeue {
public:
    std::list<T> data;

    ListDequeue() = default;

    ~ListDequeue() = default;

    bool isEmpty() const { return data.empty(); }

    int getLength() const { return data.size(); }

    void pushFront(const T& value) {
        data.push_front(value);
    }

    void pushRear(const T& value) {
        data.push_back(value);
    }

    std::expected<T, DataStructureError> popFront() {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = data.front();
        data.pop_front();
        return value;
    }

    std::expected<T, DataStructureError> popRear() {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = data.back();
        data.pop_back();
        return value;
    }

    std::expected<T, DataStructureError> getFront() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data.front();
    }

    std::expected<T, DataStructureError> getRear() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data.back();
    }

    void clear() {
        data.clear();
    }
};

} // namespace containerium
