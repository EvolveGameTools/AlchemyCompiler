#pragma once

#include "../PrimitiveTypes.h"

namespace Alchemy::MsiHash {

    inline int32 FNV1a(char* str, int32 size) {
        constexpr uint32 FNV_PRIME_32 = 0x01000193;
        constexpr uint32 OFFSET_BASIS_32 = 2166136261;
        uint32 hash = OFFSET_BASIS_32;

        for (size_t i = 0; i < size; ++i) {
            hash ^= static_cast<uint32>(str[i]) & 255;
            hash *= FNV_PRIME_32;
        }

        return static_cast<int32>(hash);
    }

    inline int32 Lookup32(int32 hash, int32 exponent, int32 idx) {
        uint32 mask = ((uint32) 1 << exponent) - 1;
        uint32 step = (hash >> (32 - exponent)) | 1;
        return (idx + step) & mask;
    }

    inline int64 Lookup64(int64 hash, int32 exponent, int32 idx) {
        uint32 mask = ((uint32) 1 << exponent) - 1;
        uint32 step = (hash >> (64 - exponent)) | 1;
        return (idx + step) & mask;
    }

}
