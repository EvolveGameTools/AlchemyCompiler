#include "Src/Compiler/Compiler.h"

int32 main(int32 argc, char** argv) {
    Alchemy::Compilation::Compile(argv[0]);
    return 0;
}
