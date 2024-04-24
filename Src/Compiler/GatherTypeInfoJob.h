#pragma once

#include "./StringUtil.h"
#include "./TypeInfo.h"
#include "../Collections/MSIHashmap.h"
#include "./Diagnostics.h"
#include "./MemberInfo.h"
#include "../Collections/FixedPodList.h"

namespace Alchemy::Compilation {

    TypeContext* AllocateTypeContext(TypeInfo* typeInfo, LinearAllocator* contextAllocator);

    struct GatherTypeInfoJob : Alchemy::Jobs::IJob {

        struct GatherTypeInfoJobImpl {

            FileInfo* fileInfo;
            Alchemy::FixedPodList<NamespaceInfo*> namespaceStack;
            Alchemy::FixedPodList<DeclaredType> declaredTypesList;

            template<class T>
            T* GetNode(NodeIndex<T> nodeIndex) {
                if (nodeIndex.index == 0) {
                    return nullptr;
                }
                return (T*) fileInfo->parseResult.psiNodes.GetPointer(nodeIndex.index);
            }

            FixedCharSpan GetDeclarationName(NodeIndex<IdentifierNode> idNode) {
                if (!idNode.IsValid()) {
                    return FixedCharSpan(nullptr, 0);
                }
                return GetNode(idNode)->GetIdentifier();
            }

            DeclarationModifier GetModifiers(NodeIndex<ModifierListNode> modifiers) {
                if (!modifiers.IsValid()) {
                    return DeclarationModifier::None;
                }
                return GetNode(modifiers)->modifiers;
            }

            int32 CountTypeArguments(NodeIndex<TypeParameterListNode> typeArgumentListNodeIndex) {

                TypeParameterListNode* pNode = GetNode(typeArgumentListNodeIndex);
                if (pNode == nullptr || !pNode->first.IsValid()) {
                    return 0;
                }

                NodeIndex<IdentifierNode> ptr = pNode->first;

                int32 cnt = 0;

                while (ptr.IsValid()) {
                    cnt++;
                    IdentifierNode* p = GetNode(ptr);
                    if (p == nullptr) {
                        break;
                    }
                    ptr = p->next;
                }

                return cnt;
            }

            NamespaceInfo* MakeTypeBasedNamespace(TypeInfo* typeInfo, int32 typeArgumentCount) {
                FixedCharSpan typeName = typeInfo->GetTypeName();
                NamespaceInfo* info = fileInfo->allocator.Allocate<NamespaceInfo>(1);
                info->parent = namespaceStack.Peek();
                info->typeInfo = typeInfo;
                info->name = typeInfo->GetTypeName();

                if (typeArgumentCount > 0) {
                    char* pName = fileInfo->allocator.Allocate<char>(typeName.size + 1 + typeArgumentCount);
                    memcpy(pName, typeName.ptr, typeName.size);
                    pName[typeName.size] = '`';
                    int32 written = snprintf(pName + typeName.size + 1, typeArgumentCount, "%d", typeArgumentCount);
                    info->name = FixedCharSpan(pName, typeName.size + 1 + written);
                }

                return info;

            }

            FixedCharSpan MakeFullyQualifiedName(FixedCharSpan name, int32 genericCount) {

                FixedCharSpan namespaceName = namespaceStack.Peek()->fullyQualifiedName;

                int32 size = namespaceName.size + name.size + 2;

                if (genericCount > 0) {
                    size += 1 + IntToAsciiCount(genericCount);
                }

                char* buffer = fileInfo->allocator.Allocate<char>(size);
                char* ptr = buffer;

                memcpy(ptr, namespaceName.ptr, namespaceName.size);
                ptr += namespaceName.size;
                ptr[0] = ':';
                ptr[1] = ':';
                ptr += 2;
                memcpy(ptr, name.ptr, name.size);
                ptr += name.size;
                if (genericCount > 0) {
                    ptr[0] = '`';
                    ptr++;
                    IntToAscii(genericCount, ptr);
                }

                return FixedCharSpan(buffer, size);

            }

