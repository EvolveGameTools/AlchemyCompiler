
#include "../PrimitiveTypes.h"
#include "../Util/FixedCharSpan.h"
#include "../Parsing2/Tokenizer.h"

namespace Alchemy::Compilation {

    struct InputFile {

        FixedCharSpan src;
        FixedCharSpan absolutePath;
        uint64 lastWrite {};

        InputFile() = default;

        InputFile(FixedCharSpan absolutePath, FixedCharSpan src, uint64 lastWrite)
            : src(src)
            , absolutePath(absolutePath)
            , lastWrite(lastWrite)
        {}

    };

    struct CompilationFile {
        InputFile inputFile;
        CheckedArray<LineColumn> lineInfos;
        CheckedArray<SyntaxToken> tokens;
    };

    struct Compiler {

        void LoadFiles() {}

    };

// first pass compiles everything

    void LoadFiles() {

    }
}