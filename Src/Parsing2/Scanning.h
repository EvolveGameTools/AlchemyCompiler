#pragma once

#include "../PrimitiveTypes.h"
#include "../Util/FixedCharSpan.h"
#include "../Collections/CheckedArray.h"
#include "./SyntaxKind.h"
#include "./Diagnostics.h"

namespace Alchemy::Compilation {

    struct TextWindow;

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

    DEFINE_ENUM_FLAGS(SyntaxTokenFlags, uint8, {
        None = 0,
        Error = 1 << 1,
        LeadingTrivia = 1 << 2,
        TrailingTrivia = 1 << 3,
        Missing = 1 << 4,
        Skipped = 1 << 5,
        Omitted = 1 << 6
    })

    struct SyntaxToken {

        SyntaxKind kind {};
        SyntaxKind contextualKind {};
        uint16 textSize {};
        uint32 id_flags {};
        char * text {};

        inline FixedCharSpan GetText() {
            return FixedCharSpan(text, textSize);
        }

        inline int32 GetId() const {
            return static_cast<int32>((id_flags >> 8) & 0xffffff);
        }

        inline SyntaxTokenFlags GetFlags() const {
            return static_cast<SyntaxTokenFlags>(id_flags & 0xff);
        }

        // Setter functions to update data
        inline void SetId(int32 id) {
            id_flags &= ~(0xFFFFFF << 8); // Clear the ID bits
            id_flags |= (static_cast<uint32>(id) << 8); // Set new ID
        }

        inline void SetFlags(SyntaxTokenFlags flags) {
            id_flags &= ~(0xFF); // Clear the flags bits
            id_flags |= (uint8)flags; // Set new flags
        }

        inline bool ContainsDiagnostics() {
            return (GetFlags() & SyntaxTokenFlags::Error) != 0;
        }

        inline bool IsValid() {
            return kind != SyntaxKind::None;
        }

        inline bool IsMissing() {
            return (GetFlags() & SyntaxTokenFlags::Missing) != 0;
        }

        inline void AddFlag(SyntaxTokenFlags flags) {
            SetFlags(GetFlags() | flags);
        }

    };

    static_assert(sizeof(SyntaxToken) == 16);
    enum class TriviaType : uint8 {
        Whitespace,
        SingleLineComment,
        MultiLineComment,
        NewLine,
        BadToken,
        Directive
    };

    struct Trivia {
        union {
            char* span {};
            LiteralValue literalValue;
        };
        uint32 length {};
        TriviaType type {};
        bool isLeading {};
        bool isTrailing {};

    };

    bool IsNewline(char32 c);

    bool IsIdentifierStartCharacter(uint32 codepoint);

    uint32 ScanUnicodeEscape(TextWindow* textWindow, Diagnostic* error);

    bool ScanIdentifier_FastPath(TextWindow* textWindow, FixedCharSpan* identifier);

    bool ScanIdentifier_SlowPath(TextWindow* textWindow, FixedCharSpan* identifier);

    void ScanWhitespace(TextWindow* textWindow, FixedCharSpan* whitespace);

    void ScanToEndOfLine(TextWindow* textWindow, FixedCharSpan* span);

    void ScanSingleLineComment(TextWindow* textWindow, FixedCharSpan* span);

    void ScanMultiLineComment(TextWindow* textWindow, FixedCharSpan* span, bool* isTerminated);

    bool TryMatchKeyword_Generated(char* buffer, int32 length, SyntaxKind* keywordType);

    bool ScanNumericLiteral(TextWindow* textWindow, Diagnostics* diagnostics, SyntaxToken* info);

    void ScanSyntaxToken(TextWindow* textWindow, SyntaxToken* info, Diagnostics* diagnostics, int32* badTokenCount);

    bool ScanIdentifierOrKeyword(TextWindow* textWindow, SyntaxToken* info);

    float GetFloatValue(char * str, int32 length);
    uint64 GetUInt64Value(char * str, int32 length);
    int64 GetInt64Value(char * str, int32 length);
    int32 GetInt32Value(char * str, int32 length);
    uint32 GetUInt32Value(char * str, int32 length);
    uint64 GetHexValue(char * str, int32 length);
    uint64 GetBinaryValue(char * str, int32 length);

}