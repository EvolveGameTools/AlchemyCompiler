#include "./BytePoolAllocator.h"

namespace Alchemy {

    int32 CeilPow2(int32 x) {
        x -= 1;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x + 1;
    }

    void FreeBlock(BytePoolAllocator::Block** blk, void* ptr) {
        BytePoolAllocator::Block* b = (BytePoolAllocator::Block*) ptr;
        b->next = *blk;
        *blk = b;
    }

    void* AllocateBlock(BytePoolAllocator::Block** ptr, size_t allocSize, LinearAllocator* allocator) {
        if (*ptr == nullptr) {
            return allocator->AllocateUncleared<uint8>(allocSize);
        }
        BytePoolAllocator::Block* retnBlock = *ptr;
        if (retnBlock->next != nullptr) {
            *ptr = retnBlock->next;
        }
        return retnBlock;
    }

    BytePoolAllocator::BytePoolAllocator(LinearAllocator* allocator)
            : allocator(allocator)
              , b32(nullptr)
              , b64(nullptr)
              , b128(nullptr)
              , b256(nullptr)
              , b512(nullptr)
              , b1024(nullptr)
              , b2048(nullptr)
              , b4096(nullptr) {}

    void* BytePoolAllocator::Allocate(size_t bytes) {
        void* ptr = AllocateUncleared(bytes);
        memset(ptr, 0, bytes);
        return ptr;
    }

    void* BytePoolAllocator::AllocateUncleared(size_t bytes) {
        size_t allocBytes = CeilPow2(bytes);

        if (allocBytes < 32) {
            allocBytes = 32;
        }

        switch (allocBytes) {
            case 32:
                return AllocateBlock(&b32, allocBytes, allocator);
            case 64:
                return AllocateBlock(&b64, allocBytes, allocator);
            case 128:
                return AllocateBlock(&b128, allocBytes, allocator);
            case 256:
                return AllocateBlock(&b256, allocBytes, allocator);
            case 512:
                return AllocateBlock(&b512, allocBytes, allocator);
            case 1024:
                return AllocateBlock(&b1024, allocBytes, allocator);
            case 2048:
                return AllocateBlock(&b2048, allocBytes, allocator);
            case 4096:
                return AllocateBlock(&b4096, allocBytes, allocator);
            default:
                return allocator->AllocateUncleared<uint8>(bytes);
        }
    }

    void BytePoolAllocator::Free(void* ptr, size_t bytes) {
        size_t allocBytes = CeilPow2(bytes);

        if (allocBytes < 32) {
            allocBytes = 32;
        }

        switch (allocBytes) {
            case 32:
                return FreeBlock(&b32, ptr);
            case 64:
                return FreeBlock(&b64, ptr);
            case 128:
                return FreeBlock(&b128, ptr);
            case 256:
                return FreeBlock(&b256, ptr);
            case 512:
                return FreeBlock(&b512, ptr);
            case 1024:
                return FreeBlock(&b1024, ptr);
            case 2048:
                return FreeBlock(&b2048, ptr);
            case 4096:
                return FreeBlock(&b4096, ptr);
            default: {
                return; // anything bigger we just leak into the allocator
            }
        }
    }

}