#pragma once
#include "./LinearAllocator.h"

namespace Alchemy {

    struct BytePoolAllocator {

        struct Block {
            Block * next;
        };

        Block* b32;
        Block* b64;
        Block* b128;
        Block* b256;
        Block* b512;
        Block* b1024;
        Block* b2048;
        Block* b4096;
        LinearAllocator * allocator;

        explicit BytePoolAllocator(LinearAllocator * allocator);

        void * Allocate(size_t bytes);

        void * AllocateUncleared(size_t bytes);

        void Free(void * ptr, size_t bytes) ;

    };

}