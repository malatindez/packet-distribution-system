#pragma once
#include "include/library-pch.hpp"

namespace utils
{
    /**
     * @brief Inserts a value into a sorted vector while maintaining the sorted order.
     * @param range The sorted vector to insert into.
     * @param value The value to insert.
     */
    template <typename T> void SortedInsert(std::vector<T> &range, T &&value)
    {
        auto lower = std::lower_bound(range.begin(), range.end(), value);
        range.emplace(lower, std::forward<T>(value));
    }

    /**
     * @brief Inserts a value into a sorted vector while maintaining the sorted order using a custom
     * comparator.
     * @param range The sorted vector to insert into.
     * @param value The value to insert.
     * @param comparator A function that compares two values.
     */
    template <typename T>
    void SortedInsert(std::vector<T> &range, T &&value,
                      std::function<bool(T const &, T const &)> const &comparator)
    {
        auto lower = std::lower_bound(range.begin(), range.end(), value, comparator);
        range.emplace(lower, std::forward<T>(value));
    }

    /**
     * @brief Efficiently erases elements equal to a given value from a sorted vector.
     * @param range The sorted vector to erase from.
     * @param value The value to erase.
     */
    template <typename T> void SortedErase(std::vector<T> &range, T &&value)
    {
        auto t = std::equal_range(range.begin(), range.end(), value);
        range.erase(t.first, t.second);
    }

    /**
     * @brief Efficiently erases elements equal to a given value from a sorted vector using a custom
     * comparator.
     * @param range The sorted vector to erase from.
     * @param value The value to erase.
     * @param comparator A function that compares two values.
     */
    template <typename T>
    void SortedErase(std::vector<T> &range, T &&value,
                     std::function<bool(T const &, T const &)> const &comparator)
    {
        auto t = std::equal_range(range.begin(), range.end(), value, comparator);
        range.erase(t.first, t.second);
    }

    /**
     * @brief Combines a hash value into an existing hash using bitwise operations.
     * @param seed The existing hash value, which will be updated.
     * @param hash The hash value to combine.
     */
    template <typename T> void hash_combine(size_t &seed, T const &hash)
    {
        seed ^= std::hash<T>{}(hash) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
} // namespace utils
