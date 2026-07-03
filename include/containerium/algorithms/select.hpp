#pragma once

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include <containerium/core/error.hpp>
#include <containerium/containers/heap.hpp>

namespace containerium {

template<typename T, typename Less>
inline bool isEqualBy(const T& a, const T& b, Less less) {
    return !less(a, b) && !less(b, a);
}

template<typename T, typename Less>
inline std::pair<int, int> partition3(std::vector<T>& vec, int left, int right, const T& pivot, Less less) {
    int lt = left;
    int i = left;
    int gt = right;
    while (i <= gt) {
        if (less(vec[i], pivot)) {
            std::swap(vec[lt], vec[i]);
            lt++;
            i++;
        } 
        else if (less(pivot, vec[i])) {
            std::swap(vec[i], vec[gt]);
            gt--;
        } 
        else { i++; }
    }
    return {lt, gt};
}

template<typename T, typename Less>
inline T medianOfSmallRange(std::vector<T>& vec, int left, int right, Less less) {
    std::sort(vec.begin() + left, vec.begin() + right + 1, less);
    int mid = left + (right - left) / 2;
    return vec[mid];
}

template<typename T, typename Less>
inline T bfprtSelectPivot(std::vector<T>& vec, int left, int right, Less less) {
    int n = right - left + 1;
    if (n <= 5) return medianOfSmallRange(vec, left, right, less);

    int groupCount = (n + 4) / 5;
    std::vector<T> medians;
    medians.reserve(static_cast<size_t>(groupCount));

    for (int i = 0; i < groupCount; ++i) {
        int groupLeft = left + i * 5;
        int groupRight = std::min(groupLeft + 4, right);
        medians.push_back(medianOfSmallRange(vec, groupLeft, groupRight, less));
    }

    int k = (groupCount + 1) / 2;
    int l = 0;
    int r = groupCount - 1;
    while (true) {
        if (l == r) return medians[static_cast<size_t>(l)];
        T pivot = bfprtSelectPivot(medians, l, r, less);
        auto [eqL, eqR] = partition3(medians, l, r, pivot, less);
        if (k - 1 < eqL) r = eqL - 1;
        else if (k - 1 > eqR) l = eqR + 1;
        else return pivot;
    }
}

template<typename T, typename Less>
inline std::expected<T, DataStructureError> quickSelectKthInPlace(std::vector<T>& vec, int k1Based, Less less) {
    if (vec.empty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
    if (k1Based <= 0 || k1Based > static_cast<int>(vec.size())) return std::unexpected(DataStructureError::IndexOutOfRange);

    int target = k1Based - 1;
    int left = 0;
    int right = static_cast<int>(vec.size()) - 1;
    while (left <= right) {
        T pivot = vec[static_cast<size_t>(right)];
        auto [eqL, eqR] = partition3(vec, left, right, pivot, less);
        if (target < eqL) right = eqL - 1;
        else if (target > eqR) left = eqR + 1;
        else return pivot;
    }
    return std::unexpected(DataStructureError::ElementNotFound);
}

template<typename T, typename Less>
inline std::expected<T, DataStructureError> bfprtSelectKthInPlace(std::vector<T>& vec, int k1Based, Less less) {
    if (vec.empty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
    if (k1Based <= 0 || k1Based > static_cast<int>(vec.size())) return std::unexpected(DataStructureError::IndexOutOfRange);

    int target = k1Based - 1;
    int left = 0;
    int right = static_cast<int>(vec.size()) - 1;
    while (true) {
        if (left == right) return vec[static_cast<size_t>(left)];
        T pivot = bfprtSelectPivot(vec, left, right, less);
        auto [eqL, eqR] = partition3(vec, left, right, pivot, less);
        if (target < eqL) right = eqL - 1;
        else if (target > eqR) left = eqR + 1;
        else return pivot;
    }
}

template<typename T>
inline std::expected<T, DataStructureError> kthSmallest(std::vector<T> vec, int k1Based) {
    return quickSelectKthInPlace(vec, k1Based, std::less<T>());
}

template<typename T>
inline std::expected<T, DataStructureError> kthLargest(std::vector<T> vec, int k1Based) {
    return quickSelectKthInPlace(vec, k1Based, std::greater<T>());
}

template<typename T>
inline std::expected<T, DataStructureError> kthSmallestBFPRT(std::vector<T> vec, int k1Based) {
    return bfprtSelectKthInPlace(vec, k1Based, std::less<T>());
}

template<typename T>
inline std::expected<std::vector<T>, DataStructureError> topKSmallest(std::vector<T> vec, int k) {
    if (vec.empty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
    if (k <= 0 || k > static_cast<int>(vec.size())) return std::unexpected(DataStructureError::IndexOutOfRange);

    MaxHeap<T> heap;
    auto less = std::less<T>();
    for (int i = 0; i < k; ++i) heap.push(vec[static_cast<size_t>(i)]);
    for (size_t i = static_cast<size_t>(k); i < vec.size(); ++i) {
        auto top = heap.top();
        if (!top.has_value()) return std::unexpected(top.error());
        if (less(vec[i], top.value())) {
            auto popped = heap.pop();
            if (!popped.has_value()) return std::unexpected(popped.error());
            heap.push(vec[i]);
        }
    }

    std::vector<T> result;
    result.reserve(static_cast<size_t>(k));
    while (!heap.isEmpty()) {
        auto v = heap.pop();
        if (!v.has_value()) return std::unexpected(v.error());
        result.push_back(v.value());
    }
    return result;
}

} // namespace containerium
