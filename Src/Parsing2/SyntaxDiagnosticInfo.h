#pragma once

#include "./ErrorCode.h"

namespace Alchemy::Parsing {

    struct SyntaxDiagnosticInfo {
        char* source {};
        char* position {};
        char* end {};
        ErrorCode errorCode {};
    };

}