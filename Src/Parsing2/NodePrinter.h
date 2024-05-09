#pragma once

#include "../PrimitiveTypes.h"
#include "./SyntaxNodes.h"
#include "./SyntaxKind.h"

namespace Alchemy::Compilation {

    struct NodePrinter {

        bool printTrivia;
        bool printSkipped;
        bool printComments;
        bool printWhitespace;

        CheckedArray<SyntaxToken> hotTokens;
        CheckedArray<SyntaxTokenCold> coldTokens;
        PodList<char> buffer;

        int32 indent;

        NodePrinter(CheckedArray<SyntaxToken> hotTokens, CheckedArray<SyntaxTokenCold> coldTokens)
            : hotTokens(hotTokens)
            , coldTokens(coldTokens)
            , indent(0)
            , printComments(true)
            , printWhitespace(true)
            , printTrivia(true)
            , printSkipped(true)
            , buffer(2048)
            {}

        void PrintIndent() {
            for (int32 i = 0; i < 4 * indent; i++) {
                buffer.Add(' ');
            }
        }

        void PrintSyntaxList(SyntaxListUntyped* list) {
            if (list == nullptr) {
                return;
            }
        }

        void PrintSeparatedSyntaxList(SeparatedSyntaxListUntyped* list) {
            if (list == nullptr) {
                return;
            }
        }

        void PrintLine() {
            buffer.Add('\n');
        }

        void PrintLine(char* str, int32 length) {
            buffer.Add('\n');
            PrintIndent();
            PrintInline(str, length);
        }

        void PrintLine(const char* str) {
            buffer.Add('\n');
            PrintIndent();
            PrintInline(str);
        }

        void PrintInline(char* str, int32 length) {
            if (length == 0) return;

            for (int32 i = 0; i < length; i++) {
                buffer.Add(str[i]);
            }
        }

        void PrintInline(const char* str) {
            PrintInline((char*) str, strlen(str));
        }

        void PrintTrivia(Trivia trivia) {

            if (trivia.type == TriviaType::Whitespace || trivia.type == TriviaType::NewLine && printWhitespace) {
                PrintLine(trivia.span, trivia.length);
            }
            else if (trivia.type == TriviaType::SingleLineComment || trivia.type == TriviaType::MultiLineComment && printComments) {
                PrintLine(trivia.span, trivia.length);
            }
        }

        void PrintToken(SyntaxToken token) {
            if (!token.IsValid()) {
                return;
            }

            if (!token.IsMissing()) {

                SyntaxTokenCold cold = coldTokens[token.id];

                for (int32 i = 0; i < cold.triviaCount; i++) {
                    Trivia trivia = cold.triviaList[i];
                    if (trivia.isLeading) {
                        PrintTrivia(trivia);
                    }
                }

                PrintInline(SyntaxKindToString(token.kind));
                if (token.contextualKind != SyntaxKind::None) {
                    PrintInline(" (");
                    SyntaxKindToString(token.contextualKind);
                    PrintInline(" )");
                }

                PrintInline(" -> ");
                PrintLine(cold.text, cold.textSize);

                for (int32 i = 0; i < cold.triviaCount; i++) {
                    Trivia trivia = cold.triviaList[i];
                    if (trivia.isTrailing) {
                        PrintTrivia(trivia);
                    }
                }

            }

        }

        void PrintNode(SyntaxBase* syntaxBase) {

            if (syntaxBase == nullptr) {
                return;
            }

            indent++;
            switch (syntaxBase->kind) {
                default: {
                    break;
                }
                case SyntaxKind::ArrayRankSpecifier: {
                    ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*) syntaxBase;
                    PrintLine("ArrayRankSpecifierSyntax");
                    indent++;
                    PrintToken(p->open);
                    PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*) p->ranks);
                    PrintToken(p->close);
                    indent--;
                    break;
                }

                case SyntaxKind::TypeArgumentList: {
                    TypeArgumentListSyntax* p = (TypeArgumentListSyntax*) syntaxBase;
                    PrintLine("TypeArgumentListSyntax");
                    indent++;
                    PrintToken(p->lessThanToken);
                    PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*) p->arguments);
                    PrintToken(p->greaterThanToken);
                    indent--;
                    break;
                }

                case SyntaxKind::GenericName: {
                    GenericNameSyntax* p = (GenericNameSyntax*) syntaxBase;
                    PrintLine("GenericNameSyntax");
                    indent++;
                    PrintToken(p->identifier);
                    PrintNode(p->typeArgumentList);
                    indent--;
                    break;
                }

                case SyntaxKind::QualifiedName: {
                    QualifiedNameSyntax* p = (QualifiedNameSyntax*) syntaxBase;
                    PrintLine("QualifiedNameSyntax");
                    indent++;
                    PrintNode(p->left);
                    PrintToken(p->dotToken);
                    PrintNode(p->right);
                    indent--;
                    break;
                }

                case SyntaxKind::IdentifierName: {
                    IdentifierNameSyntax* p = (IdentifierNameSyntax*) syntaxBase;
                    PrintLine("IdentifierNameSyntax");
                    indent++;
                    PrintToken(p->identifier);
                    indent--;
                    break;
                }

                case SyntaxKind::PredefinedType: {
                    PredefinedTypeSyntax* p = (PredefinedTypeSyntax*) syntaxBase;
                    PrintLine("PredefinedTypeSyntax");
                    indent++;
                    PrintToken(p->typeToken);
                    indent--;
                    break;
                }

                case SyntaxKind::TupleElement: {
                    TupleElementSyntax* p = (TupleElementSyntax*) syntaxBase;
                    PrintLine("TupleElementSyntax");
                    indent++;
                    PrintNode(p->type);
                    PrintToken(p->identifier);
                    indent--;
                    break;
                }

                case SyntaxKind::TupleType: {
                    TupleTypeSyntax* p = (TupleTypeSyntax*) syntaxBase;
                    PrintLine("TupleTypeSyntax");
                    indent++;
                    PrintToken(p->openParenToken);
                    PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*) p->elements);
                    PrintToken(p->closeParenToken);
                    indent--;
                    break;
                }

                case SyntaxKind::ArrayType: {
                    ArrayTypeSyntax* p = (ArrayTypeSyntax*) syntaxBase;
                    PrintLine("ArrayTypeSyntax");
                    indent++;
                    PrintNode(p->elementType);
                    PrintSyntaxList((SyntaxListUntyped*) p->ranks);
                    indent--;
                    break;
                }

                case SyntaxKind::RefType: {
                    RefTypeSyntax* p = (RefTypeSyntax*) syntaxBase;
                    PrintLine("RefTypeSyntax");
                    indent++;
                    PrintToken(p->refKeyword);
                    PrintToken(p->readonlyKeyword);
                    PrintNode(p->type);
                    indent--;
                    break;
                }

                case SyntaxKind::OmittedArraySizeExpression: {
                    OmittedArraySizeExpressionSyntax* p = (OmittedArraySizeExpressionSyntax*) syntaxBase;
                    PrintLine("OmittedArraySizeExpressionSyntax");
                    indent++;
                    PrintToken(p->token);
                    indent--;
                    break;
                }

                case SyntaxKind::NullableType: {
                    NullableType* p = (NullableType*) syntaxBase;
                    PrintLine("NullableType");
                    indent++;
                    PrintNode(p->elementType);
                    PrintToken(p->questionMark);
                    indent--;
                    break;
                }

            }

        }
    };

}