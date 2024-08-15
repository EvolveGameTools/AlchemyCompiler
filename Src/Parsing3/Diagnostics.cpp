#include <cstring>
#include "./Diagnostics.h"

namespace Alchemy::Compilation {

    Diagnostic::Diagnostic(ErrorCode errorCode, char* start, char* end)
        : errorCode(errorCode)
        , start(start)
        , end(end)
        , messageLength(0)
        , message(nullptr) {}

    Diagnostic::Diagnostic(ErrorCode errorCode, Alchemy::FixedCharSpan span)
        : errorCode(errorCode)
        , start(span.ptr)
        , end(span.ptr + span.size)
        , messageLength(0)
        , message(nullptr) {}

    Diagnostic::Diagnostic(ErrorCode errorCode, FixedCharSpan span, FixedCharSpan message)
        : errorCode(errorCode)
        , start(span.ptr)
        , end(span.ptr + span.size)
        , messageLength(message.size)
        , message(message.ptr) {}

    Diagnostics::Diagnostics()
        : diagnostics() {}

    void Diagnostics::AddError(Diagnostic error) {
        diagnostics.Add(error);
        if(error.errorCode == ErrorCode::ERR_IdentifierExpected) {
            puts("bad");
        }
    }

    void Diagnostics::AddError(ErrorCode error, FixedCharSpan sourceSpan) {
        AddError(Diagnostic(error, sourceSpan));
    }

    void Diagnostics::AddError(ErrorCode error, FixedCharSpan sourceSpan, FixedCharSpan message) {
        AddError(Diagnostic(error, sourceSpan ,message));
    }

}