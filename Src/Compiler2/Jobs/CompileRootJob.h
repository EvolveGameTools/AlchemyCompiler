#pragma once

#include "../Compiler.h"

namespace Alchemy::Compilation {

    struct CompileRootJob : Jobs::IJob {

        Diagnostics* diagnostics;
        MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap;
        FixedCharSpan rootPackageName;
        CheckedArray<TypeInfo*> builtInTypes;
        Compiler* compiler;
        MSIDictionary<FixedCharSpan, FileInfo*> fileInfoByAbsolutePath;

        CheckedArray<AssemblyInfo> assemblies;

        explicit CompileRootJob(Compiler* compiler)
            : compiler(compiler)
            , resolveMap(&compiler->resolveMap)
            , diagnostics(&compiler->diagnostics) {}

        void Execute() override;

        void LoadAssemblyFiles(AssemblyInfo info, PagedList<FileInfo*>* addedFiles, PagedList<FileInfo*>* touchedFiles, PagedList<FileInfo*>* changedFiles);

    };
}