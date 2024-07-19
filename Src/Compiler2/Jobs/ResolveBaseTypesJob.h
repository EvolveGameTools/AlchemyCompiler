#pragma once

#include "../../JobSystem/Job.h"
#include "../FullyQualifiedName.h"
#include "../ResolvedType.h"
#include "../TypeResolver.h"
#include "../FieldInfo.h"

namespace Alchemy::Compilation {

    struct ResolveMemberTypesJob : Jobs::IJob {

        TypeResolutionMap* resolutionMap;
        CheckedArray<SourceFileInfo*> files;

        ResolveMemberTypesJob(CheckedArray<SourceFileInfo*> files, TypeResolutionMap* resolutionMap)
            : files(files)
            , resolutionMap(resolutionMap) {}

        // do we need members before base types?
        // generics will clone field infos etc
        // so maybe we do these are part of gather type info, otherwise there's nothing to copy when instantiating

        void Execute(int32 index) override {

            SourceFileInfo* file = files.Get(index);
            TypeResolver typeResolver(file, resolutionMap);

            TypeInfo ** tempTypeInfos = GetThreadLocalAllocator()->AllocateUncleared<TypeInfo*>(64);
            FixedPodList<TypeInfo*> genericArgumentStack(tempTypeInfos, 64);

            for(int32 typeIndex = 0; typeIndex < file->declaredTypes.size; typeIndex++) {

                TypeInfo * typeInfo = file->declaredTypes[typeIndex];

                genericArgumentStack.size = 0;
                typeResolver.inputGenericArguments = genericArgumentStack.ToCheckedArray();

                switch (typeInfo->typeClass) {
                    case TypeClass::Class: {
                        ClassDeclarationSyntax* classDeclarationSyntax = (ClassDeclarationSyntax*) typeInfo->syntaxNode;
                        SyntaxList<MemberDeclarationSyntax> * members = classDeclarationSyntax->members;

                        int32 genStackSize = genericArgumentStack.size;
                        for (int32 a = 0; a < typeInfo->genericArgumentCount; a++) {
                            genericArgumentStack.Add(typeInfo->genericArguments[a].typeInfo);
                        }
                        typeResolver.inputGenericArguments = genericArgumentStack.ToCheckedArray();

                        int32 fieldIndex = 0;
                        for(int32 m = 0; m < members->size; m++) {
                            MemberDeclarationSyntax * member = members->array[m];
                            switch(member->GetKind()) {
                                case SyntaxKind::FieldDeclaration: {

                                    FieldDeclarationSyntax * fieldDeclarationSyntax = (FieldDeclarationSyntax*)member;

                                    //HandleFieldModifiers(fieldDeclarationSyntax->modifiers);

                                    ResolvedType fieldType;
                                    if(typeResolver.TryResolveType(fieldDeclarationSyntax->declaration->type, &fieldType)) {
                                        fieldType = typeResolver.Unresolved();
                                    }

                                    VariableDeclaratorSyntax ** variables = fieldDeclarationSyntax->declaration->variables->items;

                                    for(int32 f = 0; f < fieldDeclarationSyntax->declaration->variables->itemCount; f++) {
                                        FieldInfo * fieldInfo = &typeInfo->fields[fieldIndex++];
                                        fieldInfo->type = fieldType;
                                        fieldInfo->declaringType = typeInfo;
                                        fieldInfo->identifier = file->GetText(variables[f]->identifier);
                                        fieldInfo->syntaxNode = variables[f];
                                        // variables[f]->initializer;
                                    }

                                    break;
                                }
                                case SyntaxKind::PropertyDeclaration: {
                                    break;
                                }
                                case SyntaxKind::MethodDeclaration: {
                                    break;
                                }
                                case SyntaxKind::IndexerDeclaration: {
                                    break;
                                }
                                case SyntaxKind::ConstructorDeclaration: {
                                    break;
                                }
                                default:{
                                    NOT_IMPLEMENTED(SyntaxKindToString(member->GetKind()));
                                    break;
                                }
                            }
                        }

                        break;
                    }
                    case TypeClass::Struct:
                        break;
                    case TypeClass::Interface:
                        break;
                    case TypeClass::Enum:
                        break;
                    case TypeClass::Delegate:
                        break;
                    case TypeClass::Widget:
                        break;
                    case TypeClass::GenericArgument:
                        break;
                    case TypeClass::Unresolved:
                        break;
                    case TypeClass::Void:
                        break;
                }

            }

        }

    };

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

