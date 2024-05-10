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


}

#undef abstract
