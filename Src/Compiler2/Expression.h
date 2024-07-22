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


    enum class ExpressionKind : uint8 {

        Invalid,
        Binary,
        FieldAccess,
        PropertyAccess,

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
        // 3 bytes padding
        LineColumn location;

        Expression(ExpressionKind kind, LineColumn location)
            : kind(kind)
            , location(location) {}

    };

    struct BinaryExpression : Expression {

        BlitPointerField(Expression, Left);
        BlitPointerField(Expression, Right);
        BinaryExpressionOp op;

        BinaryExpression(Expression* left, BinaryExpressionOp op, Expression* right, LineColumn location)
            : Expression(ExpressionKind::Binary, location)
            , op(op) {
            SetLeft(left);
            SetRight(right);
        }

    };

    struct PropertyAccessExpression : Expression {

        BlitPointerField(Expression, Instance);
        PropertyInfo* propertyInfo;

        PropertyAccessExpression(Expression* instance, PropertyInfo* propertyInfo, LineColumn location)
            : Expression(ExpressionKind::PropertyAccess, location)
            , Instance_offset(0)
            , propertyInfo(propertyInfo) {
            SetInstance(instance);
        }

    };

    struct FieldAccessExpression : Expression {

        BlitPointerField(Expression, Instance);
        FieldInfo* fieldInfo;

        FieldAccessExpression(Expression* instance, FieldInfo* fieldInfo, LineColumn location)
            : Expression(ExpressionKind::FieldAccess, location)
            , Instance_offset(0)
            , fieldInfo(fieldInfo) {
            SetInstance(instance);
        }

    };

}