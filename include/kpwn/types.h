/**
 * kpwn/types.h — Shared type definitions
 */
#ifndef KPWN_TYPES_H
#define KPWN_TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;


#define NO_KASLR_BASE    0xffffffff81000000ULL
#define PAGE_SIZE_4K    0x1000ULL

#endif /* KPWN_TYPES_H */
