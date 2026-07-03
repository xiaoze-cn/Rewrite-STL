#pragma once

#include <expected>
#include <string>

#define CONTAINERIUM_TRY(var, expr) \
    auto var##Result = (expr); \
    if (!var##Result) return std::unexpected(var##Result.error()); \
    auto var = var##Result.value()

namespace containerium {
enum class DataStructureError {
    IndexOutOfRange,
    ContainerIsEmpty,
    ContainerIsFull,
    InvalidArgument,
    MemoryAllocationFailed,
    ElementNotFound,
    DuplicateValue,
    CycleDetected,
    InvalidOperation,
    InvalidRange,
    InvalidSampleSize,
    RandomGenerationFailed,
};

inline std::string errorMessage(DataStructureError error) {
    switch (error) {
        case DataStructureError::IndexOutOfRange: return "Index out of range";
        case DataStructureError::ContainerIsEmpty: return "Container is empty";
        case DataStructureError::ContainerIsFull: return "Container is full";
        case DataStructureError::InvalidArgument: return "Invalid argument";
        case DataStructureError::MemoryAllocationFailed: return "Memory allocation failed";
        case DataStructureError::ElementNotFound: return "Element not found";
        case DataStructureError::DuplicateValue: return "Duplicate value";
        case DataStructureError::CycleDetected: return "Cycle detected in graph";
        case DataStructureError::InvalidOperation: return "Invalid operation for this graph type";
        case DataStructureError::InvalidRange: return "Invalid range";
        case DataStructureError::InvalidSampleSize: return "Invalid sample size";
        case DataStructureError::RandomGenerationFailed: return "Random generation failed";
        default: return "Unknown error";
    }
}
} // namespace containerium
