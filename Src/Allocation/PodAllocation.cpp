#include "PodAllocation.h"
#include <new>
#include <cstring>

typedef uint8* (* MallocFn)(size_t size, size_t alignment, const char * file, int32 line);
typedef uint8* (* FreeFn)(void* ptr, size_t alignment);

MallocFn pMallocFn;
FreeFn pFreeFn;

extern "C" DLLEXPORT
void SetMallocFree(MallocFn mallocFn, FreeFn freeFn) {
    pMallocFn = mallocFn;
    pFreeFn = freeFn;
}

extern "C" DLLEXPORT
void* MallocByteArrayImpl(size_t size, size_t align, const char* file, int32 line) {
    if (pMallocFn != nullptr) {
        return pMallocFn(size, align, file, line);
    }
    return ::operator new(size, std::align_val_t {align});
}

extern "C" DLLEXPORT
void* MallocByteArrayClearedImpl(size_t size, size_t align, const char* file, int32 line) {
    void* retn = MallocByteArrayImpl(size, align, file, line);
    memset(retn, 0, size);
    return retn;
}

extern "C" DLLEXPORT
void FreeByteArray(void* array, size_t align) {
    if (array == nullptr) return;
    if (pFreeFn != nullptr) {
        pFreeFn(array, align);
    }
    else {
        ::operator delete[](array, std::align_val_t {align});
    }
}

#if 0
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

constexpr int32 kPageSize = 4096;

struct ProtectableHeader {
    int32 sizeInPages;
    int32 offset;
};

extern "C" DLLEXPORT
void* MallocProtectable(size_t size) {

    size += sizeof(ProtectableHeader);

    // todo -- likely still want to push this so the allocation aligns with buffer end to catch out of bounds writes

    // if size < page size we want to compute an offset to fill the remaining data with 0xed
    int32 offset = 0;
    if(size < kPageSize) {
        offset = kPageSize - size;
    }

    int32 sizeInPages = size / kPageSize;
    if (size % kPageSize != 0) {
        sizeInPages++;  // account for the remainder
    }

    // allocate 1 page before our target page range and 1 range after
    // we will protect these to make them unwritable to catch overflow/underflow
    int32 allocatedSize = ((sizeInPages + 2) * kPageSize);

    byte* virtualAlloc = (byte*) VirtualAlloc(nullptr, allocatedSize, MEM_COMMIT, PAGE_READWRITE);

    DWORD oldProtect;
    VirtualProtect(virtualAlloc, kPageSize, PAGE_NOACCESS, &oldProtect);

    VirtualProtect(virtualAlloc + ((sizeInPages + 1) * kPageSize), kPageSize, PAGE_NOACCESS, &oldProtect);

    byte* alloc = virtualAlloc + kPageSize;
    byte* retn = alloc + offset + sizeof(ProtectableHeader);

    ProtectableHeader* header = (ProtectableHeader*) (alloc + offset);
    header->sizeInPages = sizeInPages;
    header->offset = offset;

    return retn;

}

extern "C" DLLEXPORT
void MakeProtectableReadOnly(void* ptr) {
    uint8* alloc = (uint8*) ptr;
    ProtectableHeader* header = (ProtectableHeader*) (alloc - sizeof(ProtectableHeader));
    uint8* memory = alloc - header->offset;

    DWORD oldProtect;
    VirtualProtect(memory, kPageSize * header->sizeInPages, PAGE_READONLY, &oldProtect);

}

extern "C" DLLEXPORT
void FreeProtectable(void* ptr) {

    uint8* alloc = (uint8*) ptr;
    ProtectableHeader* header = (ProtectableHeader*) (alloc - sizeof(ProtectableHeader));

    byte* memory = alloc - header->offset;

    // for (int i = 0; i < remaining; i++) {
    //     assert(memory[i] != 0xED);
    // }

    memset(memory, 0xcc, kPageSize * header->sizeInPages);

    // decommit, not release, they are not available for reuse again, any future access will throw
    VirtualFree(memory, header->sizeInPages * kPageSize, MEM_DECOMMIT);

}

#endif