#pragma once
#include "../PrimitiveTypes.h"
#include "Keyword.h"

namespace Alchemy {
    const char * KeywordToString(Keyword keyword) {
        switch(keyword) {
            case Keyword::As: return "as";
            case Keyword::Is: return "is";
            case Keyword::Out: return "out";
            case Keyword::New: return "new";
            case Keyword::Using: return "using";
            case Keyword::Typeof: return "typeof";
            case Keyword::Var: return "var";
            case Keyword::If: return "if";
            case Keyword::Else: return "else";
            case Keyword::Foreach: return "foreach";
            case Keyword::Continue: return "continue";
            case Keyword::While: return "while";
            case Keyword::Break: return "break";
            case Keyword::Catch: return "catch";
            case Keyword::Do: return "do";
            case Keyword::Finally: return "finally";
            case Keyword::For: return "for";
            case Keyword::Switch: return "switch";
            case Keyword::Case: return "case";
            case Keyword::Return: return "return";
            case Keyword::Try: return "try";
            case Keyword::Throw: return "throw";
            case Keyword::Remember: return "remember";
            case Keyword::Const: return "const";
            case Keyword::Namespace: return "namespace";
            case Keyword::Params: return "params";
            case Keyword::This: return "this";
            case Keyword::In: return "in";
            case Keyword::NameOf: return "nameof";
            case Keyword::Sizeof: return "sizeof";
            case Keyword::Null: return "null";
            case Keyword::Default: return "default";
            case Keyword::Ref: return "ref";
            case Keyword::Temp: return "temp";
            case Keyword::Scoped: return "scoped";
            case Keyword::Static: return "static";
            case Keyword::Override: return "override";
            case Keyword::Abstract: return "abstract";
            case Keyword::Virtual: return "virtual";
            case Keyword::Sealed: return "sealed";
            case Keyword::Class: return "class";
            case Keyword::Struct: return "struct";
            case Keyword::Delegate: return "delegate";
            case Keyword::Enum: return "enum";
            case Keyword::Interface: return "interface";
            case Keyword::Base: return "base";
            case Keyword::Extern: return "extern";
            case Keyword::Public: return "public";
            case Keyword::Private: return "private";
            case Keyword::Protected: return "protected";
            case Keyword::Internal: return "internal";
            case Keyword::Export: return "export";
            case Keyword::Void: return "void";
            case Keyword::Get: return "get";
            case Keyword::Set: return "set";
            case Keyword::Where: return "where";
            case Keyword::When: return "when";
            case Keyword::With: return "with";
            case Keyword::True: return "true";
            case Keyword::False: return "false";
            case Keyword::Dynamic: return "dynamic";
            case Keyword::Object: return "object";
            case Keyword::String: return "string";
            case Keyword::Ulong: return "ulong";
            case Keyword::Long: return "long";
            case Keyword::Ushort: return "ushort";
            case Keyword::Short: return "short";
            case Keyword::Double: return "double";
            case Keyword::Bool: return "bool";
            case Keyword::Byte: return "byte";
            case Keyword::Sbyte: return "sbyte";
            case Keyword::Char: return "char";
            case Keyword::Float: return "float";
            case Keyword::Float2: return "float2";
            case Keyword::Float3: return "float3";
            case Keyword::Float4: return "float4";
            case Keyword::Int: return "int";
            case Keyword::Int2: return "int2";
            case Keyword::Int3: return "int3";
            case Keyword::Int4: return "int4";
            case Keyword::Uint: return "uint";
            case Keyword::Uint2: return "uint2";
            case Keyword::Uint3: return "uint3";
            case Keyword::Uint4: return "uint4";
            case Keyword::Color: return "color";
            case Keyword::Color32: return "color32";
            case Keyword::Color64: return "color64";
            case Keyword::Single: return "single";
            case Keyword::Int8: return "int8";
            case Keyword::Int16: return "int16";
            case Keyword::Int32: return "int32";
            case Keyword::Int64: return "int64";
            case Keyword::Uint8: return "uint8";
            case Keyword::Uint16: return "uint16";
            case Keyword::Uint32: return "uint32";
            case Keyword::Uint64: return "uint64";
            case Keyword::Tempalloc: return "tempalloc";
            case Keyword::Scopealloc: return "scopealloc";
            case Keyword::Stackalloc: return "stackalloc";
            case Keyword::Constructor: return "constructor";
            case Keyword::Readonly: return "readonly";
            default: return "Invalid";
        }
    }
}