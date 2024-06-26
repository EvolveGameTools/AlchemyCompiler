#include "../Token.h"

namespace Alchemy::Parsing {

// --- Generate TokenTypeToString Start
    const char* TokenTypeToString(TokenType e) {
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


}