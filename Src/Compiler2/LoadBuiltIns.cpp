#include "./LoadBuiltIns.h"

namespace Alchemy::Compilation {

    SourceFileInfo* MakeBuiltInFile(PoolAllocator<SourceFileInfo>* fileAllocator, const char* fileName, const char* source) {
        SourceFileInfo* file = fileAllocator->Allocate();

        new(file) SourceFileInfo();

        file->wasTouched = true;
        file->wasChanged = true;
        file->isBuiltIn = true;
        file->dependantsVisited = false;
        file->lastEditTime = 0;
        file->path = FixedCharSpan(fileName);
        file->assemblyName = FixedCharSpan("BuiltIn");
        file->contents = FixedCharSpan(source);

        return file;
    }

}