            static FixedCharSpan MakeFullyQualifiedGenericArgName(FixedCharSpan declaringTypeName, FixedCharSpan argumentName, LinearAllocator* allocator) {
                int32 size = declaringTypeName.size + 2 + argumentName.size;
                char* c = allocator->AllocateUncleared<char>(size);
                memcpy(c, declaringTypeName.ptr, declaringTypeName.size);
                c[declaringTypeName.size] = '`';
                memcpy(c + declaringTypeName.size + 1, argumentName.ptr, argumentName.size);
                c[size - 1] = '\0';
                return FixedCharSpan(c, size - 1);
            }

            void CreateGenericArguments(TypeInfo* typeInfo, TypeParameterListNode* typeParameterListNode) {

                if (typeParameterListNode == nullptr) {
                    return;
                }

                typeInfo->flags |= TypeInfoFlags::IsGenericTypeDefinition;
                IdentifierNode* pTypeParameter = GetNode(typeParameterListNode->first);
                int32 cnt = 0;
                while (pTypeParameter != nullptr) {
                    cnt++;
                    pTypeParameter = GetNode(pTypeParameter->next);
                }

                if (cnt <= 0) {
                    return; // this was a parse error
                }

                typeInfo->genericArguments = fileInfo->allocator.Allocate<ResolvedType>(cnt);
                typeInfo->genericArgumentCount = cnt;

                pTypeParameter = GetNode(typeParameterListNode->first);
                cnt = 0;

                while (pTypeParameter != nullptr) {
                    TypeInfo* argTypeInfo = fileInfo->allocator.AllocateUncleared<TypeInfo>(1);

                    TypeInfo::MakeGenericTypeInfo(
                        argTypeInfo,
                        pTypeParameter->GetIdentifier(),
                        fileInfo,
                        NodeIndex<IdentifierNode>(pTypeParameter->nodeIndex)
                    );

                    ResolvedType* argInfo = &typeInfo->genericArguments[cnt++];
                    argInfo->typeInfo = argTypeInfo;
                    pTypeParameter = GetNode(pTypeParameter->next);
                }

            }

            int32 CountDeclarations() {

                int32 cnt = 0;

                CheckedArray<AbstractPsiNode> nodes = fileInfo->parseResult.psiNodes;
                // 0 is invalid, 1 is the file, so 2 is our first real possible node
                for (int32 i = 2; i < nodes.size; i++) {
                    NodeType nodeType = nodes.array[i].nodeBase.nodeType;
                    if (
                        nodeType == NodeType::NamespaceDeclaration ||
                        nodeType == NodeType::DelegateDeclaration ||
                        nodeType == NodeType::EnumDeclaration ||
                        nodeType == NodeType::InterfaceDeclaration ||
                        nodeType == NodeType::StructDeclaration ||
                        nodeType == NodeType::ClassDeclaration) {
                        cnt++;
                    }
                }

                return cnt;

            }

