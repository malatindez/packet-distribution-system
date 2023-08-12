#pragma once
#include "include/library-pch.hpp"
#include "include/win.hpp"
namespace utils
{
    /**
     * @brief Converts a wide string (UTF16) to a UTF-8 string.
     *
     * @param wstr The input wide string.
     * @return The UTF-8 string equivalent of the input wide string.
     */
    inline std::string wstring_to_string(std::wstring_view const wstr)
    {
        if (wstr.empty())
            return std::string();
        int size =
            WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string rv(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), rv.data(), size, NULL, NULL);
        return rv;
    }

    /**
     * @brief Converts a UTF-8 string to a wide string (UTF16).
     *
     * @param str The input UTF-8 string.
     * @return The wide string equivalent of the input UTF-8 string.
     */
    inline std::wstring string_to_wstring(std::string_view const str)
    {
        if (str.empty())
            return std::wstring();
        int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);
        std::wstring rv(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), rv.data(), size);
        return rv;
    }

    /**
     * @brief Formats a Win32 error code as a string.
     *
     * This function converts a Win32 error code into a human-readable error message.
     * If the error message ID is 0, an empty string is returned.
     *
     * @param errorMessageID The Win32 error code.
     * @return The formatted error message as a string.
     */
    inline std::string FormatErrorAsString(DWORD errorMessageID)
    {
        if (errorMessageID == 0)
        {
            return std::string(); // No error message has been recorded
        }

        LPSTR messageBuffer = nullptr;

        // Ask Win32 to give us the string version of that message ID.
        // The parameters we pass in, tell Win32 to create the buffer that holds the message for us
        // (because we don't yet know how long the message string will be).
        size_t size =
            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                               FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPSTR)&messageBuffer, 0, NULL);

        // Copy the error message into a std::string.
        std::string message(messageBuffer, size);

        // Free the Win32's string's buffer.
        LocalFree(messageBuffer);
        return message.substr(0, message.find_last_of("\n", std::max(0ull, message.size() - 5)));
    }
} // namespace utils