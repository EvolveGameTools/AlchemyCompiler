#pragma once

#include "../PrimitiveTypes.h"
#include "../Parsing3/TokenKind.h"

namespace Alchemy::Compilation {

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

    constexpr int32 kBuiltInTypeCount = (int32) ((BuiltInTypeName::Void)) + 1;

    bool IsPrimitiveTypeName(BuiltInTypeName typeName);

    BuiltInTypeName BuiltInTypeNameFromKeyword(TokenKind keyword);

    const char* BuiltInTypeNameToFullyQualifiedName(BuiltInTypeName typeName);

    const char* BuiltInTypeNameToString(BuiltInTypeName typeName);

    bool IsBuiltInTypeName(TokenKind keyword);
}