#include "../Src/Parsing3/SyntaxBase.h"
#include "../Src/Parsing3/SyntaxNodes.h"

namespace Alchemy::Compilation {
    
    SyntaxToken GetFirstToken(SyntaxBase * syntaxBase) {
    
        if(syntaxBase == nullptr) {
            return SyntaxToken();
        }
        
        switch(syntaxBase->GetKind()) {
            case SyntaxKind::EmptyStatement: {
                EmptyStatementSyntax* p = (EmptyStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::BreakStatement: {
                BreakStatementSyntax* p = (BreakStatementSyntax*)syntaxBase;
                if(p->breakKeyword.IsValid()) return p->breakKeyword;
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::ContinueStatement: {
                ContinueStatementSyntax* p = (ContinueStatementSyntax*)syntaxBase;
                if(p->continueKeyword.IsValid()) return p->continueKeyword;
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::ForStatement: {
                ForStatementSyntax* p = (ForStatementSyntax*)syntaxBase;
                if(p->forKeyword.IsValid()) return p->forKeyword;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->declaration != nullptr) return GetFirstToken((SyntaxBase*)p->declaration);
                if(p->initializers != nullptr && p->initializers->itemCount != 0) return GetFirstToken(p->initializers->items[0]);
                if(p->firstSemiColon.IsValid()) return p->firstSemiColon;
                if(p->condition != nullptr) return GetFirstToken((SyntaxBase*)p->condition);
                if(p->secondSemiColon.IsValid()) return p->secondSemiColon;
                if(p->incrementors != nullptr && p->incrementors->itemCount != 0) return GetFirstToken(p->incrementors->items[0]);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->statement != nullptr) return GetFirstToken((SyntaxBase*)p->statement);
                return SyntaxToken();
            }

            case SyntaxKind::ThrowStatement: {
                ThrowStatementSyntax* p = (ThrowStatementSyntax*)syntaxBase;
                if(p->throwKeyword.IsValid()) return p->throwKeyword;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::CatchDeclaration: {
                CatchDeclarationSyntax* p = (CatchDeclarationSyntax*)syntaxBase;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->closeParen.IsValid()) return p->closeParen;
                return SyntaxToken();
            }

            case SyntaxKind::CatchFilterClause: {
                CatchFilterClauseSyntax* p = (CatchFilterClauseSyntax*)syntaxBase;
                if(p->whenKeyword.IsValid()) return p->whenKeyword;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->filterExpression != nullptr) return GetFirstToken((SyntaxBase*)p->filterExpression);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::CatchClause: {
                CatchClauseSyntax* p = (CatchClauseSyntax*)syntaxBase;
                if(p->catchKeyword.IsValid()) return p->catchKeyword;
                if(p->declaration != nullptr) return GetFirstToken((SyntaxBase*)p->declaration);
                if(p->filter != nullptr) return GetFirstToken((SyntaxBase*)p->filter);
                if(p->block != nullptr) return GetFirstToken((SyntaxBase*)p->block);
                return SyntaxToken();
            }

            case SyntaxKind::FinallyClause: {
                FinallyClauseSyntax* p = (FinallyClauseSyntax*)syntaxBase;
                if(p->finallyKeyword.IsValid()) return p->finallyKeyword;
                if(p->block != nullptr) return GetFirstToken((SyntaxBase*)p->block);
                return SyntaxToken();
            }

            case SyntaxKind::TryStatement: {
                TryStatementSyntax* p = (TryStatementSyntax*)syntaxBase;
                if(p->tryKeyword.IsValid()) return p->tryKeyword;
                if(p->tryBlock != nullptr) return GetFirstToken((SyntaxBase*)p->tryBlock);
                if(p->catchClauses != nullptr && p->catchClauses->size != 0) return GetFirstToken(p->catchClauses->array[0]);
                if(p->finallyClaus != nullptr) return GetFirstToken((SyntaxBase*)p->finallyClaus);
                return SyntaxToken();
            }

            case SyntaxKind::DefaultSwitchLabel: {
                DefaultSwitchLabelSyntax* p = (DefaultSwitchLabelSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->colon.IsValid()) return p->colon;
                return SyntaxToken();
            }

            case SyntaxKind::CaseSwitchLabel: {
                CaseSwitchLabelSyntax* p = (CaseSwitchLabelSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->value != nullptr) return GetFirstToken((SyntaxBase*)p->value);
                if(p->colon.IsValid()) return p->colon;
                return SyntaxToken();
            }

            case SyntaxKind::CasePatternSwitchLabel: {
                CasePatternSwitchLabelSyntax* p = (CasePatternSwitchLabelSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->pattern != nullptr) return GetFirstToken((SyntaxBase*)p->pattern);
                if(p->whenClause != nullptr) return GetFirstToken((SyntaxBase*)p->whenClause);
                if(p->colonToken.IsValid()) return p->colonToken;
                return SyntaxToken();
            }

            case SyntaxKind::SwitchSection: {
                SwitchSectionSyntax* p = (SwitchSectionSyntax*)syntaxBase;
                if(p->labels != nullptr && p->labels->size != 0) return GetFirstToken(p->labels->array[0]);
                if(p->statements != nullptr && p->statements->size != 0) return GetFirstToken(p->statements->array[0]);
                return SyntaxToken();
            }

            case SyntaxKind::SwitchStatement: {
                SwitchStatementSyntax* p = (SwitchStatementSyntax*)syntaxBase;
                if(p->switchKeyword.IsValid()) return p->switchKeyword;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->sections != nullptr && p->sections->size != 0) return GetFirstToken(p->sections->array[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::UsingStatement: {
                UsingStatementSyntax* p = (UsingStatementSyntax*)syntaxBase;
                if(p->usingKeyword.IsValid()) return p->usingKeyword;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->declaration != nullptr) return GetFirstToken((SyntaxBase*)p->declaration);
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->statement != nullptr) return GetFirstToken((SyntaxBase*)p->statement);
                return SyntaxToken();
            }

            case SyntaxKind::WhileStatement: {
                WhileStatementSyntax* p = (WhileStatementSyntax*)syntaxBase;
                if(p->whileKeyword.IsValid()) return p->whileKeyword;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->condition != nullptr) return GetFirstToken((SyntaxBase*)p->condition);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->statement != nullptr) return GetFirstToken((SyntaxBase*)p->statement);
                return SyntaxToken();
            }

            case SyntaxKind::DoStatement: {
                DoStatementSyntax* p = (DoStatementSyntax*)syntaxBase;
                if(p->doKeyword.IsValid()) return p->doKeyword;
                if(p->statement != nullptr) return GetFirstToken((SyntaxBase*)p->statement);
                if(p->whileKeyword.IsValid()) return p->whileKeyword;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->condition != nullptr) return GetFirstToken((SyntaxBase*)p->condition);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*)syntaxBase;
                if(p->open.IsValid()) return p->open;
                if(p->ranks != nullptr && p->ranks->itemCount != 0) return GetFirstToken(p->ranks->items[0]);
                if(p->close.IsValid()) return p->close;
                return SyntaxToken();
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* p = (TypeArgumentListSyntax*)syntaxBase;
                if(p->lessThanToken.IsValid()) return p->lessThanToken;
                if(p->arguments != nullptr && p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
                if(p->greaterThanToken.IsValid()) return p->greaterThanToken;
                return SyntaxToken();
            }

            case SyntaxKind::GenericName: {
                GenericNameSyntax* p = (GenericNameSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeArgumentList != nullptr) return GetFirstToken((SyntaxBase*)p->typeArgumentList);
                return SyntaxToken();
            }

            case SyntaxKind::ElementBindingExpression: {
                ElementBindingExpressionSyntax* p = (ElementBindingExpressionSyntax*)syntaxBase;
                if(p->argumentList != nullptr) return GetFirstToken((SyntaxBase*)p->argumentList);
                return SyntaxToken();
            }

            case SyntaxKind::MemberBindingExpression: {
                MemberBindingExpressionSyntax* p = (MemberBindingExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->name != nullptr) return GetFirstToken((SyntaxBase*)p->name);
                return SyntaxToken();
            }

            case SyntaxKind::ConditionalAccessExpression: {
                ConditionalAccessExpressionSyntax* p = (ConditionalAccessExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->whenNotNull != nullptr) return GetFirstToken((SyntaxBase*)p->whenNotNull);
                return SyntaxToken();
            }

            case SyntaxKind::SimpleMemberAccessExpression: {
                MemberAccessExpressionSyntax* p = (MemberAccessExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->name != nullptr) return GetFirstToken((SyntaxBase*)p->name);
                return SyntaxToken();
            }
            case SyntaxKind::PointerMemberAccessExpression: {
                MemberAccessExpressionSyntax* p = (MemberAccessExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->name != nullptr) return GetFirstToken((SyntaxBase*)p->name);
                return SyntaxToken();
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* p = (QualifiedNameSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->dotToken.IsValid()) return p->dotToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }

            case SyntaxKind::IdentifierName: {
                IdentifierNameSyntax* p = (IdentifierNameSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::NameColon: {
                NameColonSyntax* p = (NameColonSyntax*)syntaxBase;
                if(p->name != nullptr) return GetFirstToken((SyntaxBase*)p->name);
                if(p->colonToken.IsValid()) return p->colonToken;
                return SyntaxToken();
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                if(p->typeToken.IsValid()) return p->typeToken;
                return SyntaxToken();
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->elements != nullptr && p->elements->itemCount != 0) return GetFirstToken(p->elements->items[0]);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                if(p->refKeyword.IsValid()) return p->refKeyword;
                if(p->readonlyKeyword.IsValid()) return p->readonlyKeyword;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::NullableType: {
                NullableTypeSyntax* p = (NullableTypeSyntax*)syntaxBase;
                if(p->elementType != nullptr) return GetFirstToken((SyntaxBase*)p->elementType);
                if(p->questionMark.IsValid()) return p->questionMark;
                return SyntaxToken();
            }

            case SyntaxKind::LabeledStatement: {
                LabeledStatementSyntax* p = (LabeledStatementSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->colon.IsValid()) return p->colon;
                if(p->statement != nullptr) return GetFirstToken((SyntaxBase*)p->statement);
                return SyntaxToken();
            }

            case SyntaxKind::Argument: {
                ArgumentSyntax* p = (ArgumentSyntax*)syntaxBase;
                if(p->nameColon != nullptr) return GetFirstToken((SyntaxBase*)p->nameColon);
                if(p->refKindKeyword.IsValid()) return p->refKindKeyword;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::NameEquals: {
                NameEqualsSyntax* p = (NameEqualsSyntax*)syntaxBase;
                if(p->name != nullptr) return GetFirstToken((SyntaxBase*)p->name);
                if(p->equalsToken.IsValid()) return p->equalsToken;
                return SyntaxToken();
            }

            case SyntaxKind::ImplicitArrayCreationExpression: {
                ImplicitArrayCreationExpressionSyntax* p = (ImplicitArrayCreationExpressionSyntax*)syntaxBase;
                if(p->newKeyword.IsValid()) return p->newKeyword;
                if(p->openBracket.IsValid()) return p->openBracket;
                if(p->commas != nullptr && p->commas->size != 0) return p->commas->array[0];
                if(p->closeBracket.IsValid()) return p->closeBracket;
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                return SyntaxToken();
            }

            case SyntaxKind::ObjectInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::CollectionInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::ArrayInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::ComplexElementInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::WithInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::StackAllocArrayCreationExpression: {
                StackAllocArrayCreationExpressionSyntax* p = (StackAllocArrayCreationExpressionSyntax*)syntaxBase;
                if(p->stackallocKeyword.IsValid()) return p->stackallocKeyword;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                return SyntaxToken();
            }

            case SyntaxKind::ImplicitStackAllocArrayCreationExpression: {
                ImplicitStackAllocArrayCreationExpressionSyntax* p = (ImplicitStackAllocArrayCreationExpressionSyntax*)syntaxBase;
                if(p->stackallocKeyword.IsValid()) return p->stackallocKeyword;
                if(p->openBracket.IsValid()) return p->openBracket;
                if(p->closeBracket.IsValid()) return p->closeBracket;
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                return SyntaxToken();
            }

            case SyntaxKind::ArgumentList: {
                ArgumentListSyntax* p = (ArgumentListSyntax*)syntaxBase;
                if(p->openToken.IsValid()) return p->openToken;
                if(p->arguments != nullptr && p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
                if(p->closeToken.IsValid()) return p->closeToken;
                return SyntaxToken();
            }

            case SyntaxKind::ObjectCreationExpression: {
                ObjectCreationExpressionSyntax* p = (ObjectCreationExpressionSyntax*)syntaxBase;
                if(p->newKeyword.IsValid()) return p->newKeyword;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->arguments != nullptr) return GetFirstToken((SyntaxBase*)p->arguments);
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                return SyntaxToken();
            }

            case SyntaxKind::ImplicitObjectCreationExpression: {
                ImplicitObjectCreationExpressionSyntax* p = (ImplicitObjectCreationExpressionSyntax*)syntaxBase;
                if(p->newKeyword.IsValid()) return p->newKeyword;
                if(p->arguments != nullptr) return GetFirstToken((SyntaxBase*)p->arguments);
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                return SyntaxToken();
            }

            case SyntaxKind::AnonymousObjectMemberDeclarator: {
                AnonymousObjectMemberDeclaratorSyntax* p = (AnonymousObjectMemberDeclaratorSyntax*)syntaxBase;
                if(p->nameEquals != nullptr) return GetFirstToken((SyntaxBase*)p->nameEquals);
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::AnonymousObjectCreationExpression: {
                AnonymousObjectCreationExpressionSyntax* p = (AnonymousObjectCreationExpressionSyntax*)syntaxBase;
                if(p->newToken.IsValid()) return p->newToken;
                if(p->openBrace.IsValid()) return p->openBrace;
                if(p->initializers != nullptr && p->initializers->itemCount != 0) return GetFirstToken(p->initializers->items[0]);
                if(p->closeBrace.IsValid()) return p->closeBrace;
                return SyntaxToken();
            }

            case SyntaxKind::TupleExpression: {
                TupleExpressionSyntax* p = (TupleExpressionSyntax*)syntaxBase;
                if(p->openToken.IsValid()) return p->openToken;
                if(p->arguments != nullptr && p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
                if(p->closeToken.IsValid()) return p->closeToken;
                return SyntaxToken();
            }

            case SyntaxKind::ParenthesizedExpression: {
                ParenthesizedExpressionSyntax* p = (ParenthesizedExpressionSyntax*)syntaxBase;
                if(p->openToken.IsValid()) return p->openToken;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->closeToken.IsValid()) return p->closeToken;
                return SyntaxToken();
            }

            case SyntaxKind::BracketedArgumentList: {
                BracketedArgumentListSyntax* p = (BracketedArgumentListSyntax*)syntaxBase;
                if(p->openBracket.IsValid()) return p->openBracket;
                if(p->arguments != nullptr && p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
                if(p->closeBracket.IsValid()) return p->closeBracket;
                return SyntaxToken();
            }

            case SyntaxKind::EqualsValueClause: {
                EqualsValueClauseSyntax* p = (EqualsValueClauseSyntax*)syntaxBase;
                if(p->equalsToken.IsValid()) return p->equalsToken;
                if(p->value != nullptr) return GetFirstToken((SyntaxBase*)p->value);
                return SyntaxToken();
            }

            case SyntaxKind::RefExpression: {
                RefExpressionSyntax* p = (RefExpressionSyntax*)syntaxBase;
                if(p->refKeyword.IsValid()) return p->refKeyword;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclarator: {
                VariableDeclaratorSyntax* p = (VariableDeclaratorSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameter: {
                TypeParameterSyntax* p = (TypeParameterSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameterList: {
                TypeParameterListSyntax* p = (TypeParameterListSyntax*)syntaxBase;
                if(p->lessThanToken.IsValid()) return p->lessThanToken;
                if(p->parameters != nullptr && p->parameters->itemCount != 0) return GetFirstToken(p->parameters->items[0]);
                if(p->greaterThanToken.IsValid()) return p->greaterThanToken;
                return SyntaxToken();
            }

            case SyntaxKind::ArrowExpressionClause: {
                ArrowExpressionClauseSyntax* p = (ArrowExpressionClauseSyntax*)syntaxBase;
                if(p->arrowToken.IsValid()) return p->arrowToken;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::Block: {
                BlockSyntax* p = (BlockSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->statements != nullptr && p->statements->size != 0) return GetFirstToken(p->statements->array[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::DefaultLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::FalseLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::NullLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::NumericLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::EmptyStringLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::TrueLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }

            case SyntaxKind::CastExpression: {
                CastExpressionSyntax* p = (CastExpressionSyntax*)syntaxBase;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::BaseExpression: {
                BaseExpressionSyntax* p = (BaseExpressionSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::ThisExpression: {
                ThisExpressionSyntax* p = (ThisExpressionSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::DefaultExpression: {
                DefaultExpressionSyntax* p = (DefaultExpressionSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::TypeOfExpression: {
                TypeOfExpressionSyntax* p = (TypeOfExpressionSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::DiscardDesignation: {
                DiscardDesignationSyntax* p = (DiscardDesignationSyntax*)syntaxBase;
                if(p->underscoreToken.IsValid()) return p->underscoreToken;
                return SyntaxToken();
            }

            case SyntaxKind::SingleVariableDesignation: {
                SingleVariableDesignationSyntax* p = (SingleVariableDesignationSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::ParenthesizedVariableDesignation: {
                ParenthesizedVariableDesignationSyntax* p = (ParenthesizedVariableDesignationSyntax*)syntaxBase;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->designators != nullptr && p->designators->itemCount != 0) return GetFirstToken(p->designators->items[0]);
                if(p->closeParen.IsValid()) return p->closeParen;
                return SyntaxToken();
            }

            case SyntaxKind::ExpressionElement: {
                ExpressionElementSyntax* p = (ExpressionElementSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::SpreadElement: {
                SpreadElementSyntax* p = (SpreadElementSyntax*)syntaxBase;
                if(p->dotDotToken.IsValid()) return p->dotDotToken;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::CollectionExpression: {
                CollectionExpressionSyntax* p = (CollectionExpressionSyntax*)syntaxBase;
                if(p->open.IsValid()) return p->open;
                if(p->elements != nullptr && p->elements->itemCount != 0) return GetFirstToken(p->elements->items[0]);
                if(p->close.IsValid()) return p->close;
                return SyntaxToken();
            }

            case SyntaxKind::DeclarationExpression: {
                DeclarationExpressionSyntax* p = (DeclarationExpressionSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->designation != nullptr) return GetFirstToken((SyntaxBase*)p->designation);
                return SyntaxToken();
            }

            case SyntaxKind::ThrowExpression: {
                ThrowExpressionSyntax* p = (ThrowExpressionSyntax*)syntaxBase;
                if(p->throwKeyword.IsValid()) return p->throwKeyword;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::PostIncrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }
            case SyntaxKind::PostDecrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }

            case SyntaxKind::ElementAccessExpression: {
                ElementAccessExpressionSyntax* p = (ElementAccessExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->argumentList != nullptr) return GetFirstToken((SyntaxBase*)p->argumentList);
                return SyntaxToken();
            }

            case SyntaxKind::InvocationExpression: {
                InvocationExpressionSyntax* p = (InvocationExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->argumentList != nullptr) return GetFirstToken((SyntaxBase*)p->argumentList);
                return SyntaxToken();
            }

            case SyntaxKind::ConditionalExpression: {
                ConditionalExpressionSyntax* p = (ConditionalExpressionSyntax*)syntaxBase;
                if(p->condition != nullptr) return GetFirstToken((SyntaxBase*)p->condition);
                if(p->questionToken.IsValid()) return p->questionToken;
                if(p->whenTrue != nullptr) return GetFirstToken((SyntaxBase*)p->whenTrue);
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->whenFalse != nullptr) return GetFirstToken((SyntaxBase*)p->whenFalse);
                return SyntaxToken();
            }

            case SyntaxKind::RangeExpression: {
                RangeExpressionSyntax* p = (RangeExpressionSyntax*)syntaxBase;
                if(p->leftOperand != nullptr) return GetFirstToken((SyntaxBase*)p->leftOperand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->rightOperand != nullptr) return GetFirstToken((SyntaxBase*)p->rightOperand);
                return SyntaxToken();
            }

            case SyntaxKind::UnaryPlusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->operand != nullptr) return GetFirstToken((SyntaxBase*)p->operand);
                return SyntaxToken();
            }
            case SyntaxKind::UnaryMinusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->operand != nullptr) return GetFirstToken((SyntaxBase*)p->operand);
                return SyntaxToken();
            }
            case SyntaxKind::BitwiseNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->operand != nullptr) return GetFirstToken((SyntaxBase*)p->operand);
                return SyntaxToken();
            }
            case SyntaxKind::LogicalNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->operand != nullptr) return GetFirstToken((SyntaxBase*)p->operand);
                return SyntaxToken();
            }
            case SyntaxKind::PreIncrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->operand != nullptr) return GetFirstToken((SyntaxBase*)p->operand);
                return SyntaxToken();
            }
            case SyntaxKind::PreDecrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->operand != nullptr) return GetFirstToken((SyntaxBase*)p->operand);
                return SyntaxToken();
            }
            case SyntaxKind::IndexExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->operand != nullptr) return GetFirstToken((SyntaxBase*)p->operand);
                return SyntaxToken();
            }

            case SyntaxKind::Parameter: {
                ParameterSyntax* p = (ParameterSyntax*)syntaxBase;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->defaultValue != nullptr) return GetFirstToken((SyntaxBase*)p->defaultValue);
                return SyntaxToken();
            }

            case SyntaxKind::SimpleLambdaExpression: {
                SimpleLambdaExpressionSyntax* p = (SimpleLambdaExpressionSyntax*)syntaxBase;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->parameter != nullptr) return GetFirstToken((SyntaxBase*)p->parameter);
                if(p->arrowToken.IsValid()) return p->arrowToken;
                if(p->blockBody != nullptr) return GetFirstToken((SyntaxBase*)p->blockBody);
                if(p->expressionBody != nullptr) return GetFirstToken((SyntaxBase*)p->expressionBody);
                return SyntaxToken();
            }

            case SyntaxKind::ParenthesizedLambdaExpression: {
                ParenthesizedLambdaExpressionSyntax* p = (ParenthesizedLambdaExpressionSyntax*)syntaxBase;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->returnType != nullptr) return GetFirstToken((SyntaxBase*)p->returnType);
                if(p->parameters != nullptr) return GetFirstToken((SyntaxBase*)p->parameters);
                if(p->arrowToken.IsValid()) return p->arrowToken;
                if(p->blockBody != nullptr) return GetFirstToken((SyntaxBase*)p->blockBody);
                if(p->expressionBody != nullptr) return GetFirstToken((SyntaxBase*)p->expressionBody);
                return SyntaxToken();
            }

            case SyntaxKind::BaseConstructorInitializer: {
                BaseConstructorInitializerSyntax* p = (BaseConstructorInitializerSyntax*)syntaxBase;
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->baseKeyword.IsValid()) return p->baseKeyword;
                if(p->argumentListSyntax != nullptr) return GetFirstToken((SyntaxBase*)p->argumentListSyntax);
                return SyntaxToken();
            }

            case SyntaxKind::ThisConstructorInitializer: {
                ThisConstructorInitializerSyntax* p = (ThisConstructorInitializerSyntax*)syntaxBase;
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->thisKeyword.IsValid()) return p->thisKeyword;
                if(p->argumentListSyntax != nullptr) return GetFirstToken((SyntaxBase*)p->argumentListSyntax);
                return SyntaxToken();
            }

            case SyntaxKind::NamedConstructorInitializer: {
                NamedConstructorInitializerSyntax* p = (NamedConstructorInitializerSyntax*)syntaxBase;
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->name.IsValid()) return p->name;
                if(p->argumentListSyntax != nullptr) return GetFirstToken((SyntaxBase*)p->argumentListSyntax);
                return SyntaxToken();
            }

            case SyntaxKind::ParameterList: {
                ParameterListSyntax* p = (ParameterListSyntax*)syntaxBase;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->parameters != nullptr && p->parameters->itemCount != 0) return GetFirstToken(p->parameters->items[0]);
                if(p->closeParen.IsValid()) return p->closeParen;
                return SyntaxToken();
            }

            case SyntaxKind::BracketedParameterList: {
                BracketedParameterListSyntax* p = (BracketedParameterListSyntax*)syntaxBase;
                if(p->openBracket.IsValid()) return p->openBracket;
                if(p->parameters != nullptr && p->parameters->itemCount != 0) return GetFirstToken(p->parameters->items[0]);
                if(p->closeBracket.IsValid()) return p->closeBracket;
                return SyntaxToken();
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->returnType != nullptr) return GetFirstToken((SyntaxBase*)p->returnType);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeParameters != nullptr) return GetFirstToken((SyntaxBase*)p->typeParameters);
                if(p->parameters != nullptr) return GetFirstToken((SyntaxBase*)p->parameters);
                if(p->constraints != nullptr && p->constraints->size != 0) return GetFirstToken(p->constraints->array[0]);
                if(p->blockBody != nullptr) return GetFirstToken((SyntaxBase*)p->blockBody);
                if(p->arrowBody != nullptr) return GetFirstToken((SyntaxBase*)p->arrowBody);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* p = (VariableDeclarationSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->variables != nullptr && p->variables->itemCount != 0) return GetFirstToken(p->variables->items[0]);
                return SyntaxToken();
            }

            case SyntaxKind::LocalDeclarationStatement: {
                LocalDeclarationStatementSyntax* p = (LocalDeclarationStatementSyntax*)syntaxBase;
                if(p->usingKeyword.IsValid()) return p->usingKeyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->declaration != nullptr) return GetFirstToken((SyntaxBase*)p->declaration);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* p = (FieldDeclarationSyntax*)syntaxBase;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->declaration != nullptr) return GetFirstToken((SyntaxBase*)p->declaration);
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                return SyntaxToken();
            }

