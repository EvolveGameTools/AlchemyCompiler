#include "./TypeInfo.h"
#include "./ResolvedType.h"
#include "./FieldInfo.h"

namespace Alchemy::Compilation {

    CheckedArray<ResolvedType> TypeInfo::GetGenericArguments() {
        return CheckedArray<ResolvedType>(genericArguments, genericArgumentCount);
    }

    CheckedArray<FieldInfo> TypeInfo::GetFields() {
        return CheckedArray<FieldInfo>(fields, fieldCount);
    }

    CheckedArray<MethodInfo> TypeInfo::GetMethods() {
        return CheckedArray<MethodInfo>(methods, methodCount);
    }

    CheckedArray<PropertyInfo> TypeInfo::GetProperties() {
        return CheckedArray<PropertyInfo>(properties, propertyCount);
    }

    CheckedArray<ConstructorInfo> TypeInfo::GetConstructors() {
        return CheckedArray<ConstructorInfo>(constructors, constructorCount);
    }

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
            case TypeClass::Widget: {
                return "widget";
            }
            case TypeClass::Unresolved: {
                return "__UNRESOLVED__";
            }
            case TypeClass::Void: {
                return "__VOID__";
            }
            default:
                return "invalid";
        }

    }
}