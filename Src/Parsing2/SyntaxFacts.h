#pragma once

#include "../PrimitiveTypes.h"

namespace Alchemy::Parsing::SyntaxFacts {

    bool IsHexDigit(char c);
    int32 HexValue(char c);

    bool IsBinaryDigit(char ch);

    bool IsDecDigit(char c);
}