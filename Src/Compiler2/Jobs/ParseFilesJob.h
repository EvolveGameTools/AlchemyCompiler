#pragma once

#include "../../JobSystem/Job.h"
#include "../../JobSystem/JobSystem.h"
#include "../Compiler.h"

namespace Alchemy::Compilation {

    struct ParseFilesJobRoot : Jobs::IJob {

        CheckedArray<SourceFileInfo*> files;
        VirtualFileSystem* vfs;

        explicit ParseFilesJobRoot(VirtualFileSystem* vfs, CheckedArray<SourceFileInfo*> files)
            : vfs(vfs)
            , files(files) {}

        void Execute() override;

    };

    struct ParseFileJob : Jobs::IJob {

        CheckedArray<SourceFileInfo*> files;
        VirtualFileSystem* vfs;

        explicit ParseFileJob(VirtualFileSystem* vfs, CheckedArray<SourceFileInfo*> files)
            : vfs(vfs)
            , files(files) {}

        void Execute(int32 idx) override;

    };

}