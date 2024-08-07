#pragma once
#include "../PrimitiveTypes.h"

namespace Alchemy::Compilation {

    enum class TokenKind : uint8 {
        None,
        // --- Literal Types ---
        __FirstLiteral__,
        BoolLiteral,
        FloatLiteral,
        DoubleLiteral,
        Int32Literal,
        Int64Literal,
        UInt32Literal,
        UInt64Literal,
        __LastLiteral__,

        // --- Reserved Keyword
        __FirstKeyword__,
        Int2Keyword,
        Int3Keyword,
        Int4Keyword,

        Uint2Keyword,
        Uint3Keyword,
        Uint4Keyword,

        Float2Keyword,
        Float3Keyword,
        Float4Keyword,

        Color8Keyword,
        Color16Keyword,
        Color32Keyword,

        BoolKeyword,
        ByteKeyword,
        SByteKeyword,
        ShortKeyword,
        UShortKeyword,
        IntKeyword,
        UIntKeyword,
        LongKeyword,
        ULongKeyword,
        DoubleKeyword,
        FloatKeyword,
        StringKeyword,
        CharKeyword,
        VoidKeyword,
        ObjectKeyword,
        NullKeyword,
        TupleKeyword,
        DynamicKeyword,

        // list,
        // list2D
        // dictionary
        // set
        // stack
        // queue

        TrueKeyword,
        FalseKeyword,

        TypeofKeyword,
        InKeyword,
        IfKeyword,
        ElseKeyword,
        WhileKeyword,
        ForKeyword,
        ForEachKeyword,
        DoKeyword,
        SwitchKeyword,
        CaseKeyword,
        DefaultKeyword,
        TryKeyword,
        CatchKeyword,
        FinallyKeyword,
        BreakKeyword,
        ContinueKeyword,
        ReturnKeyword,
        ThrowKeyword,
        YieldKeyword,

        ExportKeyword,
        PublicKeyword,
        PrivateKeyword,
        ProtectedKeyword,
        InternalKeyword,

        StaticKeyword,
        ReadOnlyKeyword,
        SealedKeyword,
        ConstKeyword,
        NewKeyword,
        OverrideKeyword,
        AbstractKeyword,
        VirtualKeyword,
        ExternKeyword,
        RefKeyword,
        OutKeyword,
        IsKeyword,
        AsKeyword,
        ParamsKeyword,
        ThisKeyword,
        BaseKeyword,
        UsingKeyword,
        ClassKeyword,
        StructKeyword,
        InterfaceKeyword,
        EnumKeyword,
        DelegateKeyword,
        ImplicitKeyword,
        VarKeyword,
        ElifKeyword,
        EndIfKeyword,
        ConstructorKeyword,

        GotoKeyword, // reserved but not used atm
        LockKeyword, // reserved but not used atm
        OperatorKeyword, // reserved but not used atm
        NamespaceKeyword, // reserved but not used atm

        __LastKeyword__,
        // --- End Reserved Keywords

        // --- Contextual Keywords
        __FirstContextualKeyword__,
        GetKeyword,
        SetKeyword,
        WhereKeyword,
        FromKeyword,
        WhenKeyword,
        OrKeyword,
        AndKeyword,
        NotKeyword,
        WithKeyword,
        InitKeyword,
        RequiredKeyword,
        __LastContextualKeyword__,
        // --- End Contextual Keywords

        // --- Tokens
        __FirstToken__,
        IdentifierToken,
        SlashEqualsToken,
        SlashToken,
        DotToken,
        DotDotToken,
        DotDotDotToken,
        CommaToken,
        ColonColonToken,
        ColonToken,
        SemicolonToken,
        TildeToken,
        ExclamationToken,
        ExclamationEqualsToken,
        EqualsEqualsToken,
        EqualsGreaterThanToken,
        EqualsToken,
        AsteriskEqualsToken,
        AsteriskToken,
        OpenParenToken,
        CloseParenToken,
        OpenBraceToken,
        CloseBraceToken,
        OpenBracketToken,
        CloseBracketToken,
        QuestionQuestionEqualsToken,
        QuestionQuestionToken,
        QuestionToken,
        PlusEqualsToken,
        PlusPlusToken,
        PlusToken,
        MinusEqualsToken,
        MinusMinusToken,
        MinusGreaterThanToken,
        MinusToken,
        PercentToken,
        PercentEqualsToken,
        AmpersandEqualsToken,
        AmpersandAmpersandToken,
        AmpersandToken,
        CaretEqualsToken,
        CaretToken,
        BarEqualsToken,
        BarBarToken,
        BarToken,
        LessThanEqualsToken,
        LessThanLessThanEqualsToken,
        LessThanLessThanToken,
        LessThanToken,
        GreaterThanToken,
        GreaterThanEqualsToken,
        NumericLiteralToken,
        EndOfFileToken,
        GreaterThanGreaterThanToken,
        GreaterThanGreaterThanGreaterThanToken,
        GreaterThanGreaterThanEqualsToken,
        GreaterThanGreaterThanGreaterThanEqualsToken,
        UnderscoreToken,
        DollarToken,
        BackslashToken,
        DoubleQuoteToken,
        SingleQuoteToken,
        HashToken,
        OmittedArraySizeExpressionToken,
        __LastToken__,
        // --- End Tokens


        Trivia,
        MultiLineComment,
        SingleLineComment,
        Whitespace,
        NewLine,

        InterpolatedIdentifier,
        InterpolatedExpressionStart,
        InterpolatedExpressionEnd,
        StringLiteralEmpty,
        StringLiteralStart,
        StringLiteralPart,
        StringLiteralEnd,
        RawStringLiteralStart,
        RawStringLiteralEnd,
        CharLiteralStart,
        CharLiteralContent,
        CharLiteralEnd,
    };

    const char* TokenKindToString(TokenKind kind);

}