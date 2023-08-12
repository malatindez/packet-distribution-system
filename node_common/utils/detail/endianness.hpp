#pragma once
#include "include/library-pch.hpp"
#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>

#include <bit>

/**
 * @file endianness.hpp
 * @brief Provides functions and utilities for handling endianness conversions.
 */

namespace utils::endianness
{
    /**
     * @brief Determines if the system's native endianness is big-endian.
     * @return True if the system's native endianness is big-endian, false otherwise.
     */
    constexpr bool is_big_endian() { return std::endian::native == std::endian::big; }

    /**
     * @brief Determines if the system's native endianness is little-endian.
     * @return True if the system's native endianness is little-endian, false otherwise.
     */
    constexpr bool is_little_endian() { return std::endian::native == std::endian::little; }

    /**
     * @brief Represents the determined endianness of the system.
     * @details This is a compile-time constant indicating whether the system's native endianness is
     * big-endian or little-endian.
     */
    constexpr bool kIsBigEndian = is_big_endian();

    /**
     * @brief Represents the determined endianness of the system.
     * @details This is a compile-time constant indicating whether the system's native endianness is
     * little-endian or big-endian.
     */
    constexpr bool kIsLittleEndian = is_little_endian();

    /**
     * @brief Ensures that the determined endianness is valid.
     * @details This compile-time assertion checks if the determined endianness is either big-endian
     * or little-endian.
     */
    static_assert(kIsBigEndian || kIsLittleEndian, "Unknown endianness");

    /**
     * @brief Indicates the compiler type.
     */
    enum class CompilerType : uint32_t
    {
        kUnknown = 1 << 0,
        kGCC = 1 << 1,
        kMSVC = 1 << 2,
        kICC = 1 << 3
    };

    /**
     * @brief Determines the compiler type used to compile the code.
     * @return A bitmask representing the compiler type.
     */
    constexpr uint32_t compiler_type()
    {
        uint32_t result = 0;
#define ENGINE_UTILS_DETAIL_ENDIANNES_ICC 0
#define ENGINE_UTILS_DETAIL_ENDIANNES_GCC 0
#define ENGINE_UTILS_DETAIL_ENDIANNES_MSVC 0
#if defined(__INTEL_COMPILER) || defined(__ICC)
        result |= uint32_t(CompilerType::kICC);
#define ENGINE_UTILS_DETAIL_ENDIANNES_ICC 1
#elif defined(__GNUC__)
        result |= uint32_t(CompilerType::kGCC);
#define ENGINE_UTILS_DETAIL_ENDIANNES_GCC 1
#elif defined(_MSC_VER)
        result |= uint32_t(CompilerType::kMSVC);
#define ENGINE_UTILS_DETAIL_ENDIANNES_MSVC 1
#endif
        return result;
    }

    /**
     * @brief Indicates the compiler type used for the code.
     */
    constexpr uint32_t kCompilerType = compiler_type();

    /**
     * @brief Checks if the code was compiled with ICC (Intel C++ Compiler).
     * @return True if the code was compiled with ICC, false otherwise.
     */
    constexpr bool is_compiler_icc() { return (kCompilerType & uint32_t(CompilerType::kICC)) != 0; }

    /**
     * @brief Checks if the code was compiled with GCC (GNU Compiler Collection).
     * @return True if the code was compiled with GCC, false otherwise.
     */
    constexpr bool is_compiler_gcc() { return (kCompilerType & uint32_t(CompilerType::kGCC)) != 0; }

    /**
     * @brief Checks if the code was compiled with MSVC (Microsoft Visual C++).
     * @return True if the code was compiled with MSVC, false otherwise.
     */
    constexpr bool is_compiler_msvc()
    {
        return (kCompilerType & uint32_t(CompilerType::kMSVC)) != 0;
    }

