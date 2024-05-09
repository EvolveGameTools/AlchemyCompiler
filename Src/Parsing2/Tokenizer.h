#pragma once
#include "./Scanning.h"
#include "../Collections/PodList.h"

namespace Alchemy::Compilation {

    struct LineColumn {
        int32 line;
        int32 column;
    };

    void ComputeTokenLineColumns(CheckedArray<SyntaxToken> hotTokens, CheckedArray<SyntaxTokenCold> coldTokens, CheckedArray<LineColumn> lineCols);
    void PrintTokens(CheckedArray<SyntaxToken> hotTokens, CheckedArray<SyntaxTokenCold> coldTokens, CheckedArray<LineColumn> lineCols);
    void Tokenize(TextWindow * textWindow, LinearAllocator* allocator, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens, PodList<SyntaxTokenCold>* coldTokens, PodList<Trivia>* triviaBuffer);

}