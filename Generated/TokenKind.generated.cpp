#include "../Src/Parsing3/TokenKind.h"
namespace Alchemy::Compilation {
    const char* TokenKindToString(TokenKind e) {
        switch(e) {
            case TokenKind::None: return "None";
            case TokenKind::__FirstLiteral__: return "__FirstLiteral__";
            case TokenKind::BoolLiteral: return "BoolLiteral";
            case TokenKind::FloatLiteral: return "FloatLiteral";
            case TokenKind::DoubleLiteral: return "DoubleLiteral";
            case TokenKind::Int32Literal: return "Int32Literal";
            case TokenKind::Int64Literal: return "Int64Literal";
            case TokenKind::UInt32Literal: return "UInt32Literal";
            case TokenKind::UInt64Literal: return "UInt64Literal";
            case TokenKind::__LastLiteral__: return "__LastLiteral__";
            case TokenKind::__FirstKeyword__: return "__FirstKeyword__";
            case TokenKind::Int2Keyword: return "Int2Keyword";
            case TokenKind::Int3Keyword: return "Int3Keyword";
            case TokenKind::Int4Keyword: return "Int4Keyword";
            case TokenKind::Uint2Keyword: return "Uint2Keyword";
            case TokenKind::Uint3Keyword: return "Uint3Keyword";
            case TokenKind::Uint4Keyword: return "Uint4Keyword";
            case TokenKind::Float2Keyword: return "Float2Keyword";
            case TokenKind::Float3Keyword: return "Float3Keyword";
            case TokenKind::Float4Keyword: return "Float4Keyword";
            case TokenKind::Color8Keyword: return "Color8Keyword";
            case TokenKind::Color16Keyword: return "Color16Keyword";
            case TokenKind::Color32Keyword: return "Color32Keyword";
            case TokenKind::BoolKeyword: return "BoolKeyword";
            case TokenKind::ByteKeyword: return "ByteKeyword";
            case TokenKind::SByteKeyword: return "SByteKeyword";
            case TokenKind::ShortKeyword: return "ShortKeyword";
            case TokenKind::UShortKeyword: return "UShortKeyword";
            case TokenKind::IntKeyword: return "IntKeyword";
            case TokenKind::UIntKeyword: return "UIntKeyword";
            case TokenKind::LongKeyword: return "LongKeyword";
            case TokenKind::ULongKeyword: return "ULongKeyword";
            case TokenKind::DoubleKeyword: return "DoubleKeyword";
            case TokenKind::FloatKeyword: return "FloatKeyword";
            case TokenKind::StringKeyword: return "StringKeyword";
            case TokenKind::CharKeyword: return "CharKeyword";
            case TokenKind::VoidKeyword: return "VoidKeyword";
            case TokenKind::ObjectKeyword: return "ObjectKeyword";
            case TokenKind::NullKeyword: return "NullKeyword";
            case TokenKind::TupleKeyword: return "TupleKeyword";
            case TokenKind::DynamicKeyword: return "DynamicKeyword";
            case TokenKind::TrueKeyword: return "TrueKeyword";
            case TokenKind::FalseKeyword: return "FalseKeyword";
            case TokenKind::TypeofKeyword: return "TypeofKeyword";
            case TokenKind::InKeyword: return "InKeyword";
            case TokenKind::IfKeyword: return "IfKeyword";
            case TokenKind::ElseKeyword: return "ElseKeyword";
            case TokenKind::WhileKeyword: return "WhileKeyword";
            case TokenKind::ForKeyword: return "ForKeyword";
            case TokenKind::ForEachKeyword: return "ForEachKeyword";
            case TokenKind::DoKeyword: return "DoKeyword";
            case TokenKind::SwitchKeyword: return "SwitchKeyword";
            case TokenKind::CaseKeyword: return "CaseKeyword";
            case TokenKind::DefaultKeyword: return "DefaultKeyword";
            case TokenKind::TryKeyword: return "TryKeyword";
            case TokenKind::CatchKeyword: return "CatchKeyword";
            case TokenKind::FinallyKeyword: return "FinallyKeyword";
            case TokenKind::BreakKeyword: return "BreakKeyword";
            case TokenKind::ContinueKeyword: return "ContinueKeyword";
            case TokenKind::ReturnKeyword: return "ReturnKeyword";
            case TokenKind::ThrowKeyword: return "ThrowKeyword";
            case TokenKind::YieldKeyword: return "YieldKeyword";
            case TokenKind::ExportKeyword: return "ExportKeyword";
            case TokenKind::PublicKeyword: return "PublicKeyword";
            case TokenKind::PrivateKeyword: return "PrivateKeyword";
            case TokenKind::ProtectedKeyword: return "ProtectedKeyword";
            case TokenKind::InternalKeyword: return "InternalKeyword";
            case TokenKind::StaticKeyword: return "StaticKeyword";
            case TokenKind::ReadOnlyKeyword: return "ReadOnlyKeyword";
            case TokenKind::SealedKeyword: return "SealedKeyword";
            case TokenKind::ConstKeyword: return "ConstKeyword";
            case TokenKind::NewKeyword: return "NewKeyword";
            case TokenKind::OverrideKeyword: return "OverrideKeyword";
            case TokenKind::AbstractKeyword: return "AbstractKeyword";
            case TokenKind::VirtualKeyword: return "VirtualKeyword";
            case TokenKind::ExternKeyword: return "ExternKeyword";
            case TokenKind::RefKeyword: return "RefKeyword";
            case TokenKind::OutKeyword: return "OutKeyword";
            case TokenKind::IsKeyword: return "IsKeyword";
            case TokenKind::AsKeyword: return "AsKeyword";
            case TokenKind::ParamsKeyword: return "ParamsKeyword";
            case TokenKind::ThisKeyword: return "ThisKeyword";
            case TokenKind::BaseKeyword: return "BaseKeyword";
            case TokenKind::UsingKeyword: return "UsingKeyword";
            case TokenKind::ClassKeyword: return "ClassKeyword";
            case TokenKind::StructKeyword: return "StructKeyword";
            case TokenKind::InterfaceKeyword: return "InterfaceKeyword";
            case TokenKind::EnumKeyword: return "EnumKeyword";
            case TokenKind::DelegateKeyword: return "DelegateKeyword";
            case TokenKind::ImplicitKeyword: return "ImplicitKeyword";
            case TokenKind::VarKeyword: return "VarKeyword";
            case TokenKind::ElifKeyword: return "ElifKeyword";
            case TokenKind::EndIfKeyword: return "EndIfKeyword";
            case TokenKind::ConstructorKeyword: return "ConstructorKeyword";
            case TokenKind::GotoKeyword: return "GotoKeyword";
            case TokenKind::LockKeyword: return "LockKeyword";
            case TokenKind::OperatorKeyword: return "OperatorKeyword";
            case TokenKind::NamespaceKeyword: return "NamespaceKeyword";
            case TokenKind::__LastKeyword__: return "__LastKeyword__";
            case TokenKind::__FirstContextualKeyword__: return "__FirstContextualKeyword__";
            case TokenKind::GetKeyword: return "GetKeyword";
            case TokenKind::SetKeyword: return "SetKeyword";
            case TokenKind::WhereKeyword: return "WhereKeyword";
            case TokenKind::FromKeyword: return "FromKeyword";
            case TokenKind::WhenKeyword: return "WhenKeyword";
            case TokenKind::OrKeyword: return "OrKeyword";
            case TokenKind::AndKeyword: return "AndKeyword";
            case TokenKind::NotKeyword: return "NotKeyword";
            case TokenKind::WithKeyword: return "WithKeyword";
            case TokenKind::InitKeyword: return "InitKeyword";
            case TokenKind::RequiredKeyword: return "RequiredKeyword";
            case TokenKind::__LastContextualKeyword__: return "__LastContextualKeyword__";
            case TokenKind::__FirstToken__: return "__FirstToken__";
            case TokenKind::IdentifierToken: return "IdentifierToken";
            case TokenKind::SlashEqualsToken: return "SlashEqualsToken";
            case TokenKind::SlashToken: return "SlashToken";
            case TokenKind::DotToken: return "DotToken";
            case TokenKind::DotDotToken: return "DotDotToken";
            case TokenKind::DotDotDotToken: return "DotDotDotToken";
            case TokenKind::CommaToken: return "CommaToken";
            case TokenKind::ColonColonToken: return "ColonColonToken";
            case TokenKind::ColonToken: return "ColonToken";
            case TokenKind::SemicolonToken: return "SemicolonToken";
            case TokenKind::TildeToken: return "TildeToken";
            case TokenKind::ExclamationToken: return "ExclamationToken";
            case TokenKind::ExclamationEqualsToken: return "ExclamationEqualsToken";
            case TokenKind::EqualsEqualsToken: return "EqualsEqualsToken";
            case TokenKind::EqualsGreaterThanToken: return "EqualsGreaterThanToken";
            case TokenKind::EqualsToken: return "EqualsToken";
            case TokenKind::AsteriskEqualsToken: return "AsteriskEqualsToken";
            case TokenKind::AsteriskToken: return "AsteriskToken";
            case TokenKind::OpenParenToken: return "OpenParenToken";
            case TokenKind::CloseParenToken: return "CloseParenToken";
            case TokenKind::OpenBraceToken: return "OpenBraceToken";
            case TokenKind::CloseBraceToken: return "CloseBraceToken";
            case TokenKind::OpenBracketToken: return "OpenBracketToken";
            case TokenKind::CloseBracketToken: return "CloseBracketToken";
            case TokenKind::QuestionQuestionEqualsToken: return "QuestionQuestionEqualsToken";
            case TokenKind::QuestionQuestionToken: return "QuestionQuestionToken";
            case TokenKind::QuestionToken: return "QuestionToken";
            case TokenKind::PlusEqualsToken: return "PlusEqualsToken";
            case TokenKind::PlusPlusToken: return "PlusPlusToken";
            case TokenKind::PlusToken: return "PlusToken";
            case TokenKind::MinusEqualsToken: return "MinusEqualsToken";
            case TokenKind::MinusMinusToken: return "MinusMinusToken";
            case TokenKind::MinusGreaterThanToken: return "MinusGreaterThanToken";
            case TokenKind::MinusToken: return "MinusToken";
            case TokenKind::PercentToken: return "PercentToken";
            case TokenKind::PercentEqualsToken: return "PercentEqualsToken";
            case TokenKind::AmpersandEqualsToken: return "AmpersandEqualsToken";
            case TokenKind::AmpersandAmpersandToken: return "AmpersandAmpersandToken";
            case TokenKind::AmpersandToken: return "AmpersandToken";
            case TokenKind::CaretEqualsToken: return "CaretEqualsToken";
            case TokenKind::CaretToken: return "CaretToken";
            case TokenKind::BarEqualsToken: return "BarEqualsToken";
            case TokenKind::BarBarToken: return "BarBarToken";
            case TokenKind::BarToken: return "BarToken";
            case TokenKind::LessThanEqualsToken: return "LessThanEqualsToken";
            case TokenKind::LessThanLessThanEqualsToken: return "LessThanLessThanEqualsToken";
            case TokenKind::LessThanLessThanToken: return "LessThanLessThanToken";
            case TokenKind::LessThanToken: return "LessThanToken";
            case TokenKind::GreaterThanToken: return "GreaterThanToken";
            case TokenKind::GreaterThanEqualsToken: return "GreaterThanEqualsToken";
            case TokenKind::NumericLiteralToken: return "NumericLiteralToken";
            case TokenKind::EndOfFileToken: return "EndOfFileToken";
            case TokenKind::GreaterThanGreaterThanToken: return "GreaterThanGreaterThanToken";
            case TokenKind::GreaterThanGreaterThanGreaterThanToken: return "GreaterThanGreaterThanGreaterThanToken";
            case TokenKind::GreaterThanGreaterThanEqualsToken: return "GreaterThanGreaterThanEqualsToken";
            case TokenKind::GreaterThanGreaterThanGreaterThanEqualsToken: return "GreaterThanGreaterThanGreaterThanEqualsToken";
            case TokenKind::UnderscoreToken: return "UnderscoreToken";
            case TokenKind::DollarToken: return "DollarToken";
            case TokenKind::BackslashToken: return "BackslashToken";
            case TokenKind::DoubleQuoteToken: return "DoubleQuoteToken";
            case TokenKind::SingleQuoteToken: return "SingleQuoteToken";
            case TokenKind::HashToken: return "HashToken";
            case TokenKind::OmittedArraySizeExpressionToken: return "OmittedArraySizeExpressionToken";
            case TokenKind::__LastToken__: return "__LastToken__";
            case TokenKind::Trivia: return "Trivia";
            case TokenKind::MultiLineComment: return "MultiLineComment";
            case TokenKind::SingleLineComment: return "SingleLineComment";
            case TokenKind::Whitespace: return "Whitespace";
            case TokenKind::NewLine: return "NewLine";
            case TokenKind::InterpolatedIdentifier: return "InterpolatedIdentifier";
            case TokenKind::InterpolatedExpressionStart: return "InterpolatedExpressionStart";
            case TokenKind::InterpolatedExpressionEnd: return "InterpolatedExpressionEnd";
            case TokenKind::StringLiteralEmpty: return "StringLiteralEmpty";
            case TokenKind::StringLiteralStart: return "StringLiteralStart";
            case TokenKind::StringLiteralPart: return "StringLiteralPart";
            case TokenKind::StringLiteralEnd: return "StringLiteralEnd";
            case TokenKind::RawStringLiteralStart: return "RawStringLiteralStart";
            case TokenKind::RawStringLiteralEnd: return "RawStringLiteralEnd";
            case TokenKind::CharLiteralStart: return "CharLiteralStart";
            case TokenKind::CharLiteralContent: return "CharLiteralContent";
            case TokenKind::CharLiteralEnd: return "CharLiteralEnd";
            default: return "";
        }
    }

}