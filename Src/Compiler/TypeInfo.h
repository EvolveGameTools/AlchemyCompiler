#pragma once

#include "../Util/FixedCharSpan.h"
#include "../Parsing/ParseTypes.h"
#include "../Collections/CheckedArray.h"
#include "../Parsing/NodeType.h"
#include "../Parsing/Nodes.h"

namespace Alchemy::Compilation {

    using namespace Alchemy;

    struct TypeInfo;
    struct ResolvedType;

    struct UsingAlias {
        FixedCharSpan name;
        TypeInfo* typeInfo;
    };

    enum class TypeClass : uint8 {
        Class,
        Struct,
        Interface,
        Enum,
        Delegate,
        GenericArgument,
    };

    const char* TypeClassToString(TypeClass typeClass) {
        switch (typeClass) {
            case TypeClass::GenericArgument: {
                return "generic argument";
            }
            case TypeClass::Class:
                return "class";
            case TypeClass::Struct:
                return "struct";
            case TypeClass::Interface:
                return "interface";
            case TypeClass::Enum:
                return "enum";
            case TypeClass::Delegate:
                return "delegate";
            default:
                return "invalid";
        }

    }

    FixedCharSpan GetDeclaringFilePathImpl(TypeInfo* typeInfo);

    bool TypeInfoIsReferenceType(TypeInfo* typeInfo);

    bool TypeInfoIsAssignableFrom(TypeInfo* typeInfo, TypeInfo* from);

    bool TypeInfoIsIndexable(TypeInfo* pInfo);

    enum class TypeInfoFlags : uint8 {

        None = 0,
        IsGeneric = 1 << 0, // this is for a T
        IsGenericTypeDefinition = 1 << 1, // this is for a List<T>
        IsNullable = 1 << 2,
        IsArray = 1 << 3, // remove this?
        RequiresInitConstructor = 1 << 4,
        InstantiatedGeneric = 1 << 5,
        ContainsOpenGenericTypes = 1 << 6,
        IsPrimitive = 1 << 7
    };

    IMPL_ENUM_FLAGS_OPERATORS(TypeInfoFlags);

    struct GenericArgument;
    struct GenericConstraint;
    struct FileInfo;
    struct FieldInfo;
    struct MethodInfo;
    struct IndexerInfo;
    struct PropertyInfo;
    struct ConstructorInfo;
    struct MethodGroup;

    struct NamespaceInfo {
        NamespaceInfo* parent {};
        TypeInfo* typeInfo {};
        FixedCharSpan name;
        FixedCharSpan fullyQualifiedName;
    };

    struct TypeInfo {

        FileInfo* declaringFile;
        char* typeName;
        char* fullyQualifiedName;
        NamespaceInfo* namespacePath;
        TypeInfo** baseType;
        FieldInfo* fields;
        MethodInfo* methods;
        IndexerInfo* indexers;
        PropertyInfo* properties;
        ConstructorInfo* constructors;
        ResolvedType* genericArguments;
        GenericConstraint* constraints;

        DeclarationModifier modifiers;
        TypeClass typeClass;
        TypeInfoFlags flags;
        NodeIndex<DeclarationNode> nodeIndex;

        uint16 fieldCount;
        uint16 methodCount;
        uint16 indexerCount;
        uint16 propertyCount;
        uint16 baseTypeCount; // this includes interfaces right now, base class will always be at index 0 if it exists
        uint16 constructorCount;
        uint16 genericArgumentCount;
        uint16 constraintCount;
        uint16 typeNameLength;
        uint16 fullyQualifiedNameLength;

        static void MakeGenericTypeInfo(TypeInfo* typeInfo, FixedCharSpan name, FileInfo* declaringFile, NodeIndex <IdentifierNode> nodeIndex) {

            // todo -- probably need a new internal name

            memset(typeInfo, 0, sizeof(TypeInfo));

            typeInfo->typeClass = TypeClass::GenericArgument;
            typeInfo->typeName = name.ptr;
            typeInfo->typeNameLength = name.size;
            typeInfo->flags = TypeInfoFlags::IsGeneric;
            typeInfo->nodeIndex = NodeIndex<DeclarationNode>(0);
            typeInfo->declaringFile = declaringFile;
            typeInfo->baseType = nullptr; // todo -- maybe point at defining type?

            // not sure if we set this or not
            typeInfo->fullyQualifiedName = nullptr;
            typeInfo->fullyQualifiedNameLength = 0;
        }

