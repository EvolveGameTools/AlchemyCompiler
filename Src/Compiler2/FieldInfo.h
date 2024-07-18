#pragma once

#include "./ResolvedType.h"

namespace Alchemy::Compilation {

    struct FieldInfo {
        TypeInfo* declaringType {};
        ResolvedType type;
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