            void GatherDeclarationsRecursive(NodeIndex<DeclarationNode> first, TypeInfo* parent) {

                if (!first.IsValid()) {
                    return;
                }

                NodeIndex<DeclarationNode> ptr = first;

                while (ptr.IsValid()) {

                    DeclarationNode* pDecl = (DeclarationNode*) fileInfo->parseResult.psiNodes.GetPointer(ptr.index);

                    // probably should assert that this a declaration node type

                    switch (pDecl->nodeType) {

                        case NodeType::NamespaceDeclaration: {

                            NamespaceDeclarationNode* namespaceNode = (NamespaceDeclarationNode*) pDecl;
                            NamespaceInfo* namespaceInfo = fileInfo->allocator.Allocate<NamespaceInfo>(1);
                            namespaceInfo->name = namespaceNode->GetName();
                            namespaceInfo->fullyQualifiedName = MakeFullyQualifiedName(namespaceNode->GetName(), 0);
                            namespaceInfo->parent = namespaceStack.Peek();
                            namespaceInfo->typeInfo = nullptr;

                            namespaceStack.Push(namespaceInfo);

                            GatherDeclarationsRecursive(namespaceNode->firstDeclaration, nullptr);

                            namespaceStack.Pop();

                            break;
                        }

                        case NodeType::DelegateDeclaration: {
                            DelegateDeclarationNode* delegateNode = (DelegateDeclarationNode*) pDecl;

                            TypeInfo* typeInfo = fileInfo->allocator.Allocate<TypeInfo>(1);

                            DeclaredType declaredType;

                            declaredType.typeInfo = typeInfo;
                            declaredType.typeContext = nullptr; // todo -- probably don't need this for a delegate

                            declaredTypesList.Add(declaredType);
                            int32 genericCount = CountTypeArguments(delegateNode->typeParameters);

                            FixedCharSpan name = GetDeclarationName(delegateNode->identifier);
                            FixedCharSpan fullyQualified = MakeFullyQualifiedName(name, genericCount);

                            typeInfo->declaringFile = fileInfo;
                            typeInfo->namespacePath = namespaceStack.Peek();
                            typeInfo->nodeIndex = ptr;
                            typeInfo->modifiers = GetModifiers(delegateNode->modifiers);
                            typeInfo->typeName = name.ptr;
                            typeInfo->typeNameLength = name.size;
                            typeInfo->fullyQualifiedName = fullyQualified.ptr;
                            typeInfo->fullyQualifiedNameLength = fullyQualified.size;
                            typeInfo->typeClass = TypeClass::Delegate;

                            break;
                        }

                        case NodeType::EnumDeclaration: {
                            EnumDeclarationNode* enumNode = (EnumDeclarationNode*) pDecl;

                            TypeInfo* typeInfo = fileInfo->allocator.Allocate<TypeInfo>(1);

                            DeclaredType declaredType;

                            declaredType.typeInfo = typeInfo;
                            declaredType.typeContext = nullptr;

                            declaredTypesList.Add(declaredType);

                            FixedCharSpan name = GetDeclarationName(enumNode->identifier);
                            FixedCharSpan fullyQualified = MakeFullyQualifiedName(name, 0);

                            typeInfo->declaringFile = fileInfo;
                            typeInfo->namespacePath = namespaceStack.Peek();
                            typeInfo->nodeIndex = ptr;
                            typeInfo->modifiers = GetModifiers(enumNode->modifiers);
                            typeInfo->typeName = name.ptr;
                            typeInfo->typeNameLength = name.size;
                            typeInfo->fullyQualifiedName = fullyQualified.ptr;
                            typeInfo->fullyQualifiedNameLength = fullyQualified.size;
                            typeInfo->typeClass = TypeClass::Enum;
                            break;
                        }

                        case NodeType::InterfaceDeclaration: {

                            InterfaceDeclarationNode* interfaceNode = (InterfaceDeclarationNode*) pDecl;

                            TypeInfo* typeInfo = fileInfo->allocator.Allocate<TypeInfo>(1);

                            DeclaredType declaredType;

                            declaredType.typeInfo = typeInfo;
                            declaredType.typeContext = nullptr;

                            declaredTypesList.Add(declaredType);

                            int32 genericCount = CountTypeArguments(interfaceNode->typeParameters);
                            FixedCharSpan name = GetDeclarationName(interfaceNode->identifier);
                            FixedCharSpan fullyQualified = MakeFullyQualifiedName(name, genericCount);

                            typeInfo->declaringFile = fileInfo;
                            typeInfo->namespacePath = namespaceStack.Peek();
                            typeInfo->nodeIndex = ptr;
                            typeInfo->modifiers = GetModifiers(interfaceNode->modifiers);
                            typeInfo->typeName = name.ptr;
                            typeInfo->typeNameLength = name.size;
                            typeInfo->fullyQualifiedName = fullyQualified.ptr;
                            typeInfo->fullyQualifiedNameLength = fullyQualified.size;
                            typeInfo->typeClass = TypeClass::Interface;

                            // interfaces cannot declare any nested types so no need to descend

                            break;
                        }

                        case NodeType::StructDeclaration: {

                            StructDeclarationNode* structNode = (StructDeclarationNode*) pDecl;

                            TypeInfo* typeInfo = fileInfo->allocator.Allocate<TypeInfo>(1);
                            DeclaredType declaredType;

                            declaredType.typeInfo = typeInfo;
                            declaredType.typeContext = nullptr;

                            declaredTypesList.Add(declaredType);

                            int32 genericCount = CountTypeArguments(structNode->typeParameters);
                            FixedCharSpan name = GetDeclarationName(structNode->identifier);
                            FixedCharSpan fullyQualified = MakeFullyQualifiedName(name, genericCount);

                            typeInfo->declaringFile = fileInfo;
                            typeInfo->namespacePath = namespaceStack.Peek();
                            typeInfo->nodeIndex = ptr;
                            typeInfo->modifiers = GetModifiers(structNode->modifiers);
                            typeInfo->typeName = name.ptr;
                            typeInfo->typeNameLength = name.size;
                            typeInfo->fullyQualifiedName = fullyQualified.ptr;
                            typeInfo->fullyQualifiedNameLength = fullyQualified.size;
                            typeInfo->typeClass = TypeClass::Struct;

                            CreateGenericArguments(typeInfo, GetNode(structNode->typeParameters));

                            ClassBodyNode* bodyNode = GetNode(structNode->classBody);

                            if (bodyNode != nullptr) {
                                namespaceStack.Push(MakeTypeBasedNamespace(typeInfo, genericCount));
                                GatherDeclarationsRecursive(bodyNode->first, typeInfo);
                                namespaceStack.Pop();
                            }

                            break;
                        }

                        case NodeType::ClassDeclaration: {

                            ClassDeclarationNode* classNode = (ClassDeclarationNode*) pDecl;

                            TypeInfo* typeInfo = fileInfo->allocator.Allocate<TypeInfo>(1);

                            DeclaredType declaredType;

                            declaredType.typeInfo = typeInfo;
                            declaredType.typeContext = nullptr;

                            declaredTypesList.Add(declaredType);

                            int32 genericCount = CountTypeArguments(classNode->typeParameters);
                            FixedCharSpan name = GetDeclarationName(classNode->identifier);
                            FixedCharSpan fullyQualified = MakeFullyQualifiedName(name, genericCount);

                            typeInfo->declaringFile = fileInfo;
                            typeInfo->namespacePath = namespaceStack.Peek();
                            typeInfo->nodeIndex = ptr;
                            typeInfo->modifiers = GetModifiers(classNode->modifiers);
                            typeInfo->typeName = name.ptr;
                            typeInfo->typeNameLength = name.size;
                            typeInfo->fullyQualifiedName = fullyQualified.ptr;
                            typeInfo->fullyQualifiedNameLength = fullyQualified.size;
                            typeInfo->typeClass = TypeClass::Class;

                            CreateGenericArguments(typeInfo, GetNode(classNode->typeParameters));

                            ClassBodyNode* bodyNode = GetNode(classNode->classBody);

                            if (bodyNode != nullptr) {
                                namespaceStack.Push(MakeTypeBasedNamespace(typeInfo, genericCount));
                                GatherDeclarationsRecursive(bodyNode->first, typeInfo);
                                namespaceStack.Pop();
                            }

                            break;
                        }

                        case NodeType::FieldDeclaration: {
                            if (parent != nullptr) {
                                parent->fieldCount++;
                                if (((FieldDeclarationNode*) pDecl)->initExpression.IsValid()) {
                                    parent->flags |= TypeInfoFlags::RequiresInitConstructor;
                                }
                            }
                            break;
                        }
                        case NodeType::ConstantDeclaration: {
                            // i guess constants are kind of fields
                            if (parent != nullptr) {
                                parent->fieldCount++;
                            }
                            break;
                        }
                        case NodeType::IndexerDeclaration: {
                            if (parent != nullptr) {
                                parent->indexerCount++;
                            }
                            break;
                        }
                        case NodeType::PropertyDeclaration: {
                            if (parent != nullptr) {
                                parent->propertyCount++;
                                PropertyDeclarationNode* prop = (PropertyDeclarationNode*) pDecl;
                                // if(prop->initExpression.IsValid()) {
                                // parent->flags |= TypeInfoFlags::RequiresInitConstructor;
                                // }
                            }
                        }
                        case NodeType::ConstructorDeclaration: {
                            if (parent != nullptr) {
                                parent->constructorCount++;
                            }
                            break;
                        }
                        case NodeType::MethodDeclaration: {
                            if (parent != nullptr) {
                                parent->methodCount++;

                                MethodDeclarationNode* methodDecl = (MethodDeclarationNode*) pDecl;

                                // when we see a method with optional parameters we want to make a new method info for them w/ concrete values
                                // so we don't need to bother with resolving defaults later

                                FormalParameterListNode* parameterListNode = GetNode(methodDecl->parameters);
                                if (parameterListNode != nullptr) {
                                    FormalParameterNode* param = GetNode(parameterListNode->listStart);
                                    int32 cnt = 0;
                                    while (param != nullptr) {
                                        cnt++;
                                        param = GetNode(param->next);
                                    }
                                    int32 firstDefault = -1;
                                    param = GetNode(parameterListNode->listStart);
                                    int32 i = 0;
                                    while (param != nullptr) {
                                        if (param->defaultValue.IsValid()) {
                                            firstDefault = i;
                                            break;
                                        }
                                        i++;
                                        param = GetNode(param->next);
                                    }

                                    if (firstDefault != -1) {
                                        parent->methodCount += (cnt - firstDefault + 1);
                                    }

                                }
                            }
                        }
                        default: {
                            break;
                        }

                    }

                    ptr = pDecl->next;

                }

            }


