#include "../ParseTypes.h"

namespace Alchemy::Parsing {

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

    BuiltInTypeName BuiltInTypeNameFromKeyword(Keyword keyword) {
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

    const char* BuiltInTypeNameToFullyQualifiedName(BuiltInTypeName typeName) {
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

    bool IsBuiltInTypeName(Keyword keyword) {
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
}