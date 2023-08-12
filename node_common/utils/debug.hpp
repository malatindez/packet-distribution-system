#pragma once
#include "include/library-pch.hpp"
#include "include/spdlog.hpp"

/**
 * @file debug_utils.hpp
 *
 * @brief This file provides utilities for debugging and assertion handling.
 */

#ifndef DEBUG_UTILS_ASSERT_LOGS

/**
 * @brief Indicates whether assertion logs are enabled (1) or disabled (0).
 *
 * If enabled, assertion logs will be generated when assertions fail.
 * Default value is 1.
 */
#define DEBUG_UTILS_ASSERT_LOGS 1
#endif

/**
 * @def DEBUG_UTILS_ASSERT_ABORTS
 * @brief Controls whether assertions should result in program abortion.
 *
 * If set to 1, failed assertions will lead to program abortion. If set to 0, assertions
 * will not abort the program, and behavior will depend on DEBUG_UTILS_ASSERT_THROWS.
 * This option is ignored in _DEBUG mode.
 */

#if !defined(DEBUG_UTILS_ASSERT_ABORTS)
#if defined(_DEBUG)
#define DEBUG_UTILS_ASSERT_ABORTS 1
#define DEBUG_UTILS_ASSERT_THROWS 0
#else
#define DEBUG_UTILS_ASSERT_ABORTS 0
#endif
#endif

/**
 * @def DEBUG_UTILS_ASSERT_THROWS
 * @brief Controls whether assertions throw exceptions in case of failure.
 *
 * If set to 1, failed assertions will throw exceptions. If set to 0, assertions will
 * not throw exceptions and will follow the behavior defined by DEBUG_UTILS_ASSERT_ABORTS.
 * This option is ignored if DEBUG_UTILS_ASSERT_ABORTS is set to 1.
 */
#ifndef DEBUG_UTILS_ASSERT_THROWS
#define DEBUG_UTILS_ASSERT_THROWS 1
#endif

#ifdef _DEBUG
#ifndef DEBUG_UTILS_ASSERT_ENABLED
/**
 * @def DEBUG_UTILS_ASSERT_ENABLED
 * @brief Controls whether assertions are enabled in debug mode.
 *
 * If set to 1, assertions are enabled in debug mode. If set to 0, assertions are disabled
 * in debug mode.
 */
#define DEBUG_UTILS_ASSERT_ENABLED 1
#endif
#ifndef DEBUG_UTILS_ALWAYS_ASSERT_ENABLED
/**
 * @def DEBUG_UTILS_ALWAYS_ASSERT_ENABLED
 * @brief Controls whether always assertions are enabled in debug mode.
 *
 * If set to 1, always assertions are enabled in debug mode. If set to 0, always assertions
 * are disabled in debug mode.
 */
#define DEBUG_UTILS_ALWAYS_ASSERT_ENABLED 1
#endif
#else
#ifndef DEBUG_UTILS_ASSERT_ENABLED
#define DEBUG_UTILS_ASSERT_ENABLED 0
#endif
#ifndef DEBUG_UTILS_ALWAYS_ASSERT_ENABLED
#define DEBUG_UTILS_ALWAYS_ASSERT_ENABLED 1
#endif
#endif

#ifndef DEBUG_UTILS_FORCE_ASSERT
#ifdef _DEBUG
/**
 * @def DEBUG_UTILS_FORCE_ASSERT
 * @brief Controls whether assertions are forced to execute.
 *
 * If set to 1, assertions are forcibly executed. If set to 0, assertions are executed
 * based on the conditions defined by other macros.
 */
#define DEBUG_UTILS_FORCE_ASSERT 1
#else
#define DEBUG_UTILS_FORCE_ASSERT 0
#endif
#endif

#if DEBUG_UTILS_ASSERT_THROWS
#define DEBUG_UTILS_ASSERT_NOEXCEPT
#else
#define DEBUG_UTILS_ASSERT_NOEXCEPT noexcept
#endif

#ifdef ENGINE_NO_SOURCE_LOCATION
#include "detail/debug_without_source_location.hpp"
#else
#include "detail/debug_with_source_location.hpp"
#endif