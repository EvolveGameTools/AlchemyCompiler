#include "../private/AlchemyParserImpl.inc"
#include "../Parser.h"

namespace Alchemy::Parsing {

    static_assert(sizeof(Parser) == sizeof(ParserImpl));

    Parser::Parser() {
        new(parserImpl)(ParserImpl);
    }

    Parser::~Parser() {
        ((ParserImpl*) parserImpl)->~ParserImpl();
    }

    bool Parser::TryParseFile(char* src, int32 srcLength, ParseResult* parseResult, LinearAllocator* outputAllocator) {
        ParserImpl * pParser = (ParserImpl*)parserImpl;
        return pParser->TryParseFile(src, srcLength, parseResult, outputAllocator);
    }

}

int32 main(int32 argc, char** argv) {
    Alchemy::Parsing::Parser p;
    return 0;
}