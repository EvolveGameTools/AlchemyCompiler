#pragma once
#include "./Scanning.h"
#include "./LineColumn.h"
#include "../Collections/PodList.h"

namespace Alchemy::Compilation {

    struct TokenizerResult {
        CheckedArray<char*> texts;
        CheckedArray<SyntaxToken> tokens;
    };

    TokenizerResult Tokenize(TextWindow textWindow, Diagnostics * diagnostics, LinearAllocator * allocator);

}
