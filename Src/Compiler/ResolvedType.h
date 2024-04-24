#pragma once

namespace Alchemy::Compilation {

    TypeInfo* GetBuiltInTypeInfo(BuiltInTypeName typeName);

    enum class ResolvedTypeFlags : uint16 {
        None = 0,
        IsVoid = 1 << 0,
        IsNullable = 1 << 1,
        IsArray = 1 << 2,
        IsNullableArray = 1 << 3,
        IsEnum = 1 << 5,
        IsVector = 1 << 6,
        IsNullOrDefault = 1 << 7,
        IsMethodGroup = 1 << 8,
        IsVar = 1 << 9
    };

    IMPL_ENUM_FLAGS_OPERATORS(ResolvedTypeFlags);

    struct ResolvedType {

        TypeInfo* typeInfo {};

        ResolvedTypeFlags flags;
        uint8 arrayRank {};
        BuiltInTypeName builtInTypeName;

        ResolvedType()
                : typeInfo(nullptr)
                  , arrayRank(0)
                  , flags(ResolvedTypeFlags::None)
                  , builtInTypeName(BuiltInTypeName::Invalid) {}

        explicit ResolvedType(TypeInfo* typeInfo)
            : typeInfo(typeInfo)
            , arrayRank(0)
            , flags(ResolvedTypeFlags::None)
            , builtInTypeName(BuiltInTypeName::Invalid) {}

        explicit ResolvedType(BuiltInTypeName name)
            : typeInfo(GetBuiltInTypeInfo(name))
            , arrayRank(0)
            , builtInTypeName(name)
            , flags(ResolvedTypeFlags::None) {}

        ResolvedType(TypeInfo* typeInfo, BuiltInTypeName builtInTypeName, int32 arrayRank, ResolvedTypeFlags flags)
            : typeInfo(typeInfo)
            , arrayRank(arrayRank)
            , builtInTypeName(builtInTypeName)
            , flags(flags) {}

        int32 GetHashCode() const {

            const uint32 kKnuthConstant = 2654435761;

            uint64 values[2];
            values[0] = (*(uint64*)this) * kKnuthConstant;
            values[1] = (*(uint64*)this) * kKnuthConstant;

            uint64 hash = 17;
            hash = hash * 31 + values[0];
            hash = hash * 31 + values[1];
            return (int32)hash;

        }

        bool IsPrimitive() {
            return IsVoid() || (!IsNullable() && !IsArray() && IsPrimitiveTypeName(builtInTypeName));
        }

        bool IsCallable() const {
            return (typeInfo != nullptr && typeInfo->typeClass == TypeClass::Delegate) || (flags & ResolvedTypeFlags::IsMethodGroup) != 0;
        }

        static ResolvedType Var() {
            ResolvedType retn;
            retn.flags |= ResolvedTypeFlags::IsVar;
            return retn;
        }

        static ResolvedType Null() {
            ResolvedType retn;
            retn.flags |= ResolvedTypeFlags::IsNullOrDefault;
            return retn;
        }

        static ResolvedType MethodGroup() {
            ResolvedType retn;
            retn.flags |= ResolvedTypeFlags::IsMethodGroup;
            return retn;
        }

        ResolvedType MakeNullable() {
            ResolvedType retn = *this;
            if (IsArray()) {
                retn.flags |= ResolvedTypeFlags::IsNullableArray;
            }
            else {
                retn.flags |= ResolvedTypeFlags::IsNullable;
            }
            return retn;
        }

        ResolvedType ToNonNullable() {
            ResolvedType retn = *this;
            if (IsArray()) {
                retn.flags &= ~ResolvedTypeFlags::IsNullableArray;
            }
            else {
                retn.flags &= ~ResolvedTypeFlags::IsNullable;
            }
            return retn;
        }

        bool IsNullOrDefault() const {
            return (flags & ResolvedTypeFlags::IsNullOrDefault) != 0;
        }

        bool IsBool() {
            return flags == ResolvedTypeFlags::None && builtInTypeName == BuiltInTypeName::Bool;
        }

