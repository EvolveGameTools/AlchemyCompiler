#pragma once

#include "../Collections/PodList.h"
#include "../Util/FixedCharSpan.h"
#include "../Parsing3/Diagnostics.h"
#include "../Parsing3/Tokenizer.h"
#include <mutex>

namespace Alchemy::Compilation {

    struct CompilationUnitSyntax;
    struct TypeInfo;

    struct SourceFileInfo {

        FixedCharSpan assemblyName;
        FixedCharSpan path;
        uint64 lastEditTime {};
        PodList<SourceFileInfo*> dependencies;
        PodList<SourceFileInfo*> dependants;
        LinearAllocator allocator; // might want a mutex for this eventually
        Diagnostics diagnostics;
        CompilationUnitSyntax * syntaxTree {};

        FixedCharSpan namespaceName;
        TokenizerResult tokenizerResult;
        CheckedArray<TypeInfo*> declaredTypes;
        CheckedArray<FixedCharSpan> usingDirectives;

        bool wasTouched {};
        bool wasChanged {};
        bool dependantsVisited {};
        std::mutex mutex;

        SourceFileInfo()
            : allocator(MEGABYTES(128), KILOBYTES(32))
            , diagnostics(allocator.MakeAllocator()) {}

        void Invalidate();

        static uint8* AllocateLocked(void * cookie, size_t size, size_t alignment);

        Allocator GetLockedAllocator();

    };


}