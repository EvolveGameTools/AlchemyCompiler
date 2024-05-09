#pragma once
#include "./DLLExport.h"
#include "./Panic.h"

typedef unsigned char			byte;
typedef char16_t                char16;
typedef char32_t                char32;

#if defined(_MSC_VER)

typedef signed char				int8;
typedef short					int16;
typedef int						int32;
typedef __int64					int64;
typedef unsigned char			uint8;
typedef unsigned char			byte;
typedef unsigned short			uint16;
typedef unsigned int			uint32;
typedef unsigned __int64		uint64;

#elif defined(__ORBIS__) || defined(__PROSPERO__)

typedef signed char				int8;
typedef short					int16;
typedef int						int32;
typedef long					int64;
typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned int			uint32;
typedef unsigned long			uint64;

#elif defined(__GNUC__)

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

#endif

// define size_t without using another header
#ifndef size_t
using x___ = decltype(alignof(char));
typedef x___ size_t;
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif

#undef NDEBUG
#if EVOLVE_INCLUDE_ASSERTS
#endif
#include <cassert>

#if _MSC_VER
#define tzcnt64(n) (n == 0 ? 64 : (int32) __builtin_ia32_tzcnt_u64((uint64) n))
#define tzcnt32(n) (n == 0 ? 32 : (int32) __builtin_ia32_tzcnt_u32((uint32) n))
#define __builtin_popcountll __popcnt64
#define __builtin_popcount __popcnt
#else
#define tzcnt64(n) (n == 0 ? 64 : __builtin_ctzll(n))
#define tzcnt32(n) (n == 0 ? 32 : __builtin_ctz(n))
#endif

#define DEFINE_ENUM_FLAGS(enumType, typeName, ...) \
    enum class enumType : typeName           \
        __VA_ARGS__                          \
    ;                                        \
    constexpr enumType operator|(enumType lhs, enumType rhs) { return static_cast<enumType>(static_cast<typeName>(lhs) | static_cast<typeName>(rhs));  } \
    constexpr enumType operator&(enumType lhs, enumType rhs) { return static_cast<enumType>(static_cast<typeName>(lhs) & static_cast<typeName>(rhs));  } \
    constexpr enumType operator^(enumType lhs, enumType rhs) { return static_cast<enumType>(static_cast<typeName>(lhs) ^ static_cast<typeName>(rhs));  } \
    constexpr enumType operator~(enumType rhs) { return static_cast<enumType>(~static_cast<typeName>(rhs)); }                                            \
    constexpr bool operator == (enumType lhs, int32 rhs) { return static_cast<int32>(lhs) == rhs; }                                                      \
    constexpr bool operator != (enumType lhs, int32 rhs) { return static_cast<int32>(lhs) != rhs; }                                                      \
    inline enumType& operator|=(enumType& lhs, enumType rhs) { lhs = lhs | rhs; return lhs; }                                                            \
    inline enumType& operator&=(enumType& lhs, enumType rhs) { lhs = lhs & rhs; return lhs; }

#define GIGABYTES(x) (1073741824ll * ((int64)x))
#define MEGABYTES(x) (1048576ll * ((int64)x))
#define KILOBYTES(x) (1024ll * ((int64)x))

#define NOT_IMPLEMENTED(x) Panic(PanicType::NotImplemented, (void*)(x))
#define UNREACHABLE(x) Panic(PanicType::Unreachable, (void*)(x))