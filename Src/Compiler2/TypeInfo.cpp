#include "./TypeInfo.h"
#include "./ResolvedType.h"
#include "./MemberInfo.h"
#include "./SourceFileInfo.h"

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

    FixedCharSpan TypeInfo::DeclaringFileName() {
        return declaringFile->path;
    }

    bool TypeInfo::DetectClassCycle(TypeInfo* type, CheckedArray<TypeInfo*> visited, int32 depth, FixedPodList<FixedCharSpan>* path) {
        for (int32 i = 0; i < depth; i++) {
            if (visited[i] == type) {
                if (path != nullptr) {
                    path->Push(type->GetFullyQualifiedTypeName());
                }
                return true;
            }
        }

        // If the type has no baseType, there's no cycle from this node
        if (type->baseTypeCount == 0 || !type->baseTypes[0].IsClass()) {
            return false;
        }

        visited[depth] = type;
        if (path != nullptr) {
            path->Push(type->GetFullyQualifiedTypeName());
        }

        if (DetectClassCycle(type->baseTypes[0].typeInfo, visited, depth + 1, path)) {
            return true;
        }

        if (path != nullptr) {
            path->Pop();
        }

        return false;
    }

    bool TypeInfo::DetectClassCycle(CheckedArray<TypeInfo*> visited, FixedPodList<FixedCharSpan>* path) {
        return DetectClassCycle(this, visited, 0, path);
    }

    FixedCharSpan TypeInfo::GetNamespaceName() {
        return declaringFile->namespaceName.size == 0
            ? FixedCharSpan("global")
            : declaringFile->namespaceName;
    }

    TypeInfo* TypeInfo::GetBaseClass() {
        return typeClass != TypeClass::Class || baseTypeCount == 0
            ? nullptr
            : baseTypes[0].IsClass()
                ? baseTypes[0].typeInfo
                : nullptr;
    }

    CheckedArray<FieldInfo*> TypeInfo::GatherFieldInfos(Allocator allocator) {
        if (typeClass == TypeClass::Struct) {
            CheckedArray<FieldInfo*> retn = CheckedArray<FieldInfo*>(allocator.AllocateUncleared<FieldInfo*>(fieldCount), fieldCount);
            for(int32 i = 0; i < fieldCount; i++) {
                retn[i] = &fields[i];
            }
            return retn;
        }
        else if (typeClass == TypeClass::Class) {

            int32 cnt = 0;
            TypeInfo* ptr = this;

            while (ptr != nullptr) {
                cnt += ptr->fieldCount;
                ptr = ptr->GetBaseClass();
            }

            CheckedArray<FieldInfo*> retn(allocator.AllocateUncleared<FieldInfo*>(cnt), cnt);
            int32 writeIdx = cnt - 1; // return fields in reverse order
            ptr = this;

            while (ptr != nullptr) {
                for (int32 i = 0; i < ptr->fieldCount; i++) {
                    retn[writeIdx--] = &ptr->fields[i];
                }
                ptr = ptr->GetBaseClass();
            }

            assert(writeIdx == -1);

            return retn;

        }

        return CheckedArray<FieldInfo*>();
    }

    bool TypeInfo::IsBuiltIn() {
        return declaringFile->isBuiltIn;
    }

    FixedCharSpan TypeInfo::GetSimpleTypeName() {

        if(builtInTypeName == BuiltInTypeName::Invalid) {
            return GetTypeName();
        }

        return FixedCharSpan(BuiltInTypeNameToString(builtInTypeName));
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

    FixedCharSpan TypeVisibilityToString(TypeVisibility visibility) {
        switch (visibility) {

            case TypeVisibility::Public:
                return FixedCharSpan("public");
            case TypeVisibility::Private:
                return FixedCharSpan("private");
            case TypeVisibility::Internal:
                return FixedCharSpan("internal");
            case TypeVisibility::Export:
                return FixedCharSpan("export");
            default:
                return FixedCharSpan("invalid");
        }
    }
}