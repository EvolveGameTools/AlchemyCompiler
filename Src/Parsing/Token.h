#pragma once

#include "./Keyword.h"
#include "./TokenType.h"
#include "./TokenFlags.h"

namespace Alchemy::Parsing {

    struct Token {

        TokenType tokenType {TokenType::Invalid};
        Keyword keyword {Keyword::NotAKeyword}; // todo -- move reserved keywords into token type and other keywords into a separate category
        ContextualKeyword contextualKeyword {ContextualKeyword::NotAKeyword};
        TokenFlags flags {0};
        int32 position {};

        Token() = default;

        Token(TokenType tokenType, int32 position)
            : tokenType(tokenType)
            , position(position)
            , contextualKeyword(ContextualKeyword::NotAKeyword)
            , flags(TokenFlags::None)
            , keyword(Keyword::NotAKeyword) {}

        Token(TokenType tokenType, Keyword keyword, int32 position)
            : tokenType(tokenType)
            , position(position)
            , contextualKeyword(ContextualKeyword::NotAKeyword)
            , flags(TokenFlags::None)
            , keyword(keyword) {}

    };
}