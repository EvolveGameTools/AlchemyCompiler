#pragma once

#include "../Allocation/LinearAllocator.h"

namespace Alchemy::Compilation {

    template<typename T>
    struct PooledPointerBlock {

        T** array;
        int32 capacityInItems;

        PooledPointerBlock()
                : array(nullptr), capacityInItems(0) {}

        PooledPointerBlock(T** memory, int32 count)
                : array(memory), capacityInItems(count) {}

        int32 CapacityInBytes() {
            return capacityInItems * sizeof(T);
        }

    };

    struct PoolingAllocator {
        struct BlockHeader {
            BlockHeader* next{};
        };

        struct BlockAllocator {
            BlockHeader* next;

            BlockAllocator()
                    : next(nullptr) {}
        };

        LinearAllocator* linearAllocator;

        explicit PoolingAllocator(LinearAllocator * linearAllocator) : linearAllocator(linearAllocator) {}

        BlockAllocator block8;
        BlockAllocator block16;
        BlockAllocator block32;
        BlockAllocator block64;
        BlockAllocator block128;
        BlockAllocator block256;
        BlockAllocator block512;
        BlockAllocator block1024;
        BlockAllocator block2048;
        BlockAllocator block4096;
        BlockAllocator block8192;
        BlockAllocator block16384;
        BlockAllocator block32768;
        BlockAllocator block65536;
        BlockAllocator block131072;

        template<class T>
        T* Allocate(int32 count) {
            T * retn = linearAllocator->AllocateUncleared<T>(count);
            memset(retn, 0, sizeof(T) * count);
            return retn;
        }

        template<class T>
        T* AllocateUncleared(int32 count) {
            T * retn = linearAllocator->AllocateUncleared<T>(count);
            memset(retn, 0, sizeof(T) * count);
            return retn;
        }

        template<class T>
        T* AllocatePooled(int32 count) {
            T * retn = AllocatePooledUncleared<T>(count);
            memset(retn, 0, sizeof(T) * count);
            return retn;
        }


        template<typename TType, typename... Args>
        TType* New(Args &&... args) {
            TType* retn = linearAllocator->AllocateUncleared<TType>(1);
            new(retn) TType(std::forward<Args>(args)...);
            return retn;
        }

        template<class T>
        T* AllocatePooledUncleared(int32 count) {
            int32 bytes = (int32)(sizeof(T)) * count;

            return linearAllocator->AllocateUncleared<T>(bytes);

            // todo -- we need the log pow2 for this to work

            int32 pow2Count = Alchemy::MathUtil::CeilPow2(bytes); // compute the next power of 2
            static_assert(sizeof(T) >= 8); // make sure we allocate at least 16 bytes
            int32 offset = (1 << pow2Count) - 8; // find the offset of the block field we care about
            BlockAllocator* allocator = &block8 + offset;

            if (allocator > &block131072 || allocator->next == nullptr) {
                return linearAllocator->AllocateUncleared<T>(bytes);
            }

            BlockHeader* ptr = allocator->next;
            allocator->next = ptr->next;
            return (T*) ptr;
        }

        template<class T>
        void Free(T* item, int32 count) {
            if(item == nullptr || count <= 0) {
                return;
            }
            int32 bytes = (int32)(sizeof(T)) * count;
            int32 pow2Count = Alchemy::MathUtil::CeilPow2(bytes); // compute the next power of 2
            static_assert(sizeof(T) >= 8); // make sure we allocate at least 16 bytes
            // we could assert the item pointer is within our allocation range
            int32 offset = (1 << pow2Count) - 8; // find the offset of the block field we care about
            BlockAllocator* allocator = &block8 + offset;

            if (allocator > &block131072) {
                return; // leak large allocations I guess?
            }

            BlockHeader* blockHeader = (BlockHeader*) item;
            blockHeader->next = allocator->next;
            allocator->next = blockHeader;
        }

    };

    struct PooledPointerBlockAllocator {

