#pragma once

#include "../Compiler.h"

namespace Alchemy::Compilation {

    struct CompileRootJob : Jobs::IJob {

        Diagnostics* diagnostics;
        MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap;
        FixedCharSpan rootPackageName;
        CheckedArray<TypeInfo*> builtInTypes;
        Compiler* compiler;

        CheckedArray<AssemblyInfo> libraries;

        explicit CompileRootJob(Compiler* compiler)
            : compiler(compiler)
            , resolveMap(&compiler->resolveMap)
            , diagnostics(&compiler->diagnostics) {}

        void Execute() override;

    };

    struct ParseFilesJob : Jobs::IJob {

        CheckedArray<SourceFileInfo*> fileInfos;
        VirtualFileSystem * vfs;

        void Execute(int32 idx) override {

            SourceFileInfo * fileInfo = fileInfos[idx];

            if(!fileInfo->wasChanged) {
                return;
            }

            FixedCharSpan span = vfs->ReadFile(fileInfo->path, GetAllocator());

        }

    };

}