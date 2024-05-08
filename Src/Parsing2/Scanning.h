#pragma once

#include "../PrimitiveTypes.h"
#include "../Util/FixedCharSpan.h"
#include "SyntaxKind.h"
#include "Diagnostics.h"
#include "./SyntaxDiagnosticInfo.h"

namespace Alchemy::Compilation {

    struct TextWindow;

    struct ScanningResult {
        int32 errorCount;
        int32 tokenCount;
//        SyntaxError* errors;
//        SyntaxToken* tokens;
//        SyntaxTriviaList* triviaList;
    };

    enum class ParseTypeMode {
        Normal,
        Parameter,
        AfterIs,
        DefinitePattern,
        AfterOut,
        AfterRef,
        AfterTupleComma,
        AsExpression,
        NewExpression,
        FirstElementOfPossibleTupleLiteral,
    };

    enum class ScanTypeFlags {
        // Definitely not a type name.
        NotType,

        // Definitely a type name: either a predefined type (int, string, etc.) or an array
        // type (ending with a [] brackets), or a pointer type (ending with *s), or a function
        // pointer type (ending with > in valid cases, or a *, ), or calling convention
        // identifier, in invalid cases).
        MustBeType,

        // Might be a generic (qualified) type name or a method name.
        GenericTypeOrMethod,

        // Might be a generic (qualified) type name or an expression or a method name.
        GenericTypeOrExpression,

        ///Might be a non-generic (qualified) type name or an expression.
        NonGenericTypeOrExpression,

        /// A type name with alias prefix (Alias::Name).  Note that Alias::Name.X would not fall under this.  This
        /// only is returned for exactly Alias::Name.
        AliasQualifiedName,

        /// Nullable type (ending with ?).
        NullableType,

        /// Might be a pointer type or a multiplication.
        PointerOrMultiplication,

        /// Might be a tuple type.
        TupleType,
    };

    union LiteralValue {
        bool boolValue;
        char charValue;
        int32 int32Value;
        uint32 uint32Value;
        int64 int64Value;
        uint64 uint64Value;
        float floatValue;
        double doubleValue;
    };

    enum class LiteralType : uint8 {
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
        SkippedTokens,
        LiteralValue,
        Diagnostic
    };

    struct Trivia {
        union {
            char* span;
            SyntaxDiagnosticInfo* diagnostic;
            LiteralValue literalValue;
        };
        uint32 length;
        TriviaType type;
        bool isLeading;
        bool isTrailing;
    };

    struct TokenInfo {

        FixedCharSpan text;
        SyntaxKind Kind;
        SyntaxKind ContextualKind;
        LiteralType valueKind;
        LiteralValue literalValue;

    };

    DEFINE_ENUM_FLAGS(SyntaxTokenFlags, uint8, {
        None = 0,
        Error = 1 << 1,
        LeadingTrivia = 1 << 2,
        TrailingTrivia = 1 << 3,
        Missing = 1 << 4
    })

    struct SyntaxTokenCold {
        char* text;
        Trivia* triviaList;
        uint16 triviaCount;
        uint16 triviaCapacity;
        uint32 textSize;
    };

    struct SyntaxToken {
        int32 id {};
        SyntaxKind kind {};
        SyntaxKind contextualKind {};
        SyntaxTokenFlags flags {};
        LiteralType literalType {};
    };

//    struct SyntaxToken {
//        char* text;
//        LiteralValue literalValue;
//        int32 id;
//        uint32 textSize; // encode w/ flags maybe
//        SyntaxTokenFlags flags;
//        SyntaxKind kind;
//        SyntaxKind contextualKind;
//        LiteralType literalType;
//    };

    bool IsNewline(char32 c);

    bool IsIdentifierStartCharacter(uint32 codepoint);

    uint32 ScanUnicodeEscape(TextWindow* textWindow, SyntaxDiagnosticInfo* error);

    bool ScanIdentifier(TextWindow* textWindow, FixedCharSpan* identifier);

    bool ScanIdentifier_FastPath(TextWindow* textWindow, FixedCharSpan* identifier);

    bool ScanIdentifier_SlowPath(TextWindow* textWindow, FixedCharSpan* identifier);

    void ScanWhitespace(TextWindow* textWindow, FixedCharSpan* whitespace);

    void ScanToEndOfLine(TextWindow* textWindow, FixedCharSpan* span);

    void ScanSingleLineComment(TextWindow* textWindow, FixedCharSpan* span);

    void ScanMultiLineComment(TextWindow* textWindow, FixedCharSpan* span, bool* isTerminated);

    bool TryMatchKeyword_Generated(char* buffer, int32 length, SyntaxKind* keywordType);

    bool ScanNumericLiteral(TextWindow* textWindow, Diagnostics* diagnostics, TokenInfo* info);

}