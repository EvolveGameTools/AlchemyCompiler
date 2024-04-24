#pragma once
#include "./DLLExport.h"

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

#if defined(_M_X64)

    typedef __int64				machine;
    typedef unsigned __int64	umachine;
    typedef unsigned __int64	machine_address;

#else

    typedef int					machine;
    typedef unsigned int		umachine;
    typedef unsigned int		machine_address;

#endif


#elif defined(__ORBIS__) || defined(__PROSPERO__)

typedef signed char				int8;
typedef short					int16;
typedef int						int32;
typedef long					int64;
typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned int			uint32;
typedef unsigned long			uint64;
typedef long					machine;
typedef unsigned long			umachine;
typedef unsigned long			machine_address;

#elif defined(__GNUC__)

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

#if defined(__LP64__) || defined(_WIN64)

typedef long long machine;
typedef unsigned long long umachine;
typedef unsigned long long machine_address;

#else

typedef int	machine;
typedef unsigned int umachine;
typedef unsigned int machine_address;

#endif
#endif

// define size_t without using another header
#ifndef size_t
using ___x = decltype(alignof(char));
typedef ___x size_t;
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif

#undef NDEBUG
#if EVOLVE_INCLUDE_ASSERTS
#endif
#include <cassert>

#if _MSC_VER
#define tzcnt64(n) (n == 0 ? 64 : (int32) _tzcnt_u64((uint64) n))
#define tzcnt32(n) (n == 0 ? 32 : (int32) _tzcnt_u32((uint32) n))
#define __builtin_popcountll __popcnt64
#define __builtin_popcount __popcnt
#else
#define tzcnt64(n) (n == 0 ? 64 : __builtin_ctzll(n))
#define tzcnt32(n) (n == 0 ? 32 : __builtin_ctz(n))
#endif


#include <type_traits>

#define IMPL_ENUM_FLAGS_OPERATORS(enumType) \
    constexpr enumType operator|(enumType lhs, enumType rhs) \
    { \
        typedef std::underlying_type<enumType>::type IntegralType; \
        typedef std::make_unsigned<IntegralType>::type RawType; \
        return static_cast<enumType>(static_cast<RawType>(lhs) | static_cast<RawType>(rhs)); \
    } \
    constexpr enumType operator&(enumType lhs, enumType rhs) \
    { \
        typedef std::underlying_type<enumType>::type IntegralType; \
        typedef std::make_unsigned<IntegralType>::type RawType; \
        return static_cast<enumType>(static_cast<RawType>(lhs) & static_cast<RawType>(rhs)); \
    } \
    constexpr enumType operator^(enumType lhs, enumType rhs) \
    { \
        typedef std::underlying_type<enumType>::type IntegralType; \
        typedef std::make_unsigned<IntegralType>::type RawType; \
        return static_cast<enumType>(static_cast<RawType>(lhs) ^ static_cast<RawType>(rhs)); \
    } \
    constexpr enumType operator~(enumType rhs) \
    { \
        typedef std::underlying_type<enumType>::type IntegralType; \
        typedef std::make_unsigned<IntegralType>::type RawType; \
        return static_cast<enumType>(~static_cast<RawType>(rhs)); \
    }                                       \
    constexpr bool operator == (enumType lhs, int rhs) \
    { \
        return static_cast<int>(lhs) == rhs; \
    } \
    constexpr bool operator != (enumType lhs, int rhs) \
    { \
        return static_cast<int>(lhs) != rhs; \
    }                                       \
inline enumType& operator|=(enumType& lhs, enumType rhs)\
{\
lhs = lhs | rhs;\
return lhs;\
}\
inline enumType& operator&=(enumType& lhs, enumType rhs)\
{\
    lhs = lhs & rhs;\
    return lhs;\
}                                           \

#define GIGABYTES(x) 1073741824ll * ((int64)x)
#define MEGABYTES(x) 1048576ll * ((int64)x)
#define KILOBYTES(x) 1024ll * ((int64)x)
