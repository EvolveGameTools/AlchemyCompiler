#pragma once

#include "../PrimitiveTypes.h"
#include "../Util/FixedCharSpan.h"
#include "../Collections/CheckedArray.h"
#include "../Collections/FixedPodList.h"
#include "../Allocation/LinearAllocator.h"
#include "../Util/StringUtil.h"
#include "BuiltInTypeName.h"

namespace Alchemy::Compilation {

    struct TypeInfo;
    struct CompilationUnitSyntax;
    struct SourceFileInfo;
    struct GenericConstraint {};
    struct FieldInfo;
    struct MethodInfo;
    struct IndexerInfo;
    struct PropertyInfo;
    struct ConstructorInfo;
    struct ResolvedType;
    struct SyntaxBase;

    enum class TypeClass : uint8 {
        Class,
        Struct,
        Interface,
        Enum,
        Delegate,
        Widget,
        GenericArgument,
        Unresolved,
        Void,
    };

    const char* TypeClassToString(TypeClass typeClass);

    DEFINE_ENUM_FLAGS(TypeInfoFlags, uint16, {
        None = 0,
        IsGenericArgumentDefinition = 1 << 0, // this is for a T
        IsGenericTypeDefinition = 1 << 1, // this is for a List<T>
        IsNullable = 1 << 2,
        Sealed = 1 << 3,
        RequiresInitConstructor = 1 << 4,
        InstantiatedGeneric = 1 << 5,
        IsPrimitive = 1 << 6,
        Abstract = 1 << 7,
    })

    inline size_t TypeInfoFlagsToString(TypeInfoFlags flags, char* buffer) {

        PrintFlagVars(buffer);

        PrintFlagZero(TypeInfoFlags, None)
        PrintFlag(TypeInfoFlags, IsGenericArgumentDefinition)
        PrintFlag(TypeInfoFlags, IsGenericTypeDefinition)
        PrintFlag(TypeInfoFlags, IsNullable)
        PrintFlag(TypeInfoFlags, Sealed)
        PrintFlag(TypeInfoFlags, RequiresInitConstructor)
        PrintFlag(TypeInfoFlags, InstantiatedGeneric)
        PrintFlag(TypeInfoFlags, IsPrimitive)
        PrintFlag(TypeInfoFlags, Abstract)

        return PrintFlagLength;

    }

    enum class TypeVisibility : uint8 {
        Public,
        Private,
        Internal,
        Export
    };

    FixedCharSpan TypeVisibilityToString(TypeVisibility visibility);

    struct TypeInfo {

        SourceFileInfo* declaringFile {};
        SyntaxBase* syntaxNode {};
        char* typeName {};
        char* fullyQualifiedName {};
        ResolvedType* baseTypes {};
        FieldInfo* fields {};
        MethodInfo* methods {};
        IndexerInfo* indexers {};
        PropertyInfo* properties {};
        ConstructorInfo* constructors {};
        ResolvedType* genericArguments {};
        GenericConstraint* constraints {};

        TypeClass typeClass {};
        TypeInfoFlags flags {};
        TypeVisibility visibility {};
        BuiltInTypeName builtInTypeName {};

        uint16 fieldCount {};
        uint16 methodCount {};
        uint16 indexerCount {};
        uint16 propertyCount {};
        uint16 baseTypeCount {}; // this includes interfaces right now, base class will always be at index 0 if it exists
        uint16 constructorCount {};
        uint16 genericArgumentCount {};
        uint16 constraintCount {};
        uint16 typeNameLength {};
        uint16 fullyQualifiedNameLength {};

        FixedCharSpan GetTypeName() {
            return FixedCharSpan(typeName, typeNameLength);
        }

        FixedCharSpan GetFullyQualifiedTypeName() {
            return FixedCharSpan(fullyQualifiedName, fullyQualifiedNameLength);
        }

        FixedCharSpan DeclaringFileName();


        bool IsExported() {
            return typeClass == TypeClass::Class && (visibility == TypeVisibility::Export) && genericArgumentCount == 0;
        }

        bool IsEnum() {
            return typeClass == TypeClass::Enum;
        }

        inline bool IsGenericArgumentDefinition() {
            return (flags & TypeInfoFlags::IsGenericArgumentDefinition) != 0;
        }

        inline bool IsVoid() {
            return typeClass == TypeClass::Void;
        }

        inline bool IsUnresolved() {
            return typeClass == TypeClass::Unresolved;
        }

        inline bool IsGenericTypeDefinition() {
            return (flags & TypeInfoFlags::IsGenericTypeDefinition) != 0;
        }

        bool DetectClassCycle(CheckedArray<TypeInfo*> visited, FixedPodList<FixedCharSpan>* path = nullptr);

        bool DetectClassCycle(TypeInfo* type, CheckedArray<TypeInfo*> visited, int32 depth, FixedPodList<FixedCharSpan>* path = nullptr);

        CheckedArray<ResolvedType> GetGenericArguments();

        CheckedArray<FieldInfo> GetFields();

        CheckedArray<MethodInfo> GetMethods();

        CheckedArray<PropertyInfo> GetProperties();

        CheckedArray<ConstructorInfo> GetConstructors();

        FixedCharSpan GetNamespaceName();

        CheckedArray<FieldInfo*> GatherFieldInfos(Allocator allocator);

        TypeInfo* GetBaseClass();

        bool IsBuiltIn();

        FixedCharSpan GetSimpleTypeName();
    };


}