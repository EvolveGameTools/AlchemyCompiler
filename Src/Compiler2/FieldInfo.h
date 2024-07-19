#pragma once

#include "./ResolvedType.h"

namespace Alchemy::Compilation {

    struct VariableDeclaratorSyntax;

    struct FieldInfo {

        ResolvedType type;
        FixedCharSpan identifier;
        TypeInfo* declaringType {};
        VariableDeclaratorSyntax* syntaxNode {};

    };

    struct PropertyInfo {
        TypeInfo* declaringType {};
        ResolvedType type;
    };

    struct IndexerInfo {};

    struct ParameterInfo {
        ResolvedType type;
    };

    struct MethodInfo {
        TypeInfo* declaringType {};
        ParameterInfo* parameters {};
        ResolvedType returnType;
        int32 parameterCount {};
    };

    struct ConstructorInfo {};

}