#pragma once
namespace utils
{
    /**
     * @brief Converts a string to lowercase.
     * @param s The input string view.
     * @return A new string containing the lowercase characters.
     */
    inline std::string as_lowercase(std::string_view const s) noexcept
    {
        std::string rv;
        rv.reserve(s.size());
        std::ranges::for_each(s,
                              [&rv](char const &c) { rv += static_cast<char>(std::tolower(c)); });
        return rv;
    }

    /**
     * @brief Converts a string to uppercase.
     * @param s The input string view.
     * @return A new string containing the uppercase characters.
     */
    inline std::string as_uppercase(std::string_view const s) noexcept
    {
        std::string rv;
        rv.reserve(s.size());
        std::ranges::for_each(s,
                              [&rv](char const &c) { rv += static_cast<char>(std::toupper(c)); });
        return rv;
    }

    /**
     * @brief Trims leading whitespace from a string view.
     * @param s The input string view.
     * @return A new string view with leading whitespace removed.
     */
    constexpr std::string_view ltrimview(std::string_view const s) noexcept
    {
        return std::string_view(
            std::ranges::find_if(s, [](unsigned char ch) { return !std::isspace(ch); }), s.end());
    }

    /**
     * @brief Trims trailing whitespace from a string view.
     * @param s The input string view.
     * @return A new string view with trailing whitespace removed.
     */
    constexpr std::string_view rtrimview(std::string_view const s) noexcept
    {
        return std::string_view(
            s.begin(),
            std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); })
                .base());
    }

    /**
     * @brief Trims leading and trailing whitespace from a string view.
     * @param s The input string view.
     * @return A new string view with leading and trailing whitespace removed.
     */
    constexpr std::string_view trimview(std::string_view const s) noexcept
    {
        return ltrimview(rtrimview(s));
    }

    /**
     * @brief Trims leading whitespace from a string.
     * @param s The input string.
     * @return A new string with leading whitespace removed.
     */
    inline std::string ltrim(std::string const &s) noexcept { return std::string(ltrimview(s)); }
    /**
     * @brief Trims trailing whitespace from a string.
     * @param s The input string.
     * @return A new string with trailing whitespace removed.
     */
    inline std::string rtrim(std::string const &s) noexcept { return std::string(rtrimview(s)); }


    /**
     * @brief Trims leading and trailing whitespace from a string.
     * @param s The input string.
     * @return A new string with leading and trailing whitespace removed.
     */
    inline std::string trim(std::string const &s) noexcept { return std::string(trimview(s)); }
} // namespace utils