                if (!typeResolver->TryResolveType(item->type, resolved)) {
                    *resolved = typeResolver->Unresolved();
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

        FixedCharSpan MakeCycleError(CheckedArray<FixedCharSpan> path, Allocator allocator) {
            size_t s = 0;
            for (int32 x = 0; x < path.size; x++) {
                s += path[x].size;
                if (x != path.size - 1) {
                    s += 4;
                }

            }
            s++;
            char* c = allocator.AllocateUncleared<char>(s);
            char* b = c;
            for (int32 x = 0; x < path.size; x++) {
                memcpy(b, path[x].ptr, path[x].size);
                b += path[x].size;
                if (x != path.size - 1) {
                    b[0] = ' ';
                    b[1] = '-';
                    b[2] = '>';
                    b[3] = ' ';
                    b += 4;
                }

            }
            b[0] = '\0';
            return FixedCharSpan(c, b - c);
        }

        void ValidateBaseList(TypeInfo* typeInfo, CheckedArray<TypeInfo*> visitList, FixedPodList<FixedCharSpan> path, BaseListSyntax* baseListSyntax) {

            if (typeInfo->baseTypeCount == 0) {
                return;
            }

            BaseTypeSyntax** baseList = baseListSyntax->types->items;

            bool isStruct = typeInfo->typeClass == TypeClass::Struct;

            if (!isStruct && typeInfo->baseTypes[0].IsClass() && typeInfo->DetectClassCycle(visitList, &path)) {
                FixedCharSpan error = MakeCycleError(path.ToCheckedArray(), typeInfo->declaringFile->allocator.MakeAllocator());
                FixedCharSpan sourceRange = baseListSyntax->types->items[0]->GetText(typeInfo->declaringFile->tokenizerResult);
                Diagnostic diagnostic(ErrorCode::ERR_CycleDetectedInClassHierarchy, sourceRange, error);
                typeInfo->declaringFile->diagnostics.AddError(diagnostic);
            }


            // check for duplicates
            for (int32 b = 0; b < typeInfo->baseTypeCount; b++) {

                ResolvedType t = typeInfo->baseTypes[b];

                if (t.IsUnresolved()) {
                    continue;
                }

                for (int32 c = b + 1; c < typeInfo->baseTypeCount; c++) {
                    ResolvedType a = typeInfo->baseTypes[c];
                    if (a.typeInfo == t.typeInfo) {
                        FixedCharSpan sourceRange = baseListSyntax->types->items[b]->GetText(typeInfo->declaringFile->tokenizerResult);
                        Diagnostic diagnostic(ErrorCode::ERR_BaseTypeAppearsMultipleTimes, sourceRange);
                        typeInfo->declaringFile->diagnostics.AddError(diagnostic);
                    }
                }
            }

            for (int32 b = 0; b < typeInfo->baseTypeCount; b++) {
                ResolvedType resolvedType = typeInfo->baseTypes[b];

                if (!resolvedType.IsUnresolved()) {
                    continue;
                }

                visitList.size = 0;
                FixedCharSpan span = baseList[b]->GetText(typeInfo->declaringFile->tokenizerResult);

                if (resolvedType.typeInfo == typeInfo) {
                    typeInfo->declaringFile->diagnostics.AddError(Diagnostic(ErrorCode::ERR_CannotInheritRecursively, span));
                    continue;
                }

                if (resolvedType.IsClass() && resolvedType.IsSealed()) {
                    typeInfo->declaringFile->diagnostics.AddError(Diagnostic(ErrorCode::ERR_CannotInheritASealedType, span));
                    continue;
                }

                if (isStruct && b == 0 && !resolvedType.IsInterface()) {
                    typeInfo->declaringFile->diagnostics.AddError(Diagnostic(ErrorCode::ERR_StructsCanOnlyInheritInterfaces, span));
                    continue;
                }

                if (!isStruct && resolvedType.IsClass() && b != 0) {
                    typeInfo->declaringFile->diagnostics.AddError(Diagnostic(ErrorCode::ERR_OnlyFirstBaseTypeCanBeClass, span));
                    continue;
                }

                if (b != 0 && !resolvedType.IsInterface()) {
                    typeInfo->declaringFile->diagnostics.AddError(Diagnostic(ErrorCode::ERR_BaseTypeInNonZeroPositionMustBeAnInterface, span));
                    continue;
                }

                if (resolvedType.IsRef()) {
                    typeInfo->declaringFile->diagnostics.AddError(Diagnostic(ErrorCode::ERR_BaseTypeCannotBeRef, span));
                    continue;
                }

                if (resolvedType.IsNullable()) {
                    typeInfo->declaringFile->diagnostics.AddError(Diagnostic(ErrorCode::ERR_BaseTypeCannotBeNullable, span));
                    continue;
                }

                if (resolvedType.IsTuple()) {
                    typeInfo->declaringFile->diagnostics.AddError(Diagnostic(ErrorCode::ERR_BaseTypeCannotBeTuple, span));
                    continue;
                }

            }

        }

        void Execute(int32 idx) override {

            SourceFileInfo* file = files.Get(idx);

            TypeResolver typeResolver(file, resolutionMap);

            FixedPodList<TypeInfo*> inheritStack(GetThreadLocalAllocator()->Allocate<TypeInfo*>(64), 64);
            FixedPodList<FixedCharSpan> cyclePath(GetThreadLocalAllocator()->Allocate<FixedCharSpan>(64), 64);

            for (int32 i = 0; i < file->declaredTypes.size; i++) {

                TypeInfo* typeInfo = file->declaredTypes[i];

                TempAllocator::ScopedMarker m(GetThreadLocalAllocator());

                switch (typeInfo->typeClass) {
                    case TypeClass::Class: {

                        ClassDeclarationSyntax* classDeclarationSyntax = (ClassDeclarationSyntax*) typeInfo->syntaxNode;
                        TypeParameterListSyntax* typeParameterList = classDeclarationSyntax->typeParameterList;
                        ValidateGenericArgs(typeParameterList, file, &typeResolver);
                        HandleBaseList(typeInfo, classDeclarationSyntax->baseList, &typeResolver);

                        ValidateBaseList(typeInfo, inheritStack.ToCheckedArray(), cyclePath, classDeclarationSyntax->baseList);

                        break;
                    }
                    case TypeClass::Struct: {
                        StructDeclarationSyntax* structDeclarationSyntax = (StructDeclarationSyntax*) typeInfo->syntaxNode;
                        TypeParameterListSyntax* typeParameterList = structDeclarationSyntax->typeParameterList;
                        ValidateGenericArgs(typeParameterList, file, &typeResolver);
                        HandleBaseList(typeInfo, structDeclarationSyntax->baseList, &typeResolver);

                        ValidateBaseList(typeInfo, inheritStack.ToCheckedArray(), cyclePath, structDeclarationSyntax->baseList);

                        break;
                    }

                    case TypeClass::GenericArgument: {
                        continue; // I think this is a no-op
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
                    case TypeClass::Void:
                    case TypeClass::Unresolved:
                        UNREACHABLE("ResolveBaseTypesJob");
                        break;
                }
            }

        }

    };

}