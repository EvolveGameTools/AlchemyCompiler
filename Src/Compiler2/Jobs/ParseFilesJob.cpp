#include "./ParseFilesJob.h"
#include "../../Parsing3/Tokenizer.h"
#include "../../Parsing3/Parsing.h"
#include "../../Parsing3/TextWindow.h"
#include "../../Parsing3/Parser.h"

namespace Alchemy::Compilation {

    void ParseFilesJobRoot::Execute() {

        Await(Jobs::Parallel::Foreach(files.size, 1), ParseFileJob(vfs, files));

    }

    void ParseFileJob::Execute(int32 idx) {

        SourceFileInfo * fileInfo = files[idx];

        FixedCharSpan source = vfs->ReadFileText(fileInfo->path, fileInfo->allocator.MakeAllocator());

        TextWindow window(source.ptr, source.size);

        TokenizerResult result = Tokenize(window, &fileInfo->diagnostics, &fileInfo->allocator);

        Parser parser(result, &fileInfo->diagnostics, &fileInfo->allocator);

        fileInfo->syntaxTree = ParseCompilationUnit(&parser);

    }

}