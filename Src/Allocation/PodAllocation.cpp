#include "./PodAllocation.h"

#if ALCHEMY_MALLOC_DEBUG != 0
#if 1 || ALCHEMY_ELECTRIC_MEMORY != 0

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

static size_t kPageSize = []() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
}();

#else

#ifdef __APPLE__
#include <unistd.h>
#include <sys/mman.h> // For macOS and iOS
static size_t kPageSize = []() {
    long page_size = sysconf(_SC_PAGESIZE);
    return static_cast<size_t>(page_size);
}();
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static size_t kPageSize = []() {
    long page_size = sysconf(_SC_PAGESIZE);
    return static_cast<size_t>(page_size);
}();

#endif
#endif // _Win32
#endif // ALCHEMY_ELECTRIC_MEMORY

#include <cstring>
#include <mutex>
#include "../PrimitiveTypes.h"
#include "../Panic.h"

#pragma message("debug malloc enabled")

namespace MallocDebug {

    struct ProtectableHeader {
        size_t sizeInPages;
        size_t offset;
    };

    void FreeElectric(void * ptr) {

        uint8* alloc = (uint8*) ptr;
        ProtectableHeader* header = (ProtectableHeader*) (alloc - sizeof(ProtectableHeader));
        byte* memory = alloc - header->offset;

        byte* p = (byte*)ptr;
        size_t remaining = (uint64)p % kPageSize;
        byte* firstWritablePage = p - remaining;

        for (int i = 0; i < remaining; i++) {
            if (firstWritablePage[i] != 0xED) {
                Alchemy::Panic(Alchemy::PanicType::MemoryCorruption, nullptr);
            }
        }

        memset(memory, 0xcc, kPageSize * header->sizeInPages);

        VirtualFree(firstWritablePage, header->sizeInPages * kPageSize, MEM_DECOMMIT);

    }

    /*

     1 page before allocation, made non writable
     top bytes are the header
     the rest are 0xcc
     allocation shifted to it ends on the page boundary
     the space between page-start and first allocated byte are set to 0xcc
     1 page after the allocation, made non writable
     all bytes set to 0xcc

    */

    uintptr_t RoundDownToBufferMultiple(uintptr_t ptr) {
        return (ptr & (kPageSize - 1)) == 0 ? 0 : ptr & ~(kPageSize - 1);
    }

    void* MallocProtectable(size_t size) {

        // do we need to make sure size is aligned? not sure it matters since we're debugging anyway if this is on  

        size_t sizeInPages = size / kPageSize;
        size_t allocatedSize = ((sizeInPages + 2) * kPageSize);

        byte* virtualAlloc = (byte*) VirtualAlloc(nullptr, allocatedSize, MEM_COMMIT, PAGE_READWRITE);

        byte* preamblePage = virtualAlloc;
        byte* postamblePage = virtualAlloc + (allocatedSize - kPageSize);
        byte* firstUserPage = virtualAlloc + kPageSize;

        size_t offset = kPageSize - (size % kPageSize);

        if(offset == kPageSize) {
            offset = 0;
        }

        byte* retn = firstUserPage + offset; // the pointer we actually return to the user

        ProtectableHeader* header = (ProtectableHeader*) (preamblePage);
        header->sizeInPages = sizeInPages;
        header->offset = offset; // we use this to verify that we did the right pointer math later

        // allocate 1 page before our target page range and 1 range after
        // we will protect these to make them unwritable to catch overflow/underflow

        DWORD oldProtect;
        VirtualProtect(preamblePage, kPageSize, PAGE_NOACCESS, &oldProtect);
        VirtualProtect(postamblePage, kPageSize, PAGE_NOACCESS, &oldProtect);

        return retn;
    }

    struct MemoryTracking {
        uint8* ptr;
        const char* file;
        size_t byteCount;
    };

    struct ListSize {
        int32 size;
        int32 capacity;
    };

    std::mutex gMallocMutex;
    ListSize trackingListSize = {0, 256};
    ListSize freeListSize = {0, 64};

    static MemoryTracking* trackingList = []() {
        return (MemoryTracking*) malloc(sizeof(MemoryTracking) * trackingListSize.capacity);
    }();

    static int32* freeList = []() {
        return (int32*) malloc(sizeof(int32) * freeListSize.capacity);
    }();


    static unsigned char End[] = {
        0xA5, 0xA5, 0xA5, 0xA5,    /* odd */
        0x5B, 0x5B, 0x5B, 0x5B,    /* odd */
        0xAB, 0xAB, 0xAB, 0xAB,    /* odd */
        0xAA, 0x55, 0xAA, 0x55    /* odd */
    };

    uint64 Align(size_t number, size_t alignment) {
        return ((number + (alignment - 1)) & (-alignment));
    }

    void* malloc_impl(size_t byteCount) {
        return malloc(byteCount);
    }

