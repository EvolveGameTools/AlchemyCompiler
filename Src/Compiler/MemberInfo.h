#pragma once

#include "./ResolvedType.h"

namespace Alchemy::Compilation {

    struct FieldInfo {

        ResolvedType type;
        TypeInfo* declaringType; // I'm not sure yet if this is better as a Resolved type, that might mean we need to register fields etc there instead of or in addition to type info
        char* name;
        uint16 nameLength;
        DeclarationModifier modifiers;
        NodeIndex<FieldDeclarationNode> nodeIndex;

        FixedCharSpan GetName() const;

        bool IsStatic();

        bool IsConst();

    };

    FixedCharSpan FieldInfo::GetName() const {
        return FixedCharSpan(name, nameLength);
    }

    bool FieldInfo::IsStatic() {
        return (modifiers & DeclarationModifier::Static) != 0;
    }

    bool FieldInfo::IsConst() {
        return (modifiers & DeclarationModifier::Const) != 0;
    }

    struct ParameterInfo {
        ResolvedType type;
        char* name;
        uint16 nameLength;
        StorageClass storage;
        ArgumentPassByModifier passByModifier;
        NodeIndex<FormalParameterNode> nodeIndex;
        bool hasDefaultValue;

        FixedCharSpan GetName() const {
            return FixedCharSpan(name, nameLength);
        }

    };

    struct ConstructorInfo {
        char* name;
        ParameterInfo* parameters;
        uint16 nameLength;
        uint16 parameterCount;
        DeclarationModifier modifiers;
        NodeIndex<ConstructorDeclarationNode> nodeIndex;
    };

    struct MethodInfo {
        TypeInfo* genericArguments;
        ParameterInfo* parameters;
        MethodInfo* prototype; // for handling methods w/ optional args, this is the original method declaration
        TypeInfo * declaringType;
        char* name;
        ResolvedType returnType;
        uint16 nameLength;
        uint16 parameterCount;
        uint16 genericArgumentCount;
        DeclarationModifier modifiers;
        NodeIndex<MethodDeclarationNode> nodeIndex;
        bool isGenericDefinition;
        bool isOptionalParameterPrototype;
        std::atomic<bool> hasCodeGen;

        FixedCharSpan GetName() const {
            return FixedCharSpan(name, nameLength);
        }

        CheckedArray<ParameterInfo> GetParameters() {
            return CheckedArray<ParameterInfo>(parameters, parameterCount);
        }

        CheckedArray<TypeInfo> GetGenericArguments() {
            return CheckedArray<TypeInfo>(genericArguments, genericArgumentCount);
        }

        bool IsStatic() {
            return (modifiers & DeclarationModifier::Static) != 0;
        }

        bool IsGeneric() {
            return genericArgumentCount != 0;
        }

        bool IsGenericMethodDefinition() {
            return isGenericDefinition;
        }

    };

    struct IndexerInfo {

        ResolvedType type;
        TypeInfo* declaringType;
        DeclarationModifier modifiers;

        NodeIndex<PropertyDeclarationNode> nodeIndex;
        NodeIndex<PropertyGetterNode> getterNodeIndex;
        NodeIndex<PropertySetterNode> setterNodeIndex;

    };

    struct MethodGroup {
        FixedCharSpan name;
        CheckedArray<MethodInfo*> methodInfos;
    };

    struct PropertyInfo {
        ResolvedType type;
        TypeInfo* declaringType;
        char* name;
        uint16 nameLength;
        DeclarationModifier modifiers;

        NodeIndex<PropertyDeclarationNode> nodeIndex;
        NodeIndex<PropertyGetterNode> getterNodeIndex;
        NodeIndex<PropertySetterNode> setterNodeIndex;

        FixedCharSpan GetName() const {
            return FixedCharSpan(name, nameLength);
        }

        bool IsBackedProperty() {
            return true;
        }

        bool IsStatic() {
            return (modifiers & DeclarationModifier::Static) != 0;
        }

    };

    struct GenericConstraint {
        // todo
    };

    bool HasAnyMethodWithName(TypeInfo* typeInfo, FixedCharSpan name) {
        while (true) {

            for (int32 i = 0; i < typeInfo->methodCount; i++) {

                MethodInfo* methodInfo = &typeInfo->methods[i];

                if (methodInfo->isOptionalParameterPrototype) {
                    continue;
                }

                if (name == methodInfo->GetName()) {
                    return true;
                }

            }

            if (typeInfo->baseTypeCount == 0) {
                return false;
            }

            typeInfo = typeInfo->baseType[0];

            if (typeInfo->typeClass != TypeClass::Class) {
                return false;
            }

        }
    }

    bool TypeInfoGetMethodCountWithParameterCount(TypeInfo* typeInfo, FixedCharSpan name, int32 parameterCount, int32* retn) {
        int32 cnt = 0;
        while (true) {

            for (int32 i = 0; i < typeInfo->methodCount; i++) {

                MethodInfo* methodInfo = &typeInfo->methods[i];

                if (methodInfo->isOptionalParameterPrototype || methodInfo->parameterCount != parameterCount) {
                    continue;
                }

                if (name == methodInfo->GetName()) {
                    cnt++;
                }

            }

            if (typeInfo->baseTypeCount == 0) {
                *retn = cnt;
                return cnt != 0;
            }

            typeInfo = typeInfo->baseType[0];

            if (typeInfo->typeClass != TypeClass::Class) {
                *retn = cnt;
                return cnt != 0;
            }
        }
    }

