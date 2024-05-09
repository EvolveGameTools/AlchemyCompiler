#include "./SyntaxFacts.h"

namespace Alchemy::Compilation::SyntaxFacts {

    bool IsHexDigit(char c) {
        return (c >= '0' && c <= '9') ||
               (c >= 'A' && c <= 'F') ||
               (c >= 'a' && c <= 'f');
    }

    int32 HexValue(char c) {
        assert(IsHexDigit(c));
        return (c >= '0' && c <= '9') ? c - '0' : (c & 0xdf) - 'A' + 10;
    }

    bool IsBinaryDigit(char c) {
        return c == '0' | c == '1';
    }

    bool IsDecDigit(char c) {
        return c >= '0' && c <= '9';
    }

    SyntaxKind GetPostfixUnaryExpression(SyntaxKind token) {
        switch (token) {
            case SyntaxKind::PlusPlusToken:
                return SyntaxKind::PostIncrementExpression;
            case SyntaxKind::MinusMinusToken:
                return SyntaxKind::PostDecrementExpression;
            case SyntaxKind::ExclamationToken:
                NOT_IMPLEMENTED("! postfix expression");
                return SyntaxKind::None; // todo -- this should be our make-it-work-or-panic operator
            default:
                return SyntaxKind::None;
        }
    }

    SyntaxKind GetPrefixUnaryExpression(SyntaxKind token) {
        switch (token) {
            case SyntaxKind::PlusToken:
                return SyntaxKind::UnaryPlusExpression;
            case SyntaxKind::MinusToken:
                return SyntaxKind::UnaryMinusExpression;
            case SyntaxKind::TildeToken:
                return SyntaxKind::BitwiseNotExpression;
            case SyntaxKind::ExclamationToken:
                return SyntaxKind::LogicalNotExpression;
            case SyntaxKind::PlusPlusToken:
                return SyntaxKind::PreIncrementExpression;
            case SyntaxKind::MinusMinusToken:
                return SyntaxKind::PreDecrementExpression;
            case SyntaxKind::AmpersandToken:
                return SyntaxKind::AddressOfExpression;
            case SyntaxKind::AsteriskToken:
                return SyntaxKind::PointerIndirectionExpression;
            case SyntaxKind::CaretToken:
                return SyntaxKind::IndexExpression;
            default:
                return SyntaxKind::None;
        }
    }

    bool IsPrefixUnaryExpression(SyntaxKind token) {
        return GetPrefixUnaryExpression(token) != SyntaxKind::None;
    }

    bool IsPrefixUnaryExpressionOperatorToken(SyntaxKind token) {
        return GetPrefixUnaryExpression(token) != SyntaxKind::None;
    }

    bool IsPostfixUnaryExpression(SyntaxKind token) {
        return GetPostfixUnaryExpression(token) != SyntaxKind::None;
    }

    bool IsPostfixUnaryExpressionToken(SyntaxKind token) {
        return GetPostfixUnaryExpression(token) != SyntaxKind::None;
    }

    bool IsAnyUnaryExpression(Alchemy::Compilation::SyntaxKind kind) {
        return IsPrefixUnaryExpression(kind) || IsPostfixUnaryExpression(kind);
    }

    SyntaxKind GetBinaryExpression(SyntaxKind kind) {
        switch (kind) {
            case SyntaxKind::QuestionQuestionToken:
                return SyntaxKind::CoalesceExpression;
            case SyntaxKind::IsKeyword:
                return SyntaxKind::IsExpression;
            case SyntaxKind::AsKeyword:
                return SyntaxKind::AsExpression;
            case SyntaxKind::BarToken:
                return SyntaxKind::BitwiseOrExpression;
            case SyntaxKind::CaretToken:
                return SyntaxKind::ExclusiveOrExpression;
            case SyntaxKind::AmpersandToken:
                return SyntaxKind::BitwiseAndExpression;
            case SyntaxKind::EqualsEqualsToken:
                return SyntaxKind::EqualsExpression;
            case SyntaxKind::ExclamationEqualsToken:
                return SyntaxKind::NotEqualsExpression;
            case SyntaxKind::LessThanToken:
                return SyntaxKind::LessThanExpression;
            case SyntaxKind::LessThanEqualsToken:
                return SyntaxKind::LessThanOrEqualExpression;
            case SyntaxKind::GreaterThanToken:
                return SyntaxKind::GreaterThanExpression;
            case SyntaxKind::GreaterThanEqualsToken:
                return SyntaxKind::GreaterThanOrEqualExpression;
            case SyntaxKind::LessThanLessThanToken:
                return SyntaxKind::LeftShiftExpression;
            case SyntaxKind::GreaterThanGreaterThanToken:
                return SyntaxKind::RightShiftExpression;
            case SyntaxKind::GreaterThanGreaterThanGreaterThanToken:
                return SyntaxKind::UnsignedRightShiftExpression;
            case SyntaxKind::PlusToken:
                return SyntaxKind::AddExpression;
            case SyntaxKind::MinusToken:
                return SyntaxKind::SubtractExpression;
            case SyntaxKind::AsteriskToken:
                return SyntaxKind::MultiplyExpression;
            case SyntaxKind::SlashToken:
                return SyntaxKind::DivideExpression;
            case SyntaxKind::PercentToken:
                return SyntaxKind::ModuloExpression;
            case SyntaxKind::AmpersandAmpersandToken:
                return SyntaxKind::LogicalAndExpression;
            case SyntaxKind::BarBarToken:
                return SyntaxKind::LogicalOrExpression;
            default:
                return SyntaxKind::None;
        }
    }

