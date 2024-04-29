#include "./SyntaxFacts.h"

namespace Alchemy::Parsing::SyntaxFacts {

    bool IsHexDigit(char c) {
        return (c >= '0' && c <= '9') ||
               (c >= 'A' && c <= 'F') ||
               (c >= 'a' && c <= 'f');
    }

    int32 HexValue(char c) {
        assert(IsHexDigit(c));
        return (c >= '0' && c <= '9') ? c - '0' : (c & 0xdf) - 'A' + 10;
    }

    bool IsBinaryDigit(char c) {
        return c == '0' | c == '1';
    }

    bool IsDecDigit(char c) {
        return c >= '0' && c <= '9';
    }

}