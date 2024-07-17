#pragma once

#include "../JobSystem/JobSystem.h"
#include "../Parsing3/Diagnostics.h"
#include "./TypeInfo.h"
#include "../Collections/MSIHashmap.h"
#include "../Collections/PagedList.h"
#include "../Allocation/PoolAllocator.h"

namespace Alchemy::Compilation {

    struct FileInfo {

        FixedCharSpan assemblyName;
        FixedCharSpan path;
        uint64 lastEditTime;
        PodList<FileInfo*> dependencies;

        FileInfo(FixedCharSpan assemblyName, FixedCharSpan path)
            : assemblyName(assemblyName)
            , path(path)
            , lastEditTime(0)
            , dependencies(4)
        {}

    };

    struct AssemblyInfo {
        FixedCharSpan assemblyName;
        FixedCharSpan path;
    };

    struct Compiler {

        Diagnostics diagnostics;
        Alchemy::Jobs::JobSystem jobSystem;
        MSIDictionary<FixedCharSpan, TypeInfo*> resolveMap;
        MSIDictionary<FixedCharSpan, FileInfo*> fileInfoByAbsolutePath;
        Alchemy::PoolAllocator<FileInfo> fileAllocator;

        static CheckedArray<TypeInfo*> s_BuiltInTypeInfos;

        PodList<FileInfo*> fileInfos;

        void SetupCompilationRun(TempAllocator * tempAllocator);

    };


}