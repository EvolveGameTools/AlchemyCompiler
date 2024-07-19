#include "./TypeResolutionMap.h"
# include "../Util/MathUtil.h"
#include "../Allocation/ThreadLocalTemp.h"
#include "./FieldInfo.h"
#include "../Collections/PodList.h"
#include "../Collections/Sort.h"
#include "./SourceFileInfo.h"

namespace Alchemy::Compilation {

    TypeResolutionMap::TypeResolutionMap(Allocator allocator)
        : allocator(allocator)
        , mutex()
        , size(0)
        , values()
        , unresolvedType(nullptr)
        , voidType(nullptr)
        , longestEntrySize(0)
        , exponent(MathUtil::LogPow2(16)) {

        values = CheckedArray<TypeInfo*>(allocator.Allocate<TypeInfo*>(1 << exponent), 1 << exponent);

    }

    void TypeResolutionMap::ResizeTable() {
        // otherwise we need to re-hash the table
        int32 newExponent = exponent + 1;

        TypeInfo** newList = allocator.Allocate<TypeInfo*>(1 << newExponent);
        int32 previousTotalSize = 1 << exponent;
        for (int32 i = 0; i < previousTotalSize; i++) {
            TypeInfo* pInfo = values[i];
            if (pInfo == nullptr) {
                continue;
            }
            int32 h = MsiHash::FNV1a(pInfo->fullyQualifiedName, pInfo->fullyQualifiedNameLength);
            int32 idx = h;
            while (true) {
                idx = MsiHash::Lookup32(h, newExponent, idx);
                if (newList[idx] == nullptr) {
                    newList[idx] = pInfo;
                    break;
                }
            }
        }

        exponent = newExponent;
        values = CheckedArray(newList, 1 << newExponent);

    }

    bool TypeResolutionMap::AddInternal(TypeInfo* typeInfo) {

        FixedCharSpan qualifiedName = typeInfo->GetFullyQualifiedTypeName();

        int32 h = MsiHash::FNV1a(qualifiedName);

        for (int32 idx = h;;) {
            idx = MsiHash::Lookup32(h, exponent, idx);
            TypeInfo* value = values.Get(idx);

            if (value == nullptr) {

                values[idx] = typeInfo;
                size++;

                int32 threshold = (1 << exponent) >> 1;

                if (typeInfo->fullyQualifiedNameLength > longestEntrySize) {
                    longestEntrySize = typeInfo->fullyQualifiedNameLength;
                }

                if (size > threshold) {
                    ResizeTable();
                }

                return true;
            }
            else if (typeInfo == value) {
                // already in the table, nothing to do
                return true;
            }
            else if (typeInfo->fullyQualifiedNameLength == value->fullyQualifiedNameLength) {

                if (memcmp(typeInfo->fullyQualifiedName, value->fullyQualifiedName, typeInfo->fullyQualifiedNameLength) == 0) {
                    return false; // collision but not identical instances
                }

            }

        }

        UNREACHABLE("TypeResolutionMap::Add");

    }

    bool TypeResolutionMap::AddLocked(TypeInfo* typeInfo) {
        std::unique_lock lock(mutex);
        return AddInternal(typeInfo);
    }

    bool TypeResolutionMap::AddUnlocked(TypeInfo* typeInfo) {
        return AddInternal(typeInfo);
    }

    CheckedArray<TypeInfo*> TypeResolutionMap::GetValues(Allocator alloc) {
        TypeInfo** retn = alloc.AllocateUncleared<TypeInfo*>(size);
        int32 total = 1 << exponent;
        int32 write = 0;
        for (int32 i = 0; i < total; i++) {
            if (values[i] != nullptr) {
                retn[write++] = values[i];
            }
        }
        return CheckedArray<TypeInfo*>(retn, size);
    }

    void TypeResolutionMap::ReplaceValues(CheckedArray<TypeInfo*> array) {

        int32 maxItemCount = 1 << exponent;

        assert(array.size <= maxItemCount / 2);

        memset(values.array, 0, sizeof(TypeInfo*) * maxItemCount);

        size = array.size;
        longestEntrySize = 0;

        for (int32 i = 0; i < size; i++) {
            TypeInfo* info = array[i];
            int32 h = MsiHash::FNV1a(info->fullyQualifiedName, info->fullyQualifiedNameLength);
            int32 idx = h;
            while (true) {
                idx = MsiHash::Lookup32(h, exponent, idx);
                if (values[idx] == nullptr) {
                    values[idx] = info;

                    if (info->fullyQualifiedNameLength > longestEntrySize) {
                        longestEntrySize = info->fullyQualifiedNameLength;
                    }

                    break;
                }
            }
        }

    }

