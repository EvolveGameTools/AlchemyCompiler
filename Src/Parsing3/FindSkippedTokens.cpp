#include "./FindSkippedTokens.h"

namespace Alchemy::Compilation {

    FindSkippedTokens::FindSkippedTokens(CheckedArray<SyntaxToken> tokens, SyntaxBase * node)
    : tokens(tokens) {

        // init to skipped
        for (int32 i = 0; i < tokens.size; i++) {
            this->tokens[i].AddFlag(SyntaxTokenFlags::Skipped);
        }

        TouchNode(node);

    }

    void FindSkippedTokens::TouchTokenList(TokenList* tokenList) {

        if (tokenList == nullptr) {
            return;
        }

        for (int32 i = 0; i < tokenList->size; i++) {
            TouchToken(tokenList->array[i]);
        }

    }

    void FindSkippedTokens::TouchSyntaxList(SyntaxListUntyped* list) {
        if (list == nullptr) {
            return;
        }
        for (int32 i = 0; i < list->size; i++) {
            TouchNode(list->array[i]);
        }
    }

    void FindSkippedTokens::TouchSeparatedSyntaxList(SeparatedSyntaxListUntyped* list) {
        if (list == nullptr) {
            return;
        }
        for (int32 i = 0; i < list->itemCount; i++) {
            TouchNode(list->items[i]);
        }

        for (int32 i = 0; i < list->separatorCount; i++) {
            TouchToken(list->separators[i]);
        }
    }

    void FindSkippedTokens::TouchToken(SyntaxToken token) {
        if (!token.IsValid() || token.IsMissing()) {
            return;
        }

        if(token.kind == TokenKind::Trivia) {
            return;
        }

        tokens[token.GetId()].SetFlags(tokens[token.GetId()].GetFlags() & ~SyntaxTokenFlags::Skipped);

    }

}