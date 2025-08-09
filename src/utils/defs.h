#pragma once

#include <stdint.h>

#define BYTE uint8_t
#define WORD uint16_t
#define DWORD uint32_t

#define _BIT(__BIT__) (1 << __BIT__)
#define BYTE_BITS 8
#define WORD_BITS 16
#define DWORD_BITS 32

#define ENABLE 1
#define DISABLE 0

#define ARRAY_SIZE(__ARRAY__) (sizeof(__ARRAY__)/sizeof(__ARRAY__[0]))
