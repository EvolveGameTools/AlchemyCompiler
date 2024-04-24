#pragma once

#include "../PrimitiveTypes.h"
#include "./NodeIndex.h"
#include "./Keyword.h"
#include "./BuiltInTypeName.h"

namespace Alchemy::Parsing {

    enum class DefinitionVisibility : uint8 {

        Unspecified,
        Public,
        Private,
        Internal,
        Protected,
        Export

    };

    enum class TypeAccessModifier : uint8 {
        Default,
        Ref,
        // Readonly
    };

    enum class StorageClass : uint8 {

        Default,
        Scoped,
        Temp

    };

    DEFINE_ENUM_FLAGS(DeclarationModifier, uint16, {

        None = 0,
        Static = 1 << 0,
        Sealed = 1 << 1,
        Abstract = 1 << 2,
        Readonly = 1 << 3,
        Virtual = 1 << 4,
        Override = 1 << 5,
        Public = 1 << 6,
        Private = 1 << 7,
        Protected = 1 << 8,
        Internal = 1 << 9,
        Ref = 1 << 10,
        Export = 1 << 11,
        Const = 1 << 12,

        VisibilityMask = Public | Private | Export | Internal | Protected

    });

    enum class MethodTypeSpecifier : uint8 {

        Default,
        Sealed,
        Override,
        Virtual,
        Abstract,
        Static,

    };

    enum class VariableTypeModifier : uint8 {

        None,
        Ref,
        Temp,
        Scoped,
        TempRef,

    };

    enum class BinaryExpressionType : uint8 {

        None,
        ConditionalOr,
        ConditionalAnd,
        BitwiseOr,
        BitwiseXor,
        BitwiseAnd,

        Equal,
        NotEqual,

        LessThan,
        GreaterThan,
        LessThanOrEqualTo,
        GreaterThanOrEqualTo,
        ShiftRight,
        ShiftLeft,

        Add,
        Subtract,
        Multiply,
        Divide,
        Modulus

    };

    enum class UnaryExpressionType : uint8 {

        None,
        Plus,
        Minus,
        Not,
        BitwiseNot,
        PreIncrement,
        PreDecrement

    };

    union LiteralValue {

        double doubleValue;
        float floatValue;
        int64 int64Value;
        int32 int32Value;
        int16 int16Value;
        int8 int8Value;

        uint64 uint64Value;
        uint32 uint32Value;
        uint16 uint16Value;
        uint8 uint8Value;
        char charValue; // maybe take a char32 here?
        bool boolValue;
        UntypedNodeIndex stringNodeIndex;

        LiteralValue() {
            doubleValue = 0;
        }

    };

    enum class ArgumentPassByModifier : uint8 {

        None,
        Ref,
        Out

    };

    enum class AllocatorType : uint8 {
        Default = 0,
        TempAllocator,
        ScopeAllocator,
        StackAllocator
    };

    enum class StringPartType : uint8 {

        RegularStringLiteral,
        ShortInterpolation,
        LongInterpolation

    };

    // these are the type names we can have literals for via suffixes
    // other types require a cast to represent
    enum class NumericLiteralTypeName : uint8 {
        Int32,
        Int64,
        UInt32,
        UInt64,
        Float,
        Double
    };

    union NumericLiteralValue {
        int32 i32;
        int64 i64;
        uint32 u32;
        uint64 u64;
        float f;
        double d {0};
    };


    enum class AssignmentOperator {

        Invalid,
        Assign,
        PlusEquals,
        MinusEquals,
        TimesEquals,
        DivideEquals,
        ModEquals,
        AndEquals,
        OrEquals,
        XorEquals,
        LeftShiftEquals,
        RightShiftEquals,
        CoalesceEquals

    };
}