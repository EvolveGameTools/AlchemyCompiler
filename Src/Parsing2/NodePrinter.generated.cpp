#include "./NodePrinter.h"

namespace Alchemy::Compilation {

    void NodePrinter::PrintNode(SyntaxBase* syntaxBase) {

        if (syntaxBase == nullptr) {
            PrintLine("nullptr");
            return;
        }

        switch (syntaxBase->GetKind()) {
            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*)syntaxBase;
                PrintNodeHeader("ArrayRankSpecifierSyntax", syntaxBase);
                indent++;
                PrintFieldName("open");
                PrintToken(p->open);
                PrintFieldName("ranks");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->ranks);
                PrintFieldName("close");
                PrintToken(p->close);
                indent--;
                break;
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* p = (TypeArgumentListSyntax*)syntaxBase;
                PrintNodeHeader("TypeArgumentListSyntax", syntaxBase);
                indent++;
                PrintFieldName("lessThanToken");
                PrintToken(p->lessThanToken);
                PrintFieldName("arguments");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                PrintFieldName("greaterThanToken");
                PrintToken(p->greaterThanToken);
                indent--;
                break;
            }

            case SyntaxKind::GenericName: {
                GenericNameSyntax* p = (GenericNameSyntax*)syntaxBase;
                PrintNodeHeader("GenericNameSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeArgumentList");
                PrintNode(p->typeArgumentList);
                indent--;
                break;
            }

            case SyntaxKind::ElementBindingExpression: {
                ElementBindingExpressionSyntax* p = (ElementBindingExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ElementBindingExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                indent--;
                break;
            }

            case SyntaxKind::MemberBindingExpression: {
                MemberBindingExpressionSyntax* p = (MemberBindingExpressionSyntax*)syntaxBase;
                PrintNodeHeader("MemberBindingExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("name");
                PrintNode(p->name);
                indent--;
                break;
            }

            case SyntaxKind::ConditionalAccessExpression: {
                ConditionalAccessExpressionSyntax* p = (ConditionalAccessExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ConditionalAccessExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("whenNotNull");
                PrintNode(p->whenNotNull);
                indent--;
                break;
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* p = (QualifiedNameSyntax*)syntaxBase;
                PrintNodeHeader("QualifiedNameSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("dotToken");
                PrintToken(p->dotToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }

            case SyntaxKind::IdentifierName: {
                IdentifierNameSyntax* p = (IdentifierNameSyntax*)syntaxBase;
                PrintNodeHeader("IdentifierNameSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::NameColon: {
                NameColonSyntax* p = (NameColonSyntax*)syntaxBase;
                PrintNodeHeader("NameColonSyntax", syntaxBase);
                indent++;
                PrintFieldName("name");
                PrintNode(p->name);
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                indent--;
                break;
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                PrintNodeHeader("PredefinedTypeSyntax", syntaxBase);
                indent++;
                PrintFieldName("typeToken");
                PrintToken(p->typeToken);
                indent--;
                break;
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                PrintNodeHeader("TupleElementSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                PrintNodeHeader("TupleTypeSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("elements");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->elements);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::ArrayType: {
                ArrayTypeSyntax* p = (ArrayTypeSyntax*)syntaxBase;
                PrintNodeHeader("ArrayTypeSyntax", syntaxBase);
                indent++;
                PrintFieldName("elementType");
                PrintNode(p->elementType);
                PrintFieldName("ranks");
                PrintSyntaxList((SyntaxListUntyped*)p->ranks);
                indent--;
                break;
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                PrintNodeHeader("RefTypeSyntax", syntaxBase);
                indent++;
                PrintFieldName("refKeyword");
                PrintToken(p->refKeyword);
                PrintFieldName("readonlyKeyword");
                PrintToken(p->readonlyKeyword);
                PrintFieldName("type");
                PrintNode(p->type);
                indent--;
                break;
            }

            case SyntaxKind::OmittedArraySizeExpression: {
                OmittedArraySizeExpressionSyntax* p = (OmittedArraySizeExpressionSyntax*)syntaxBase;
                PrintNodeHeader("OmittedArraySizeExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("token");
                PrintToken(p->token);
                indent--;
                break;
            }

            case SyntaxKind::NullableType: {
                NullableType* p = (NullableType*)syntaxBase;
                PrintNodeHeader("NullableType", syntaxBase);
                indent++;
                PrintFieldName("elementType");
                PrintNode(p->elementType);
                PrintFieldName("questionMark");
                PrintToken(p->questionMark);
                indent--;
                break;
            }

            case SyntaxKind::Argument: {
                ArgumentSyntax* p = (ArgumentSyntax*)syntaxBase;
                PrintNodeHeader("ArgumentSyntax", syntaxBase);
                indent++;
                PrintFieldName("nameColon");
                PrintNode(p->nameColon);
                PrintFieldName("refKindKeyword");
                PrintToken(p->refKindKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::NameEquals: {
                NameEqualsSyntax* p = (NameEqualsSyntax*)syntaxBase;
                PrintNodeHeader("NameEqualsSyntax", syntaxBase);
                indent++;
                PrintFieldName("name");
                PrintNode(p->name);
                PrintFieldName("equalsToken");
                PrintToken(p->equalsToken);
                indent--;
                break;
            }

            case SyntaxKind::ImplicitArrayCreationExpression: {
                ImplicitArrayCreationExpressionSyntax* p = (ImplicitArrayCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ImplicitArrayCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("newKeyword");
                PrintToken(p->newKeyword);
                PrintFieldName("openBracket");
                PrintToken(p->openBracket);
                PrintFieldName("commas");
                PrintTokenList(p->commas);
                PrintFieldName("closeBracket");
                PrintToken(p->closeBracket);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::ObjectInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }
            case SyntaxKind::CollectionInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }
            case SyntaxKind::ArrayInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }
            case SyntaxKind::ComplexElementInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }
            case SyntaxKind::WithInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }

            case SyntaxKind::StackAllocArrayCreationExpression: {
                StackAllocArrayCreationExpressionSyntax* p = (StackAllocArrayCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("StackAllocArrayCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("stackallocKeyword");
                PrintToken(p->stackallocKeyword);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::ImplicitStackAllocArrayCreationExpression: {
                ImplicitStackAllocArrayCreationExpressionSyntax* p = (ImplicitStackAllocArrayCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ImplicitStackAllocArrayCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("stackallocKeyword");
                PrintToken(p->stackallocKeyword);
                PrintFieldName("openBracket");
                PrintToken(p->openBracket);
                PrintFieldName("closeBracket");
                PrintToken(p->closeBracket);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::ArgumentList: {
                ArgumentListSyntax* p = (ArgumentListSyntax*)syntaxBase;
                PrintNodeHeader("ArgumentListSyntax", syntaxBase);
                indent++;
                PrintFieldName("openToken");
                PrintToken(p->openToken);
                PrintFieldName("arguments");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                PrintFieldName("closeToken");
                PrintToken(p->closeToken);
                indent--;
                break;
            }

            case SyntaxKind::ObjectCreationExpression: {
                ObjectCreationExpressionSyntax* p = (ObjectCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ObjectCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("newKeyword");
                PrintToken(p->newKeyword);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("arguments");
                PrintNode(p->arguments);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::ImplicitObjectCreationExpression: {
                ImplicitObjectCreationExpressionSyntax* p = (ImplicitObjectCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ImplicitObjectCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("newKeyword");
                PrintToken(p->newKeyword);
                PrintFieldName("arguments");
                PrintNode(p->arguments);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::ArrayCreationExpression: {
                ArrayCreationExpressionSyntax* p = (ArrayCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ArrayCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("newKeyword");
                PrintToken(p->newKeyword);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::AnonymousObjectMemberDeclarator: {
                AnonymousObjectMemberDeclaratorSyntax* p = (AnonymousObjectMemberDeclaratorSyntax*)syntaxBase;
                PrintNodeHeader("AnonymousObjectMemberDeclaratorSyntax", syntaxBase);
                indent++;
                PrintFieldName("nameEquals");
                PrintNode(p->nameEquals);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::AnonymousObjectCreationExpression: {
                AnonymousObjectCreationExpressionSyntax* p = (AnonymousObjectCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AnonymousObjectCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("newToken");
                PrintToken(p->newToken);
                PrintFieldName("openBrace");
                PrintToken(p->openBrace);
                PrintFieldName("initializers");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->initializers);
                PrintFieldName("closeBrace");
                PrintToken(p->closeBrace);
                indent--;
                break;
            }

            case SyntaxKind::TupleExpression: {
                TupleExpressionSyntax* p = (TupleExpressionSyntax*)syntaxBase;
                PrintNodeHeader("TupleExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openToken");
                PrintToken(p->openToken);
                PrintFieldName("arguments");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                PrintFieldName("closeToken");
                PrintToken(p->closeToken);
                indent--;
                break;
            }

            case SyntaxKind::ParenthesizedExpression: {
                ParenthesizedExpressionSyntax* p = (ParenthesizedExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ParenthesizedExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openToken");
                PrintToken(p->openToken);
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("closeToken");
                PrintToken(p->closeToken);
                indent--;
                break;
            }

            case SyntaxKind::BracketedArgumentList: {
                BracketedArgumentListSyntax* p = (BracketedArgumentListSyntax*)syntaxBase;
                PrintNodeHeader("BracketedArgumentListSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBracket");
                PrintToken(p->openBracket);
                PrintFieldName("arguments");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                PrintFieldName("closeBracket");
                PrintToken(p->closeBracket);
                indent--;
                break;
            }

            case SyntaxKind::EqualsValueClause: {
                EqualsValueClauseSyntax* p = (EqualsValueClauseSyntax*)syntaxBase;
                PrintNodeHeader("EqualsValueClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("equalsToken");
                PrintToken(p->equalsToken);
                PrintFieldName("value");
                PrintNode(p->value);
                indent--;
                break;
            }

            case SyntaxKind::RefExpression: {
                RefExpressionSyntax* p = (RefExpressionSyntax*)syntaxBase;
                PrintNodeHeader("RefExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("refKeyword");
                PrintToken(p->refKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::VariableDeclarator: {
                VariableDeclaratorSyntax* p = (VariableDeclaratorSyntax*)syntaxBase;
                PrintNodeHeader("VariableDeclaratorSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::TypeParameter: {
                TypeParameterSyntax* p = (TypeParameterSyntax*)syntaxBase;
                PrintNodeHeader("TypeParameterSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::TypeParameterList: {
                TypeParameterListSyntax* p = (TypeParameterListSyntax*)syntaxBase;
                PrintNodeHeader("TypeParameterListSyntax", syntaxBase);
                indent++;
                PrintFieldName("lessThanToken");
                PrintToken(p->lessThanToken);
                PrintFieldName("parameters");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->parameters);
                PrintFieldName("greaterThanToken");
                PrintToken(p->greaterThanToken);
                indent--;
                break;
            }

            case SyntaxKind::ArrowExpressionClause: {
                ArrowExpressionClauseSyntax* p = (ArrowExpressionClauseSyntax*)syntaxBase;
                PrintNodeHeader("ArrowExpressionClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("arrowToken");
                PrintToken(p->arrowToken);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::Block: {
                BlockSyntax* p = (BlockSyntax*)syntaxBase;
                PrintNodeHeader("BlockSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("statements");
                PrintSyntaxList((SyntaxListUntyped*)p->statements);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }

            case SyntaxKind::CharacterLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::DefaultLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::FalseLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::NullLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::NumericLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::StringLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::TrueLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }

            case SyntaxKind::CastExpression: {
                CastExpressionSyntax* p = (CastExpressionSyntax*)syntaxBase;
                PrintNodeHeader("CastExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::BaseExpression: {
                BaseExpressionSyntax* p = (BaseExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BaseExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                indent--;
                break;
            }

            case SyntaxKind::ThisExpression: {
                ThisExpressionSyntax* p = (ThisExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ThisExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                indent--;
                break;
            }

            case SyntaxKind::DefaultExpression: {
                DefaultExpressionSyntax* p = (DefaultExpressionSyntax*)syntaxBase;
                PrintNodeHeader("DefaultExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::SizeOfExpression: {
                SizeOfExpressionSyntax* p = (SizeOfExpressionSyntax*)syntaxBase;
                PrintNodeHeader("SizeOfExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::TypeOfExpression: {
                TypeOfExpressionSyntax* p = (TypeOfExpressionSyntax*)syntaxBase;
                PrintNodeHeader("TypeOfExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::DiscardDesignation: {
                DiscardDesignationSyntax* p = (DiscardDesignationSyntax*)syntaxBase;
                PrintNodeHeader("DiscardDesignationSyntax", syntaxBase);
                indent++;
                PrintFieldName("underscoreToken");
                PrintToken(p->underscoreToken);
                indent--;
                break;
            }

            case SyntaxKind::SingleVariableDesignation: {
                SingleVariableDesignationSyntax* p = (SingleVariableDesignationSyntax*)syntaxBase;
                PrintNodeHeader("SingleVariableDesignationSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::ParenthesizedVariableDesignation: {
                ParenthesizedVariableDesignationSyntax* p = (ParenthesizedVariableDesignationSyntax*)syntaxBase;
                PrintNodeHeader("ParenthesizedVariableDesignationSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("designators");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->designators);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                indent--;
                break;
            }

            case SyntaxKind::DeclarationExpression: {
                DeclarationExpressionSyntax* p = (DeclarationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("DeclarationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("designation");
                PrintNode(p->designation);
                indent--;
                break;
            }

            case SyntaxKind::ThrowExpression: {
                ThrowExpressionSyntax* p = (ThrowExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ThrowExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("throwKeyword");
                PrintToken(p->throwKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::PostIncrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PostfixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                indent--;
                break;
            }
            case SyntaxKind::PostDecrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PostfixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                indent--;
                break;
            }

            case SyntaxKind::ElementAccessExpression: {
                ElementAccessExpressionSyntax* p = (ElementAccessExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ElementAccessExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                indent--;
                break;
            }

            case SyntaxKind::InvocationExpression: {
                InvocationExpressionSyntax* p = (InvocationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InvocationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                indent--;
                break;
            }

            case SyntaxKind::ConditionalExpression: {
                ConditionalExpressionSyntax* p = (ConditionalExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ConditionalExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("condition");
                PrintNode(p->condition);
                PrintFieldName("questionToken");
                PrintToken(p->questionToken);
                PrintFieldName("whenTrue");
                PrintNode(p->whenTrue);
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("whenFalse");
                PrintNode(p->whenFalse);
                indent--;
                break;
            }

            case SyntaxKind::RangeExpression: {
                RangeExpressionSyntax* p = (RangeExpressionSyntax*)syntaxBase;
                PrintNodeHeader("RangeExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("leftOperand");
                PrintNode(p->leftOperand);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("rightOperand");
                PrintNode(p->rightOperand);
                indent--;
                break;
            }

            case SyntaxKind::UnaryPlusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::UnaryMinusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::BitwiseNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::LogicalNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::PreIncrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::PreDecrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::AddressOfExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::PointerIndirectionExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::IndexExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }

            case SyntaxKind::Parameter: {
                ParameterSyntax* p = (ParameterSyntax*)syntaxBase;
                PrintNodeHeader("ParameterSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("defaultValue");
                PrintNode(p->defaultValue);
                indent--;
                break;
            }

            case SyntaxKind::BaseConstructorInitializer: {
                BaseConstructorInitializerSyntax* p = (BaseConstructorInitializerSyntax*)syntaxBase;
                PrintNodeHeader("BaseConstructorInitializerSyntax", syntaxBase);
                indent++;
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("baseKeyword");
                PrintToken(p->baseKeyword);
                PrintFieldName("argumentListSyntax");
                PrintNode(p->argumentListSyntax);
                indent--;
                break;
            }

            case SyntaxKind::ThisConstructorInitializer: {
                ThisConstructorInitializerSyntax* p = (ThisConstructorInitializerSyntax*)syntaxBase;
                PrintNodeHeader("ThisConstructorInitializerSyntax", syntaxBase);
                indent++;
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("thisKeyword");
                PrintToken(p->thisKeyword);
                PrintFieldName("argumentListSyntax");
                PrintNode(p->argumentListSyntax);
                indent--;
                break;
            }

            case SyntaxKind::NamedConstructorInitializer: {
                NamedConstructorInitializerSyntax* p = (NamedConstructorInitializerSyntax*)syntaxBase;
                PrintNodeHeader("NamedConstructorInitializerSyntax", syntaxBase);
                indent++;
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("name");
                PrintToken(p->name);
                PrintFieldName("argumentListSyntax");
                PrintNode(p->argumentListSyntax);
                indent--;
                break;
            }

            case SyntaxKind::ParameterList: {
                ParameterListSyntax* p = (ParameterListSyntax*)syntaxBase;
                PrintNodeHeader("ParameterListSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("parameters");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->parameters);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                indent--;
                break;
            }

            case SyntaxKind::ConstraintClauses: {
                ConstraintClausesSyntax* p = (ConstraintClausesSyntax*)syntaxBase;
                PrintNodeHeader("ConstraintClausesSyntax", syntaxBase);
                indent++;
                PrintFieldName("dummy");
                PrintToken(p->dummy);
                indent--;
                break;
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                PrintNodeHeader("LocalFunctionStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("returnType");
                PrintNode(p->returnType);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeParameters");
                PrintNode(p->typeParameters);
                PrintFieldName("parameters");
                PrintNode(p->parameters);
                PrintFieldName("constraints");
                PrintNode(p->constraints);
                PrintFieldName("body");
                PrintNode(p->body);
                indent--;
                break;
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* p = (VariableDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("VariableDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("variables");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->variables);
                indent--;
                break;
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* p = (FieldDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("FieldDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("declaration");
                PrintNode(p->declaration);
                PrintFieldName("semicolonToken");
                PrintToken(p->semicolonToken);
                indent--;
                break;
            }

            case SyntaxKind::ExpressionColon: {
                ExpressionColonSyntax* p = (ExpressionColonSyntax*)syntaxBase;
                PrintNodeHeader("ExpressionColonSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                indent--;
                break;
            }

            case SyntaxKind::Subpattern: {
                SubpatternSyntax* p = (SubpatternSyntax*)syntaxBase;
                PrintNodeHeader("SubpatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("expressionColon");
                PrintNode(p->expressionColon);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                indent--;
                break;
            }

            case SyntaxKind::PropertyPatternClause: {
                PropertyPatternClauseSyntax* p = (PropertyPatternClauseSyntax*)syntaxBase;
                PrintNodeHeader("PropertyPatternClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("subpatterns");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->subpatterns);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }

            case SyntaxKind::DeclarationPattern: {
                DeclarationPatternSyntax* p = (DeclarationPatternSyntax*)syntaxBase;
                PrintNodeHeader("DeclarationPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("designation");
                PrintNode(p->designation);
                indent--;
                break;
            }

            case SyntaxKind::PositionalPatternClause: {
                PositionalPatternClauseSyntax* p = (PositionalPatternClauseSyntax*)syntaxBase;
                PrintNodeHeader("PositionalPatternClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("subpatterns");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->subpatterns);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::RecursivePattern: {
                RecursivePatternSyntax* p = (RecursivePatternSyntax*)syntaxBase;
                PrintNodeHeader("RecursivePatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("positionalPatternClause");
                PrintNode(p->positionalPatternClause);
                PrintFieldName("propertyPatternClause");
                PrintNode(p->propertyPatternClause);
                PrintFieldName("designation");
                PrintNode(p->designation);
                indent--;
                break;
            }

            case SyntaxKind::ParenthesizedPattern: {
                ParenthesizedPatternSyntax* p = (ParenthesizedPatternSyntax*)syntaxBase;
                PrintNodeHeader("ParenthesizedPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::VarPattern: {
                VarPatternSyntax* p = (VarPatternSyntax*)syntaxBase;
                PrintNodeHeader("VarPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("varKeyword");
                PrintToken(p->varKeyword);
                PrintFieldName("designation");
                PrintNode(p->designation);
                indent--;
                break;
            }

            case SyntaxKind::TypePattern: {
                TypePatternSyntax* p = (TypePatternSyntax*)syntaxBase;
                PrintNodeHeader("TypePatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                indent--;
                break;
            }

            case SyntaxKind::ConstantPattern: {
                ConstantPatternSyntax* p = (ConstantPatternSyntax*)syntaxBase;
                PrintNodeHeader("ConstantPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::RelationalPattern: {
                RelationalPatternSyntax* p = (RelationalPatternSyntax*)syntaxBase;
                PrintNodeHeader("RelationalPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::SlicePattern: {
                SlicePatternSyntax* p = (SlicePatternSyntax*)syntaxBase;
                PrintNodeHeader("SlicePatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("dotDotToken");
                PrintToken(p->dotDotToken);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                indent--;
                break;
            }

            case SyntaxKind::DiscardPattern: {
                DiscardPatternSyntax* p = (DiscardPatternSyntax*)syntaxBase;
                PrintNodeHeader("DiscardPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("underscore");
                PrintToken(p->underscore);
                indent--;
                break;
            }

            case SyntaxKind::NotPattern: {
                UnaryPatternSyntax* p = (UnaryPatternSyntax*)syntaxBase;
                PrintNodeHeader("UnaryPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                indent--;
                break;
            }

            case SyntaxKind::OrPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                PrintNodeHeader("BinaryPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::AndPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                PrintNodeHeader("BinaryPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }

            case SyntaxKind::IsPatternExpression: {
                IsPatternExpressionSyntax* p = (IsPatternExpressionSyntax*)syntaxBase;
                PrintNodeHeader("IsPatternExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("leftOperand");
                PrintNode(p->leftOperand);
                PrintFieldName("opToken");
                PrintToken(p->opToken);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                indent--;
                break;
            }

            case SyntaxKind::AddExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::SubtractExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::MultiplyExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::DivideExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::ModuloExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LeftShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::RightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::UnsignedRightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LogicalOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LogicalAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::BitwiseOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::BitwiseAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::ExclusiveOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::EqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::NotEqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LessThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LessThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::GreaterThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::GreaterThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::IsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::AsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::CoalesceExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }

            case SyntaxKind::SimpleAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::AddAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::SubtractAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::MultiplyAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::DivideAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::ModuloAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::AndAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::ExclusiveOrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::OrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LeftShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::RightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::UnsignedRightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::CoalesceAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }

            default: {
                break;
            }
            
        }
    }
}
