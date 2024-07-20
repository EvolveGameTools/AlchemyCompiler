#pragma once

#include "../PrimitiveTypes.h"
#include "./BuiltInTypeName.h"
#include "./TypeInfo.h"

namespace Alchemy::Compilation {

    DEFINE_ENUM_FLAGS(ResolvedTypeFlags, uint16, {
        None = 0,
        IsVoid = 1 << 0,
        IsNullable = 1 << 1,
        IsArray = 1 << 2,
        IsEnum = 1 << 3,
        IsVector = 1 << 4,
        IsNullOrDefault = 1 << 5,
        IsMethodGroup = 1 << 6,
        IsVar = 1 << 7,
        IsRef = 1 << 8,
        IsTuple = 1 << 9,
        IsUnresolved = 1 << 10,
    })

    struct ResolvedType {

        TypeInfo* typeInfo;
        ResolvedTypeFlags resolvedTypeFlags;

        ResolvedType()
            : typeInfo(nullptr)
            , resolvedTypeFlags(ResolvedTypeFlags::IsUnresolved) {}

        explicit ResolvedType(TypeInfo* typeInfo, ResolvedTypeFlags flags = ResolvedTypeFlags::None)
            : typeInfo(typeInfo)
            , resolvedTypeFlags(flags) {}

        inline bool IsVoid() {
            return ((resolvedTypeFlags & ResolvedTypeFlags::IsVoid) != 0);
        }

        inline bool IsUnresolved() {
            return ((resolvedTypeFlags & ResolvedTypeFlags::IsUnresolved) != 0) || (typeInfo != nullptr && typeInfo->IsUnresolved());
        }

        inline bool IsUnresolvedGeneric() {
            return typeInfo != nullptr && typeInfo->IsGenericTypeDefinition();
        }

        bool operator ==(ResolvedType other) {
            return other.typeInfo == typeInfo && other.resolvedTypeFlags == resolvedTypeFlags;
        }

        bool operator !=(ResolvedType other) {
            return other.typeInfo != typeInfo || other.resolvedTypeFlags != resolvedTypeFlags;
        }

        inline bool IsClass() {

            if (typeInfo != nullptr) {
                return typeInfo->typeClass == TypeClass::Class;
            }

            return false;

        }

        inline bool IsSealed() {

            if (IsUnresolved()) {
                return false;
            }

            if (typeInfo != nullptr) {
                return (typeInfo->flags & TypeInfoFlags::Sealed) != 0;
            }

            return false;

        }

        inline bool IsInterface() {
            return typeInfo != nullptr && typeInfo->typeClass == TypeClass::Interface;
        }

        inline bool IsNullable() {
            return (resolvedTypeFlags & ResolvedTypeFlags::IsNullable) != 0;
        }

        inline bool IsTuple() {
            return (resolvedTypeFlags & ResolvedTypeFlags::IsTuple) != 0;
        }

        inline bool IsRef() {
            return (resolvedTypeFlags & ResolvedTypeFlags::IsRef) != 0;
        }
    };

}