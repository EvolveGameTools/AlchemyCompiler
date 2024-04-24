#pragma once

#include "NodeIndex.h"
#include "Keyword.h"

namespace Alchemy {

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

    enum class DeclarationModifier : uint16 {

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

    };

    IMPL_ENUM_FLAGS_OPERATORS(DeclarationModifier);

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

    enum class BuiltInTypeName : uint8 {
        Invalid,

        Int,
        Int2,
        Int3,
        Int4,
        Uint,
        Uint2,
        Uint3,
        Uint4,
        Float,
        Float2,
        Float3,
        Float4,
        Bool,
        Char,
        Color,
        Color32,
        Color64,
        Byte,
        Sbyte,
        Short,
        Ushort,
        Long,
        Ulong,
        Dynamic,
        String,
        Object,
        Double,
        Void, // must be last non alias

        // aliases
        Single = Float,
        Int8 = Sbyte,
        Int16 = Short,
        Int32 = Int,
        Int64 = Long,

        UInt8 = Byte,
        UInt16 = Ushort,
        UInt32 = Uint,
        UInt64 = Ulong,


    };

    inline bool IsPrimitiveTypeName(BuiltInTypeName typeName) {
        switch (typeName) {
            case BuiltInTypeName::Bool:
            case BuiltInTypeName::Char:
            case BuiltInTypeName::Int8:
            case BuiltInTypeName::Int16:
            case BuiltInTypeName::Int32:
            case BuiltInTypeName::Int64:
            case BuiltInTypeName::UInt8:
            case BuiltInTypeName::UInt16:
            case BuiltInTypeName::UInt32:
            case BuiltInTypeName::UInt64:
            case BuiltInTypeName::Float:
            case BuiltInTypeName::Double:
                return true;
            default:
                return false;
        }
    }

    constexpr int32 kBuiltInTypeCount = (int32) ((BuiltInTypeName::Void)) + 1;

    inline BuiltInTypeName BuiltInTypeNameFromKeyword(Keyword keyword) {
        switch (keyword) {
            case Keyword::Int:
                return BuiltInTypeName::Int;
            case Keyword::Int2:
                return BuiltInTypeName::Int2;
            case Keyword::Int3:
                return BuiltInTypeName::Int3;
            case Keyword::Int4:
                return BuiltInTypeName::Int4;
            case Keyword::Uint:
                return BuiltInTypeName::Uint;
            case Keyword::Uint2:
                return BuiltInTypeName::Uint2;
            case Keyword::Uint3:
                return BuiltInTypeName::Uint3;
            case Keyword::Uint4:
                return BuiltInTypeName::Uint4;
            case Keyword::Float:
                return BuiltInTypeName::Float;
            case Keyword::Float2:
                return BuiltInTypeName::Float2;
            case Keyword::Float3:
                return BuiltInTypeName::Float3;
            case Keyword::Float4:
                return BuiltInTypeName::Float4;
            case Keyword::Bool:
                return BuiltInTypeName::Bool;
            case Keyword::Char:
                return BuiltInTypeName::Char;
            case Keyword::Color:
                return BuiltInTypeName::Color;
            case Keyword::Color32:
                return BuiltInTypeName::Color32;
            case Keyword::Color64:
                return BuiltInTypeName::Color64;
            case Keyword::Byte:
                return BuiltInTypeName::Byte;
            case Keyword::Sbyte:
                return BuiltInTypeName::Sbyte;
            case Keyword::Short:
                return BuiltInTypeName::Short;
            case Keyword::Ushort:
                return BuiltInTypeName::Ushort;
            case Keyword::Long:
                return BuiltInTypeName::Long;
            case Keyword::Ulong:
                return BuiltInTypeName::Ulong;
            case Keyword::Dynamic:
                return BuiltInTypeName::Dynamic;
            case Keyword::String:
                return BuiltInTypeName::String;
            case Keyword::Object:
                return BuiltInTypeName::Object;
            case Keyword::Double:
                return BuiltInTypeName::Double;
            case Keyword::Void:
                return BuiltInTypeName::Void;

            case Keyword::Single:
                return BuiltInTypeName::Single;
            case Keyword::Int8:
                return BuiltInTypeName::Int8;
            case Keyword::Int16:
                return BuiltInTypeName::Int16;
            case Keyword::Int32:
                return BuiltInTypeName::Int32;
            case Keyword::Int64:
                return BuiltInTypeName::Int64;
            case Keyword::Uint8:
                return BuiltInTypeName::UInt8;
            case Keyword::Uint16:
                return BuiltInTypeName::UInt16;
            case Keyword::Uint32:
                return BuiltInTypeName::UInt32;
            case Keyword::Uint64:
                return BuiltInTypeName::UInt64;

            default: {
                return BuiltInTypeName::Invalid;
            }
        }
    }

