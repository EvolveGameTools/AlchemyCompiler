#pragma once

#include "../JobSystem/Worker.h"
#include "TypeInfo.h"
#include "Diagnostics.h"
#include "../Collections/MSIHashmap.h"
#include "StringUtil.h"
#include "./TypeResolver.h"

namespace Alchemy::Compilation {

    struct ResolveMemberTypesJob : Alchemy::Jobs::IJob {

        CheckedArray<FileInfo*> fileList;
        CheckedArray<TypeInfo*> builtInTypeInfos;
        MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap;
        Diagnostics* diagnostics;
        CheckedArray<AbstractPsiNode> psiNodes;
        TypeResolver typeResolver;
        GenericTypeCache * genericTypeCache;
        FixedPodList<TypeInfo*> genericArgumentStack;

        ResolveMemberTypesJob(CheckedArray<FileInfo*> fileList, GenericTypeCache * genericTypeCache, MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap, CheckedArray<TypeInfo*> builtInTypeInfos, Diagnostics* diagnostics)
            : fileList(fileList)
            , resolveMap(resolveMap)
            , genericTypeCache(genericTypeCache)
            , builtInTypeInfos(builtInTypeInfos)
            , diagnostics(diagnostics)
            , typeResolver() {}


        void Execute(int32 start, int32 end) override {

            genericArgumentStack = FixedPodList(GetAllocator()->AllocateUncleared<TypeInfo*>(128), 128);

            for (int32 i = start; i < end; i++) {
                FileInfo* fileInfo = fileList[i];
                ResolveMembers(fileInfo);
            }

        }

        template<class T>
        T* GetNode(NodeIndex<T> nodeIndex) {
            if (nodeIndex.index == 0) {
                return nullptr;
            }
            return (T*) psiNodes.GetPointer(nodeIndex.index);
        }


        static void CreatePropertyInfo(TypeInfo* declaringType, DeclarationModifier modifiers, ResolvedType propertyType, FixedCharSpan propertyName, bool hasGetter, bool hasSetter, NodeIndex<PropertyDeclarationNode> nodeIndex) {
            PropertyInfo* propertyInfo = &declaringType->properties[declaringType->propertyCount++];
            assert(propertyInfo != nullptr);
            propertyInfo->type = propertyType;
            propertyInfo->modifiers = modifiers;
            propertyInfo->nodeIndex = nodeIndex;
            propertyInfo->declaringType = declaringType;
            propertyInfo->getterNodeIndex = propertyInfo->getterNodeIndex;
            propertyInfo->setterNodeIndex = propertyInfo->setterNodeIndex;
            propertyInfo->name = propertyName.ptr;
            propertyInfo->nameLength = propertyName.size;
        }

        static void CreateFieldInfo(TypeInfo* declaringType, DeclarationModifier modifiers, ResolvedType fieldType, FixedCharSpan fieldName, NodeIndex<FieldDeclarationNode> nodeIndex) {
            FieldInfo* fieldInfo = &declaringType->fields[declaringType->fieldCount++];
            fieldInfo->type = fieldType;
            fieldInfo->modifiers = modifiers;
            fieldInfo->nodeIndex = nodeIndex;
            fieldInfo->declaringType = declaringType;
            fieldInfo->name = fieldName.ptr;
            fieldInfo->nameLength = fieldName.size;
        }

