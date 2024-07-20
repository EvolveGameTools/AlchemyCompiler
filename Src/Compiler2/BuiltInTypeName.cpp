#include "./BuiltInTypeName.h"
#include "../Parsing3/TokenKind.h"

namespace Alchemy::Compilation {

    bool IsPrimitiveTypeName(BuiltInTypeName typeName) {
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

    BuiltInTypeName BuiltInTypeNameFromKeyword(TokenKind keyword) {
        switch (keyword) {
            case TokenKind::IntKeyword:
                return BuiltInTypeName::Int;
            case TokenKind::Int2Keyword:
                return BuiltInTypeName::Int2;
            case TokenKind::Int3Keyword:
                return BuiltInTypeName::Int3;
            case TokenKind::Int4Keyword:
                return BuiltInTypeName::Int4;
            case TokenKind::UIntKeyword:
                return BuiltInTypeName::Uint;
            case TokenKind::Uint2Keyword:
                return BuiltInTypeName::Uint2;
            case TokenKind::Uint3Keyword:
                return BuiltInTypeName::Uint3;
            case TokenKind::Uint4Keyword:
                return BuiltInTypeName::Uint4;
            case TokenKind::FloatKeyword:
                return BuiltInTypeName::Float;
            case TokenKind::Float2Keyword:
                return BuiltInTypeName::Float2;
            case TokenKind::Float3Keyword:
                return BuiltInTypeName::Float3;
            case TokenKind::Float4Keyword:
                return BuiltInTypeName::Float4;
            case TokenKind::BoolKeyword:
                return BuiltInTypeName::Bool;
            case TokenKind::CharKeyword:
                return BuiltInTypeName::Char;
            case TokenKind::Color8Keyword:
                return BuiltInTypeName::Color;
            case TokenKind::Color16Keyword:
                return BuiltInTypeName::Color32;
            case TokenKind::Color32Keyword:
                return BuiltInTypeName::Color64;
            case TokenKind::ByteKeyword:
                return BuiltInTypeName::Byte;
            case TokenKind::SByteKeyword:
                return BuiltInTypeName::Sbyte;
            case TokenKind::ShortKeyword:
                return BuiltInTypeName::Short;
            case TokenKind::UShortKeyword:
                return BuiltInTypeName::Ushort;
            case TokenKind::LongKeyword:
                return BuiltInTypeName::Long;
            case TokenKind::ULongKeyword:
                return BuiltInTypeName::Ulong;
            case TokenKind::DynamicKeyword:
                return BuiltInTypeName::Dynamic;
            case TokenKind::StringKeyword:
                return BuiltInTypeName::String;
            case TokenKind::ObjectKeyword:
                return BuiltInTypeName::Object;
            case TokenKind::DoubleKeyword:
                return BuiltInTypeName::Double;
            case TokenKind::VoidKeyword:
                return BuiltInTypeName::Void;
//            case TokenKind::Single:
//                return BuiltInTypeName::Single;
//            case TokenKind::Int8:
//                return BuiltInTypeName::Int8;
//            case TokenKind::Int16:
//                return BuiltInTypeName::Int16;
//            case TokenKind::Int32:
//                return BuiltInTypeName::Int32;
//            case TokenKind::Int64:
//                return BuiltInTypeName::Int64;
//            case TokenKind::Uint8:
//                return BuiltInTypeName::UInt8;
//            case TokenKind::Uint16:
//                return BuiltInTypeName::UInt16;
//            case TokenKind::Uint32:
//                return BuiltInTypeName::UInt32;
//            case TokenKind::Uint64:
//                return BuiltInTypeName::UInt64;

            default: {
                return BuiltInTypeName::Invalid;
            }
        }
    }

    const char* BuiltInTypeNameToFullyQualifiedName(BuiltInTypeName typeName) {
        switch (typeName) {
            case BuiltInTypeName::Int:
                return "BuiltIn::Int32";
            case BuiltInTypeName::Int2:
                return "BuiltIn::Int2";
            case BuiltInTypeName::Int3:
                return "BuiltIn::Int3";
            case BuiltInTypeName::Int4:
                return "BuiltIn::Int4";
            case BuiltInTypeName::Uint:
                return "BuiltIn::UInt";
            case BuiltInTypeName::Uint2:
                return "BuiltIn::UInt2";
            case BuiltInTypeName::Uint3:
                return "BuiltIn::UInt3";
            case BuiltInTypeName::Uint4:
                return "BuiltIn::UInt4";
            case BuiltInTypeName::Float:
                return "BuiltIn::Float";
            case BuiltInTypeName::Float2:
                return "BuiltIn::Float2";
            case BuiltInTypeName::Float3:
                return "BuiltIn::Float3";
            case BuiltInTypeName::Float4:
                return "BuiltIn::Float4";
            case BuiltInTypeName::Bool:
                return "BuiltIn::Bool";
            case BuiltInTypeName::Char:
                return "BuiltIn::Char";
            case BuiltInTypeName::Color:
                return "BuiltIn::Color";
            case BuiltInTypeName::Color32:
                return "BuiltIn::Color32";
            case BuiltInTypeName::Color64:
                return "BuiltIn::Color64";
            case BuiltInTypeName::Byte:
                return "BuiltIn::Byte";
            case BuiltInTypeName::Sbyte:
                return "BuiltIn::SByte";
            case BuiltInTypeName::Short:
                return "BuiltIn::Int16";
            case BuiltInTypeName::Ushort:
                return "BuiltIn::UInt16";
            case BuiltInTypeName::Long:
                return "BuiltIn::Int64";
            case BuiltInTypeName::Ulong:
                return "BuiltIn::UInt64";
            case BuiltInTypeName::Dynamic:
                return "BuiltIn::Dynamic";
            case BuiltInTypeName::String:
                return "BuiltIn::String";
            case BuiltInTypeName::Object:
                return "BuiltIn:Object";
            case BuiltInTypeName::Double:
                return "BuiltIn::Double";
            case BuiltInTypeName::Void:
                return "BuiltIn::Void";
            case BuiltInTypeName::Invalid:
            default: {
                return "";
            }
        }
    }

    const char* BuiltInTypeNameToString(BuiltInTypeName typeName) {
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

    bool IsBuiltInTypeName(TokenKind keyword) {
        switch (keyword) {
            case TokenKind::IntKeyword:
            case TokenKind::Int2Keyword:
            case TokenKind::Int3Keyword:
            case TokenKind::Int4Keyword:
            case TokenKind::UIntKeyword:
            case TokenKind::Uint2Keyword:
            case TokenKind::Uint3Keyword:
            case TokenKind::Uint4Keyword:
            case TokenKind::FloatKeyword:
            case TokenKind::Float2Keyword:
            case TokenKind::Float3Keyword:
            case TokenKind::Float4Keyword:
            case TokenKind::BoolKeyword:
            case TokenKind::CharKeyword:
            case TokenKind::Color8Keyword:
            case TokenKind::Color16Keyword:
            case TokenKind::Color32Keyword:
            case TokenKind::ByteKeyword:
            case TokenKind::SByteKeyword:
            case TokenKind::ShortKeyword:
            case TokenKind::UShortKeyword:
            case TokenKind::LongKeyword:
            case TokenKind::ULongKeyword:
            case TokenKind::DynamicKeyword:
            case TokenKind::StringKeyword:
            case TokenKind::ObjectKeyword:
            case TokenKind::DoubleKeyword:
            case TokenKind::VoidKeyword:
                return true;
            default: {
                return false;
            }
        }
    }
}