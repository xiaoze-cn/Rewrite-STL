#pragma once

#include <cstddef>
#include <functional>
#include <vector>

#include <ds/common/error.hpp>

namespace ds {

template<typename T, typename Equal = std::equal_to<T>>
inline std::expected<int, DataStructureError> linearSearch(const std::vector<T>& vec, const T& value, Equal equal = Equal()) {
    if (vec.empty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
    for (size_t i = 0; i < vec.size(); i++) {
        if (equal(vec[i], value)) return static_cast<int>(i);
    }
    return std::unexpected(DataStructureError::ElementNotFound);
}

template<typename T, typename Less = std::less<T>>
inline std::expected<int, DataStructureError> lowerBound(const std::vector<T>& vec, const T& value, Less less = Less()) {
    if (vec.empty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
    size_t left = 0;
    size_t right = vec.size();
    while (left < right) {
        size_t middle = left + (right - left) / 2;
        if (less(vec[middle], value)) left = middle + 1;
        else right = middle;
    }
    return static_cast<int>(left);
}

template<typename T, typename Less = std::less<T>>
inline std::expected<int, DataStructureError> upperBound(const std::vector<T>& vec, const T& value, Less less = Less()) {
    if (vec.empty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
    size_t left = 0;
    size_t right = vec.size();
    while (left < right) {
        size_t middle = left + (right - left) / 2;
        if (!less(value, vec[middle])) left = middle + 1;
        else right = middle;
    }
    return static_cast<int>(left);
}

template<typename T, typename Less = std::less<T>>
inline std::expected<int, DataStructureError> binarySearch(const std::vector<T>& vec, const T& value, Less less = Less()) {
    DS_TRY(index, lowerBound(vec, value, less));
    if (index < static_cast<int>(vec.size()) && !less(value, vec[static_cast<size_t>(index)])) return index;
    return std::unexpected(DataStructureError::ElementNotFound);
}

} // namespace ds
