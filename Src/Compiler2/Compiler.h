#pragma once

#include "../Parsing3/Diagnostics.h"
#include "./TypeInfo.h"
#include "../Collections/MSIHashmap.h"
#include "../Collections/PagedList.h"
#include "../Allocation/PoolAllocator.h"
#include "../FileSystem/VirtualFileSystem.h"
#include "../JobSystem/JobSystem.h"
#include "SourceFileInfo.h"

namespace Alchemy::Compilation {

    struct AssemblyInfo {
        FixedCharSpan assemblyName;
        FixedCharSpan path;
    };

    struct PackageInfo {

        FixedCharSpan packageName;
        FixedCharSpan relativeDirectory;
        FixedCharSpan absolutePath;

    };

    struct Compiler {

        Diagnostics diagnostics;
        VirtualFileSystem vfs;
        Alchemy::Jobs::JobSystem jobSystem;
        MSIDictionary<FixedCharSpan, TypeInfo*> resolveMap;
        Alchemy::PoolAllocator<SourceFileInfo> fileAllocator;

        static CheckedArray<TypeInfo*> s_BuiltInTypeInfos;

        PodList<SourceFileInfo*> fileInfos;
        PodList<VirtualFileInfo> sourceFileBuffer;
        PodList<PackageInfo> packages;

        void SetupCompilationRun(TempAllocator * tempAllocator, CheckedArray<VirtualFileInfo> files);

        void LoadDependencies();

        void Compile();
    };


}