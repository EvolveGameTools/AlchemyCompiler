#include "./TypeResolver.h"
#include "../Allocation/ThreadLocalTemp.h"
#include "../Parsing3/SyntaxNodes.h"
#include "./FullyQualifiedName.h"

namespace Alchemy::Compilation {

    TypeResolver::TypeResolver(SourceFileInfo* file, TypeResolutionMap* resolutionMap)
        : file(file)
        , resolutionMap(resolutionMap)
        , supressDiagnostics(false)
        , inputGenericArguments() {}

    bool TypeResolver::TryResolveGenericName(GenericNameSyntax* genericNameSyntax, ResolvedType* resolvedType) {
        TempAllocator* tempAllocator = GetThreadLocalAllocator();
        TempAllocator::ScopedMarker m(tempAllocator);
        char* buffer = tempAllocator->AllocateUncleared<char>(resolutionMap->GetLongestEntrySize() + 32);

        FixedCharSpan name = genericNameSyntax->identifier.GetText(file->tokenizerResult.texts);
        int32 genericCount = genericNameSyntax->typeArgumentList->arguments->itemCount;

        TypeInfo* value = nullptr;
        bool found = false;

        if(name == "Array" && genericCount == 1 && resolutionMap->TryResolve(MakeFullyQualifiedName(FixedCharSpan("BuiltIn"), name, genericCount, buffer), &value)) {
            found = true;
        }

        // we need to look in all the usings in case of an ambiguous match
        // the file's namespace is the first entry in usingDirectives
        for (int32 u = 0; u < file->usingDirectives.size; u++) {

            if (resolutionMap->TryResolve(MakeFullyQualifiedName(file->usingDirectives[u], name, genericCount, buffer), &value)) {
                if (found && !supressDiagnostics) {
                    file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_AmbiguousTypeMatch, name));
                }
                found = true;
            }

        }

        if(!found) {
            FixedCharSpan gbl("global");
            FixedCharSpan x = MakeFullyQualifiedName(gbl, name, genericCount, buffer);
            resolutionMap->TryResolve(x, &value);
        }

        if (value == nullptr) {
            // unresolved, we could maybe try some other generic counts as a test (1 more or 1 less)
            if (!supressDiagnostics) {
                file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_UnresolvedType, name));
            }
            return false;
        }
        else {

            // found it. lets see if it's arguments are concrete or not
            ResolvedType* generics = tempAllocator->AllocateUncleared<ResolvedType>(genericCount);

            for (int32 g = 0; g < genericCount; g++) {
                TypeSyntax* typeSyntax = genericNameSyntax->typeArgumentList->arguments->items[g];

                ResolvedType r;

                if (TryResolveType(typeSyntax, &r)) {
                    generics[g] = r;
                }
                else {
                    // what do we return for a partial resolution? probably just treat it as entirely unresolved w/ some diagnostic messages
                    return false;
                }
            }

            *resolvedType = resolutionMap->MakeGenericType(value, CheckedArray<ResolvedType>(generics, genericCount), file->GetLockedAllocator());

            return true;

        }

    }

    bool TypeResolver::TryResolveIdentifierName(FixedCharSpan identifierName, ResolvedType* resolvedType) {

        TempAllocator* tempAllocator = GetThreadLocalAllocator();
        TempAllocator::ScopedMarker m(tempAllocator);

        char* buffer = tempAllocator->AllocateUncleared<char>(resolutionMap->GetLongestEntrySize() + 32);

        TypeInfo* value = nullptr;
        bool found = false;

        // check against our input generics, maybe we have a match. We've already ensured the generic args type names
        // do not conflict with any other type names that are in scope.
        for (int32 g = 0; g < inputGenericArguments.size; g++) {
            // safe since we know this is a generic arg name
            TypeParameterSyntax * typeSyntax = (TypeParameterSyntax*)inputGenericArguments[g]->syntaxNode;
            FixedCharSpan id = file->GetText(typeSyntax->identifier);
            if (identifierName == id) {
                *resolvedType = ResolvedType(inputGenericArguments[g]);
                return true;
            }
        }

        // we need to look in all the usings in case of an ambiguous match
        // the file's namespace is the first entry in usingDirectives

        for (int32 u = 0; u < file->usingDirectives.size; u++) {

            if (resolutionMap->TryResolve(MakeFullyQualifiedName(file->namespaceName, identifierName, 0, buffer), &value)) {
                if (found && !supressDiagnostics) {
                    file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_AmbiguousTypeMatch, identifierName));
                }
                found = true;
            }

        }

        if(!found) {
            resolutionMap->TryResolve(MakeFullyQualifiedName(FixedCharSpan("global"), identifierName, 0, buffer), &value);
        }

        if (value == nullptr) {
            // unresolved
            if (!supressDiagnostics) {
                file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_UnresolvedType, identifierName));
            }
            return false;
        }
        else {


            *resolvedType = ResolvedType(value);
            return true;
        }
    }

    bool TypeResolver::TryResolveType(TypeSyntax* typeSyntax, ResolvedType* resolvedType) {

        if (typeSyntax == nullptr) {
            return false;
        }

        switch (typeSyntax->GetKind()) {
            case SyntaxKind::TupleType: {
                NOT_IMPLEMENTED("ResolveTupleType");
                return false;
            }
            case SyntaxKind::RefType: {
                RefTypeSyntax* refTypeSyntax = (RefTypeSyntax*) typeSyntax;
                ResolvedType r;
                if (TryResolveType(refTypeSyntax->type, &r)) {
                    *resolvedType = ResolvedType(r.typeInfo, ResolvedTypeFlags::IsRef | r.resolvedTypeFlags);
                    return true;
                }
                return false;
            }
            case SyntaxKind::NullableType: {
                ResolvedType r;
                NullableTypeSyntax* pNullableTypeSyntax = (NullableTypeSyntax*) typeSyntax;

                if (TryResolveType(pNullableTypeSyntax->elementType, &r)) {
                    *resolvedType = ResolvedType(r.typeInfo, ResolvedTypeFlags::IsNullable | r.resolvedTypeFlags);
                    return true;
                }

                return false;
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* predefinedTypeSyntax = (PredefinedTypeSyntax*) typeSyntax;
                BuiltInTypeName typeName = BuiltInTypeNameFromKeyword(predefinedTypeSyntax->typeToken.kind);
                assert(typeName != BuiltInTypeName::Invalid);
                *resolvedType = ResolvedType(resolutionMap->builtInTypeInfos[(int32) typeName]);
                return true;
            }

            case SyntaxKind::QualifiedName: {
                NOT_IMPLEMENTED("ResolveQualifiedName");
                return false;
            }

            case SyntaxKind::IdentifierName: {
                ResolvedType r;
                IdentifierNameSyntax* identifierNameSyntax = (IdentifierNameSyntax*) typeSyntax;
                FixedCharSpan name = identifierNameSyntax->identifier.GetText(file->tokenizerResult.texts);

                if (TryResolveIdentifierName(name, &r)) {
                    *resolvedType = r;
                    return true;
                }

                return false;
            }

            case SyntaxKind::GenericName: {
                ResolvedType r;
                if (TryResolveGenericName((GenericNameSyntax*) typeSyntax, &r)) {
                    *resolvedType = r;
                    return true;
                }
                return false;
            }

            default: {
                NOT_IMPLEMENTED(SyntaxKindToString(typeSyntax->GetKind()));
                return false;
            }
        }

    }

    bool TypeResolver::TryResolveType(FixedCharSpan identifierName, ResolvedType* resolvedType) {
        ResolvedType r;
        if (TryResolveIdentifierName(identifierName, &r)) {
            *resolvedType = r;
            return true;
        }
        return false;
    }

    ResolvedType TypeResolver::Unresolved() {
        return ResolvedType(resolutionMap->unresolvedType);
    }


}