        void GatherUsings(FileInfo* fileInfo, NodeIndex<DeclarationNode> first) {

            if (!first.IsValid()) {
                return;
            }
            typeResolver = TypeResolver(
                fileInfo,
                genericTypeCache,
                resolveMap,
                diagnostics,
                builtInTypeInfos,
                CheckedArray<TypeInfo*>(),
                GetAllocator()
            );
            NodeIndex<DeclarationNode> ptr = first;

            int32 cnt = 1;  // first slot is for the implicit package name
            int32 aliasCount = 0;

            while (ptr.IsValid()) {

                DeclarationNode* pDecl = (DeclarationNode*) fileInfo->parseResult.psiNodes.GetPointer(ptr.index);

                if (pDecl->nodeType == NodeType::UsingNamespaceDeclaration) {
                    cnt++;
                }
                else if (pDecl->nodeType == NodeType::UsingAliasDeclaration) {
                    aliasCount++;
                }

                ptr = pDecl->next;

            }

            fileInfo->usingNamespaces = CheckedArray<FixedCharSpan>(fileInfo->allocator.Allocate<FixedCharSpan>(cnt), cnt);
            fileInfo->usingAliases = CheckedArray<UsingAlias>(fileInfo->allocator.Allocate<UsingAlias>(aliasCount), aliasCount);

            fileInfo->usingNamespaces[0] = fileInfo->packageName;

            cnt = 1;
            aliasCount = 0;

            ptr = first;
            while (ptr.IsValid()) {

                DeclarationNode* pDecl = (DeclarationNode*) fileInfo->parseResult.psiNodes.GetPointer(ptr.index);

                if (pDecl->nodeType == NodeType::UsingNamespaceDeclaration) {
                    // todo -- validate that we actually can resolve the namespace

                    UsingNamespaceNode* namespaceNode = (UsingNamespaceNode*) pDecl;
                    if (namespaceNode->namespacePath.IsValid()) {
                        fileInfo->usingNamespaces[cnt++] = GetNode(namespaceNode->namespacePath)->GetNamespacePath();
                    }
                    else {
                        fileInfo->usingNamespaces.size--;
                    }

                }
                else if (pDecl->nodeType == NodeType::UsingAliasDeclaration) {
                    UsingAliasNode* aliasNode = (UsingAliasNode*) pDecl;

                    // todo -- make sure using alias doesn't include nullable or array types at the outer level
                    // [] and ? cannot appear anywhere else
                    // generics must be concrete (but can contain [] and ?

                    // System::List<string>::SubType
                    // System::List<string>::SubType[]
                    // System::List<string>::SubType?
                    // System::List<string>::SubType[]?

                    fileInfo->usingAliases[aliasCount].name = aliasNode->GetAliasName();

                    ResolvedType* resolvedType = fileInfo->allocator.Allocate<ResolvedType>(1);
                    if (typeResolver.TryResolveTypePath(GetNode(aliasNode->type), resolvedType)) {
                        aliasCount++;
                        fileInfo->usingAliases[aliasCount].typeInfo = resolvedType->typeInfo;
                    }

                }
                ptr = pDecl->next;

            }

            fileInfo->usingAliases.size = aliasCount;

        }

