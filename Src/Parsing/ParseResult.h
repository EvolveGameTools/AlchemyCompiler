#pragma once

#include "./Token.h"
#include "../Collections/CheckedArray.h"
#include "./Nodes.generated.h"

namespace Alchemy {

    using namespace Alchemy;

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

        inline bool HasValidParse() const {
            return !(hasUnmatchedDelimiters || hasBadCharacters || hasUnmatchedDelimiters);
        }

        inline FileNode * GetRootNode() {
            if(psiNodes.size >= 1) {
                AbstractPsiNode * pNode = psiNodes.GetPointer(1);
                if(pNode->nodeBase.nodeType != NodeType::File) {
                    return nullptr;
                }
                return (FileNode*)pNode;
            }
            return nullptr;
        }

        inline LineColumn GetLocationFromToken(int32 token) {
            return GetLocationFromByteOffset(tokens[token].position);
        }

        inline LineColumn GetLocationFromNodeIndex(int32 nodeIndex) {
            int32 token =  psiNodes[nodeIndex].nodeBase.tokenStart;
            return GetLocationFromByteOffset(tokens[token].position);
        }

        inline LineColumn GetLocationFromByteOffset(int32 position) const {
            LineColumn retn;
            retn.line = 1;
            retn.column = 1;
            for (int32 i = 0; i < position; i++) {
                char c = src[i];
                if (c == '\n') {
                    retn.line++;
                    retn.column = 1; // Reset column number for new line
                }
                else if(c == '\r') {
                    if(i + 1 < position && src[i + 1] == '\n') {
                        i++;
                        retn.line++;
                        retn.column = 1; // Reset column number for new line
                    }
                }
                else {
                    retn.column++;
                }
            }

            return retn;
        }

        inline void PrintErrors() {

            for (int32 i = 0; i < errors.size; i++) {
                ParseError* error = errors.GetPointer(i);
                LineColumn lineColumn = GetLocationFromByteOffset(error->sourceStart);
                printf("Error: %.*s (%d:%d)", error->messageLength, error->message, lineColumn.line, lineColumn.column);
            }

        }

    };

}