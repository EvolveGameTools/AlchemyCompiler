#pragma once

#include "Keyword.h"

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

// --- Generate TokenTypeToString Start
    inline const char* TokenTypeToString(TokenType e) {
        switch(e) {
            case TokenType::EndOfInput: return "EndOfInput";
            case TokenType::Comment: return "Comment";
            case TokenType::Whitespace: return "Whitespace";
            case TokenType::KeywordOrIdentifier: return "KeywordOrIdentifier";
            case TokenType::Literal: return "Literal";
            case TokenType::OpenQuote: return "OpenQuote";
            case TokenType::CloseQuote: return "CloseQuote";
            case TokenType::RegularStringPart: return "RegularStringPart";
            case TokenType::RegularStylePart: return "RegularStylePart";
            case TokenType::RegularCharacterPart: return "RegularCharacterPart";
            case TokenType::ShortStringInterpolation: return "ShortStringInterpolation";
            case TokenType::LongStringInterpolationStart: return "LongStringInterpolationStart";
            case TokenType::LongStringInterpolationEnd: return "LongStringInterpolationEnd";
            case TokenType::OpenStyle: return "OpenStyle";
            case TokenType::CloseStyle: return "CloseStyle";
            case TokenType::OpenCharacter: return "OpenCharacter";
            case TokenType::CloseCharacter: return "CloseCharacter";
            case TokenType::MultiLineStringStart: return "MultiLineStringStart";
            case TokenType::MultiLineStringEnd: return "MultiLineStringEnd";
            case TokenType::BadCharacter: return "BadCharacter";
            case TokenType::Coalesce: return "Coalesce";
            case TokenType::ConditionalAccess: return "ConditionalAccess";
            case TokenType::ConditionalAnd: return "ConditionalAnd";
            case TokenType::ConditionalOr: return "ConditionalOr";
            case TokenType::FatArrow: return "FatArrow";
            case TokenType::ConditionalEquals: return "ConditionalEquals";
            case TokenType::ConditionalNotEquals: return "ConditionalNotEquals";
            case TokenType::Increment: return "Increment";
            case TokenType::Decrement: return "Decrement";
            case TokenType::ThinArrow: return "ThinArrow";
            case TokenType::Assign: return "Assign";
            case TokenType::AddAssign: return "AddAssign";
            case TokenType::SubtractAssign: return "SubtractAssign";
            case TokenType::DivideAssign: return "DivideAssign";
            case TokenType::MultiplyAssign: return "MultiplyAssign";
            case TokenType::ModAssign: return "ModAssign";
            case TokenType::AndAssign: return "AndAssign";
            case TokenType::OrAssign: return "OrAssign";
            case TokenType::XorAssign: return "XorAssign";
            case TokenType::LeftShiftAssign: return "LeftShiftAssign";
            case TokenType::RightShiftAssign: return "RightShiftAssign";
            case TokenType::GreaterThanEqualTo: return "GreaterThanEqualTo";
            case TokenType::LessThanEqualTo: return "LessThanEqualTo";
            case TokenType::AngleBracketClose: return "AngleBracketClose";
            case TokenType::AngleBracketOpen: return "AngleBracketOpen";
            case TokenType::Not: return "Not";
            case TokenType::Plus: return "Plus";
            case TokenType::Minus: return "Minus";
            case TokenType::Divide: return "Divide";
            case TokenType::Multiply: return "Multiply";
            case TokenType::Modulus: return "Modulus";
            case TokenType::BinaryNot: return "BinaryNot";
            case TokenType::BinaryOr: return "BinaryOr";
            case TokenType::BinaryAnd: return "BinaryAnd";
            case TokenType::BinaryXor: return "BinaryXor";
            case TokenType::QuestionMark: return "QuestionMark";
            case TokenType::Colon: return "Colon";
            case TokenType::SemiColon: return "SemiColon";
            case TokenType::DoubleColon: return "DoubleColon";
            case TokenType::Dot: return "Dot";
            case TokenType::At: return "At";
            case TokenType::Comma: return "Comma";
            case TokenType::OpenParen: return "OpenParen";
            case TokenType::CloseParen: return "CloseParen";
            case TokenType::SquareBraceOpen: return "SquareBraceOpen";
            case TokenType::SquareBraceClose: return "SquareBraceClose";
            case TokenType::CurlyBraceOpen: return "CurlyBraceOpen";
            case TokenType::CurlyBraceClose: return "CurlyBraceClose";
            case TokenType::CoalesceAssign: return "CoalesceAssign";
            case TokenType::HashTag: return "HashTag";
            case TokenType::Splat: return "Splat";
            case TokenType::UInt32Literal: return "UInt32Literal";
            case TokenType::UInt64Literal: return "UInt64Literal";
            case TokenType::Int32Literal: return "Int32Literal";
            case TokenType::Int64Literal: return "Int64Literal";
            case TokenType::FloatLiteral: return "FloatLiteral";
            case TokenType::DoubleLiteral: return "DoubleLiteral";
            case TokenType::BinaryNumberLiteral: return "BinaryNumberLiteral";
            case TokenType::HexLiteral: return "HexLiteral";
            case TokenType::StringStart: return "StringStart";
            case TokenType::StringEnd: return "StringEnd";
            default: return "";
        }
    }