        FixedCharSpan GetTypeName() {
            return FixedCharSpan(typeName, typeNameLength);
        }

        FixedCharSpan GetFullyQualifiedTypeName() {
            return FixedCharSpan(fullyQualifiedName, fullyQualifiedNameLength);
        }

        bool IsExported() const {
            return typeClass == TypeClass::Class && (modifiers & DeclarationModifier::Export) != 0 && genericArgumentCount == 0;
        }

        bool IsEnum() const {
            return typeClass == TypeClass::Enum;
        }

        bool IsGeneric() {
            return (flags & TypeInfoFlags::IsGeneric) != 0;
        }

        bool IsGenericTypeDefinition() {
            return (flags & TypeInfoFlags::IsGenericTypeDefinition) != 0;
        }

        CheckedArray<ResolvedType> GetGenericArguments() const {
            return CheckedArray<ResolvedType>(genericArguments, genericArgumentCount);
        }

        CheckedArray<FieldInfo> GetFields() const {
            return CheckedArray<FieldInfo>(fields, fieldCount);
        }

        CheckedArray<MethodInfo> GetMethods() const {
            return CheckedArray<MethodInfo>(methods, methodCount);
        }

        CheckedArray<PropertyInfo> GetProperties() const {
            return CheckedArray<PropertyInfo>(properties, propertyCount);
        }

        CheckedArray<ConstructorInfo> GetConstructors() const {
            return CheckedArray<ConstructorInfo>(constructors, constructorCount);
        }

        bool IsSubclassOf(TypeInfo* pInfo) {

            if (typeClass == TypeClass::Struct || typeClass == TypeClass::Enum) {
                return false;
            }

            if (pInfo->baseTypeCount == 0) {
                return false;
            }

            TypeInfo* ptr = baseType[0];

            while (ptr != nullptr) {

                if (ptr->typeClass != typeClass) {
                    break;
                }

                if (ptr == pInfo) {
                    return true;
                }

                if (pInfo->baseTypeCount == 0) {
                    break;
                }

                ptr = ptr->baseType[0];

            }

            return false;
        }

        FixedCharSpan GetPackageName() const {
            NamespaceInfo* p = namespacePath;
            while (true) {
                if (p->parent == nullptr) {
                    return p->name;
                }
                p = p->parent;
            }
        }

        bool IsPrimitive() {
            return (flags & TypeInfoFlags::IsPrimitive) != 0;
        }

        bool IsStruct() {
            return typeClass == TypeClass::Struct;
        }

        bool IsClass() {
            return typeClass == TypeClass::Class;
        }

    };

    bool TypeInfoImplements(TypeInfo* typeInfo, TypeInfo* interface) {

        if (typeInfo == interface) {
            return true;
        }

        for (int32 i = 0; i < typeInfo->baseTypeCount; i++) {
            if (TypeInfoImplements(typeInfo->baseType[i], interface)) {
                return true;
            }
        }

        return false;

    }

    bool TypeInfoIsIndexable(TypeInfo* typeInfo) {

        if (typeInfo == nullptr || typeInfo->indexerCount > 0) {
            return true;
        }

        if (typeInfo->typeClass != TypeClass::Struct && typeInfo->typeClass != TypeClass::Class) {
            return false;
        }

        if (typeInfo->baseTypeCount == 0) {
            return false;
        }

        TypeInfo* ptr = typeInfo->baseType[0];
        while (ptr != nullptr) {

            if (ptr->indexerCount > 0) {
                return true;
            }

            if (ptr->baseTypeCount == 0) {
                return false;
            }

            ptr = ptr->baseType[0];

        }

        return false;
    }

}