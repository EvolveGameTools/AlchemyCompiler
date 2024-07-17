#pragma once

#include "./Token.h"
#include "./Nodes.h"
#include "../Collections/CheckedArray.h"

namespace Alchemy::Parsing {

    struct LineColumn {
        int32 line {};
        int32 column {};
    };

    struct ParseError {
        char* message {};
        int32 messageLength {};
        int32 psiIndex {};
        int32 tokenStart {};
        int32 tokenEnd {};
        int32 sourceStart {};
        int32 sourceEnd {};
    };

    struct ParseResult {

        char* src {};
        int32 srcLength {};
        CheckedArray<Token> tokens;
        CheckedArray<int32> production;
        CheckedArray<AbstractPsiNode> psiNodes;
        CheckedArray<ParseError> errors;

        bool hasTooManyTokens {};
        bool hasBadCharacters {};
        bool hasUnmatchedDelimiters {};
        bool hasNonTrivialContent {};

        bool HasValidParse() const;

        FileNode * GetRootNode();

        LineColumn GetLocationFromToken(int32 token);
        LineColumn GetLocationFromNodeIndex(int32 nodeIndex);
        LineColumn GetLocationFromByteOffset(int32 position) const;

        void PrintErrors();

    };

}