            void GatherDeclarations(FileInfo* _fileInfo, LinearAllocator* pAllocator) {

                this->fileInfo = _fileInfo;

                namespaceStack = FixedPodList(pAllocator->AllocateUncleared<NamespaceInfo*>(256), 256);

                FileNode* fileNode = fileInfo->parseResult.GetRootNode();

                assert(fileNode != nullptr && "FileNode should not be null");

                NamespaceInfo* rootNamespace = fileInfo->allocator.Allocate<NamespaceInfo>(1);
                rootNamespace->name = fileInfo->packageName;
                rootNamespace->typeInfo = nullptr;
                rootNamespace->parent = nullptr;
                rootNamespace->fullyQualifiedName = rootNamespace->name;

                int32 declarationCount = CountDeclarations();
                DeclaredType* declaredTypes = fileInfo->allocator.Allocate<DeclaredType>(declarationCount);
                declaredTypesList = FixedPodList(declaredTypes, declarationCount);

                namespaceStack.Push(rootNamespace);

                GatherDeclarationsRecursive(fileNode->firstDeclaration, nullptr);

                namespaceStack.Pop();

                for (int32 i = 0; i < declaredTypesList.size; i++) {
                    declaredTypesList[i].typeContext = AllocateTypeContext(declaredTypesList[i].typeInfo, &fileInfo->allocator);
                }

                fileInfo->declaredTypes = declaredTypesList.ToCheckedArray();

                assert(namespaceStack.size == 0);

                // now we have a list of typeInfos local to this file


            }

        };

