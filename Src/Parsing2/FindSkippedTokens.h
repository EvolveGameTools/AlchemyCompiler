#pragma once

#include "./SyntaxBase.h"
#include "./SyntaxNodes.h"

namespace Alchemy::Compilation {

    struct FindSkippedTokens {

        CheckedArray<SyntaxToken> tokens;
        explicit FindSkippedTokens(CheckedArray<SyntaxToken> tokens, SyntaxBase * node);

        void TouchToken(SyntaxToken token);
        void TouchTokenList(TokenList * tokenList);
        void TouchSeparatedSyntaxList(SeparatedSyntaxListUntyped * syntaxList);
        void TouchSyntaxList(SyntaxListUntyped * syntaxList);
        void TouchNode(SyntaxBase * syntaxBase);

    };

}