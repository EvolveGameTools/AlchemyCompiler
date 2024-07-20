#pragma once

#include "./ResolvedType.h"
#include "../Util/FixedCharSpan.h"
#include "TypeInfo.h"

namespace Alchemy::Compilation {

    struct VariableDeclaratorSyntax;
    struct MethodDeclarationSyntax;
    struct ParameterSyntax;

    DEFINE_ENUM_FLAGS(FieldModifiers, uint8, {
        None = 0,
        Readonly = 1 << 0,
        Static = 1 << 1,
        Const = 1 << 2
    });

    enum class MemberVisibility : uint8 {
        Public,
        Private,
        Internal,
        Protected,
        Export
    };

    struct FieldInfo {

        ResolvedType type;
        FixedCharSpan identifier;
        TypeInfo* declaringType {};
        VariableDeclaratorSyntax* syntaxNode {};
        FieldModifiers modifiers {};
        MemberVisibility visibility {};

    };

    struct PropertyInfo {
        TypeInfo* declaringType {};
        ResolvedType type;
    };

    struct IndexerInfo {};

    size_t FieldModifiersToString(FieldModifiers modifiers, char * buffer);

    DEFINE_ENUM_FLAGS(MethodModifiers, uint8, {
        None = 0,
        Override = 1 << 0,
        Sealed = 1 << 1,
        Abstract = 1 << 2,
        Virtual = 1 << 3,
        Static = 1 << 4
    });

    size_t MethodModifiersToString(MethodModifiers modifiers, char * buffer);

    DEFINE_ENUM_FLAGS(ParameterModifiers, uint8, {
        None = 0,
        Ref = 1 << 0,
        Temp = 1 << 1,
        Out = 1 << 2,
        Readonly = 1 << 3
    })

    size_t ParameterModifiersToString(ParameterModifiers modifiers, char * buffer);

    struct ParameterInfo {

        ResolvedType type;
        FixedCharSpan name;
        ParameterModifiers modifiers {};
        ParameterSyntax * syntaxNode;

    };

    const char * MemberVisibilityToString(MemberVisibility visibility);

    struct MethodInfo {
        TypeInfo* declaringType {};
        ParameterInfo* parameters {};
        MethodDeclarationSyntax* syntaxNode {};
        ResolvedType returnType;
        FixedCharSpan fullyQualifiedName; // may be able to remove this, lets see if it ends up being used
        FixedCharSpan name;
        int32 parameterCount {};
        bool isDefaultParameterOverload {};
        MemberVisibility visibility;
        MethodModifiers modifiers;
    };

    struct ConstructorInfo {};

}