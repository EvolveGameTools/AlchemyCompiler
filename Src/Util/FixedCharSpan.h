#pragma once

#include "../PrimitiveTypes.h"
#include <cstring>

namespace Alchemy {

    struct FixedCharSpan {

        char* ptr;
        size_t size;

        FixedCharSpan() : ptr(nullptr), size(0) {}

        explicit FixedCharSpan(const char* ptr_) : ptr((char*) ptr_), size(strlen(ptr_)) {}

        FixedCharSpan(char* ptr, size_t size) : ptr(ptr), size(size) {}
        FixedCharSpan(const char* ptr, size_t size) : ptr((char*)ptr), size(size) {}

        bool operator==(const char* other) const {
            size_t len = strlen(other);
            if (len != size) {
                return false;
            }
            return std::memcmp(ptr, other, size) == 0;
        }

        bool operator!=(const char* other) const {
            size_t len = strlen(other);
            if (len == size) {
                return false;
            }
            return std::memcmp(ptr, other, size) != 0;
        }

        bool operator==(const FixedCharSpan& other) const {
            if (size != other.size) {
                return false;
            }
            return std::memcmp(ptr, other.ptr, size) == 0;
        }

        bool operator!=(const FixedCharSpan& other) const {
            if (size == other.size) {
                return false;
            }
            return std::memcmp(ptr, other.ptr, size) != 0;
        }

        inline bool StartsWith(FixedCharSpan prefix) {
            return prefix.size <= size && strncmp(ptr, prefix.ptr, prefix.size) == 0;
        }

        int32 GetHashCode() const {

            // FNV-1a

            constexpr uint32 FNV_PRIME_32 = 0x01000193;
            constexpr uint32 OFFSET_BASIS_32 = 2166136261;
            uint32 hash = OFFSET_BASIS_32;

            for (size_t i = 0; i < size; ++i) {
                hash *= FNV_PRIME_32;
                hash ^= static_cast<uint32>(ptr[i]);
            }

            return static_cast<int32>(hash);
        }

        bool Equals(FixedCharSpan other) const {
            if (size != other.size) {
                return false;
            }
            return memcmp(ptr, other.ptr, size) == 0;
        }

    };

}