    inline const char* BuiltInTypeNameToFullyQualifiedName(BuiltInTypeName typeName) {
        switch (typeName) {
            case BuiltInTypeName::Int:
                return "System::Int32";
            case BuiltInTypeName::Int2:
                return "System::Int2";
            case BuiltInTypeName::Int3:
                return "System::Int3";
            case BuiltInTypeName::Int4:
                return "System::Int4";
            case BuiltInTypeName::Uint:
                return "System::UInt";
            case BuiltInTypeName::Uint2:
                return "System::UInt2";
            case BuiltInTypeName::Uint3:
                return "System::UInt3";
            case BuiltInTypeName::Uint4:
                return "System::UInt4";
            case BuiltInTypeName::Float:
                return "System::Float";
            case BuiltInTypeName::Float2:
                return "System::Float2";
            case BuiltInTypeName::Float3:
                return "System::Float3";
            case BuiltInTypeName::Float4:
                return "System::Float4";
            case BuiltInTypeName::Bool:
                return "System::Bool";
            case BuiltInTypeName::Char:
                return "System::Char";
            case BuiltInTypeName::Color:
                return "System::Color";
            case BuiltInTypeName::Color32:
                return "System::Color32";
            case BuiltInTypeName::Color64:
                return "System::Color64";
            case BuiltInTypeName::Byte:
                return "System::Byte";
            case BuiltInTypeName::Sbyte:
                return "System::SByte";
            case BuiltInTypeName::Short:
                return "System::Int16";
            case BuiltInTypeName::Ushort:
                return "System::UInt16";
            case BuiltInTypeName::Long:
                return "System::Int64";
            case BuiltInTypeName::Ulong:
                return "System::UInt64";
            case BuiltInTypeName::Dynamic:
                return "System::Dynamic";
            case BuiltInTypeName::String:
                return "System::String";
            case BuiltInTypeName::Object:
                return "System:Object";
            case BuiltInTypeName::Double:
                return "System::Double";
            case BuiltInTypeName::Void:
                return "System::Void";
            case BuiltInTypeName::Invalid:
            default: {
                return "";
            }
        }
    }

    inline const char* BuiltInTypeNameToString(BuiltInTypeName typeName) {
        switch (typeName) {
            case BuiltInTypeName::Int:
                return "int";
            case BuiltInTypeName::Int2:
                return "int2";
            case BuiltInTypeName::Int3:
                return "int3";
            case BuiltInTypeName::Int4:
                return "int4";
            case BuiltInTypeName::Uint:
                return "uint";
            case BuiltInTypeName::Uint2:
                return "uint2";
            case BuiltInTypeName::Uint3:
                return "uint3";
            case BuiltInTypeName::Uint4:
                return "uint4";
            case BuiltInTypeName::Float:
                return "float";
            case BuiltInTypeName::Float2:
                return "float2";
            case BuiltInTypeName::Float3:
                return "float3";
            case BuiltInTypeName::Float4:
                return "float4";
            case BuiltInTypeName::Bool:
                return "bool";
            case BuiltInTypeName::Char:
                return "char";
            case BuiltInTypeName::Color:
                return "color";
            case BuiltInTypeName::Color32:
                return "color32";
            case BuiltInTypeName::Color64:
                return "color64";
            case BuiltInTypeName::Byte:
                return "byte";
            case BuiltInTypeName::Sbyte:
                return "sbyte";
            case BuiltInTypeName::Short:
                return "short";
            case BuiltInTypeName::Ushort:
                return "ushort";
            case BuiltInTypeName::Long:
                return "long";
            case BuiltInTypeName::Ulong:
                return "ulong";
            case BuiltInTypeName::Dynamic:
                return "dynamic";
            case BuiltInTypeName::String:
                return "string";
            case BuiltInTypeName::Object:
                return "object";
            case BuiltInTypeName::Double:
                return "double";
            case BuiltInTypeName::Void:
                return "void";
            default: {
                return "";
            }
        }
    }

    inline bool IsBuiltInTypeName(Keyword keyword) {
        switch (keyword) {
            case Keyword::Int:
            case Keyword::Int2:
            case Keyword::Int3:
            case Keyword::Int4:
            case Keyword::Uint:
            case Keyword::Uint2:
            case Keyword::Uint3:
            case Keyword::Uint4:
            case Keyword::Float:
            case Keyword::Float2:
            case Keyword::Float3:
            case Keyword::Float4:
            case Keyword::Bool:
            case Keyword::Char:
            case Keyword::Color:
            case Keyword::Color32:
            case Keyword::Color64:
            case Keyword::Byte:
            case Keyword::Sbyte:
            case Keyword::Short:
            case Keyword::Ushort:
            case Keyword::Long:
            case Keyword::Ulong:
            case Keyword::Dynamic:
            case Keyword::String:
            case Keyword::Object:
            case Keyword::Double:
            case Keyword::Void:
                return true;
            default: {
                return false;
            }
        }
    }

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