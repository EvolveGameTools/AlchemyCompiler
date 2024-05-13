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
                PrintLine("ArrayRankSpecifierSyntax");
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
                PrintLine("TypeArgumentListSyntax");
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
                PrintLine("GenericNameSyntax");
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeArgumentList");
                PrintNode(p->typeArgumentList);
                indent--;
                break;
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* p = (QualifiedNameSyntax*)syntaxBase;
                PrintLine("QualifiedNameSyntax");
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
                PrintLine("IdentifierNameSyntax");
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                PrintLine("PredefinedTypeSyntax");
                indent++;
                PrintFieldName("typeToken");
                PrintToken(p->typeToken);
                indent--;
                break;
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                PrintLine("TupleElementSyntax");
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
                PrintLine("TupleTypeSyntax");
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
                PrintLine("ArrayTypeSyntax");
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
                PrintLine("RefTypeSyntax");
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
                PrintLine("OmittedArraySizeExpressionSyntax");
                indent++;
                PrintFieldName("token");
                PrintToken(p->token);
                indent--;
                break;
            }

            case SyntaxKind::NullableType: {
                NullableType* p = (NullableType*)syntaxBase;
                PrintLine("NullableType");
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
                PrintLine("ArgumentSyntax");
                indent++;
                PrintFieldName("nameColon");
                PrintToken(p->nameColon);
                PrintFieldName("refKindKeyword");
                PrintToken(p->refKindKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::BracketedArgumentList: {
                BracketedArgumentListSyntax* p = (BracketedArgumentListSyntax*)syntaxBase;
                PrintLine("BracketedArgumentListSyntax");
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
                PrintLine("EqualsValueClauseSyntax");
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
                PrintLine("RefExpressionSyntax");
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
                PrintLine("VariableDeclaratorSyntax");
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
                PrintLine("TypeParameterSyntax");
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::TypeParameterList: {
                TypeParameterListSyntax* p = (TypeParameterListSyntax*)syntaxBase;
                PrintLine("TypeParameterListSyntax");
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

            case SyntaxKind::Parameter: {
                ParameterSyntax* p = (ParameterSyntax*)syntaxBase;
                PrintLine("ParameterSyntax");
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

            case SyntaxKind::ParameterList: {
                ParameterListSyntax* p = (ParameterListSyntax*)syntaxBase;
                PrintLine("ParameterListSyntax");
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
                PrintLine("ConstraintClausesSyntax");
                indent++;
                PrintFieldName("dummy");
                PrintToken(p->dummy);
                indent--;
                break;
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                PrintLine("LocalFunctionStatementSyntax");
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
                PrintLine("VariableDeclarationSyntax");
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
                PrintLine("FieldDeclarationSyntax");
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

            default: {
                break;
            }
            
        }
    }
}
