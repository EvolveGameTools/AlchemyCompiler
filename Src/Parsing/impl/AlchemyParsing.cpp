//#include "../private/AlchemyParserImpl.inc"
//#include "../Parser.h"
//#include "../../Collections/MSIHashmap.h"
//
//namespace Alchemy::Parsing {
//
//    static_assert(sizeof(Parser) == sizeof(ParserImpl));
//
//    Parser::Parser() {
//        new(parserImpl)(ParserImpl);
//    }
//
//    Parser::~Parser() {
//        ((ParserImpl*) parserImpl)->~ParserImpl();
//    }
//
//    bool Parser::TryParseFile(char* src, int32 srcLength, ParseResult* parseResult, LinearAllocator* outputAllocator) {
//        ParserImpl * pParser = (ParserImpl*)parserImpl;
//        return pParser->TryParseFile(src, srcLength, parseResult, outputAllocator);
//    }
//
//}
//
//namespace Alchemy {
//
//    template<typename T, typename U>
//    struct MapPair {
//        T * key;
//        U * value;
//    };
//
//    template<typename T, typename U>
//    struct Map {
//        T * keys;
//        U * values;
//        int32 size;
//
//        MapPair<T, U> ToMapPair() {
//            return { keys, values };
//        }
//
//        int32 GetValue() {
//            return size;
//        }
//
//    };
//
//}

#include "../../PrimitiveTypes.h"
#include "../../Allocation/PodAllocation.h"
#include <cstdio>

void fn() {

    float * pValues = MallocateTyped(float, 32);
    char * pChars = MallocateTypedUncleared(char, 128);
    char * pChars2 = (char*)MallocateUncleared(128);

    pValues[33] = 10.0f;

    Mfree(pValues, 128);

    MallocDebug::ReportAndFreeLeakedAllocations();
}

int32 main(int32 argc, char** argv) {


fn();
    return 0;
}