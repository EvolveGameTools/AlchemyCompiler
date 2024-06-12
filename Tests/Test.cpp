#include <catch2/catch_all.hpp>
#include "../Src/PrimitiveTypes.h"
#include "../Src/Parsing2/TextWindow.h"
#include "../Src/Parsing2/Scanning.h"
#include "../Src/Allocation/ThreadLocalTemp.h"
#include "../Src/Parsing2/Tokenizer.h"
#include "../Src/Parsing2/Parser.h"
#include "../Src/Parsing2/Parsing.h"
#include "../Src/Parsing2/NodePrinter.h"
#include "./TestUtil.h"

using namespace Alchemy::Compilation;

TextWindow MakeTextWindow(const char* src) {
    return TextWindow((char*) src, strlen(src));
}

#define INITIALIZE_PARSER_TEST \
    const char * file;         \
    Alchemy::LinearAllocator allocator(MEGABYTES(64), KILOBYTES(32)); \
    Alchemy::TempAllocator* tempAllocator = Alchemy::GetThreadLocalAllocator(); \
    Alchemy::TempAllocator::ScopedMarker marker(tempAllocator); \
    Alchemy::PodList<SyntaxToken> tokens; \
    Diagnostics diagnostics(&allocator);

#define INITIALIZE_PARSER(str) \
    tokens.size = 0;        \
    diagnostics.size = 0; \
    TextWindow textWindow = TextWindow((char*) str, strlen(str)); \
    Tokenize(&textWindow, &diagnostics, &tokens); \
    Parser parser(&allocator, tempAllocator, &diagnostics, tokens.ToCheckedArray());

#define TestFile(x) "../Tests/ParsingExpectations/" x ".output"

#define FILE_TEST_SECTION(x) \
file = TestFile(x);     \
SECTION(x)

