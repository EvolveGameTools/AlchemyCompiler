#pragma once

#include "../../JobSystem/Job.h"
#include "../../JobSystem/JobSystem.h"
#include "../FullyQualifiedName.h"
#include "../ResolvedType.h"
#include "../TypeResolver.h"
#include "../MemberInfo.h"
#include "../../PrimitiveTypes.h"
#include "../../Allocation/ThreadLocalTemp.h"

namespace Alchemy::Compilation {

    struct ResolveMemberTypesJob : Jobs::IJob {

        TypeResolutionMap* resolutionMap;
        CheckedArray<SourceFileInfo*> files;

        ResolveMemberTypesJob(CheckedArray<SourceFileInfo*> files, TypeResolutionMap* resolutionMap)
            : files(files)
            , resolutionMap(resolutionMap) {}

        void HandleFieldModifiers(SourceFileInfo* fileInfo, TokenList* pList, MemberVisibility* pVisibility, FieldModifiers* pModifiers) {

            *pVisibility = MemberVisibility::Public;
            *pModifiers = FieldModifiers::None;

            int32 visCount = 0;
            int32 readonlyCount = 0;
            int32 staticCount = 0;
            int32 constCount = 0;
            for (int32 i = 0; i < pList->size; i++) {
                SyntaxToken token = pList->array[i];
                switch (token.kind) {
                    case TokenKind::PublicKeyword:
                    case TokenKind::ProtectedKeyword:
                    case TokenKind::InternalKeyword:
                    case TokenKind::PrivateKeyword: {
                        if (visCount == 0) {
                            if (token.kind == TokenKind::PublicKeyword) {
                                *pVisibility = MemberVisibility::Public;
                            }
                            else if (token.kind == TokenKind::ProtectedKeyword) {
                                *pVisibility = MemberVisibility::Protected;
                            }
                            else if (token.kind == TokenKind::InternalKeyword) {
                                *pVisibility = MemberVisibility::Internal;
                            }
                            else if (token.kind == TokenKind::PrivateKeyword) {
                                *pVisibility = MemberVisibility::Private;
                            }
                        }
                        else if (visCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleVisibilityModifiers, fileInfo->GetText(token)));
                        }
                        visCount++;
                        break;
                    }
                    case TokenKind::ReadOnlyKeyword: {
                        if (readonlyCount == 0) {
                            *pModifiers |= FieldModifiers::Readonly;
                            if ((*pModifiers & FieldModifiers::Const) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_ReadOnlyIsExclusiveWithConst, fileInfo->GetText(token)));
                            }
                        }
                        else if (readonlyCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleModifiers, fileInfo->GetText(token)));
                        }
                        readonlyCount++;
                    }
                    case TokenKind::StaticKeyword: {
                        if (staticCount == 0) {
                            *pModifiers |= FieldModifiers::Static;

                            if ((*pModifiers & FieldModifiers::Const) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_StaticIsExclusiveWithConst, fileInfo->GetText(token)));
                            }

                        }
                        else if (staticCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleModifiers, fileInfo->GetText(token)));
                        }
                        staticCount++;
                    }
                    case TokenKind::ConstKeyword: {
                        if (constCount == 0) {
                            *pModifiers |= FieldModifiers::Const;
                            if ((*pModifiers & FieldModifiers::Static) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_StaticIsExclusiveWithConst, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & FieldModifiers::Readonly) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_ReadOnlyIsExclusiveWithConst, fileInfo->GetText(token)));
                            }
                        }
                        else if (constCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleModifiers, fileInfo->GetText(token)));
                        }
                        constCount++;
                    }
                    default: {
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_InvalidModifierForFieldDeclaration, fileInfo->GetText(token)));
                        break;
                    }
                }
            }
        }

        void HandleMethodModifiers(SourceFileInfo* fileInfo, TokenList* pList, MemberVisibility* pVisibility, MethodModifiers* pModifiers) {
            *pVisibility = MemberVisibility::Public;
            *pModifiers = MethodModifiers::None;

            int32 visCount = 0;
            int32 overrideCount = 0;
            int32 staticCount = 0;
            int32 abstractCount = 0;
            int32 virtualCount = 0;
            int32 sealedCount = 0;
            for (int32 i = 0; i < pList->size; i++) {
                SyntaxToken token = pList->array[i];
                switch (token.kind) {
                    case TokenKind::PublicKeyword:
                    case TokenKind::ProtectedKeyword:
                    case TokenKind::InternalKeyword:
                    case TokenKind::PrivateKeyword: {
                        if (visCount == 0) {
                            if (token.kind == TokenKind::PublicKeyword) {
                                *pVisibility = MemberVisibility::Public;
                            }
                            else if (token.kind == TokenKind::ProtectedKeyword) {
                                *pVisibility = MemberVisibility::Protected;
                            }
                            else if (token.kind == TokenKind::InternalKeyword) {
                                *pVisibility = MemberVisibility::Internal;
                            }
                            else if (token.kind == TokenKind::PrivateKeyword) {
                                *pVisibility = MemberVisibility::Private;
                            }
                        }
                        else if (visCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleVisibilityModifiers, fileInfo->GetText(token)));
                        }
                        visCount++;
                        break;
                    }
                    case TokenKind::OverrideKeyword: {
                        if (overrideCount == 0) {
                            *pModifiers |= MethodModifiers::Override;
                            if ((*pModifiers & MethodModifiers::Sealed) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_OverrideIsExclusiveWithSealed, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Virtual) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_OverrideIsExclusiveWithVirtual, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Abstract) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_OverrideIsExclusiveWithAbstract, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Static) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_OverrideIsExclusiveWithStatic, fileInfo->GetText(token)));
                            }
                        }
                        else if (overrideCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleModifiers, fileInfo->GetText(token)));
                        }
                        overrideCount++;
                        break;
                    }
                    case TokenKind::SealedKeyword: {
                        if (sealedCount == 0) {
                            *pModifiers |= MethodModifiers::Sealed;
                            if ((*pModifiers & MethodModifiers::Static) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_SealedIsExclusiveWithStatic, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Virtual) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_SealedIsExclusiveWithVirtual, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Abstract) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_SealedIsExclusiveWithAbstract, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Override) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_SealedIsExclusiveWithOverride, fileInfo->GetText(token)));
                            }
                        }
                        else if (sealedCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleModifiers, fileInfo->GetText(token)));
                        }
                        sealedCount++;
                        break;

                    }
                    case TokenKind::AbstractKeyword: {
                        if (abstractCount == 0) {
                            *pModifiers |= MethodModifiers::Abstract;
                            if ((*pModifiers & MethodModifiers::Sealed) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_AbstractIsExclusiveWithSealed, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Virtual) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_AbstractIsExclusiveWithVirtual, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Override) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_AbstractIsExclusiveWithOverride, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Static) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_AbstractIsExclusiveWithStatic, fileInfo->GetText(token)));
                            }
                        }
                        else if (abstractCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleModifiers, fileInfo->GetText(token)));
                        }
                        abstractCount++;
                        break;
                    }
                    case TokenKind::VirtualKeyword: {
                        if (virtualCount == 0) {
                            *pModifiers |= MethodModifiers::Virtual;
                            if ((*pModifiers & MethodModifiers::Sealed) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_VirtualIsExclusiveWithSealed, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Abstract) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_VirtualIsExclusiveWithAbstract, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Override) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_VirtualIsExclusiveWithOverride, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Static) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_VirtualIsExclusiveWithStatic, fileInfo->GetText(token)));
                            }
                        }
                        else if (virtualCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleModifiers, fileInfo->GetText(token)));
                        }
                        virtualCount++;
                        break;
                    }
                    case TokenKind::StaticKeyword: {
                        if (staticCount == 0) {
                            *pModifiers |= MethodModifiers::Static;
                            if ((*pModifiers & MethodModifiers::Sealed) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_StaticIsExclusiveWithSealed, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Virtual) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_StaticIsExclusiveWithVirtual, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Abstract) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_StaticIsExclusiveWithAbstract, fileInfo->GetText(token)));
                            }
                            if ((*pModifiers & MethodModifiers::Override) != 0) {
                                fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_StaticIsExclusiveWithOverride, fileInfo->GetText(token)));
                            }
                        }
                        else if (staticCount == 1) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MultipleModifiers, fileInfo->GetText(token)));
                        }
                        staticCount++;
                        break;
                    }

                    default: {
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_InvalidModifierForMethodDeclaration, fileInfo->GetText(token)));
                        break;
                    }
                }
            }
        }

        ParameterModifiers HandleParameterModifiers(SourceFileInfo* file, ParameterSyntax* pSyntax) {

            if(pSyntax->modifiers == nullptr) {
                return ParameterModifiers::None;
            }

            ParameterModifiers modifiers = ParameterModifiers::None;

            int32 readonlyCount = 0;
            int32 refCount = 0;
            int32 outCount = 0;

            for(int32 i = 0; i < pSyntax->modifiers->size; i++) {
                SyntaxToken token = pSyntax->modifiers->array[i];
                switch(token.kind) {
                    case TokenKind::ReadOnlyKeyword: {
                        if(refCount == 0 && outCount == 0) {
                            modifiers |= ParameterModifiers::Readonly;
                        }
                        else {
                            file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_ReadOnlyIsExclusiveWithOutAndRef, file->GetText(token)));
                        }
                        readonlyCount++;
                        break;
                    }
                    case TokenKind::RefKeyword: {
                        if(readonlyCount == 0 && outCount == 0) {
                            modifiers |= ParameterModifiers::Ref;
                        }
                        else {
                            file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_RefIsExclusiveWithOutAndReadonly, file->GetText(token)));
                        }
                        refCount++;
                        break;
                    }
                    case TokenKind::OutKeyword: {
                        if(readonlyCount == 0 && refCount == 0) {
                            modifiers |= ParameterModifiers::Out;
                        }
                        else {
                            file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_OutIsExclusiveWithRefAndReadonly, file->GetText(token)));
                        }
                        outCount++;
                        break;
                    }
                    default: {
                        file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_InvalidModifierForParameterDeclaration, file->GetText(token)));
                        break;
                    }
                }
            }

            return ParameterModifiers::None;
        }

        void Execute(int32 index) override {

            SourceFileInfo* file = files.Get(index);
            TypeResolver typeResolver(file, resolutionMap);

            TypeInfo** tempTypeInfos = GetThreadLocalAllocator()->AllocateUncleared<TypeInfo*>(64);
            FixedPodList<TypeInfo*> genericArgumentStack(tempTypeInfos, 64);

            // todo -- we need to verify in another pass that we don't have duplicate members, missing interface/abstract, etc
            // needs to be another pass because we don't know what order this is happening in right now and we actually
            // don't even have base type resolved at this point in the compilation process

            for (int32 typeIndex = 0; typeIndex < file->declaredTypes.size; typeIndex++) {

                TypeInfo* typeInfo = file->declaredTypes[typeIndex];

                genericArgumentStack.size = 0;
                typeResolver.inputGenericArguments = genericArgumentStack.ToCheckedArray();

                switch (typeInfo->typeClass) {
                    case TypeClass::Struct:
                    case TypeClass::Class: {
                        SyntaxList<MemberDeclarationSyntax>* members;

                        if (typeInfo->typeClass == TypeClass::Class) {
                            ClassDeclarationSyntax* classDeclarationSyntax = (ClassDeclarationSyntax*) typeInfo->syntaxNode;
                            members = classDeclarationSyntax->members;
                        }
                        else {
                            StructDeclarationSyntax* structDeclarationSyntax = (StructDeclarationSyntax*) typeInfo->syntaxNode;
                            members = structDeclarationSyntax->members;
                        }

                        int32 genStackSize = genericArgumentStack.size;
                        for (int32 a = 0; a < typeInfo->genericArgumentCount; a++) {
                            genericArgumentStack.Add(typeInfo->genericArguments[a].typeInfo);
                        }

                        typeResolver.inputGenericArguments = genericArgumentStack.ToCheckedArray();

                        int32 fieldIndex = 0;
                        int32 methodIndex = 0;

                        for (int32 m = 0; m < members->size; m++) {
                            MemberDeclarationSyntax* member = members->array[m];
                            switch (member->GetKind()) {
                                case SyntaxKind::FieldDeclaration: {

                                    FieldDeclarationSyntax* fieldDeclarationSyntax = (FieldDeclarationSyntax*) member;

                                    MemberVisibility visibility;
                                    FieldModifiers modifiers;

                                    HandleFieldModifiers(typeInfo->declaringFile, fieldDeclarationSyntax->modifiers, &visibility, &modifiers);

                                    ResolvedType fieldType;
                                    if (!typeResolver.TryResolveType(fieldDeclarationSyntax->declaration->type, &fieldType)) {
                                        fieldType = typeResolver.Unresolved();
                                        file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_UnresolvedType, file->GetText(fieldDeclarationSyntax->declaration->type)));
                                    }

                                    VariableDeclaratorSyntax** variables = fieldDeclarationSyntax->declaration->variables->items;

                                    for (int32 f = 0; f < fieldDeclarationSyntax->declaration->variables->itemCount; f++) {
                                        FieldInfo* fieldInfo = &typeInfo->fields[fieldIndex++];
                                        fieldInfo->type = fieldType;
                                        fieldInfo->declaringType = typeInfo;
                                        fieldInfo->identifier = file->GetText(variables[f]->identifier);
                                        fieldInfo->syntaxNode = variables[f];
                                        fieldInfo->modifiers = modifiers;
                                        fieldInfo->visibility = visibility;
                                    }

                                    break;
                                }
                                case SyntaxKind::PropertyDeclaration: {
                                    break;
                                }
                                case SyntaxKind::MethodDeclaration: {
                                    MethodDeclarationSyntax* methodDeclarationSyntax = (MethodDeclarationSyntax*) member;

                                    FixedCharSpan methodName = file->GetText(methodDeclarationSyntax->identifier);

                                    MemberVisibility visibility;
                                    MethodModifiers modifiers;
                                    HandleMethodModifiers(typeInfo->declaringFile, methodDeclarationSyntax->modifiers, &visibility, &modifiers);
                                    int32 genStackSizeStart = genericArgumentStack.size;

                                    // todo -- handle method templates
                                    if (methodDeclarationSyntax->typeParameterList != nullptr) {

                                        // technically I think we need to create new types for these placeholders unfortunately
                                        // unless we can figure out how to treat them as placeholders / temporary
                                        // argtypeName = type.fqn->methodName$genCount_argName[argIndex]
                                        for (int32 t = 0; t < methodDeclarationSyntax->typeParameterList->parameters->itemCount; t++) {
                                            TypeParameterSyntax* genericArg = methodDeclarationSyntax->typeParameterList->parameters->items[t];
                                            FixedCharSpan argName = file->GetText(genericArg->identifier);
                                            // genericArgumentStack.Add(typeInfo->genericArguments[a].typeInfo);
                                        }
                                    }

                                    ResolvedType returnType;
                                    if (!typeResolver.TryResolveType(methodDeclarationSyntax->returnType, &returnType)) {
                                        returnType = typeResolver.Unresolved();
                                        file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_UnresolvedType, file->GetText(methodDeclarationSyntax->returnType)));
                                    }

                                    SeparatedSyntaxList<ParameterSyntax>* parameterList = methodDeclarationSyntax->parameterList->parameters;

                                    int32 firstDefault = -1;
                                    int32 methodCount = 1;

                                    for (int32 p = 0; p < parameterList->itemCount; p++) {
                                        ParameterSyntax* parameter = parameterList->items[p];

                                        if (parameter->defaultValue != nullptr) {
                                            firstDefault = p;
                                            break;
                                        }

                                        // we've already warned about optional parameter ordering, we can just assume everything is ok for now
                                        // since the file has an error we won't actually try to codegen it and can safely ignore the default values for introspection purposes.

                                    }

                                    if (firstDefault != -1) {
                                        methodCount += (parameterList->itemCount - firstDefault);
                                    }

                                    // we only need one loop for parameters
                                    // subsequent methods will just point at the original method's nodes

                                    ParameterInfo* parameterInfos = file->allocator.AllocateUncleared<ParameterInfo>(parameterList->itemCount);

                                    for (int32 p = 0; p < parameterList->itemCount; p++) {

                                        ParameterSyntax* parameter = parameterList->items[p];

                                        ParameterModifiers parameterModifiers = HandleParameterModifiers(file, parameter);

                                        ResolvedType parameterType;
                                        if (!typeResolver.TryResolveType(parameter->type, &parameterType)) {
                                            returnType = typeResolver.Unresolved();
                                            file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_UnresolvedType, file->GetText(parameter->type)));
                                        }

                                        parameterInfos[p].type = parameterType;
                                        parameterInfos[p].name = file->GetText(parameter->identifier);
                                        parameterInfos[p].modifiers = parameterModifiers;
                                        parameterInfos[p].syntaxNode = parameter;

                                        for(int32 p1 = p - 1; p1 >= 0; p1--) {
                                            if(parameterInfos[p1].name == parameterInfos[p].name) {
                                                file->diagnostics.AddError(Diagnostic(ErrorCode::ERR_DuplicateParameterName, parameterInfos[p].name));
                                                break;
                                            }
                                        }

                                    }

                                    for (int32 midx = 0; midx < methodCount; midx++) {
                                        MethodInfo* methodInfo = &typeInfo->methods[methodIndex + midx];
                                        methodInfo->parameters = parameterInfos;
                                        // each subsequent method has 1 less than the base amount
                                        methodInfo->parameterCount = parameterList->itemCount - midx;
                                        methodInfo->isDefaultParameterOverload = midx != 0;
                                        methodInfo->syntaxNode = methodDeclarationSyntax;
                                        methodInfo->returnType = returnType;
                                        methodInfo->declaringType = typeInfo;
                                        methodInfo->name = methodName;
                                        // methodInfo->fullyQualifiedName = FixedCharSpan("TODO -- DO WE NEED THIS?");
                                        methodInfo->modifiers = modifiers;
                                        methodInfo->visibility = visibility;
                                    }

                                    methodIndex += methodCount;

                                    if (methodDeclarationSyntax->typeParameterList != nullptr) {
                                        genericArgumentStack.size -= methodDeclarationSyntax->typeParameterList->parameters->itemCount;
                                    }

                                    break;
                                }
                                case SyntaxKind::IndexerDeclaration: {
                                    break;
                                }
                                case SyntaxKind::ConstructorDeclaration: {
                                    break;
                                }
                                default: {
                                    NOT_IMPLEMENTED(SyntaxKindToString(member->GetKind()));
                                    break;
                                }
                            }
                        }

                        break;
                    }
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
}