#pragma once
#include "./Scanning.h"
#include "./LineColumn.h"
#include "../Collections/PodList.h"

namespace Alchemy::Compilation {

    void ComputeTokenLineColumns(CheckedArray<SyntaxToken> tokens, CheckedArray<LineColumn> lineCols);
    void PrintTokens(CheckedArray<SyntaxToken> tokens, CheckedArray<LineColumn> lineCols);
    void Tokenize(TextWindow * textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens);
    void LexStringLiteral(TextWindow* textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens);
    void LexCharacterLiteral(TextWindow* textWindow, Diagnostics * diagnostics, PodList<SyntaxToken> * tokens);

}