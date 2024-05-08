#pragma once
#include "./ErrorCode.h"
#include "../PrimitiveTypes.h"

namespace Alchemy {
    struct BytePoolAllocator;
}

namespace Alchemy::Compilation {

    struct ScanningError {
        char* start;
        char* end;
        char* message;
        uint32 messageLength;
        ErrorCode errorCode;

        ScanningError(ErrorCode errorCode, char* start, char* end);

    };

    struct Diagnostics {

        Alchemy::BytePoolAllocator* allocator;

        int32 cnt;
        int32 capacity;
        ScanningError* errorList;

        explicit Diagnostics(Alchemy::BytePoolAllocator * allocator);

        void AddScanningError(ScanningError error);

    };

}