// --- Generate TokenTypeToString End

inline const char * GetTokenTypeName(TokenType type) {
    switch (type) {

        case TokenType::Invalid:
            return "Invalid";
        case TokenType::EndOfInput:
            return "EOF";
        case TokenType::Comment:
            return "Comment";
        case TokenType::Whitespace:
            return "Whitespace";
        case TokenType::KeywordOrIdentifier:
            return "KeywordOrIdentifier";
        case TokenType::Literal:
            return "Literal";
        case TokenType::OpenQuote:
            return "OpenQuote";
        case TokenType::CloseQuote:
            return "CloseQuote";
        case TokenType::RegularStringPart:
            return "RegularStringPart";
        case TokenType::ShortStringInterpolation:
            return "ShortStringInterpolation";
        case TokenType::LongStringInterpolationStart:
            return "LongStringInterpolationStart";
        case TokenType::LongStringInterpolationEnd:
            return "LongStringInterpolationEnd";
        case TokenType::BadCharacter:
            return "BadCharacter";
        default:
            return "Invalid";
    }
}

enum class LiteralType : uint8 {

    None = 0,
    UInt64,
    UInt32,
    Int32,
    Int64,
    Float,
    Double,
    Bool,
    Null,
    Default,
    String

};

enum class TokenFlags {

    None = 0,
    FollowedByWhitespaceOrComment = 1 << 0,
    InvalidMatch = 1 << 1

};

IMPL_ENUM_FLAGS_OPERATORS(TokenFlags);

struct Token {

    TokenType tokenType { TokenType::Invalid };
    Keyword keyword { Keyword::NotAKeyword }; // todo -- move reserved keywords into token type and other keywords into a separate category
    ContextualKeyword contextualKeyword { ContextualKeyword::NotAKeyword };
    TokenFlags flags { 0 };
    int32 position { };

    Token() = default;

    Token(TokenType tokenType, int32 position)
        : tokenType(tokenType)
        , position(position)
        , contextualKeyword(ContextualKeyword::NotAKeyword)
        , flags(TokenFlags::None)
        , keyword(Keyword::NotAKeyword) {}

    Token(TokenType tokenType, Keyword keyword, int32 position)
        : tokenType(tokenType)
        , position(position)
        , contextualKeyword(ContextualKeyword::NotAKeyword)
        , flags(TokenFlags::None)
        , keyword(keyword) {}

    inline bool IsWhitespaceOrComment() const {
        return tokenType == TokenType::Whitespace || tokenType == TokenType::Comment;
    }

    inline bool IsKeyword(Keyword _keyword) const {
        return tokenType == TokenType::KeywordOrIdentifier && keyword == _keyword;
    }

};