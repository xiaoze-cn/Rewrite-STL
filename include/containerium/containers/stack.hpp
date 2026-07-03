#pragma once

#include <list>
#include <vector>
#include <containerium/core/error.hpp>

namespace containerium {

template<typename T>
class VectorStack {
public:
    std::vector<T> data;

    VectorStack() = default;
    explicit VectorStack(int capacity) { data.reserve(capacity); }

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

} // namespace containerium