    /**
     * @brief Converts a 16-bit value between big-endian and little-endian.
     * @param x The value to be converted.
     * @return The converted value.
     */
    inline uint16_t betole16(uint16_t x)
    {
        if constexpr (kIsLittleEndian)
        {
            return ((x & 0xFF00u) >> 8u) | ((x & 0x00FFu) << 8u);
        }
#if ENGINE_UTILS_DETAIL_ENDIANNES_ICC
        return _bswap16(x);
#elif ENGINE_UTILS_DETAIL_ENDIANNES_GCC
        return __builtin_bswap16(x);
#elif ENGINE_UTILS_DETAIL_ENDIANNES_MSVC
        return _byteswap_ushort(x);
#else
        return ((x & 0xFF00u) >> 8u) | ((x & 0x00FFu) << 8u);
#endif
    }

    /**
     * @brief Converts a 32-bit value between big-endian and little-endian.
     * @param x The value to be converted.
     * @return The converted value.
     */
    inline uint32_t betole32(uint32_t x)
    {
        if constexpr (kIsLittleEndian)
        {
            return ((x & 0xFF000000u) >> 24u) | ((x & 0x00FF0000u) >> 8u) |
                   ((x & 0x0000FF00u) << 8u) | ((x & 0x000000FFu) << 24u);
        }
#if ENGINE_UTILS_DETAIL_ENDIANNES_ICC
        return _bswap(x);
#elif ENGINE_UTILS_DETAIL_ENDIANNES_GCC
        return __builtin_bswap32(x);
#elif ENGINE_UTILS_DETAIL_ENDIANNES_MSVC
        return _byteswap_ulong(x);
#else
        return ((x & 0xFF000000u) >> 24u) | ((x & 0x00FF0000u) >> 8u) | ((x & 0x0000FF00u) << 8u) |
               ((x & 0x000000FFu) << 24u);
#endif
    }

    /**
     * @brief Converts a 64-bit value between big-endian and little-endian.
     * @param x The value to be converted.
     * @return The converted value.
     */
    uint64_t betole64(uint64_t x)
    {
        if constexpr (kIsLittleEndian)
        {
            return ((x & 0xFF00000000000000u) >> 56u) | ((x & 0x00FF000000000000u) >> 40u) |
                   ((x & 0x0000FF0000000000u) >> 24u) | ((x & 0x000000FF00000000u) >> 8u) |
                   ((x & 0x00000000FF000000u) << 8u) | ((x & 0x0000000000FF0000u) << 24u) |
                   ((x & 0x000000000000FF00u) << 40u) | ((x & 0x00000000000000FFu) << 56u);
        }
#if ENGINE_UTILS_DETAIL_ENDIANNES_ICC
        return _bswap64(x);
#elif ENGINE_UTILS_DETAIL_ENDIANNES_GCC
        return __builtin_bswap64(x);
#elif ENGINE_UTILS_DETAIL_ENDIANNES_MSVC
        return _byteswap_uint64(x);
#else
        return ((x & 0xFF00000000000000u) >> 56u) | ((x & 0x00FF000000000000u) >> 40u) |
               ((x & 0x0000FF0000000000u) >> 24u) | ((x & 0x000000FF00000000u) >> 8u) |
               ((x & 0x00000000FF000000u) << 8u) | ((x & 0x0000000000FF0000u) << 24u) |
               ((x & 0x000000000000FF00u) << 40u) | ((x & 0x00000000000000FFu) << 56u);
#endif
    }

    /**
     * @brief Converts a 128-bit value between big-endian and little-endian using SSE2 intrinsics.
     * @param x The value to be converted.
     * @return The converted value.
     */
    __m128i betole128(__m128i x)
    {
        if constexpr (kIsLittleEndian)
        {
            return x;
        }

        const __m128i shuffle = _mm_set_epi64x(0x0001020304050607, 0x08090a0b0c0d0e0f);
        return _mm_shuffle_epi8(x, shuffle);
    }

    /**
     * @brief Converts a 256-bit value between big-endian and little-endian using AVX2 intrinsics.
     * @param x The value to be converted.
     * @return The converted value.
     */
    __m256i betole256(__m256i x)
    {
        if constexpr (kIsLittleEndian)
        {
            return x;
        }
        const __m256i shuffle = _mm256_set_epi64x(0x0001020304050607, 0x08090a0b0c0d0e0f,
                                                  0x0001020304050607, 0x08090a0b0c0d0e0f);
        return _mm256_shuffle_epi8(x, shuffle);
    }
} // namespace utils::endianness