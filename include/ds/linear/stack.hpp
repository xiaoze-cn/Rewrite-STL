#pragma once

#include <list>
#include <stdexcept>
#include <vector>
#include <ds/common/error.hpp>

namespace ds {

template<typename T>
class VectorStack {
public:
    std::vector<T> data;

    VectorStack() = default;
    explicit VectorStack(int capacity) {
        if (capacity < 0) throw std::invalid_argument("capacity must not be negative");
        data.reserve(static_cast<size_t>(capacity));
    }

    ~VectorStack() = default;

    bool isEmpty() const { return data.empty(); }

    std::expected<T, DataStructureError> top() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data.back();
    }

    int getSize() const { return data.size(); }

    void push(const T& value) {
        data.push_back(value);
    }

    std::expected<T, DataStructureError> pop() {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = data.back();
        data.pop_back();
        return value;
    }
    
    void clear() {
        data.clear();
    }
};

template<typename T>
class ListStack {
public:
    std::list<T> data;

    ListStack() = default;

    ~ListStack() = default;

    bool isEmpty() const { return data.empty(); }

    std::expected<T, DataStructureError> top() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data.back();
    }

    int getSize() const { return data.size(); }

    void push(const T& value) {
        data.push_back(value);
    }

    std::expected<T, DataStructureError> pop() {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T value = data.back();
        data.pop_back();
        return value;
    }

     void clear() {
        data.clear();
    }
};

} // namespace ds
