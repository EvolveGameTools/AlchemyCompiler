#pragma once

#include "./SyntaxBase.h"

#define abstract

namespace Alchemy::Compilation {

    struct ArrayRankSpecifierSyntax : ExpressionSyntax {
        SyntaxToken open;
        SeparatedSyntaxList<ExpressionSyntax>* ranks;
        SyntaxToken close;

        ArrayRankSpecifierSyntax(SyntaxToken open, SeparatedSyntaxList<ExpressionSyntax>* ranks, SyntaxToken close)
            : ExpressionSyntax(SyntaxKind::ArrayRankSpecifier)
            , open(open)
            , ranks(ranks)
            , close(close) {}

    };

    struct TypeArgumentListSyntax : SyntaxBase {

        SyntaxToken lessThanToken;
        SeparatedSyntaxList<TypeSyntax>* arguments;
        SyntaxToken greaterThanToken;

        TypeArgumentListSyntax(SyntaxToken lessThanToken, SeparatedSyntaxList<TypeSyntax>* arguments, SyntaxToken greaterThanToken)
            : SyntaxBase(SyntaxKind::TypeArgumentList)
            , lessThanToken(lessThanToken)
            , arguments(arguments)
            , greaterThanToken(greaterThanToken) {}

    };

    struct GenericNameSyntax : SimpleNameSyntax {

        SyntaxToken identifier;
        TypeArgumentListSyntax* typeArgumentList;

        GenericNameSyntax(SyntaxToken identifier, TypeArgumentListSyntax* typeArgumentListSyntax)
            : SimpleNameSyntax(SyntaxKind::GenericName)
            , identifier(identifier)
            , typeArgumentList(typeArgumentListSyntax) {}

    };

    struct BracketedArgumentListSyntax;

    struct ElementBindingExpressionSyntax : ExpressionSyntax {

        BracketedArgumentListSyntax* argumentList;

        explicit ElementBindingExpressionSyntax(BracketedArgumentListSyntax* argumentList)
            : ExpressionSyntax(SyntaxKind::ElementBindingExpression)
            , argumentList(argumentList) {}

    };

    struct MemberBindingExpressionSyntax : ExpressionSyntax {

        SyntaxToken operatorToken;
        SimpleNameSyntax* name;

        MemberBindingExpressionSyntax(SyntaxToken operatorToken, SimpleNameSyntax* name)
            : ExpressionSyntax(SyntaxKind::MemberBindingExpression)
            , operatorToken(operatorToken)
            , name(name) {}

    };

    struct ConditionalAccessExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        SyntaxToken operatorToken;
        ExpressionSyntax* whenNotNull;

