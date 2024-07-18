#include "./SourceFileInfo.h"

namespace Alchemy::Compilation {

    void SourceFileInfo::Invalidate() {
        allocator.Clear();
        diagnostics.size = 0;
        wasChanged = true;
        wasTouched = true;
        dependantsVisited = true;
        dependants.size = 0;
        dependencies.size = 0;
        syntaxTree = nullptr;
        namespaceName = FixedCharSpan();
        declaredTypes = CheckedArray<TypeInfo*>();
        usingDirectives = CheckedArray<FixedCharSpan>();
        tokenizerResult = TokenizerResult();

    }

    uint8* SourceFileInfo::AllocateLocked(void* cookie, size_t size, size_t alignment) {
        SourceFileInfo* src = (SourceFileInfo*)cookie;
        std::unique_lock lock(src->mutex);
        return (uint8*) src->allocator.AllocateBytesUncleared(size, alignment);
    }

    Allocator SourceFileInfo::GetLockedAllocator() {
        return Allocator(this, AllocateLocked);
    }

}