        void ResolveMembers(FileInfo* fileInfo) {

            genericArgumentStack.size = 0;

            typeResolver.inputGenericArguments = genericArgumentStack.ToCheckedArray();

            psiNodes = fileInfo->parseResult.psiNodes;
            GatherUsings(fileInfo, fileInfo->parseResult.GetRootNode()->firstDeclaration);

            for (int32 i = 0; i < fileInfo->declaredTypes.size; i++) {
                TypeInfo* typeInfo = fileInfo->declaredTypes[i].typeInfo;

                if (typeInfo->fieldCount > 0) {
                    typeInfo->fields = fileInfo->allocator.Allocate<FieldInfo>(typeInfo->fieldCount);
                    typeInfo->fieldCount = 0; // use as an accumulator
                }

                if (typeInfo->propertyCount > 0) {
                    typeInfo->properties = fileInfo->allocator.Allocate<PropertyInfo>(typeInfo->propertyCount);
                    typeInfo->propertyCount = 0;
                }

                if (typeInfo->methodCount > 0) {
                    typeInfo->methods = fileInfo->allocator.Allocate<MethodInfo>(typeInfo->methodCount);
                    typeInfo->methodCount = 0;
                }

                if (typeInfo->indexerCount > 0) {
                    typeInfo->indexers = fileInfo->allocator.Allocate<IndexerInfo>(typeInfo->indexerCount);
                    typeInfo->indexerCount = 0;
                }

                if (typeInfo->constructorCount > 0) {
                    typeInfo->constructors = fileInfo->allocator.Allocate<ConstructorInfo>(typeInfo->constructorCount);
                    typeInfo->constructorCount = 0;
                }

                switch (typeInfo->typeClass) {

                    case TypeClass::GenericArgument: {
                        break;
                    }

                    case TypeClass::Class: {
                        ClassDeclarationNode* classNode = (ClassDeclarationNode*) psiNodes.GetPointer(typeInfo->nodeIndex.index);
                        ClassBodyNode* classBodyNode = GetNode(classNode->classBody);

                        NodeIndex<DeclarationNode> ptr = classBodyNode->first;

                        CheckedArray<ResolvedType> typeArguments = typeInfo->GetGenericArguments();

                        int32 genStackSize = genericArgumentStack.size;

                        for (int32 a = 0; a < typeArguments.size; a++) {
                            genericArgumentStack.Add(typeArguments[a].typeInfo); // todo -- not sure about this yet
                        }

                        typeResolver.inputGenericArguments = genericArgumentStack.ToCheckedArray();

                        TypeListNode* baseTypeListNode = GetNode(classNode->baseType);

                        if (baseTypeListNode != nullptr) {

                            TypePathNode* firstBasePath = GetNode(baseTypeListNode->first);
                            TypePathNode* typePtr = firstBasePath;

                            int32 baseCount = 0;
                            while (typePtr != nullptr) {
                                baseCount++;
                                typePtr = GetNode(typePtr->next);
                            }

                            typeInfo->baseType = fileInfo->allocator.AllocateUncleared<TypeInfo*>(baseCount);
                            typeInfo->baseTypeCount = baseCount;

                            if (firstBasePath != nullptr) {

                                int32 baseTypeIndex = 0;

                                ResolvedType baseType;
                                if (typeResolver.TryResolveTypePath(firstBasePath, &baseType)) {

                                    assert(!baseType.IsArray());
                                    assert(!baseType.IsNullable());
                                    assert(!baseType.IsNullableArray());

                                    if (!baseType.typeInfo->IsClass()) {
                                        assert(false);
                                    }

                                    typeInfo->baseType[baseTypeIndex++] = baseType.typeInfo;

                                }

                                typePtr = GetNode(firstBasePath->next);

                                while (typePtr != nullptr) {
                                    ResolvedType interface;

                                    if (typeResolver.TryResolveTypePath(typePtr, &interface)) {
                                        assert(interface.IsInterface());
                                        assert(!interface.IsNullable());
                                        assert(!interface.IsArray());
                                        assert(!interface.IsNullableArray());
                                        typeInfo->baseType[baseTypeIndex++] = interface.typeInfo;
                                    }

                                    typePtr = GetNode(typePtr->next);
                                }

                                typeInfo->baseTypeCount = baseTypeIndex;

                            }

                        }

                        while (ptr.IsValid()) {

                            DeclarationNode* decl = (DeclarationNode*) psiNodes.GetPointer(ptr.index);

                            switch (decl->nodeType) {

                                case NodeType::ConstructorDeclaration: {
                                    ConstructorDeclarationNode* constructorNode = (ConstructorDeclarationNode*) decl;
                                    FormalParameterListNode* parameterListNode = GetNode(constructorNode->parameters);
                                    ParameterInfo* parameters = nullptr;
                                    int32 parameterCount = 0;

                                    ConstructorInfo* constructorInfo = &typeInfo->constructors[typeInfo->constructorCount++];

                                    if (parameterListNode != nullptr) {
                                        FormalParameterNode* first = GetNode(parameterListNode->listStart);
                                        FormalParameterNode* p = first;
                                        while (p != nullptr) {
                                            parameterCount++;
                                            p = GetNode(p->next);
                                        }
                                        parameters = fileInfo->allocator.Allocate<ParameterInfo>(parameterCount);
                                        p = first;
                                        parameterCount = 0;
                                        while (p != nullptr) {

                                            FixedCharSpan name = GetNode(p->identifier)->GetIdentifier();
                                            TypePathNode* parameterTypeNode = GetNode(p->type);
                                            ResolvedType parameterType;

                                            typeResolver.TryResolveTypePath(parameterTypeNode, &parameterType);

                                            ParameterInfo* parameterInfo = &parameters[parameterCount++];

                                            parameterInfo->type = parameterType;
                                            parameterInfo->nodeIndex = NodeIndex<FormalParameterNode>(p->nodeIndex);
                                            parameterInfo->storage = p->storage;
                                            parameterInfo->passByModifier = p->passByModifier;
                                            parameterInfo->name = name.ptr;
                                            parameterInfo->nameLength = name.size;

                                            p = GetNode(p->next);
                                        }
                                    }

                                    FixedCharSpan constructorName;
                                    IdentifierNode* constructorNameNode = GetNode(constructorNode->identifier);
                                    ModifierListNode* modifierListNode = GetNode(constructorNode->modifiers);
                                    DeclarationModifier modifiers = DeclarationModifier::None;

                                    if (modifierListNode != nullptr) {
                                        modifiers = modifierListNode->modifiers;
                                    }

                                    if (constructorNameNode != nullptr) {
                                        constructorName = constructorNameNode->GetIdentifier();
                                    }

                                    constructorInfo->name = constructorName.ptr;
                                    constructorInfo->nameLength = constructorName.size;
                                    constructorInfo->parameters = parameters;
                                    constructorInfo->parameterCount = parameterCount;
                                    constructorInfo->nodeIndex = NodeIndex<ConstructorDeclarationNode>(constructorNode->nodeIndex);
                                    constructorInfo->modifiers = modifiers;
                                    break;
                                }

                                case NodeType::MethodDeclaration: {
                                    MethodDeclarationNode* methodDeclarationNode = (MethodDeclarationNode*) decl;

                                    TypePathNode* returnTypeNode = GetNode(methodDeclarationNode->returnType);
                                    ResolvedType returnType;

                                    TypeParameterListNode* typeParameterListNode = GetNode(methodDeclarationNode->typeParameters);
                                    MethodInfo* methodInfo = &typeInfo->methods[typeInfo->methodCount++];

                                    FixedCharSpan methodName;
                                    IdentifierNode* methodNameNode = GetNode(methodDeclarationNode->identifier);
                                    ModifierListNode* modifierListNode = GetNode(methodDeclarationNode->modifiers);
                                    DeclarationModifier modifiers = DeclarationModifier::Internal;

                                    methodInfo->declaringType = typeInfo;

                                    if (modifierListNode != nullptr) {
                                        modifiers = modifierListNode->modifiers;
                                    }

                                    if (methodNameNode != nullptr) {
                                        methodName = methodNameNode->GetIdentifier();
                                    }

                                    // see if this type already declared a method with the same name
                                    // if so we need to allocate a new name to differentiate it
                                    int32 stackSize = genericArgumentStack.size;

                                    if (typeParameterListNode != nullptr) {

                                        IdentifierNode* pTypeParameter = GetNode(typeParameterListNode->first);
                                        int32 methodGenCount = 0;

                                        while (pTypeParameter != nullptr) {
                                            methodGenCount++;
                                            pTypeParameter = GetNode(pTypeParameter->next);
                                        }

                                        methodInfo->genericArguments = fileInfo->allocator.Allocate<TypeInfo>(methodGenCount);
                                        methodInfo->genericArgumentCount = methodGenCount;
                                        pTypeParameter = GetNode(typeParameterListNode->first);
                                        methodGenCount = 0;

                                        while (pTypeParameter != nullptr) {

                                            TypeInfo* argInfo = &methodInfo->genericArguments[methodGenCount++];

                                            argInfo->typeName = pTypeParameter->GetIdentifier().ptr;
                                            argInfo->typeNameLength = pTypeParameter->GetIdentifier().size;
                                            argInfo->fullyQualifiedName = nullptr; // todo -- might need to create this
                                            argInfo->fullyQualifiedNameLength = 0;
                                            argInfo->declaringFile = fileInfo;
                                            argInfo->typeClass = TypeClass::GenericArgument;
                                            argInfo->flags |= TypeInfoFlags::IsGeneric;
                                            argInfo->nodeIndex = NodeIndex<DeclarationNode>(0);

                                            // todo -- constraints

                                            pTypeParameter = GetNode(pTypeParameter->next);

                                            genericArgumentStack.Add(argInfo);
                                        }

                                        typeResolver.inputGenericArguments = genericArgumentStack.ToCheckedArray();

                                    }

                                    typeResolver.TryResolveTypePath(returnTypeNode, &returnType);

                                    FormalParameterListNode* parameterListNode = GetNode(methodDeclarationNode->parameters);
                                    ParameterInfo* parameters = nullptr;
                                    int32 parameterCount = 0;

                                    if (parameterListNode != nullptr) {
                                        FormalParameterNode* first = GetNode(parameterListNode->listStart);
                                        FormalParameterNode* p = first;
                                        while (p != nullptr) {
                                            parameterCount++;
                                            p = GetNode(p->next);
                                        }
                                        parameters = fileInfo->allocator.Allocate<ParameterInfo>(parameterCount);
                                        p = first;
                                        parameterCount = 0;

                                        while (p != nullptr) {

                                            FixedCharSpan name = GetNode(p->identifier)->GetIdentifier();
                                            TypePathNode* parameterTypeNode = GetNode(p->type);
                                            ResolvedType parameterType;

                                            typeResolver.TryResolveTypePath(parameterTypeNode, &parameterType);

                                            ParameterInfo* parameterInfo = &parameters[parameterCount++];

                                            parameterInfo->type = parameterType;
                                            parameterInfo->nodeIndex = NodeIndex<FormalParameterNode>(p->nodeIndex);
                                            parameterInfo->storage = p->storage;
                                            parameterInfo->passByModifier = p->passByModifier;
                                            parameterInfo->name = name.ptr;
                                            parameterInfo->nameLength = name.size;
                                            parameterInfo->hasDefaultValue = p->defaultValue.IsValid();

                                            p = GetNode(p->next);
                                        }
                                    }

                                    for (int32 p = 1; p < parameterCount; p++) {

                                        ParameterInfo& param = parameters[p];

                                        if (!param.hasDefaultValue) {
                                            if (parameters[p - 1].hasDefaultValue) {
                                                diagnostics->LogErrorConst(fileInfo, GetNode(param.nodeIndex)->GetTokenRange(), "Optional parameters must appear after all required parameters");
                                            }
                                        }

                                        if (param.hasDefaultValue) {

                                            // make sure the value isn't ref or out or has a storage class

                                            if (param.passByModifier != ArgumentPassByModifier::None) {
                                                diagnostics->LogErrorConst(fileInfo, GetNode(param.nodeIndex)->GetTokenRange(), "Optional parameters cannot be passed by ref or out");
                                            }

                                            if (param.storage != StorageClass::Default) {
                                                diagnostics->LogErrorConst(fileInfo, GetNode(param.nodeIndex)->GetTokenRange(), "Optional parameters cannot specify storage requirements");
                                            }

                                            param.storage = StorageClass::Default;
                                            param.passByModifier = ArgumentPassByModifier::None;

                                        }

                                        for (int32 j = p - 1; j >= 0; j--) {
                                            if (parameters[j].GetName() == param.GetName()) {
                                                diagnostics->LogErrorArgs(fileInfo, GetNode(param.nodeIndex)->GetTokenRange(), "Duplicate parameter `%.*s`", param.GetName().size, param.GetName().ptr);
                                            }
                                        }

                                    }

                                    int32 defaultParameterStart = -1;

                                    for (int32 p = 0; p < parameterCount; p++) {
                                        if (parameters[p].hasDefaultValue) {
                                            defaultParameterStart = p;
                                            break;
                                        }
                                    }

                                    int32 defaultParameterCount = defaultParameterStart >= 0 ? parameterCount - defaultParameterStart : 0;

                                    // void A(int32 x, int32 y = 1, int32 z = 2);
                                    // --- 2 defaults -> spawn 3 methods
                                    // void A(int32 x)
                                    // void A(int32 x, int32 y)
                                    // void A(int32 x, int32 y, int32 z)

                                    // void B(int32 x = 0);
                                    // -- 1 default -> spawn 2 method
                                    // void B(int32 x);
                                    // void B();

                                    methodInfo->name = methodName.ptr;
                                    methodInfo->nameLength = methodName.size;
                                    methodInfo->parameters = parameters;
                                    methodInfo->parameterCount = parameterCount;
                                    methodInfo->returnType = returnType;
                                    methodInfo->nodeIndex = NodeIndex<MethodDeclarationNode>(methodDeclarationNode->nodeIndex);
                                    methodInfo->modifiers = modifiers;
                                    methodInfo->isOptionalParameterPrototype = defaultParameterCount != 0;

                                    // we generate new methods for optional parameters, it makes method overload resolution way easier later
                                    if (defaultParameterCount > 0) {

                                        int32 nonDefaultCount = defaultParameterStart;

                                        for (int32 d = 0; d < defaultParameterCount + 1; d++) {
                                            MethodInfo* spawned = &typeInfo->methods[typeInfo->methodCount++];

                                            memcpy(spawned, methodInfo, sizeof(MethodInfo));
                                            spawned->isOptionalParameterPrototype = false;
                                            spawned->prototype = methodInfo;
                                            spawned->parameterCount = nonDefaultCount + d;
                                            spawned->parameters = fileInfo->allocator.AllocateUncleared<ParameterInfo>(spawned->parameterCount);

                                            for (int32 p = 0; p < spawned->parameterCount; p++) {
                                                ParameterInfo* parameterInfo = &spawned->parameters[p];
                                                *parameterInfo = parameters[p];
                                                parameterInfo->hasDefaultValue = false;

                                            }

                                        }

                                    }

                                    genericArgumentStack.size = stackSize;
                                    typeResolver.inputGenericArguments = genericArgumentStack.ToCheckedArray();

                                    break;
                                }

                                case NodeType::PropertyDeclaration: {
                                    PropertyDeclarationNode* propertyDeclaration = (PropertyDeclarationNode*) decl;
                                    TypePathNode* propertyType = GetNode(propertyDeclaration->typePath);

                                    if (propertyType == nullptr || !propertyType->namePath.IsValid()) {
                                        break;
                                    }

                                    ModifierListNode* modifierListNode = GetNode(propertyDeclaration->modifiers);
                                    IdentifierNode* fieldNameNode = GetNode(propertyDeclaration->identifier);

                                    DeclarationModifier modifiers = modifierListNode == nullptr ? DeclarationModifier::None : modifierListNode->modifiers;
                                    FixedCharSpan fieldName = fieldNameNode == nullptr ? FixedCharSpan() : fieldNameNode->GetIdentifier();

                                    ResolvedType resolvedType;

                                    typeResolver.TryResolveTypePath(propertyType, &resolvedType);

                                    CreatePropertyInfo(
                                        typeInfo,
                                        modifiers,
                                        resolvedType,
                                        fieldName,
                                        GetNode(propertyDeclaration->getterNode) != nullptr,
                                        GetNode(propertyDeclaration->setterNode) != nullptr,
                                        NodeIndex<PropertyDeclarationNode>(propertyDeclaration->nodeIndex)
                                    );

                                    break;

                                }

                                case NodeType::FieldDeclaration: {
                                    FieldDeclarationNode* fieldDecl = (FieldDeclarationNode*) decl;
                                    TypePathNode* fieldType = GetNode(fieldDecl->typePath);

                                    if (fieldType == nullptr || !fieldType->namePath.IsValid()) {
                                        break;
                                    }

                                    ModifierListNode* modifierListNode = GetNode(fieldDecl->modifiers);
                                    IdentifierNode* fieldNameNode = GetNode(fieldDecl->identifier);
                                    DeclarationModifier modifiers = modifierListNode == nullptr ? DeclarationModifier::None : modifierListNode->modifiers;
                                    FixedCharSpan fieldName = fieldNameNode == nullptr ? FixedCharSpan() : fieldNameNode->GetIdentifier();

                                    ResolvedType resolvedType;

                                    // we still want to add a field even if the type isn't resolved so we can give a proper error
                                    typeResolver.TryResolveTypePath(fieldType, &resolvedType);

                                    CreateFieldInfo(typeInfo, modifiers, resolvedType, fieldName, NodeIndex<FieldDeclarationNode>(fieldDecl->nodeIndex));

                                    break;
                                }
                                default: {
                                    break;
                                }
                            }

                            ptr = decl->next;

                        }

                        genericArgumentStack.size = genStackSize;
                        break;
                    }
                    case TypeClass::Struct:
                    case TypeClass::Interface:
                    case TypeClass::Enum:
                    case TypeClass::Delegate:
                        // not supported
                        break;
                }

            }

        }

    };

}