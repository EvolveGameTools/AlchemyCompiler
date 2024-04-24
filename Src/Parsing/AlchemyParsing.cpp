#include "./AlchemyParser2.h"
#include "./Parser.h"

namespace Alchemy {

    static_assert(sizeof(Parser) == sizeof(ParserImpl));

    Parser::Parser() {
        new(parserImpl)(ParserImpl);
    }

    Parser::~Parser() {
        ((ParserImpl*) parserImpl)->~ParserImpl();
    }

    bool Parser::TryParseFile(char* src, int32 srcLength, Alchemy::ParseResult* parseResult, LinearAllocator* outputAllocator) {
        ParserImpl * pParser = (ParserImpl*)parserImpl;
        return pParser->TryParseFile(src, srcLength, parseResult, outputAllocator);
    }

}
