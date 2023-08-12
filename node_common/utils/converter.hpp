#pragma once
#include "include/library-pch.hpp"
namespace utils
{
    /**
     * @brief Convert a string_view to an optional int64_t.
     *
     * Converts the given string_view to an int64_t using the strtoll function.
     * If the conversion is successful, returns an optional containing the result,
     * otherwise returns an empty optional.
     *
     * @param str The string_view to convert.
     * @return An optional containing the converted int64_t or an empty optional if conversion
     * fails.
     */
    inline std::optional<int64_t> to_int64(std::string_view str)
    {
        char *end;
        int64_t ll = strtoll(str.data(), &end, static_cast<int>(str.size()));
        if (str.data() != end)
        {
            return ll;
        }
        return std::nullopt;
    }

    /**
     * @brief Convert a string_view to an optional uint64_t.
     *
     * Converts the given string_view to a uint64_t using the strtoull function.
     * If the conversion is successful, returns an optional containing the result,
     * otherwise returns an empty optional.
     *
     * @param str The string_view to convert.
     * @return An optional containing the converted uint64_t or an empty optional if conversion
     * fails.
     */
    inline std::optional<uint64_t> to_uint64(std::string_view str)
    {
        char *end;
        uint64_t ll = strtoull(str.data(), &end, static_cast<int>(str.size()));
        if (str.data() != end)
        {
            return ll;
        }
        return std::nullopt;
    }

    /**
     * @brief Convert a string_view to an optional long double.
     *
     * Converts the given string_view to a long double using the strtold function.
     * If the conversion is successful, returns an optional containing the result,
     * otherwise returns an empty optional.
     *
     * @param str The string_view to convert.
     * @return An optional containing the converted long double or an empty optional if conversion
     * fails.
     */
    inline std::optional<long double> to_long_double(std::string_view str)
    {
        char *end;
        long double ld = strtold(str.data(), &end);
        if (str.data() != end)
        {
            return ld;
        }
        return std::nullopt;
    }
} // namespace utils