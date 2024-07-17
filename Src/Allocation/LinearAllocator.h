#pragma once

#include "../PrimitiveTypes.h"
#include "../Collections/CheckedArray.h"
#include <cstring>
#include <memory>

namespace Alchemy {

    typedef uint8* (* AllocatorFn)(void* cookie, size_t size, size_t alignment);
    typedef void (* FreeFn)(void* cookie, void* ptr, size_t size);

    uint8* LinearAlloc(void* cookie, size_t size, size_t alignment);

    void LinearFree(void* cookie, void * ptr, size_t size) {
        void(0); // no op
    }

    uint8* MallocAlloc(void* cookie, size_t size, size_t alignment);

    void MallocFree(void* cookie, void * ptr, size_t size) {
        Mfree(ptr, size);
    }

    class Allocator {

        void* cookie;
        AllocatorFn allocFn;
        FreeFn freeFn;

    public:

        static Allocator MakeMallocator() {
            return Allocator(nullptr, MallocAlloc, MallocFree);
        }

        explicit Allocator(void* cookie, AllocatorFn allocUncleared, FreeFn freeFn)
            : cookie(cookie)
            , allocFn(allocUncleared)
            , freeFn(freeFn)
            {}

        template<typename T>
        T* Allocate(size_t count) {
            uint8* retn = allocFn(cookie, sizeof(T) * count, alignof(T));
            memset(retn, 0, sizeof(T) * count);
            return (T*) retn;
        }

        template<typename T>
        T* AllocateUncleared(size_t count) {
            return (T*) allocFn(cookie, sizeof(T) * count, alignof(T));
        }

        template<typename T>
        void Free(T * ptr, size_t itemCount = 1) {
            freeFn(cookie, ptr, sizeof(T) * itemCount);
        }

        template<typename T, typename... Args>
        T* New(Args&& ... args) {
            size_t bytes = sizeof(T);
            T* retn = (T*) allocFn(cookie, bytes, alignof(T));
            new(retn) T(std::forward<Args>(args)...);
            return retn;
        }

    };


    class LinearAllocator {

    protected:

        uint8* base;
        size_t reserved;
        size_t committed;
        size_t minCommitStep;


    public:

        Allocator MakeAllocator() {
            return Allocator(this, LinearAlloc, LinearFree);
        }

        explicit LinearAllocator(size_t reserveSize, size_t minCommitSize);

        virtual ~LinearAllocator();

        inline void Clear() {
            offset = 0;
        }

        uint8* AllocateBytesUncleared(size_t bytes, size_t alignment);

        template<typename T>
        T* Allocate(size_t count) {
            size_t bytes = count * sizeof(T);
            void* retn = AllocateBytesUncleared(bytes, alignof(T));
            memset(retn, 0, bytes);
            return (T*) retn;
        }

        template<typename T>
        T* AllocateUncleared(size_t count) {
            return (T*) AllocateBytesUncleared(sizeof(T) * count, alignof(T));
        }

        template<typename T>
        Alchemy::CheckedArray<T> Copy(Alchemy::CheckedArray<T> other) {
            size_t bytes = other.size * sizeof(T);
            void* retn = AllocateBytesUncleared(bytes, alignof(T));
            memcpy(retn, other.array, other.size * sizeof(T));
            return Alchemy::CheckedArray<T>((T*) retn, other.size);
        }

        template<typename T, typename... Args>
        T* New(Args&& ... args) {
            size_t bytes = sizeof(T);
            T* retn = (T*) AllocateBytesUncleared(bytes, alignof(T));
            new(retn) T(std::forward<Args>(args)...);
            return retn;
        }

        template<typename T>
        T* ResizeList(T* array, int32 size, int32* capacity) {
            if (size + 1 > *capacity) {
                T* ptr = AllocateUncleared<T>(*capacity * 2);
                memcpy(ptr, array, sizeof(T) * size);
                array = ptr;
                *capacity *= 2;
            }
            return array;
        }


        size_t GetOffset(void* ptr);

        uint8* GetBase();

        size_t offset;
    };

    inline uint8 * LinearAlloc(void * cookie, size_t size, size_t alignment) {
        return ((LinearAllocator*)cookie)->AllocateBytesUncleared(size, alignment);
    }

    inline uint8 * MallocAlloc(void * cookie, size_t size, size_t alignment) {
        return (uint8*)Mallocate(size);
    }

    struct TempAllocator : public LinearAllocator {

        class Marker {
            size_t offset {};

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
}