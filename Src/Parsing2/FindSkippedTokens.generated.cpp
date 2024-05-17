#include "./FindSkippedTokens.h"

namespace Alchemy::Compilation {
    
    void FindSkippedTokens::TouchNode(SyntaxBase * syntaxBase) {

        if(syntaxBase == nullptr) {
            return;
        }
        
        switch(syntaxBase->GetKind()) {
            case SyntaxKind::EmptyStatement: {
                EmptyStatementSyntax* p = (EmptyStatementSyntax*)syntaxBase;
                TouchToken(p->semicolon);
                break;
            }

            case SyntaxKind::BreakStatement: {
                BreakStatementSyntax* p = (BreakStatementSyntax*)syntaxBase;
                TouchToken(p->breakKeyword);
                TouchToken(p->semicolon);
                break;
            }

            case SyntaxKind::ContinueStatement: {
                ContinueStatementSyntax* p = (ContinueStatementSyntax*)syntaxBase;
                TouchToken(p->continueKeyword);
                TouchToken(p->semicolon);
                break;
            }

            case SyntaxKind::ForStatement: {
                ForStatementSyntax* p = (ForStatementSyntax*)syntaxBase;
                TouchToken(p->forKeyword);
                TouchToken(p->openParenToken);
                TouchNode(p->declaration);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->initializers);
                TouchToken(p->firstSemiColon);
                TouchNode(p->condition);
                TouchToken(p->secondSemiColon);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->incrementors);
                TouchToken(p->closeParenToken);
                TouchNode(p->statement);
                break;
            }

            case SyntaxKind::ThrowStatement: {
                ThrowStatementSyntax* p = (ThrowStatementSyntax*)syntaxBase;
                TouchToken(p->throwKeyword);
                TouchNode(p->expression);
                TouchToken(p->semicolon);
                break;
            }

            case SyntaxKind::CatchDeclaration: {
                CatchDeclarationSyntax* p = (CatchDeclarationSyntax*)syntaxBase;
                TouchToken(p->openParen);
                TouchNode(p->type);
                TouchToken(p->identifier);
                TouchToken(p->closeParen);
                break;
            }

            case SyntaxKind::CatchFilterClause: {
                CatchFilterClauseSyntax* p = (CatchFilterClauseSyntax*)syntaxBase;
                TouchToken(p->whenKeyword);
                TouchToken(p->openParenToken);
                TouchNode(p->filterExpression);
                TouchToken(p->closeParenToken);
                break;
            }

            case SyntaxKind::CatchClause: {
                CatchClauseSyntax* p = (CatchClauseSyntax*)syntaxBase;
                TouchToken(p->catchKeyword);
                TouchNode(p->declaration);
                TouchNode(p->filter);
                TouchNode(p->block);
                break;
            }

            case SyntaxKind::FinallyClause: {
                FinallyClauseSyntax* p = (FinallyClauseSyntax*)syntaxBase;
                TouchToken(p->finallyKeyword);
                TouchNode(p->block);
                break;
            }

            case SyntaxKind::TryStatement: {
                TryStatementSyntax* p = (TryStatementSyntax*)syntaxBase;
                TouchToken(p->tryKeyword);
                TouchNode(p->tryBlock);
                TouchSyntaxList((SyntaxListUntyped*)p->catchClauses);
                TouchNode(p->finallyClaus);
                break;
            }

            case SyntaxKind::DefaultSwitchLabel: {
                DefaultSwitchLabelSyntax* p = (DefaultSwitchLabelSyntax*)syntaxBase;
                TouchToken(p->keyword);
                TouchToken(p->colon);
                break;
            }

            case SyntaxKind::CaseSwitchLabel: {
                CaseSwitchLabelSyntax* p = (CaseSwitchLabelSyntax*)syntaxBase;
                TouchToken(p->keyword);
                TouchNode(p->value);
                TouchToken(p->colon);
                break;
            }

            case SyntaxKind::CasePatternSwitchLabel: {
                CasePatternSwitchLabelSyntax* p = (CasePatternSwitchLabelSyntax*)syntaxBase;
                TouchToken(p->keyword);
                TouchNode(p->pattern);
                TouchNode(p->whenClause);
                TouchToken(p->colonToken);
                break;
            }