    int32 TypeResolutionMap::GetLongestEntrySize() {
        return longestEntrySize;
    }

    bool TypeResolutionMap::TryResolve(FixedCharSpan span, TypeInfo** pInfo) {
        int32 h = MsiHash::FNV1a(span);
        int32 idx = h;

        while (true) {
            idx = MsiHash::Lookup32(h, exponent, idx);

            TypeInfo* test = values[idx];

            if (test == nullptr) {
                // don't set the out value if not found
                return false;
            }

            if (test->GetFullyQualifiedTypeName() == span) {
                *pInfo = test;
                return true;
            }
        }

    }

    ResolvedType TypeResolutionMap::RecursiveResolveGenerics(ResolvedType input, CheckedArray<GenericReplacement> replacements, Allocator& alloc) {

        // simple type name reference, no work to do
        if (input.typeInfo == nullptr) {
            return input;
        }

        if ((input.typeInfo->flags & TypeInfoFlags::IsGenericArgumentDefinition) != 0) {
            FixedCharSpan typeName = input.typeInfo->GetTypeName();
            for (int32 i = 0; i < replacements.size; i++) {
                if (replacements[i].genericName == typeName) {
                    return replacements[i].resolvedGeneric;
                }
            }
            UNREACHABLE("RecursiveResolveGenerics");
        }

        if ((input.typeInfo->flags & TypeInfoFlags::ContainsOpenGenericTypes) != 0) {

            int32 cnt = input.typeInfo->genericArgumentCount;

            CheckedArray<ResolvedType> replacedArgs(GetThreadLocalAllocator()->AllocateUncleared<ResolvedType>(cnt), cnt);

            for (int32 i = 0; i < cnt; i++) {
                // for each generic type argument, return a replacement of it
                replacedArgs[i] = RecursiveResolveGenerics(input.typeInfo->genericArguments[i], replacements, alloc);
            }

            ResolvedType retn = MakeGenericType(input.typeInfo, replacedArgs, alloc);
            retn.resolvedTypeFlags |= input.resolvedTypeFlags;

            return retn;

        }

        return input;

    }

