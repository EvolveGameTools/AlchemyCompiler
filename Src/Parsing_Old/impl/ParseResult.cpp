#include <cstdio>
#include "../ParseResult.h"
#include "../Nodes.h"

namespace Alchemy::Parsing {

    bool ParseResult::HasValidParse() const {
        return !(hasUnmatchedDelimiters || hasBadCharacters || hasUnmatchedDelimiters);
    }

    FileNode* ParseResult::GetRootNode() {
        if(psiNodes.size >= 1) {
            AbstractPsiNode * pNode = psiNodes.GetPointer(1);
            if(pNode->nodeBase.nodeType != NodeType::File) {
                return nullptr;
            }
            return (FileNode*)pNode;
        }
        return nullptr;
    }

    LineColumn ParseResult::GetLocationFromToken(int32 token) {
        return GetLocationFromByteOffset(tokens[token].position);
    }

    LineColumn ParseResult::GetLocationFromNodeIndex(int32 nodeIndex) {
        int32 token =  psiNodes[nodeIndex].nodeBase.tokenStart;
        return GetLocationFromByteOffset(tokens[token].position);
    }

    LineColumn ParseResult::GetLocationFromByteOffset(int32 position) const {
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

    void ParseResult::PrintErrors() {

        for (int32 i = 0; i < errors.size; i++) {
            ParseError* error = errors.GetPointer(i);
            LineColumn lineColumn = GetLocationFromByteOffset(error->sourceStart);
            printf("Error: %.*s (%d:%d)", error->messageLength, error->message, lineColumn.line, lineColumn.column);
        }

    }
}