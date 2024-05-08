#pragma once

#include "./ErrorCode.h"

namespace Alchemy::Compilation {

    enum class DiagnosticType {
        Info,
        Error,
        Warning,
        Suggestion
    };

    struct SyntaxDiagnosticInfo {
        char* text {};
        SyntaxDiagnosticInfo * prevDiagnostic {};
        DiagnosticType diagnosticType {};
        ErrorCode errorCode {};
        int32 textLength {};
        int32 offset;
    };

}