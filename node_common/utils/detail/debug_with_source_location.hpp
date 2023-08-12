#pragma once
#include "../debug.hpp"

/**
 * @file debug_with_source_location.hpp
 *
 * @brief This file provides utility functions for debugging and assertion handling.
 */

namespace utils
{
    /**
     * @brief Generates a string representation of the current source location.
     *
     * @param location The source location information (default: current location).
     * @return A string containing file name, function name, line, and column information.
     */
    inline std::string
    CurrentSourceLocation(std::source_location location = std::source_location::current())
    {
        std::stringstream ss;
        ss << "[" << location.file_name() << "] ";
        ss << location.function_name() << "(line " << location.line() << ", column "
           << location.column() << ") ";
        return ss.str();
    }
    /**
     * @brief Asserts a condition with customizable behavior based on debug mode.
     *
     * This function is used to assert a given condition in debug mode. If the condition is true,
     * the function has no effect. If the condition is false, the assertion behavior depends on
     * the configuration settings and the current debug mode.
     *
     * In debug mode (DEBUG_UTILS_DEBUG_ENABLED set to true), an assertion is triggered using
     * the assert macro. The program halts, and debugging tools can be used to inspect the error.
     *
     * In release mode, the behavior of the assertion is determined by the following macros:
     *
     * - DEBUG_UTILS_ASSERT_ABORTS: Determines whether an assertion failure should result in
     *   program abortion. If set to true, the program is immediately terminated upon an
     *   assertion failure. If set to false, a runtime_error exception is thrown, allowing
     *   for more controlled error handling, especially in release mode.
     *
     * - DEBUG_UTILS_ASSERT_LOGS: Indicates whether logging is enabled for assertion failures.
     *   If set to true, a critical log message is generated using the SPDLog library to indicate
     *   the assertion failure.
     *
     * It is important to configure these macros according to the desired behavior for your
     * application's debugging and error-handling strategy.
     *
     * @param value The condition to assert.
     * @param message An optional message to include in the log and assertion.
     */
    inline void Assert(bool value, std::string_view message = "Assert failed",
                       std::source_location location = std::source_location::current())
    {
        if constexpr (!DEBUG_UTILS_ASSERT_ENABLED)
        {
            return;
        }

        if (value) [[likely]]
        {
            return;
        }

        if constexpr (DEBUG_UTILS_ASSERT_LOGS)
        {
            spdlog::critical(CurrentSourceLocation(location) + std::basic_string(message));
        }
        if constexpr (DEBUG_UTILS_FORCE_ASSERT)
        {
            assert(value);
        }
        else
        {
            if constexpr (DEBUG_UTILS_ASSERT_ABORTS)
            {
                std::abort();
            }
            else
            {
                throw std::runtime_error(CurrentSourceLocation(location) +
                                         std::basic_string(message));
            }
        }
    }

    /**
     * @brief Always asserts a condition with customizable behavior.
     *
     * This function is used to assert a given condition, irrespective of the current
     * debugging mode. If the condition evaluates to true, the function has no effect.
     * However, if the condition is false, the behavior of the assertion depends on the
     * configuration settings.
     *
     * If logging is enabled (DEBUG_UTILS_ASSERT_LOGS is set to true), the function generates
     * a critical log message using the SPDLog library to indicate the assertion failure.
     * Additionally, the assertion behavior in different contexts (debug or release mode) is
     * determined by the following macros:
     *
     * - DEBUG_UTILS_DEBUG_ENABLED: Indicates whether debug mode is enabled. In debug mode,
     *   an assertion is triggered using the assert macro. The program halts, and debugging
     *   tools can be used to inspect the error.
     *
     * - DEBUG_UTILS_ASSERT_ABORTS: Determines whether an assertion failure should result in
     *   program abortion. If set to true, the program is immediately terminated upon an
     *   assertion failure. If set to false, a runtime_error exception is thrown, allowing
     *   for more controlled error handling, especially in release mode.
     *
     * It is important to configure these macros according to the desired behavior for your
     * application's debugging and error-handling strategy.
     *
     * @param value The condition to assert.
     * @param message An optional message to include in the log and assertion.
     */
    inline void AlwaysAssert(bool value, std::string_view message = "Assert failed",
                             std::source_location location = std::source_location::current())
    {
        if constexpr (!DEBUG_UTILS_ALWAYS_ASSERT_ENABLED)
        {
            return;
        }

        if (value) [[likely]]
        {
            return;
        }

        if constexpr (DEBUG_UTILS_ASSERT_LOGS)
        {
            spdlog::critical(CurrentSourceLocation(location) + std::basic_string(message));
        }

        if constexpr (DEBUG_UTILS_FORCE_ASSERT)
        {
            assert(value);
        }
        else
        {
            if constexpr (DEBUG_UTILS_ASSERT_ABORTS)
            {
                std::abort();
            }
            else
            {
                throw std::runtime_error(CurrentSourceLocation(location) +
                                         std::basic_string(message));
            }
        }
    }
} // namespace utils