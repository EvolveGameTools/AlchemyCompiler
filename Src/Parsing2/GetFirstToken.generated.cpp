#include "./SyntaxBase.h"
#include "./SyntaxNodes.h"

namespace Alchemy::Compilation {
    
    SyntaxToken GetFirstToken(SyntaxBase * syntaxBase) {
    
        if(syntaxBase == nullptr) {
            return SyntaxToken();
        }
        
        switch(syntaxBase->GetKind()) {
            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*)syntaxBase;
                if(p->open.IsValid()) return p->open;
                if(p->ranks->itemCount != 0) return GetFirstToken(p->ranks->items[0]);
                if(p->close.IsValid()) return p->close;
                return SyntaxToken();
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* p = (TypeArgumentListSyntax*)syntaxBase;
                if(p->lessThanToken.IsValid()) return p->lessThanToken;
                if(p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
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

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                if(p->typeToken.IsValid()) return p->typeToken;
                return SyntaxToken();
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->elements->itemCount != 0) return GetFirstToken(p->elements->items[0]);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::ArrayType: {
                ArrayTypeSyntax* p = (ArrayTypeSyntax*)syntaxBase;
                if(p->elementType != nullptr) return GetFirstToken((SyntaxBase*)p->elementType);
                if(p->ranks->size != 0) return GetFirstToken(p->ranks->array[0]);
                return SyntaxToken();
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                if(p->refKeyword.IsValid()) return p->refKeyword;
                if(p->readonlyKeyword.IsValid()) return p->readonlyKeyword;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::OmittedArraySizeExpression: {
                OmittedArraySizeExpressionSyntax* p = (OmittedArraySizeExpressionSyntax*)syntaxBase;
                if(p->token.IsValid()) return p->token;
                return SyntaxToken();
            }

            case SyntaxKind::NullableType: {
                NullableType* p = (NullableType*)syntaxBase;
                if(p->elementType != nullptr) return GetFirstToken((SyntaxBase*)p->elementType);
                if(p->questionMark.IsValid()) return p->questionMark;
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
                if(p->commas->size != 0) return p->commas->array[0];
                if(p->closeBracket.IsValid()) return p->closeBracket;
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                return SyntaxToken();
            }

            case SyntaxKind::ObjectInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::CollectionInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::ArrayInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::ComplexElementInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }
            case SyntaxKind::WithInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                if(p->list->itemCount != 0) return GetFirstToken(p->list->items[0]);
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
                if(p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
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

            case SyntaxKind::ArrayCreationExpression: {
                ArrayCreationExpressionSyntax* p = (ArrayCreationExpressionSyntax*)syntaxBase;
                if(p->newKeyword.IsValid()) return p->newKeyword;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
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
                if(p->initializers->itemCount != 0) return GetFirstToken(p->initializers->items[0]);
                if(p->closeBrace.IsValid()) return p->closeBrace;
                return SyntaxToken();
            }

            case SyntaxKind::TupleExpression: {
                TupleExpressionSyntax* p = (TupleExpressionSyntax*)syntaxBase;
                if(p->openToken.IsValid()) return p->openToken;
                if(p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
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
                if(p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
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
                if(p->argumentList != nullptr) return GetFirstToken((SyntaxBase*)p->argumentList);
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
                if(p->parameters->itemCount != 0) return GetFirstToken(p->parameters->items[0]);
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
                if(p->statements->size != 0) return GetFirstToken(p->statements->array[0]);
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::CharacterLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
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
            case SyntaxKind::StringLiteralExpression: {
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

            case SyntaxKind::SizeOfExpression: {
                SizeOfExpressionSyntax* p = (SizeOfExpressionSyntax*)syntaxBase;
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
                if(p->designators->itemCount != 0) return GetFirstToken(p->designators->items[0]);
                if(p->closeParen.IsValid()) return p->closeParen;
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
            case SyntaxKind::AddressOfExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operatorToken.IsValid()) return p->operatorToken;
                if(p->operand != nullptr) return GetFirstToken((SyntaxBase*)p->operand);
                return SyntaxToken();
            }
            case SyntaxKind::PointerIndirectionExpression: {
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
                if(p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->defaultValue != nullptr) return GetFirstToken((SyntaxBase*)p->defaultValue);
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
                if(p->parameters->itemCount != 0) return GetFirstToken(p->parameters->items[0]);
                if(p->closeParen.IsValid()) return p->closeParen;
                return SyntaxToken();
            }

            case SyntaxKind::ConstraintClauses: {
                ConstraintClausesSyntax* p = (ConstraintClausesSyntax*)syntaxBase;
                if(p->dummy.IsValid()) return p->dummy;
                return SyntaxToken();
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                if(p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->returnType != nullptr) return GetFirstToken((SyntaxBase*)p->returnType);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeParameters != nullptr) return GetFirstToken((SyntaxBase*)p->typeParameters);
                if(p->parameters != nullptr) return GetFirstToken((SyntaxBase*)p->parameters);
                if(p->constraints != nullptr) return GetFirstToken((SyntaxBase*)p->constraints);
                if(p->body != nullptr) return GetFirstToken((SyntaxBase*)p->body);
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* p = (VariableDeclarationSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->variables->itemCount != 0) return GetFirstToken(p->variables->items[0]);
                return SyntaxToken();
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* p = (FieldDeclarationSyntax*)syntaxBase;
                if(p->modifiers->size != 0) return p->modifiers->array[0];
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
                if(p->subpatterns->itemCount != 0) return GetFirstToken(p->subpatterns->items[0]);
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
                if(p->subpatterns->itemCount != 0) return GetFirstToken(p->subpatterns->items[0]);
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
            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*)syntaxBase;
                if(p->close.IsValid()) return p->close;
                if(p->ranks->itemCount != 0) {
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
                if(p->arguments->itemCount != 0) {
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

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                if(p->typeToken.IsValid()) return p->typeToken;
                return SyntaxToken();
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->elements->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->elements->items[p->elements->itemCount - 1]);
                    SyntaxToken b = p->elements->separatorCount == 0 ? SyntaxToken() : p->elements->separators[p->elements->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openParenToken.IsValid()) return p->openParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::ArrayType: {
                ArrayTypeSyntax* p = (ArrayTypeSyntax*)syntaxBase;
                if(p->ranks->size != 0) return GetLastToken(p->ranks->array[p->ranks->size - 1]);
                if(p->elementType != nullptr) return GetLastToken((SyntaxBase*)p->elementType);
                return SyntaxToken();
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->readonlyKeyword.IsValid()) return p->readonlyKeyword;
                if(p->refKeyword.IsValid()) return p->refKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::OmittedArraySizeExpression: {
                OmittedArraySizeExpressionSyntax* p = (OmittedArraySizeExpressionSyntax*)syntaxBase;
                if(p->token.IsValid()) return p->token;
                return SyntaxToken();
            }

            case SyntaxKind::NullableType: {
                NullableType* p = (NullableType*)syntaxBase;
                if(p->questionMark.IsValid()) return p->questionMark;
                if(p->elementType != nullptr) return GetLastToken((SyntaxBase*)p->elementType);
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
                if(p->commas->size != 0) return p->commas->array[p->commas->size - 1];
                if(p->openBracket.IsValid()) return p->openBracket;
                if(p->newKeyword.IsValid()) return p->newKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::ObjectInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                if(p->closeBraceToken.IsValid()) return p->closeBraceToken;
                if(p->list->itemCount != 0) {
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
                if(p->list->itemCount != 0) {
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
                if(p->list->itemCount != 0) {
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
                if(p->list->itemCount != 0) {
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
                if(p->list->itemCount != 0) {
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
                if(p->arguments->itemCount != 0) {
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

            case SyntaxKind::ArrayCreationExpression: {
                ArrayCreationExpressionSyntax* p = (ArrayCreationExpressionSyntax*)syntaxBase;
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
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
                if(p->initializers->itemCount != 0) {
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
                if(p->arguments->itemCount != 0) {
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
                if(p->arguments->itemCount != 0) {
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
                if(p->argumentList != nullptr) return GetLastToken((SyntaxBase*)p->argumentList);
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
                if(p->parameters->itemCount != 0) {
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
                if(p->statements->size != 0) return GetLastToken(p->statements->array[p->statements->size - 1]);
                if(p->openBraceToken.IsValid()) return p->openBraceToken;
                return SyntaxToken();
            }

            case SyntaxKind::CharacterLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                if(p->literal.IsValid()) return p->literal;
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
            case SyntaxKind::StringLiteralExpression: {
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

            case SyntaxKind::SizeOfExpression: {
                SizeOfExpressionSyntax* p = (SizeOfExpressionSyntax*)syntaxBase;
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
                if(p->designators->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->designators->items[p->designators->itemCount - 1]);
                    SyntaxToken b = p->designators->separatorCount == 0 ? SyntaxToken() : p->designators->separators[p->designators->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openParen.IsValid()) return p->openParen;
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
            case SyntaxKind::AddressOfExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                if(p->operand != nullptr) return GetLastToken((SyntaxBase*)p->operand);
                if(p->operatorToken.IsValid()) return p->operatorToken;
                return SyntaxToken();
            }
            case SyntaxKind::PointerIndirectionExpression: {
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
                if(p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
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
                if(p->parameters->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->parameters->items[p->parameters->itemCount - 1]);
                    SyntaxToken b = p->parameters->separatorCount == 0 ? SyntaxToken() : p->parameters->separators[p->parameters->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openParen.IsValid()) return p->openParen;
                return SyntaxToken();
            }

            case SyntaxKind::ConstraintClauses: {
                ConstraintClausesSyntax* p = (ConstraintClausesSyntax*)syntaxBase;
                if(p->dummy.IsValid()) return p->dummy;
                return SyntaxToken();
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                if(p->body != nullptr) return GetLastToken((SyntaxBase*)p->body);
                if(p->constraints != nullptr) return GetLastToken((SyntaxBase*)p->constraints);
                if(p->parameters != nullptr) return GetLastToken((SyntaxBase*)p->parameters);
                if(p->typeParameters != nullptr) return GetLastToken((SyntaxBase*)p->typeParameters);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->returnType != nullptr) return GetLastToken((SyntaxBase*)p->returnType);
                if(p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* p = (VariableDeclarationSyntax*)syntaxBase;
                if(p->variables->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->variables->items[p->variables->itemCount - 1]);
                    SyntaxToken b = p->variables->separatorCount == 0 ? SyntaxToken() : p->variables->separators[p->variables->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* p = (FieldDeclarationSyntax*)syntaxBase;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                if(p->declaration != nullptr) return GetLastToken((SyntaxBase*)p->declaration);
                if(p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
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
                if(p->subpatterns->itemCount != 0) {
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
                if(p->subpatterns->itemCount != 0) {
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

            default: {
                return SyntaxToken();
            }
            
        }        
        
    }
    
}