        bool IsInteger() {

            if (flags != ResolvedTypeFlags::None) {
                return false;
            }

            switch (builtInTypeName) {

                case BuiltInTypeName::Int8:
                case BuiltInTypeName::Int16:
                case BuiltInTypeName::Int32:
                case BuiltInTypeName::Int64:

                case BuiltInTypeName::UInt8:
                case BuiltInTypeName::UInt16:
                case BuiltInTypeName::UInt32:
                case BuiltInTypeName::UInt64:

                    return true;

                default:
                    return false;
            }
        }

        bool IsArithmetic() {

            if (flags != ResolvedTypeFlags::None) {
                return false;
            }

            switch (builtInTypeName) {

                case BuiltInTypeName::Int8:
                case BuiltInTypeName::Int16:
                case BuiltInTypeName::Int32:
                case BuiltInTypeName::Int64:

                case BuiltInTypeName::UInt8:
                case BuiltInTypeName::UInt16:
                case BuiltInTypeName::UInt32:
                case BuiltInTypeName::UInt64:

                case BuiltInTypeName::Double:
                case BuiltInTypeName::Single:
                    return true;

                default:
                    return false;

            }

        }

        bool IsUnsignedInteger() {

            if (flags != ResolvedTypeFlags::None) {
                return false;
            }

            switch (builtInTypeName) {
                case BuiltInTypeName::UInt8:
                case BuiltInTypeName::UInt16:
                case BuiltInTypeName::UInt32:
                case BuiltInTypeName::UInt64:
                    return true;
                default:
                    return false;
            }
        }

        bool IsFloatingPoint() {

            if (flags != ResolvedTypeFlags::None) {
                return false;
            }

            return builtInTypeName == BuiltInTypeName::Float || builtInTypeName == BuiltInTypeName::Double;
        }


        bool IsVector() {
            if (flags != ResolvedTypeFlags::None) {
                return false;
            }
            switch (builtInTypeName) {

                case BuiltInTypeName::Int2:
                case BuiltInTypeName::Int3:
                case BuiltInTypeName::Int4:

                case BuiltInTypeName::Uint2:
                case BuiltInTypeName::Uint3:
                case BuiltInTypeName::Uint4:

                case BuiltInTypeName::Float2:
                case BuiltInTypeName::Float3:
                case BuiltInTypeName::Float4:

                case BuiltInTypeName::Color:

                    return true;
                default:
                    return false;
            }
        }

        bool IsVar() const {
            return (flags & ResolvedTypeFlags::IsVar) != 0;
        }

        bool IsEnum() {
            return (flags & ResolvedTypeFlags::IsEnum) != 0;
        }

        bool IsResolved() {
            return typeInfo != nullptr || (flags & ResolvedTypeFlags::IsVoid) != 0;
        }

        bool IsVoid() const {
            return (flags & ResolvedTypeFlags::IsVoid) != 0;
        }

        bool IsNullable() const {
            return (flags & ResolvedTypeFlags::IsNullable) != 0;
        }

        bool IsNullableArray() const {
            return (flags & ResolvedTypeFlags::IsNullableArray) != 0;
        }

        bool IsArray() const {
            return arrayRank > 0;
        }

        FixedCharSpan GetDeclaringFilePath() {
            return GetDeclaringFilePathImpl(typeInfo);
        }

        FixedCharSpan ToString(LinearAllocator* allocator) {
            int32 cnt = ToStringCount();
            char* buffer = allocator->AllocateUncleared<char>(cnt + 1);
            ToString(buffer);
            buffer[cnt] = 0;
            return FixedCharSpan(buffer, cnt);
        }

        const char* ToCString(LinearAllocator* allocator) {
            int32 cnt = ToStringCount();
            char* buffer = allocator->AllocateUncleared<char>(cnt + 1);
            ToString(buffer);
            buffer[cnt] = 0;
            return buffer;
        }

