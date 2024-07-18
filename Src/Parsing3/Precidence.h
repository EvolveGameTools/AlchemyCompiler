#pragma once
#include "../PrimitiveTypes.h"

namespace Alchemy::Compilation {

    enum class Precedence : uint8 {
        Expression = 0, // Loosest possible precedence, used to accept all expressions
        Assignment = Expression,
        Lambda = Assignment, // "The => operator has the same precedence as assignment (=) and is right-associative."
        Conditional,
        Coalescing,
        ConditionalOr,
        ConditionalAnd,
        LogicalOr,
        LogicalXor,
        LogicalAnd,
        Equality,
        Relational,
        Shift,
        Additive,
        Multiplicative,
        Switch,
        Range,
        Unary,
        Cast,
        Primary,
    };

}