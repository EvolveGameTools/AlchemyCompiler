#include <catch2/catch_all.hpp>
#include "../Src/Allocation/ThreadLocalTemp.h"
#include "../Src/Parsing3/TextWindow.h"
#include "../Src/Parsing3/Scanning.h"
#include "./TestUtil.h"
#include "../Src/Parsing3/Parser.h"
#include "../Src/Parsing3/Parsing.h"
#include "../Src/Compiler2/FullyQualifiedName.h"
#include "../Src/FileSystem/VirtualFileSystem.h"
#include "../Src/Compiler2/Compiler.h"

using namespace Alchemy::Compilation;

TEST_CASE("MakeGenericArgumentName", "[parser]") {

    FixedCharSpan name("System::Collections::Dictionary$2");
    Allocator a = Allocator::MakeMallocator();
    SECTION("A") {
        FixedCharSpan argName("T");
        FixedCharSpan span = MakeFullyQualifiedGenericArgName(name, argName, 0, a);
        REQUIRE(span == FixedCharSpan("System::Collections::Dictionary$2_T[0]"));
    }SECTION("B") {
        FixedCharSpan argName("TValueWithALongerName");
        FixedCharSpan span = MakeFullyQualifiedGenericArgName(name, argName, 100, a);
        REQUIRE(span == FixedCharSpan("System::Collections::Dictionary$2_TValueWithALongerName[100]"));
    }
}

TEST_CASE("xyz") {

    FixedCharSpan package("Package");

    VirtualFileInfo f1(package, FixedCharSpan("path/one.wyx"));
    VirtualFileInfo f2(package, FixedCharSpan("path/two.wyx"));

    Compiler compiler(0, FileSystemType::Virtual);

    compiler.vfs.AddFile(f1, FixedCharSpan(R"(

        public class Thing {}

        public class OtherThing {}

    )"));

    compiler.vfs.AddFile(f2, FixedCharSpan(R"(

        public class Thing2 {}

        public class OtherThing2 {}

    )"));

    PackageInfo info;

    info.absolutePath = FixedCharSpan("path/");
    info.packageName = package;

    compiler.Compile(CheckedArray<PackageInfo>(&info, 1));

}

TEST_CASE("compilation unit", "[parser]") {

    INITIALIZE_PARSER_TEST

    FILE_TEST_SECTION("Field") {

        INITIALIZE_PARSER("public List<Something> myList;")

        SyntaxBase* x = ParseMemberDeclaration(&parser, SyntaxKind::StructDeclaration);

        REQUIRE(CompareLines(file, TreeToLine(tokenizerResult, x)));

    }

}