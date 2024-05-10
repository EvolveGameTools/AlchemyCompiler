#include <catch2/catch_all.hpp>
#include "../Src/PrimitiveTypes.h"
#include "../Src/Parsing2/TextWindow.h"
#include "../Src/Parsing2/Scanning.h"
#include "../Src/Allocation/ThreadLocalTemp.h"
#include "../Src/Allocation/BytePoolAllocator.h"
#include "../Src/Parsing2/Tokenizer.h"
#include "../Src/Collections/PodList.h"
#include "../Src/Collections/CheckedArray.h"
#include "../Src/Allocation/LinearAllocator.h"
#include "../Src/Parsing2/Parser.h"
#include "../Src/Parsing2/Parsing.h"
#include "../Src/Parsing2/NodePrinter.h"

using namespace Alchemy::Compilation;

TextWindow MakeTextWindow(const char* src) {
    return TextWindow((char*) src, strlen(src));
}
#define INITIALIZE_PARSER_TEST \
    Alchemy::LinearAllocator allocator(MEGABYTES(64), KILOBYTES(32)); \
    Alchemy::TempAllocator* tempAllocator = Alchemy::GetThreadLocalAllocator(); \
    Alchemy::TempAllocator::ScopedMarker marker(tempAllocator); \
    Alchemy::PodList<SyntaxToken> hotTokens; \
    Alchemy::PodList<SyntaxTokenCold> coldTokens; \
    Alchemy::PodList<Trivia> triviaBuffer; \
    Diagnostics diagnostics(&allocator);

#define INITIALIZE_PARSER(str) \
    hotTokens.size = 0;        \
    coldTokens.size = 0;       \
    triviaBuffer.size = 0;     \
    diagnostics.size = 0; \
    TextWindow textWindow = MakeTextWindow((str)); \
    Tokenize(&textWindow, &allocator, &diagnostics, &hotTokens, &coldTokens, &triviaBuffer); \
    Parser parser(&allocator, tempAllocator, &diagnostics, hotTokens.ToCheckedArray(), coldTokens.ToCheckedArray())

TEST_CASE("Parse Types", "[parser]") {

    INITIALIZE_PARSER_TEST

    SECTION("basic type names") {

        INITIALIZE_PARSER("float int char bool ushort short uint byte sbyte double");

        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(ParseType(&parser, ParseTypeMode::Normal)->kind == SyntaxKind::PredefinedType);
        REQUIRE(parser.HasMoreTokens() == false);

    }

    SECTION("basic names") {

        INITIALIZE_PARSER("  something.somethingelse");

        TypeSyntax * name = ParseQualifiedName(&parser, NameOptions::None);
        REQUIRE(name->kind == SyntaxKind::QualifiedName);
        QualifiedNameSyntax * first = (QualifiedNameSyntax*)name;
//        REQUIRE(first->left->kind == SyntaxKind::IdentifierName);
//        REQUIRE(parser.HasMoreTokens() == false);

        NodePrinter p(hotTokens.ToCheckedArray(), coldTokens.ToCheckedArray());

        p.PrintNode(name);
        printf("%.*s", p.buffer.size, p.buffer.array);
    }

}

TEST_CASE("Scan Type Arguments", "[tokenizer]") {
    Alchemy::LinearAllocator allocator(MEGABYTES(64), KILOBYTES(32));
    Alchemy::TempAllocator* tempAllocator = Alchemy::GetThreadLocalAllocator();
    Alchemy::TempAllocator::ScopedMarker marker(tempAllocator);

    Alchemy::PodList<SyntaxToken> hotTokens;
    Alchemy::PodList<SyntaxTokenCold> coldTokens;
    Alchemy::PodList<Trivia> triviaBuffer;
    Diagnostics diagnostics(&allocator);

    SECTION("Definitely type arg list") {
        TextWindow textWindow = MakeTextWindow("<int, thing, string>");
        Tokenize(&textWindow, &allocator, &diagnostics, &hotTokens, &coldTokens, &triviaBuffer);
        Parser parser(&allocator, tempAllocator, &diagnostics, hotTokens.ToCheckedArray(), coldTokens.ToCheckedArray());
        REQUIRE(ScanTypeArgumentList(&parser, NameOptions::None) == ScanTypeArgumentListKind::DefiniteTypeArgumentList);
    }
    SECTION("Definitely not arg list") {
        TextWindow textWindow = MakeTextWindow("<12, thing, string>");
        Tokenize(&textWindow, &allocator, &diagnostics, &hotTokens, &coldTokens, &triviaBuffer);
        Parser parser(&allocator, tempAllocator, &diagnostics, hotTokens.ToCheckedArray(), coldTokens.ToCheckedArray());
        REQUIRE(ScanTypeArgumentList(&parser, NameOptions::InExpression) == ScanTypeArgumentListKind::NotTypeArgumentList);
    }
}

