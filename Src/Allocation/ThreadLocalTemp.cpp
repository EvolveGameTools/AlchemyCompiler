#include "./ThreadLocalTemp.h"
#include "../Allocation/PodAllocation.h"

namespace Alchemy {

    thread_local TempAllocator * ts_ThreadLocalAllocator;

    void DisposeThreadLocalAllocator() {
        Mfree(ts_ThreadLocalAllocator, sizeof(TempAllocator));
    }

    TempAllocator * GetThreadLocalAllocator() {

        if(ts_ThreadLocalAllocator == nullptr) {
            ts_ThreadLocalAllocator = (TempAllocator*)MallocateUncleared(sizeof(TempAllocator));
            new (ts_ThreadLocalAllocator) TempAllocator(GIGABYTES(1), KILOBYTES(64));
        }

        return ts_ThreadLocalAllocator;

    }

}