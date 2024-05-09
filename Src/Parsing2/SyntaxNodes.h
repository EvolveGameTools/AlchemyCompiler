#pragma once

// marker for re-ordering fields
#define NodeIndex(x)
#define abstract

namespace Alchemy::Compilation {

    abstract struct SyntaxBase {

        SyntaxKind kind;
        SyntaxTokenFlags flags;

        explicit SyntaxBase(SyntaxKind kind) : kind(kind), flags(SyntaxTokenFlags::None) {}

        inline bool IsMissing() {
            return false; // (flags & SyntaxTokenFlags::IsNotMissing) == 0;
        }

        #if true // shows better debugger output if polymorphic

        virtual void dummy() {}

        #endif

    };

    abstract struct SyntaxListUntyped : SyntaxBase {
        // DO NOT CHANGE THIS LAYOUT WITHOUT MIRRORING IN SyntaxList<T>
        int32 size;
        SyntaxBase** array;

        SyntaxListUntyped(SyntaxBase ** array, int32 size) : SyntaxBase(SyntaxKind::ListKind), array(array), size(size) {}

    };

    template<typename T>
    abstract
    struct SyntaxList : SyntaxBase {
        // DO NOT CHANGE THIS LAYOUT WITHOUT MIRRORING IN SyntaxListUntyped
        int32 size;
        T** array;
        SyntaxList(T ** array, int32 size) : SyntaxBase(SyntaxKind::ListKind), array(array), size(size) {}
    };

    abstract struct SeparatedSyntaxListUntyped : SyntaxBase {
        // DO NOT CHANGE THIS LAYOUT WITHOUT MIRRORING IN SeparatedSyntaxList<T>
        int32 itemCount {};
        int32 separatorCount {};
        SyntaxBase** items {};
        SyntaxToken* separators {};
    };

    template<typename T>
    abstract
    struct SeparatedSyntaxList : SyntaxBase {
        // DO NOT CHANGE THIS LAYOUT WITHOUT MIRRORING IN SeparatedSyntaxListUntyped
        int32 itemCount;
        int32 separatorCount;
        T** items;
        SyntaxToken* separators;

        SeparatedSyntaxList(int32 itemCount, T** items, int32 separatorCount, SyntaxToken* separators)
            : SyntaxBase(SyntaxKind::ListKind)
            , itemCount(itemCount)
            , items(items)
            , separatorCount(separatorCount)
            , separators(separators) {}

        SeparatedSyntaxListUntyped* ToUntypedList() {
            return (SeparatedSyntaxListUntyped*) this;
        }

    };

    abstract struct ExpressionSyntax : SyntaxBase {

        explicit ExpressionSyntax(SyntaxKind kind) : SyntaxBase(kind) {}

    };

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

    abstract struct TypeSyntax : ExpressionSyntax {

        explicit TypeSyntax(SyntaxKind kind) : ExpressionSyntax(kind) {}

    };

    abstract struct NameSyntax : TypeSyntax {

        explicit NameSyntax(SyntaxKind kind) : TypeSyntax(kind) {}

    };

    abstract struct SimpleNameSyntax : NameSyntax {

        explicit SimpleNameSyntax(SyntaxKind kind) : NameSyntax(kind) {}

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

#undef NodeIndex
#undef abstract