            case SyntaxKind::ExpressionColon: {
                ExpressionColonSyntax* p = (ExpressionColonSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->colonToken.IsValid()) return p->colonToken;
                return SyntaxToken();
            }

            case SyntaxKind::Subpattern: {
                SubpatternSyntax* p = (SubpatternSyntax*)syntaxBase;
                if(p->expressionColon != nullptr) return GetFirstToken((SyntaxBase*)p->expressionColon);
                if(p->pattern != nullptr) return GetFirstToken((SyntaxBase*)p->pattern);
                return SyntaxToken();
            }

            case SyntaxKind::PropertyPatternClause: {
                PropertyPatternClauseSyntax* p = (PropertyPatternClauseSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->subpatterns != nullptr && p->subpatterns->itemCount != 0) return GetFirstToken(p->subpatterns->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::DeclarationPattern: {
                DeclarationPatternSyntax* p = (DeclarationPatternSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->designation != nullptr) return GetFirstToken((SyntaxBase*)p->designation);
                return SyntaxToken();
            }

            case SyntaxKind::PositionalPatternClause: {
                PositionalPatternClauseSyntax* p = (PositionalPatternClauseSyntax*)syntaxBase;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->subpatterns != nullptr && p->subpatterns->itemCount != 0) return GetFirstToken(p->subpatterns->items[0]);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::RecursivePattern: {
                RecursivePatternSyntax* p = (RecursivePatternSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->positionalPatternClause != nullptr) return GetFirstToken((SyntaxBase*)p->positionalPatternClause);
                if(p->propertyPatternClause != nullptr) return GetFirstToken((SyntaxBase*)p->propertyPatternClause);
                if(p->designation != nullptr) return GetFirstToken((SyntaxBase*)p->designation);
                return SyntaxToken();
            }

            case SyntaxKind::ParenthesizedPattern: {
                ParenthesizedPatternSyntax* p = (ParenthesizedPatternSyntax*)syntaxBase;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->pattern != nullptr) return GetFirstToken((SyntaxBase*)p->pattern);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::VarPattern: {
                VarPatternSyntax* p = (VarPatternSyntax*)syntaxBase;
                if(p->varKeyword.IsValid()) return p->varKeyword;
                if(p->designation != nullptr) return GetFirstToken((SyntaxBase*)p->designation);
                return SyntaxToken();
            }

            case SyntaxKind::TypePattern: {
                TypePatternSyntax* p = (TypePatternSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::ConstantPattern: {
                ConstantPatternSyntax* p = (ConstantPatternSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::RelationalPattern: {
                RelationalPatternSyntax* p = (RelationalPatternSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::SlicePattern: {
                SlicePatternSyntax* p = (SlicePatternSyntax*)syntaxBase;
                if(p->dotDotToken.IsValid()) return p->dotDotToken;
                if(p->pattern != nullptr) return GetFirstToken((SyntaxBase*)p->pattern);
                return SyntaxToken();
            }

            case SyntaxKind::DiscardPattern: {
                DiscardPatternSyntax* p = (DiscardPatternSyntax*)syntaxBase;
                if(p->underscore.IsValid()) return p->underscore;
                return SyntaxToken();
            }

            case SyntaxKind::NotPattern: {
                UnaryPatternSyntax* p = (UnaryPatternSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->pattern != nullptr) return GetFirstToken((SyntaxBase*)p->pattern);
                return SyntaxToken();
            }

            case SyntaxKind::OrPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::AndPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }

            case SyntaxKind::IsPatternExpression: {
                IsPatternExpressionSyntax* p = (IsPatternExpressionSyntax*)syntaxBase;
                if(p->leftOperand != nullptr) return GetFirstToken((SyntaxBase*)p->leftOperand);
                if(p->opToken.IsValid()) return p->opToken;
                if(p->pattern != nullptr) return GetFirstToken((SyntaxBase*)p->pattern);
                return SyntaxToken();
            }

            case SyntaxKind::AddExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::SubtractExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::MultiplyExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::DivideExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::ModuloExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::LeftShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::RightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::UnsignedRightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::LogicalOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::LogicalAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::BitwiseOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::BitwiseAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::ExclusiveOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::EqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::NotEqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::LessThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::LessThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::GreaterThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::GreaterThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::IsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::AsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::CoalesceExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }

            case SyntaxKind::ImplicitElementAccess: {
                ImplicitElementAccessSyntax* p = (ImplicitElementAccessSyntax*)syntaxBase;
                if(p->argumentList != nullptr) return GetFirstToken((SyntaxBase*)p->argumentList);
                return SyntaxToken();
            }

            case SyntaxKind::WhenClause: {
                WhenClauseSyntax* p = (WhenClauseSyntax*)syntaxBase;
                if(p->whenKeyword.IsValid()) return p->whenKeyword;
                if(p->condition != nullptr) return GetFirstToken((SyntaxBase*)p->condition);
                return SyntaxToken();
            }

            case SyntaxKind::SwitchExpressionArm: {
                SwitchExpressionArmSyntax* p = (SwitchExpressionArmSyntax*)syntaxBase;
                if(p->pattern != nullptr) return GetFirstToken((SyntaxBase*)p->pattern);
                if(p->whenClause != nullptr) return GetFirstToken((SyntaxBase*)p->whenClause);
                if(p->equalsGreaterThanToken.IsValid()) return p->equalsGreaterThanToken;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::SwitchExpression: {
                SwitchExpressionSyntax* p = (SwitchExpressionSyntax*)syntaxBase;
                if(p->governingExpression != nullptr) return GetFirstToken((SyntaxBase*)p->governingExpression);
                if(p->switchKeyword.IsValid()) return p->switchKeyword;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->arms != nullptr && p->arms->itemCount != 0) return GetFirstToken(p->arms->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::ListPattern: {
                ListPatternSyntax* p = (ListPatternSyntax*)syntaxBase;
                if(p->openBracketToken.IsValid()) return p->openBracketToken;
                if(p->patterns != nullptr && p->patterns->itemCount != 0) return GetFirstToken(p->patterns->items[0]);
                if(p->closeBracketToken.IsValid()) return p->closeBracketToken;
                if(p->designation != nullptr) return GetFirstToken((SyntaxBase*)p->designation);
                return SyntaxToken();
            }

            case SyntaxKind::SimpleAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::AddAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::SubtractAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::MultiplyAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::DivideAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::ModuloAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::AndAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::ExclusiveOrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::OrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::LeftShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::RightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::UnsignedRightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }
            case SyntaxKind::CoalesceAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }

            case SyntaxKind::ForEachStatement: {
                ForEachStatementSyntax* p = (ForEachStatementSyntax*)syntaxBase;
                if(p->foreachKeyword.IsValid()) return p->foreachKeyword;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->inKeyword.IsValid()) return p->inKeyword;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->statement != nullptr) return GetFirstToken((SyntaxBase*)p->statement);
                return SyntaxToken();
            }

            case SyntaxKind::ForEachVariableStatement: {
                ForEachVariableStatementSyntax* p = (ForEachVariableStatementSyntax*)syntaxBase;
                if(p->foreachKeyword.IsValid()) return p->foreachKeyword;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->variable != nullptr) return GetFirstToken((SyntaxBase*)p->variable);
                if(p->inKeyword.IsValid()) return p->inKeyword;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->statement != nullptr) return GetFirstToken((SyntaxBase*)p->statement);
                return SyntaxToken();
            }

            case SyntaxKind::GotoCaseStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                if(p->gotoToken.IsValid()) return p->gotoToken;
                if(p->caseOrDefault.IsValid()) return p->caseOrDefault;
                if(p->arg != nullptr) return GetFirstToken((SyntaxBase*)p->arg);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }
            case SyntaxKind::GotoDefaultStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                if(p->gotoToken.IsValid()) return p->gotoToken;
                if(p->caseOrDefault.IsValid()) return p->caseOrDefault;
                if(p->arg != nullptr) return GetFirstToken((SyntaxBase*)p->arg);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }
            case SyntaxKind::GotoStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                if(p->gotoToken.IsValid()) return p->gotoToken;
                if(p->caseOrDefault.IsValid()) return p->caseOrDefault;
                if(p->arg != nullptr) return GetFirstToken((SyntaxBase*)p->arg);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::ElseClause: {
                ElseClauseSyntax* p = (ElseClauseSyntax*)syntaxBase;
                if(p->elseKeyword.IsValid()) return p->elseKeyword;
                if(p->statement != nullptr) return GetFirstToken((SyntaxBase*)p->statement);
                return SyntaxToken();
            }

