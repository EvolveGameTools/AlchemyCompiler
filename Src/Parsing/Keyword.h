#pragma once

#include "../PrimitiveTypes.h"

namespace Alchemy::Parsing {

    enum class ContextualKeyword : uint8 {
        NotAKeyword
    };

    enum class Keyword : uint8 {

        NotAKeyword,

        As,
        Is,
        Out,
        New,
        Using,
        Typeof,
        Var,

        If,
        Else,
        Foreach,
        Continue,
        While,
        Break,
        Catch,
        Do,
        Finally,
        For,
        Switch,
        Case,
        Return,
        Try,
        Throw,

        Remember,
        Const,
        Namespace,
        Params,
        This,
        In,
        NameOf,
        Sizeof,

        Null,
        Default,

        Ref,
        Temp,
        Scoped,

        Static,
        Override,
        Abstract,
        Virtual,
        Sealed,

        Class,
        Struct,
        Delegate,
        Enum,
        Interface,

        Base,
        Extern,

        Public,
        Private,
        Protected,
        Internal,
        Export,

        Void,

        Get,
        Set,
        Where,
        When,
        With,

        True,
        False,

        // built in types
        Dynamic,
        Object,
        String,
        Ulong,
        Long,
        Ushort,
        Short,
        Double,
        Bool,
        Byte,
        Sbyte,
        Char,
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
        Uint,
        Uint2,
        Uint3,
        Uint4,
        Color,
        Color32,
        Color64,

        Single,
        Int8,
        Int16,
        Int32,
        Int64,

        Uint8,
        Uint16,
        Uint32,
        Uint64,

        Tempalloc,
        Scopealloc,
        Stackalloc,
        Constructor,

        Readonly,
    };

    int32 Fn();
    const char * KeywordToString(Keyword keyword);
}