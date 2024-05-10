#include "./Builders.h"

namespace Alchemy::Compilation {

    struct ArrayRankSpecifierSyntaxBuilder: ExpressionSyntaxBuilder {

        SyntaxToken open {};
        SeparatedSyntaxListBuilder* ranks {};
        SyntaxToken close {};

        inline ArrayRankSpecifierSyntaxBuilder* Open(SyntaxToken builder) {
            open = builder;
            return this;
        }

        inline ArrayRankSpecifierSyntaxBuilder* Ranks(SeparatedSyntaxListBuilder* builder) {
            ranks = builder;
            return this;
        }

        inline ArrayRankSpecifierSyntaxBuilder* Close(SyntaxToken builder) {
            close = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            ArrayRankSpecifierSyntax* retn = allocator->Allocate<ArrayRankSpecifierSyntax>(1);
            retn->_kind = SyntaxKind::ArrayRankSpecifier;
            retn->open = open;
            retn->ranks = ranks != nullptr ? (SeparatedSyntaxList<ExpressionSyntax>*)ranks->Build() : nullptr;
            retn->close = close;
            return retn;
        }

    };

    struct TypeArgumentListSyntaxBuilder: SyntaxBaseBuilder {

        SyntaxToken lessThanToken {};
        SeparatedSyntaxListBuilder* arguments {};
        SyntaxToken greaterThanToken {};

        inline TypeArgumentListSyntaxBuilder* LessThanToken(SyntaxToken builder) {
            lessThanToken = builder;
            return this;
        }

        inline TypeArgumentListSyntaxBuilder* Arguments(SeparatedSyntaxListBuilder* builder) {
            arguments = builder;
            return this;
        }

        inline TypeArgumentListSyntaxBuilder* GreaterThanToken(SyntaxToken builder) {
            greaterThanToken = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            TypeArgumentListSyntax* retn = allocator->Allocate<TypeArgumentListSyntax>(1);
            retn->_kind = SyntaxKind::TypeArgumentList;
            retn->lessThanToken = lessThanToken;
            retn->arguments = arguments != nullptr ? (SeparatedSyntaxList<TypeSyntax>*)arguments->Build() : nullptr;
            retn->greaterThanToken = greaterThanToken;
            return retn;
        }

    };

    struct GenericNameSyntaxBuilder: SimpleNameSyntaxBuilder {

        SyntaxToken identifier {};
        TypeArgumentListSyntaxBuilder * typeArgumentList {};

        inline GenericNameSyntaxBuilder* Identifier(SyntaxToken builder) {
            identifier = builder;
            return this;
        }

        inline GenericNameSyntaxBuilder* TypeArgumentList(TypeArgumentListSyntaxBuilder * builder) {
            typeArgumentList = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            GenericNameSyntax* retn = allocator->Allocate<GenericNameSyntax>(1);
            retn->_kind = SyntaxKind::GenericName;
            retn->identifier = identifier;
            retn->typeArgumentList = typeArgumentList != nullptr ? (TypeArgumentListSyntax*)typeArgumentList->Build() : nullptr;
            return retn;
        }

    };

    struct QualifiedNameSyntaxBuilder: NameSyntaxBuilder {

        NameSyntaxBuilder * left {};
        SyntaxToken dotToken {};
        SimpleNameSyntaxBuilder * right {};

        inline QualifiedNameSyntaxBuilder* Left(NameSyntaxBuilder * builder) {
            left = builder;
            return this;
        }

        inline QualifiedNameSyntaxBuilder* DotToken(SyntaxToken builder) {
            dotToken = builder;
            return this;
        }

        inline QualifiedNameSyntaxBuilder* Right(SimpleNameSyntaxBuilder * builder) {
            right = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            QualifiedNameSyntax* retn = allocator->Allocate<QualifiedNameSyntax>(1);
            retn->_kind = SyntaxKind::QualifiedName;
            retn->left = left != nullptr ? (NameSyntax*)left->Build() : nullptr;
            retn->dotToken = dotToken;
            retn->right = right != nullptr ? (SimpleNameSyntax*)right->Build() : nullptr;
            return retn;
        }

    };

    struct IdentifierNameSyntaxBuilder: SimpleNameSyntaxBuilder {

        SyntaxToken identifier {};

        inline IdentifierNameSyntaxBuilder* Identifier(SyntaxToken builder) {
            identifier = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            IdentifierNameSyntax* retn = allocator->Allocate<IdentifierNameSyntax>(1);
            retn->_kind = SyntaxKind::IdentifierName;
            retn->identifier = identifier;
            return retn;
        }

    };

