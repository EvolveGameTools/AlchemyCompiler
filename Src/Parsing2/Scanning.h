#pragma once

#include "../PrimitiveTypes.h"
#include "../Util/FixedCharSpan.h"
#include "SyntaxKind.h"

namespace Alchemy::Parsing {

    struct TextWindow;
    struct SyntaxDiagnosticInfo;

    enum class LiteralType :uint8 {
        None,
        Bool,
        Char,
        Float,
        Double,
        Int32,
        Int64,
        UInt32,
        UInt64
    };

    enum class TriviaType : uint8 {
        Whitespace,
        SingleLineComment,
        MultiLineComment,
        NewLine,
        BadToken,
        SkippedTokens
    };

    struct Trivia {
        char * span;
        uint32 length;
        TriviaType type;
    };

    struct TokenInfo {

        Alchemy::FixedCharSpan text;
        SyntaxKind Kind;
        SyntaxKind ContextualKind;
        LiteralType valueKind;

        union {
            bool boolValue;
            char charValue;
            int32 int32Value;
            uint32 uint32Value;
            int64 int64Value;
            uint64 uint64Value;
            float floatValue;
            double doubleValue;
        };
    };

    bool IsNewline(char32 c);
    bool IsIdentifierStartCharacter(uint32 codepoint);
    uint32 ScanUnicodeEscape(TextWindow* textWindow, SyntaxDiagnosticInfo* error);
    bool ScanIdentifier(TextWindow* textWindow, FixedCharSpan * identifier);
    bool ScanIdentifier_FastPath(TextWindow* textWindow, FixedCharSpan * identifier);
    bool ScanIdentifier_SlowPath(TextWindow* textWindow, FixedCharSpan * identifier);
    void ScanWhitespace(TextWindow* textWindow, FixedCharSpan * whitespace);
    void ScanToEndOfLine(TextWindow * textWindow, FixedCharSpan * span);
    void ScanSingleLineComment(TextWindow* textWindow, FixedCharSpan* span);
    void ScanMultiLineComment(TextWindow* textWindow, FixedCharSpan* span, bool* isTerminated);
    bool TryMatchKeyword_Generated(char * buffer, int32 length, SyntaxKind * keywordType);

}