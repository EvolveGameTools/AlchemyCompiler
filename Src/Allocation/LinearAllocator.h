#pragma once

#include "../PrimitiveTypes.h"
#include "../Collections/CheckedArray.h"
#include <cstring>
#include <memory>

class LinearAllocator {
protected:

    uint8* base;
    size_t reserved;
    size_t committed;
    size_t minCommitStep;
    size_t offset;

    uint8* AllocateBytesUncleared(size_t bytes, size_t alignment);

public:

    explicit LinearAllocator(size_t reserveSize, size_t minCommitSize);

    virtual ~LinearAllocator();

    inline void Clear() {
        offset = 0;
    }

    template<typename T>
    T* Allocate(int32 count) {
        size_t bytes = count * sizeof(T);
        void* retn = AllocateBytesUncleared(bytes, alignof(T));
        memset(retn, 0, bytes);
        return (T*) retn;
    }

    template<typename T>
    Alchemy::CheckedArray<T> Copy(Alchemy::CheckedArray<T> other) {
        size_t bytes = other.size * sizeof(T);
        void* retn = AllocateBytesUncleared(bytes, alignof(T));
        memcpy(retn, other.array, other.size * sizeof(T));
        return Alchemy::CheckedArray<T>((T*) retn, other.size);
    }

    template<typename T, typename... Args>
    T* New(Args &&... args) {
        size_t bytes = sizeof(T);
        T* retn = (T*) AllocateBytesUncleared(bytes, alignof(T));
        new(retn) T(std::forward<Args>(args)...);
        return retn;
    }

    template<typename T>
    T* AllocateUncleared(int32 count) {
        return (T*) AllocateBytesUncleared(sizeof(T) * count, alignof(T));
    }

};

struct TempAllocator : public LinearAllocator {

    class Marker {
        size_t offset{};

        explicit Marker(size_t offset)
                : offset(offset) {};

        friend class TempAllocator;

    };

    class ScopedMarker {
        Marker m;
        TempAllocator* allocator;

    public:
        explicit ScopedMarker(TempAllocator* allocator)
                : allocator(allocator)
                  , m(allocator->Mark()) {}

        ~ScopedMarker() {
            allocator->RollbackTo(m);
        }
    };

    inline Marker Mark() {
        return Marker(offset);
    }

    inline void RollbackTo(Marker m) {
        offset = m.offset;
    }

    Marker MarkerFromOffset(void* p);

    TempAllocator(size_t reservation, size_t commitSize);


};