    ResolvedType TypeResolutionMap::MakeGenericType(TypeInfo* openType, CheckedArray<ResolvedType> typeArguments, Allocator typeAllocator) {

        // todo -- fast path for when we instantiate a Something<T> : Base<T> we dont' need to copy methods etc when creating Base<T>

        assert(openType != nullptr);
        assert(openType->IsGenericTypeDefinition());
        assert(openType->genericArgumentCount == typeArguments.size);

        TempAllocator* tempAllocator = GetThreadLocalAllocator();
        TempAllocator::ScopedMarker scopedMarker(tempAllocator);

        FixedCharSpan fullyQualifiedName = openType->GetFullyQualifiedTypeName(); // returns typename`genericCount w/o <types>

        size_t nameSize = fullyQualifiedName.size + 2; // 2 for < >

        for (int32 i = 0; i < typeArguments.size; i++) {
            nameSize += typeArguments[i].typeInfo->fullyQualifiedNameLength;
            if (i != typeArguments.size - 1) {
                nameSize += 1; // ,
            }
        }

        // we end up with a string like System::Collections::List$1<float>

        char* p = tempAllocator->Allocate<char>(nameSize + 1);
        char* tempNameLookup = p;

        memcpy(p, fullyQualifiedName.ptr, fullyQualifiedName.size);
        p += fullyQualifiedName.size;
        *p++ = '<';
        for (int32 i = 0; i < typeArguments.size; i++) {
            TypeInfo* arg = typeArguments[i].typeInfo;
            memcpy(p, arg->fullyQualifiedName, arg->fullyQualifiedNameLength);
            p += arg->fullyQualifiedNameLength;
            if (i != typeArguments.size - 1) {
                *p = ',';
            }
        }
        *p++ = '>';
        *p = '\0';

        FixedCharSpan lookup(tempNameLookup, nameSize);

        TypeInfo* result;

        {
            // if we already created this type, return it
            std::unique_lock lock(mutex);
            if (TryResolve(lookup, &result)) {
                return ResolvedType(result);
            }
        }

        size_t totalSize = sizeof(TypeInfo) +
            openType->baseTypeCount * sizeof(TypeInfo*) +
            openType->fieldCount * sizeof(FieldInfo) +
            openType->propertyCount * sizeof(PropertyInfo) +
            openType->methodCount * sizeof(MethodInfo) +
            openType->indexerCount * sizeof(IndexerInfo) +
            openType->constructorCount * sizeof(ConstructorInfo) +
            openType->constraintCount * sizeof(GenericConstraint) +
            openType->genericArgumentCount * sizeof(ResolvedType) +
            (nameSize + 1) * sizeof(char);

        uint8* memoryBlock = typeAllocator.Allocate<uint8>(totalSize);

        TypeInfo* newType = (TypeInfo*) (memoryBlock + nameSize + 1);

        *newType = *openType;

        newType->fullyQualifiedName = (char*) memoryBlock;
        newType->fullyQualifiedNameLength = nameSize;

        memcpy(newType->fullyQualifiedName, tempNameLookup, nameSize + 1); // + 1 copies terminator
        newType->typeName = newType->fullyQualifiedName + openType->GetNamespaceName().size + 2;
        newType->typeNameLength = newType->fullyQualifiedNameLength - openType->GetNamespaceName().size - 2;

        CheckedArray<ResolvedType> openGenerics = openType->GetGenericArguments();
        CheckedArray<GenericReplacement> replacements(tempAllocator->AllocateUncleared<GenericReplacement>(typeArguments.size), typeArguments.size);

        for (int32 i = 0; i < typeArguments.size; i++) {
            replacements[i].genericName = openGenerics[i].typeInfo->GetTypeName();
            replacements[i].resolvedGeneric = typeArguments[i];
        }

        for (int32 i = 0; i < newType->genericArgumentCount; i++) {
            newType->genericArguments[i] = typeArguments[i];
        }

        for (int32 i = 0; i < openType->baseTypeCount; i++) {
            newType->baseTypes[i] = RecursiveResolveGenerics(ResolvedType(openType->baseTypes[i]), replacements, typeAllocator);
        }

        for (int32 i = 0; i < openType->fieldCount; i++) {
            newType->fields[i] = openType->fields[i];
            newType->fields[i].declaringType = newType;
            newType->fields[i].type = RecursiveResolveGenerics(newType->fields[i].type, replacements, typeAllocator);
        }

        for (int32 i = 0; i < openType->propertyCount; i++) {
            newType->properties[i] = openType->properties[i];
            newType->properties[i].declaringType = newType;
            newType->properties[i].type = RecursiveResolveGenerics(newType->properties[i].type, replacements, typeAllocator);
        }

        for (int32 i = 0; i < openType->methodCount; i++) {
            newType->methods[i] = openType->methods[i];
            MethodInfo* methodInfo = &newType->methods[i];
            methodInfo->declaringType = newType;
            methodInfo->returnType = RecursiveResolveGenerics(openType->methods[i].returnType, replacements, typeAllocator);

            for (int32 paramIndex = 0; paramIndex < methodInfo->parameterCount; paramIndex++) {
                methodInfo->parameters[paramIndex] = openType->methods[i].parameters[paramIndex];
                methodInfo->parameters[paramIndex].type = RecursiveResolveGenerics(methodInfo->parameters[paramIndex].type, replacements, typeAllocator);
            }

        }

        // todo -- not sure this is true, we may need to check that all of our type args are actually concrete now
        newType->flags &= ~TypeInfoFlags::IsGenericTypeDefinition;
        newType->flags |= TypeInfoFlags::InstantiatedGeneric;

        {
            std::unique_lock lock(mutex);
            TypeInfo* retn = nullptr;
            // in the time we took to create the type data, its possible another thread already created the type and registered it
            if (TryResolve(lookup, &retn)) {
                typeAllocator.Free(newType);
                return ResolvedType(retn);
            }

            AddUnlocked(newType);
            openType->declaringFile->genericInstances.Add(newType);
            return ResolvedType(newType);

        }

    }

    struct TypeInfoPrinter {

        int32 indent;
        PodList<char> buffer;

        TypeInfoPrinter()
            : indent(0), buffer(MEGABYTES(1)) {}

        void PrintLine(const char* str) {
            PrintInline(str);
            buffer.Add('\n');
        }

