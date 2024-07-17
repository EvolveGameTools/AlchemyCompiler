#pragma once

#include "./PagedAllocator.h"

namespace Alchemy {

    template<class T>
    struct PoolAllocator {

        explicit PoolAllocator(int32 pageSize = 128)
            : buffer(pageSize)
            , allocator() {}

        T* Allocate() {

            if (allocator.next != nullptr) {
                BlockHeader* ptr = allocator.next;
                allocator.next = ptr->next;
                T* retn = (T*) ptr;
                memset((void*)retn, 0, sizeof(T));
                return retn;
            }
            // buffer will always return zero'd memory
            return buffer.Allocate(1);
        }

        void Free(T* allocation) {

            BlockHeader* blockHeader = (BlockHeader*) allocation;
            blockHeader->next = allocator.next;
            allocator.next = blockHeader;
        }

    private:

        struct BlockHeader {
            BlockHeader* next {};
        };

        PagedAllocator<T> buffer;
        BlockHeader allocator;

    };

}