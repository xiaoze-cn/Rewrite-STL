#pragma once

#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <containerium/core/error.hpp>

namespace containerium {

inline std::expected<int, DataStructureError> randInt(int min, int max) {
    if (min > max) return std::unexpected(DataStructureError::InvalidArgument);
    static std::mt19937 gen(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    ));
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

inline std::expected<std::vector<int>, DataStructureError> randSample(int min, int max, int count) {
    if (min > max) return std::unexpected(DataStructureError::InvalidArgument);
    if (count <= 0) return std::unexpected(DataStructureError::InvalidArgument);
    if (count > (max - min + 1)) return std::unexpected(DataStructureError::InvalidArgument);
    std::vector<int> pool;
    pool.reserve(max - min + 1);
    for (int i = min; i <= max; ++i) pool.push_back(i);
    std::shuffle(pool.begin(), pool.end(), 
        std::mt19937(static_cast<unsigned>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        ))
    );
    return std::vector<int>(pool.begin(), pool.begin() + count);
}

} // namespace containerium
