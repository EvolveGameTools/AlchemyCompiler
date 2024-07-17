#pragma once

#include "../Collections/PodList.h"
#include "../Util/FixedCharSpan.h"
#include "../Parsing3/Diagnostics.h"

namespace Alchemy::Compilation {

    struct CompilationUnitSyntax;

    struct SourceFileInfo {

        FixedCharSpan assemblyName;
        FixedCharSpan path;
        uint64 lastEditTime {};
        PodList<SourceFileInfo*> dependencies;
        PodList<SourceFileInfo*> dependants;
        LinearAllocator allocator;
        Diagnostics diagnostics;
        CompilationUnitSyntax * syntaxTree {};

        bool wasTouched {};
        bool wasChanged {};
        bool dependantsVisited {};

        SourceFileInfo()
            : allocator(MEGABYTES(128), KILOBYTES(32))
            , diagnostics(&allocator) {}

        void Invalidate();

    };

}