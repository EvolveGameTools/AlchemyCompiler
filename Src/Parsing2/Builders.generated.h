//#include "./Builders.h"
//
//namespace Alchemy::Compilation {
//
//    struct ArrayRankSpecifierSyntaxBuilder: ExpressionSyntaxBuilder {
//
//        SyntaxToken open {};
//        SeparatedSyntaxListBuilder* ranks {};
//        SyntaxToken close {};
//
//        inline ArrayRankSpecifierSyntaxBuilder* Open(SyntaxToken builder) {
//            open = builder;
//            return this;
//        }
//
//        inline ArrayRankSpecifierSyntaxBuilder* Ranks(SeparatedSyntaxListBuilder* builder) {
//            ranks = builder;
//            return this;
//        }
//
//        inline ArrayRankSpecifierSyntaxBuilder* Close(SyntaxToken builder) {
//            close = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            ArrayRankSpecifierSyntax* retn = allocator->Allocate<ArrayRankSpecifierSyntax>(1);
//            retn->_kind = SyntaxKind::ArrayRankSpecifier;
//            retn->open = open;
//            retn->ranks = ranks != nullptr ? (SeparatedSyntaxList<ExpressionSyntax>*)ranks->Build() : nullptr;
//            retn->close = close;
//            return retn;
//        }
//
//    };
//
//    struct TypeArgumentListSyntaxBuilder: SyntaxBaseBuilder {
//
//        SyntaxToken lessThanToken {};
//        SeparatedSyntaxListBuilder* arguments {};
//        SyntaxToken greaterThanToken {};
//
//        inline TypeArgumentListSyntaxBuilder* LessThanToken(SyntaxToken builder) {
//            lessThanToken = builder;
//            return this;
//        }
//
//        inline TypeArgumentListSyntaxBuilder* Arguments(SeparatedSyntaxListBuilder* builder) {
//            arguments = builder;
//            return this;
//        }
//
//        inline TypeArgumentListSyntaxBuilder* GreaterThanToken(SyntaxToken builder) {
//            greaterThanToken = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            TypeArgumentListSyntax* retn = allocator->Allocate<TypeArgumentListSyntax>(1);
//            retn->_kind = SyntaxKind::TypeArgumentList;
//            retn->lessThanToken = lessThanToken;
//            retn->arguments = arguments != nullptr ? (SeparatedSyntaxList<TypeSyntax>*)arguments->Build() : nullptr;
//            retn->greaterThanToken = greaterThanToken;
//            return retn;
//        }
//
//    };
//
//    struct GenericNameSyntaxBuilder: SimpleNameSyntaxBuilder {
//
//        SyntaxToken identifier {};
//        TypeArgumentListSyntaxBuilder * typeArgumentList {};
//
//        inline GenericNameSyntaxBuilder* Identifier(SyntaxToken builder) {
//            identifier = builder;
//            return this;
//        }
//
//        inline GenericNameSyntaxBuilder* TypeArgumentList(TypeArgumentListSyntaxBuilder * builder) {
//            typeArgumentList = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            GenericNameSyntax* retn = allocator->Allocate<GenericNameSyntax>(1);
//            retn->_kind = SyntaxKind::GenericName;
//            retn->identifier = identifier;
//            retn->typeArgumentList = typeArgumentList != nullptr ? (TypeArgumentListSyntax*)typeArgumentList->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct QualifiedNameSyntaxBuilder: NameSyntaxBuilder {
//
//        NameSyntaxBuilder * left {};
//        SyntaxToken dotToken {};
//        SimpleNameSyntaxBuilder * right {};
//
//        inline QualifiedNameSyntaxBuilder* Left(NameSyntaxBuilder * builder) {
//            left = builder;
//            return this;
//        }
//
//        inline QualifiedNameSyntaxBuilder* DotToken(SyntaxToken builder) {
//            dotToken = builder;
//            return this;
//        }
//
//        inline QualifiedNameSyntaxBuilder* Right(SimpleNameSyntaxBuilder * builder) {
//            right = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            QualifiedNameSyntax* retn = allocator->Allocate<QualifiedNameSyntax>(1);
//            retn->_kind = SyntaxKind::QualifiedName;
//            retn->left = left != nullptr ? (NameSyntax*)left->Build() : nullptr;
//            retn->dotToken = dotToken;
//            retn->right = right != nullptr ? (SimpleNameSyntax*)right->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct IdentifierNameSyntaxBuilder: SimpleNameSyntaxBuilder {
//
//        SyntaxToken identifier {};
//
//        inline IdentifierNameSyntaxBuilder* Identifier(SyntaxToken builder) {
//            identifier = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            IdentifierNameSyntax* retn = allocator->Allocate<IdentifierNameSyntax>(1);
//            retn->_kind = SyntaxKind::IdentifierName;
//            retn->identifier = identifier;
//            return retn;
//        }
//
//    };
//
//    struct PredefinedTypeSyntaxBuilder: TypeSyntaxBuilder {
//
//        SyntaxToken typeToken {};
//
//        inline PredefinedTypeSyntaxBuilder* TypeToken(SyntaxToken builder) {
//            typeToken = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            PredefinedTypeSyntax* retn = allocator->Allocate<PredefinedTypeSyntax>(1);
//            retn->_kind = SyntaxKind::PredefinedType;
//            retn->typeToken = typeToken;
//            return retn;
//        }
//
//    };
//
//    struct TupleElementSyntaxBuilder: SyntaxBaseBuilder {
//
//        TypeSyntaxBuilder * type {};
//        SyntaxToken identifier {};
//
//        inline TupleElementSyntaxBuilder* Type(TypeSyntaxBuilder * builder) {
//            type = builder;
//            return this;
//        }
//
//        inline TupleElementSyntaxBuilder* Identifier(SyntaxToken builder) {
//            identifier = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            TupleElementSyntax* retn = allocator->Allocate<TupleElementSyntax>(1);
//            retn->_kind = SyntaxKind::TupleElement;
//            retn->type = type != nullptr ? (TypeSyntax*)type->Build() : nullptr;
//            retn->identifier = identifier;
//            return retn;
//        }
//
//    };
//
//    struct TupleTypeSyntaxBuilder: TypeSyntaxBuilder {
//
//        SyntaxToken openParenToken {};
//        SeparatedSyntaxListBuilder* elements {};
//        SyntaxToken closeParenToken {};
//
//        inline TupleTypeSyntaxBuilder* OpenParenToken(SyntaxToken builder) {
//            openParenToken = builder;
//            return this;
//        }
//
//        inline TupleTypeSyntaxBuilder* Elements(SeparatedSyntaxListBuilder* builder) {
//            elements = builder;
//            return this;
//        }
//
//        inline TupleTypeSyntaxBuilder* CloseParenToken(SyntaxToken builder) {
//            closeParenToken = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            TupleTypeSyntax* retn = allocator->Allocate<TupleTypeSyntax>(1);
//            retn->_kind = SyntaxKind::TupleType;
//            retn->openParenToken = openParenToken;
//            retn->elements = elements != nullptr ? (SeparatedSyntaxList<TupleElementSyntax>*)elements->Build() : nullptr;
//            retn->closeParenToken = closeParenToken;
//            return retn;
//        }
//
//    };
//
//    struct ArrayTypeSyntaxBuilder: TypeSyntaxBuilder {
//
//        TypeSyntaxBuilder * elementType {};
//        SyntaxListBuilder* ranks {};
//
//        inline ArrayTypeSyntaxBuilder* ElementType(TypeSyntaxBuilder * builder) {
//            elementType = builder;
//            return this;
//        }
//
//        inline ArrayTypeSyntaxBuilder* Ranks(SyntaxListBuilder* builder) {
//            ranks = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            ArrayTypeSyntax* retn = allocator->Allocate<ArrayTypeSyntax>(1);
//            retn->_kind = SyntaxKind::ArrayType;
//            retn->elementType = elementType != nullptr ? (TypeSyntax*)elementType->Build() : nullptr;
//            retn->ranks = ranks != nullptr ? (SyntaxList<ArrayRankSpecifierSyntax>*)ranks->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct RefTypeSyntaxBuilder: TypeSyntaxBuilder {
//
//        SyntaxToken refKeyword {};
//        SyntaxToken readonlyKeyword {};
//        TypeSyntaxBuilder * type {};
//
//        inline RefTypeSyntaxBuilder* RefKeyword(SyntaxToken builder) {
//            refKeyword = builder;
//            return this;
//        }
//
//        inline RefTypeSyntaxBuilder* ReadonlyKeyword(SyntaxToken builder) {
//            readonlyKeyword = builder;
//            return this;
//        }
//
//        inline RefTypeSyntaxBuilder* Type(TypeSyntaxBuilder * builder) {
//            type = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            RefTypeSyntax* retn = allocator->Allocate<RefTypeSyntax>(1);
//            retn->_kind = SyntaxKind::RefType;
//            retn->refKeyword = refKeyword;
//            retn->readonlyKeyword = readonlyKeyword;
//            retn->type = type != nullptr ? (TypeSyntax*)type->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct OmittedArraySizeExpressionSyntaxBuilder: ExpressionSyntaxBuilder {
//
//        SyntaxToken token {};
//
//        inline OmittedArraySizeExpressionSyntaxBuilder* Token(SyntaxToken builder) {
//            token = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            OmittedArraySizeExpressionSyntax* retn = allocator->Allocate<OmittedArraySizeExpressionSyntax>(1);
//            retn->_kind = SyntaxKind::OmittedArraySizeExpression;
//            retn->token = token;
//            return retn;
//        }
//
//    };
//
//    struct NullableTypeBuilder: TypeSyntaxBuilder {
//
//        TypeSyntaxBuilder * elementType {};
//        SyntaxToken questionMark {};
//
//        inline NullableTypeBuilder* ElementType(TypeSyntaxBuilder * builder) {
//            elementType = builder;
//            return this;
//        }
//
//        inline NullableTypeBuilder* QuestionMark(SyntaxToken builder) {
//            questionMark = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            NullableType* retn = allocator->Allocate<NullableType>(1);
//            retn->_kind = SyntaxKind::NullableType;
//            retn->elementType = elementType != nullptr ? (TypeSyntax*)elementType->Build() : nullptr;
//            retn->questionMark = questionMark;
//            return retn;
//        }
//
//    };
//
//    struct ArgumentSyntaxBuilder: SyntaxBaseBuilder {
//
//        SyntaxToken nameColon {};
//        SyntaxToken refKindKeyword {};
//        ExpressionSyntaxBuilder * expression {};
//
//        inline ArgumentSyntaxBuilder* NameColon(SyntaxToken builder) {
//            nameColon = builder;
//            return this;
//        }
//
//        inline ArgumentSyntaxBuilder* RefKindKeyword(SyntaxToken builder) {
//            refKindKeyword = builder;
//            return this;
//        }
//
//        inline ArgumentSyntaxBuilder* Expression(ExpressionSyntaxBuilder * builder) {
//            expression = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            ArgumentSyntax* retn = allocator->Allocate<ArgumentSyntax>(1);
//            retn->_kind = SyntaxKind::Argument;
//            retn->nameColon = nameColon;
//            retn->refKindKeyword = refKindKeyword;
//            retn->expression = expression != nullptr ? (ExpressionSyntax*)expression->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct BracketedArgumentListSyntaxBuilder: SyntaxBaseBuilder {
//
//        SyntaxToken openBracket {};
//        SeparatedSyntaxListBuilder* arguments {};
//        SyntaxToken closeBracket {};
//
//        inline BracketedArgumentListSyntaxBuilder* OpenBracket(SyntaxToken builder) {
//            openBracket = builder;
//            return this;
//        }
//
//        inline BracketedArgumentListSyntaxBuilder* Arguments(SeparatedSyntaxListBuilder* builder) {
//            arguments = builder;
//            return this;
//        }
//
//        inline BracketedArgumentListSyntaxBuilder* CloseBracket(SyntaxToken builder) {
//            closeBracket = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            BracketedArgumentListSyntax* retn = allocator->Allocate<BracketedArgumentListSyntax>(1);
//            retn->_kind = SyntaxKind::BracketedArgumentList;
//            retn->openBracket = openBracket;
//            retn->arguments = arguments != nullptr ? (SeparatedSyntaxList<ArgumentSyntax>*)arguments->Build() : nullptr;
//            retn->closeBracket = closeBracket;
//            return retn;
//        }
//
//    };
//
//    struct EqualsValueClauseSyntaxBuilder: SyntaxBaseBuilder {
//
//        SyntaxToken equalsToken {};
//        ExpressionSyntaxBuilder * value {};
//
//        inline EqualsValueClauseSyntaxBuilder* EqualsToken(SyntaxToken builder) {
//            equalsToken = builder;
//            return this;
//        }
//
//        inline EqualsValueClauseSyntaxBuilder* Value(ExpressionSyntaxBuilder * builder) {
//            value = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            EqualsValueClauseSyntax* retn = allocator->Allocate<EqualsValueClauseSyntax>(1);
//            retn->_kind = SyntaxKind::EqualsValueClause;
//            retn->equalsToken = equalsToken;
//            retn->value = value != nullptr ? (ExpressionSyntax*)value->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct RefExpressionSyntaxBuilder: ExpressionSyntaxBuilder {
//
//        SyntaxToken refKeyword {};
//        ExpressionSyntaxBuilder * expression {};
//
//        inline RefExpressionSyntaxBuilder* RefKeyword(SyntaxToken builder) {
//            refKeyword = builder;
//            return this;
//        }
//
//        inline RefExpressionSyntaxBuilder* Expression(ExpressionSyntaxBuilder * builder) {
//            expression = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            RefExpressionSyntax* retn = allocator->Allocate<RefExpressionSyntax>(1);
//            retn->_kind = SyntaxKind::RefExpression;
//            retn->refKeyword = refKeyword;
//            retn->expression = expression != nullptr ? (ExpressionSyntax*)expression->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct VariableDeclaratorSyntaxBuilder: SyntaxBaseBuilder {
//
//        SyntaxToken identifier {};
//        BracketedArgumentListSyntaxBuilder * argumentList {};
//        EqualsValueClauseSyntaxBuilder * initializer {};
//
//        inline VariableDeclaratorSyntaxBuilder* Identifier(SyntaxToken builder) {
//            identifier = builder;
//            return this;
//        }
//
//        inline VariableDeclaratorSyntaxBuilder* ArgumentList(BracketedArgumentListSyntaxBuilder * builder) {
//            argumentList = builder;
//            return this;
//        }
//
//        inline VariableDeclaratorSyntaxBuilder* Initializer(EqualsValueClauseSyntaxBuilder * builder) {
//            initializer = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            VariableDeclaratorSyntax* retn = allocator->Allocate<VariableDeclaratorSyntax>(1);
//            retn->_kind = SyntaxKind::VariableDeclarator;
//            retn->identifier = identifier;
//            retn->argumentList = argumentList != nullptr ? (BracketedArgumentListSyntax*)argumentList->Build() : nullptr;
//            retn->initializer = initializer != nullptr ? (EqualsValueClauseSyntax*)initializer->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct TypeParameterSyntaxBuilder: SyntaxBaseBuilder {
//
//        SyntaxToken identifier {};
//
//        inline TypeParameterSyntaxBuilder* Identifier(SyntaxToken builder) {
//            identifier = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            TypeParameterSyntax* retn = allocator->Allocate<TypeParameterSyntax>(1);
//            retn->_kind = SyntaxKind::TypeParameter;
//            retn->identifier = identifier;
//            return retn;
//        }
//
//    };
//
//    struct TypeParameterListSyntaxBuilder: SyntaxBaseBuilder {
//
//        SyntaxToken lessThanToken {};
//        SeparatedSyntaxListBuilder* parameters {};
//        SyntaxToken greaterThanToken {};
//
//        inline TypeParameterListSyntaxBuilder* LessThanToken(SyntaxToken builder) {
//            lessThanToken = builder;
//            return this;
//        }
//
//        inline TypeParameterListSyntaxBuilder* Parameters(SeparatedSyntaxListBuilder* builder) {
//            parameters = builder;
//            return this;
//        }
//
//        inline TypeParameterListSyntaxBuilder* GreaterThanToken(SyntaxToken builder) {
//            greaterThanToken = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            TypeParameterListSyntax* retn = allocator->Allocate<TypeParameterListSyntax>(1);
//            retn->_kind = SyntaxKind::TypeParameterList;
//            retn->lessThanToken = lessThanToken;
//            retn->parameters = parameters != nullptr ? (SeparatedSyntaxList<TypeParameterSyntax>*)parameters->Build() : nullptr;
//            retn->greaterThanToken = greaterThanToken;
//            return retn;
//        }
//
//    };
//
//    struct ParameterSyntaxBuilder: SyntaxBaseBuilder {
//
//        TokenListBuilder * modifiers {};
//        TypeSyntaxBuilder * type {};
//        SyntaxToken identifier {};
//        EqualsValueClauseSyntaxBuilder * defaultValue {};
//
//        inline ParameterSyntaxBuilder* Modifiers(TokenListBuilder * builder) {
//            modifiers = builder;
//            return this;
//        }
//
//        inline ParameterSyntaxBuilder* Type(TypeSyntaxBuilder * builder) {
//            type = builder;
//            return this;
//        }
//
//        inline ParameterSyntaxBuilder* Identifier(SyntaxToken builder) {
//            identifier = builder;
//            return this;
//        }
//
//        inline ParameterSyntaxBuilder* DefaultValue(EqualsValueClauseSyntaxBuilder * builder) {
//            defaultValue = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            ParameterSyntax* retn = allocator->Allocate<ParameterSyntax>(1);
//            retn->_kind = SyntaxKind::Parameter;
//            retn->modifiers = modifiers != nullptr ? (TokenList*)modifiers->Build() : nullptr;
//            retn->type = type != nullptr ? (TypeSyntax*)type->Build() : nullptr;
//            retn->identifier = identifier;
//            retn->defaultValue = defaultValue != nullptr ? (EqualsValueClauseSyntax*)defaultValue->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct ParameterListSyntaxBuilder: SyntaxBaseBuilder {
//
//        SyntaxToken openParen {};
//        SeparatedSyntaxListBuilder* parameters {};
//        SyntaxToken closeParen {};
//
//        inline ParameterListSyntaxBuilder* OpenParen(SyntaxToken builder) {
//            openParen = builder;
//            return this;
//        }
//
//        inline ParameterListSyntaxBuilder* Parameters(SeparatedSyntaxListBuilder* builder) {
//            parameters = builder;
//            return this;
//        }
//
//        inline ParameterListSyntaxBuilder* CloseParen(SyntaxToken builder) {
//            closeParen = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            ParameterListSyntax* retn = allocator->Allocate<ParameterListSyntax>(1);
//            retn->_kind = SyntaxKind::ParameterList;
//            retn->openParen = openParen;
//            retn->parameters = parameters != nullptr ? (SeparatedSyntaxList<ParameterSyntax>*)parameters->Build() : nullptr;
//            retn->closeParen = closeParen;
//            return retn;
//        }
//
//    };
//
//    struct ConstraintClausesSyntaxBuilder: SyntaxBaseBuilder {
//
//        SyntaxToken dummy {};
//
//        inline ConstraintClausesSyntaxBuilder* Dummy(SyntaxToken builder) {
//            dummy = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            ConstraintClausesSyntax* retn = allocator->Allocate<ConstraintClausesSyntax>(1);
//            retn->_kind = SyntaxKind::ConstraintClauses;
//            retn->dummy = dummy;
//            return retn;
//        }
//
//    };
//
//    struct LocalFunctionStatementSyntaxBuilder: SyntaxBaseBuilder {
//
//        TokenListBuilder * modifiers {};
//        TypeSyntaxBuilder * returnType {};
//        SyntaxToken identifier {};
//        TypeParameterListSyntaxBuilder * typeParameters {};
//        ParameterListSyntaxBuilder * parameters {};
//        ConstraintClausesSyntaxBuilder * constraints {};
//        SyntaxBaseBuilder * body {};
//
//        inline LocalFunctionStatementSyntaxBuilder* Modifiers(TokenListBuilder * builder) {
//            modifiers = builder;
//            return this;
//        }
//
//        inline LocalFunctionStatementSyntaxBuilder* ReturnType(TypeSyntaxBuilder * builder) {
//            returnType = builder;
//            return this;
//        }
//
//        inline LocalFunctionStatementSyntaxBuilder* Identifier(SyntaxToken builder) {
//            identifier = builder;
//            return this;
//        }
//
//        inline LocalFunctionStatementSyntaxBuilder* TypeParameters(TypeParameterListSyntaxBuilder * builder) {
//            typeParameters = builder;
//            return this;
//        }
//
//        inline LocalFunctionStatementSyntaxBuilder* Parameters(ParameterListSyntaxBuilder * builder) {
//            parameters = builder;
//            return this;
//        }
//
//        inline LocalFunctionStatementSyntaxBuilder* Constraints(ConstraintClausesSyntaxBuilder * builder) {
//            constraints = builder;
//            return this;
//        }
//
//        inline LocalFunctionStatementSyntaxBuilder* Body(SyntaxBaseBuilder * builder) {
//            body = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            LocalFunctionStatementSyntax* retn = allocator->Allocate<LocalFunctionStatementSyntax>(1);
//            retn->_kind = SyntaxKind::LocalFunctionStatement;
//            retn->modifiers = modifiers != nullptr ? (TokenList*)modifiers->Build() : nullptr;
//            retn->returnType = returnType != nullptr ? (TypeSyntax*)returnType->Build() : nullptr;
//            retn->identifier = identifier;
//            retn->typeParameters = typeParameters != nullptr ? (TypeParameterListSyntax*)typeParameters->Build() : nullptr;
//            retn->parameters = parameters != nullptr ? (ParameterListSyntax*)parameters->Build() : nullptr;
//            retn->constraints = constraints != nullptr ? (ConstraintClausesSyntax*)constraints->Build() : nullptr;
//            retn->body = body != nullptr ? (SyntaxBase*)body->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct VariableDeclarationSyntaxBuilder: SyntaxBaseBuilder {
//
//        TypeSyntaxBuilder * type {};
//        SeparatedSyntaxListBuilder* variables {};
//
//        inline VariableDeclarationSyntaxBuilder* Type(TypeSyntaxBuilder * builder) {
//            type = builder;
//            return this;
//        }
//
//        inline VariableDeclarationSyntaxBuilder* Variables(SeparatedSyntaxListBuilder* builder) {
//            variables = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            VariableDeclarationSyntax* retn = allocator->Allocate<VariableDeclarationSyntax>(1);
//            retn->_kind = SyntaxKind::VariableDeclaration;
//            retn->type = type != nullptr ? (TypeSyntax*)type->Build() : nullptr;
//            retn->variables = variables != nullptr ? (SeparatedSyntaxList<VariableDeclaratorSyntax>*)variables->Build() : nullptr;
//            return retn;
//        }
//
//    };
//
//    struct FieldDeclarationSyntaxBuilder: MemberDeclarationSyntaxBuilder {
//
//        TokenListBuilder * modifiers {};
//        VariableDeclarationSyntaxBuilder * declaration {};
//        SyntaxToken semicolonToken {};
//
//        inline FieldDeclarationSyntaxBuilder* Modifiers(TokenListBuilder * builder) {
//            modifiers = builder;
//            return this;
//        }
//
//        inline FieldDeclarationSyntaxBuilder* Declaration(VariableDeclarationSyntaxBuilder * builder) {
//            declaration = builder;
//            return this;
//        }
//
//        inline FieldDeclarationSyntaxBuilder* SemicolonToken(SyntaxToken builder) {
//            semicolonToken = builder;
//            return this;
//        }
//
//        inline SyntaxBase* Build() override {
//            FieldDeclarationSyntax* retn = allocator->Allocate<FieldDeclarationSyntax>(1);
//            retn->_kind = SyntaxKind::FieldDeclaration;
//            retn->modifiers = modifiers != nullptr ? (TokenList*)modifiers->Build() : nullptr;
//            retn->declaration = declaration != nullptr ? (VariableDeclarationSyntax*)declaration->Build() : nullptr;
//            retn->semicolonToken = semicolonToken;
//            return retn;
//        }
//
//    };
//
//    struct Builder : BuilderBase {
//
//        explicit Builder(LinearAllocator * allocator) : BuilderBase((allocator)) {}
//
//        inline ArrayRankSpecifierSyntaxBuilder* ArrayRankSpecifierSyntax() {
//            ArrayRankSpecifierSyntaxBuilder * retn = allocator->Allocate<ArrayRankSpecifierSyntaxBuilder>(1);
//            new (retn) ArrayRankSpecifierSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline TypeArgumentListSyntaxBuilder* TypeArgumentListSyntax() {
//            TypeArgumentListSyntaxBuilder * retn = allocator->Allocate<TypeArgumentListSyntaxBuilder>(1);
//            new (retn) TypeArgumentListSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline GenericNameSyntaxBuilder* GenericNameSyntax() {
//            GenericNameSyntaxBuilder * retn = allocator->Allocate<GenericNameSyntaxBuilder>(1);
//            new (retn) GenericNameSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline QualifiedNameSyntaxBuilder* QualifiedNameSyntax() {
//            QualifiedNameSyntaxBuilder * retn = allocator->Allocate<QualifiedNameSyntaxBuilder>(1);
//            new (retn) QualifiedNameSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline IdentifierNameSyntaxBuilder* IdentifierNameSyntax() {
//            IdentifierNameSyntaxBuilder * retn = allocator->Allocate<IdentifierNameSyntaxBuilder>(1);
//            new (retn) IdentifierNameSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline PredefinedTypeSyntaxBuilder* PredefinedTypeSyntax() {
//            PredefinedTypeSyntaxBuilder * retn = allocator->Allocate<PredefinedTypeSyntaxBuilder>(1);
//            new (retn) PredefinedTypeSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline TupleElementSyntaxBuilder* TupleElementSyntax() {
//            TupleElementSyntaxBuilder * retn = allocator->Allocate<TupleElementSyntaxBuilder>(1);
//            new (retn) TupleElementSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline TupleTypeSyntaxBuilder* TupleTypeSyntax() {
//            TupleTypeSyntaxBuilder * retn = allocator->Allocate<TupleTypeSyntaxBuilder>(1);
//            new (retn) TupleTypeSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline ArrayTypeSyntaxBuilder* ArrayTypeSyntax() {
//            ArrayTypeSyntaxBuilder * retn = allocator->Allocate<ArrayTypeSyntaxBuilder>(1);
//            new (retn) ArrayTypeSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline RefTypeSyntaxBuilder* RefTypeSyntax() {
//            RefTypeSyntaxBuilder * retn = allocator->Allocate<RefTypeSyntaxBuilder>(1);
//            new (retn) RefTypeSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline OmittedArraySizeExpressionSyntaxBuilder* OmittedArraySizeExpressionSyntax() {
//            OmittedArraySizeExpressionSyntaxBuilder * retn = allocator->Allocate<OmittedArraySizeExpressionSyntaxBuilder>(1);
//            new (retn) OmittedArraySizeExpressionSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline NullableTypeBuilder* NullableType() {
//            NullableTypeBuilder * retn = allocator->Allocate<NullableTypeBuilder>(1);
//            new (retn) NullableTypeBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline ArgumentSyntaxBuilder* ArgumentSyntax() {
//            ArgumentSyntaxBuilder * retn = allocator->Allocate<ArgumentSyntaxBuilder>(1);
//            new (retn) ArgumentSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline BracketedArgumentListSyntaxBuilder* BracketedArgumentListSyntax() {
//            BracketedArgumentListSyntaxBuilder * retn = allocator->Allocate<BracketedArgumentListSyntaxBuilder>(1);
//            new (retn) BracketedArgumentListSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline EqualsValueClauseSyntaxBuilder* EqualsValueClauseSyntax() {
//            EqualsValueClauseSyntaxBuilder * retn = allocator->Allocate<EqualsValueClauseSyntaxBuilder>(1);
//            new (retn) EqualsValueClauseSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline RefExpressionSyntaxBuilder* RefExpressionSyntax() {
//            RefExpressionSyntaxBuilder * retn = allocator->Allocate<RefExpressionSyntaxBuilder>(1);
//            new (retn) RefExpressionSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline VariableDeclaratorSyntaxBuilder* VariableDeclaratorSyntax() {
//            VariableDeclaratorSyntaxBuilder * retn = allocator->Allocate<VariableDeclaratorSyntaxBuilder>(1);
//            new (retn) VariableDeclaratorSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline TypeParameterSyntaxBuilder* TypeParameterSyntax() {
//            TypeParameterSyntaxBuilder * retn = allocator->Allocate<TypeParameterSyntaxBuilder>(1);
//            new (retn) TypeParameterSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline TypeParameterListSyntaxBuilder* TypeParameterListSyntax() {
//            TypeParameterListSyntaxBuilder * retn = allocator->Allocate<TypeParameterListSyntaxBuilder>(1);
//            new (retn) TypeParameterListSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline ParameterSyntaxBuilder* ParameterSyntax() {
//            ParameterSyntaxBuilder * retn = allocator->Allocate<ParameterSyntaxBuilder>(1);
//            new (retn) ParameterSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline ParameterListSyntaxBuilder* ParameterListSyntax() {
//            ParameterListSyntaxBuilder * retn = allocator->Allocate<ParameterListSyntaxBuilder>(1);
//            new (retn) ParameterListSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline ConstraintClausesSyntaxBuilder* ConstraintClausesSyntax() {
//            ConstraintClausesSyntaxBuilder * retn = allocator->Allocate<ConstraintClausesSyntaxBuilder>(1);
//            new (retn) ConstraintClausesSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline LocalFunctionStatementSyntaxBuilder* LocalFunctionStatementSyntax() {
//            LocalFunctionStatementSyntaxBuilder * retn = allocator->Allocate<LocalFunctionStatementSyntaxBuilder>(1);
//            new (retn) LocalFunctionStatementSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline VariableDeclarationSyntaxBuilder* VariableDeclarationSyntax() {
//            VariableDeclarationSyntaxBuilder * retn = allocator->Allocate<VariableDeclarationSyntaxBuilder>(1);
//            new (retn) VariableDeclarationSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//        inline FieldDeclarationSyntaxBuilder* FieldDeclarationSyntax() {
//            FieldDeclarationSyntaxBuilder * retn = allocator->Allocate<FieldDeclarationSyntaxBuilder>(1);
//            new (retn) FieldDeclarationSyntaxBuilder();
//            retn->allocator = allocator;
//            return retn;
//        }
//
//    };
//
//}