TEST_CASE("Parse Field", "[parser]") {
    INITIALIZE_PARSER_TEST

    FILE_TEST_SECTION("Field") {

        INITIALIZE_PARSER("public List<Something> myList;")

        MemberDeclarationSyntax* x = ParseMemberDeclaration(&parser, SyntaxKind::StructDeclaration);

        //WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }

}



TEST_CASE("Parse Switch Expression", "[parser]") {
    INITIALIZE_PARSER_TEST

    FILE_TEST_SECTION("SwitchExpression") {

        INITIALIZE_PARSER(R"(
expr switch {
    10 => 12,
    11 => 13,
    _ => 0
}
)")

        SyntaxBase* x = ParseExpression(&parser);
        // WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }

}

TEST_CASE("Parse Parameter List", "[parser]") {
    INITIALIZE_PARSER_TEST

    FILE_TEST_SECTION("EmptyList") {

        INITIALIZE_PARSER("()")

        SyntaxBase* x = ParseParenthesizedParameterList(&parser);

        //WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }
    FILE_TEST_SECTION("SingleList_Simple") {

        INITIALIZE_PARSER("(float x)")

        SyntaxBase* x = ParseParenthesizedParameterList(&parser);

        //WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }
}

TEST_CASE("string literal expressions", "[parser]") {
    INITIALIZE_PARSER_TEST

    FILE_TEST_SECTION("SimpleStringLiteral") {

        INITIALIZE_PARSER("\"hello this is a string\"")

        ExpressionSyntax* x = ParseExpression(&parser);
        // WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }

    FILE_TEST_SECTION("InterpolatedIdentifierStringLiteral") {

        INITIALIZE_PARSER("\"hello $user, this is a string\"")

        ExpressionSyntax* x = ParseExpression(&parser);
        // WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }

    FILE_TEST_SECTION("EmptyStringLiteral") {

        INITIALIZE_PARSER("\"\"")

        ExpressionSyntax* x = ParseExpression(&parser);
        // WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }

    FILE_TEST_SECTION("InterpolatedStringLiteral") {

        INITIALIZE_PARSER("\"hello ${SomeExpression()}, this is a string\"")

        ExpressionSyntax* x = ParseExpression(&parser);
        //WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }

    FILE_TEST_SECTION("InterpolatedNestedStringLiteral") {

        INITIALIZE_PARSER("\"this is ${SomeExpression(\"also ${interpolated} you see?\")}\"")

        ExpressionSyntax* x = ParseExpression(&parser);
        // WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }

}

TEST_CASE("binary expressions", "[parser]") {
    INITIALIZE_PARSER_TEST

    FILE_TEST_SECTION("BinaryAdd") {

        INITIALIZE_PARSER("x + y")

        ExpressionSyntax* x = ParseExpression(&parser);
        //WriteTreeToFile(file, tokens, x);
        REQUIRE(CompareLines(file, TreeToLine(tokens, x)));

    }

}

TEST_CASE("Scan Type Arguments", "[tokenizer]") {
    Alchemy::LinearAllocator allocator(MEGABYTES(64), KILOBYTES(32));
    Alchemy::TempAllocator* tempAllocator = Alchemy::GetThreadLocalAllocator();
    Alchemy::TempAllocator::ScopedMarker marker(tempAllocator);

    Alchemy::PodList<SyntaxToken> tokens;
    Diagnostics diagnostics(&allocator);

    SECTION("Definitely type arg list") {
        TextWindow textWindow = MakeTextWindow("<int, thing, string>");
        Tokenize(&textWindow, &diagnostics, &tokens);
        Parser parser(&allocator, tempAllocator, &diagnostics, tokens.ToCheckedArray());
        REQUIRE(ScanTypeArgumentList(&parser, NameOptions::None) == ScanTypeArgumentListKind::DefiniteTypeArgumentList);
    }SECTION("Definitely not arg list") {
        TextWindow textWindow = MakeTextWindow("<12, thing, string>");
        Tokenize(&textWindow, &diagnostics, &tokens);
        Parser parser(&allocator, tempAllocator, &diagnostics, tokens.ToCheckedArray());
        REQUIRE(ScanTypeArgumentList(&parser, NameOptions::InExpression) == ScanTypeArgumentListKind::NotTypeArgumentList);
    }
}

TEST_CASE("Tokenize", "[tokenizer]") {
    Alchemy::TempAllocator* allocator = Alchemy::GetThreadLocalAllocator();
    Alchemy::TempAllocator::ScopedMarker marker(allocator);
    Diagnostics diagnostics(allocator);

    Alchemy::PodList<SyntaxToken> tokens;

    SECTION("Count Columns") {
        TextWindow textWindow = MakeTextWindow(R"(this is a test)");
        Tokenize(&textWindow, &diagnostics, &tokens);
        Alchemy::CheckedArray<LineColumn> lc(allocator->AllocateUncleared<LineColumn>(tokens.size), tokens.size);
        ComputeTokenLineColumns(tokens.ToCheckedArray(), lc);
        REQUIRE(lc[0].column == 1);
        REQUIRE(lc[1].column == 5);
        REQUIRE(lc[2].column == 6);
        REQUIRE(lc[3].column == 8);
        REQUIRE(lc[4].column == 9);
        REQUIRE(lc[5].column == 10);
        REQUIRE(lc[6].column == 11);
    }

    SECTION("Count Lines / newline") {
        TextWindow textWindow = MakeTextWindow("this is\n a test");
        Tokenize(&textWindow, &diagnostics, &tokens);
        Alchemy::CheckedArray<LineColumn> lc(allocator->AllocateUncleared<LineColumn>(tokens.size), tokens.size);
        ComputeTokenLineColumns(tokens.ToCheckedArray(), lc);
        REQUIRE(lc[0].line == 1);
        REQUIRE(lc[1].line == 1);
        REQUIRE(lc[2].line == 1);
        REQUIRE(lc[3].line == 1);
        REQUIRE(lc[4].line == 2);
        REQUIRE(lc[5].line == 2);
        REQUIRE(lc[6].line == 2);
        REQUIRE(lc[7].line == 2);

        REQUIRE(lc[0].column == 1);
        REQUIRE(lc[1].column == 5);
        REQUIRE(lc[2].column == 6);
        REQUIRE(lc[3].column == 8);
        // line 2
        REQUIRE(lc[4].column == 1);
        REQUIRE(lc[5].column == 2);
        REQUIRE(lc[6].column == 3);
        REQUIRE(lc[7].column == 4);
    }

    SECTION("Count Lines / crlf") {
        TextWindow textWindow = MakeTextWindow("this is\r\n a test");
        Tokenize(&textWindow, &diagnostics, &tokens);
        Alchemy::CheckedArray<LineColumn> lc(allocator->AllocateUncleared<LineColumn>(tokens.size), tokens.size);
        ComputeTokenLineColumns(tokens.ToCheckedArray(), lc);
        REQUIRE(lc[0].line == 1);
        REQUIRE(lc[1].line == 1);
        REQUIRE(lc[2].line == 1);
        REQUIRE(lc[3].line == 1);
        REQUIRE(lc[4].line == 2);
        REQUIRE(lc[5].line == 2);
        REQUIRE(lc[6].line == 2);
        REQUIRE(lc[7].line == 2);

        REQUIRE(lc[0].column == 1);
        REQUIRE(lc[1].column == 5);
        REQUIRE(lc[2].column == 6);
        REQUIRE(lc[3].column == 8);
        // line 2
        REQUIRE(lc[4].column == 1);
        REQUIRE(lc[5].column == 2);
        REQUIRE(lc[6].column == 3);
        REQUIRE(lc[7].column == 4);

    }

}

TEST_CASE("Scan numeric literals", "[scanner]") {
    Alchemy::TempAllocator* allocator = Alchemy::GetThreadLocalAllocator();
    Alchemy::TempAllocator::ScopedMarker marker(allocator);
    Diagnostics diagnostics(allocator);
    SyntaxToken info;

    SECTION("Reals") {
        TextWindow textWindow = MakeTextWindow(R"(123.0f)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::FloatLiteral);
        REQUIRE(GetFloatValue(info.text, info.textSize) == 123);

        textWindow = MakeTextWindow(R"(123.1f)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::FloatLiteral);
        REQUIRE(GetFloatValue(info.text, info.textSize) == 123.1f);

        textWindow = MakeTextWindow(R"(1_23.1f)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::FloatLiteral);
        REQUIRE(GetFloatValue(info.text, info.textSize) == 123.1f);

        textWindow = MakeTextWindow(R"(1e4ff)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::FloatLiteral);
        REQUIRE(GetFloatValue(info.text, info.textSize) == 1e4f);

        // todo -- parsing currently truncates, we want to do what c# does with real parsing
        // but that's too much work for now.

    }

    SECTION("Integers") {
        TextWindow textWindow = MakeTextWindow(R"(123)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::Int32Literal);
        REQUIRE(GetInt32Value(info.text, info.textSize) == 123);

        textWindow = MakeTextWindow(R"(2147483648)");// max int + 1
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::UInt32Literal);
        REQUIRE(GetUInt32Value(info.text, info.textSize) == 2147483648);

        textWindow = MakeTextWindow(R"(2147483647)");// max int
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::Int32Literal);
        REQUIRE(GetInt32Value(info.text, info.textSize) == 2147483647);

        textWindow = MakeTextWindow(R"(214748364782)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::Int64Literal);
        REQUIRE(GetInt64Value(info.text, info.textSize) == 214748364782);

        textWindow = MakeTextWindow(R"(214ul)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::UInt64Literal);
        REQUIRE(GetUInt64Value(info.text, info.textSize) == 214ull);

        textWindow = MakeTextWindow(R"(214l)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::Int64Literal);
        REQUIRE(GetInt64Value(info.text, info.textSize) == 214ll);

        textWindow = MakeTextWindow(R"(0xfeedbeef)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::UInt32Literal);
        REQUIRE(GetHexValue(info.text, info.textSize) == 0xfeedbeef);

        textWindow = MakeTextWindow(R"(0xfeed_beef)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::UInt32Literal);
        REQUIRE(GetHexValue(info.text, info.textSize) == 0xfeedbeef);

        textWindow = MakeTextWindow(R"(0xbeeful)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::UInt64Literal);
        REQUIRE(GetHexValue(info.text, info.textSize) == 0xbeefull);

        textWindow = MakeTextWindow(R"(0b1101)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.contextualKind == TokenKind::Int32Literal);
        REQUIRE(GetBinaryValue(info.text, info.textSize) == 13);

        textWindow = MakeTextWindow(R"(214748364899999999999999999)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(diagnostics.size == 1);
        REQUIRE(diagnostics.array[0]->errorCode == ErrorCode::ERR_IntOverflow);
        diagnostics.size = 0; // reset for next test

        textWindow = MakeTextWindow(R"(214__)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(diagnostics.size == 1);
        REQUIRE(diagnostics.array[0]->errorCode == ErrorCode::ERR_InvalidNumber);
        diagnostics.size = 0; // reset for next test

        textWindow = MakeTextWindow(R"(_214)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(diagnostics.size == 1);
        REQUIRE(diagnostics.array[0]->errorCode == ErrorCode::ERR_InvalidNumber);

    }


}

TEST_CASE("Scan slow path identifiers", "[scanner]") {
    Alchemy::FixedCharSpan identifier;
    SECTION("valid identifier") {
        TextWindow textWindow = MakeTextWindow(R"(something)");

        bool valid = ScanIdentifier_SlowPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "something");
        REQUIRE(textWindow.HasMoreContent() == false);
    }

    SECTION("invalid identifier") {
        TextWindow textWindow = MakeTextWindow(R"(5something)");

        bool valid = ScanIdentifier_SlowPath(&textWindow, &identifier);
        REQUIRE(valid == false);
        REQUIRE(textWindow.HasMoreContent() == true);
    }

    SECTION("identifier with numbers") {
        TextWindow textWindow = MakeTextWindow(R"(something123 124)");

        bool valid = ScanIdentifier_SlowPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "something123");
        REQUIRE(textWindow.HasMoreContent() == true);
    }

    SECTION("identifier with underscores") {
        TextWindow textWindow = MakeTextWindow(R"(_something)");

        bool valid = ScanIdentifier_SlowPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "_something");
        REQUIRE(textWindow.HasMoreContent() == false);
    }

    SECTION("identifier with single underscore") {
        TextWindow textWindow = MakeTextWindow(R"(_ abc)");

        bool valid = ScanIdentifier_SlowPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "_");
        REQUIRE(textWindow.HasMoreContent() == true);
    }

    SECTION("identifier with only underscores") {
        TextWindow textWindow = MakeTextWindow(R"(____)");

        bool valid = ScanIdentifier_SlowPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "____");
        REQUIRE(textWindow.HasMoreContent() == false);
    }

    SECTION("identifier with unicode") {
        TextWindow textWindow = MakeTextWindow(R"(äpple)");

        bool valid = ScanIdentifier_SlowPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "äpple");
        REQUIRE(textWindow.HasMoreContent() == false);
    }
}

TEST_CASE("Scan identifiers", "[scanner]") {

    Alchemy::FixedCharSpan identifier;
    SECTION("valid identifier") {
        TextWindow textWindow = MakeTextWindow(R"(something)");

        bool valid = ScanIdentifier_FastPath(&textWindow, &identifier);

        REQUIRE(valid == true);
        REQUIRE(identifier == "something");
        REQUIRE(textWindow.HasMoreContent() == false);
    }

}

TEST_CASE("Scan fast path identifiers", "[scanner]") {

    Alchemy::FixedCharSpan identifier;
    SECTION("valid identifier") {
        TextWindow textWindow = MakeTextWindow(R"(something)");

        bool valid = ScanIdentifier_FastPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "something");
        REQUIRE(textWindow.HasMoreContent() == false);
    }

    SECTION("invalid identifier") {
        TextWindow textWindow = MakeTextWindow(R"(5something)");

        bool valid = ScanIdentifier_FastPath(&textWindow, &identifier);
        REQUIRE(valid == false);
        REQUIRE(textWindow.HasMoreContent() == true);
    }

    SECTION("identifier with numbers") {
        TextWindow textWindow = MakeTextWindow(R"(something123 124)");

        bool valid = ScanIdentifier_FastPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "something123");
        REQUIRE(textWindow.HasMoreContent() == true);
    }

    SECTION("identifier with underscores") {
        TextWindow textWindow = MakeTextWindow(R"(_something)");

        bool valid = ScanIdentifier_FastPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "_something");
        REQUIRE(textWindow.HasMoreContent() == false);
    }

    SECTION("identifier with single underscore") {
        TextWindow textWindow = MakeTextWindow(R"(_ abc)");

        bool valid = ScanIdentifier_FastPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "_");
        REQUIRE(textWindow.HasMoreContent() == true);
    }

    SECTION("identifier with only underscores") {
        TextWindow textWindow = MakeTextWindow(R"(____)");

        bool valid = ScanIdentifier_FastPath(&textWindow, &identifier);
        REQUIRE(valid == true);
        REQUIRE(identifier == "____");
        REQUIRE(textWindow.HasMoreContent() == false);
    }

    SECTION("identifier with unicode") {
        TextWindow textWindow = MakeTextWindow(R"(äpple)");

        bool valid = ScanIdentifier_FastPath(&textWindow, &identifier);
        REQUIRE(valid == false);
        REQUIRE(textWindow.HasMoreContent() == true);
    }
}

TEST_CASE("Scan Unicode Escapes", "[scanner]") {

    Diagnostic error;

    SECTION("scan unicode escape utf32") {
        TextWindow textWindow = MakeTextWindow(R"(\U0001F600)");
        uint32 unicode = ScanUnicodeEscape(&textWindow, &error);
        REQUIRE(unicode == 0x0001F600);
    }

    SECTION("scan unicode escape utf32 overflow") {
        TextWindow textWindow = MakeTextWindow(R"(\Uffffffff)");
        ScanUnicodeEscape(&textWindow, &error);
        REQUIRE(error.errorCode == ErrorCode::ERR_IllegalEscape);
    }

    SECTION("scan unicode utf16") {
        TextWindow textWindow = MakeTextWindow(R"(\uD83D\uDE00)");
        uint32 unicode0 = ScanUnicodeEscape(&textWindow, &error);
        REQUIRE(error.errorCode == ErrorCode::None);
        uint32 unicode1 = ScanUnicodeEscape(&textWindow, &error);
        REQUIRE(unicode0 == 0xD83D);
        REQUIRE(unicode1 == 0xDE00);
    }

    SECTION("scan unicode utf16 illegal") {
        TextWindow textWindow = MakeTextWindow(R"(\uFjFF\uDE00)");
        uint32 unicode0 = ScanUnicodeEscape(&textWindow, &error);
        REQUIRE(error.errorCode == ErrorCode::ERR_IllegalEscape);
        uint32 unicode1 = ScanUnicodeEscape(&textWindow, &error);
        REQUIRE(unicode0 == 0);
        REQUIRE(unicode1 == 0);
    }

}
