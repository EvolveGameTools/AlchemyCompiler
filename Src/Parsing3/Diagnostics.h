#pragma once

#include "./ErrorCode.h"
#include "../PrimitiveTypes.h"
#include "../Allocation/LinearAllocator.h"
#include "../Util/FixedCharSpan.h"
#include "../Collections/PodList.h"

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

        PodList<Diagnostic> diagnostics;

        explicit Diagnostics();

        void AddError(Diagnostic error);
        void AddError(ErrorCode error, FixedCharSpan sourceSpan);
        void AddError(ErrorCode error, FixedCharSpan sourceSpan, FixedCharSpan message);

    };

}
