#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <containerium/core/error.hpp>

namespace containerium {

inline std::expected<std::vector<int>, DataStructureError> kmpBuildLps(std::string_view pattern) {
    if (pattern.empty()) return std::unexpected(DataStructureError::InvalidArgument);
    std::vector<int> lps(pattern.size(), 0);
    int len = 0;
    for (size_t i = 1; i < pattern.size(); ) {
        if (pattern[i] == pattern[static_cast<size_t>(len)]) {
            len++;
            lps[i] = len;
            i++;
        } 
        else if (len != 0) { len = lps[static_cast<size_t>(len - 1)]; } 
        else {
            lps[i] = 0;
            i++;
        }
    }
    return lps;
}

inline std::expected<int, DataStructureError> kmpFindFirst(std::string_view text, std::string_view pattern) {
    if (text.empty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
    if (pattern.empty()) return std::unexpected(DataStructureError::InvalidArgument);
    if (pattern.size() > text.size()) return std::unexpected(DataStructureError::ElementNotFound);
    CONTAINERIUM_TRY(lps, kmpBuildLps(pattern));
    size_t i = 0;
    size_t j = 0;
    while (i < text.size()) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
            if (j == pattern.size()) return static_cast<int>(i - j);
        } 
        else if (j != 0) { j = static_cast<size_t>(lps[j - 1]); } 
        else { i++; }
    }
    return std::unexpected(DataStructureError::ElementNotFound);
}

inline std::expected<std::vector<int>, DataStructureError> kmpFindAll(std::string_view text, std::string_view pattern) {
    if (text.empty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
    if (pattern.empty()) return std::unexpected(DataStructureError::InvalidArgument);
    if (pattern.size() > text.size()) return std::unexpected(DataStructureError::ElementNotFound);
    CONTAINERIUM_TRY(lps, kmpBuildLps(pattern));
    std::vector<int> matches;
    size_t i = 0;
    size_t j = 0;
    while (i < text.size()) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
            if (j == pattern.size()) {
                matches.push_back(static_cast<int>(i - j));
                j = static_cast<size_t>(lps[j - 1]);
            }
        } 
        else if (j != 0) { j = static_cast<size_t>(lps[j - 1]); } 
        else { i++; }
    }
    if (matches.empty()) return std::unexpected(DataStructureError::ElementNotFound);
    return matches;
}

} // namespace containerium
