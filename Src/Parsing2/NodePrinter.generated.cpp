#include "./NodePrinter.h"

namespace Alchemy::Compilation {

    void NodePrinter::PrintNode(SyntaxBase* syntaxBase) {

        if (syntaxBase == nullptr) {
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

            default: {
                break;
            }
            
        }
    }

    void NodePrinter::Dump() {
        printf("%.*s", buffer.size - 2, buffer.array);
    }
}
