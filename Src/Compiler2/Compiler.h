#pragma once

#include "../Parsing3/Diagnostics.h"
#include "../Collections/PagedList.h"
#include "../Allocation/PoolAllocator.h"
#include "../FileSystem/VirtualFileSystem.h"
#include "../JobSystem/JobSystem.h"
#include "./TypeInfo.h"
#include "./SourceFileInfo.h"
#include "./TypeResolutionMap.h"

namespace Alchemy::Compilation {

    struct PackageInfo {

        FixedCharSpan packageName;
        FixedCharSpan absolutePath;

    };

    struct Compiler {

        Diagnostics diagnostics;
        VirtualFileSystem vfs;
        Jobs::JobSystem jobSystem;
        TypeResolutionMap resolveMap;
        PoolAllocator<SourceFileInfo> fileAllocator;

        PodList<SourceFileInfo*> fileInfos;
        PodList<SourceFileInfo*> builtInFiles;
        PodList<VirtualFileInfo> sourceFileBuffer;

        TypeInfo* typeBuffer[kBuiltInTypeCount];

        Compiler(int32 workerCount, FileSystemType fileSystemType);

        void SetupCompilationRun(TempAllocator * tempAllocator, CheckedArray<VirtualFileInfo> includedSourceFiles);

        void LoadDependencies();

        void Compile(CheckedArray<PackageInfo> compiledPackages);

        void AssignBuiltInType(const char* name, BuiltInTypeName builtInTypeName);
    };


}