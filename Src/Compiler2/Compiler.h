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
        Alchemy::Jobs::JobSystem jobSystem;
        TypeResolutionMap resolveMap;
        Alchemy::PoolAllocator<SourceFileInfo> fileAllocator;

        PodList<SourceFileInfo*> fileInfos;
        PodList<VirtualFileInfo> sourceFileBuffer;

        CheckedArray<TypeInfo*> builtInTypes;

        TypeInfo* typeBuffer[kBuiltInTypeCount];

        Compiler(int32 workerCount, FileSystemType fileSystemType);

        void SetupCompilationRun(TempAllocator * tempAllocator, CheckedArray<VirtualFileInfo> includedSourceFiles);

        void LoadDependencies();

        void Compile(CheckedArray<PackageInfo> compiledPackages);

        void AssignPrimitiveType(const char* name, BuiltInTypeName builtInTypeName);
    };


}