            case SyntaxKind::IfStatement: {
                IfStatementSyntax* p = (IfStatementSyntax*)syntaxBase;
                if(p->ifKeyword.IsValid()) return p->ifKeyword;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->condition != nullptr) return GetFirstToken((SyntaxBase*)p->condition);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->statement != nullptr) return GetFirstToken((SyntaxBase*)p->statement);
                if(p->elseClause != nullptr) return GetFirstToken((SyntaxBase*)p->elseClause);
                return SyntaxToken();
            }

            case SyntaxKind::ExpressionStatement: {
                ExpressionStatementSyntax* p = (ExpressionStatementSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::ReturnStatement: {
                ReturnStatementSyntax* p = (ReturnStatementSyntax*)syntaxBase;
                if(p->returnKeyword.IsValid()) return p->returnKeyword;
                if(p->expressionSyntax != nullptr) return GetFirstToken((SyntaxBase*)p->expressionSyntax);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::BaseList: {
                BaseListSyntax* p = (BaseListSyntax*)syntaxBase;
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->types != nullptr && p->types->itemCount != 0) return GetFirstToken(p->types->items[0]);
                return SyntaxToken();
            }

            case SyntaxKind::Attribute: {
                AttributeSyntax* p = (AttributeSyntax*)syntaxBase;
                if(p->name != nullptr) return GetFirstToken((SyntaxBase*)p->name);
                if(p->argumentList != nullptr) return GetFirstToken((SyntaxBase*)p->argumentList);
                return SyntaxToken();
            }

            case SyntaxKind::AttributeList: {
                AttributeListSyntax* p = (AttributeListSyntax*)syntaxBase;
                if(p->openBracket.IsValid()) return p->openBracket;
                if(p->attributes != nullptr && p->attributes->itemCount != 0) return GetFirstToken(p->attributes->items[0]);
                if(p->closeBracket.IsValid()) return p->closeBracket;
                return SyntaxToken();
            }

            case SyntaxKind::TypeConstraint: {
                TypeConstraintSyntax* p = (TypeConstraintSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::ConstructorConstraint: {
                ConstructorConstraintSyntax* p = (ConstructorConstraintSyntax*)syntaxBase;
                if(p->newKeyword.IsValid()) return p->newKeyword;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->closeParen.IsValid()) return p->closeParen;
                return SyntaxToken();
            }

            case SyntaxKind::ClassConstraint: {
                ClassOrStructConstraintSyntax* p = (ClassOrStructConstraintSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->questionToken.IsValid()) return p->questionToken;
                return SyntaxToken();
            }
            case SyntaxKind::StructConstraint: {
                ClassOrStructConstraintSyntax* p = (ClassOrStructConstraintSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->questionToken.IsValid()) return p->questionToken;
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameterConstraintClause: {
                TypeParameterConstraintClauseSyntax* p = (TypeParameterConstraintClauseSyntax*)syntaxBase;
                if(p->whereKeyword.IsValid()) return p->whereKeyword;
                if(p->name != nullptr) return GetFirstToken((SyntaxBase*)p->name);
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->constraints != nullptr && p->constraints->itemCount != 0) return GetFirstToken(p->constraints->items[0]);
                return SyntaxToken();
            }

            case SyntaxKind::StructDeclaration: {
                StructDeclarationSyntax* p = (StructDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->keyword.IsValid()) return p->keyword;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeParameterList != nullptr) return GetFirstToken((SyntaxBase*)p->typeParameterList);
                if(p->parameterList != nullptr) return GetFirstToken((SyntaxBase*)p->parameterList);
                if(p->baseList != nullptr) return GetFirstToken((SyntaxBase*)p->baseList);
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetFirstToken(p->constraintClauses->array[0]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->members != nullptr && p->members->size != 0) return GetFirstToken(p->members->array[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                return SyntaxToken();
            }

            case SyntaxKind::EnumMemberDeclaration: {
                EnumMemberDeclarationSyntax* p = (EnumMemberDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->equalsValue != nullptr) return GetFirstToken((SyntaxBase*)p->equalsValue);
                return SyntaxToken();
            }

            case SyntaxKind::EnumDeclaration: {
                EnumDeclarationSyntax* p = (EnumDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->keyword.IsValid()) return p->keyword;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->baseList != nullptr) return GetFirstToken((SyntaxBase*)p->baseList);
                if(p->openBrace.IsValid()) return p->openBrace;
                if(p->members != nullptr && p->members->itemCount != 0) return GetFirstToken(p->members->items[0]);
                if(p->closeBrace.IsValid()) return p->closeBrace;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                return SyntaxToken();
            }

            case SyntaxKind::DelegateDeclaration: {
                DelegateDeclarationSyntax* p = (DelegateDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->keyword.IsValid()) return p->keyword;
                if(p->returnType != nullptr) return GetFirstToken((SyntaxBase*)p->returnType);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeParameterList != nullptr) return GetFirstToken((SyntaxBase*)p->typeParameterList);
                if(p->parameterList != nullptr) return GetFirstToken((SyntaxBase*)p->parameterList);
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetFirstToken(p->constraintClauses->array[0]);
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                return SyntaxToken();
            }

            case SyntaxKind::ClassDeclaration: {
                ClassDeclarationSyntax* p = (ClassDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->keyword.IsValid()) return p->keyword;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeParameterList != nullptr) return GetFirstToken((SyntaxBase*)p->typeParameterList);
                if(p->parameterList != nullptr) return GetFirstToken((SyntaxBase*)p->parameterList);
                if(p->baseList != nullptr) return GetFirstToken((SyntaxBase*)p->baseList);
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetFirstToken(p->constraintClauses->array[0]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->members != nullptr && p->members->size != 0) return GetFirstToken(p->members->array[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                return SyntaxToken();
            }

            case SyntaxKind::NamespaceDeclaration: {
                NamespaceDeclarationSyntax* p = (NamespaceDeclarationSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->names != nullptr && p->names->itemCount != 0) return GetFirstToken(p->names->items[0]);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::InterfaceDeclaration: {
                InterfaceDeclarationSyntax* p = (InterfaceDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->keyword.IsValid()) return p->keyword;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeParameterList != nullptr) return GetFirstToken((SyntaxBase*)p->typeParameterList);
                if(p->parameterList != nullptr) return GetFirstToken((SyntaxBase*)p->parameterList);
                if(p->baseList != nullptr) return GetFirstToken((SyntaxBase*)p->baseList);
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetFirstToken(p->constraintClauses->array[0]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->members != nullptr && p->members->size != 0) return GetFirstToken(p->members->array[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                return SyntaxToken();
            }

            case SyntaxKind::ConstructorDeclaration: {
                ConstructorDeclarationSyntax* p = (ConstructorDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->identifier.IsValid()) return p->identifier;
                if(p->parameterList != nullptr) return GetFirstToken((SyntaxBase*)p->parameterList);
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                if(p->bodyBlock != nullptr) return GetFirstToken((SyntaxBase*)p->bodyBlock);
                if(p->bodyExpression != nullptr) return GetFirstToken((SyntaxBase*)p->bodyExpression);
                if(p->semiColon.IsValid()) return p->semiColon;
                return SyntaxToken();
            }

            case SyntaxKind::BaseType: {
                BaseTypeSyntax* p = (BaseTypeSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->argumentList != nullptr) return GetFirstToken((SyntaxBase*)p->argumentList);
                return SyntaxToken();
            }

            case SyntaxKind::StringLiteralExpression: {
                StringLiteralExpression* p = (StringLiteralExpression*)syntaxBase;
                if(p->start.IsValid()) return p->start;
                if(p->parts != nullptr && p->parts->size != 0) return GetFirstToken(p->parts->array[0]);
                if(p->end.IsValid()) return p->end;
                return SyntaxToken();
            }

            case SyntaxKind::RawStringLiteralExpression: {
                RawStringLiteralExpression* p = (RawStringLiteralExpression*)syntaxBase;
                if(p->start.IsValid()) return p->start;
                if(p->parts != nullptr && p->parts->size != 0) return GetFirstToken(p->parts->array[0]);
                if(p->end.IsValid()) return p->end;
                return SyntaxToken();
            }

            case SyntaxKind::InterpolatedIdentifierPart: {
                InterpolatedIdentifierPartSyntax* p = (InterpolatedIdentifierPartSyntax*)syntaxBase;
                if(p->interpolatedIdentifier.IsValid()) return p->interpolatedIdentifier;
                return SyntaxToken();
            }

            case SyntaxKind::InterpolatedStringExpression: {
                InterpolatedStringExpressionSyntax* p = (InterpolatedStringExpressionSyntax*)syntaxBase;
                if(p->start.IsValid()) return p->start;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                if(p->end.IsValid()) return p->end;
                return SyntaxToken();
            }

            case SyntaxKind::StringLiteralPart: {
                StringLiteralPartSyntax* p = (StringLiteralPartSyntax*)syntaxBase;
                if(p->part.IsValid()) return p->part;
                return SyntaxToken();
            }

            case SyntaxKind::CharacterLiteralExpression: {
                CharacterLiteralExpressionSyntax* p = (CharacterLiteralExpressionSyntax*)syntaxBase;
                if(p->start.IsValid()) return p->start;
                if(p->contents.IsValid()) return p->contents;
                if(p->end.IsValid()) return p->end;
                return SyntaxToken();
            }

            case SyntaxKind::IncompleteMember: {
                IncompleteMemberSyntax* p = (IncompleteMemberSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::GetAccessorDeclaration: {
                AccessorDeclarationSyntax* p = (AccessorDeclarationSyntax*)syntaxBase;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->keyword.IsValid()) return p->keyword;
                if(p->bodyBlock != nullptr) return GetFirstToken((SyntaxBase*)p->bodyBlock);
                if(p->expressionBody != nullptr) return GetFirstToken((SyntaxBase*)p->expressionBody);
                if(p->semiColon.IsValid()) return p->semiColon;
                return SyntaxToken();
            }
            case SyntaxKind::SetAccessorDeclaration: {
                AccessorDeclarationSyntax* p = (AccessorDeclarationSyntax*)syntaxBase;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->keyword.IsValid()) return p->keyword;
                if(p->bodyBlock != nullptr) return GetFirstToken((SyntaxBase*)p->bodyBlock);
                if(p->expressionBody != nullptr) return GetFirstToken((SyntaxBase*)p->expressionBody);
                if(p->semiColon.IsValid()) return p->semiColon;
                return SyntaxToken();
            }
            case SyntaxKind::InitAccessorDeclaration: {
                AccessorDeclarationSyntax* p = (AccessorDeclarationSyntax*)syntaxBase;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->keyword.IsValid()) return p->keyword;
                if(p->bodyBlock != nullptr) return GetFirstToken((SyntaxBase*)p->bodyBlock);
                if(p->expressionBody != nullptr) return GetFirstToken((SyntaxBase*)p->expressionBody);
                if(p->semiColon.IsValid()) return p->semiColon;
                return SyntaxToken();
            }

            case SyntaxKind::AccessorList: {
                AccessorListSyntax* p = (AccessorListSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->accessors != nullptr && p->accessors->size != 0) return GetFirstToken(p->accessors->array[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::IndexerDeclaration: {
                IndexerDeclarationSyntax* p = (IndexerDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->thisKeyword.IsValid()) return p->thisKeyword;
                if(p->parameters != nullptr) return GetFirstToken((SyntaxBase*)p->parameters);
                if(p->accessorList != nullptr) return GetFirstToken((SyntaxBase*)p->accessorList);
                if(p->expressionBody != nullptr) return GetFirstToken((SyntaxBase*)p->expressionBody);
                if(p->semiColon.IsValid()) return p->semiColon;
                return SyntaxToken();
            }

            case SyntaxKind::PropertyDeclaration: {
                PropertyDeclarationSyntax* p = (PropertyDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->accessorList != nullptr) return GetFirstToken((SyntaxBase*)p->accessorList);
                if(p->expressionBody != nullptr) return GetFirstToken((SyntaxBase*)p->expressionBody);
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                if(p->semiColon.IsValid()) return p->semiColon;
                return SyntaxToken();
            }

            case SyntaxKind::MethodDeclaration: {
                MethodDeclarationSyntax* p = (MethodDeclarationSyntax*)syntaxBase;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetFirstToken(p->attributes->array[0]);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->returnType != nullptr) return GetFirstToken((SyntaxBase*)p->returnType);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeParameterList != nullptr) return GetFirstToken((SyntaxBase*)p->typeParameterList);
                if(p->parameterList != nullptr) return GetFirstToken((SyntaxBase*)p->parameterList);
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetFirstToken(p->constraintClauses->array[0]);
                if(p->body != nullptr) return GetFirstToken((SyntaxBase*)p->body);
                if(p->expressionBody != nullptr) return GetFirstToken((SyntaxBase*)p->expressionBody);
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                return SyntaxToken();
            }

            case SyntaxKind::UsingNamespaceDeclaration: {
                UsingNamespaceDeclarationSyntax* p = (UsingNamespaceDeclarationSyntax*)syntaxBase;
                if(p->usingKeyword.IsValid()) return p->usingKeyword;
                if(p->namePath != nullptr && p->namePath->itemCount != 0) return GetFirstToken(p->namePath->items[0]);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::UsingDeclaration: {
                UsingDeclarationSyntax* p = (UsingDeclarationSyntax*)syntaxBase;
                if(p->usingKeyword.IsValid()) return p->usingKeyword;
                if(p->staticKeyword.IsValid()) return p->staticKeyword;
                if(p->alias != nullptr) return GetFirstToken((SyntaxBase*)p->alias);
                if(p->namespaceOrType != nullptr) return GetFirstToken((SyntaxBase*)p->namespaceOrType);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::ExternDeclaration: {
                ExternDeclarationSyntax* p = (ExternDeclarationSyntax*)syntaxBase;
                if(p->externKeyword.IsValid()) return p->externKeyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->returnType != nullptr) return GetFirstToken((SyntaxBase*)p->returnType);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->parameterList != nullptr) return GetFirstToken((SyntaxBase*)p->parameterList);
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::CompilationUnit: {
                CompilationUnitSyntax* p = (CompilationUnitSyntax*)syntaxBase;
                if(p->members != nullptr && p->members->size != 0) return GetFirstToken(p->members->array[0]);
                if(p->eof.IsValid()) return p->eof;
                return SyntaxToken();
            }

            default: {
                return SyntaxToken();
            }
            
        }        
        
    }
    
    SyntaxToken GetLastToken(SyntaxBase * syntaxBase) {
    
        if(syntaxBase == nullptr) {
            return SyntaxToken();
        }
        
        switch(syntaxBase->GetKind()) {
            case SyntaxKind::EmptyStatement: {
                EmptyStatementSyntax* p = (EmptyStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                return SyntaxToken();
            }

            case SyntaxKind::BreakStatement: {
                BreakStatementSyntax* p = (BreakStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->breakKeyword.IsValid()) return p->breakKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ContinueStatement: {
                ContinueStatementSyntax* p = (ContinueStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->continueKeyword.IsValid()) return p->continueKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ForStatement: {
                ForStatementSyntax* p = (ForStatementSyntax*)syntaxBase;
                if(p->statement != nullptr) return GetLastToken((SyntaxBase*)p->statement);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->incrementors != nullptr && p->incrementors->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->incrementors->items[p->incrementors->itemCount - 1]);
                    SyntaxToken b = p->incrementors->separatorCount == 0 ? SyntaxToken() : p->incrementors->separators[p->incrementors->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->secondSemiColon.IsValid()) return p->secondSemiColon;
                if(p->condition != nullptr) return GetLastToken((SyntaxBase*)p->condition);
                if(p->firstSemiColon.IsValid()) return p->firstSemiColon;
                if(p->initializers != nullptr && p->initializers->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->initializers->items[p->initializers->itemCount - 1]);
                    SyntaxToken b = p->initializers->separatorCount == 0 ? SyntaxToken() : p->initializers->separators[p->initializers->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->declaration != nullptr) return GetLastToken((SyntaxBase*)p->declaration);
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->forKeyword.IsValid()) return p->forKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ThrowStatement: {
                ThrowStatementSyntax* p = (ThrowStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->throwKeyword.IsValid()) return p->throwKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::CatchDeclaration: {
                CatchDeclarationSyntax* p = (CatchDeclarationSyntax*)syntaxBase;
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->openParen.IsValid()) return p->openParen;
                return SyntaxToken();
            }

            case SyntaxKind::CatchFilterClause: {
                CatchFilterClauseSyntax* p = (CatchFilterClauseSyntax*)syntaxBase;
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->filterExpression != nullptr) return GetLastToken((SyntaxBase*)p->filterExpression);
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->whenKeyword.IsValid()) return p->whenKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::CatchClause: {
                CatchClauseSyntax* p = (CatchClauseSyntax*)syntaxBase;
                if(p->block != nullptr) return GetLastToken((SyntaxBase*)p->block);
                if(p->filter != nullptr) return GetLastToken((SyntaxBase*)p->filter);
                if(p->declaration != nullptr) return GetLastToken((SyntaxBase*)p->declaration);
                if(p->catchKeyword.IsValid()) return p->catchKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::FinallyClause: {
                FinallyClauseSyntax* p = (FinallyClauseSyntax*)syntaxBase;
                if(p->block != nullptr) return GetLastToken((SyntaxBase*)p->block);
                if(p->finallyKeyword.IsValid()) return p->finallyKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::TryStatement: {
                TryStatementSyntax* p = (TryStatementSyntax*)syntaxBase;
                if(p->finallyClaus != nullptr) return GetLastToken((SyntaxBase*)p->finallyClaus);
                if(p->catchClauses != nullptr && p->catchClauses->size != 0) return GetLastToken(p->catchClauses->array[p->catchClauses->size - 1]);
                if(p->tryBlock != nullptr) return GetLastToken((SyntaxBase*)p->tryBlock);
                if(p->tryKeyword.IsValid()) return p->tryKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::DefaultSwitchLabel: {
                DefaultSwitchLabelSyntax* p = (DefaultSwitchLabelSyntax*)syntaxBase;
                if(p->colon.IsValid()) return p->colon;
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::CaseSwitchLabel: {
                CaseSwitchLabelSyntax* p = (CaseSwitchLabelSyntax*)syntaxBase;
                if(p->colon.IsValid()) return p->colon;
                if(p->value != nullptr) return GetLastToken((SyntaxBase*)p->value);
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::CasePatternSwitchLabel: {
                CasePatternSwitchLabelSyntax* p = (CasePatternSwitchLabelSyntax*)syntaxBase;
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->whenClause != nullptr) return GetLastToken((SyntaxBase*)p->whenClause);
                if(p->pattern != nullptr) return GetLastToken((SyntaxBase*)p->pattern);
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::SwitchSection: {
                SwitchSectionSyntax* p = (SwitchSectionSyntax*)syntaxBase;
                if(p->statements != nullptr && p->statements->size != 0) return GetLastToken(p->statements->array[p->statements->size - 1]);
                if(p->labels != nullptr && p->labels->size != 0) return GetLastToken(p->labels->array[p->labels->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::SwitchStatement: {
                SwitchStatementSyntax* p = (SwitchStatementSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->sections != nullptr && p->sections->size != 0) return GetLastToken(p->sections->array[p->sections->size - 1]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->switchKeyword.IsValid()) return p->switchKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::UsingStatement: {
                UsingStatementSyntax* p = (UsingStatementSyntax*)syntaxBase;
                if(p->statement != nullptr) return GetLastToken((SyntaxBase*)p->statement);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->declaration != nullptr) return GetLastToken((SyntaxBase*)p->declaration);
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->usingKeyword.IsValid()) return p->usingKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::WhileStatement: {
                WhileStatementSyntax* p = (WhileStatementSyntax*)syntaxBase;
                if(p->statement != nullptr) return GetLastToken((SyntaxBase*)p->statement);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->condition != nullptr) return GetLastToken((SyntaxBase*)p->condition);
                if(p->openParen.IsValid()) return p->openParen;
                if(p->whileKeyword.IsValid()) return p->whileKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::DoStatement: {
                DoStatementSyntax* p = (DoStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->condition != nullptr) return GetLastToken((SyntaxBase*)p->condition);
                if(p->openParen.IsValid()) return p->openParen;
                if(p->whileKeyword.IsValid()) return p->whileKeyword;
                if(p->statement != nullptr) return GetLastToken((SyntaxBase*)p->statement);
                if(p->doKeyword.IsValid()) return p->doKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*)syntaxBase;
                if(p->close.IsValid()) return p->close;
                if(p->ranks != nullptr && p->ranks->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->ranks->items[p->ranks->itemCount - 1]);
                    SyntaxToken b = p->ranks->separatorCount == 0 ? SyntaxToken() : p->ranks->separators[p->ranks->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->open.IsValid()) return p->open;
                return SyntaxToken();
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* p = (TypeArgumentListSyntax*)syntaxBase;
                if(p->greaterThanToken.IsValid()) return p->greaterThanToken;
                if(p->arguments != nullptr && p->arguments->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->arguments->items[p->arguments->itemCount - 1]);
                    SyntaxToken b = p->arguments->separatorCount == 0 ? SyntaxToken() : p->arguments->separators[p->arguments->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->lessThanToken.IsValid()) return p->lessThanToken;
                return SyntaxToken();
            }

            case SyntaxKind::GenericName: {
                GenericNameSyntax* p = (GenericNameSyntax*)syntaxBase;
                if(p->typeArgumentList != nullptr) return GetLastToken((SyntaxBase*)p->typeArgumentList);
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::ElementBindingExpression: {
                ElementBindingExpressionSyntax* p = (ElementBindingExpressionSyntax*)syntaxBase;
                if(p->argumentList != nullptr) return GetLastToken((SyntaxBase*)p->argumentList);
                return SyntaxToken();
            }

            case SyntaxKind::MemberBindingExpression: {
                MemberBindingExpressionSyntax* p = (MemberBindingExpressionSyntax*)syntaxBase;
                if(p->name != nullptr) return GetLastToken((SyntaxBase*)p->name);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }

            case SyntaxKind::ConditionalAccessExpression: {
                ConditionalAccessExpressionSyntax* p = (ConditionalAccessExpressionSyntax*)syntaxBase;
                if(p->whenNotNull != nullptr) return GetLastToken((SyntaxBase*)p->whenNotNull);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::SimpleMemberAccessExpression: {
                MemberAccessExpressionSyntax* p = (MemberAccessExpressionSyntax*)syntaxBase;
                if(p->name != nullptr) return GetLastToken((SyntaxBase*)p->name);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }
            case SyntaxKind::PointerMemberAccessExpression: {
                MemberAccessExpressionSyntax* p = (MemberAccessExpressionSyntax*)syntaxBase;
                if(p->name != nullptr) return GetLastToken((SyntaxBase*)p->name);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* p = (QualifiedNameSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->dotToken.IsValid()) return p->dotToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }

            case SyntaxKind::IdentifierName: {
                IdentifierNameSyntax* p = (IdentifierNameSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::NameColon: {
                NameColonSyntax* p = (NameColonSyntax*)syntaxBase;
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->name != nullptr) return GetLastToken((SyntaxBase*)p->name);
                return SyntaxToken();
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                if(p->typeToken.IsValid()) return p->typeToken;
                return SyntaxToken();
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->elements != nullptr && p->elements->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->elements->items[p->elements->itemCount - 1]);
                    SyntaxToken b = p->elements->separatorCount == 0 ? SyntaxToken() : p->elements->separators[p->elements->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openParenToken.IsValid()) return p->openParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->readonlyKeyword.IsValid()) return p->readonlyKeyword;
                if(p->refKeyword.IsValid()) return p->refKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::NullableType: {
                NullableTypeSyntax* p = (NullableTypeSyntax*)syntaxBase;
                if(p->questionMark.IsValid()) return p->questionMark;
                if(p->elementType != nullptr) return GetLastToken((SyntaxBase*)p->elementType);
                return SyntaxToken();
            }

            case SyntaxKind::LabeledStatement: {
                LabeledStatementSyntax* p = (LabeledStatementSyntax*)syntaxBase;
                if(p->statement != nullptr) return GetLastToken((SyntaxBase*)p->statement);
                if(p->colon.IsValid()) return p->colon;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::Argument: {
                ArgumentSyntax* p = (ArgumentSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->refKindKeyword.IsValid()) return p->refKindKeyword;
                if(p->nameColon != nullptr) return GetLastToken((SyntaxBase*)p->nameColon);
                return SyntaxToken();
            }

            case SyntaxKind::NameEquals: {
                NameEqualsSyntax* p = (NameEqualsSyntax*)syntaxBase;
                if(p->equalsToken.IsValid()) return p->equalsToken;
                if(p->name != nullptr) return GetLastToken((SyntaxBase*)p->name);
                return SyntaxToken();
            }

            case SyntaxKind::ImplicitArrayCreationExpression: {
                ImplicitArrayCreationExpressionSyntax* p = (ImplicitArrayCreationExpressionSyntax*)syntaxBase;
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->closeBracket.IsValid()) return p->closeBracket;
                if(p->commas != nullptr && p->commas->size != 0) return p->commas->array[p->commas->size - 1];
                if(p->openBracket.IsValid()) return p->openBracket;
                if(p->newKeyword.IsValid()) return p->newKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ObjectInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->list->items[p->list->itemCount - 1]);
                    SyntaxToken b = p->list->separatorCount == 0 ? SyntaxToken() : p->list->separators[p->list->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::CollectionInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->list->items[p->list->itemCount - 1]);
                    SyntaxToken b = p->list->separatorCount == 0 ? SyntaxToken() : p->list->separators[p->list->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::ArrayInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->list->items[p->list->itemCount - 1]);
                    SyntaxToken b = p->list->separatorCount == 0 ? SyntaxToken() : p->list->separators[p->list->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::ComplexElementInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->list->items[p->list->itemCount - 1]);
                    SyntaxToken b = p->list->separatorCount == 0 ? SyntaxToken() : p->list->separators[p->list->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::WithInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->list != nullptr && p->list->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->list->items[p->list->itemCount - 1]);
                    SyntaxToken b = p->list->separatorCount == 0 ? SyntaxToken() : p->list->separators[p->list->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::StackAllocArrayCreationExpression: {
                StackAllocArrayCreationExpressionSyntax* p = (StackAllocArrayCreationExpressionSyntax*)syntaxBase;
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->stackallocKeyword.IsValid()) return p->stackallocKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ImplicitStackAllocArrayCreationExpression: {
                ImplicitStackAllocArrayCreationExpressionSyntax* p = (ImplicitStackAllocArrayCreationExpressionSyntax*)syntaxBase;
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->closeBracket.IsValid()) return p->closeBracket;
                if(p->openBracket.IsValid()) return p->openBracket;
                if(p->stackallocKeyword.IsValid()) return p->stackallocKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ArgumentList: {
                ArgumentListSyntax* p = (ArgumentListSyntax*)syntaxBase;
                if(p->closeToken.IsValid()) return p->closeToken;
                if(p->arguments != nullptr && p->arguments->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->arguments->items[p->arguments->itemCount - 1]);
                    SyntaxToken b = p->arguments->separatorCount == 0 ? SyntaxToken() : p->arguments->separators[p->arguments->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openToken.IsValid()) return p->openToken;
                return SyntaxToken();
            }

            case SyntaxKind::ObjectCreationExpression: {
                ObjectCreationExpressionSyntax* p = (ObjectCreationExpressionSyntax*)syntaxBase;
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->arguments != nullptr) return GetLastToken((SyntaxBase*)p->arguments);
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->newKeyword.IsValid()) return p->newKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ImplicitObjectCreationExpression: {
                ImplicitObjectCreationExpressionSyntax* p = (ImplicitObjectCreationExpressionSyntax*)syntaxBase;
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->arguments != nullptr) return GetLastToken((SyntaxBase*)p->arguments);
                if(p->newKeyword.IsValid()) return p->newKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::AnonymousObjectMemberDeclarator: {
                AnonymousObjectMemberDeclaratorSyntax* p = (AnonymousObjectMemberDeclaratorSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->nameEquals != nullptr) return GetLastToken((SyntaxBase*)p->nameEquals);
                return SyntaxToken();
            }

            case SyntaxKind::AnonymousObjectCreationExpression: {
                AnonymousObjectCreationExpressionSyntax* p = (AnonymousObjectCreationExpressionSyntax*)syntaxBase;
                if(p->closeBrace.IsValid()) return p->closeBrace;
                if(p->initializers != nullptr && p->initializers->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->initializers->items[p->initializers->itemCount - 1]);
                    SyntaxToken b = p->initializers->separatorCount == 0 ? SyntaxToken() : p->initializers->separators[p->initializers->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBrace.IsValid()) return p->openBrace;
                if(p->newToken.IsValid()) return p->newToken;
                return SyntaxToken();
            }

            case SyntaxKind::TupleExpression: {
                TupleExpressionSyntax* p = (TupleExpressionSyntax*)syntaxBase;
                if(p->closeToken.IsValid()) return p->closeToken;
                if(p->arguments != nullptr && p->arguments->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->arguments->items[p->arguments->itemCount - 1]);
                    SyntaxToken b = p->arguments->separatorCount == 0 ? SyntaxToken() : p->arguments->separators[p->arguments->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openToken.IsValid()) return p->openToken;
                return SyntaxToken();
            }

            case SyntaxKind::ParenthesizedExpression: {
                ParenthesizedExpressionSyntax* p = (ParenthesizedExpressionSyntax*)syntaxBase;
                if(p->closeToken.IsValid()) return p->closeToken;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->openToken.IsValid()) return p->openToken;
                return SyntaxToken();
            }

            case SyntaxKind::BracketedArgumentList: {
                BracketedArgumentListSyntax* p = (BracketedArgumentListSyntax*)syntaxBase;
                if(p->closeBracket.IsValid()) return p->closeBracket;
                if(p->arguments != nullptr && p->arguments->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->arguments->items[p->arguments->itemCount - 1]);
                    SyntaxToken b = p->arguments->separatorCount == 0 ? SyntaxToken() : p->arguments->separators[p->arguments->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBracket.IsValid()) return p->openBracket;
                return SyntaxToken();
            }

            case SyntaxKind::EqualsValueClause: {
                EqualsValueClauseSyntax* p = (EqualsValueClauseSyntax*)syntaxBase;
                if(p->value != nullptr) return GetLastToken((SyntaxBase*)p->value);
                if(p->equalsToken.IsValid()) return p->equalsToken;
                return SyntaxToken();
            }

            case SyntaxKind::RefExpression: {
                RefExpressionSyntax* p = (RefExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->refKeyword.IsValid()) return p->refKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclarator: {
                VariableDeclaratorSyntax* p = (VariableDeclaratorSyntax*)syntaxBase;
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameter: {
                TypeParameterSyntax* p = (TypeParameterSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameterList: {
                TypeParameterListSyntax* p = (TypeParameterListSyntax*)syntaxBase;
                if(p->greaterThanToken.IsValid()) return p->greaterThanToken;
                if(p->parameters != nullptr && p->parameters->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->parameters->items[p->parameters->itemCount - 1]);
                    SyntaxToken b = p->parameters->separatorCount == 0 ? SyntaxToken() : p->parameters->separators[p->parameters->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->lessThanToken.IsValid()) return p->lessThanToken;
                return SyntaxToken();
            }

            case SyntaxKind::ArrowExpressionClause: {
                ArrowExpressionClauseSyntax* p = (ArrowExpressionClauseSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->arrowToken.IsValid()) return p->arrowToken;
                return SyntaxToken();
            }

            case SyntaxKind::Block: {
                BlockSyntax* p = (BlockSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->statements != nullptr && p->statements->size != 0) return GetLastToken(p->statements->array[p->statements->size - 1]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::DefaultLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::FalseLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::NullLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::NumericLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::EmptyStringLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }
            case SyntaxKind::TrueLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
                return SyntaxToken();
            }

            case SyntaxKind::CastExpression: {
                CastExpressionSyntax* p = (CastExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->openParen.IsValid()) return p->openParen;
                return SyntaxToken();
            }

            case SyntaxKind::BaseExpression: {
                BaseExpressionSyntax* p = (BaseExpressionSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::ThisExpression: {
                ThisExpressionSyntax* p = (ThisExpressionSyntax*)syntaxBase;
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::DefaultExpression: {
                DefaultExpressionSyntax* p = (DefaultExpressionSyntax*)syntaxBase;
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::TypeOfExpression: {
                TypeOfExpressionSyntax* p = (TypeOfExpressionSyntax*)syntaxBase;
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::DiscardDesignation: {
                DiscardDesignationSyntax* p = (DiscardDesignationSyntax*)syntaxBase;
                if(p->underscoreToken.IsValid()) return p->underscoreToken;
                return SyntaxToken();
            }

            case SyntaxKind::SingleVariableDesignation: {
                SingleVariableDesignationSyntax* p = (SingleVariableDesignationSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::ParenthesizedVariableDesignation: {
                ParenthesizedVariableDesignationSyntax* p = (ParenthesizedVariableDesignationSyntax*)syntaxBase;
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->designators != nullptr && p->designators->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->designators->items[p->designators->itemCount - 1]);
                    SyntaxToken b = p->designators->separatorCount == 0 ? SyntaxToken() : p->designators->separators[p->designators->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openParen.IsValid()) return p->openParen;
                return SyntaxToken();
            }

            case SyntaxKind::ExpressionElement: {
                ExpressionElementSyntax* p = (ExpressionElementSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::SpreadElement: {
                SpreadElementSyntax* p = (SpreadElementSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->dotDotToken.IsValid()) return p->dotDotToken;
                return SyntaxToken();
            }

            case SyntaxKind::CollectionExpression: {
                CollectionExpressionSyntax* p = (CollectionExpressionSyntax*)syntaxBase;
                if(p->close.IsValid()) return p->close;
                if(p->elements != nullptr && p->elements->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->elements->items[p->elements->itemCount - 1]);
                    SyntaxToken b = p->elements->separatorCount == 0 ? SyntaxToken() : p->elements->separators[p->elements->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->open.IsValid()) return p->open;
                return SyntaxToken();
            }

            case SyntaxKind::DeclarationExpression: {
                DeclarationExpressionSyntax* p = (DeclarationExpressionSyntax*)syntaxBase;
                if(p->designation != nullptr) return GetLastToken((SyntaxBase*)p->designation);
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::ThrowExpression: {
                ThrowExpressionSyntax* p = (ThrowExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->throwKeyword.IsValid()) return p->throwKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::PostIncrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }
            case SyntaxKind::PostDecrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::ElementAccessExpression: {
                ElementAccessExpressionSyntax* p = (ElementAccessExpressionSyntax*)syntaxBase;
                if(p->argumentList != nullptr) return GetLastToken((SyntaxBase*)p->argumentList);
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::InvocationExpression: {
                InvocationExpressionSyntax* p = (InvocationExpressionSyntax*)syntaxBase;
                if(p->argumentList != nullptr) return GetLastToken((SyntaxBase*)p->argumentList);
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::ConditionalExpression: {
                ConditionalExpressionSyntax* p = (ConditionalExpressionSyntax*)syntaxBase;
                if(p->whenFalse != nullptr) return GetLastToken((SyntaxBase*)p->whenFalse);
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->whenTrue != nullptr) return GetLastToken((SyntaxBase*)p->whenTrue);
                if(p->questionToken.IsValid()) return p->questionToken;
                if(p->condition != nullptr) return GetLastToken((SyntaxBase*)p->condition);
                return SyntaxToken();
            }

            case SyntaxKind::RangeExpression: {
                RangeExpressionSyntax* p = (RangeExpressionSyntax*)syntaxBase;
                if(p->rightOperand != nullptr) return GetLastToken((SyntaxBase*)p->rightOperand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->leftOperand != nullptr) return GetLastToken((SyntaxBase*)p->leftOperand);
                return SyntaxToken();
            }

            case SyntaxKind::UnaryPlusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operand != nullptr) return GetLastToken((SyntaxBase*)p->operand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }
            case SyntaxKind::UnaryMinusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operand != nullptr) return GetLastToken((SyntaxBase*)p->operand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }
            case SyntaxKind::BitwiseNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operand != nullptr) return GetLastToken((SyntaxBase*)p->operand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }
            case SyntaxKind::LogicalNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operand != nullptr) return GetLastToken((SyntaxBase*)p->operand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }
            case SyntaxKind::PreIncrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operand != nullptr) return GetLastToken((SyntaxBase*)p->operand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }
            case SyntaxKind::PreDecrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operand != nullptr) return GetLastToken((SyntaxBase*)p->operand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }
            case SyntaxKind::IndexExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operand != nullptr) return GetLastToken((SyntaxBase*)p->operand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }

            case SyntaxKind::Parameter: {
                ParameterSyntax* p = (ParameterSyntax*)syntaxBase;
                if(p->defaultValue != nullptr) return GetLastToken((SyntaxBase*)p->defaultValue);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            case SyntaxKind::SimpleLambdaExpression: {
                SimpleLambdaExpressionSyntax* p = (SimpleLambdaExpressionSyntax*)syntaxBase;
                if(p->expressionBody != nullptr) return GetLastToken((SyntaxBase*)p->expressionBody);
                if(p->blockBody != nullptr) return GetLastToken((SyntaxBase*)p->blockBody);
                if(p->arrowToken.IsValid()) return p->arrowToken;
                if(p->parameter != nullptr) return GetLastToken((SyntaxBase*)p->parameter);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            case SyntaxKind::ParenthesizedLambdaExpression: {
                ParenthesizedLambdaExpressionSyntax* p = (ParenthesizedLambdaExpressionSyntax*)syntaxBase;
                if(p->expressionBody != nullptr) return GetLastToken((SyntaxBase*)p->expressionBody);
                if(p->blockBody != nullptr) return GetLastToken((SyntaxBase*)p->blockBody);
                if(p->arrowToken.IsValid()) return p->arrowToken;
                if(p->parameters != nullptr) return GetLastToken((SyntaxBase*)p->parameters);
                if(p->returnType != nullptr) return GetLastToken((SyntaxBase*)p->returnType);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            case SyntaxKind::BaseConstructorInitializer: {
                BaseConstructorInitializerSyntax* p = (BaseConstructorInitializerSyntax*)syntaxBase;
                if(p->argumentListSyntax != nullptr) return GetLastToken((SyntaxBase*)p->argumentListSyntax);
                if(p->baseKeyword.IsValid()) return p->baseKeyword;
                if(p->colonToken.IsValid()) return p->colonToken;
                return SyntaxToken();
            }

            case SyntaxKind::ThisConstructorInitializer: {
                ThisConstructorInitializerSyntax* p = (ThisConstructorInitializerSyntax*)syntaxBase;
                if(p->argumentListSyntax != nullptr) return GetLastToken((SyntaxBase*)p->argumentListSyntax);
                if(p->thisKeyword.IsValid()) return p->thisKeyword;
                if(p->colonToken.IsValid()) return p->colonToken;
                return SyntaxToken();
            }

            case SyntaxKind::NamedConstructorInitializer: {
                NamedConstructorInitializerSyntax* p = (NamedConstructorInitializerSyntax*)syntaxBase;
                if(p->argumentListSyntax != nullptr) return GetLastToken((SyntaxBase*)p->argumentListSyntax);
                if(p->name.IsValid()) return p->name;
                if(p->colonToken.IsValid()) return p->colonToken;
                return SyntaxToken();
            }

            case SyntaxKind::ParameterList: {
                ParameterListSyntax* p = (ParameterListSyntax*)syntaxBase;
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->parameters != nullptr && p->parameters->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->parameters->items[p->parameters->itemCount - 1]);
                    SyntaxToken b = p->parameters->separatorCount == 0 ? SyntaxToken() : p->parameters->separators[p->parameters->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openParen.IsValid()) return p->openParen;
                return SyntaxToken();
            }

            case SyntaxKind::BracketedParameterList: {
                BracketedParameterListSyntax* p = (BracketedParameterListSyntax*)syntaxBase;
                if(p->closeBracket.IsValid()) return p->closeBracket;
                if(p->parameters != nullptr && p->parameters->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->parameters->items[p->parameters->itemCount - 1]);
                    SyntaxToken b = p->parameters->separatorCount == 0 ? SyntaxToken() : p->parameters->separators[p->parameters->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBracket.IsValid()) return p->openBracket;
                return SyntaxToken();
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->arrowBody != nullptr) return GetLastToken((SyntaxBase*)p->arrowBody);
                if(p->blockBody != nullptr) return GetLastToken((SyntaxBase*)p->blockBody);
                if(p->constraints != nullptr && p->constraints->size != 0) return GetLastToken(p->constraints->array[p->constraints->size - 1]);
                if(p->parameters != nullptr) return GetLastToken((SyntaxBase*)p->parameters);
                if(p->typeParameters != nullptr) return GetLastToken((SyntaxBase*)p->typeParameters);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->returnType != nullptr) return GetLastToken((SyntaxBase*)p->returnType);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* p = (VariableDeclarationSyntax*)syntaxBase;
                if(p->variables != nullptr && p->variables->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->variables->items[p->variables->itemCount - 1]);
                    SyntaxToken b = p->variables->separatorCount == 0 ? SyntaxToken() : p->variables->separators[p->variables->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::LocalDeclarationStatement: {
                LocalDeclarationStatementSyntax* p = (LocalDeclarationStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->declaration != nullptr) return GetLastToken((SyntaxBase*)p->declaration);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->usingKeyword.IsValid()) return p->usingKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* p = (FieldDeclarationSyntax*)syntaxBase;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                if(p->declaration != nullptr) return GetLastToken((SyntaxBase*)p->declaration);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            case SyntaxKind::ExpressionColon: {
                ExpressionColonSyntax* p = (ExpressionColonSyntax*)syntaxBase;
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::Subpattern: {
                SubpatternSyntax* p = (SubpatternSyntax*)syntaxBase;
                if(p->pattern != nullptr) return GetLastToken((SyntaxBase*)p->pattern);
                if(p->expressionColon != nullptr) return GetLastToken((SyntaxBase*)p->expressionColon);
                return SyntaxToken();
            }

            case SyntaxKind::PropertyPatternClause: {
                PropertyPatternClauseSyntax* p = (PropertyPatternClauseSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->subpatterns != nullptr && p->subpatterns->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->subpatterns->items[p->subpatterns->itemCount - 1]);
                    SyntaxToken b = p->subpatterns->separatorCount == 0 ? SyntaxToken() : p->subpatterns->separators[p->subpatterns->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::DeclarationPattern: {
                DeclarationPatternSyntax* p = (DeclarationPatternSyntax*)syntaxBase;
                if(p->designation != nullptr) return GetLastToken((SyntaxBase*)p->designation);
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::PositionalPatternClause: {
                PositionalPatternClauseSyntax* p = (PositionalPatternClauseSyntax*)syntaxBase;
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->subpatterns != nullptr && p->subpatterns->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->subpatterns->items[p->subpatterns->itemCount - 1]);
                    SyntaxToken b = p->subpatterns->separatorCount == 0 ? SyntaxToken() : p->subpatterns->separators[p->subpatterns->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openParenToken.IsValid()) return p->openParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::RecursivePattern: {
                RecursivePatternSyntax* p = (RecursivePatternSyntax*)syntaxBase;
                if(p->designation != nullptr) return GetLastToken((SyntaxBase*)p->designation);
                if(p->propertyPatternClause != nullptr) return GetLastToken((SyntaxBase*)p->propertyPatternClause);
                if(p->positionalPatternClause != nullptr) return GetLastToken((SyntaxBase*)p->positionalPatternClause);
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::ParenthesizedPattern: {
                ParenthesizedPatternSyntax* p = (ParenthesizedPatternSyntax*)syntaxBase;
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->pattern != nullptr) return GetLastToken((SyntaxBase*)p->pattern);
                if(p->openParenToken.IsValid()) return p->openParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::VarPattern: {
                VarPatternSyntax* p = (VarPatternSyntax*)syntaxBase;
                if(p->designation != nullptr) return GetLastToken((SyntaxBase*)p->designation);
                if(p->varKeyword.IsValid()) return p->varKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::TypePattern: {
                TypePatternSyntax* p = (TypePatternSyntax*)syntaxBase;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::ConstantPattern: {
                ConstantPatternSyntax* p = (ConstantPatternSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::RelationalPattern: {
                RelationalPatternSyntax* p = (RelationalPatternSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }

            case SyntaxKind::SlicePattern: {
                SlicePatternSyntax* p = (SlicePatternSyntax*)syntaxBase;
                if(p->pattern != nullptr) return GetLastToken((SyntaxBase*)p->pattern);
                if(p->dotDotToken.IsValid()) return p->dotDotToken;
                return SyntaxToken();
            }

            case SyntaxKind::DiscardPattern: {
                DiscardPatternSyntax* p = (DiscardPatternSyntax*)syntaxBase;
                if(p->underscore.IsValid()) return p->underscore;
                return SyntaxToken();
            }

            case SyntaxKind::NotPattern: {
                UnaryPatternSyntax* p = (UnaryPatternSyntax*)syntaxBase;
                if(p->pattern != nullptr) return GetLastToken((SyntaxBase*)p->pattern);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }

            case SyntaxKind::OrPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::AndPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }

            case SyntaxKind::IsPatternExpression: {
                IsPatternExpressionSyntax* p = (IsPatternExpressionSyntax*)syntaxBase;
                if(p->pattern != nullptr) return GetLastToken((SyntaxBase*)p->pattern);
                if(p->opToken.IsValid()) return p->opToken;
                if(p->leftOperand != nullptr) return GetLastToken((SyntaxBase*)p->leftOperand);
                return SyntaxToken();
            }

            case SyntaxKind::AddExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::SubtractExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::MultiplyExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::DivideExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::ModuloExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::LeftShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::RightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::UnsignedRightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::LogicalOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::LogicalAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::BitwiseOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::BitwiseAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::ExclusiveOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::EqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::NotEqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::LessThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::LessThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::GreaterThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::GreaterThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::IsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::AsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::CoalesceExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }

            case SyntaxKind::ImplicitElementAccess: {
                ImplicitElementAccessSyntax* p = (ImplicitElementAccessSyntax*)syntaxBase;
                if(p->argumentList != nullptr) return GetLastToken((SyntaxBase*)p->argumentList);
                return SyntaxToken();
            }

            case SyntaxKind::WhenClause: {
                WhenClauseSyntax* p = (WhenClauseSyntax*)syntaxBase;
                if(p->condition != nullptr) return GetLastToken((SyntaxBase*)p->condition);
                if(p->whenKeyword.IsValid()) return p->whenKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::SwitchExpressionArm: {
                SwitchExpressionArmSyntax* p = (SwitchExpressionArmSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->equalsGreaterThanToken.IsValid()) return p->equalsGreaterThanToken;
                if(p->whenClause != nullptr) return GetLastToken((SyntaxBase*)p->whenClause);
                if(p->pattern != nullptr) return GetLastToken((SyntaxBase*)p->pattern);
                return SyntaxToken();
            }

            case SyntaxKind::SwitchExpression: {
                SwitchExpressionSyntax* p = (SwitchExpressionSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->arms != nullptr && p->arms->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->arms->items[p->arms->itemCount - 1]);
                    SyntaxToken b = p->arms->separatorCount == 0 ? SyntaxToken() : p->arms->separators[p->arms->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->switchKeyword.IsValid()) return p->switchKeyword;
                if(p->governingExpression != nullptr) return GetLastToken((SyntaxBase*)p->governingExpression);
                return SyntaxToken();
            }

            case SyntaxKind::ListPattern: {
                ListPatternSyntax* p = (ListPatternSyntax*)syntaxBase;
                if(p->designation != nullptr) return GetLastToken((SyntaxBase*)p->designation);
                if(p->closeBracketToken.IsValid()) return p->closeBracketToken;
                if(p->patterns != nullptr && p->patterns->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->patterns->items[p->patterns->itemCount - 1]);
                    SyntaxToken b = p->patterns->separatorCount == 0 ? SyntaxToken() : p->patterns->separators[p->patterns->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBracketToken.IsValid()) return p->openBracketToken;
                return SyntaxToken();
            }

            case SyntaxKind::SimpleAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::AddAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::SubtractAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::MultiplyAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::DivideAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::ModuloAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::AndAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::ExclusiveOrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::OrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::LeftShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::RightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::UnsignedRightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }
            case SyntaxKind::CoalesceAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }

            case SyntaxKind::ForEachStatement: {
                ForEachStatementSyntax* p = (ForEachStatementSyntax*)syntaxBase;
                if(p->statement != nullptr) return GetLastToken((SyntaxBase*)p->statement);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->inKeyword.IsValid()) return p->inKeyword;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->openParen.IsValid()) return p->openParen;
                if(p->foreachKeyword.IsValid()) return p->foreachKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ForEachVariableStatement: {
                ForEachVariableStatementSyntax* p = (ForEachVariableStatementSyntax*)syntaxBase;
                if(p->statement != nullptr) return GetLastToken((SyntaxBase*)p->statement);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->inKeyword.IsValid()) return p->inKeyword;
                if(p->variable != nullptr) return GetLastToken((SyntaxBase*)p->variable);
                if(p->openParen.IsValid()) return p->openParen;
                if(p->foreachKeyword.IsValid()) return p->foreachKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::GotoCaseStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->arg != nullptr) return GetLastToken((SyntaxBase*)p->arg);
                if(p->caseOrDefault.IsValid()) return p->caseOrDefault;
                if(p->gotoToken.IsValid()) return p->gotoToken;
                return SyntaxToken();
            }
            case SyntaxKind::GotoDefaultStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->arg != nullptr) return GetLastToken((SyntaxBase*)p->arg);
                if(p->caseOrDefault.IsValid()) return p->caseOrDefault;
                if(p->gotoToken.IsValid()) return p->gotoToken;
                return SyntaxToken();
            }
            case SyntaxKind::GotoStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->arg != nullptr) return GetLastToken((SyntaxBase*)p->arg);
                if(p->caseOrDefault.IsValid()) return p->caseOrDefault;
                if(p->gotoToken.IsValid()) return p->gotoToken;
                return SyntaxToken();
            }

            case SyntaxKind::ElseClause: {
                ElseClauseSyntax* p = (ElseClauseSyntax*)syntaxBase;
                if(p->statement != nullptr) return GetLastToken((SyntaxBase*)p->statement);
                if(p->elseKeyword.IsValid()) return p->elseKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::IfStatement: {
                IfStatementSyntax* p = (IfStatementSyntax*)syntaxBase;
                if(p->elseClause != nullptr) return GetLastToken((SyntaxBase*)p->elseClause);
                if(p->statement != nullptr) return GetLastToken((SyntaxBase*)p->statement);
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->condition != nullptr) return GetLastToken((SyntaxBase*)p->condition);
                if(p->openParen.IsValid()) return p->openParen;
                if(p->ifKeyword.IsValid()) return p->ifKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ExpressionStatement: {
                ExpressionStatementSyntax* p = (ExpressionStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::ReturnStatement: {
                ReturnStatementSyntax* p = (ReturnStatementSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->expressionSyntax != nullptr) return GetLastToken((SyntaxBase*)p->expressionSyntax);
                if(p->returnKeyword.IsValid()) return p->returnKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::BaseList: {
                BaseListSyntax* p = (BaseListSyntax*)syntaxBase;
                if(p->types != nullptr && p->types->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->types->items[p->types->itemCount - 1]);
                    SyntaxToken b = p->types->separatorCount == 0 ? SyntaxToken() : p->types->separators[p->types->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->colonToken.IsValid()) return p->colonToken;
                return SyntaxToken();
            }

            case SyntaxKind::Attribute: {
                AttributeSyntax* p = (AttributeSyntax*)syntaxBase;
                if(p->argumentList != nullptr) return GetLastToken((SyntaxBase*)p->argumentList);
                if(p->name != nullptr) return GetLastToken((SyntaxBase*)p->name);
                return SyntaxToken();
            }

            case SyntaxKind::AttributeList: {
                AttributeListSyntax* p = (AttributeListSyntax*)syntaxBase;
                if(p->closeBracket.IsValid()) return p->closeBracket;
                if(p->attributes != nullptr && p->attributes->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->attributes->items[p->attributes->itemCount - 1]);
                    SyntaxToken b = p->attributes->separatorCount == 0 ? SyntaxToken() : p->attributes->separators[p->attributes->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBracket.IsValid()) return p->openBracket;
                return SyntaxToken();
            }

            case SyntaxKind::TypeConstraint: {
                TypeConstraintSyntax* p = (TypeConstraintSyntax*)syntaxBase;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::ConstructorConstraint: {
                ConstructorConstraintSyntax* p = (ConstructorConstraintSyntax*)syntaxBase;
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->newKeyword.IsValid()) return p->newKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ClassConstraint: {
                ClassOrStructConstraintSyntax* p = (ClassOrStructConstraintSyntax*)syntaxBase;
                if(p->questionToken.IsValid()) return p->questionToken;
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }
            case SyntaxKind::StructConstraint: {
                ClassOrStructConstraintSyntax* p = (ClassOrStructConstraintSyntax*)syntaxBase;
                if(p->questionToken.IsValid()) return p->questionToken;
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameterConstraintClause: {
                TypeParameterConstraintClauseSyntax* p = (TypeParameterConstraintClauseSyntax*)syntaxBase;
                if(p->constraints != nullptr && p->constraints->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->constraints->items[p->constraints->itemCount - 1]);
                    SyntaxToken b = p->constraints->separatorCount == 0 ? SyntaxToken() : p->constraints->separators[p->constraints->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->colonToken.IsValid()) return p->colonToken;
                if(p->name != nullptr) return GetLastToken((SyntaxBase*)p->name);
                if(p->whereKeyword.IsValid()) return p->whereKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::StructDeclaration: {
                StructDeclarationSyntax* p = (StructDeclarationSyntax*)syntaxBase;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->members != nullptr && p->members->size != 0) return GetLastToken(p->members->array[p->members->size - 1]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetLastToken(p->constraintClauses->array[p->constraintClauses->size - 1]);
                if(p->baseList != nullptr) return GetLastToken((SyntaxBase*)p->baseList);
                if(p->parameterList != nullptr) return GetLastToken((SyntaxBase*)p->parameterList);
                if(p->typeParameterList != nullptr) return GetLastToken((SyntaxBase*)p->typeParameterList);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::EnumMemberDeclaration: {
                EnumMemberDeclarationSyntax* p = (EnumMemberDeclarationSyntax*)syntaxBase;
                if(p->equalsValue != nullptr) return GetLastToken((SyntaxBase*)p->equalsValue);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::EnumDeclaration: {
                EnumDeclarationSyntax* p = (EnumDeclarationSyntax*)syntaxBase;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                if(p->closeBrace.IsValid()) return p->closeBrace;
                if(p->members != nullptr && p->members->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->members->items[p->members->itemCount - 1]);
                    SyntaxToken b = p->members->separatorCount == 0 ? SyntaxToken() : p->members->separators[p->members->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBrace.IsValid()) return p->openBrace;
                if(p->baseList != nullptr) return GetLastToken((SyntaxBase*)p->baseList);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::DelegateDeclaration: {
                DelegateDeclarationSyntax* p = (DelegateDeclarationSyntax*)syntaxBase;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetLastToken(p->constraintClauses->array[p->constraintClauses->size - 1]);
                if(p->parameterList != nullptr) return GetLastToken((SyntaxBase*)p->parameterList);
                if(p->typeParameterList != nullptr) return GetLastToken((SyntaxBase*)p->typeParameterList);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->returnType != nullptr) return GetLastToken((SyntaxBase*)p->returnType);
                if(p->keyword.IsValid()) return p->keyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::ClassDeclaration: {
                ClassDeclarationSyntax* p = (ClassDeclarationSyntax*)syntaxBase;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->members != nullptr && p->members->size != 0) return GetLastToken(p->members->array[p->members->size - 1]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetLastToken(p->constraintClauses->array[p->constraintClauses->size - 1]);
                if(p->baseList != nullptr) return GetLastToken((SyntaxBase*)p->baseList);
                if(p->parameterList != nullptr) return GetLastToken((SyntaxBase*)p->parameterList);
                if(p->typeParameterList != nullptr) return GetLastToken((SyntaxBase*)p->typeParameterList);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::NamespaceDeclaration: {
                NamespaceDeclarationSyntax* p = (NamespaceDeclarationSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->names != nullptr && p->names->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->names->items[p->names->itemCount - 1]);
                    SyntaxToken b = p->names->separatorCount == 0 ? SyntaxToken() : p->names->separators[p->names->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->keyword.IsValid()) return p->keyword;
                return SyntaxToken();
            }

            case SyntaxKind::InterfaceDeclaration: {
                InterfaceDeclarationSyntax* p = (InterfaceDeclarationSyntax*)syntaxBase;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->members != nullptr && p->members->size != 0) return GetLastToken(p->members->array[p->members->size - 1]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetLastToken(p->constraintClauses->array[p->constraintClauses->size - 1]);
                if(p->baseList != nullptr) return GetLastToken((SyntaxBase*)p->baseList);
                if(p->parameterList != nullptr) return GetLastToken((SyntaxBase*)p->parameterList);
                if(p->typeParameterList != nullptr) return GetLastToken((SyntaxBase*)p->typeParameterList);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->keyword.IsValid()) return p->keyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::ConstructorDeclaration: {
                ConstructorDeclarationSyntax* p = (ConstructorDeclarationSyntax*)syntaxBase;
                if(p->semiColon.IsValid()) return p->semiColon;
                if(p->bodyExpression != nullptr) return GetLastToken((SyntaxBase*)p->bodyExpression);
                if(p->bodyBlock != nullptr) return GetLastToken((SyntaxBase*)p->bodyBlock);
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->parameterList != nullptr) return GetLastToken((SyntaxBase*)p->parameterList);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::BaseType: {
                BaseTypeSyntax* p = (BaseTypeSyntax*)syntaxBase;
                if(p->argumentList != nullptr) return GetLastToken((SyntaxBase*)p->argumentList);
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::StringLiteralExpression: {
                StringLiteralExpression* p = (StringLiteralExpression*)syntaxBase;
                if(p->end.IsValid()) return p->end;
                if(p->parts != nullptr && p->parts->size != 0) return GetLastToken(p->parts->array[p->parts->size - 1]);
                if(p->start.IsValid()) return p->start;
                return SyntaxToken();
            }

            case SyntaxKind::RawStringLiteralExpression: {
                RawStringLiteralExpression* p = (RawStringLiteralExpression*)syntaxBase;
                if(p->end.IsValid()) return p->end;
                if(p->parts != nullptr && p->parts->size != 0) return GetLastToken(p->parts->array[p->parts->size - 1]);
                if(p->start.IsValid()) return p->start;
                return SyntaxToken();
            }

            case SyntaxKind::InterpolatedIdentifierPart: {
                InterpolatedIdentifierPartSyntax* p = (InterpolatedIdentifierPartSyntax*)syntaxBase;
                if(p->interpolatedIdentifier.IsValid()) return p->interpolatedIdentifier;
                return SyntaxToken();
            }

            case SyntaxKind::InterpolatedStringExpression: {
                InterpolatedStringExpressionSyntax* p = (InterpolatedStringExpressionSyntax*)syntaxBase;
                if(p->end.IsValid()) return p->end;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->start.IsValid()) return p->start;
                return SyntaxToken();
            }

            case SyntaxKind::StringLiteralPart: {
                StringLiteralPartSyntax* p = (StringLiteralPartSyntax*)syntaxBase;
                if(p->part.IsValid()) return p->part;
                return SyntaxToken();
            }

            case SyntaxKind::CharacterLiteralExpression: {
                CharacterLiteralExpressionSyntax* p = (CharacterLiteralExpressionSyntax*)syntaxBase;
                if(p->end.IsValid()) return p->end;
                if(p->contents.IsValid()) return p->contents;
                if(p->start.IsValid()) return p->start;
                return SyntaxToken();
            }

            case SyntaxKind::IncompleteMember: {
                IncompleteMemberSyntax* p = (IncompleteMemberSyntax*)syntaxBase;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::GetAccessorDeclaration: {
                AccessorDeclarationSyntax* p = (AccessorDeclarationSyntax*)syntaxBase;
                if(p->semiColon.IsValid()) return p->semiColon;
                if(p->expressionBody != nullptr) return GetLastToken((SyntaxBase*)p->expressionBody);
                if(p->bodyBlock != nullptr) return GetLastToken((SyntaxBase*)p->bodyBlock);
                if(p->keyword.IsValid()) return p->keyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }
            case SyntaxKind::SetAccessorDeclaration: {
                AccessorDeclarationSyntax* p = (AccessorDeclarationSyntax*)syntaxBase;
                if(p->semiColon.IsValid()) return p->semiColon;
                if(p->expressionBody != nullptr) return GetLastToken((SyntaxBase*)p->expressionBody);
                if(p->bodyBlock != nullptr) return GetLastToken((SyntaxBase*)p->bodyBlock);
                if(p->keyword.IsValid()) return p->keyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }
            case SyntaxKind::InitAccessorDeclaration: {
                AccessorDeclarationSyntax* p = (AccessorDeclarationSyntax*)syntaxBase;
                if(p->semiColon.IsValid()) return p->semiColon;
                if(p->expressionBody != nullptr) return GetLastToken((SyntaxBase*)p->expressionBody);
                if(p->bodyBlock != nullptr) return GetLastToken((SyntaxBase*)p->bodyBlock);
                if(p->keyword.IsValid()) return p->keyword;
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            case SyntaxKind::AccessorList: {
                AccessorListSyntax* p = (AccessorListSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->accessors != nullptr && p->accessors->size != 0) return GetLastToken(p->accessors->array[p->accessors->size - 1]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::IndexerDeclaration: {
                IndexerDeclarationSyntax* p = (IndexerDeclarationSyntax*)syntaxBase;
                if(p->semiColon.IsValid()) return p->semiColon;
                if(p->expressionBody != nullptr) return GetLastToken((SyntaxBase*)p->expressionBody);
                if(p->accessorList != nullptr) return GetLastToken((SyntaxBase*)p->accessorList);
                if(p->parameters != nullptr) return GetLastToken((SyntaxBase*)p->parameters);
                if(p->thisKeyword.IsValid()) return p->thisKeyword;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::PropertyDeclaration: {
                PropertyDeclarationSyntax* p = (PropertyDeclarationSyntax*)syntaxBase;
                if(p->semiColon.IsValid()) return p->semiColon;
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->expressionBody != nullptr) return GetLastToken((SyntaxBase*)p->expressionBody);
                if(p->accessorList != nullptr) return GetLastToken((SyntaxBase*)p->accessorList);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::MethodDeclaration: {
                MethodDeclarationSyntax* p = (MethodDeclarationSyntax*)syntaxBase;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                if(p->expressionBody != nullptr) return GetLastToken((SyntaxBase*)p->expressionBody);
                if(p->body != nullptr) return GetLastToken((SyntaxBase*)p->body);
                if(p->constraintClauses != nullptr && p->constraintClauses->size != 0) return GetLastToken(p->constraintClauses->array[p->constraintClauses->size - 1]);
                if(p->parameterList != nullptr) return GetLastToken((SyntaxBase*)p->parameterList);
                if(p->typeParameterList != nullptr) return GetLastToken((SyntaxBase*)p->typeParameterList);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->returnType != nullptr) return GetLastToken((SyntaxBase*)p->returnType);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->attributes != nullptr && p->attributes->size != 0) return GetLastToken(p->attributes->array[p->attributes->size - 1]);
                return SyntaxToken();
            }

            case SyntaxKind::UsingNamespaceDeclaration: {
                UsingNamespaceDeclarationSyntax* p = (UsingNamespaceDeclarationSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->namePath != nullptr && p->namePath->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->namePath->items[p->namePath->itemCount - 1]);
                    SyntaxToken b = p->namePath->separatorCount == 0 ? SyntaxToken() : p->namePath->separators[p->namePath->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->usingKeyword.IsValid()) return p->usingKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::UsingDeclaration: {
                UsingDeclarationSyntax* p = (UsingDeclarationSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->namespaceOrType != nullptr) return GetLastToken((SyntaxBase*)p->namespaceOrType);
                if(p->alias != nullptr) return GetLastToken((SyntaxBase*)p->alias);
                if(p->staticKeyword.IsValid()) return p->staticKeyword;
                if(p->usingKeyword.IsValid()) return p->usingKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ExternDeclaration: {
                ExternDeclarationSyntax* p = (ExternDeclarationSyntax*)syntaxBase;
                if(p->semicolon.IsValid()) return p->semicolon;
                if(p->parameterList != nullptr) return GetLastToken((SyntaxBase*)p->parameterList);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->returnType != nullptr) return GetLastToken((SyntaxBase*)p->returnType);
                if(p->modifiers != nullptr && p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                if(p->externKeyword.IsValid()) return p->externKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::CompilationUnit: {
                CompilationUnitSyntax* p = (CompilationUnitSyntax*)syntaxBase;
                if(p->eof.IsValid()) return p->eof;
                if(p->members != nullptr && p->members->size != 0) return GetLastToken(p->members->array[p->members->size - 1]);
                return SyntaxToken();
            }

            default: {
                return SyntaxToken();
            }
            
        }        
        
    }
    
}
