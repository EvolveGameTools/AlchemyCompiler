#include <catch2/catch_all.hpp>
#include "../Src/Allocation/ThreadLocalTemp.h"
#include "../Src/Parsing3/TextWindow.h"
#include "../Src/Parsing3/Scanning.h"
#include "./TestUtil.h"
#include "../Src/Parsing3/Parser.h"
#include "../Src/Parsing3/Parsing.h"

using namespace Alchemy::Compilation;

TEST_CASE("compilation unit", "[parser]") {

    INITIALIZE_PARSER_TEST

    FILE_TEST_SECTION("Field") {

        INITIALIZE_PARSER("public List<Something> myList;")

        SyntaxBase* x = ParseMemberDeclaration(&parser, SyntaxKind::StructDeclaration);

        REQUIRE(CompareLines(file, TreeToLine(tokenizerResult, x)));

    }

}