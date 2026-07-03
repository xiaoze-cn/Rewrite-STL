#pragma once

#include <functional>
#include <utility>
#include <vector>
#include <containerium/core/error.hpp>

namespace containerium {

template<typename T, typename Compare>
class BinaryHeap {
public:
    std::vector<T> data;
    Compare comp;

    BinaryHeap() = default;

    explicit BinaryHeap(int capacity, Compare compare = Compare())
        : comp(compare) {
        if (capacity > 0) data.reserve(static_cast<size_t>(capacity));
    }

    explicit BinaryHeap(std::vector<T> values, Compare compare = Compare())
        : data(std::move(values)), comp(compare) {
        heapify();
    }

    bool isEmpty() const { return data.empty(); }

    int size() const { return static_cast<int>(data.size()); }

    void clear() { data.clear(); }

    std::expected<T, DataStructureError> top() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return data.front();
    }

    std::expected<void, DataStructureError> push(const T& value) {
        data.push_back(value);
        siftUp(static_cast<int>(data.size()) - 1);
        return {};
    }

    std::expected<T, DataStructureError> pop() {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        T result = data.front();
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) siftDown(0);
        return result;
    }

    void heapify() {
        for (int i = parentIndex(size() - 1); i >= 0; --i) siftDown(i);
    }

protected:
    static int parentIndex(int i) { return (i - 1) / 2; }
    static int leftIndex(int i) { return 2 * i + 1; }
    static int rightIndex(int i) { return 2 * i + 2; }

    void siftUp(int i) {
        while (i > 0) {
            int p = parentIndex(i);
            if (!comp(data[i], data[p])) break;
            std::swap(data[i], data[p]);
            i = p;
        }
    }

    void siftDown(int i) {
        int n = size();
        while (true) {
            int l = leftIndex(i);
            int r = rightIndex(i);
            int best = i;

            if (l < n && comp(data[l], data[best])) best = l;
            if (r < n && comp(data[r], data[best])) best = r;
            if (best == i) break;

            std::swap(data[i], data[best]);
            i = best;
        }
    }
};

template<typename T>
using MinHeap = BinaryHeap<T, std::less<T>>;

template<typename T>
using MaxHeap = BinaryHeap<T, std::greater<T>>;

template<typename T, typename Compare>
class PriorityQueue {
public:
    BinaryHeap<T, Compare> heap;

    PriorityQueue() = default;

    explicit PriorityQueue(int capacity, Compare comp = Compare())
        : heap(capacity, comp) {}

    bool isEmpty() const { return heap.isEmpty(); }

    int size() const { return heap.size(); }

    void clear() { heap.clear(); }

    std::expected<T, DataStructureError> top() const { return heap.top(); }

    std::expected<void, DataStructureError> push(const T& value) { return heap.push(value); }

    std::expected<T, DataStructureError> pop() { return heap.pop(); }
};

template<typename T>
using MinPriorityQueue = PriorityQueue<T, std::less<T>>;

template<typename T>
using MaxPriorityQueue = PriorityQueue<T, std::greater<T>>;

} // namespace containerium
