#pragma once

#include "../PrimitiveTypes.h"

namespace Alchemy::Compilation {

    struct LineColumn {

        int32 line;
        int32 endLine;
        uint16 column;
        uint16 endColumn;

        LineColumn()
            : line(0)
            , endLine(0)
            , column(0)
            , endColumn(0) {}

        LineColumn(int32 startLine, int32 startColumn, int32 endLine, int32 endColumn)
            : line(startLine)
            , endLine(endLine)
            , column((uint16) startColumn)
            , endColumn(uint16(endColumn)) {}

    };

}