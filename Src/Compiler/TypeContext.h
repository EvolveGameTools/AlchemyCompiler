#pragma once

#include "PooledBlockAllocator.h"

namespace Alchemy::Compilation {

    struct PropertyDefinition {
        Scope* getterScope {};
        Scope* setterScope {};
        PropertyInfo * propertyInfo {};
    };

    struct MethodDefinition {

        Scope* scope;
        MethodInfo* methodInfo;
        VEP** parameters;

        CheckedArray<VEP*> GetParameters() const {
            return CheckedArray<VEP*>(parameters, methodInfo->parameterCount);
        };

    };

    struct TypeContext {

        TypeInfo* typeInfo {};
        Scope* rootScope {};

        VEP** fields {};
        MethodDefinition* methods {};
        PropertyDefinition* properties {};

        uint16 fieldCount {};
        uint16 methodCount {};
        uint16 propertyCount {};
        uint16 indexerCount {};

        explicit TypeContext(TypeInfo* typeInfo)
            : typeInfo(typeInfo) {}

    };

    TypeContext * AllocateTypeContext(TypeInfo * typeInfo, LinearAllocator * allocator) {
        TypeContext * typeContext = allocator->New<TypeContext>(typeInfo);
        // we need to allocate here because we need method declarations to handle entry point searching
        // the data will be populated in the ScopeIntrospection pass

        typeContext->fields = allocator->Allocate<VEP*>(typeInfo->fieldCount);
        typeContext->methods = allocator->Allocate<MethodDefinition>(typeInfo->methodCount);
        typeContext->properties = allocator->Allocate<PropertyDefinition>(typeInfo->propertyCount);

        typeContext->fieldCount = typeInfo->fieldCount;
        typeContext->methodCount = typeInfo->methodCount;
        typeContext->propertyCount = typeInfo->propertyCount;

        for (int32 m = 0; m < typeInfo->methodCount; m++) {
            typeContext->methods[m].methodInfo = &typeInfo->methods[m];
        }

        return typeContext;
    }

}