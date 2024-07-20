#pragma once

#include "./ErrorCode.h"
#include "../PrimitiveTypes.h"
#include "../Allocation/LinearAllocator.h"
#include "../Util/FixedCharSpan.h"

namespace Alchemy::Compilation {

    struct Diagnostic {

        char* start;
        char* end;
        char* message;
        uint32 messageLength;
        ErrorCode errorCode;

        Diagnostic()
            : start(nullptr)
            , end(nullptr)
            , message(nullptr)
            , messageLength(0)
            , errorCode(ErrorCode::None) {}

        Diagnostic(ErrorCode code, FixedCharSpan span);

        Diagnostic(ErrorCode code, FixedCharSpan span, FixedCharSpan message);

        Diagnostic(ErrorCode errorCode, char* start, char* end);

    };

    struct Diagnostics {

        Allocator allocator;

        int32 size;
        int32 capacity;
        Diagnostic** array;

        explicit Diagnostics(Allocator allocator);

        void AddError(Diagnostic error);

        void AddError(ErrorCode error, FixedCharSpan sourceSpan);
        void AddError(ErrorCode error, FixedCharSpan sourceSpan, FixedCharSpan message);

    };

}