    struct PredefinedTypeSyntaxBuilder: TypeSyntaxBuilder {

        SyntaxToken typeToken {};

        inline PredefinedTypeSyntaxBuilder* TypeToken(SyntaxToken builder) {
            typeToken = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            PredefinedTypeSyntax* retn = allocator->Allocate<PredefinedTypeSyntax>(1);
            retn->_kind = SyntaxKind::PredefinedType;
            retn->typeToken = typeToken;
            return retn;
        }

    };

    struct TupleElementSyntaxBuilder: SyntaxBaseBuilder {

        TypeSyntaxBuilder * type {};
        SyntaxToken identifier {};

        inline TupleElementSyntaxBuilder* Type(TypeSyntaxBuilder * builder) {
            type = builder;
            return this;
        }

        inline TupleElementSyntaxBuilder* Identifier(SyntaxToken builder) {
            identifier = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            TupleElementSyntax* retn = allocator->Allocate<TupleElementSyntax>(1);
            retn->_kind = SyntaxKind::TupleElement;
            retn->type = type != nullptr ? (TypeSyntax*)type->Build() : nullptr;
            retn->identifier = identifier;
            return retn;
        }

    };

    struct TupleTypeSyntaxBuilder: TypeSyntaxBuilder {

        SyntaxToken openParenToken {};
        SeparatedSyntaxListBuilder* elements {};
        SyntaxToken closeParenToken {};

        inline TupleTypeSyntaxBuilder* OpenParenToken(SyntaxToken builder) {
            openParenToken = builder;
            return this;
        }

        inline TupleTypeSyntaxBuilder* Elements(SeparatedSyntaxListBuilder* builder) {
            elements = builder;
            return this;
        }

        inline TupleTypeSyntaxBuilder* CloseParenToken(SyntaxToken builder) {
            closeParenToken = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            TupleTypeSyntax* retn = allocator->Allocate<TupleTypeSyntax>(1);
            retn->_kind = SyntaxKind::TupleType;
            retn->openParenToken = openParenToken;
            retn->elements = elements != nullptr ? (SeparatedSyntaxList<TupleElementSyntax>*)elements->Build() : nullptr;
            retn->closeParenToken = closeParenToken;
            return retn;
        }

    };

    struct ArrayTypeSyntaxBuilder: TypeSyntaxBuilder {

        TypeSyntaxBuilder * elementType {};
        SyntaxListBuilder* ranks {};

        inline ArrayTypeSyntaxBuilder* ElementType(TypeSyntaxBuilder * builder) {
            elementType = builder;
            return this;
        }

        inline ArrayTypeSyntaxBuilder* Ranks(SyntaxListBuilder* builder) {
            ranks = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            ArrayTypeSyntax* retn = allocator->Allocate<ArrayTypeSyntax>(1);
            retn->_kind = SyntaxKind::ArrayType;
            retn->elementType = elementType != nullptr ? (TypeSyntax*)elementType->Build() : nullptr;
            retn->ranks = ranks != nullptr ? (SyntaxList<ArrayRankSpecifierSyntax>*)ranks->Build() : nullptr;
            return retn;
        }

    };

    struct RefTypeSyntaxBuilder: TypeSyntaxBuilder {

        SyntaxToken refKeyword {};
        SyntaxToken readonlyKeyword {};
        TypeSyntaxBuilder * type {};

        inline RefTypeSyntaxBuilder* RefKeyword(SyntaxToken builder) {
            refKeyword = builder;
            return this;
        }

        inline RefTypeSyntaxBuilder* ReadonlyKeyword(SyntaxToken builder) {
            readonlyKeyword = builder;
            return this;
        }

        inline RefTypeSyntaxBuilder* Type(TypeSyntaxBuilder * builder) {
            type = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            RefTypeSyntax* retn = allocator->Allocate<RefTypeSyntax>(1);
            retn->_kind = SyntaxKind::RefType;
            retn->refKeyword = refKeyword;
            retn->readonlyKeyword = readonlyKeyword;
            retn->type = type != nullptr ? (TypeSyntax*)type->Build() : nullptr;
            return retn;
        }

    };

    struct OmittedArraySizeExpressionSyntaxBuilder: ExpressionSyntaxBuilder {

        SyntaxToken token {};

