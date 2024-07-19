#pragma once

#include "./SourceFileInfo.h"
#include "../Allocation/PoolAllocator.h"

namespace Alchemy::Compilation {

    SourceFileInfo* MakeBuiltInFile(PoolAllocator<SourceFileInfo>* fileAllocator, const char* fileName, const char* source);

    void LoadBuiltIns(PodList<SourceFileInfo*>* files, PoolAllocator<SourceFileInfo>* fileAllocator);

}