            case SyntaxKind::SwitchSection: {
                SwitchSectionSyntax* p = (SwitchSectionSyntax*)syntaxBase;
                TouchSyntaxList((SyntaxListUntyped*)p->labels);
                TouchSyntaxList((SyntaxListUntyped*)p->statements);
                break;
            }

            case SyntaxKind::SwitchStatement: {
                SwitchStatementSyntax* p = (SwitchStatementSyntax*)syntaxBase;
                TouchToken(p->switchKeyword);
                TouchToken(p->openParenToken);
                TouchNode(p->expression);
                TouchToken(p->closeParenToken);
                TouchToken(p->openBraceToken);
                TouchSyntaxList((SyntaxListUntyped*)p->sections);
                TouchToken(p->closeBraceToken);
                break;
            }

            case SyntaxKind::UsingStatement: {
                UsingStatementSyntax* p = (UsingStatementSyntax*)syntaxBase;
                TouchToken(p->usingKeyword);
                TouchToken(p->openParenToken);
                TouchNode(p->declaration);
                TouchNode(p->expression);
                TouchToken(p->closeParenToken);
                TouchNode(p->statement);
                break;
            }

            case SyntaxKind::WhileStatement: {
                WhileStatementSyntax* p = (WhileStatementSyntax*)syntaxBase;
                TouchToken(p->whileKeyword);
                TouchToken(p->openParen);
                TouchNode(p->condition);
                TouchToken(p->closeParen);
                TouchNode(p->statement);
                break;
            }

