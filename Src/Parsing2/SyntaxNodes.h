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

        SyntaxToken nameColon;
        SyntaxToken refKindKeyword;
        ExpressionSyntax* expression;

        ArgumentSyntax(SyntaxToken nameColon, SyntaxToken refKindKeyword, ExpressionSyntax* expression)
            : SyntaxBase(SyntaxKind::Argument)
            , nameColon(nameColon)
            , refKindKeyword(refKindKeyword)
            , expression(expression) {}

    };

    struct ArgumentListSyntax : SyntaxBase {
        SyntaxToken openParenToken;
        SeparatedSyntaxList<ArgumentSyntax>* arguments;
        SyntaxToken closeParenToken;

        ArgumentListSyntax(SyntaxToken openParenToken, SeparatedSyntaxList<ArgumentSyntax>* arguments, SyntaxToken closeParenToken)
            : SyntaxBase(SyntaxKind::ArgumentList)
            , openParenToken(openParenToken)
            , arguments(arguments)
            , closeParenToken(closeParenToken) {}

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

    struct ConstructorInitializerSyntax : SyntaxBase {
        SyntaxToken colonToken;
        SyntaxToken thisOrBaseOrCtorNameToken;
        ArgumentListSyntax* argumentListSyntax;

        ConstructorInitializerSyntax(SyntaxToken colonToken, SyntaxToken thisOrBaseOrCtorNameToken, ArgumentListSyntax* argumentListSyntax)
            : SyntaxBase(SyntaxKind::ConstructorInitializer)
            , colonToken(colonToken)
            , thisOrBaseOrCtorNameToken(thisOrBaseOrCtorNameToken)
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


}

#undef abstract
