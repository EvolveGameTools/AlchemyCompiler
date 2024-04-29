#include <catch2/catch_all.hpp>
#include "../Src/PrimitiveTypes.h"
#include "../Src/Parsing2/TextWindow.h"
#include "../Src/Parsing2/SyntaxDiagnosticInfo.h"
#include "../Src/Parsing2/Scanning.h"
#include "../Src/Util/FixedCharSpan.h"

using namespace Alchemy::Parsing;

TextWindow MakeTextWindow(const char* src) {
    return TextWindow((char*) src, strlen(src));
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
