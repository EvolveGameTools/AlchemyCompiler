
namespace Alchemy::Parsing {

    enum class TokenType : uint8 {

        Invalid,
        EndOfInput,
        Comment,
        Whitespace,
        KeywordOrIdentifier,
        Literal,

        OpenQuote,
        CloseQuote,

        RegularStringPart,
        RegularStylePart,
        RegularCharacterPart,

        ShortStringInterpolation,
        LongStringInterpolationStart,
        LongStringInterpolationEnd,

        OpenStyle,
        CloseStyle,

        OpenCharacter,
        CloseCharacter,

        MultiLineStringStart,
        MultiLineStringEnd,

        BadCharacter,

        Coalesce,
        ConditionalAccess,
        ConditionalAnd,
        ConditionalOr,
        FatArrow,
        ConditionalEquals,
        ConditionalNotEquals,
        Increment,
        Decrement,
        ThinArrow,
        Assign,
        AddAssign,
        SubtractAssign,
        DivideAssign,
        MultiplyAssign,
        ModAssign,
        AndAssign,
        OrAssign,
        XorAssign,
        LeftShiftAssign,
        RightShiftAssign,
        GreaterThanEqualTo,
        LessThanEqualTo,
        AngleBracketClose,
        AngleBracketOpen,
        Not,
        Plus,
        Minus,
        Divide,
        Multiply,
        Modulus,
        BinaryNot,
        BinaryOr,
        BinaryAnd,
        BinaryXor,
        QuestionMark,
        Colon,
        SemiColon,
        DoubleColon,
        Dot,
        At,
        Comma,
        OpenParen,
        CloseParen,
        SquareBraceOpen,
        SquareBraceClose,
        CurlyBraceOpen,
        CurlyBraceClose,
        CoalesceAssign,
        HashTag,
        Splat,

        UInt32Literal,
        UInt64Literal,
        Int32Literal,
        Int64Literal,
        FloatLiteral,
        DoubleLiteral,
        BinaryNumberLiteral,
        HexLiteral,
        StringStart,
        StringEnd

    };

    const char* TokenTypeToString(TokenType e);

}