        void PrintInline(char* str, int32 length) {
            if (length == 0) return;

            for (int32 i = 0; i < length; i++) {
                buffer.Add(str[i]);
            }
        }

        void PrintInline(const char* str) {
            PrintInline((char*) str, strlen(str));
        }

        void PrintInline(FixedCharSpan span) {
            PrintInline(span.ptr, span.size);
        }

        void PrintIndent() {
            for (int32 i = 0; i < 4 * indent; i++) {
                buffer.Add(' ');
            }
        }

        void PrintLine() {
            buffer.Add('\n');
        }

        void PrintTypes(TypeInfo * typeInfo, int32 cnt, ResolvedType * array) {
            PrintInline("[");
            PrintLine();
            indent++;
            for (int32 i = 0; i < cnt; i++) {
                ResolvedType resolvedType = array[i];
                PrintIndent();
                if (resolvedType.builtInTypeName != BuiltInTypeName::Invalid) {
                    PrintInline(BuiltInTypeNameToString(resolvedType.builtInTypeName));
                }
                if (resolvedType.IsUnresolved()) {
                    PrintInline("UNRESOLVED");
                }
                else if (resolvedType.IsVoid()) {
                    PrintInline("void");
                }
                else if (resolvedType.typeInfo != nullptr) {
                    PrintInline(resolvedType.typeInfo->GetFullyQualifiedTypeName());
                }
                else {
                    PrintInline("UNRESOLVED");
                }
                PrintLine();
            }
            indent--;
            PrintIndent();
            PrintInline("]");
        }

        void Print(TypeInfo* typeInfo) {
            buffer.AddRange(typeInfo->fullyQualifiedName, typeInfo->fullyQualifiedNameLength);
            buffer.Add('\n');
            indent++;

            PrintIndent();
            PrintInline("declaringFile = ");
            PrintInline(typeInfo->DeclaringFileName());
            PrintLine();

            PrintIndent();
            PrintInline("visibility = ");
            PrintInline(TypeVisibilityToString(typeInfo->visibility));
            PrintLine();

            PrintIndent();
            PrintInline("typeName = ");
            PrintInline(typeInfo->GetTypeName());
            PrintLine();

            if(typeInfo->genericArgumentCount != 0) {
                PrintIndent();
                PrintInline("generics = ");
                PrintTypes(typeInfo, typeInfo->genericArgumentCount, typeInfo->genericArguments);
                PrintLine();
            }

            PrintIndent();
            PrintInline("baseTypes = ");
            if (typeInfo->baseTypeCount == 0) {
                PrintInline("[]");
            }
            else {
                PrintTypes(typeInfo, typeInfo->baseTypeCount, typeInfo->baseTypes);
            }
            PrintLine();

            PrintIndent();
            PrintInline("flags = ");
            size_t cnt = TypeInfoFlagsToString(typeInfo->flags, nullptr);
            char* c = buffer.Reserve((int32) cnt);
            TypeInfoFlagsToString(typeInfo->flags, c);
            PrintLine();

            indent--;
            PrintLine();

        }

    };

    FixedCharSpan TypeResolutionMap::DumpTypeTable(Allocator dumpAllocator) {
        Allocator temp = GetThreadLocalAllocator()->MakeAllocator();
        CheckedArray<TypeInfo*> typeInfos = GetValues(temp);

        IntrospectionSort(typeInfos.array, typeInfos.size, [](const TypeInfo* a, const TypeInfo* b) {
            FixedCharSpan aName = FixedCharSpan(a->fullyQualifiedName, a->fullyQualifiedNameLength);
            FixedCharSpan bName = FixedCharSpan(b->fullyQualifiedName, b->fullyQualifiedNameLength);

            size_t minSize = (aName.size < bName.size) ? aName.size : bName.size;
            int32 cmp = memcmp(aName.ptr, bName.ptr, minSize);

            if (cmp == 0) {
                if (aName.size < bName.size) return -1;
                if (aName.size > bName.size) return 1;
            }

            return cmp;

        });

        TypeInfoPrinter printer;

        for (int32 i = 0; i < typeInfos.size; i++) {
            printer.Print(typeInfos[i]);
        }

        char* output = dumpAllocator.AllocateUncleared<char>(printer.buffer.size);
        memcpy(output, printer.buffer.array, printer.buffer.size);
        return FixedCharSpan(output, printer.buffer.size);

    }

}