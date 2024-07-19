#include "./GatherTypeInfoJob.h"
#include "../TypeInfo.h"
#include "../FullyQualifiedName.h"
#include "../FieldInfo.h"

namespace Alchemy::Compilation {

    FixedCharSpan ColonColonNamePathToSpan(SeparatedSyntaxList<IdentifierNameSyntax>* names, SourceFileInfo* fileInfo) {
        IdentifierNameSyntax** items = names->items;
        int32 itemCount = names->itemCount;
        size_t size = 0;

        for (int32 n = 0; n < itemCount; n++) {
            SyntaxToken token = items[n]->identifier;
            FixedCharSpan name = token.GetText(fileInfo->tokenizerResult.texts);
            size += name.size;
        }

        int32 separatorCount = itemCount - 1;
        char* c = fileInfo->allocator.AllocateUncleared<char>(size + (2 * separatorCount) + 1);
        char* ptr = c;

        for (int32 n = 0; n < itemCount; n++) {
            SyntaxToken token = items[n]->identifier;
            FixedCharSpan name = token.GetText(fileInfo->tokenizerResult.texts);
            memcpy(ptr, name.ptr, name.size);
            ptr += name.size;
            if (n != itemCount - 1) {
                *ptr++ = ':';
                *ptr++ = ':';
            }
            size += name.size;
        }

        *ptr = '\0';

        return FixedCharSpan(c, size);

    }

    GatherTypeInfoJob::GatherTypeInfoJob(CheckedArray<SourceFileInfo*> files)
        : files(files) {}

    void GatherTypeInfoJob::Execute(int32 idx) {
        fileInfo = files[idx];

        CompilationUnitSyntax* syntaxTree = fileInfo->syntaxTree;

        SyntaxList<MemberDeclarationSyntax>* members = syntaxTree->members;

        int32 usingCount = 0;
        int32 declarationCount = 0;

        for (int32 i = 0; i < members->size; i++) {
            MemberDeclarationSyntax* member = members->array[i];

            switch (member->GetKind()) {
                case SyntaxKind::NamespaceDeclaration: {

                    if (usingCount != 0 || declarationCount != 0) {
                        FixedCharSpan span = member->GetText(fileInfo->tokenizerResult);
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_NamespaceMustComeBeforeUsingsAndDeclarations, span));
                        break;
                    }

                    if (fileInfo->namespaceName.ptr != nullptr) {
                        FixedCharSpan span = member->GetText(fileInfo->tokenizerResult);
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MulitpleNamespaces, span));
                        break;
                    }

                    NamespaceDeclarationSyntax* namespaceDeclarationSyntax = (NamespaceDeclarationSyntax*) member;

                    fileInfo->namespaceName = ColonColonNamePathToSpan(namespaceDeclarationSyntax->names, fileInfo);

                    usingCount++;

