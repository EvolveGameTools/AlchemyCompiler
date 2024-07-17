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
    }

}


