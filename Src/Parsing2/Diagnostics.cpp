#include <cstring>
#include "./Diagnostics.h"
#include "../Allocation/BytePoolAllocator.h"

namespace Alchemy::Compilation {

    ScanningError::ScanningError(ErrorCode errorCode, char* start, char* end)
            : errorCode(errorCode)
              , start(start)
              , end(end)
              , messageLength(0)
              , message(nullptr) {}

    Diagnostics::Diagnostics(Alchemy::BytePoolAllocator* allocator)
            : allocator(allocator)
              , capacity(32)
              , cnt(0)
              , errorList((ScanningError*) allocator->AllocateUncleared(sizeof(ScanningError) * capacity)) {}

    void Diagnostics::AddScanningError(ScanningError error) {
        if (cnt + 1 > capacity) {
            ScanningError* newList = (ScanningError*) allocator->AllocateUncleared(sizeof(ScanningError) * capacity * 2);
            memcpy(newList, errorList, sizeof(ScanningError) * cnt);
            allocator->Free(errorList, capacity * sizeof(ScanningError));
            errorList = newList;
            capacity *= 2;
        }
        errorList[cnt++] = error;
    }

}