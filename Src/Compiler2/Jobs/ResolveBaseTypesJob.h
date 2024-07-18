#pragma once

#include "../../JobSystem/Job.h"
#include "../FullyQualifiedName.h"
#include "../ResolvedType.h"
#include "../TypeResolver.h"

namespace Alchemy::Compilation {

    struct ResolveBaseTypesJob : Jobs::IJob {

        TypeResolutionMap* resolutionMap;
        CheckedArray<SourceFileInfo*> files;

        ResolveBaseTypesJob(CheckedArray<SourceFileInfo*> files, TypeResolutionMap* resolutionMap)
            : files(files)
            , resolutionMap(resolutionMap) {}

        void HandleBaseList(TypeInfo* pInfo, BaseListSyntax* baseList, TypeResolver* typeResolver) {

            if (baseList == nullptr || baseList->types == nullptr || baseList->types->itemCount == 0) {
                return;
            }

            if (pInfo->genericArgumentCount != 0) {

                TypeInfo** generics = GetThreadLocalAllocator()->Allocate<TypeInfo*>(pInfo->genericArgumentCount);

                for (int32 i = 0; i < pInfo->genericArgumentCount; i++) {
                    generics[i] = pInfo->genericArguments[i].typeInfo;
                }

                typeResolver->inputGenericArguments = CheckedArray<TypeInfo*>(generics, pInfo->genericArgumentCount);
            }

            for (int32 i = 0; i < baseList->types->itemCount; i++) {
                BaseTypeSyntax* item = baseList->types->items[i];

                ResolvedType* resolved = &pInfo->baseTypes[i];

                if (typeResolver->TryResolveType(item->type, resolved)) {

                    if (item->GetKind() == SyntaxKind::RefType || item->GetKind() == SyntaxKind::NullableType || item->GetKind() == SyntaxKind::TupleType) {
                        // bad
                    }

                }
                else {

                }

            }

            typeResolver->inputGenericArguments = CheckedArray<TypeInfo*>();

        }

        void ValidateGenericArgs(TypeParameterListSyntax* typeParameterList, SourceFileInfo* file, TypeResolver* typeResolver) {

            // make sure none of these are resolvable types, they are placeholder identifiers
            if (typeParameterList != nullptr && typeParameterList->parameters->itemCount != 0) {
                typeResolver->supressDiagnostics = true;

                for (int32 t = 0; t < typeParameterList->parameters->itemCount; t++) {

                    TypeParameterSyntax* parameter = typeParameterList->parameters->items[t];

                    if (parameter->identifier.IsMissing()) {
                        continue;
                    }

                    FixedCharSpan name = parameter->identifier.GetText(file->tokenizerResult.texts);
                    ResolvedType* resolvedType = nullptr;
                    if (typeResolver->TryResolveType(name, resolvedType)) {
                        file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_TypeArgumentShouldNotBeAnExistingTypeName, name));
                    }
                }

                typeResolver->supressDiagnostics = false;
            }
        }

        void Execute(int32 idx) override {

            SourceFileInfo* file = files.Get(idx);

            TypeResolver typeResolver(file, resolutionMap);

            for (int32 i = 0; i < file->declaredTypes.size; i++) {

                TypeInfo* typeInfo = file->declaredTypes[i];

                TempAllocator::ScopedMarker m(GetThreadLocalAllocator());

                switch (typeInfo->typeClass) {
                    case TypeClass::Class: {

                        ClassDeclarationSyntax* classDeclarationSyntax = (ClassDeclarationSyntax*) typeInfo->syntaxNode;
                        TypeParameterListSyntax* typeParameterList = classDeclarationSyntax->typeParameterList;
                        ValidateGenericArgs(typeParameterList, file, &typeResolver);
                        HandleBaseList(typeInfo, classDeclarationSyntax->baseList, &typeResolver);
                        break;
                    }
                    case TypeClass::Struct: {
                        StructDeclarationSyntax* structDeclarationSyntax = (StructDeclarationSyntax*) typeInfo->syntaxNode;
                        TypeParameterListSyntax* typeParameterList = structDeclarationSyntax->typeParameterList;
                        ValidateGenericArgs(typeParameterList, file, &typeResolver);
                        HandleBaseList(typeInfo, structDeclarationSyntax->baseList, &typeResolver);
                        break;
                    }
                    case TypeClass::Interface: {
                        NOT_IMPLEMENTED("Interface");
                        break;
                    }
                    case TypeClass::Enum: {
                        NOT_IMPLEMENTED("Enum");
                        break;
                    }
                    case TypeClass::Delegate: {
                        NOT_IMPLEMENTED("Delegate");
                        break;
                    }
                    case TypeClass::Widget: {
                        NOT_IMPLEMENTED("Widget");
                        break;
                    }
                    case TypeClass::GenericArgument:
                    case TypeClass::Void:
                    case TypeClass::Unresolved:
                        UNREACHABLE("ResolveBaseTypesJob");
                        break;
                }
            }

        }

    };

}