    void free_impl(void* ptr, size_t byteCount) {
        free(ptr);
    }

    void* malloc_tracked(size_t byteCount, const char* location) {
        size_t aligned = Align(byteCount + sizeof(End) + sizeof(uint64), 16);
        uint8* bytes = (uint8*) malloc_impl(aligned);
        uint8* pEnd = bytes + byteCount;
        int32* pIdx = (int32*) (pEnd + sizeof(End));
        memcpy(pEnd, End, sizeof(End));

        MemoryTracking* pTracking;
        {
            std::unique_lock lock(gMallocMutex);

            if (freeListSize.size > 0) {
                int32 idx = freeList[freeListSize.size - 1];
                freeListSize.size--;
                pTracking = &trackingList[idx];
            }
            else {
                if (trackingListSize.size + 1 >= trackingListSize.capacity) {
                    trackingListSize.capacity *= 2;
                    MemoryTracking* newList = (MemoryTracking*) malloc(sizeof(MemoryTracking) * trackingListSize.capacity);
                    memcpy(newList, trackingList, sizeof(MemoryTracking) * trackingListSize.size);
                    free(trackingList);
                    trackingList = newList;
                }
                pTracking = &trackingList[trackingListSize.size];
                trackingListSize.size++;
            }
        }

        pTracking->byteCount = byteCount;
        pTracking->ptr = bytes;
        pTracking->file = location;
        *pIdx = trackingListSize.size - 1;

        return bytes;
    }

    void* malloc_tracked_cleared(size_t byteCount, const char* location) {
        void* retn = malloc_tracked(byteCount, location);
        memset(retn, 0, byteCount);
        return retn;
    }

    void free_tracked(void* ptr, size_t byteCount, const char* location) {

        if (ptr == nullptr) {
            return;
        }

        std::unique_lock lock(gMallocMutex);

        uint8* bytes = (uint8*) ptr;
        uint8* pEnd = bytes + byteCount;
        int32 listIndex = *(int32*) (pEnd + sizeof(End));
        MemoryTracking* tracked = &trackingList[listIndex];

        bool hasCorruption = memcmp(pEnd, End, sizeof(End)) != 0 || listIndex < 0 || listIndex > trackingListSize.size;

        if (hasCorruption || tracked->ptr != bytes) {

            // bad, memory corruption. linear search to find the pointer and report it

            for (int32 i = 0; i < trackingListSize.size; i++) {
                if (trackingList[i].ptr == bytes) {

                    if (tracked->byteCount != byteCount) {
                        uint64 byteInfo[3];
                        byteInfo[0] = tracked->byteCount;
                        byteInfo[1] = byteCount;
                        byteInfo[2] = reinterpret_cast<uint64>(location);
                        Panic(Alchemy::PanicType::PartialFree, (void*) byteInfo);
                        return;
                    }

                    const char* data[2];
                    data[0] = trackingList[i].file;
                    data[1] = location;

                    Panic(Alchemy::PanicType::MemoryCorruption, (void*) data);
                    return;
                }
            }

            // if we get here we didn't find it, was either already free'd or the pointer is invalid
            Panic(Alchemy::PanicType::InvalidFree, (void*) location);
            return;
        }

        if (freeListSize.size + 1 >= freeListSize.capacity) {
            freeListSize.capacity *= 2;
            int32* newList = (int32*) malloc(sizeof(int32) * freeListSize.capacity);
            memcpy(newList, freeList, sizeof(int32) * freeListSize.size);
            free(freeList);
            freeList = newList;
        }

        freeList[freeListSize.size] = listIndex;
        freeListSize.size++;

        trackingList[listIndex].ptr = nullptr;
        trackingList[listIndex].byteCount = 0;
        trackingList[listIndex].file = nullptr;

        free_impl(bytes, byteCount);

    }

    void ReportAndFreeLeakedAllocations() {
        std::unique_lock lock(gMallocMutex);
        for (int32 i = 0; i < trackingListSize.size; i++) {
            MemoryTracking* pTracking = &trackingList[i];
            if (pTracking->ptr != nullptr) {
                printf("Leak Detected! %zu bytes allocated from %s\n", pTracking->byteCount, pTracking->file);
            }
        }

        for (int32 i = 0; i < trackingListSize.size; i++) {
            MemoryTracking* pTracking = &trackingList[i];
            if (pTracking->ptr != nullptr) {
                free_impl(pTracking->ptr, pTracking->byteCount);
            }
        }
        memset(trackingList, 0, sizeof(MemoryTracking) * trackingListSize.capacity);
        memset(freeList, 0, sizeof(int32) * freeListSize.capacity);
        trackingListSize.size = 0;
        freeListSize.size = 0;
    }

}
#else
void MallocDebug::ReportAndFreeLeakedAllocations() {
    // no op
}
#endif


