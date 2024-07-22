#pragma once

#include "../PrimitiveTypes.h"

#define BlitPointerField(x, y) int32 y##_offset {};                         \
        inline x * Get##y() {                                               \
            return (x*)(ts_LocalExpressionBase + y##_offset);               \
        }                                                                   \
        inline void Set##y(x * value) {                                     \
            if(value == nullptr) y##_offset = 0;                            \
            else y##_offset = ((uint8*)value) - ts_LocalExpressionBase;     \
        }                                                                   \
                                                                            \

namespace Alchemy::Compilation {

    thread_local uint8* ts_LocalExpressionBase;


    enum class ExpressionKind {
        Invalid,
        Binary
    };

    enum class BinaryExpressionOp {
        Invalid,
        Add,
        Subtract,
        Multiply,
        Divide,
        Modulo,
        ShiftRight,
        ShiftLeft
    };

    struct Expression {

        ExpressionKind kind;

        Expression(ExpressionKind kind)
            : kind(kind) {}

    };

    struct BinaryExpression : Expression {

        BlitPointerField(Expression, Left);
        BlitPointerField(Expression, Right);
        BinaryExpressionOp op;

        BinaryExpression(Expression* left, BinaryExpressionOp op, Expression* right)
            : Expression(ExpressionKind::Binary)
            , op(op) {
            SetLeft(left);
            SetRight(right);
        }

    };

}