    bool TypeInfoGetMethodCount(TypeInfo* typeInfo, FixedCharSpan name, int32* retn) {
        int32 cnt = 0;
        while (true) {

            for (int32 i = 0; i < typeInfo->methodCount; i++) {

                MethodInfo* methodInfo = &typeInfo->methods[i];

                if (methodInfo->isOptionalParameterPrototype) {
                    continue;
                }

                if (name == methodInfo->GetName()) {
                    cnt++;
                }

            }

            if (typeInfo->baseTypeCount == 0) {
                *retn = cnt;
                return cnt != 0;
            }

            typeInfo = typeInfo->baseType[0];

            if (typeInfo->typeClass != TypeClass::Class) {
                *retn = cnt;
                return cnt != 0;
            }
        }

    }

    bool TypeInfoTryGetMethodGroupWithParameterCount(TypeInfo* typeInfo, FixedCharSpan name, LinearAllocator* allocator, int32 parameterCount, MethodGroup* methodGroup) {
        int32 cnt = 0;

        if (!TypeInfoGetMethodCountWithParameterCount(typeInfo, name, parameterCount, &cnt)) {
            return false;
        }

        methodGroup->name = name;
        methodGroup->methodInfos = CheckedArray<MethodInfo*>(allocator->AllocateUncleared<MethodInfo*>(cnt), cnt);

        cnt = 0;
        while (true) {

            for (int32 i = 0; i < typeInfo->methodCount; i++) {
                MethodInfo* methodInfo = &typeInfo->methods[i];

                if (methodInfo->isOptionalParameterPrototype || methodInfo->parameterCount != parameterCount) {
                    continue;
                }

                if (name == methodInfo->GetName()) {
                    methodGroup->methodInfos[cnt++] = methodInfo;
                }

            }

            if (typeInfo->baseTypeCount == 0) {
                return true;
            }

            typeInfo = typeInfo->baseType[0];

            if (typeInfo->typeClass != TypeClass::Class) {
                return true;
            }

        }
    }

    bool TypeInfoTryGetMethodGroup(TypeInfo* typeInfo, FixedCharSpan name, LinearAllocator* allocator, MethodGroup* methodGroup) {

        int32 cnt = 0;
        if (!TypeInfoGetMethodCount(typeInfo, name, &cnt)) {
            return false;
        }

        methodGroup->name = name;
        methodGroup->methodInfos = CheckedArray<MethodInfo*>(allocator->AllocateUncleared<MethodInfo*>(cnt), cnt);

        cnt = 0;
        while (true) {

            for (int32 i = 0; i < typeInfo->methodCount; i++) {
                MethodInfo* methodInfo = &typeInfo->methods[i];

                if (methodInfo->isOptionalParameterPrototype) {
                    continue;
                }

                if (name == methodInfo->GetName()) {
                    methodGroup->methodInfos[cnt++] = methodInfo;
                }

            }

            if (typeInfo->baseTypeCount == 0) {
                return true;
            }

            typeInfo = typeInfo->baseType[0];

            if (typeInfo->typeClass != TypeClass::Class) {
                return true;
            }

        }

    }

    bool TypeInfoTryGetProperty(TypeInfo* typeInfo, FixedCharSpan name, PropertyInfo** retn) {
        while (true) {

            for (int32 i = 0; i < typeInfo->propertyCount; i++) {
                if (name == typeInfo->properties[i].GetName()) {
                    *retn = &typeInfo->properties[i];
                    return true;
                }
            }

            if (typeInfo->baseTypeCount == 0) {
                return false;
            }

            typeInfo = typeInfo->baseType[0];

            if (typeInfo->typeClass != TypeClass::Class) {
                return false;
            }

        }
    }

    bool TypeInfoTryGetField(TypeInfo* typeInfo, FixedCharSpan name, FieldInfo** retn) {

        while (true) {

            for (int32 i = 0; i < typeInfo->fieldCount; i++) {
                if (name == typeInfo->fields[i].GetName()) {
                    *retn = &typeInfo->fields[i];
                    return true;
                }
            }

            if (typeInfo->baseTypeCount == 0) {
                return false;
            }

            typeInfo = typeInfo->baseType[0];

            // interfaces shouldn't define fields
            if (typeInfo->typeClass != TypeClass::Class) {
                return false;
            }

        }

    }

    bool TypeInfoIsAssignableFrom(TypeInfo* typeInfo, TypeInfo* from) {

        if (typeInfo == from) {
            return true;
        }

        // todo -- this only checks reference assignability right now

        // derived type
        if (from->IsSubclassOf(typeInfo)) {
            return true;
        }

        // interface compatible
        if (typeInfo->typeClass == TypeClass::Interface) {
            return TypeInfoImplements(from, typeInfo);
        }

        // todo -- implicit & explicit conversions if we support them

        return false;

    }


}