        int32 ToString(char* buffer) {

            char* p = buffer;

            if (typeInfo == nullptr) {
                assert(builtInTypeName != BuiltInTypeName::Invalid);
                const char* name = BuiltInTypeNameToString(builtInTypeName);
                memcpy(p, name, strlen(name));
                p += strlen(name);
            }
            else {

                FixedCharSpan fullyQualified = typeInfo->GetFullyQualifiedTypeName();

                memcpy(p, fullyQualified.ptr, fullyQualified.size);
                p += fullyQualified.size;

                if (typeInfo->genericArgumentCount > 0) {
                    *p++ = '<';
                    for (int32 i = 0; i < typeInfo->genericArgumentCount; i++) {
                        typeInfo->genericArguments[i].ToString(p);
                        if (i != typeInfo->genericArgumentCount - 1) {
                            *p++ = ',';
                        }
                    }
                    *p++ = '>';
                }
            }

            if ((flags & ResolvedTypeFlags::IsNullable) != 0) {
                *p++ = '?';
            }

            if (IsArray()) {
                *p++ = '[';
                *p++ = ']';
            }

            if ((flags & ResolvedTypeFlags::IsNullableArray) != 0) {
                *p++ = '?';
            }

            return (int32) (p - buffer);
        }

        int32 ToStringCount() {

            int32 cnt;

            if (typeInfo == nullptr) {
                assert(builtInTypeName != BuiltInTypeName::Invalid);
                const char* name = BuiltInTypeNameToString(builtInTypeName);
                cnt = strlen(name);
            }
            else {

                FixedCharSpan fullyQualified = typeInfo->GetFullyQualifiedTypeName();

                cnt = fullyQualified.size;

                if (typeInfo->genericArgumentCount > 0) {
                    cnt++;
                    for (int32 i = 0; i < typeInfo->genericArgumentCount; i++) {
                        cnt += typeInfo->genericArguments[i].ToStringCount();
                        if (i != typeInfo->genericArgumentCount - 1) {
                            cnt++;
                        }
                    }
                    cnt++;
                }
            }

            if ((flags & ResolvedTypeFlags::IsNullable) != 0) {
                cnt++;
            }

            if (IsArray()) {
                cnt += 2;
            }

            if ((flags & ResolvedTypeFlags::IsNullableArray) != 0) {
                cnt++;
            }

            return cnt;
        }

        bool EqualsRef(const ResolvedType& pType) {
            return typeInfo == pType.typeInfo && flags == pType.flags && arrayRank == pType.arrayRank && builtInTypeName == pType.builtInTypeName;
        }

        bool Equals(ResolvedType other) {
            return typeInfo == other.typeInfo && flags == other.flags && arrayRank == other.arrayRank && builtInTypeName == other.builtInTypeName;
        }

        bool Equals(ResolvedType* pType) {
            return typeInfo == pType->typeInfo && flags == pType->flags && arrayRank == pType->arrayRank && builtInTypeName == pType->builtInTypeName;
        }

        bool IsReferenceType() {
            switch (builtInTypeName) {
                case BuiltInTypeName::Dynamic:
                case BuiltInTypeName::String:
                case BuiltInTypeName::Object:
                    return true;
                default: {
                    return IsArray() || TypeInfoIsReferenceType(typeInfo);
                }
            }
        }

        IndexerInfo* GetIndexerInfo(CheckedArray<ResolvedType> indexerTypes) {
            return nullptr;
        }

        bool IsAssignableFrom(ResolvedType type) {

            if (IsVar() || Equals(&type)) {
                return true;
            }

            if (IsVoid() || type.IsVoid()) {
                return false;
            }

            if (type.IsArray() != IsArray()) {
                return false;
            }

            if (IsNullable() && !type.IsNullable()) {
                // we can assign a value to a nullable
                return TypeInfoIsAssignableFrom(typeInfo, type.typeInfo);
            }

            return TypeInfoIsAssignableFrom(typeInfo, type.typeInfo);

        }

        bool IsIndexable() const {
            return IsArray() || TypeInfoIsIndexable(typeInfo);
        }

        bool IsValueType() {
            return !IsArray() && typeInfo->typeClass == TypeClass::Enum || typeInfo->typeClass == TypeClass::Struct;
        }

        bool IsInterface() {
            return typeInfo->typeClass == TypeClass::Interface;
        }

        bool IsOrContainsGeneric() {
            return  false;
        }
    };

    struct GenericReplacement {
        FixedCharSpan genericName;
        ResolvedType resolvedGeneric;
    };

}