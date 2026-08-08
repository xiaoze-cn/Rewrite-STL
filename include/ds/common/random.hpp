#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <random>
#include <vector>
#include <ds/common/error.hpp>

namespace ds {

inline std::expected<int, DataStructureError> randInt(int min, int max) {
    if (min > max) return std::unexpected(DataStructureError::InvalidArgument);
    static thread_local std::mt19937 gen(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    ));
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

inline std::expected<std::vector<int>, DataStructureError> randSample(int min, int max, int count) {
    if (min > max) return std::unexpected(DataStructureError::InvalidArgument);
    if (count <= 0) return std::unexpected(DataStructureError::InvalidArgument);
    const std::int64_t rangeSize = static_cast<std::int64_t>(max) - static_cast<std::int64_t>(min) + 1;
    if (static_cast<std::int64_t>(count) > rangeSize) return std::unexpected(DataStructureError::InvalidArgument);
    std::vector<int> pool;
    pool.reserve(static_cast<size_t>(rangeSize));
    for (std::int64_t value = min; value <= static_cast<std::int64_t>(max); ++value) pool.push_back(static_cast<int>(value));
    static thread_local std::mt19937 gen(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    ));
    std::shuffle(pool.begin(), pool.end(), gen);
    return std::vector<int>(pool.begin(), pool.begin() + count);
}

} // namespace ds
