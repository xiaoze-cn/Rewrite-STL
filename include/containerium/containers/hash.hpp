#pragma once

#include <functional>
#include <optional>
#include <utility>
#include <vector>
#include <containerium/core/error.hpp>

namespace containerium {

inline bool isPrimeNumber(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) { if (n % i == 0 || n % (i + 2) == 0) return false; }
    return true;
}

inline int nextPrimeNumber(int n) {
    if (n <= 2) return 2;
    int candidate = (n % 2 == 0) ? (n + 1) : n;
    while (!isPrimeNumber(candidate)) candidate += 2;
    return candidate;
}

template<typename K, typename V, typename Hasher = std::hash<K>>
class HashMap {
public:
    struct Entry {
        std::optional<std::pair<K, V>> kv;
        bool isDeleted;
    };

    std::vector<Entry> table;
    int capacity;
    int length;
    double maxLoadFactor;
    Hasher hasher;

    explicit HashMap(int initCapacity = 11, double maxLoad = 0.75, Hasher h = Hasher())
        : capacity(nextPrimeNumber(initCapacity)),
          length(0),
          maxLoadFactor(maxLoad),
          hasher(h) { table.resize(static_cast<size_t>(capacity), Entry{std::nullopt, false}); }

    int getCapacity() const { return capacity; }

    int getLength() const { return length; }

    bool isEmpty() const { return length == 0; }

    void clear() {
        for (auto& e : table) {
            e.kv = std::nullopt;
            e.isDeleted = false;
        }
        length = 0;
    }

    std::expected<void, DataStructureError> insert(const K& key, const V& value) {
        if (maxLoadFactor <= 0.0 || maxLoadFactor >= 1.0) return std::unexpected(DataStructureError::InvalidArgument);
        if (needRehash(length + 1)) rehash(nextPrimeNumber(capacity * 2 + 1));

        int firstDeleted = -1;
        size_t start = indexOf(key);
        for (int i = 0; i < capacity; ++i) {
            int idx = static_cast<int>((start + static_cast<size_t>(i)) % static_cast<size_t>(capacity));
            Entry& e = table[static_cast<size_t>(idx)];
            if (e.kv.has_value()) {
                if (e.kv->first == key) return std::unexpected(DataStructureError::DuplicateValue);
                continue;
            }
            if (e.isDeleted) {
                if (firstDeleted == -1) firstDeleted = idx;
                continue;
            }
            int target = (firstDeleted != -1) ? firstDeleted : idx;
            table[static_cast<size_t>(target)].kv = std::make_pair(key, value);
            table[static_cast<size_t>(target)].isDeleted = false;
            length++;
            return {};
        }
        if (firstDeleted != -1) {
            table[static_cast<size_t>(firstDeleted)].kv = std::make_pair(key, value);
            table[static_cast<size_t>(firstDeleted)].isDeleted = false;
            length++;
            return {};
        }
        rehash(nextPrimeNumber(capacity * 2 + 1));
        return insert(key, value);
    }

    std::expected<V, DataStructureError> get(const K& key) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        int idx = findIndex(key);
        if (idx < 0) return std::unexpected(DataStructureError::ElementNotFound);
        return table[static_cast<size_t>(idx)].kv->second;
    }

    std::expected<bool, DataStructureError> contains(const K& key) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return findIndex(key) >= 0;
    }

    std::expected<V, DataStructureError> update(const K& key, const V& newValue) {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        int idx = findIndex(key);
        if (idx < 0) return std::unexpected(DataStructureError::ElementNotFound);
        Entry& e = table[static_cast<size_t>(idx)];
        V oldValue = e.kv->second;
        e.kv->second = newValue;
        return oldValue;
    }

    std::expected<V, DataStructureError> erase(const K& key) {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        int idx = findIndex(key);
        if (idx < 0) return std::unexpected(DataStructureError::ElementNotFound);
        Entry& e = table[static_cast<size_t>(idx)];
        V oldValue = e.kv->second;
        e.kv = std::nullopt;
        e.isDeleted = true;
        length--;
        return oldValue;
    }

protected:
    bool needRehash(int nextLength) const { return static_cast<double>(nextLength) / static_cast<double>(capacity) > maxLoadFactor; }

    size_t indexOf(const K& key) const { return static_cast<size_t>(hasher(key)) % static_cast<size_t>(capacity); }

    int findIndex(const K& key) const {
        size_t start = indexOf(key);
        for (int i = 0; i < capacity; ++i) {
            int idx = static_cast<int>((start + static_cast<size_t>(i)) % static_cast<size_t>(capacity));
            const Entry& e = table[static_cast<size_t>(idx)];
            if (e.kv.has_value()) {
                if (e.kv->first == key) return idx;
                continue;
            }
            if (!e.isDeleted) return -1;
        }
        return -1;
    }

    void rehash(int newCapacity) {
        newCapacity = nextPrimeNumber(newCapacity);
        std::vector<Entry> oldTable = std::move(table);
        int oldCapacity = capacity;
        capacity = newCapacity;
        table.clear();
        table.resize(static_cast<size_t>(capacity), Entry{std::nullopt, false});
        length = 0;
        for (int i = 0; i < oldCapacity; ++i) {
            Entry& e = oldTable[static_cast<size_t>(i)];
            if (e.kv.has_value()) { insert(e.kv->first, e.kv->second); }
        }
    }
};

template<typename T, typename Hasher = std::hash<T>>
class HashSet {
public:
    HashMap<T, char, Hasher> map;

    explicit HashSet(int initCapacity = 11, double maxLoad = 0.75, Hasher h = Hasher()) : map(initCapacity, maxLoad, h) {}

    int getCapacity() const { return map.getCapacity(); }

    int getLength() const { return map.getLength(); }

    bool isEmpty() const { return map.isEmpty(); }

    void clear() { map.clear(); }

    std::expected<void, DataStructureError> insert(const T& value) { return map.insert(value, 1); }

    std::expected<void, DataStructureError> erase(const T& value) {
        auto res = map.erase(value);
        if (!res.has_value()) return std::unexpected(res.error());
        return {};
    }

    std::expected<bool, DataStructureError> contains(const T& value) const { return map.contains(value); }
};

} // namespace containerium