        inline OmittedArraySizeExpressionSyntaxBuilder* Token(SyntaxToken builder) {
            token = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            OmittedArraySizeExpressionSyntax* retn = allocator->Allocate<OmittedArraySizeExpressionSyntax>(1);
            retn->_kind = SyntaxKind::OmittedArraySizeExpression;
            retn->token = token;
            return retn;
        }

    };

    struct NullableTypeBuilder: TypeSyntaxBuilder {

        TypeSyntaxBuilder * elementType {};
        SyntaxToken questionMark {};

        inline NullableTypeBuilder* ElementType(TypeSyntaxBuilder * builder) {
            elementType = builder;
            return this;
        }

        inline NullableTypeBuilder* QuestionMark(SyntaxToken builder) {
            questionMark = builder;
            return this;
        }

        inline SyntaxBase* Build() override {
            NullableType* retn = allocator->Allocate<NullableType>(1);
            retn->_kind = SyntaxKind::NullableType;
            retn->elementType = elementType != nullptr ? (TypeSyntax*)elementType->Build() : nullptr;
            retn->questionMark = questionMark;
            return retn;
        }

    };

    struct Builder : BuilderBase {
    
        explicit Builder(LinearAllocator * allocator) : BuilderBase((allocator)) {}

        inline ArrayRankSpecifierSyntaxBuilder* ArrayRankSpecifierSyntax() { 
            ArrayRankSpecifierSyntaxBuilder * retn = allocator->Allocate<ArrayRankSpecifierSyntaxBuilder>(1);
            new (retn) ArrayRankSpecifierSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline TypeArgumentListSyntaxBuilder* TypeArgumentListSyntax() { 
            TypeArgumentListSyntaxBuilder * retn = allocator->Allocate<TypeArgumentListSyntaxBuilder>(1);
            new (retn) TypeArgumentListSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline GenericNameSyntaxBuilder* GenericNameSyntax() { 
            GenericNameSyntaxBuilder * retn = allocator->Allocate<GenericNameSyntaxBuilder>(1);
            new (retn) GenericNameSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline QualifiedNameSyntaxBuilder* QualifiedNameSyntax() { 
            QualifiedNameSyntaxBuilder * retn = allocator->Allocate<QualifiedNameSyntaxBuilder>(1);
            new (retn) QualifiedNameSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline IdentifierNameSyntaxBuilder* IdentifierNameSyntax() { 
            IdentifierNameSyntaxBuilder * retn = allocator->Allocate<IdentifierNameSyntaxBuilder>(1);
            new (retn) IdentifierNameSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline PredefinedTypeSyntaxBuilder* PredefinedTypeSyntax() { 
            PredefinedTypeSyntaxBuilder * retn = allocator->Allocate<PredefinedTypeSyntaxBuilder>(1);
            new (retn) PredefinedTypeSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline TupleElementSyntaxBuilder* TupleElementSyntax() { 
            TupleElementSyntaxBuilder * retn = allocator->Allocate<TupleElementSyntaxBuilder>(1);
            new (retn) TupleElementSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline TupleTypeSyntaxBuilder* TupleTypeSyntax() { 
            TupleTypeSyntaxBuilder * retn = allocator->Allocate<TupleTypeSyntaxBuilder>(1);
            new (retn) TupleTypeSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline ArrayTypeSyntaxBuilder* ArrayTypeSyntax() { 
            ArrayTypeSyntaxBuilder * retn = allocator->Allocate<ArrayTypeSyntaxBuilder>(1);
            new (retn) ArrayTypeSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline RefTypeSyntaxBuilder* RefTypeSyntax() { 
            RefTypeSyntaxBuilder * retn = allocator->Allocate<RefTypeSyntaxBuilder>(1);
            new (retn) RefTypeSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline OmittedArraySizeExpressionSyntaxBuilder* OmittedArraySizeExpressionSyntax() { 
            OmittedArraySizeExpressionSyntaxBuilder * retn = allocator->Allocate<OmittedArraySizeExpressionSyntaxBuilder>(1);
            new (retn) OmittedArraySizeExpressionSyntaxBuilder();
            retn->allocator = allocator;
            return retn;
        }

        inline NullableTypeBuilder* NullableType() { 
            NullableTypeBuilder * retn = allocator->Allocate<NullableTypeBuilder>(1);
            new (retn) NullableTypeBuilder();
            retn->allocator = allocator;
            return retn;
        }        
        
    };

}
