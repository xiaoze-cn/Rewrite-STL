#pragma once

#include <algorithm>
#include <vector>
#include <containerium/core/error.hpp>

namespace containerium {

template<typename T>
class UnionFindSet {
public:
    std::vector<T> elements;
    std::vector<size_t> parent;
    std::vector<size_t> deepth;

    UnionFindSet(const std::vector<T>& elems) {
        elements = elems;
        parent.resize(elems.size());
        for (size_t i = 0; i < elems.size(); i++) parent[i] = i;
        deepth.resize(elems.size(), 1);
    }

    ~UnionFindSet() {
        elements.clear();
        parent.clear();
        deepth.clear();
    }

    std::expected<T, DataStructureError> find(T elem) {
        auto it = std::find(elements.begin(), elements.end(), elem);
        if (it == elements.end()) return std::unexpected(DataStructureError::ElementNotFound);
        size_t index = std::distance(elements.begin(), it);
        size_t root = index;
        while (parent[root] != root) root = parent[root];
        while (parent[index] != root) {
            size_t next = parent[index];
            parent[index] = root;
            index = next;
        }
        return elements[root];
    }

    std::expected<void, DataStructureError> unionSet(T set1, T set2) {
        CONTAINERIUM_TRY(root1, find(set1));
        CONTAINERIUM_TRY(root2, find(set2));
        if (root1 == root2) return {};
        auto it1 = std::find(elements.begin(), elements.end(), root1);
        auto it2 = std::find(elements.begin(), elements.end(), root2);
        size_t index1 = std::distance(elements.begin(), it1);
        size_t index2 = std::distance(elements.begin(), it2);
        if (deepth[index1] > deepth[index2]) parent[index2] = index1;
        else if (deepth[index1] < deepth[index2]) parent[index1] = index2;
        else {
            parent[index2] = index1;
            deepth[index1]++;
        }
        return {};
    }

    std::expected<void, DataStructureError> isConnected(T elem1, T elem2) {
        CONTAINERIUM_TRY(root1, find(elem1));
        CONTAINERIUM_TRY(root2, find(elem2));
        if (root1 == root2) return {};
        else return std::unexpected(DataStructureError::InvalidOperation);
    }
};

} // namespace containerium