TEST_CASE("Tokenize", "[tokenizer]") {
    Alchemy::TempAllocator* allocator = Alchemy::GetThreadLocalAllocator();
    Alchemy::TempAllocator::ScopedMarker marker(allocator);
    Diagnostics diagnostics(allocator);

    Alchemy::PodList<SyntaxToken> hotTokens;
    Alchemy::PodList<SyntaxTokenCold> coldTokens;
    Alchemy::PodList<Trivia> triviaBuffer;

    SECTION("Count Columns") {
        TextWindow textWindow = MakeTextWindow(R"(this is a test)");
        Tokenize(&textWindow, allocator, &diagnostics, &hotTokens, &coldTokens, &triviaBuffer);
        Alchemy::CheckedArray<LineColumn> lc(allocator->AllocateUncleared<LineColumn>(hotTokens.size), hotTokens.size);
        ComputeTokenLineColumns(hotTokens.ToCheckedArray(), coldTokens.ToCheckedArray(), lc);
        REQUIRE(lc[0].column == 1);
        REQUIRE(lc[1].column == 6);
        REQUIRE(lc[2].column == 9);
        REQUIRE(lc[3].column == 11);
    }

    SECTION("Count Lines / newline") {
        TextWindow textWindow = MakeTextWindow("this is\n a test");
        Tokenize(&textWindow, allocator, &diagnostics, &hotTokens, &coldTokens, &triviaBuffer);
        Alchemy::CheckedArray<LineColumn> lc(allocator->AllocateUncleared<LineColumn>(hotTokens.size), hotTokens.size);
        ComputeTokenLineColumns(hotTokens.ToCheckedArray(), coldTokens.ToCheckedArray(), lc);
        REQUIRE(lc[0].line == 1);
        REQUIRE(lc[1].line == 1);
        REQUIRE(lc[2].line == 2);
        REQUIRE(lc[3].line == 2);
        REQUIRE(lc[0].column == 1);
        REQUIRE(lc[1].column == 6);
        REQUIRE(lc[2].column == 2);
        REQUIRE(lc[3].column == 4);
    }

    SECTION("Count Lines / crlf") {
        TextWindow textWindow = MakeTextWindow("this is\r\n a test");
        Tokenize(&textWindow, allocator, &diagnostics, &hotTokens, &coldTokens, &triviaBuffer);
        Alchemy::CheckedArray<LineColumn> lc(allocator->AllocateUncleared<LineColumn>(hotTokens.size), hotTokens.size);
        ComputeTokenLineColumns(hotTokens.ToCheckedArray(), coldTokens.ToCheckedArray(), lc);
        REQUIRE(lc[0].line == 1);
        REQUIRE(lc[1].line == 1);
        REQUIRE(lc[2].line == 2);
        REQUIRE(lc[3].line == 2);
        REQUIRE(lc[0].column == 1);
        REQUIRE(lc[1].column == 6);
        REQUIRE(lc[2].column == 2);
        REQUIRE(lc[3].column == 4);
        PrintTokens(hotTokens.ToCheckedArray(), coldTokens.ToCheckedArray(), lc);
    }

}

TEST_CASE("Scan numeric literals", "[scanner]") {
    Alchemy::TempAllocator * allocator = Alchemy::GetThreadLocalAllocator();
    Alchemy::TempAllocator::ScopedMarker marker(allocator);
    Diagnostics diagnostics(allocator);
    TokenInfo info;

    SECTION("Reals") {
        TextWindow textWindow = MakeTextWindow(R"(123.0f)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::Float);
        REQUIRE(info.literalValue.floatValue == 123);

        textWindow = MakeTextWindow(R"(123.1f)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::Float);
        REQUIRE(info.literalValue.floatValue == 123.1f);

        textWindow = MakeTextWindow(R"(1_23.1f)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::Float);
        REQUIRE(info.literalValue.floatValue == 123.1f);

        textWindow = MakeTextWindow(R"(1e4ff)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::Float);
        REQUIRE(info.literalValue.floatValue == 1e4f);

        // todo -- parsing currently truncates, we want to do what c# does with real parsing
        // but that's too much work for now.

    }

    SECTION("Integers") {
        TextWindow textWindow = MakeTextWindow(R"(123)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::Int32);
        REQUIRE(info.literalValue.int32Value == 123);

        textWindow = MakeTextWindow(R"(2147483648)");// max int + 1
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::UInt32);
        REQUIRE(info.literalValue.uint32Value == 2147483648);

        textWindow = MakeTextWindow(R"(2147483647)");// max int
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::Int32);
        REQUIRE(info.literalValue.uint32Value == 2147483647);

        textWindow = MakeTextWindow(R"(214748364782)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::Int64);
        REQUIRE(info.literalValue.int64Value == 214748364782);

        textWindow = MakeTextWindow(R"(214ul)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::UInt64);
        REQUIRE(info.literalValue.int64Value == 214ull);

        textWindow = MakeTextWindow(R"(214l)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::Int64);
        REQUIRE(info.literalValue.int64Value == 214ll);

        textWindow = MakeTextWindow(R"(0xfeedbeef)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::UInt32);
        REQUIRE(info.literalValue.uint32Value == 0xfeedbeef);

        textWindow = MakeTextWindow(R"(0xfeed_beef)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::UInt32);
        REQUIRE(info.literalValue.uint32Value == 0xfeedbeef);

        textWindow = MakeTextWindow(R"(0xbeeful)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::UInt64);
        REQUIRE(info.literalValue.uint64Value == 0xbeefull);

        textWindow = MakeTextWindow(R"(0b1101)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(info.valueKind == LiteralType::Int32);
        REQUIRE(info.literalValue.int32Value == 13);

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
