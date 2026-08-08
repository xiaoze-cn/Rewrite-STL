#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <ds/common/error.hpp>

namespace ds {

template<typename T>
class UnionFindSet {
public:
    std::vector<T> elements;
    std::vector<size_t> parent;
    std::vector<size_t> rank;

    UnionFindSet(const std::vector<T>& elems) {
        for (size_t i = 0; i < elems.size(); ++i) {
            if (std::find(elems.begin(), elems.begin() + static_cast<std::ptrdiff_t>(i), elems[i]) != elems.begin() + static_cast<std::ptrdiff_t>(i)) {
                throw std::invalid_argument("union-find elements must be unique");
            }
        }
        elements = elems;
        parent.resize(elems.size());
        for (size_t i = 0; i < elems.size(); i++) parent[i] = i;
        rank.resize(elems.size(), 0);
    }

    ~UnionFindSet() = default;

    std::expected<T, DataStructureError> find(const T& elem) {
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

    std::expected<void, DataStructureError> unionSet(const T& set1, const T& set2) {
        DS_TRY(root1, find(set1));
        DS_TRY(root2, find(set2));
        if (root1 == root2) return {};
        auto it1 = std::find(elements.begin(), elements.end(), root1);
        auto it2 = std::find(elements.begin(), elements.end(), root2);
        size_t index1 = std::distance(elements.begin(), it1);
        size_t index2 = std::distance(elements.begin(), it2);
        if (rank[index1] > rank[index2]) parent[index2] = index1;
        else if (rank[index1] < rank[index2]) parent[index1] = index2;
        else {
            parent[index2] = index1;
            rank[index1]++;
        }
        return {};
    }

    std::expected<bool, DataStructureError> isConnected(const T& elem1, const T& elem2) {
        DS_TRY(root1, find(elem1));
        DS_TRY(root2, find(elem2));
        return root1 == root2;
    }
};

} // namespace ds
