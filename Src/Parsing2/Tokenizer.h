#pragma once
#include "./Scanning.h"
#include "../Collections/PodList.h"

namespace Alchemy::Compilation {

    struct LineColumn {
        int32 line;
        int32 column;
        int32 endLine;
        int32 endColumn;
    };

    void ComputeTokenLineColumns(CheckedArray<SyntaxToken> tokens, CheckedArray<LineColumn> lineCols);
    void PrintTokens(CheckedArray<SyntaxToken> tokens, CheckedArray<LineColumn> lineCols);
    void Tokenize(TextWindow * textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens);

}