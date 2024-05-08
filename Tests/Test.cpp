#include <catch2/catch_all.hpp>
#include "../Src/PrimitiveTypes.h"
#include "../Src/Parsing2/TextWindow.h"
#include "../Src/Parsing2/SyntaxDiagnosticInfo.h"
#include "../Src/Parsing2/Scanning.h"
#include "../Src/Allocation/ThreadLocalTemp.h"
#include "../Src/Allocation/BytePoolAllocator.h"

using namespace Alchemy::Compilation;

TextWindow MakeTextWindow(const char* src) {
    return TextWindow((char*) src, strlen(src));
}

TEST_CASE("Scan numeric literals", "[scanner]") {
    TempAllocator * allocator = Alchemy::GetThreadLocalAllocator();
    TempAllocator::ScopedMarker marker(allocator);
    Alchemy::BytePoolAllocator bytePoolAllocator(allocator);
    Diagnostics diagnostics(&bytePoolAllocator);
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
        REQUIRE(diagnostics.cnt == 1);
        REQUIRE(diagnostics.errorList[0].errorCode == ErrorCode::ERR_IntOverflow);
        diagnostics.cnt = 0; // reset for next test

        textWindow = MakeTextWindow(R"(214__)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(diagnostics.cnt == 1);
        REQUIRE(diagnostics.errorList[0].errorCode == ErrorCode::ERR_InvalidNumber);
        diagnostics.cnt = 0; // reset for next test

        textWindow = MakeTextWindow(R"(_214)");
        REQUIRE(ScanNumericLiteral(&textWindow, &diagnostics, &info));
        REQUIRE(diagnostics.cnt == 1);
        REQUIRE(diagnostics.errorList[0].errorCode == ErrorCode::ERR_InvalidNumber);

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

    SyntaxDiagnosticInfo error;

    SECTION("scan unicode escape utf32") {
        TextWindow textWindow = MakeTextWindow(R"(\U0001F600)");
        uint32 unicode = ScanUnicodeEscape(&textWindow, &error);
        REQUIRE(unicode == 0x0001F600);
    }

    SECTION("scan unicode escape utf32 overflow") {
        TextWindow textWindow = MakeTextWindow(R"(\Uffffffff)");
        ScanUnicodeEscape(&textWindow, &error);
        REQUIRE(error.errorCode == ErrorCode::IllegalEscape);
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
        REQUIRE(error.errorCode == ErrorCode::IllegalEscape);
        uint32 unicode1 = ScanUnicodeEscape(&textWindow, &error);
        REQUIRE(unicode0 == 0);
        REQUIRE(unicode1 == 0);
    }

}