                    break;
                }
                case SyntaxKind::StructDeclaration: {
                    StructDeclarationSyntax* structDeclarationSyntax = (StructDeclarationSyntax*) member;
                    if (structDeclarationSyntax->typeParameterList != nullptr) {
                        declarationCount += structDeclarationSyntax->typeParameterList->parameters->itemCount;
                    }
                    declarationCount++;
                }
                case SyntaxKind::ClassDeclaration: {
                    ClassDeclarationSyntax* classDeclarationSyntax = (ClassDeclarationSyntax*) member;
                    if (classDeclarationSyntax->typeParameterList != nullptr) {
                        declarationCount += classDeclarationSyntax->typeParameterList->parameters->itemCount;
                    }
                    declarationCount++;
                    break;
                }
                case SyntaxKind::EnumDeclaration:
                case SyntaxKind::DelegateDeclaration:
                case SyntaxKind::InterfaceDeclaration:
                case SyntaxKind::WidgetDeclaration: {
                    declarationCount++;
                    break;
                }
                case SyntaxKind::UsingDeclaration: {
                    if (declarationCount != 0) {
                        FixedCharSpan span = member->GetText(fileInfo->tokenizerResult);
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_UsingsMustComeBeforeDeclarations, span));
                    }
                    usingCount++;
                    break;
                }
                case SyntaxKind::IncompleteMember: {
                    break;
                }
                default: {
                    NOT_IMPLEMENTED(SyntaxKindToString(member->GetKind()));
                    break;
                }
            }
        }

        CheckedArray<FixedCharSpan> usingDeclarations(fileInfo->allocator.AllocateUncleared<FixedCharSpan>(usingCount), usingCount);
        CheckedArray<TypeInfo*> typeDeclarations(fileInfo->allocator.Allocate<TypeInfo*>(declarationCount), declarationCount);

        usingCount = 0; // namespace is the first using
        declarationCount = 0;

        if(fileInfo->namespaceName.size > 0) {
            usingCount = 1;
            usingDeclarations[0] = fileInfo->namespaceName;
        }

        for (int32 i = 0; i < members->size; i++) {
            MemberDeclarationSyntax* member = members->array[i];

            switch (member->GetKind()) {

                case SyntaxKind::UsingNamespaceDeclaration: {

                    UsingNamespaceDeclarationSyntax* usingDeclarationSyntax = (UsingNamespaceDeclarationSyntax*) member;

                    usingDeclarations[usingCount++] = ColonColonNamePathToSpan(usingDeclarationSyntax->namePath, fileInfo);

                    for (int32 u = 0; u < usingCount - 1; u++) {
                        if (usingDeclarations[u] == usingDeclarations[usingCount - 1]) {
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_DuplicateUsingDirective, usingDeclarationSyntax->GetText(fileInfo->tokenizerResult)));
                            usingCount--;
                            break;
                        }
                    }

                    break;
                }
                case SyntaxKind::NamespaceDeclaration: {
                    break; // already handled
                }
                case SyntaxKind::ExternDeclaration: {
                    break;
                }
                case SyntaxKind::StructDeclaration:
                case SyntaxKind::ClassDeclaration:
                case SyntaxKind::EnumDeclaration:
                case SyntaxKind::DelegateDeclaration:
                case SyntaxKind::InterfaceDeclaration:
                case SyntaxKind::WidgetDeclaration: {
                    CreateTypeInfo(typeDeclarations, &declarationCount, member);
                    break;
                }
                case SyntaxKind::IncompleteMember: {
                    break;
                }
                default: {
                    NOT_IMPLEMENTED(SyntaxKindToString(member->GetKind()));
                    break;
                }
            }
        }

        fileInfo->usingDirectives = usingDeclarations;
        fileInfo->declaredTypes = typeDeclarations;
    }

    void GatherTypeInfoJob::CreateTypeInfo(CheckedArray<TypeInfo*> typeInfos, int32* typeInfoIndex, MemberDeclarationSyntax* pSyntax) {
        switch (pSyntax->GetKind()) {
            case SyntaxKind::StructDeclaration: {
                CreateStructDeclaration(typeInfos, typeInfoIndex, (StructDeclarationSyntax*) pSyntax);
                break;
            }
            case SyntaxKind::ClassDeclaration: {
                CreateClassDeclaration(typeInfos, typeInfoIndex, (ClassDeclarationSyntax*) pSyntax);
                break;
            }
            case SyntaxKind::EnumDeclaration: {
                break;
            }
            case SyntaxKind::DelegateDeclaration: {
                break;
            }
            case SyntaxKind::InterfaceDeclaration: {
                break;
            }
            case SyntaxKind::ExternDeclaration: {
                break;
            }
            case SyntaxKind::WidgetDeclaration: {
                break;
            }
            default: {
                NOT_IMPLEMENTED(SyntaxKindToString(pSyntax->GetKind()));
                break;
            }
        }
    }

    void GatherTypeInfoJob::HandleModifiers(TypeInfo* pTypeInfo, TokenList* modifiers) {

        if (modifiers == nullptr) {
            return;
        }

        bool hasVisibility = false;

        for (int32 i = 0; i < modifiers->size; i++) {
            SyntaxToken token = modifiers->array[i];

            switch (token.kind) {
                case TokenKind::PublicKeyword: {
                    if (hasVisibility) {
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MulitpleVisibiltyDeclarations, token.GetText(fileInfo->tokenizerResult.texts)));
                        break;
                    }
                    hasVisibility = true;
                    pTypeInfo->visibility = TypeVisibility::Public;
                    break;
                }
                case TokenKind::PrivateKeyword: {
                    if (hasVisibility) {
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MulitpleVisibiltyDeclarations, token.GetText(fileInfo->tokenizerResult.texts)));
                        break;
                    }
                    pTypeInfo->visibility = TypeVisibility::Private;
                    hasVisibility = true;
                    break;
                }
                case TokenKind::ExportKeyword: {
                    if (hasVisibility) {
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MulitpleVisibiltyDeclarations, token.GetText(fileInfo->tokenizerResult.texts)));
                        break;
                    }
                    pTypeInfo->visibility = TypeVisibility::Export;
                    hasVisibility = true;
                    break;
                }
                case TokenKind::InternalKeyword: {
                    if (hasVisibility) {
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_MulitpleVisibiltyDeclarations, token.GetText(fileInfo->tokenizerResult.texts)));
                        break;
                    }
                    pTypeInfo->visibility = TypeVisibility::Internal;
                    hasVisibility = true;
                    break;
                }
                case TokenKind::ProtectedKeyword: {
                    fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_TypeCannotBeProtected, token.GetText(fileInfo->tokenizerResult.texts)));
                    break;
                }
                case TokenKind::RefKeyword: {
                    fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_TypeCannotBeRef, token.GetText(fileInfo->tokenizerResult.texts)));
                    break;
                }
                case TokenKind::SealedKeyword: {
                    if ((pTypeInfo->flags & TypeInfoFlags::Abstract) != 0) {
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_SealedOrAbstract, token.GetText(fileInfo->tokenizerResult.texts)));
                    }
                    pTypeInfo->flags |= TypeInfoFlags::Sealed;
                    break;
                }
                case TokenKind::AbstractKeyword: {
                    if ((pTypeInfo->flags & TypeInfoFlags::Sealed) != 0) {
                        fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_SealedOrAbstract, token.GetText(fileInfo->tokenizerResult.texts)));
                    }
                    pTypeInfo->flags |= TypeInfoFlags::Abstract;
                    break;
                }
                default: {
                    NOT_IMPLEMENTED(TokenKindToString(token.kind));
                    break;
                }
            }
        }
    }

    void GatherTypeInfoJob::HandleMembers(TypeInfo* pTypeInfo, SyntaxList<MemberDeclarationSyntax>* members) {
        if (members == nullptr) {
            return;
        }

        int32 fieldCount = 0;
        int32 propertyCount = 0;
        int32 methodCount = 0;
        int32 indexerCount = 0;
        int32 constructorCount = 0;

        for (int32 i = 0; i < members->size; i++) {

            MemberDeclarationSyntax* member = members->array[i];

            switch (member->GetKind()) {
                case SyntaxKind::FieldDeclaration: {
                    fieldCount++;
                    break;
                }
                case SyntaxKind::PropertyDeclaration: {
                    propertyCount++;
                    break;
                }
                case SyntaxKind::MethodDeclaration: {
                    methodCount++;

                    // when we see a method with optional parameters we want to make a new method info for them w/ concrete values
                    // so we don't need to bother with resolving defaults later

                    MethodDeclarationSyntax* method = (MethodDeclarationSyntax*) member;
                    ParameterListSyntax* parameterListSyntax = method->parameterList;
                    SeparatedSyntaxList<ParameterSyntax>* parameterList = parameterListSyntax->parameters;

                    int32 firstDefault = -1;
                    for (int32 p = 0; p < parameterList->itemCount; p++) {
                        ParameterSyntax* parameter = parameterList->items[p];

                        if (firstDefault == -1 && parameter->defaultValue != nullptr) {
                            firstDefault = p;
                        }

                        if (firstDefault != -1 && parameter->defaultValue != nullptr) {
                            FixedCharSpan text = parameter->identifier.GetText(fileInfo->tokenizerResult.texts);
                            fileInfo->diagnostics.AddError(Diagnostic(ErrorCode::ERR_OptionalParameterOrder, text));
                        }

                    }

                    if (firstDefault != -1) {
                        methodCount += (parameterList->itemCount - firstDefault + 1);
                    }

                    break;
                }
                case SyntaxKind::ConstructorDeclaration: {
                    constructorCount++;
                    break;
                }
                case SyntaxKind::IndexerDeclaration: {
                    indexerCount++;
                    break;
                }
                default: {
                    NOT_IMPLEMENTED(SyntaxKindToString(member->GetKind()));
                    break;
                }
            }

        }

        pTypeInfo->methods = fileInfo->allocator.Allocate<MethodInfo>(methodCount);
        pTypeInfo->methodCount = methodCount;
        pTypeInfo->fields = fileInfo->allocator.Allocate<FieldInfo>(fieldCount);
        pTypeInfo->fieldCount = fieldCount;
        pTypeInfo->properties = fileInfo->allocator.Allocate<PropertyInfo>(propertyCount);
        pTypeInfo->propertyCount = propertyCount;
        pTypeInfo->indexers = fileInfo->allocator.Allocate<IndexerInfo>(indexerCount);
        pTypeInfo->indexerCount = indexerCount;
        pTypeInfo->constructors = fileInfo->allocator.Allocate<ConstructorInfo>(constructorCount);
        pTypeInfo->constructorCount = constructorCount;

    }

    void GatherTypeInfoJob::HandleAttributes(TypeInfo* pTypeInfo, SyntaxList<AttributeListSyntax>* attributes) {

    }

    void GatherTypeInfoJob::CreateStructDeclaration(CheckedArray<TypeInfo*> typeInfos, int32* typeInfoIndex, StructDeclarationSyntax* pSyntax) {

        TypeInfo* pInfo = fileInfo->allocator.Allocate<TypeInfo>(1);
        typeInfos[*typeInfoIndex] = pInfo;

        *typeInfoIndex = *typeInfoIndex + 1;


        FixedCharSpan typeName = pSyntax->identifier.GetText(fileInfo->tokenizerResult.texts);

        int32 genericCount = 0;

        if (pSyntax->typeParameterList != nullptr && pSyntax->typeParameterList->parameters != nullptr) {
            genericCount = pSyntax->typeParameterList->parameters->itemCount;
            pInfo->flags |= TypeInfoFlags::IsGenericTypeDefinition;
        }

        FixedCharSpan namespaceName = fileInfo->namespaceName;
        if(namespaceName.size == 0) {
            namespaceName = FixedCharSpan("global");
        }
        FixedCharSpan fqn = MakeFullyQualifiedName(namespaceName, typeName, genericCount, fileInfo->allocator.MakeAllocator());
        pInfo->fullyQualifiedName = fqn.ptr;
        pInfo->fullyQualifiedNameLength = fqn.size;
        pInfo->declaringFile = fileInfo;
        pInfo->typeName = fqn.ptr + fileInfo->namespaceName.size; // substring from the fqn
        pInfo->typeNameLength = typeName.size;
        pInfo->typeClass = TypeClass::Struct;
        pInfo->syntaxNode = pSyntax;

        HandleModifiers(pInfo, pSyntax->modifiers);

        MakeGenericArgumentTypes(pInfo, typeInfos, typeInfoIndex, pSyntax->typeParameterList);

        // I don't think I can do much with these yet, we'll figure these out later on
        // pSyntax->constraintClauses;
        // pSyntax->parameterList; // for primary constructors

        HandleBaseTypes(pInfo, pSyntax->baseList);

        HandleMembers(pInfo, pSyntax->members);

    }

    void GatherTypeInfoJob::CreateClassDeclaration(CheckedArray<TypeInfo*> typeInfos, int32* typeInfoIndex, ClassDeclarationSyntax* pSyntax) {

        FixedCharSpan typeName = pSyntax->identifier.GetText(fileInfo->tokenizerResult.texts);

        TypeInfo* pInfo = fileInfo->allocator.Allocate<TypeInfo>(1);
        typeInfos[*typeInfoIndex] = pInfo;
        *typeInfoIndex = *typeInfoIndex + 1;

        int32 genericCount = 0;

        if (pSyntax->typeParameterList != nullptr && pSyntax->typeParameterList->parameters != nullptr) {
            genericCount = pSyntax->typeParameterList->parameters->itemCount;
            pInfo->flags |= TypeInfoFlags::IsGenericTypeDefinition;
        }

        FixedCharSpan namespaceName = fileInfo->namespaceName;
        if(namespaceName.size == 0) {
            namespaceName = FixedCharSpan("global");
        }

        FixedCharSpan fqn = MakeFullyQualifiedName(namespaceName, typeName, genericCount, fileInfo->allocator.MakeAllocator());
        pInfo->fullyQualifiedName = fqn.ptr;
        pInfo->fullyQualifiedNameLength = fqn.size;
        pInfo->typeClass = TypeClass::Class;
        pInfo->syntaxNode = pSyntax;
        pInfo->typeName = fqn.ptr + fileInfo->namespaceName.size; // substring from the fqn
        pInfo->typeNameLength = typeName.size;
        pInfo->declaringFile = fileInfo;
        HandleModifiers(pInfo, pSyntax->modifiers);

        MakeGenericArgumentTypes(pInfo, typeInfos, typeInfoIndex, pSyntax->typeParameterList);

        // I don't think I can do much with these yet, we'll figure these out later on
        // pSyntax->constraintClauses;
        // pSyntax->parameterList; // for primary constructors

        HandleBaseTypes(pInfo, pSyntax->baseList);

        HandleMembers(pInfo, pSyntax->members);

    }

    void GatherTypeInfoJob::HandleBaseTypes(TypeInfo* pInfo, BaseListSyntax* pSyntax) {
        if (pSyntax == nullptr || pSyntax->types == nullptr || pSyntax->types->itemCount == 0) {
            pInfo->baseTypeCount = 0;
            pInfo->baseTypes = nullptr;
        }
        else {
            pInfo->baseTypeCount = pSyntax->types->itemCount;
            pInfo->baseTypes = pInfo->declaringFile->allocator.Allocate<ResolvedType>(pSyntax->types->itemCount);
        }
    }

    void GatherTypeInfoJob::MakeGenericArgumentTypes(TypeInfo* pInfo, CheckedArray<TypeInfo*> typeInfos, int32* typeInfoIndex, TypeParameterListSyntax* typeParameterList) {

        if (typeParameterList == nullptr || typeParameterList->parameters == nullptr) {
            return;
        }

        for (int32 i = 0; i < typeParameterList->parameters->itemCount; i++) {
            TypeParameterSyntax* typeParameterSyntax = typeParameterList->parameters->items[i];
            FixedCharSpan identifier = typeParameterSyntax->identifier.GetText(pInfo->declaringFile->tokenizerResult.texts);
            FixedCharSpan argName = MakeFullyQualifiedGenericArgName(pInfo->GetFullyQualifiedTypeName(), identifier, i, fileInfo->allocator.MakeAllocator());

            TypeInfo* genericArg = fileInfo->allocator.Allocate<TypeInfo>(1);
            typeInfos[*typeInfoIndex] = genericArg;
            *typeInfoIndex = *typeInfoIndex + 1;

            genericArg->fullyQualifiedName = argName.ptr;
            genericArg->fullyQualifiedNameLength = argName.size;
            genericArg->flags |= TypeInfoFlags::IsGenericArgumentDefinition;
            genericArg->syntaxNode = typeParameterSyntax;
            genericArg->declaringFile = fileInfo;
            genericArg->typeClass = TypeClass::GenericArgument;
            genericArg->typeName = identifier.ptr;
            genericArg->typeNameLength = identifier.size;
            genericArg->visibility = pInfo->visibility;

        }

    }

}