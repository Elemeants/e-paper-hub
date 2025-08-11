/**
 * @file defs.h
 * @author jdanypa@gmail.com (Elemeants)
 * @brief Type definitions for commonly used data types
 *
 * This header file defines common data types used throughout the project, including
 * fixed-width integer types and bit manipulation macros.
 */
#pragma once

#include <stdint.h>

// Define common data types for better readability and portability

#define BYTE uint8_t
#define WORD uint16_t
#define DWORD uint32_t

// Bit manipulation macros

#define _BIT(__BIT__) (1 << __BIT__)
#define BYTE_BITS 8
#define WORD_BITS 16
#define DWORD_BITS 32

// Enable/disable macros for clarity

#define ENABLE 1
#define DISABLE 0

/**
 * @brief Get the number of elements in an static array
 *
 * This macro calculates the number of elements in a statically allocated array.
 *
 * @param __ARRAY__ The array to measure
 * @return The number of elements in the array
 */
#define ARRAY_SIZE(__ARRAY__) (sizeof(__ARRAY__)/sizeof(__ARRAY__[0]))
