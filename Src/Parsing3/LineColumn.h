#pragma once
#include "../PrimitiveTypes.h"

namespace Alchemy::Compilation {

    struct LineColumn {
        int32 line;
        int32 column;
        int32 endLine;
        int32 endColumn;
    };

}