    bool IsBinaryExpression(SyntaxKind token) {
        return GetBinaryExpression(token) != SyntaxKind::None;
    }

    bool IsAssignmentExpressionOperatorToken(SyntaxKind token) {
        switch (token) {
            case SyntaxKind::QuestionQuestionEqualsToken:
            case SyntaxKind::BarEqualsToken:
            case SyntaxKind::AmpersandEqualsToken:
            case SyntaxKind::CaretEqualsToken:
            case SyntaxKind::LessThanLessThanEqualsToken:
            case SyntaxKind::GreaterThanGreaterThanEqualsToken:
            case SyntaxKind::GreaterThanGreaterThanGreaterThanEqualsToken:
            case SyntaxKind::PlusEqualsToken:
            case SyntaxKind::MinusEqualsToken:
            case SyntaxKind::AsteriskEqualsToken:
            case SyntaxKind::SlashEqualsToken:
            case SyntaxKind::PercentEqualsToken:
            case SyntaxKind::EqualsToken:
                return true;
            default:
                return false;
        }
    }

    bool IsPredefinedType(SyntaxKind kind) {
        switch (kind) {
            // todo -- aliases, float vectors, colors, dynamic
            case SyntaxKind::BoolKeyword:
            case SyntaxKind::ByteKeyword:
            case SyntaxKind::SByteKeyword:
            case SyntaxKind::IntKeyword:
            case SyntaxKind::UIntKeyword:
            case SyntaxKind::ShortKeyword:
            case SyntaxKind::UShortKeyword:
            case SyntaxKind::LongKeyword:
            case SyntaxKind::ULongKeyword:
            case SyntaxKind::FloatKeyword:
            case SyntaxKind::DoubleKeyword:
            case SyntaxKind::StringKeyword:
            case SyntaxKind::CharKeyword:
            case SyntaxKind::ObjectKeyword:
            case SyntaxKind::VoidKeyword:
                return true;
            default:
                return false;
        }
    }

    bool IsReservedKeyword(SyntaxKind kind) {
        int32 v = (int32) kind;
        constexpr int32 lower = (int32) SyntaxKind::__FirstKeyword__;
        constexpr int32 upper = (int32) SyntaxKind::__LastKeyword__;
        return v > lower && v < upper;
    }

    bool IsToken(SyntaxKind kind) {
        int32 v = (int32) kind;
        constexpr int32 lower = (int32) SyntaxKind::__FirstToken__;
        constexpr int32 upper = (int32) SyntaxKind::__LastToken__;
        return v > lower && v < upper;
    }

    bool IsContextualKeyword(SyntaxKind kind) {
        int32 v = (int32) kind;
        constexpr int32 lower = (int32) SyntaxKind::__FirstContextualKeyword__;
        constexpr int32 upper = (int32) SyntaxKind::__LastContextualKeyword__;
        return v > lower && v < upper;
    }

