#include "Src/Compiler2/Compiler.h"

using namespace Alchemy;

int32 main(int32 argc, char** argv) {

    Compilation::Compiler compiler(0, FileSystemType::Real);

    Compilation::PackageInfo info;
    info.absolutePath = FixedCharSpan("C:\\Users\\matth\\Dev\\AlchemyCompiler\\TestApp");
    info.packageName = FixedCharSpan("Wyx");

    compiler.Compile(CheckedArray<Compilation::PackageInfo>(&info, 1));

    return 0;
}