        ConditionalAccessExpressionSyntax(ExpressionSyntax* expression, SyntaxToken operatorToken, ExpressionSyntax* whenNotNull)
            : ExpressionSyntax(SyntaxKind::ConditionalAccessExpression)
            , expression(expression)
            , operatorToken(operatorToken)
            , whenNotNull(whenNotNull) {}

    };

    abstract struct MemberAccessExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        SyntaxToken operatorToken;
        SimpleNameSyntax* name;

        MemberAccessExpressionSyntax(SyntaxKind kind, ExpressionSyntax* expression, SyntaxToken operatorToken, SimpleNameSyntax* name)
            : ExpressionSyntax(kind)
            , expression(expression)
            , operatorToken(operatorToken)
            , name(name) {}

    };

    struct QualifiedNameSyntax : NameSyntax {
        NameSyntax* left;
        SyntaxToken dotToken;
        SimpleNameSyntax* right;

        explicit QualifiedNameSyntax(NameSyntax* left, SyntaxToken dotToken, SimpleNameSyntax* right)
            : NameSyntax(SyntaxKind::QualifiedName)
            , left(left)
            , dotToken(dotToken)
            , right(right) {}

    };

    struct IdentifierNameSyntax : SimpleNameSyntax {

        SyntaxToken identifier;

        explicit IdentifierNameSyntax(SyntaxToken identifier)
            : SimpleNameSyntax(SyntaxKind::IdentifierName)
            , identifier(identifier) {}

    };

    struct NameColonSyntax : BaseExpressionColonSyntax {
        IdentifierNameSyntax* name;
        SyntaxToken colonToken;

        NameColonSyntax(IdentifierNameSyntax* name, SyntaxToken colonToken)
            : BaseExpressionColonSyntax(SyntaxKind::NameColon)
            , name(name)
            , colonToken(colonToken) {}

    };

    struct PredefinedTypeSyntax : TypeSyntax {
        SyntaxToken typeToken;

        explicit PredefinedTypeSyntax(SyntaxToken typeToken)
            : TypeSyntax(SyntaxKind::PredefinedType)
            , typeToken(typeToken) {}

    };

    struct TupleElementSyntax : SyntaxBase {
        TypeSyntax* type;
        SyntaxToken identifier; // optional

        TupleElementSyntax(TypeSyntax* type, SyntaxToken identifier)
            : SyntaxBase(SyntaxKind::TupleElement)
            , type(type)
            , identifier(identifier) {}

    };

    struct TupleTypeSyntax : TypeSyntax {

        SyntaxToken openParenToken;
        SeparatedSyntaxList<TupleElementSyntax>* elements;
        SyntaxToken closeParenToken;

        TupleTypeSyntax(SyntaxToken openParenToken, SeparatedSyntaxList<TupleElementSyntax>* elements, SyntaxToken closeParenToken)
            : TypeSyntax(SyntaxKind::TupleType)
            , openParenToken(openParenToken)
            , elements(elements)
            , closeParenToken(closeParenToken) {}

    };

    struct ArrayTypeSyntax : TypeSyntax {

        TypeSyntax* elementType;
        SyntaxList<ArrayRankSpecifierSyntax>* ranks;

        ArrayTypeSyntax(TypeSyntax* elementType, SyntaxList<ArrayRankSpecifierSyntax>* ranks)
            : TypeSyntax(SyntaxKind::ArrayType)
            , elementType(elementType)
            , ranks(ranks) {}

    };

    struct RefTypeSyntax : TypeSyntax {

        SyntaxToken refKeyword;
        SyntaxToken readonlyKeyword;
        TypeSyntax* type;

        RefTypeSyntax(SyntaxToken refKeyword, SyntaxToken readonlyKeyword, TypeSyntax* typeSyntax)
            : TypeSyntax(SyntaxKind::RefType)
            , refKeyword(refKeyword)
            , readonlyKeyword(readonlyKeyword)
            , type(typeSyntax) {}

    };

    struct OmittedArraySizeExpressionSyntax : ExpressionSyntax {

        SyntaxToken token;

        explicit OmittedArraySizeExpressionSyntax(SyntaxToken token)
            : ExpressionSyntax(SyntaxKind::OmittedArraySizeExpression)
            , token(token) {}

    };


    struct NullableType : TypeSyntax {

        TypeSyntax* elementType;
        SyntaxToken questionMark;

        NullableType(TypeSyntax* elementType, SyntaxToken questionMark)
            : TypeSyntax(SyntaxKind::NullableType)
            , elementType(elementType)
            , questionMark(questionMark) {}

    };

    struct ArgumentSyntax : SyntaxBase {

        NameColonSyntax* nameColon;
        SyntaxToken refKindKeyword;
        ExpressionSyntax* expression;

        ArgumentSyntax(NameColonSyntax* nameColon, SyntaxToken refKindKeyword, ExpressionSyntax* expression)
            : SyntaxBase(SyntaxKind::Argument)
            , nameColon(nameColon)
            , refKindKeyword(refKindKeyword)
            , expression(expression) {}

    };

    struct NameEqualsSyntax : SyntaxBase {
        IdentifierNameSyntax* name;
        SyntaxToken equalsToken;

        NameEqualsSyntax(IdentifierNameSyntax* name, SyntaxToken equalsToken)
            : SyntaxBase(SyntaxKind::NameEquals)
            , name(name)
            , equalsToken(equalsToken) {}

    };

    struct ImplicitArrayCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newKeyword;
        SyntaxToken openBracket;
        TokenList* commas;
        SyntaxToken closeBracket;
        ExpressionSyntax* initializer;

        ImplicitArrayCreationExpressionSyntax(SyntaxToken newKeyword, SyntaxToken openBracket, TokenList* commas, SyntaxToken closeBracket, ExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ImplicitArrayCreationExpression)
            , newKeyword(newKeyword)
            , openBracket(openBracket)
            , commas(commas)
            , closeBracket(closeBracket)
            , initializer(initializer) {}


    };


    struct InitializerExpressionSyntax : ExpressionSyntax {

        SyntaxToken openBraceToken;
        SeparatedSyntaxList<ExpressionSyntax>* list;
        SyntaxToken closeBraceToken;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::ObjectInitializerExpression,
            SyntaxKind::CollectionInitializerExpression,
            SyntaxKind::ArrayInitializerExpression,
            SyntaxKind::ComplexElementInitializerExpression,
            SyntaxKind::WithInitializerExpression,
        };

        InitializerExpressionSyntax(SyntaxKind kind, SyntaxToken openBraceToken, SeparatedSyntaxList<ExpressionSyntax>* list, SyntaxToken closeBraceToken)
            : ExpressionSyntax(kind)
            , openBraceToken(openBraceToken)
            , list(list)
            , closeBraceToken(closeBraceToken) {
            ASSERT_VALID_SYNTAX_KIND(kind);
        }

    };

    struct StackAllocArrayCreationExpressionSyntax : ExpressionSyntax {

        SyntaxToken stackallocKeyword;
        TypeSyntax* type;
        InitializerExpressionSyntax* initializer;

        StackAllocArrayCreationExpressionSyntax(SyntaxToken stackallocKeyword, TypeSyntax* type, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::StackAllocArrayCreationExpression)
            , stackallocKeyword(stackallocKeyword)
            , type(type)
            , initializer(initializer) {}

    };

    struct ImplicitStackAllocArrayCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken stackallocKeyword;
        SyntaxToken openBracket;
        SyntaxToken closeBracket;
        InitializerExpressionSyntax* initializer;

        ImplicitStackAllocArrayCreationExpressionSyntax(SyntaxToken stackallocKeyword, SyntaxToken openBracket, SyntaxToken closeBracket, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ImplicitStackAllocArrayCreationExpression)
            , stackallocKeyword(stackallocKeyword)
            , openBracket(openBracket)
            , closeBracket(closeBracket)
            , initializer(initializer) {}

    };

    struct ArgumentListSyntax : SyntaxBase {
        SyntaxToken openToken;
        SeparatedSyntaxList<ArgumentSyntax>* arguments;
        SyntaxToken closeToken;

        ArgumentListSyntax(SyntaxToken openToken, SeparatedSyntaxList<ArgumentSyntax>* arguments, SyntaxToken closeToken)
            : SyntaxBase(SyntaxKind::ArgumentList)
            , openToken(openToken)
            , arguments(arguments)
            , closeToken(closeToken) {}

    };

    struct ObjectCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newKeyword;
        TypeSyntax* type;
        ArgumentListSyntax* arguments;
        InitializerExpressionSyntax* initializer;

        ObjectCreationExpressionSyntax(SyntaxToken newKeyword, TypeSyntax* type, ArgumentListSyntax* arguments, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ObjectCreationExpression)
            , newKeyword(newKeyword)
            , type(type)
            , arguments(arguments)
            , initializer(initializer) {}

    };

    struct ImplicitObjectCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newKeyword;
        ArgumentListSyntax* arguments;
        InitializerExpressionSyntax* initializer;

        ImplicitObjectCreationExpressionSyntax(SyntaxToken newKeyword, ArgumentListSyntax* arguments, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ImplicitObjectCreationExpression)
            , newKeyword(newKeyword)
            , arguments(arguments)
            , initializer(initializer) {}

    };


    struct ArrayCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newKeyword;
        ArrayTypeSyntax* type;
        InitializerExpressionSyntax* initializer;

        ArrayCreationExpressionSyntax(SyntaxToken newKeyword, ArrayTypeSyntax* type, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ArrayCreationExpression)
            , newKeyword(newKeyword)
            , type(type)
            , initializer(initializer) {}
    };

    struct AnonymousObjectMemberDeclaratorSyntax : ExpressionSyntax {
        NameEqualsSyntax* nameEquals;
        ExpressionSyntax* expression;

        AnonymousObjectMemberDeclaratorSyntax(NameEqualsSyntax* nameEquals, ExpressionSyntax* expression)
            : ExpressionSyntax(SyntaxKind::AnonymousObjectMemberDeclarator)
            , nameEquals(nameEquals)
            , expression(expression) {}

    };

    struct AnonymousObjectCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newToken;
        SyntaxToken openBrace;
        SeparatedSyntaxList<AnonymousObjectMemberDeclaratorSyntax>* initializers;
        SyntaxToken closeBrace;

        AnonymousObjectCreationExpressionSyntax(SyntaxToken newToken, SyntaxToken openBrace, SeparatedSyntaxList<AnonymousObjectMemberDeclaratorSyntax>* initializers, SyntaxToken closeBrace)
            : ExpressionSyntax(SyntaxKind::AnonymousObjectCreationExpression)
            , newToken(newToken)
            , openBrace(openBrace)
            , initializers(initializers)
            , closeBrace(closeBrace) {}

    };

    struct TupleExpressionSyntax : ExpressionSyntax {
        SyntaxToken openToken;
        SeparatedSyntaxList<ArgumentSyntax>* arguments;
        SyntaxToken closeToken;

        TupleExpressionSyntax(SyntaxToken openToken, SeparatedSyntaxList<ArgumentSyntax>* arguments, SyntaxToken closeToken)
            : ExpressionSyntax(SyntaxKind::TupleExpression)
            , openToken(openToken)
            , arguments(arguments)
            , closeToken(closeToken) {}

    };

    struct ParenthesizedExpressionSyntax : ExpressionSyntax {
        SyntaxToken openToken;
        ExpressionSyntax* expression;
        SyntaxToken closeToken;

        ParenthesizedExpressionSyntax(SyntaxToken openToken, ExpressionSyntax* expression, SyntaxToken closeToken)
            : ExpressionSyntax(SyntaxKind::ParenthesizedExpression)
            , openToken(openToken)
            , expression(expression)
            , closeToken(closeToken) {}

    };


    struct BracketedArgumentListSyntax : SyntaxBase {

        SyntaxToken openBracket;
        SeparatedSyntaxList<ArgumentSyntax>* arguments;
        SyntaxToken closeBracket;

        BracketedArgumentListSyntax(SyntaxToken openBracket, SeparatedSyntaxList<ArgumentSyntax>* arguments, SyntaxToken closeBracket)
            : SyntaxBase(SyntaxKind::BracketedArgumentList)
            , openBracket(openBracket)
            , arguments(arguments)
            , closeBracket(closeBracket) {}

    };

    struct EqualsValueClauseSyntax : SyntaxBase {

        SyntaxToken equalsToken;
        ExpressionSyntax* value;

        EqualsValueClauseSyntax(SyntaxToken equalsToken, ExpressionSyntax* value)
            : SyntaxBase(SyntaxKind::EqualsValueClause)
            , equalsToken(equalsToken)
            , value(value) {}

    };

    struct RefExpressionSyntax : ExpressionSyntax {

        SyntaxToken refKeyword;
        ExpressionSyntax* expression;

        RefExpressionSyntax(SyntaxToken refKeyword, ExpressionSyntax* expression)
            : ExpressionSyntax(SyntaxKind::RefExpression)
            , refKeyword(refKeyword)
            , expression(expression) {}

    };

    struct VariableDeclaratorSyntax : SyntaxBase {

        SyntaxToken identifier;
        BracketedArgumentListSyntax* argumentList;
        EqualsValueClauseSyntax* initializer;

        explicit VariableDeclaratorSyntax(SyntaxToken identifier, BracketedArgumentListSyntax* argumentList, EqualsValueClauseSyntax* initializer)
            : SyntaxBase(SyntaxKind::VariableDeclarator)
            , identifier(identifier)
            , argumentList(argumentList)
            , initializer(initializer) {}

    };

    struct TypeParameterSyntax : SyntaxBase {

        SyntaxToken identifier;

        explicit TypeParameterSyntax(SyntaxToken identifier) : SyntaxBase(SyntaxKind::TypeParameter) {}

    };

    struct TypeParameterListSyntax : SyntaxBase {

        SyntaxToken lessThanToken;
        SeparatedSyntaxList<TypeParameterSyntax>* parameters;
        SyntaxToken greaterThanToken;

        explicit TypeParameterListSyntax(SyntaxToken lessThanToken, SeparatedSyntaxList<TypeParameterSyntax>* parameters, SyntaxToken greaterThanToken)
            : SyntaxBase(SyntaxKind::TypeParameterList)
            , lessThanToken(lessThanToken)
            , parameters(parameters)
            , greaterThanToken(greaterThanToken) {}

    };

    struct ArrowExpressionClauseSyntax : SyntaxBase {
        SyntaxToken arrowToken;
        ExpressionSyntax* expression;

        ArrowExpressionClauseSyntax(SyntaxToken arrowToken, ExpressionSyntax* expression)
            : SyntaxBase(SyntaxKind::ArrowExpressionClause)
            , arrowToken(arrowToken)
            , expression(expression) {}

    };

    struct BlockSyntax : StatementSyntax {

        SyntaxToken openBraceToken;
        SyntaxList<StatementSyntax>* statements;
        SyntaxToken closeBraceToken;

        BlockSyntax(SyntaxToken openBraceToken, SyntaxList<StatementSyntax>* statements, SyntaxToken closeBraceToken)
            : StatementSyntax(SyntaxKind::Block)
            , openBraceToken(openBraceToken)
            , statements(statements)
            , closeBraceToken(closeBraceToken) {}

    };

    struct LiteralExpressionSyntax : ExpressionSyntax {

        SyntaxToken literal;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::CharacterLiteralExpression,
            SyntaxKind::DefaultLiteralExpression,
            SyntaxKind::FalseLiteralExpression,
            SyntaxKind::NullLiteralExpression,
            SyntaxKind::NumericLiteralExpression,
            SyntaxKind::StringLiteralExpression,
            SyntaxKind::TrueLiteralExpression,
        };

        LiteralExpressionSyntax(SyntaxKind kind, SyntaxToken literal)
            : ExpressionSyntax(kind)
            , literal(literal) {

            ASSERT_VALID_SYNTAX_KIND(kind);

        }

    };

    struct CastExpressionSyntax : ExpressionSyntax {
        SyntaxToken openParen;
        TypeSyntax* type;
        SyntaxToken closeParen;
        ExpressionSyntax* expression;

        CastExpressionSyntax(SyntaxToken openParen, TypeSyntax* type, SyntaxToken closeParen, ExpressionSyntax* expression)
            : ExpressionSyntax(SyntaxKind::CastExpression)
            , openParen(openParen)
            , type(type)
            , closeParen(closeParen)
            , expression(expression) {}

    };

    struct BaseExpressionSyntax : ExpressionSyntax {

        SyntaxToken keyword;

        explicit BaseExpressionSyntax(SyntaxToken keyword)
            : ExpressionSyntax(SyntaxKind::BaseExpression)
            , keyword(keyword) {}

    };

    struct ThisExpressionSyntax : ExpressionSyntax {

        SyntaxToken keyword;

        explicit ThisExpressionSyntax(SyntaxToken keyword)
            : ExpressionSyntax(SyntaxKind::ThisExpression)
            , keyword(keyword) {}

    };

    struct DefaultExpressionSyntax : ExpressionSyntax {
        SyntaxToken keyword;
        SyntaxToken openParenToken;
        TypeSyntax* type;
        SyntaxToken closeParenToken;

        DefaultExpressionSyntax(SyntaxToken keyword, SyntaxToken openParenToken, TypeSyntax* type, SyntaxToken closeParenToken)
            : ExpressionSyntax(SyntaxKind::DefaultExpression)
            , keyword(keyword)
            , openParenToken(openParenToken)
            , type(type)
            , closeParenToken(closeParenToken) {}

    };

    struct SizeOfExpressionSyntax : ExpressionSyntax {
        SyntaxToken keyword;
        SyntaxToken openParenToken;
        TypeSyntax* type;
        SyntaxToken closeParenToken;

        SizeOfExpressionSyntax(SyntaxToken keyword, SyntaxToken openParenToken, TypeSyntax* type, SyntaxToken closeParenToken)
            : ExpressionSyntax(SyntaxKind::SizeOfExpression)
            , keyword(keyword)
            , openParenToken(openParenToken)
            , type(type)
            , closeParenToken(closeParenToken) {}

    };

    struct TypeOfExpressionSyntax : ExpressionSyntax {
        SyntaxToken keyword;
        SyntaxToken openParenToken;
        TypeSyntax* type;
        SyntaxToken closeParenToken;

        TypeOfExpressionSyntax(SyntaxToken keyword, SyntaxToken openParenToken, TypeSyntax* type, SyntaxToken closeParenToken)
            : ExpressionSyntax(SyntaxKind::TypeOfExpression)
            , keyword(keyword)
            , openParenToken(openParenToken)
            , type(type)
            , closeParenToken(closeParenToken) {}

    };

    struct DiscardDesignationSyntax : VariableDesignationSyntax {
        SyntaxToken underscoreToken;

        explicit DiscardDesignationSyntax(SyntaxToken underscoreToken)
            : VariableDesignationSyntax(SyntaxKind::DiscardDesignation)
            , underscoreToken(underscoreToken) {}

    };

    struct SingleVariableDesignationSyntax : VariableDesignationSyntax {
        SyntaxToken identifier;

        explicit SingleVariableDesignationSyntax(SyntaxToken identifier)
            : VariableDesignationSyntax(SyntaxKind::SingleVariableDesignation)
            , identifier(identifier) {}

    };

    struct ParenthesizedVariableDesignationSyntax : VariableDesignationSyntax {

        SyntaxToken openParen;
        SeparatedSyntaxList<VariableDesignationSyntax>* designators;
        SyntaxToken closeParen;

        ParenthesizedVariableDesignationSyntax(SyntaxToken openParen, SeparatedSyntaxList<VariableDesignationSyntax>* designators, SyntaxToken closeParen)
            : VariableDesignationSyntax(SyntaxKind::ParenthesizedVariableDesignation)
            , openParen(openParen)
            , designators(designators)
            , closeParen(closeParen) {}

    };

    struct DeclarationExpressionSyntax : ExpressionSyntax {
        TypeSyntax* type;
        VariableDesignationSyntax* designation;

        DeclarationExpressionSyntax(TypeSyntax* type, VariableDesignationSyntax* designation)
            : ExpressionSyntax(SyntaxKind::DeclarationExpression)
            , type(type)
            , designation(designation) {}
    };

    struct ThrowExpressionSyntax : ExpressionSyntax {

        SyntaxToken throwKeyword;
        ExpressionSyntax* expression;

        ThrowExpressionSyntax(SyntaxToken throwKeyword, ExpressionSyntax* expression)
            : ExpressionSyntax(SyntaxKind::ThrowExpression)
            , throwKeyword(throwKeyword)
            , expression(expression) {}

    };

    struct PostfixUnaryExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        SyntaxToken operatorToken;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::PostIncrementExpression,
            SyntaxKind::PostDecrementExpression,
        };

        PostfixUnaryExpressionSyntax(SyntaxKind kind, ExpressionSyntax* expression, SyntaxToken operatorToken)
            : ExpressionSyntax(kind)
            , expression(expression)
            , operatorToken(operatorToken) {

            ASSERT_VALID_SYNTAX_KIND(kind);
        }

    };

    struct ElementAccessExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        BracketedArgumentListSyntax* argumentList;

        ElementAccessExpressionSyntax(ExpressionSyntax* expression, BracketedArgumentListSyntax* argumentList)
            : ExpressionSyntax(SyntaxKind::ElementAccessExpression)
            , expression(expression)
            , argumentList(argumentList) {}
    };

    struct InvocationExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        ArgumentListSyntax* argumentList;

        InvocationExpressionSyntax(ExpressionSyntax* expression, ArgumentListSyntax* argumentList)
            : ExpressionSyntax(SyntaxKind::InvocationExpression)
            , expression(expression)
            , argumentList(argumentList) {}

    };

    struct ConditionalExpressionSyntax : ExpressionSyntax {
        ExpressionSyntax* condition;
        SyntaxToken questionToken;
        ExpressionSyntax* whenTrue;
        SyntaxToken colonToken;
        ExpressionSyntax* whenFalse;

        ConditionalExpressionSyntax(ExpressionSyntax* condition, SyntaxToken questionToken, ExpressionSyntax* whenTrue, SyntaxToken colonToken, ExpressionSyntax* whenFalse)
            : ExpressionSyntax(SyntaxKind::ConditionalExpression)
            , condition(condition)
            , questionToken(questionToken)
            , whenTrue(whenTrue)
            , colonToken(colonToken)
            , whenFalse(whenFalse) {}

    };

    struct RangeExpressionSyntax : ExpressionSyntax {
        ExpressionSyntax* leftOperand;
        SyntaxToken operatorToken;
        ExpressionSyntax* rightOperand;

        RangeExpressionSyntax(ExpressionSyntax* leftOperand, SyntaxToken operatorToken, ExpressionSyntax* rightOperand)
            : ExpressionSyntax(SyntaxKind::RangeExpression)
            , leftOperand(leftOperand)
            , operatorToken(operatorToken)
            , rightOperand(rightOperand) {}

    };

    struct PrefixUnaryExpressionSyntax : ExpressionSyntax {

        SyntaxToken operatorToken;
        ExpressionSyntax* operand;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::UnaryPlusExpression,
            SyntaxKind::UnaryMinusExpression,
            SyntaxKind::BitwiseNotExpression,
            SyntaxKind::LogicalNotExpression,
            SyntaxKind::PreIncrementExpression,
            SyntaxKind::PreDecrementExpression,
            SyntaxKind::AddressOfExpression,
            SyntaxKind::PointerIndirectionExpression,
            SyntaxKind::IndexExpression,
        };

        PrefixUnaryExpressionSyntax(SyntaxKind opKind, SyntaxToken operatorToken, ExpressionSyntax* operand)
            : ExpressionSyntax(opKind)
            , operatorToken(operatorToken)
            , operand(operand) {

            ASSERT_VALID_SYNTAX_KIND(opKind);

        }

    };

    struct ParameterSyntax : SyntaxBase {

        TokenList* modifiers;
        TypeSyntax* type;
        SyntaxToken identifier;
        EqualsValueClauseSyntax* defaultValue;

        ParameterSyntax(TokenList* modifiers, TypeSyntax* type, SyntaxToken identifier, EqualsValueClauseSyntax* defaultValue)
            : SyntaxBase(SyntaxKind::Parameter)
            , modifiers(modifiers)
            , type(type)
            , identifier(identifier)
            , defaultValue(defaultValue) {}

    };

    struct BaseConstructorInitializerSyntax : ConstructorInitializerSyntax {
        SyntaxToken colonToken;
        SyntaxToken baseKeyword;
        ArgumentListSyntax* argumentListSyntax;

        BaseConstructorInitializerSyntax(SyntaxToken colonToken, SyntaxToken baseKeyword, ArgumentListSyntax* argumentListSyntax)
            : ConstructorInitializerSyntax(SyntaxKind::BaseConstructorInitializer)
            , colonToken(colonToken)
            , baseKeyword(baseKeyword)
            , argumentListSyntax(argumentListSyntax) {}

    };

    struct ThisConstructorInitializerSyntax : ConstructorInitializerSyntax {

        SyntaxToken colonToken;
        SyntaxToken thisKeyword;
        ArgumentListSyntax* argumentListSyntax;

        ThisConstructorInitializerSyntax(SyntaxToken colonToken, SyntaxToken thisKeyword, ArgumentListSyntax* argumentListSyntax)
            : ConstructorInitializerSyntax(SyntaxKind::ThisConstructorInitializer)
            , colonToken(colonToken)
            , thisKeyword(thisKeyword)
            , argumentListSyntax(argumentListSyntax) {}

    };


    struct NamedConstructorInitializerSyntax : ConstructorInitializerSyntax {

        SyntaxToken colonToken;
        SyntaxToken name;
        ArgumentListSyntax* argumentListSyntax;

        NamedConstructorInitializerSyntax(SyntaxToken colonToken, SyntaxToken name, ArgumentListSyntax* argumentListSyntax)
            : ConstructorInitializerSyntax(SyntaxKind::NamedConstructorInitializer)
            , colonToken(colonToken)
            , name(name)
            , argumentListSyntax(argumentListSyntax) {}

    };

    struct ParameterListSyntax : SyntaxBase {

        SyntaxToken openParen;
        SeparatedSyntaxList<ParameterSyntax>* parameters;
        SyntaxToken closeParen;

        explicit ParameterListSyntax()
            : SyntaxBase(SyntaxKind::ParameterList) {}

    };

    struct ConstraintClausesSyntax : SyntaxBase {
        SyntaxToken dummy;
    };

    struct LocalFunctionStatementSyntax : SyntaxBase {

        TokenList* modifiers;
        TypeSyntax* returnType;
        SyntaxToken identifier;
        TypeParameterListSyntax* typeParameters;
        ParameterListSyntax* parameters;
        ConstraintClausesSyntax* constraints;
        SyntaxBase* body; // todo -- change to a real type

        explicit LocalFunctionStatementSyntax(TokenList* modifiers, TypeSyntax* returnType, SyntaxToken identifier, TypeParameterListSyntax* typeParameters, ParameterListSyntax* parameters, ConstraintClausesSyntax* constraints, SyntaxBase* body)
            : SyntaxBase(SyntaxKind::LocalFunctionStatement)
            , modifiers(modifiers)
            , returnType(returnType)
            , identifier(identifier)
            , typeParameters(typeParameters)
            , parameters(parameters)
            , constraints(constraints)
            , body(body) {}

    };


    struct VariableDeclarationSyntax : SyntaxBase {

        TypeSyntax* type;
        SeparatedSyntaxList<VariableDeclaratorSyntax>* variables;

        explicit VariableDeclarationSyntax(TypeSyntax* type, SeparatedSyntaxList<VariableDeclaratorSyntax>* variables)
            : SyntaxBase(SyntaxKind::VariableDeclaration)
            , type(type)
            , variables(variables) {}

    };

    struct FieldDeclarationSyntax : MemberDeclarationSyntax {
        TokenList* modifiers;
        VariableDeclarationSyntax* declaration;
        SyntaxToken semicolonToken;

        FieldDeclarationSyntax(TokenList* modifiers, VariableDeclarationSyntax* declaration, SyntaxToken semicolonToken)
            : MemberDeclarationSyntax(SyntaxKind::FieldDeclaration)
            , modifiers(modifiers)
            , declaration(declaration)
            , semicolonToken(semicolonToken) {}

    };

    struct ExpressionColonSyntax : BaseExpressionColonSyntax {
        ExpressionSyntax* expression;
        SyntaxToken colonToken;

        ExpressionColonSyntax(ExpressionSyntax* expression, SyntaxToken colonToken)
            : BaseExpressionColonSyntax(SyntaxKind::ExpressionColon)
            , expression(expression)
            , colonToken(colonToken) {}
    };

    struct SubpatternSyntax : PatternSyntax {

        BaseExpressionColonSyntax* expressionColon;
        PatternSyntax* pattern;

        SubpatternSyntax(BaseExpressionColonSyntax* expressionColon, PatternSyntax* pattern)
            : PatternSyntax(SyntaxKind::Subpattern)
            , expressionColon(expressionColon)
            , pattern(pattern) {}

    };

    struct PropertyPatternClauseSyntax : SyntaxBase {

        SyntaxToken openBraceToken;
        SeparatedSyntaxList<SubpatternSyntax>* subpatterns;
        SyntaxToken closeBraceToken;

        PropertyPatternClauseSyntax(SyntaxToken openBraceToken, SeparatedSyntaxList<SubpatternSyntax>* subpatterns, SyntaxToken closeBraceToken)
            : SyntaxBase(SyntaxKind::PropertyPatternClause)
            , openBraceToken(openBraceToken)
            , subpatterns(subpatterns)
            , closeBraceToken(closeBraceToken) {}

    };

    struct DeclarationPatternSyntax : PatternSyntax {
        TypeSyntax* type;
        VariableDesignationSyntax* designation;

        DeclarationPatternSyntax(TypeSyntax* type, VariableDesignationSyntax* designation)
            : PatternSyntax(SyntaxKind::DeclarationPattern)
            , type(type)
            , designation(designation) {}

    };

    struct PositionalPatternClauseSyntax : PatternSyntax {
        SyntaxToken openParenToken;
        SeparatedSyntaxList<SubpatternSyntax>* subpatterns;
        SyntaxToken closeParenToken;

        PositionalPatternClauseSyntax(SyntaxToken openParenToken, SeparatedSyntaxList<SubpatternSyntax>* subpatterns, SyntaxToken closeParenToken)
            : PatternSyntax(SyntaxKind::PositionalPatternClause)
            , openParenToken(openParenToken)
            , subpatterns(subpatterns)
            , closeParenToken(closeParenToken) {}

    };

    struct RecursivePatternSyntax : PatternSyntax {
        TypeSyntax* type;
        PositionalPatternClauseSyntax* positionalPatternClause;
        PropertyPatternClauseSyntax* propertyPatternClause;
        VariableDesignationSyntax* designation;

        RecursivePatternSyntax(TypeSyntax* type, PositionalPatternClauseSyntax* positionalPatternClause, PropertyPatternClauseSyntax* propertyPatternClause, VariableDesignationSyntax* designation)
            : PatternSyntax(SyntaxKind::RecursivePattern)
            , type(type)
            , positionalPatternClause(positionalPatternClause)
            , propertyPatternClause(propertyPatternClause)
            , designation(designation) {}
    };

    struct ParenthesizedPatternSyntax : PatternSyntax {
        SyntaxToken openParenToken;
        PatternSyntax* pattern;
        SyntaxToken closeParenToken;

        ParenthesizedPatternSyntax(SyntaxToken openParenToken, PatternSyntax* pattern, SyntaxToken closeParenToken)
            : PatternSyntax(SyntaxKind::ParenthesizedPattern)
            , openParenToken(openParenToken)
            , pattern(pattern)
            , closeParenToken(closeParenToken) {}
    };

    struct VarPatternSyntax : PatternSyntax {
        SyntaxToken varKeyword;
        VariableDesignationSyntax* designation;

        VarPatternSyntax(SyntaxToken varKeyword, VariableDesignationSyntax* designation)
            : PatternSyntax(SyntaxKind::VarPattern)
            , varKeyword(varKeyword)
            , designation(designation) {}

    };

    struct TypePatternSyntax : PatternSyntax {

        TypeSyntax* type;

        TypePatternSyntax(TypeSyntax* type)
            : PatternSyntax(SyntaxKind::TypePattern)
            , type(type) {}

    };

    struct ConstantPatternSyntax : PatternSyntax {

        ExpressionSyntax* expression;

        explicit ConstantPatternSyntax(ExpressionSyntax* expression)
            : PatternSyntax(SyntaxKind::ConstantPattern)
            , expression(expression) {}

    };

    struct RelationalPatternSyntax : PatternSyntax {
        SyntaxToken operatorToken;
        ExpressionSyntax* expression;

        RelationalPatternSyntax(SyntaxToken operatorToken, ExpressionSyntax* expression)
            : PatternSyntax(SyntaxKind::RelationalPattern)
            , operatorToken(operatorToken)
            , expression(expression) {}

    };

    struct SlicePatternSyntax : PatternSyntax {
        SyntaxToken dotDotToken;
        PatternSyntax* pattern;

        SlicePatternSyntax(SyntaxToken dotDotToken, PatternSyntax* pattern)
            : PatternSyntax(SyntaxKind::SlicePattern)
            , dotDotToken(dotDotToken)
            , pattern(pattern) {}

    };

    struct DiscardPatternSyntax : PatternSyntax {

        SyntaxToken underscore;

        explicit DiscardPatternSyntax(SyntaxToken underscore)
            : PatternSyntax(SyntaxKind::DiscardPattern)
            , underscore(underscore) {}

    };

    struct UnaryPatternSyntax : PatternSyntax {
        SyntaxToken operatorToken;
        PatternSyntax* pattern;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::NotPattern
        };

        UnaryPatternSyntax(SyntaxKind kind, SyntaxToken operatorToken, PatternSyntax* pattern)
            : PatternSyntax(kind)
            , operatorToken(operatorToken)
            , pattern(pattern) {
            ASSERT_VALID_SYNTAX_KIND(kind);
        }

    };

    struct BinaryPatternSyntax : PatternSyntax {
        PatternSyntax* left;
        SyntaxToken operatorToken;
        PatternSyntax* right;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::OrPattern,
            SyntaxKind::AndPattern,
        };

        BinaryPatternSyntax(SyntaxKind kind, PatternSyntax* left, SyntaxToken operatorToken, PatternSyntax* right)
            : PatternSyntax(kind)
            , left(left)
            , operatorToken(operatorToken)
            , right(right) {
            ASSERT_VALID_SYNTAX_KIND(kind);
        }

    };

    struct IsPatternExpressionSyntax : ExpressionSyntax {
        ExpressionSyntax* leftOperand;
        SyntaxToken opToken;
        PatternSyntax* pattern;

        IsPatternExpressionSyntax(ExpressionSyntax* leftOperand, SyntaxToken opToken, PatternSyntax* pattern)
            : ExpressionSyntax(SyntaxKind::IsPatternExpression)
            , leftOperand(leftOperand)
            , opToken(opToken)
            , pattern(pattern) {}

    };

    struct BinaryExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* left;
        SyntaxToken operatorToken;
        ExpressionSyntax* right;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::AddExpression,
            SyntaxKind::SubtractExpression,
            SyntaxKind::MultiplyExpression,
            SyntaxKind::DivideExpression,
            SyntaxKind::ModuloExpression,
            SyntaxKind::LeftShiftExpression,
            SyntaxKind::RightShiftExpression,
            SyntaxKind::UnsignedRightShiftExpression,
            SyntaxKind::LogicalOrExpression,
            SyntaxKind::LogicalAndExpression,
            SyntaxKind::BitwiseOrExpression,
            SyntaxKind::BitwiseAndExpression,
            SyntaxKind::ExclusiveOrExpression,
            SyntaxKind::EqualsExpression,
            SyntaxKind::NotEqualsExpression,
            SyntaxKind::LessThanExpression,
            SyntaxKind::LessThanOrEqualExpression,
            SyntaxKind::GreaterThanExpression,
            SyntaxKind::GreaterThanOrEqualExpression,
            SyntaxKind::IsExpression,
            SyntaxKind::AsExpression,
            SyntaxKind::CoalesceExpression,
        };

        BinaryExpressionSyntax(SyntaxKind kind, ExpressionSyntax* left, SyntaxToken operatorToken, ExpressionSyntax* right)
            : ExpressionSyntax(kind)
            , left(left)
            , operatorToken(operatorToken)
            , right(right) {

            ASSERT_VALID_SYNTAX_KIND(kind);

        }


    };

    struct AssignmentExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* left;
        SyntaxToken operatorToken;
        ExpressionSyntax* right;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::SimpleAssignmentExpression,
            SyntaxKind::AddAssignmentExpression,
            SyntaxKind::SubtractAssignmentExpression,
            SyntaxKind::MultiplyAssignmentExpression,
            SyntaxKind::DivideAssignmentExpression,
            SyntaxKind::ModuloAssignmentExpression,
            SyntaxKind::AndAssignmentExpression,
            SyntaxKind::ExclusiveOrAssignmentExpression,
            SyntaxKind::OrAssignmentExpression,
            SyntaxKind::LeftShiftAssignmentExpression,
            SyntaxKind::RightShiftAssignmentExpression,
            SyntaxKind::UnsignedRightShiftAssignmentExpression,
            SyntaxKind::CoalesceAssignmentExpression,
        };

        AssignmentExpressionSyntax(SyntaxKind opKind, ExpressionSyntax* left, SyntaxToken operatorToken, ExpressionSyntax* right)
            : ExpressionSyntax(opKind)
            , left(left)
            , operatorToken(operatorToken)
            , right(right) {

            ASSERT_VALID_SYNTAX_KIND(opKind);

        }

    };

}

#undef abstract
