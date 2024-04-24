#include <cstdio>
#include "./Writer.h"
#include "../Compiler/StringUtil.h"
#include "../Compiler/MemberInfo.h"
#include "../Allocation/LinearAllocator.h"

namespace Alchemy::Compilation {

    void Writer::WriteInt(int32 value) {
        int32 reserve = IntToAsciiCount(value);
        char* p = outputBuffer->Reserve(reserve);
        IntToAscii(value, p);
    }

    void Writer::WriteType(Alchemy::Compilation::ResolvedType resolvedType, bool emitStructKeyword) {
        // add to a set maybe. we'll need to output our structs eventually
        if (resolvedType.IsVoid()) {
            Write("void");
            return;
        }

        if (emitStructKeyword && resolvedType.IsNullable() || resolvedType.IsArray() || !IsPrimitiveTypeName(resolvedType.builtInTypeName)) {
            Write("struct ");
        }

        // we need to see how we handle nullable pointers, can probably decay to just using a void *
        if (resolvedType.IsNullable()) {
            Write("nullable_"); // could shorten this to "n_" if we need to save characters
        }

        switch (resolvedType.builtInTypeName) {
            case BuiltInTypeName::Invalid:
                assert(resolvedType.typeInfo != nullptr);
                outputBuffer->AddRange(resolvedType.typeInfo->typeName, resolvedType.typeInfo->typeNameLength);
                outputBuffer->Add('_');
                WriteInt(resolvedType.typeInfo->genericArgumentCount);
                outputBuffer->Add('_');
                EncodePointerBase32(resolvedType.typeInfo, outputBuffer->Reserve(kBase32PointerLength));
                break;
            case BuiltInTypeName::Int:
                Write("int32");
                break;
            case BuiltInTypeName::Int2:
                Write("int2");
                break;
            case BuiltInTypeName::Int3:
                Write("int3");
                break;
            case BuiltInTypeName::Int4:
                Write("int4");
                break;
            case BuiltInTypeName::Uint:
                Write("uint32");
                break;
            case BuiltInTypeName::Uint2:
                Write("uint2");
                break;
            case BuiltInTypeName::Uint3:
                Write("uint3");
                break;
            case BuiltInTypeName::Uint4:
                Write("uint4");
                break;
            case BuiltInTypeName::Float:
                Write("float");
                break;
            case BuiltInTypeName::Float2:
                Write("float2");
                break;
            case BuiltInTypeName::Float3:
                Write("float3");
                break;
            case BuiltInTypeName::Float4:
                Write("float4");
                break;
            case BuiltInTypeName::Bool:
                Write("bool");
                break;
            case BuiltInTypeName::Char:
                Write("unsigned char");
                break;
            case BuiltInTypeName::Color:
                Write("color");
                break;
            case BuiltInTypeName::Color32:
                Write("color32");
                break;
            case BuiltInTypeName::Color64:
                Write("color64");
                break;
            case BuiltInTypeName::Byte:
                Write("uint8");
                break;
            case BuiltInTypeName::Sbyte:
                Write("int8");
                break;
            case BuiltInTypeName::Short:
                Write("int16");
                break;
            case BuiltInTypeName::Ushort:
                Write("uint16");
                break;
            case BuiltInTypeName::Long:
                Write("int64");
                break;
            case BuiltInTypeName::Ulong:
                Write("uint64");
                break;
            case BuiltInTypeName::Dynamic:
                Write("dynamic");
                break;
            case BuiltInTypeName::String:
                Write("string");
                break;
            case BuiltInTypeName::Object:
                Write("object");
                break;
            case BuiltInTypeName::Double:
                Write("double");
                break;
            case BuiltInTypeName::Void:
                Write("void");
                break;
        }

        if (resolvedType.IsArray()) {
            Write("_array"); // could shorten to _a if we need to save characters
        }

    }

    void Writer::Write(const char * str) {
        outputBuffer->AddRange(str, (int32)strlen(str));
    }

    void Writer::Write(FixedCharSpan span) {
        outputBuffer->AddRange(span.ptr, span.size);
    }

    void Writer::WriteFloat(float value) {
        char buffer[64];
        int32 cnt = snprintf(buffer, sizeof(buffer), "%f", value);
        outputBuffer->AddRange(buffer, cnt);
    }

    void Writer::WriteMethodName(MethodInfo* methodInfo) {
        // typename_methodName_gencnt_argcnt_base32encode
        Write(methodInfo->declaringType->GetTypeName());
        Write("_");
        Write(methodInfo->GetName());
        Write("_");
        WriteInt(methodInfo->genericArgumentCount);
        Write("_");
        WriteInt(methodInfo->parameterCount);
        Write("_");
        // todo -- if we need a reproducible output across compile passes we can probably
        // map this in a stored way that we can retrieve. I don't know how yet.
        EncodePointerBase32(methodInfo, outputBuffer->Reserve(kBase32PointerLength));
    }

}