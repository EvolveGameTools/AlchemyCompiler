#pragma once

#include <cstdio>
#include "../PrimitiveTypes.h"
#include "./SyntaxNodes.h"
#include "./SyntaxKind.h"
#include "../Collections/PodList.h"

namespace Alchemy::Compilation {

    struct NodePrinter {

        bool printTrivia;
        bool printSkipped;
        bool printComments;
        bool printWhitespace;

        CheckedArray<SyntaxToken> tokens;
        PodList<char> buffer;

        int32 indent;

        NodePrinter(CheckedArray<SyntaxToken> tokens)
            : tokens(tokens)
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

            PrintLine("SeparatedSyntaxList");
            indent++;
            for(int32 i = 0; i < list->itemCount; i++) {
                PrintIndent();
                PrintNode(list->items[i]);
                if(i < list->separatorCount) {
                    PrintIndent();
                    PrintToken(list->separators[i]);
                }
            }
            indent--;
        }

        void PrintLine() {
            buffer.Add('\n');
            buffer.Add('_');
        }

        void PrintLine(char* str, int32 length) {
            buffer.Add('\n');
            buffer.Add('_');
            PrintIndent();
            PrintInline(str, length);
        }

        void PrintLine(const char* str) {
            PrintInline(str);
            buffer.Add('\n');
            buffer.Add('_');
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

        void PrintTokenList(TokenList * tokenList) {
            for(int32 i = 0; i < tokenList->size; i++) {
                PrintToken(tokenList->array[i]);
            }
        }

        void PrintTrivia(Trivia trivia) {

            if (trivia.type == TriviaType::Whitespace || trivia.type == TriviaType::NewLine && printWhitespace) {
                PrintLine(trivia.span, trivia.length);
            }
            else if (trivia.type == TriviaType::SingleLineComment || trivia.type == TriviaType::MultiLineComment && printComments) {
                PrintLine(trivia.span, trivia.length);
            }
        }

        void PrintFieldName(const char * fieldName) {
            PrintIndent();
            PrintInline("[");
            PrintInline(fieldName);
            PrintInline("] = ");
        }

        void PrintToken(SyntaxToken token) {

            if (!token.IsValid()) {
                return;
            }

            PrintInline(SyntaxKindToString(token.kind));

            if(token.IsMissing()) {
                PrintInline(" <missing> ");
            }

            if (token.contextualKind != SyntaxKind::None) {
                PrintInline(" (");
                PrintInline(SyntaxKindToString(token.contextualKind));
                PrintInline(")");
            }

            PrintInline(" -> \"");
            PrintInline(token.text, token.textSize);
            PrintInline("\"");
            PrintLine();

        }

        void PrintNode(SyntaxBase* syntaxBase);

        void Dump() {
            printf("%.*s", buffer.size - 2, buffer.array);
        }

    };

}