        struct BlockHeader {
            BlockHeader* next{};
        };

        struct BlockAllocator {
            BlockHeader* next;

            BlockAllocator()
                    : next(nullptr) {}
        };

        LinearAllocator* linearAllocator;

        BlockAllocator block2;
        BlockAllocator block4;
        BlockAllocator block8;
        BlockAllocator block16;
        BlockAllocator block32;
        BlockAllocator block64;
        BlockAllocator block128;
        BlockAllocator block256;
        BlockAllocator block512;
        BlockAllocator block1024;
        BlockAllocator block2048;
        BlockAllocator block4096;
        BlockAllocator block8192;
        BlockAllocator block16384;
        BlockAllocator block32768;

        explicit PooledPointerBlockAllocator(LinearAllocator* linearAllocator)
                : linearAllocator(linearAllocator) {}

        template<typename TType, typename... Args>
        TType* New(Args &&... args) {
            return linearAllocator->template New<TType>(std::forward<Args>(args)...);
        }

        template<typename T>
        T* Allocate(int32 cnt) {
            return linearAllocator->Allocate<T>(cnt);
        }

        template<typename T>
        T* AllocateUncleared(int32 cnt) {
            return linearAllocator->AllocateUncleared<T>(cnt);
        }

        template<typename T>
        PooledPointerBlock<T> AllocateList(int32 count) {
            PooledPointerBlock<T> retn = AllocateListUncleared<T>(count);
            memset(retn.array, 0, sizeof(T*) * retn.capacityInItems);
            return retn;
        }

        template<typename T>
        PooledPointerBlock<T> AllocateListUncleared(int32 count) {

            int32 pow2Count = Alchemy::MathUtil::CeilPow2(count);

            if (pow2Count < 2) pow2Count = 2;

            BlockAllocator* block;

            switch (pow2Count) {
                case 2: {
                    block = &block2;
                    break;
                }
                case 4:
                    block = &block4;
                    break;

                case 8:
                    block = &block8;
                    break;

                case 16:
                    block = &block16;
                    break;

                case 32:
                    block = &block32;
                    break;

                case 64:
                    block = &block64;
                    break;

                case 128:
                    block = &block128;
                    break;

                case 256:
                    block = &block256;
                    break;

                case 512:
                    block = &block512;
                    break;

                case 1024:
                    block = &block1024;
                    break;

                default: {
                    int32 cnt = (int32) (count * 1.5f);
                    T** alloc = linearAllocator->Allocate<T*>(cnt);
                    return PooledPointerBlock<T>(alloc, cnt);
                }
            }

            if (block->next != nullptr) {
                BlockHeader* ptr = block->next;
                block->next = ptr->next;
                return PooledPointerBlock<T>((T**) ptr, pow2Count);
            } else {
                T** ptr = linearAllocator->Allocate<T*>(pow2Count);
                return PooledPointerBlock<T>(ptr, pow2Count);
            }
        }

        template<typename T>
        void Free(PooledPointerBlock<T> allocation) {

            if (allocation.array == nullptr || allocation.capacityInItems == 0) {
                return;
            }

            BlockAllocator* block;

            switch (allocation.capacityInItems) {
                case 2: {
                    block = &block2;
                    break;
                }
                case 4:
                    block = &block4;
                    break;

                case 8:
                    block = &block8;
                    break;

                case 16:
                    block = &block16;
                    break;

                case 32:
                    block = &block32;
                    break;

                case 64:
                    block = &block64;
                    break;

                case 128:
                    block = &block128;
                    break;

                case 256:
                    block = &block256;
                    break;

                case 512:
                    block = &block512;
                    break;

                case 1024:
                    block = &block1024;
                    break;

                default: {
                    // we leak large allocations, they aren't tracked (and are pretty rare)
                    return;
                }
            }

            BlockHeader* blockHeader = (BlockHeader*) allocation.array;
            blockHeader->next = block->next;
            block->next = blockHeader;
        }

    };

}