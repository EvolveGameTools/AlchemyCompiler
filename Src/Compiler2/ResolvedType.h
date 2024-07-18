#pragma once

#include "../PrimitiveTypes.h"
#include "./BuiltInTypeName.h"

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
        IsUnresolved = 1 << 9
    })

    struct ResolvedType {

        TypeInfo * typeInfo;
        ResolvedTypeFlags resolvedTypeFlags;
        BuiltInTypeName builtInTypeName;

        ResolvedType()
            : typeInfo(nullptr)
            , resolvedTypeFlags(ResolvedTypeFlags::IsUnresolved)
            , builtInTypeName(BuiltInTypeName::Invalid) {}

        explicit  ResolvedType(TypeInfo * typeInfo, ResolvedTypeFlags flags = ResolvedTypeFlags::None)
            : typeInfo(typeInfo)
            , resolvedTypeFlags(flags)
            , builtInTypeName(BuiltInTypeName::Invalid)
        {}

        inline bool IsVoid() {
            return ((resolvedTypeFlags & ResolvedTypeFlags::IsVoid) != 0);
        }

        inline bool IsUnresolved() {
            return ((resolvedTypeFlags & ResolvedTypeFlags::IsUnresolved) != 0) || typeInfo->IsUnresolved();
        }

        inline bool IsUnresolvedGeneric() {
            return typeInfo != nullptr && typeInfo->IsGenericTypeDefinition();
        }

    };

}