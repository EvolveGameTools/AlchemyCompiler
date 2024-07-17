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

    SyntaxKind GetPostfixUnaryExpression(TokenKind token) {
        switch (token) {
            case TokenKind::PlusPlusToken:
                return SyntaxKind::PostIncrementExpression;
            case TokenKind::MinusMinusToken:
                return SyntaxKind::PostDecrementExpression;
            case TokenKind::ExclamationToken:
                NOT_IMPLEMENTED("! postfix expression");
                return SyntaxKind::None; // todo -- this should be our make-it-work-or-panic operator
            default:
                return SyntaxKind::None;
        }
    }

    SyntaxKind GetPrefixUnaryExpression(TokenKind token) {
        switch (token) {
            case TokenKind::PlusToken:
                return SyntaxKind::UnaryPlusExpression;
            case TokenKind::MinusToken:
                return SyntaxKind::UnaryMinusExpression;
            case TokenKind::TildeToken:
                return SyntaxKind::BitwiseNotExpression;
            case TokenKind::ExclamationToken:
                return SyntaxKind::LogicalNotExpression;
            case TokenKind::PlusPlusToken:
                return SyntaxKind::PreIncrementExpression;
            case TokenKind::MinusMinusToken:
                return SyntaxKind::PreDecrementExpression;
            case TokenKind::CaretToken:
                return SyntaxKind::IndexExpression;
            default:
                return SyntaxKind::None;
        }
    }

    bool IsPrefixUnaryExpression(TokenKind token) {
        return GetPrefixUnaryExpression(token) != SyntaxKind::None;
    }

    bool IsPrefixUnaryExpressionOperatorToken(TokenKind token) {
        return GetPrefixUnaryExpression(token) != SyntaxKind::None;
    }

    bool IsPostfixUnaryExpression(TokenKind token) {
        return GetPostfixUnaryExpression(token) != SyntaxKind::None;
    }

    bool IsPostfixUnaryExpressionToken(TokenKind token) {
        return GetPostfixUnaryExpression(token) != SyntaxKind::None;
    }

    bool IsAnyUnaryExpression(TokenKind kind) {
        return IsPrefixUnaryExpression(kind) || IsPostfixUnaryExpression(kind);
    }

    SyntaxKind GetAssignmentExpression(TokenKind token) {
        switch (token) {
            case TokenKind::BarEqualsToken:
                return SyntaxKind::OrAssignmentExpression;
            case TokenKind::AmpersandEqualsToken:
                return SyntaxKind::AndAssignmentExpression;
            case TokenKind::CaretEqualsToken:
                return SyntaxKind::ExclusiveOrAssignmentExpression;
            case TokenKind::LessThanLessThanEqualsToken:
                return SyntaxKind::LeftShiftAssignmentExpression;
            case TokenKind::GreaterThanGreaterThanEqualsToken:
                return SyntaxKind::RightShiftAssignmentExpression;
            case TokenKind::GreaterThanGreaterThanGreaterThanEqualsToken:
                return SyntaxKind::UnsignedRightShiftAssignmentExpression;
            case TokenKind::PlusEqualsToken:
                return SyntaxKind::AddAssignmentExpression;
            case TokenKind::MinusEqualsToken:
                return SyntaxKind::SubtractAssignmentExpression;
            case TokenKind::AsteriskEqualsToken:
                return SyntaxKind::MultiplyAssignmentExpression;
            case TokenKind::SlashEqualsToken:
                return SyntaxKind::DivideAssignmentExpression;
            case TokenKind::PercentEqualsToken:
                return SyntaxKind::ModuloAssignmentExpression;
            case TokenKind::EqualsToken:
                return SyntaxKind::SimpleAssignmentExpression;
            case TokenKind::QuestionQuestionEqualsToken:
                return SyntaxKind::CoalesceAssignmentExpression;
            default:
                return SyntaxKind::None;
        }
    }

    SyntaxKind GetBinaryExpression(TokenKind kind) {
        switch (kind) {
            case TokenKind::QuestionQuestionToken:
                return SyntaxKind::CoalesceExpression;
            case TokenKind::IsKeyword:
                return SyntaxKind::IsExpression;
            case TokenKind::AsKeyword:
                return SyntaxKind::AsExpression;
            case TokenKind::BarToken:
                return SyntaxKind::BitwiseOrExpression;
            case TokenKind::CaretToken:
                return SyntaxKind::ExclusiveOrExpression;
            case TokenKind::AmpersandToken:
                return SyntaxKind::BitwiseAndExpression;
            case TokenKind::EqualsEqualsToken:
                return SyntaxKind::EqualsExpression;
            case TokenKind::ExclamationEqualsToken:
                return SyntaxKind::NotEqualsExpression;
            case TokenKind::LessThanToken:
                return SyntaxKind::LessThanExpression;
            case TokenKind::LessThanEqualsToken:
                return SyntaxKind::LessThanOrEqualExpression;
            case TokenKind::GreaterThanToken:
                return SyntaxKind::GreaterThanExpression;
            case TokenKind::GreaterThanEqualsToken:
                return SyntaxKind::GreaterThanOrEqualExpression;
            case TokenKind::LessThanLessThanToken:
                return SyntaxKind::LeftShiftExpression;
            case TokenKind::GreaterThanGreaterThanToken:
                return SyntaxKind::RightShiftExpression;
            case TokenKind::GreaterThanGreaterThanGreaterThanToken:
                return SyntaxKind::UnsignedRightShiftExpression;
            case TokenKind::PlusToken:
                return SyntaxKind::AddExpression;
            case TokenKind::MinusToken:
                return SyntaxKind::SubtractExpression;
            case TokenKind::AsteriskToken:
                return SyntaxKind::MultiplyExpression;
            case TokenKind::SlashToken:
                return SyntaxKind::DivideExpression;
            case TokenKind::PercentToken:
                return SyntaxKind::ModuloExpression;
            case TokenKind::AmpersandAmpersandToken:
                return SyntaxKind::LogicalAndExpression;
            case TokenKind::BarBarToken:
                return SyntaxKind::LogicalOrExpression;
            default:
                return SyntaxKind::None;
        }
    }

    bool IsBinaryExpression(TokenKind token) {
        return GetBinaryExpression(token) != SyntaxKind::None;
    }

    bool IsAssignmentExpressionOperatorToken(TokenKind token) {
        switch (token) {
            case TokenKind::QuestionQuestionEqualsToken:
            case TokenKind::BarEqualsToken:
            case TokenKind::AmpersandEqualsToken:
            case TokenKind::CaretEqualsToken:
            case TokenKind::LessThanLessThanEqualsToken:
            case TokenKind::GreaterThanGreaterThanEqualsToken:
            case TokenKind::GreaterThanGreaterThanGreaterThanEqualsToken:
            case TokenKind::PlusEqualsToken:
            case TokenKind::MinusEqualsToken:
            case TokenKind::AsteriskEqualsToken:
            case TokenKind::SlashEqualsToken:
            case TokenKind::PercentEqualsToken:
            case TokenKind::EqualsToken:
                return true;
            default:
                return false;
        }
    }

    bool IsPredefinedType(TokenKind kind) {
        switch (kind) {
            // todo -- aliases, float vectors, colors, dynamic
            case TokenKind::BoolKeyword:
            case TokenKind::ByteKeyword:
            case TokenKind::SByteKeyword:
            case TokenKind::IntKeyword:
            case TokenKind::UIntKeyword:
            case TokenKind::ShortKeyword:
            case TokenKind::UShortKeyword:
            case TokenKind::LongKeyword:
            case TokenKind::ULongKeyword:
            case TokenKind::FloatKeyword:
            case TokenKind::DoubleKeyword:
            case TokenKind::StringKeyword:
            case TokenKind::CharKeyword:
            case TokenKind::ObjectKeyword:
            case TokenKind::VoidKeyword:
                return true;
            default:
                return false;
        }
    }

    bool IsReservedKeyword(TokenKind kind) {
        int32 v = (int32) kind;
        constexpr int32 lower = (int32) TokenKind::__FirstKeyword__;
        constexpr int32 upper = (int32) TokenKind::__LastKeyword__;
        return v > lower && v < upper;
    }

    bool IsToken(TokenKind kind) {
        int32 v = (int32) kind;
        constexpr int32 lower = (int32) TokenKind::__FirstToken__;
        constexpr int32 upper = (int32) TokenKind::__LastToken__;
        return v > lower && v < upper;
    }

    bool IsContextualKeyword(TokenKind kind) {
        int32 v = (int32) kind;
        constexpr int32 lower = (int32) TokenKind::__FirstContextualKeyword__;
        constexpr int32 upper = (int32) TokenKind::__LastContextualKeyword__;
        return v > lower && v < upper;
    }

    const char* GetText(TokenKind kind) {

        switch (kind) {
            case TokenKind::TildeToken:
                return "~";
            case TokenKind::ExclamationToken:
                return "!";
            case TokenKind::DollarToken:
                return "$";
            case TokenKind::PercentToken:
                return "%";
            case TokenKind::CaretToken:
                return "^";
            case TokenKind::AmpersandToken:
                return "&";
            case TokenKind::AsteriskToken:
                return "*";
            case TokenKind::OpenParenToken:
                return "(";
            case TokenKind::CloseParenToken:
                return ")";
            case TokenKind::MinusToken:
                return "-";
            case TokenKind::PlusToken:
                return "+";
            case TokenKind::EqualsToken:
                return "=";
            case TokenKind::OpenBraceToken:
                return "{";
            case TokenKind::CloseBraceToken:
                return "}";
            case TokenKind::OpenBracketToken:
                return "[";
            case TokenKind::CloseBracketToken:
                return "]";
            case TokenKind::BarToken:
                return "|";
            case TokenKind::BackslashToken:
                return "\\";
            case TokenKind::ColonToken:
                return ":";
            case TokenKind::SemicolonToken:
                return ";";
            case TokenKind::DoubleQuoteToken:
                return "\"";
            case TokenKind::SingleQuoteToken:
                return "'";
            case TokenKind::LessThanToken:
                return "<";
            case TokenKind::CommaToken:
                return ",";
            case TokenKind::GreaterThanToken:
                return ">";
            case TokenKind::DotToken:
                return ".";
            case TokenKind::QuestionToken:
                return "?";
            case TokenKind::HashToken:
                return "#";
            case TokenKind::SlashToken:
                return "/";

                // compound
            case TokenKind::BarBarToken:
                return "||";
            case TokenKind::AmpersandAmpersandToken:
                return "&&";
            case TokenKind::MinusMinusToken:
                return "--";
            case TokenKind::PlusPlusToken:
                return "++";
            case TokenKind::ColonColonToken:
                return "::";
            case TokenKind::QuestionQuestionToken:
                return "??";
            case TokenKind::MinusGreaterThanToken:
                return "->";
            case TokenKind::ExclamationEqualsToken:
                return "!=";
            case TokenKind::EqualsEqualsToken:
                return "==";
            case TokenKind::EqualsGreaterThanToken:
                return "=>";
            case TokenKind::LessThanEqualsToken:
                return "<=";
            case TokenKind::LessThanLessThanToken:
                return "<<";
            case TokenKind::LessThanLessThanEqualsToken:
                return "<<=";
            case TokenKind::GreaterThanEqualsToken:
                return ">=";
            case TokenKind::GreaterThanGreaterThanToken:
                return ">>";
            case TokenKind::GreaterThanGreaterThanEqualsToken:
                return ">>=";
            case TokenKind::GreaterThanGreaterThanGreaterThanToken:
                return ">>>";
            case TokenKind::GreaterThanGreaterThanGreaterThanEqualsToken:
                return ">>>=";
            case TokenKind::SlashEqualsToken:
                return "/=";
            case TokenKind::AsteriskEqualsToken:
                return "*=";
            case TokenKind::BarEqualsToken:
                return "|=";
            case TokenKind::AmpersandEqualsToken:
                return "&=";
            case TokenKind::PlusEqualsToken:
                return "+=";
            case TokenKind::MinusEqualsToken:
                return "-=";
            case TokenKind::CaretEqualsToken:
                return "^=";
            case TokenKind::PercentEqualsToken:
                return "%=";
            case TokenKind::QuestionQuestionEqualsToken:
                return "??=";
            case TokenKind::DotDotToken:
                return "..";

                // Keywords
            case TokenKind::BoolKeyword:
                return "bool";
            case TokenKind::ByteKeyword:
                return "byte";
            case TokenKind::SByteKeyword:
                return "sbyte";
            case TokenKind::ShortKeyword:
                return "short";
            case TokenKind::UShortKeyword:
                return "ushort";
            case TokenKind::IntKeyword:
                return "int";
            case TokenKind::UIntKeyword:
                return "uint";
            case TokenKind::LongKeyword:
                return "long";
            case TokenKind::ULongKeyword:
                return "ulong";
            case TokenKind::DoubleKeyword:
                return "double";
            case TokenKind::FloatKeyword:
                return "float";
            case TokenKind::StringKeyword:
                return "string";
            case TokenKind::CharKeyword:
                return "char";
            case TokenKind::VoidKeyword:
                return "void";
            case TokenKind::ObjectKeyword:
                return "object";
            case TokenKind::NullKeyword:
                return "null";
            case TokenKind::TrueKeyword:
                return "true";
            case TokenKind::FalseKeyword:
                return "false";
            case TokenKind::IfKeyword:
                return "if";
            case TokenKind::ElseKeyword:
                return "else";
            case TokenKind::WhileKeyword:
                return "while";
            case TokenKind::ForKeyword:
                return "for";
            case TokenKind::ForEachKeyword:
                return "foreach";
            case TokenKind::DoKeyword:
                return "do";
            case TokenKind::SwitchKeyword:
                return "switch";
            case TokenKind::CaseKeyword:
                return "case";
            case TokenKind::DefaultKeyword:
                return "default";
            case TokenKind::TryKeyword:
                return "try";
            case TokenKind::CatchKeyword:
                return "catch";
            case TokenKind::FinallyKeyword:
                return "finally";
            case TokenKind::BreakKeyword:
                return "break";
            case TokenKind::ContinueKeyword:
                return "continue";
            case TokenKind::ReturnKeyword:
                return "return";
            case TokenKind::ThrowKeyword:
                return "throw";
            case TokenKind::PublicKeyword:
                return "public";
            case TokenKind::PrivateKeyword:
                return "private";
            case TokenKind::InternalKeyword:
                return "internal";
            case TokenKind::ProtectedKeyword:
                return "protected";
            case TokenKind::StaticKeyword:
                return "static";
            case TokenKind::ReadOnlyKeyword:
                return "readonly";
            case TokenKind::SealedKeyword:
                return "sealed";
            case TokenKind::ConstKeyword:
                return "const";
            case TokenKind::NewKeyword:
                return "new";
            case TokenKind::OverrideKeyword:
                return "override";
            case TokenKind::AbstractKeyword:
                return "abstract";
            case TokenKind::VirtualKeyword:
                return "virtual";
            case TokenKind::ExternKeyword:
                return "extern";
            case TokenKind::RefKeyword:
                return "ref";
            case TokenKind::OutKeyword:
                return "out";
            case TokenKind::IsKeyword:
                return "is";
            case TokenKind::AsKeyword:
                return "as";
            case TokenKind::ParamsKeyword:
                return "params";
            case TokenKind::ThisKeyword:
                return "this";
            case TokenKind::BaseKeyword:
                return "base";
            case TokenKind::NamespaceKeyword:
                return "namespace";
            case TokenKind::UsingKeyword:
                return "using";
            case TokenKind::ClassKeyword:
                return "class";
            case TokenKind::StructKeyword:
                return "struct";
            case TokenKind::InterfaceKeyword:
                return "interface";
            case TokenKind::EnumKeyword:
                return "enum";
            case TokenKind::DelegateKeyword:
                return "delegate";
            case TokenKind::OperatorKeyword:
                return "operator";
            case TokenKind::ImplicitKeyword:
                return "implicit";
            case TokenKind::ElifKeyword:
                return "elif";
            case TokenKind::EndIfKeyword:
                return "endif";

                // contextual keywords
            case TokenKind::FromKeyword:
                return "from";
            case TokenKind::WhereKeyword:
                return "where";
            case TokenKind::GetKeyword:
                return "get";
            case TokenKind::SetKeyword:
                return "set";
            case TokenKind::WhenKeyword:
                return "when";
            case TokenKind::UnderscoreToken:
                return "_";
            case TokenKind::VarKeyword:
                return "var";
            case TokenKind::AndKeyword:
                return "and";
            case TokenKind::OrKeyword:
                return "or";
            case TokenKind::NotKeyword:
                return "not";
            case TokenKind::WithKeyword:
                return "with";
            case TokenKind::InitKeyword:
                return "init";
            case TokenKind::RequiredKeyword:
                return "required";
            default:

                if (IsToken(kind) || IsContextualKeyword(kind) || IsReservedKeyword(kind)) {
                    NOT_IMPLEMENTED("text for symbol kind");
                }

                return "";
        }
    }

    bool IsBinaryExpressionOperatorToken(TokenKind token) {
        return GetBinaryExpression(token) != SyntaxKind::None;
    }

    bool IsValidArgumentRefKindKeyword(TokenKind kind) {
        switch (kind) {
            case TokenKind::RefKeyword:
            case TokenKind::OutKeyword:
//            case SyntaxKind.InKeyword:
                return true;
            default:
                return false;
        }
    }

    bool IsInvalidSubExpression(TokenKind kind) {
        switch (kind) {
            case TokenKind::BreakKeyword:
            case TokenKind::CaseKeyword:
            case TokenKind::CatchKeyword:
            case TokenKind::ConstKeyword:
            case TokenKind::ContinueKeyword:
            case TokenKind::DoKeyword:
            case TokenKind::FinallyKeyword:
            case TokenKind::ForKeyword:
            case TokenKind::ForEachKeyword:
            case TokenKind::GotoKeyword:
            case TokenKind::IfKeyword:
            case TokenKind::ElseKeyword:
            case TokenKind::LockKeyword:
            case TokenKind::ReturnKeyword:
            case TokenKind::SwitchKeyword:
            case TokenKind::TryKeyword:
            case TokenKind::UsingKeyword:
            case TokenKind::WhileKeyword:
                return true;
            default:
                return false;
        }
    }

    bool IsExpectedPrefixUnaryOperator(TokenKind kind) {
        return SyntaxFacts::IsPrefixUnaryExpression(kind) && kind != TokenKind::RefKeyword && kind != TokenKind::OutKeyword;
    }

    Precedence GetPrecedence(SyntaxKind op) {
        switch (op) {
            case SyntaxKind::ParenthesizedLambdaExpression:
            case SyntaxKind::SimpleLambdaExpression:
            case SyntaxKind::AnonymousMethodExpression:
                return Precedence::Lambda;
            case SyntaxKind::SimpleAssignmentExpression:
            case SyntaxKind::AddAssignmentExpression:
            case SyntaxKind::SubtractAssignmentExpression:
            case SyntaxKind::MultiplyAssignmentExpression:
            case SyntaxKind::DivideAssignmentExpression:
            case SyntaxKind::ModuloAssignmentExpression:
            case SyntaxKind::AndAssignmentExpression:
            case SyntaxKind::ExclusiveOrAssignmentExpression:
            case SyntaxKind::OrAssignmentExpression:
            case SyntaxKind::LeftShiftAssignmentExpression:
            case SyntaxKind::RightShiftAssignmentExpression:
            case SyntaxKind::UnsignedRightShiftAssignmentExpression:
            case SyntaxKind::CoalesceAssignmentExpression:
                return Precedence::Assignment;
            case SyntaxKind::CoalesceExpression:
            case SyntaxKind::ThrowExpression:
                return Precedence::Coalescing;
            case SyntaxKind::LogicalOrExpression:
                return Precedence::ConditionalOr;
            case SyntaxKind::LogicalAndExpression:
                return Precedence::ConditionalAnd;
            case SyntaxKind::BitwiseOrExpression:
                return Precedence::LogicalOr;
            case SyntaxKind::ExclusiveOrExpression:
                return Precedence::LogicalXor;
            case SyntaxKind::BitwiseAndExpression:
                return Precedence::LogicalAnd;
            case SyntaxKind::EqualsExpression:
            case SyntaxKind::NotEqualsExpression:
                return Precedence::Equality;
            case SyntaxKind::LessThanExpression:
            case SyntaxKind::LessThanOrEqualExpression:
            case SyntaxKind::GreaterThanExpression:
            case SyntaxKind::GreaterThanOrEqualExpression:
            case SyntaxKind::IsExpression:
            case SyntaxKind::AsExpression:
            case SyntaxKind::IsPatternExpression:
                return Precedence::Relational;
            case SyntaxKind::SwitchExpression:
            case SyntaxKind::WithExpression:
                return Precedence::Switch;
            case SyntaxKind::LeftShiftExpression:
            case SyntaxKind::RightShiftExpression:
            case SyntaxKind::UnsignedRightShiftExpression:
                return Precedence::Shift;
            case SyntaxKind::AddExpression:
            case SyntaxKind::SubtractExpression:
                return Precedence::Additive;
            case SyntaxKind::MultiplyExpression:
            case SyntaxKind::DivideExpression:
            case SyntaxKind::ModuloExpression:
                return Precedence::Multiplicative;
            case SyntaxKind::UnaryPlusExpression:
            case SyntaxKind::UnaryMinusExpression:
            case SyntaxKind::BitwiseNotExpression:
            case SyntaxKind::LogicalNotExpression:
            case SyntaxKind::PreIncrementExpression:
            case SyntaxKind::PreDecrementExpression:
            case SyntaxKind::TypeOfExpression:
            case SyntaxKind::SizeOfExpression:
            case SyntaxKind::IndexExpression:
                return Precedence::Unary;
            case SyntaxKind::CastExpression:
                return Precedence::Cast;
            case SyntaxKind::RangeExpression:
                return Precedence::Range;
            case SyntaxKind::ConditionalExpression:
                return Precedence::Expression;
            case SyntaxKind::AnonymousObjectCreationExpression:
            case SyntaxKind::ArrayCreationExpression:
            case SyntaxKind::BaseExpression:
            case SyntaxKind::CharacterLiteralExpression:
            case SyntaxKind::CollectionExpression:
            case SyntaxKind::ConditionalAccessExpression:
            case SyntaxKind::DeclarationExpression:
            case SyntaxKind::DefaultExpression:
            case SyntaxKind::DefaultLiteralExpression:
            case SyntaxKind::ElementAccessExpression:
            case SyntaxKind::FalseLiteralExpression:
            case SyntaxKind::GenericName:
            case SyntaxKind::IdentifierName:
            case SyntaxKind::ImplicitArrayCreationExpression:
            case SyntaxKind::ImplicitStackAllocArrayCreationExpression:
            case SyntaxKind::ImplicitObjectCreationExpression:
            case SyntaxKind::InterpolatedStringExpression:
            case SyntaxKind::InvocationExpression:
            case SyntaxKind::NullLiteralExpression:
            case SyntaxKind::NumericLiteralExpression:
            case SyntaxKind::ObjectCreationExpression:
            case SyntaxKind::ParenthesizedExpression:
            case SyntaxKind::PointerMemberAccessExpression:
            case SyntaxKind::PostDecrementExpression:
            case SyntaxKind::PostIncrementExpression:
            case SyntaxKind::PredefinedType:
            case SyntaxKind::RefExpression:
            case SyntaxKind::SimpleMemberAccessExpression:
            case SyntaxKind::StackAllocArrayCreationExpression:
            case SyntaxKind::StringLiteralExpression:
            case SyntaxKind::ThisExpression:
            case SyntaxKind::TrueLiteralExpression:
            case SyntaxKind::TupleExpression:
                return Precedence::Primary;
            default:
                UNREACHABLE("GetPrecedence");
                return Precedence::Expression;
        }
    }

    SyntaxKind GetLiteralExpression(TokenKind kind) {
        switch (kind) {
            case TokenKind::StringLiteralStart:
                return SyntaxKind::StringLiteralExpression;
            case TokenKind::StringLiteralEmpty:
                return SyntaxKind::EmptyStringLiteralExpression;
            case TokenKind::CharLiteralStart:
                return SyntaxKind::CharacterLiteralExpression;
            case TokenKind::NumericLiteralToken:
                return SyntaxKind::NumericLiteralExpression;
            case TokenKind::NullKeyword:
                return SyntaxKind::NullLiteralExpression;
            case TokenKind::TrueKeyword:
                return SyntaxKind::TrueLiteralExpression;
            case TokenKind::FalseKeyword:
                return SyntaxKind::FalseLiteralExpression;

                // case TokenKind::DefaultKeyword:
                //     return SyntaxKind::DefaultLiteralExpression;

            default:
                return SyntaxKind::None;
        }
    }

    bool CanFollowCast(TokenKind kind) {
        switch (kind) {
            case TokenKind::AsKeyword:
            case TokenKind::IsKeyword:
            case TokenKind::SemicolonToken:
            case TokenKind::CloseParenToken:
            case TokenKind::CloseBracketToken:
            case TokenKind::OpenBraceToken:
            case TokenKind::CloseBraceToken:
            case TokenKind::CommaToken:
            case TokenKind::EqualsToken:
            case TokenKind::PlusEqualsToken:
            case TokenKind::MinusEqualsToken:
            case TokenKind::AsteriskEqualsToken:
            case TokenKind::SlashEqualsToken:
            case TokenKind::PercentEqualsToken:
            case TokenKind::AmpersandEqualsToken:
            case TokenKind::CaretEqualsToken:
            case TokenKind::BarEqualsToken:
            case TokenKind::LessThanLessThanEqualsToken:
            case TokenKind::GreaterThanGreaterThanEqualsToken:
            case TokenKind::GreaterThanGreaterThanGreaterThanEqualsToken:
            case TokenKind::QuestionToken:
            case TokenKind::ColonToken:
            case TokenKind::BarBarToken:
            case TokenKind::AmpersandAmpersandToken:
            case TokenKind::BarToken:
            case TokenKind::CaretToken:
            case TokenKind::AmpersandToken:
            case TokenKind::EqualsEqualsToken:
            case TokenKind::ExclamationEqualsToken:
            case TokenKind::LessThanToken:
            case TokenKind::LessThanEqualsToken:
            case TokenKind::GreaterThanToken:
            case TokenKind::GreaterThanEqualsToken:
            case TokenKind::QuestionQuestionEqualsToken:
            case TokenKind::LessThanLessThanToken:
            case TokenKind::GreaterThanGreaterThanToken:
            case TokenKind::GreaterThanGreaterThanGreaterThanToken:
            case TokenKind::PlusToken:
            case TokenKind::MinusToken:
            case TokenKind::AsteriskToken:
            case TokenKind::SlashToken:
            case TokenKind::PercentToken:
            case TokenKind::PlusPlusToken:
            case TokenKind::MinusMinusToken:
            case TokenKind::OpenBracketToken:
            case TokenKind::DotToken:
            case TokenKind::MinusGreaterThanToken:
            case TokenKind::QuestionQuestionToken:
            case TokenKind::EndOfFileToken:
            case TokenKind::SwitchKeyword:
            case TokenKind::EqualsGreaterThanToken:
            case TokenKind::DotDotToken:
                return false;
            default:
                return true;
        }
    }

    bool IsRightAssociative(SyntaxKind op) {
        switch (op) {
            case SyntaxKind::SimpleAssignmentExpression:
            case SyntaxKind::AddAssignmentExpression:
            case SyntaxKind::SubtractAssignmentExpression:
            case SyntaxKind::MultiplyAssignmentExpression:
            case SyntaxKind::DivideAssignmentExpression:
            case SyntaxKind::ModuloAssignmentExpression:
            case SyntaxKind::AndAssignmentExpression:
            case SyntaxKind::ExclusiveOrAssignmentExpression:
            case SyntaxKind::OrAssignmentExpression:
            case SyntaxKind::LeftShiftAssignmentExpression:
            case SyntaxKind::RightShiftAssignmentExpression:
            case SyntaxKind::UnsignedRightShiftAssignmentExpression:
            case SyntaxKind::CoalesceAssignmentExpression:
            case SyntaxKind::CoalesceExpression:
                return true;
            default:
                return false;
        }
    }

    bool IsPatternSyntax(SyntaxKind kind) {
        return (int32) kind > (int32) SyntaxKind::__PATTERN_START__ && (int32) kind < (int32) SyntaxKind::__PATTERN_END__;
    }

    bool IsTypeSyntax(SyntaxKind kind) {
        return (int32) kind > (int32) SyntaxKind::__TYPE_START__ && (int32) kind < (int32) SyntaxKind::__TYPE_END__;
    }

    bool CanTokenFollowTypeInPattern(TokenKind kind, Precedence precedence) {
        switch (kind) {
            case TokenKind::OpenParenToken:
            case TokenKind::OpenBraceToken:
            case TokenKind::IdentifierToken:
            case TokenKind::CloseBraceToken:   // for efficiency, test some tokens that can follow a type pattern
            case TokenKind::CloseBracketToken:
            case TokenKind::CloseParenToken:
            case TokenKind::CommaToken:
            case TokenKind::SemicolonToken:
                return true;
            case TokenKind::DotToken:
                // int.MaxValue is an expression, not a type.
                return false;
            case TokenKind::MinusGreaterThanToken:
            case TokenKind::ExclamationToken:
                // parse as an expression for error recovery
                return false;
            default: {
                // If we find what looks like a continuation of an expression, it is not a type.
                return !IsBinaryExpressionOperatorToken(kind) || GetPrecedence(GetBinaryExpression(kind)) <= precedence;
            }
        }
    }

    bool IsAdditionalLocalFunctionModifier(TokenKind kind) {
        switch (kind) {
            case TokenKind::StaticKeyword:
                // Not a valid modifier, but we should parse to give a good
                // error message
            case TokenKind::PublicKeyword:
            case TokenKind::InternalKeyword:
            case TokenKind::ProtectedKeyword:
            case TokenKind::PrivateKeyword:
                return true;

            default:
                return false;
        }
    }

    bool IsDeclarationModifier(TokenKind kind) {
        switch (kind) {
            case TokenKind::ConstKeyword:
            case TokenKind::StaticKeyword:
            case TokenKind::ReadOnlyKeyword:
                return true;
            default:
                return false;
        }
    }

    bool IsTypeModifierOrTypeKeyword(TokenKind kind) {
        switch (kind) {
            case TokenKind::EnumKeyword:
            case TokenKind::DelegateKeyword:
            case TokenKind::ClassKeyword:
            case TokenKind::InterfaceKeyword:
            case TokenKind::StructKeyword:
            case TokenKind::AbstractKeyword:
            case TokenKind::InternalKeyword:
            case TokenKind::NewKeyword: // this is for type constraints I think
            case TokenKind::PrivateKeyword:
            case TokenKind::ProtectedKeyword:
            case TokenKind::PublicKeyword:
            case TokenKind::SealedKeyword:
            case TokenKind::StaticKeyword:
                return true;
            default:
                return false;
        }
    }

    bool IsAccessibilityModifier(TokenKind kind) {
        switch (kind) {
            case TokenKind::PublicKeyword:
            case TokenKind::InternalKeyword:
            case TokenKind::ProtectedKeyword:
            case TokenKind::PrivateKeyword:
                return true;

            default:
                return false;
        }
    }

    bool IsExpressionSyntax(SyntaxKind kind) {
        NOT_IMPLEMENTED("IsExpressionSyntax");
        switch (kind) {
            default:
                return false;
        }
    }

    bool IsName(SyntaxKind kind) {
        switch (kind) {
            case SyntaxKind::IdentifierName:
            case SyntaxKind::GenericName:
            case SyntaxKind::QualifiedName:
                return true;
            default:
                return false;
        }
    }

    bool IsLiteralExpression(TokenKind kind) {
        return GetLiteralExpression(kind) == SyntaxKind::None;
    }

    SyntaxKind GetAccessorDeclarationKind(TokenKind keyword) {
        switch (keyword) {
            case TokenKind::GetKeyword:
                return SyntaxKind::GetAccessorDeclaration;
            case TokenKind::SetKeyword:
                return SyntaxKind::SetAccessorDeclaration;
            case TokenKind::InitKeyword:
                return SyntaxKind::InitAccessorDeclaration;
            default:
                return SyntaxKind::None;
        }
    }


}