            case SyntaxKind::DoStatement: {
                DoStatementSyntax* p = (DoStatementSyntax*)syntaxBase;
                TouchToken(p->doKeyword);
                TouchNode(p->statement);
                TouchToken(p->whileKeyword);
                TouchToken(p->openParen);
                TouchNode(p->condition);
                TouchToken(p->closeParen);
                TouchToken(p->semicolon);
                break;
            }

            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*)syntaxBase;
                TouchToken(p->open);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->ranks);
                TouchToken(p->close);
                break;
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* p = (TypeArgumentListSyntax*)syntaxBase;
                TouchToken(p->lessThanToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                TouchToken(p->greaterThanToken);
                break;
            }

            case SyntaxKind::GenericName: {
                GenericNameSyntax* p = (GenericNameSyntax*)syntaxBase;
                TouchToken(p->identifier);
                TouchNode(p->typeArgumentList);
                break;
            }

            case SyntaxKind::ElementBindingExpression: {
                ElementBindingExpressionSyntax* p = (ElementBindingExpressionSyntax*)syntaxBase;
                TouchNode(p->argumentList);
                break;
            }

            case SyntaxKind::MemberBindingExpression: {
                MemberBindingExpressionSyntax* p = (MemberBindingExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->name);
                break;
            }

            case SyntaxKind::ConditionalAccessExpression: {
                ConditionalAccessExpressionSyntax* p = (ConditionalAccessExpressionSyntax*)syntaxBase;
                TouchNode(p->expression);
                TouchToken(p->operatorToken);
                TouchNode(p->whenNotNull);
                break;
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* p = (QualifiedNameSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->dotToken);
                TouchNode(p->right);
                break;
            }

            case SyntaxKind::IdentifierName: {
                IdentifierNameSyntax* p = (IdentifierNameSyntax*)syntaxBase;
                TouchToken(p->identifier);
                break;
            }

            case SyntaxKind::NameColon: {
                NameColonSyntax* p = (NameColonSyntax*)syntaxBase;
                TouchNode(p->name);
                TouchToken(p->colonToken);
                break;
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                TouchToken(p->typeToken);
                break;
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                TouchNode(p->type);
                TouchToken(p->identifier);
                break;
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                TouchToken(p->openParenToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->elements);
                TouchToken(p->closeParenToken);
                break;
            }

            case SyntaxKind::ArrayType: {
                ArrayTypeSyntax* p = (ArrayTypeSyntax*)syntaxBase;
                TouchNode(p->elementType);
                TouchSyntaxList((SyntaxListUntyped*)p->ranks);
                break;
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                TouchToken(p->refKeyword);
                TouchToken(p->readonlyKeyword);
                TouchNode(p->type);
                break;
            }

            case SyntaxKind::LabeledStatement: {
                LabeledStatementSyntax* p = (LabeledStatementSyntax*)syntaxBase;
                TouchToken(p->identifier);
                TouchToken(p->colon);
                TouchNode(p->statement);
                break;
            }

            case SyntaxKind::OmittedArraySizeExpression: {
                OmittedArraySizeExpressionSyntax* p = (OmittedArraySizeExpressionSyntax*)syntaxBase;
                TouchToken(p->token);
                break;
            }

            case SyntaxKind::NullableType: {
                NullableTypeSyntax* p = (NullableTypeSyntax*)syntaxBase;
                TouchNode(p->elementType);
                TouchToken(p->questionMark);
                break;
            }

            case SyntaxKind::Argument: {
                ArgumentSyntax* p = (ArgumentSyntax*)syntaxBase;
                TouchNode(p->nameColon);
                TouchToken(p->refKindKeyword);
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::NameEquals: {
                NameEqualsSyntax* p = (NameEqualsSyntax*)syntaxBase;
                TouchNode(p->name);
                TouchToken(p->equalsToken);
                break;
            }

            case SyntaxKind::ImplicitArrayCreationExpression: {
                ImplicitArrayCreationExpressionSyntax* p = (ImplicitArrayCreationExpressionSyntax*)syntaxBase;
                TouchToken(p->newKeyword);
                TouchToken(p->openBracket);
                TouchTokenList(p->commas);
                TouchToken(p->closeBracket);
                TouchNode(p->initializer);
                break;
            }

            case SyntaxKind::ObjectInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                TouchToken(p->openBraceToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                TouchToken(p->closeBraceToken);
                break;
            }
            case SyntaxKind::CollectionInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                TouchToken(p->openBraceToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                TouchToken(p->closeBraceToken);
                break;
            }
            case SyntaxKind::ArrayInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                TouchToken(p->openBraceToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                TouchToken(p->closeBraceToken);
                break;
            }
            case SyntaxKind::ComplexElementInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                TouchToken(p->openBraceToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                TouchToken(p->closeBraceToken);
                break;
            }
            case SyntaxKind::WithInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                TouchToken(p->openBraceToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                TouchToken(p->closeBraceToken);
                break;
            }

            case SyntaxKind::StackAllocArrayCreationExpression: {
                StackAllocArrayCreationExpressionSyntax* p = (StackAllocArrayCreationExpressionSyntax*)syntaxBase;
                TouchToken(p->stackallocKeyword);
                TouchNode(p->type);
                TouchNode(p->initializer);
                break;
            }

            case SyntaxKind::ImplicitStackAllocArrayCreationExpression: {
                ImplicitStackAllocArrayCreationExpressionSyntax* p = (ImplicitStackAllocArrayCreationExpressionSyntax*)syntaxBase;
                TouchToken(p->stackallocKeyword);
                TouchToken(p->openBracket);
                TouchToken(p->closeBracket);
                TouchNode(p->initializer);
                break;
            }

            case SyntaxKind::ArgumentList: {
                ArgumentListSyntax* p = (ArgumentListSyntax*)syntaxBase;
                TouchToken(p->openToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                TouchToken(p->closeToken);
                break;
            }

            case SyntaxKind::ObjectCreationExpression: {
                ObjectCreationExpressionSyntax* p = (ObjectCreationExpressionSyntax*)syntaxBase;
                TouchToken(p->newKeyword);
                TouchNode(p->type);
                TouchNode(p->arguments);
                TouchNode(p->initializer);
                break;
            }

            case SyntaxKind::ImplicitObjectCreationExpression: {
                ImplicitObjectCreationExpressionSyntax* p = (ImplicitObjectCreationExpressionSyntax*)syntaxBase;
                TouchToken(p->newKeyword);
                TouchNode(p->arguments);
                TouchNode(p->initializer);
                break;
            }

            case SyntaxKind::ArrayCreationExpression: {
                ArrayCreationExpressionSyntax* p = (ArrayCreationExpressionSyntax*)syntaxBase;
                TouchToken(p->newKeyword);
                TouchNode(p->type);
                TouchNode(p->initializer);
                break;
            }

            case SyntaxKind::AnonymousObjectMemberDeclarator: {
                AnonymousObjectMemberDeclaratorSyntax* p = (AnonymousObjectMemberDeclaratorSyntax*)syntaxBase;
                TouchNode(p->nameEquals);
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::AnonymousObjectCreationExpression: {
                AnonymousObjectCreationExpressionSyntax* p = (AnonymousObjectCreationExpressionSyntax*)syntaxBase;
                TouchToken(p->newToken);
                TouchToken(p->openBrace);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->initializers);
                TouchToken(p->closeBrace);
                break;
            }

            case SyntaxKind::TupleExpression: {
                TupleExpressionSyntax* p = (TupleExpressionSyntax*)syntaxBase;
                TouchToken(p->openToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                TouchToken(p->closeToken);
                break;
            }

            case SyntaxKind::ParenthesizedExpression: {
                ParenthesizedExpressionSyntax* p = (ParenthesizedExpressionSyntax*)syntaxBase;
                TouchToken(p->openToken);
                TouchNode(p->expression);
                TouchToken(p->closeToken);
                break;
            }

            case SyntaxKind::BracketedArgumentList: {
                BracketedArgumentListSyntax* p = (BracketedArgumentListSyntax*)syntaxBase;
                TouchToken(p->openBracket);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                TouchToken(p->closeBracket);
                break;
            }

            case SyntaxKind::EqualsValueClause: {
                EqualsValueClauseSyntax* p = (EqualsValueClauseSyntax*)syntaxBase;
                TouchToken(p->equalsToken);
                TouchNode(p->value);
                break;
            }

            case SyntaxKind::RefExpression: {
                RefExpressionSyntax* p = (RefExpressionSyntax*)syntaxBase;
                TouchToken(p->refKeyword);
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::VariableDeclarator: {
                VariableDeclaratorSyntax* p = (VariableDeclaratorSyntax*)syntaxBase;
                TouchToken(p->identifier);
                TouchNode(p->argumentList);
                TouchNode(p->initializer);
                break;
            }

            case SyntaxKind::TypeParameter: {
                TypeParameterSyntax* p = (TypeParameterSyntax*)syntaxBase;
                TouchToken(p->identifier);
                break;
            }

            case SyntaxKind::TypeParameterList: {
                TypeParameterListSyntax* p = (TypeParameterListSyntax*)syntaxBase;
                TouchToken(p->lessThanToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->parameters);
                TouchToken(p->greaterThanToken);
                break;
            }

            case SyntaxKind::ArrowExpressionClause: {
                ArrowExpressionClauseSyntax* p = (ArrowExpressionClauseSyntax*)syntaxBase;
                TouchToken(p->arrowToken);
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::Block: {
                BlockSyntax* p = (BlockSyntax*)syntaxBase;
                TouchToken(p->openBraceToken);
                TouchSyntaxList((SyntaxListUntyped*)p->statements);
                TouchToken(p->closeBraceToken);
                break;
            }

            case SyntaxKind::CharacterLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                TouchToken(p->literal);
                break;
            }
            case SyntaxKind::DefaultLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                TouchToken(p->literal);
                break;
            }
            case SyntaxKind::FalseLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                TouchToken(p->literal);
                break;
            }
            case SyntaxKind::NullLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                TouchToken(p->literal);
                break;
            }
            case SyntaxKind::NumericLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                TouchToken(p->literal);
                break;
            }
            case SyntaxKind::StringLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                TouchToken(p->literal);
                break;
            }
            case SyntaxKind::TrueLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                TouchToken(p->literal);
                break;
            }

            case SyntaxKind::CastExpression: {
                CastExpressionSyntax* p = (CastExpressionSyntax*)syntaxBase;
                TouchToken(p->openParen);
                TouchNode(p->type);
                TouchToken(p->closeParen);
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::BaseExpression: {
                BaseExpressionSyntax* p = (BaseExpressionSyntax*)syntaxBase;
                TouchToken(p->keyword);
                break;
            }

            case SyntaxKind::ThisExpression: {
                ThisExpressionSyntax* p = (ThisExpressionSyntax*)syntaxBase;
                TouchToken(p->keyword);
                break;
            }

            case SyntaxKind::DefaultExpression: {
                DefaultExpressionSyntax* p = (DefaultExpressionSyntax*)syntaxBase;
                TouchToken(p->keyword);
                TouchToken(p->openParenToken);
                TouchNode(p->type);
                TouchToken(p->closeParenToken);
                break;
            }

            case SyntaxKind::SizeOfExpression: {
                SizeOfExpressionSyntax* p = (SizeOfExpressionSyntax*)syntaxBase;
                TouchToken(p->keyword);
                TouchToken(p->openParenToken);
                TouchNode(p->type);
                TouchToken(p->closeParenToken);
                break;
            }

            case SyntaxKind::TypeOfExpression: {
                TypeOfExpressionSyntax* p = (TypeOfExpressionSyntax*)syntaxBase;
                TouchToken(p->keyword);
                TouchToken(p->openParenToken);
                TouchNode(p->type);
                TouchToken(p->closeParenToken);
                break;
            }

            case SyntaxKind::DiscardDesignation: {
                DiscardDesignationSyntax* p = (DiscardDesignationSyntax*)syntaxBase;
                TouchToken(p->underscoreToken);
                break;
            }

            case SyntaxKind::SingleVariableDesignation: {
                SingleVariableDesignationSyntax* p = (SingleVariableDesignationSyntax*)syntaxBase;
                TouchToken(p->identifier);
                break;
            }

            case SyntaxKind::ParenthesizedVariableDesignation: {
                ParenthesizedVariableDesignationSyntax* p = (ParenthesizedVariableDesignationSyntax*)syntaxBase;
                TouchToken(p->openParen);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->designators);
                TouchToken(p->closeParen);
                break;
            }

            case SyntaxKind::ExpressionElement: {
                ExpressionElementSyntax* p = (ExpressionElementSyntax*)syntaxBase;
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::SpreadElement: {
                SpreadElementSyntax* p = (SpreadElementSyntax*)syntaxBase;
                TouchToken(p->dotDotToken);
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::CollectionExpression: {
                CollectionExpressionSyntax* p = (CollectionExpressionSyntax*)syntaxBase;
                TouchToken(p->open);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->elements);
                TouchToken(p->close);
                break;
            }

            case SyntaxKind::DeclarationExpression: {
                DeclarationExpressionSyntax* p = (DeclarationExpressionSyntax*)syntaxBase;
                TouchNode(p->type);
                TouchNode(p->designation);
                break;
            }

            case SyntaxKind::ThrowExpression: {
                ThrowExpressionSyntax* p = (ThrowExpressionSyntax*)syntaxBase;
                TouchToken(p->throwKeyword);
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::PostIncrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                TouchNode(p->expression);
                TouchToken(p->operatorToken);
                break;
            }
            case SyntaxKind::PostDecrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                TouchNode(p->expression);
                TouchToken(p->operatorToken);
                break;
            }

            case SyntaxKind::ElementAccessExpression: {
                ElementAccessExpressionSyntax* p = (ElementAccessExpressionSyntax*)syntaxBase;
                TouchNode(p->expression);
                TouchNode(p->argumentList);
                break;
            }

            case SyntaxKind::InvocationExpression: {
                InvocationExpressionSyntax* p = (InvocationExpressionSyntax*)syntaxBase;
                TouchNode(p->expression);
                TouchNode(p->argumentList);
                break;
            }

            case SyntaxKind::ConditionalExpression: {
                ConditionalExpressionSyntax* p = (ConditionalExpressionSyntax*)syntaxBase;
                TouchNode(p->condition);
                TouchToken(p->questionToken);
                TouchNode(p->whenTrue);
                TouchToken(p->colonToken);
                TouchNode(p->whenFalse);
                break;
            }

            case SyntaxKind::RangeExpression: {
                RangeExpressionSyntax* p = (RangeExpressionSyntax*)syntaxBase;
                TouchNode(p->leftOperand);
                TouchToken(p->operatorToken);
                TouchNode(p->rightOperand);
                break;
            }

            case SyntaxKind::UnaryPlusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->operand);
                break;
            }
            case SyntaxKind::UnaryMinusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->operand);
                break;
            }
            case SyntaxKind::BitwiseNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->operand);
                break;
            }
            case SyntaxKind::LogicalNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->operand);
                break;
            }
            case SyntaxKind::PreIncrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->operand);
                break;
            }
            case SyntaxKind::PreDecrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->operand);
                break;
            }
            case SyntaxKind::AddressOfExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->operand);
                break;
            }
            case SyntaxKind::PointerIndirectionExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->operand);
                break;
            }
            case SyntaxKind::IndexExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->operand);
                break;
            }

            case SyntaxKind::Parameter: {
                ParameterSyntax* p = (ParameterSyntax*)syntaxBase;
                TouchTokenList(p->modifiers);
                TouchNode(p->type);
                TouchToken(p->identifier);
                TouchNode(p->defaultValue);
                break;
            }

            case SyntaxKind::SimpleLambdaExpression: {
                SimpleLambdaExpressionSyntax* p = (SimpleLambdaExpressionSyntax*)syntaxBase;
                TouchTokenList(p->modifiers);
                TouchNode(p->parameter);
                TouchToken(p->arrowToken);
                TouchNode(p->blockBody);
                TouchNode(p->expressionBody);
                break;
            }

            case SyntaxKind::ParenthesizedLambdaExpression: {
                ParenthesizedLambdaExpressionSyntax* p = (ParenthesizedLambdaExpressionSyntax*)syntaxBase;
                TouchTokenList(p->modifiers);
                TouchNode(p->returnType);
                TouchNode(p->parameters);
                TouchToken(p->arrowToken);
                TouchNode(p->blockBody);
                TouchNode(p->expressionBody);
                break;
            }

            case SyntaxKind::BaseConstructorInitializer: {
                BaseConstructorInitializerSyntax* p = (BaseConstructorInitializerSyntax*)syntaxBase;
                TouchToken(p->colonToken);
                TouchToken(p->baseKeyword);
                TouchNode(p->argumentListSyntax);
                break;
            }

            case SyntaxKind::ThisConstructorInitializer: {
                ThisConstructorInitializerSyntax* p = (ThisConstructorInitializerSyntax*)syntaxBase;
                TouchToken(p->colonToken);
                TouchToken(p->thisKeyword);
                TouchNode(p->argumentListSyntax);
                break;
            }

            case SyntaxKind::NamedConstructorInitializer: {
                NamedConstructorInitializerSyntax* p = (NamedConstructorInitializerSyntax*)syntaxBase;
                TouchToken(p->colonToken);
                TouchToken(p->name);
                TouchNode(p->argumentListSyntax);
                break;
            }

            case SyntaxKind::ParameterList: {
                ParameterListSyntax* p = (ParameterListSyntax*)syntaxBase;
                TouchToken(p->openParen);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->parameters);
                TouchToken(p->closeParen);
                break;
            }

            case SyntaxKind::ConstraintClauses: {
                ConstraintClausesSyntax* p = (ConstraintClausesSyntax*)syntaxBase;
                TouchToken(p->dummy);
                break;
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                TouchTokenList(p->modifiers);
                TouchNode(p->returnType);
                TouchToken(p->identifier);
                TouchNode(p->typeParameters);
                TouchNode(p->parameters);
                TouchNode(p->constraints);
                TouchNode(p->body);
                break;
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* p = (VariableDeclarationSyntax*)syntaxBase;
                TouchNode(p->type);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->variables);
                break;
            }

            case SyntaxKind::LocalDeclarationStatement: {
                LocalDeclarationStatementSyntax* p = (LocalDeclarationStatementSyntax*)syntaxBase;
                TouchToken(p->usingKeyword);
                TouchTokenList(p->modifiers);
                TouchNode(p->declaration);
                TouchToken(p->semicolon);
                break;
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* p = (FieldDeclarationSyntax*)syntaxBase;
                TouchTokenList(p->modifiers);
                TouchNode(p->declaration);
                TouchToken(p->semicolonToken);
                break;
            }

            case SyntaxKind::ExpressionColon: {
                ExpressionColonSyntax* p = (ExpressionColonSyntax*)syntaxBase;
                TouchNode(p->expression);
                TouchToken(p->colonToken);
                break;
            }

            case SyntaxKind::Subpattern: {
                SubpatternSyntax* p = (SubpatternSyntax*)syntaxBase;
                TouchNode(p->expressionColon);
                TouchNode(p->pattern);
                break;
            }

            case SyntaxKind::PropertyPatternClause: {
                PropertyPatternClauseSyntax* p = (PropertyPatternClauseSyntax*)syntaxBase;
                TouchToken(p->openBraceToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->subpatterns);
                TouchToken(p->closeBraceToken);
                break;
            }

            case SyntaxKind::DeclarationPattern: {
                DeclarationPatternSyntax* p = (DeclarationPatternSyntax*)syntaxBase;
                TouchNode(p->type);
                TouchNode(p->designation);
                break;
            }

            case SyntaxKind::PositionalPatternClause: {
                PositionalPatternClauseSyntax* p = (PositionalPatternClauseSyntax*)syntaxBase;
                TouchToken(p->openParenToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->subpatterns);
                TouchToken(p->closeParenToken);
                break;
            }

            case SyntaxKind::RecursivePattern: {
                RecursivePatternSyntax* p = (RecursivePatternSyntax*)syntaxBase;
                TouchNode(p->type);
                TouchNode(p->positionalPatternClause);
                TouchNode(p->propertyPatternClause);
                TouchNode(p->designation);
                break;
            }

            case SyntaxKind::ParenthesizedPattern: {
                ParenthesizedPatternSyntax* p = (ParenthesizedPatternSyntax*)syntaxBase;
                TouchToken(p->openParenToken);
                TouchNode(p->pattern);
                TouchToken(p->closeParenToken);
                break;
            }

            case SyntaxKind::VarPattern: {
                VarPatternSyntax* p = (VarPatternSyntax*)syntaxBase;
                TouchToken(p->varKeyword);
                TouchNode(p->designation);
                break;
            }

            case SyntaxKind::TypePattern: {
                TypePatternSyntax* p = (TypePatternSyntax*)syntaxBase;
                TouchNode(p->type);
                break;
            }

            case SyntaxKind::ConstantPattern: {
                ConstantPatternSyntax* p = (ConstantPatternSyntax*)syntaxBase;
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::RelationalPattern: {
                RelationalPatternSyntax* p = (RelationalPatternSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::SlicePattern: {
                SlicePatternSyntax* p = (SlicePatternSyntax*)syntaxBase;
                TouchToken(p->dotDotToken);
                TouchNode(p->pattern);
                break;
            }

            case SyntaxKind::DiscardPattern: {
                DiscardPatternSyntax* p = (DiscardPatternSyntax*)syntaxBase;
                TouchToken(p->underscore);
                break;
            }

            case SyntaxKind::NotPattern: {
                UnaryPatternSyntax* p = (UnaryPatternSyntax*)syntaxBase;
                TouchToken(p->operatorToken);
                TouchNode(p->pattern);
                break;
            }

            case SyntaxKind::OrPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::AndPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }

            case SyntaxKind::IsPatternExpression: {
                IsPatternExpressionSyntax* p = (IsPatternExpressionSyntax*)syntaxBase;
                TouchNode(p->leftOperand);
                TouchToken(p->opToken);
                TouchNode(p->pattern);
                break;
            }

            case SyntaxKind::AddExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::SubtractExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::MultiplyExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::DivideExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::ModuloExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::LeftShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::RightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::UnsignedRightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::LogicalOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::LogicalAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::BitwiseOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::BitwiseAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::ExclusiveOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::EqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::NotEqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::LessThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::LessThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::GreaterThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::GreaterThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::IsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::AsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::CoalesceExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }

            case SyntaxKind::ImplicitElementAccess: {
                ImplicitElementAccessSyntax* p = (ImplicitElementAccessSyntax*)syntaxBase;
                TouchNode(p->argumentList);
                break;
            }

            case SyntaxKind::WhenClause: {
                WhenClauseSyntax* p = (WhenClauseSyntax*)syntaxBase;
                TouchToken(p->whenKeyword);
                TouchNode(p->condition);
                break;
            }

            case SyntaxKind::SwitchExpressionArm: {
                SwitchExpressionArmSyntax* p = (SwitchExpressionArmSyntax*)syntaxBase;
                TouchNode(p->pattern);
                TouchNode(p->whenClause);
                TouchToken(p->equalsGreaterThanToken);
                TouchNode(p->expression);
                break;
            }

            case SyntaxKind::SwitchExpression: {
                SwitchExpressionSyntax* p = (SwitchExpressionSyntax*)syntaxBase;
                TouchNode(p->governingExpression);
                TouchToken(p->switchKeyword);
                TouchToken(p->openBraceToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arms);
                TouchToken(p->closeBraceToken);
                break;
            }

            case SyntaxKind::ListPattern: {
                ListPatternSyntax* p = (ListPatternSyntax*)syntaxBase;
                TouchToken(p->openBracketToken);
                TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->patterns);
                TouchToken(p->closeBracketToken);
                TouchNode(p->designation);
                break;
            }

            case SyntaxKind::SimpleAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::AddAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::SubtractAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::MultiplyAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::DivideAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::ModuloAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::AndAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::ExclusiveOrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::OrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::LeftShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::RightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::UnsignedRightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }
            case SyntaxKind::CoalesceAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                TouchNode(p->left);
                TouchToken(p->operatorToken);
                TouchNode(p->right);
                break;
            }

            case SyntaxKind::ForEachStatement: {
                ForEachStatementSyntax* p = (ForEachStatementSyntax*)syntaxBase;
                TouchToken(p->foreachKeyword);
                TouchToken(p->openParen);
                TouchNode(p->type);
                TouchToken(p->identifier);
                TouchToken(p->inKeyword);
                TouchNode(p->expression);
                TouchToken(p->closeParen);
                TouchNode(p->statement);
                break;
            }

            case SyntaxKind::ForEachVariableStatement: {
                ForEachVariableStatementSyntax* p = (ForEachVariableStatementSyntax*)syntaxBase;
                TouchToken(p->foreachKeyword);
                TouchToken(p->openParen);
                TouchNode(p->variable);
                TouchToken(p->inKeyword);
                TouchNode(p->expression);
                TouchToken(p->closeParen);
                TouchNode(p->statement);
                break;
            }

            case SyntaxKind::GotoCaseStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                TouchToken(p->gotoToken);
                TouchToken(p->caseOrDefault);
                TouchNode(p->arg);
                TouchToken(p->semicolon);
                break;
            }
            case SyntaxKind::GotoDefaultStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                TouchToken(p->gotoToken);
                TouchToken(p->caseOrDefault);
                TouchNode(p->arg);
                TouchToken(p->semicolon);
                break;
            }
            case SyntaxKind::GotoStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                TouchToken(p->gotoToken);
                TouchToken(p->caseOrDefault);
                TouchNode(p->arg);
                TouchToken(p->semicolon);
                break;
            }

            case SyntaxKind::ElseClause: {
                ElseClauseSyntax* p = (ElseClauseSyntax*)syntaxBase;
                TouchToken(p->elseKeyword);
                TouchNode(p->statement);
                break;
            }

            case SyntaxKind::IfStatement: {
                IfStatementSyntax* p = (IfStatementSyntax*)syntaxBase;
                TouchToken(p->ifKeyword);
                TouchToken(p->openParen);
                TouchNode(p->condition);
                TouchToken(p->closeParen);
                TouchNode(p->statement);
                TouchNode(p->elseClause);
                break;
            }

            case SyntaxKind::ExpressionStatement: {
                ExpressionStatementSyntax* p = (ExpressionStatementSyntax*)syntaxBase;
                TouchNode(p->expression);
                TouchToken(p->semicolon);
                break;
            }

            case SyntaxKind::ReturnStatement: {
                ReturnStatementSyntax* p = (ReturnStatementSyntax*)syntaxBase;
                TouchToken(p->returnKeyword);
                TouchNode(p->expressionSyntax);
                TouchToken(p->semicolon);
                break;
            }

            default: {
                UNREACHABLE("TouchNode");
                return;
            }
            
        }        
    }
    
}