    const char* GetText(SyntaxKind kind) {

        switch (kind) {
            case SyntaxKind::TildeToken:
                return "~";
            case SyntaxKind::ExclamationToken:
                return "!";
            case SyntaxKind::DollarToken:
                return "$";
            case SyntaxKind::PercentToken:
                return "%";
            case SyntaxKind::CaretToken:
                return "^";
            case SyntaxKind::AmpersandToken:
                return "&";
            case SyntaxKind::AsteriskToken:
                return "*";
            case SyntaxKind::OpenParenToken:
                return "(";
            case SyntaxKind::CloseParenToken:
                return ")";
            case SyntaxKind::MinusToken:
                return "-";
            case SyntaxKind::PlusToken:
                return "+";
            case SyntaxKind::EqualsToken:
                return "=";
            case SyntaxKind::OpenBraceToken:
                return "{";
            case SyntaxKind::CloseBraceToken:
                return "}";
            case SyntaxKind::OpenBracketToken:
                return "[";
            case SyntaxKind::CloseBracketToken:
                return "]";
            case SyntaxKind::BarToken:
                return "|";
            case SyntaxKind::BackslashToken:
                return "\\";
            case SyntaxKind::ColonToken:
                return ":";
            case SyntaxKind::SemicolonToken:
                return ";";
            case SyntaxKind::DoubleQuoteToken:
                return "\"";
            case SyntaxKind::SingleQuoteToken:
                return "'";
            case SyntaxKind::LessThanToken:
                return "<";
            case SyntaxKind::CommaToken:
                return ",";
            case SyntaxKind::GreaterThanToken:
                return ">";
            case SyntaxKind::DotToken:
                return ".";
            case SyntaxKind::QuestionToken:
                return "?";
            case SyntaxKind::HashToken:
                return "#";
            case SyntaxKind::SlashToken:
                return "/";

                // compound
            case SyntaxKind::BarBarToken:
                return "||";
            case SyntaxKind::AmpersandAmpersandToken:
                return "&&";
            case SyntaxKind::MinusMinusToken:
                return "--";
            case SyntaxKind::PlusPlusToken:
                return "++";
            case SyntaxKind::ColonColonToken:
                return "::";
            case SyntaxKind::QuestionQuestionToken:
                return "??";
            case SyntaxKind::MinusGreaterThanToken:
                return "->";
            case SyntaxKind::ExclamationEqualsToken:
                return "!=";
            case SyntaxKind::EqualsEqualsToken:
                return "==";
            case SyntaxKind::EqualsGreaterThanToken:
                return "=>";
            case SyntaxKind::LessThanEqualsToken:
                return "<=";
            case SyntaxKind::LessThanLessThanToken:
                return "<<";
            case SyntaxKind::LessThanLessThanEqualsToken:
                return "<<=";
            case SyntaxKind::GreaterThanEqualsToken:
                return ">=";
            case SyntaxKind::GreaterThanGreaterThanToken:
                return ">>";
            case SyntaxKind::GreaterThanGreaterThanEqualsToken:
                return ">>=";
            case SyntaxKind::GreaterThanGreaterThanGreaterThanToken:
                return ">>>";
            case SyntaxKind::GreaterThanGreaterThanGreaterThanEqualsToken:
                return ">>>=";
            case SyntaxKind::SlashEqualsToken:
                return "/=";
            case SyntaxKind::AsteriskEqualsToken:
                return "*=";
            case SyntaxKind::BarEqualsToken:
                return "|=";
            case SyntaxKind::AmpersandEqualsToken:
                return "&=";
            case SyntaxKind::PlusEqualsToken:
                return "+=";
            case SyntaxKind::MinusEqualsToken:
                return "-=";
            case SyntaxKind::CaretEqualsToken:
                return "^=";
            case SyntaxKind::PercentEqualsToken:
                return "%=";
            case SyntaxKind::QuestionQuestionEqualsToken:
                return "??=";
            case SyntaxKind::DotDotToken:
                return "..";

                // Keywords
            case SyntaxKind::BoolKeyword:
                return "bool";
            case SyntaxKind::ByteKeyword:
                return "byte";
            case SyntaxKind::SByteKeyword:
                return "sbyte";
            case SyntaxKind::ShortKeyword:
                return "short";
            case SyntaxKind::UShortKeyword:
                return "ushort";
            case SyntaxKind::IntKeyword:
                return "int";
            case SyntaxKind::UIntKeyword:
                return "uint";
            case SyntaxKind::LongKeyword:
                return "long";
            case SyntaxKind::ULongKeyword:
                return "ulong";
            case SyntaxKind::DoubleKeyword:
                return "double";
            case SyntaxKind::FloatKeyword:
                return "float";
            case SyntaxKind::StringKeyword:
                return "string";
            case SyntaxKind::CharKeyword:
                return "char";
            case SyntaxKind::VoidKeyword:
                return "void";
            case SyntaxKind::ObjectKeyword:
                return "object";
            case SyntaxKind::TypeOfKeyword:
                return "typeof";
            case SyntaxKind::SizeOfKeyword:
                return "sizeof";
            case SyntaxKind::NullKeyword:
                return "null";
            case SyntaxKind::TrueKeyword:
                return "true";
            case SyntaxKind::FalseKeyword:
                return "false";
            case SyntaxKind::IfKeyword:
                return "if";
            case SyntaxKind::ElseKeyword:
                return "else";
            case SyntaxKind::WhileKeyword:
                return "while";
            case SyntaxKind::ForKeyword:
                return "for";
            case SyntaxKind::ForEachKeyword:
                return "foreach";
            case SyntaxKind::DoKeyword:
                return "do";
            case SyntaxKind::SwitchKeyword:
                return "switch";
            case SyntaxKind::CaseKeyword:
                return "case";
            case SyntaxKind::DefaultKeyword:
                return "default";
            case SyntaxKind::TryKeyword:
                return "try";
            case SyntaxKind::CatchKeyword:
                return "catch";
            case SyntaxKind::FinallyKeyword:
                return "finally";
            case SyntaxKind::BreakKeyword:
                return "break";
            case SyntaxKind::ContinueKeyword:
                return "continue";
            case SyntaxKind::ReturnKeyword:
                return "return";
            case SyntaxKind::ThrowKeyword:
                return "throw";
            case SyntaxKind::PublicKeyword:
                return "public";
            case SyntaxKind::PrivateKeyword:
                return "private";
            case SyntaxKind::InternalKeyword:
                return "internal";
            case SyntaxKind::ProtectedKeyword:
                return "protected";
            case SyntaxKind::StaticKeyword:
                return "static";
            case SyntaxKind::ReadOnlyKeyword:
                return "readonly";
            case SyntaxKind::SealedKeyword:
                return "sealed";
            case SyntaxKind::ConstKeyword:
                return "const";
            case SyntaxKind::FixedKeyword:
                return "fixed";
            case SyntaxKind::StackAllocKeyword:
                return "stackalloc";
            case SyntaxKind::NewKeyword:
                return "new";
            case SyntaxKind::OverrideKeyword:
                return "override";
            case SyntaxKind::AbstractKeyword:
                return "abstract";
            case SyntaxKind::VirtualKeyword:
                return "virtual";
            case SyntaxKind::ExternKeyword:
                return "extern";
            case SyntaxKind::RefKeyword:
                return "ref";
            case SyntaxKind::OutKeyword:
                return "out";
            case SyntaxKind::InKeyword:
                return "in";
            case SyntaxKind::IsKeyword:
                return "is";
            case SyntaxKind::AsKeyword:
                return "as";
            case SyntaxKind::ParamsKeyword:
                return "params";
            case SyntaxKind::ThisKeyword:
                return "this";
            case SyntaxKind::BaseKeyword:
                return "base";
            case SyntaxKind::NamespaceKeyword:
                return "namespace";
            case SyntaxKind::UsingKeyword:
                return "using";
            case SyntaxKind::ClassKeyword:
                return "class";
            case SyntaxKind::StructKeyword:
                return "struct";
            case SyntaxKind::InterfaceKeyword:
                return "interface";
            case SyntaxKind::EnumKeyword:
                return "enum";
            case SyntaxKind::DelegateKeyword:
                return "delegate";
            case SyntaxKind::OperatorKeyword:
                return "operator";
            case SyntaxKind::ImplicitKeyword:
                return "implicit";
            case SyntaxKind::ExplicitKeyword:
                return "explicit";
            case SyntaxKind::ElifKeyword:
                return "elif";
            case SyntaxKind::EndIfKeyword:
                return "endif";

                // contextual keywords
            case SyntaxKind::PartialKeyword:
                return "partial";
            case SyntaxKind::FromKeyword:
                return "from";
            case SyntaxKind::WhereKeyword:
                return "where";
            case SyntaxKind::GetKeyword:
                return "get";
            case SyntaxKind::SetKeyword:
                return "set";
            case SyntaxKind::AliasKeyword:
                return "alias";
//            case SyntaxKind::GlobalKeyword:
//                return "global";
            case SyntaxKind::NameOfKeyword:
                return "nameof";
            case SyntaxKind::WhenKeyword:
                return "when";
            case SyntaxKind::UnderscoreToken:
                return "_";
            case SyntaxKind::VarKeyword:
                return "var";
            case SyntaxKind::AndKeyword:
                return "and";
            case SyntaxKind::OrKeyword:
                return "or";
            case SyntaxKind::NotKeyword:
                return "not";
            case SyntaxKind::WithKeyword:
                return "with";
            case SyntaxKind::InitKeyword:
                return "init";
            case SyntaxKind::RequiredKeyword:
                return "required";
            default:

                if(IsToken(kind) || IsContextualKeyword(kind) || IsReservedKeyword(kind)) {
                    NOT_IMPLEMENTED("text for symbol kind");
                }

                return "";
        }
    }

    bool IsBinaryExpressionOperatorToken(SyntaxKind token) {
        return GetBinaryExpression(token) != SyntaxKind::None;
    }


}