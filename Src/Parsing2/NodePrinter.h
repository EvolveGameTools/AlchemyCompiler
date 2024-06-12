#pragma once

#include <cstdio>
#include "../PrimitiveTypes.h"
#include "./SyntaxNodes.h"
#include "./SyntaxKind.h"
#include "../Collections/PodList.h"
#include "Tokenizer.h"
#include "../Allocation/ThreadLocalTemp.h"
#include "FindSkippedTokens.h"

namespace Alchemy::Compilation {

    DEFINE_ENUM_FLAGS(TreePrintOptions, uint32, {
        None = 0,
        PrintLineRanges = 1 << 0,
        PrintDiagnostics = 1 << 1,
        PrintSkippedTokens = 1 << 2,
        Default,
    })

    struct NodePrinter {

        bool printTrivia;
        bool printSkipped;
        bool printComments;
        bool printWhitespace;

        CheckedArray<SyntaxToken> tokens;
        PodList<char> buffer;

        int32 indent;
        TreePrintOptions options;
        CheckedArray<LineColumn> lc;

        explicit NodePrinter(CheckedArray<SyntaxToken> tokens, TreePrintOptions options)
            : tokens(tokens)
            , indent(0)
            , options(options)
            , printComments(true)
            , printWhitespace(true)
            , printTrivia(true)
            , printSkipped(true)
            , buffer(2048) {
            lc = CheckedArray<LineColumn>(MallocateTyped(LineColumn, tokens.size), tokens.size);
            ComputeTokenLineColumns(tokens, lc);
        }

        ~NodePrinter() {
            MfreeTyped(lc.array, lc.size);
        }

        void PrintIndent() {
            for (int32 i = 0; i < 4 * indent; i++) {
                buffer.Add(' ');
            }
        }

        void PrintSyntaxList(SyntaxListUntyped* list) {
            if (list == nullptr || list->size == 0) {
                PrintInline("SyntaxList <empty>");
                return;
            }

            if (list->size > 0) {
                PrintInline("SyntaxList");
                int32 startTokenId = list->array[0]->startTokenId;
                int32 endTokenId = list->array[list->size - 1]->endTokenId;
                PrintLineRange(startTokenId, endTokenId);
                buffer.Add('\n');
            }
            else {
                PrintLine("SyntaxList <empty>");
            }
            indent++;
            for (int32 i = 0; i < list->size; i++) {
                PrintIndent();
                PrintNode(list->array[i]);
            }
            indent--;

        }

        void PrintSeparatedSyntaxList(SeparatedSyntaxListUntyped* list) {
            if (list == nullptr) {
                PrintInline("SeparatedSyntaxList <empty>\n");
                return;
            }

            if (list->itemCount > 0) {
                PrintInline("SeparatedSyntaxList");
                if (list->separatorCount == 0) {
                    PrintLineRange(list->items[0]);
                }
                else {
                    SyntaxToken s = list->separators[list->separatorCount - 1];
                    int32 itemToken = list->items[list->itemCount - 1]->endTokenId;
                    if (s.GetId() > itemToken) {
                        PrintLineRange(list->items[0]->startTokenId, s.GetId());
                    }
                    else {
                        PrintLineRange(list->items[0]->startTokenId, itemToken);
                    }
                }

                buffer.Add('\n');
            }
            else {
                PrintLine("SeparatedSyntaxList <empty>");
            }
            indent++;
            for (int32 i = 0; i < list->itemCount; i++) {
                PrintIndent();
                PrintNode(list->items[i]);
                if (i < list->separatorCount) {
                    PrintIndent();
                    PrintToken(list->separators[i]);
                }
            }
            indent--;
        }

        void PrintLine() {
            buffer.Add('\n');
        }

        void PrintNodeHeader(const char* str, SyntaxBase* base) {
            PrintInline(str);
            PrintLineRange(base);
            buffer.Add('\n');
        }