        CheckedArray<FileInfo*> fileList;
        MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap;
        Diagnostics* diagnostics;

        GatherTypeInfoJob(CheckedArray<FileInfo*> fileList, MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap, Diagnostics* diagnostics)
            : fileList(fileList)
            , resolveMap(resolveMap)
            , diagnostics(diagnostics) {}

        void Execute(int32 start, int32 end) override {

            GatherTypeInfoJobImpl impl;

            for (int32 i = start; i < end; i++) {

                FileInfo* fileInfo = fileList[i];

                TempAllocator::Marker m = GetAllocator()->Mark();

                impl.GatherDeclarations(fileInfo, GetAllocator());

                GetAllocator()->RollbackTo(m);

                for (int32 j = 0; j < fileInfo->declaredTypes.size; j++) {

                    TypeInfo* p = fileInfo->declaredTypes[j].typeInfo;

                    if (!resolveMap->TryAdd(p->GetFullyQualifiedTypeName(), p)) {
                        TypeInfo* existing;
                        resolveMap->TryGetValue(p->GetFullyQualifiedTypeName(), &existing);
                        int32 tokenStart = fileInfo->parseResult.psiNodes.GetPointer(p->nodeIndex.index)->nodeBase.tokenStart;
                        LineColumn lc = fileInfo->parseResult.GetLocationFromToken(tokenStart);
                        diagnostics->LogErrorArgs(fileInfo, lc, "%s `%.*s` was also declared in %s", TypeClassToString(p->typeClass), p->fullyQualifiedNameLength, p->fullyQualifiedName, existing->declaringFile->filePath);
                    }

                }

            }

        }

    };
}