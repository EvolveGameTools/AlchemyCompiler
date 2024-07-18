#pragma once

#include "./SourceFileInfo.h"
#include "./BuiltInTypeName.h"
#include "./TypeResolutionMap.h"

namespace Alchemy::Compilation {

    struct GenericNameSyntax;
    struct TypeSyntax;
    struct IdentifierNameSyntax;

    struct TypeResolver {

        SourceFileInfo* file;
        CheckedArray<TypeInfo*> inputGenericArguments;
        TypeResolutionMap* resolutionMap;
        bool supressDiagnostics;

        TypeResolver(SourceFileInfo* file, TypeResolutionMap* resolutionMap);

        bool TryResolveGenericName(GenericNameSyntax* genericNameSyntax, ResolvedType* resolvedType);

        bool TryResolveIdentifierName(FixedCharSpan identifierName, ResolvedType* resolvedType);

        bool TryResolveType(TypeSyntax* typeSyntax, ResolvedType* resolvedType);

        bool TryResolveType(FixedCharSpan identifierName, ResolvedType* resolvedType);

    };

}