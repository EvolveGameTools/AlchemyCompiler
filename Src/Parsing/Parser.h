#pragma once

#include "../Parsing/ParseResult.h"

namespace Alchemy {

    struct Parser {

        Parser();
        ~Parser();

        bool TryParseFile(char* src, int32 srcLength, ParseResult* parseResult, LinearAllocator * outputAllocator);

    private:
        uint8 parserImpl[192];
    };

}
