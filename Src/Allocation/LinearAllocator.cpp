#include "./LinearAllocator.h"
#include <cstdint>

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
#endif

static constexpr size_t kGigabyte = 1073741824;

static size_t Ceilpow2(size_t x) {
    x -= 1;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x + 1;
}

size_t ComputeMinCommitSize(size_t declaredSize) {
    if (declaredSize <= 0) {
        declaredSize = 16 * 1024;
    }

    declaredSize = Ceilpow2(declaredSize);

    if (declaredSize < 8 * 1024) {
        declaredSize = 8 * 1024;
    }

    return declaredSize;

}

namespace Alchemy {
    LinearAllocator::LinearAllocator(size_t reserveSize, size_t commitSize)
        : reserved(reserveSize)
        , committed(0)
        , base()
        , offset(0)
        , minCommitStep(ComputeMinCommitSize(commitSize)) {
        // reserve at least 1 gb
        if (reserved < kGigabyte) {
            reserved = kGigabyte;
        }

#if defined(_WIN32)
        base = (uint8*) VirtualAlloc(nullptr, reserved, MEM_RESERVE, PAGE_READWRITE);
#else
        base = (uint8*)mmap(nullptr, reserved, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif

    }

    TempAllocator::TempAllocator(size_t reservation, size_t commitSize)
        : LinearAllocator(reservation, commitSize) {}

    LinearAllocator::~LinearAllocator() {
        if (base == nullptr) {
            return;
        }
#ifdef _WIN32
        VirtualFree(base, 0, MEM_RELEASE);
#else
        munmap(base, reserved);
#endif
        base = nullptr;
        offset = 0;
        reserved = 0;
        committed = 0;
    }

    uint8* LinearAllocator::AllocateBytesUncleared(size_t size, size_t alignment) {

        if ((alignment & (alignment - 1)) != 0) {
            alignment = Ceilpow2(alignment);
        }

        uint8* unalignedptr = base + offset;
        uint8* alignedptr = reinterpret_cast<uint8*>((reinterpret_cast<size_t>(unalignedptr) + alignment - 1) & ~(alignment - 1));

        size_t alignmentDiff = alignedptr - unalignedptr;

        offset += size + alignmentDiff;

        if (offset > committed) {
            size_t growBy = offset - committed;
            growBy = growBy < minCommitStep ? minCommitStep : growBy;
            growBy = (growBy + kPageSize - 1) & ~(kPageSize - 1); // round to page size
#if defined(_WIN32)
            if (!VirtualAlloc(base + committed, growBy, MEM_COMMIT, PAGE_READWRITE)) {
                return nullptr; // abort?
            }
#else
            if(!mmap(base + committed, growBy, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) {
                return nullptr; // abort?
            }
#endif
            committed += growBy;
        }

        return (uint8*) alignedptr;
    }

    size_t LinearAllocator::GetOffset(void* ptr) {
        uint8* bytePtr = (uint8*) ptr;
        if (bytePtr < base || bytePtr > base + offset) {
            return -1;
        }
        return bytePtr - base;
    }

    uint8* LinearAllocator::GetBase() {
        return base;
    }

    TempAllocator::Marker TempAllocator::MarkerFromOffset(void* p) {
        if (p == nullptr || p < base || p >= base + committed) {
            return Mark();
        }

        return TempAllocator::Marker(static_cast<uint64_t>((uint8*) p - base));
    }


}