        void PrintLine(char* str, int32 length) {
            buffer.Add('\n');
            PrintIndent();
            PrintInline(str, length);
        }

        void PrintLine(const char* str) {
            PrintInline(str);
            buffer.Add('\n');
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

        void PrintTokenList(TokenList* tokenList) {
            if(tokenList == nullptr || tokenList->size == 0) {
                PrintLine("<empty>");
                return;
            }

            buffer.EnsureAdditionalCapacity(64);

            int32 min = tokenList->array[0].GetId();
            int32 max = tokenList->array[0].GetId();

            for(int32 i = 1; i < tokenList->size; i++) {
                int32 id = tokenList->array[i].GetId();
                if(id < min) min = id;
                if(id > max) max = id;
            }

            buffer.size += snprintf(buffer.array + buffer.size, 64, "[%d:%d - %d:%d]",
                lc[min].line,
                lc[min].column,
                lc[max].endLine,
                lc[max].endColumn
            );

            PrintLine();
            indent++;
            for (int32 i = 0; i < tokenList->size; i++) {
                PrintIndent();
                PrintToken(tokenList->array[i]);
            }
            indent--;
        }

        void PrintTrivia(Trivia trivia) {

            if (trivia.type == TriviaType::Whitespace || trivia.type == TriviaType::NewLine && printWhitespace) {
                PrintLine(trivia.span, trivia.length);
            }
            else if (trivia.type == TriviaType::SingleLineComment || trivia.type == TriviaType::MultiLineComment && printComments) {
                PrintLine(trivia.span, trivia.length);
            }
        }

        void PrintFieldName(const char* fieldName) {
            PrintIndent();
            PrintInline(fieldName);
            PrintInline(" = ");
        }

        void PrintToken(SyntaxToken token) {

            if (!token.IsValid()) {
                PrintInline("<unset> \n");
                return;
            }

            PrintInline(TokenKindToString(token.kind));

            if (token.IsMissing()) {
                PrintInline(" <missing> ");
            }

            if ((token.GetFlags() & SyntaxTokenFlags::Skipped) != 0) {
                PrintInline(" <skipped> ");
            }

            if (token.contextualKind != token.kind) {
                PrintInline(" (");
                PrintInline(TokenKindToString(token.contextualKind));
                PrintInline(")");
            }

            PrintInline("  ");
            PrintInline(token.text, token.textSize);
            PrintInline(" ");

            buffer.EnsureAdditionalCapacity(64);

            buffer.size += snprintf(buffer.array + buffer.size, 64, " [%d:%d - %d:%d]",
                lc[token.GetId()].line,
                lc[token.GetId()].column,
                lc[token.GetId()].endLine,
                lc[token.GetId()].endColumn
            );

            PrintLine();

        }

        void PrintLineRange(int32 start, int32 end) {
            buffer.EnsureAdditionalCapacity(64);

            buffer.size += snprintf(buffer.array + buffer.size, 64, " [%d:%d - %d:%d]",
                lc[start].line,
                lc[start].column,
                lc[end].endLine,
                lc[end].endColumn
            );

        }

        void PrintLineRange(SyntaxBase* syntaxBase) {
            PrintLineRange(syntaxBase->startTokenId, syntaxBase->endTokenId);
        }

        void PrintTree(SyntaxBase* syntaxBase, NodeEqualityOptions options = NodeEqualityOptions::Default) {

            FindSkippedTokens finder(tokens, syntaxBase);

            PrintNode(syntaxBase);

            for (int32 i = 0; i < tokens.size; i++) {
                if ((tokens[i].GetFlags() & SyntaxTokenFlags::Skipped) != 0 && tokens[i].kind != TokenKind::Trivia) {
                    PrintToken(tokens[i]);
                }

            }

        }

        void PrintNode(SyntaxBase* syntaxBase);

        void Dump() {
            printf("%.*s", buffer.size, buffer.array);
        }

    };

}