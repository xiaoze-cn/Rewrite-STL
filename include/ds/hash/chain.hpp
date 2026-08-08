#pragma once

#include <functional>
#include <list>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include <ds/common/error.hpp>
#include <ds/hash/table.hpp>

namespace ds {

template<typename K, typename V, typename Hasher = std::hash<K>>
class ChainedHashMap {
public:
    struct Entry {
        K key;
        V value;
    };

    std::vector<std::list<Entry>> table;
    int capacity;
    int length;
    double maxLoadFactor;
    Hasher hasher;

    explicit ChainedHashMap(int initCapacity = 11, double maxLoad = 0.75, Hasher h = Hasher())
        : capacity(nextPrimeNumber(initCapacity)),
          length(0),
          maxLoadFactor(maxLoad),
          hasher(h) {
        if (initCapacity < 0) throw std::invalid_argument("capacity must not be negative");
        if (maxLoadFactor <= 0.0) throw std::invalid_argument("load factor must be positive");
        table.resize(static_cast<size_t>(capacity));
    }

    int getCapacity() const { return capacity; }

    int getLength() const { return length; }

    bool isEmpty() const { return length == 0; }

    void clear() {
        for (auto& bucket : table) bucket.clear();
        length = 0;
    }

    std::expected<void, DataStructureError> insert(const K& key, const V& value) {
        if (findEntry(key) != nullptr) return std::unexpected(DataStructureError::DuplicateValue);
        if (needRehash(length + 1)) {
            if (capacity > (std::numeric_limits<int>::max() - 1) / 2) return std::unexpected(DataStructureError::MemoryAllocationFailed);
            rehash(nextPrimeNumber(capacity * 2 + 1));
        }
        table[indexOf(key)].push_back(Entry{key, value});
        length++;
        return {};
    }

    std::expected<V, DataStructureError> get(const K& key) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        const Entry* entry = findEntry(key);
        if (entry == nullptr) return std::unexpected(DataStructureError::ElementNotFound);
        return entry->value;
    }

    std::expected<bool, DataStructureError> contains(const K& key) const {
        if (isEmpty()) return false;
        return findEntry(key) != nullptr;
    }

    std::expected<V, DataStructureError> update(const K& key, const V& newValue) {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        Entry* entry = findEntry(key);
        if (entry == nullptr) return std::unexpected(DataStructureError::ElementNotFound);
        V oldValue = entry->value;
        entry->value = newValue;
        return oldValue;
    }

    std::expected<V, DataStructureError> erase(const K& key) {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        auto& bucket = table[indexOf(key)];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->key == key) {
                V oldValue = it->value;
                bucket.erase(it);
                length--;
                return oldValue;
            }
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

protected:
    bool needRehash(int nextLength) const {
        return static_cast<double>(nextLength) / static_cast<double>(capacity) > maxLoadFactor;
    }

    size_t indexOf(const K& key) const {
        return static_cast<size_t>(hasher(key)) % static_cast<size_t>(capacity);
    }

    Entry* findEntry(const K& key) {
        auto& bucket = table[indexOf(key)];
        for (auto& entry : bucket) if (entry.key == key) return &entry;
        return nullptr;
    }

    const Entry* findEntry(const K& key) const {
        const auto& bucket = table[indexOf(key)];
        for (const auto& entry : bucket) if (entry.key == key) return &entry;
        return nullptr;
    }

    void rehash(int newCapacity) {
        std::vector<std::list<Entry>> oldTable = std::move(table);
        capacity = nextPrimeNumber(newCapacity);
        table.clear();
        table.resize(static_cast<size_t>(capacity));
        length = 0;
        for (auto& bucket : oldTable) {
            for (auto& entry : bucket) {
                table[indexOf(entry.key)].push_back(std::move(entry));
                length++;
            }
        }
    }
};

template<typename T, typename Hasher = std::hash<T>>
class ChainedHashSet {
public:
    ChainedHashMap<T, char, Hasher> map;

    explicit ChainedHashSet(int initCapacity = 11, double maxLoad = 0.75, Hasher h = Hasher())
        : map(initCapacity, maxLoad, h) {}

    int getCapacity() const { return map.getCapacity(); }

    int getLength() const { return map.getLength(); }

    bool isEmpty() const { return map.isEmpty(); }

    void clear() { map.clear(); }

    std::expected<void, DataStructureError> insert(const T& value) { return map.insert(value, 1); }

    std::expected<void, DataStructureError> erase(const T& value) {
        auto result = map.erase(value);
        if (!result.has_value()) return std::unexpected(result.error());
        return {};
    }

    std::expected<bool, DataStructureError> contains(const T& value) const { return map.contains(value); }
};

} // namespace ds
