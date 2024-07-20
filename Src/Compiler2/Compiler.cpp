#include <filesystem>
#include "./Compiler.h"
#include "../Allocation/ThreadLocalTemp.h"
#include "./Jobs/ParseFilesJob.h"
#include "./Jobs/GatherTypeInfoJob.h"
#include "./Jobs/ResolveBaseTypesJob.h"
#include "./Jobs/ResolveMemberTypes.h"
#include "./Jobs/IntrospectScopesJob.h"
#include "./Jobs/ScheduleIntrospectJobs.h"
#include "./LoadBuiltIns.h"

namespace Alchemy::Compilation {

    void RecurseChangedDependants(SourceFileInfo* fileInfo) {

        if (fileInfo->dependantsVisited) {
            return;
        }

        fileInfo->dependantsVisited = true;
        fileInfo->wasChanged = true;

        for (int32 i = 0; i < fileInfo->dependants.size; i++) {
            RecurseChangedDependants(fileInfo->dependants[i]);
        }

    }

    void Compiler::AssignBuiltInType(const char* name, BuiltInTypeName builtInTypeName) {
        TypeInfo* pTypeInfo = nullptr;
        assert(resolveMap.TryResolve(FixedCharSpan(name), &pTypeInfo));
        resolveMap.builtInTypeInfos[(int32) builtInTypeName] = pTypeInfo;
        pTypeInfo->builtInTypeName = builtInTypeName;
        if(IsPrimitiveTypeName(builtInTypeName)) {
            pTypeInfo->flags |= TypeInfoFlags::IsPrimitive;
        }

    }

    Compiler::Compiler(int32 workerCount, FileSystemType fileSystemType)
        : diagnostics(Allocator::MakeMallocator())
        , jobSystem(workerCount)
        , resolveMap(Allocator::MakeMallocator())
        , vfs(fileSystemType)
        , fileInfos()
        , sourceFileBuffer()
        , fileAllocator()
        , typeBuffer() {}

    void Compiler::LoadDependencies() {}

    void Compiler::Compile(CheckedArray<PackageInfo> compiledPackages) {

        if (resolveMap.unresolvedType == nullptr) {

            // todo -- delete these eventually

            resolveMap.unresolvedType = new(MallocateTyped(TypeInfo, 1)) TypeInfo();
            resolveMap.voidType = new(MallocateTyped(TypeInfo, 1)) TypeInfo();

            resolveMap.unresolvedType->fullyQualifiedName = (char*) "__UNRESOLVED__";
            resolveMap.unresolvedType->fullyQualifiedNameLength = strlen(resolveMap.unresolvedType->fullyQualifiedName);
            resolveMap.unresolvedType->typeName = resolveMap.unresolvedType->fullyQualifiedName;
            resolveMap.unresolvedType->typeNameLength = resolveMap.unresolvedType->fullyQualifiedNameLength;
            resolveMap.unresolvedType->typeClass = TypeClass::Unresolved;

            resolveMap.voidType->fullyQualifiedName = (char*) "__VOID__";
            resolveMap.voidType->fullyQualifiedNameLength = strlen(resolveMap.voidType->fullyQualifiedName);
            resolveMap.voidType->typeName = resolveMap.voidType->fullyQualifiedName;
            resolveMap.voidType->typeNameLength = resolveMap.voidType->fullyQualifiedNameLength;
            resolveMap.voidType->typeClass = TypeClass::Void;

            LoadBuiltIns(&fileInfos, &fileAllocator);

        }

        // todo -- figure out where these are coming from

        LoadDependencies();

        sourceFileBuffer.size = 0;

        FixedCharSpan ext("wyx");

        CheckedArray<FixedCharSpan> extensions(&ext, 1);

        for (int32 i = 0; i < compiledPackages.size; i++) {
            FixedCharSpan packageName = compiledPackages.Get(i).packageName;
            FixedCharSpan pathName = compiledPackages.Get(i).absolutePath;
            vfs.LoadFileInfos(packageName, pathName, extensions, &sourceFileBuffer);
        }

        SetupCompilationRun(GetThreadLocalAllocator(), sourceFileBuffer.ToCheckedArray());

        int32 changeCount = 0;

        for (int32 i = 0; i < fileInfos.size; i++) {
            if (fileInfos[i]->wasChanged) {
                changeCount++;
            }
        }

        CheckedArray<SourceFileInfo*> changedFiles(GetThreadLocalAllocator()->Allocate<SourceFileInfo*>(changeCount), changeCount);

        changeCount = 0;

        for (int32 i = 0; i < fileInfos.size; i++) {
            if (fileInfos[i]->wasChanged) {
                changedFiles[changeCount++] = fileInfos[i];
            }
        }

        jobSystem.Execute(ParseFilesJobRoot(&vfs, changedFiles));

        jobSystem.Execute(Jobs::Parallel::Foreach(changedFiles.size), GatherTypeInfoJob(changedFiles));

        for (int32 i = 0; i < changedFiles.size; i++) {
            SourceFileInfo* file = changedFiles[i];
            for (int32 d = 0; d < file->declaredTypes.size; d++) {

                if (!resolveMap.AddUnlocked(file->declaredTypes[d])) {
                    diagnostics.AddError(Diagnostic(ErrorCode::ERR_DuplicateDeclaration, file->declaredTypes[d]->GetFullyQualifiedTypeName()));
                }

            }
        }

        // should only need to do this once I think
        resolveMap.builtInTypeInfos = CheckedArray<TypeInfo*>(typeBuffer, kBuiltInTypeCount);

//        AssignPrimitiveType("BuiltIn::Float2", BuiltInTypeName::Float2);
//        AssignPrimitiveType("BuiltIn::Float3", BuiltInTypeName::Float3);
//        AssignPrimitiveType("BuiltIn::Float4", BuiltInTypeName::Float4);

        AssignBuiltInType("BuiltIn::Int64", BuiltInTypeName::Int64);
        AssignBuiltInType("BuiltIn::Int32", BuiltInTypeName::Int32);
        AssignBuiltInType("BuiltIn::Int16", BuiltInTypeName::Int16);
        AssignBuiltInType("BuiltIn::Int8", BuiltInTypeName::Int8);

        AssignBuiltInType("BuiltIn::UInt64", BuiltInTypeName::UInt64);
        AssignBuiltInType("BuiltIn::UInt32", BuiltInTypeName::UInt32);
        AssignBuiltInType("BuiltIn::UInt16", BuiltInTypeName::UInt16);
        AssignBuiltInType("BuiltIn::UInt8", BuiltInTypeName::UInt8);

        AssignBuiltInType("BuiltIn::Double", BuiltInTypeName::Double);
        AssignBuiltInType("BuiltIn::Float", BuiltInTypeName::Float);
        AssignBuiltInType("BuiltIn::Bool", BuiltInTypeName::Bool);
        AssignBuiltInType("BuiltIn::Char", BuiltInTypeName::Char);
        AssignBuiltInType("BuiltIn::String", BuiltInTypeName::String);
        AssignBuiltInType("BuiltIn::Object", BuiltInTypeName::Object);
        AssignBuiltInType("BuiltIn::Void", BuiltInTypeName::Void);

        // we've got an initial symbol table now
        // we can go ahead and try to resolve base types now
        // we should only need to do this for changed files

        jobSystem.Execute(Jobs::Parallel::Foreach(changedFiles.size), ResolveMemberTypesJob(changedFiles, &resolveMap));
        jobSystem.Execute(Jobs::Parallel::Foreach(changedFiles.size), ResolveBaseTypesJob(changedFiles, &resolveMap));

        // here we start branching I think
        // if we are serving as an lsp we want to introspect files in a given priority w/o codegen
        // if we are compiling with full reflection we want to visit every method
        // if we are compiling without reflection we want to visit starting at the entry points

        // during compilation we're very likely to create additional types for state/closures/etc
        // where do we keep those? do we keep them around or assume we create fresh ones per-pass?
        // if ephemeral we can have each thread handle its own data and we'll just diff them before emitting for selection
//        jobSystem.Execute(Jobs::Parallel::Foreach(changedFiles.size), IntrospectScopesJob(changedFiles, &resolveMap));


        // foreach file
            // get a list of it's instantiated generics
            // pump into a master list (file*, typeInfo*)
            // go through the list
            // remove any type infos that were in a file marked as changed or not touched
            // get all remaining type infos
            // remove duplicates
            // foreach typeinfo in generic cache
            // if not in final de-dup list, remove it / delete it

        // assuming we're just compiling right now and only care about what is reachable from the list of entry points

        // if we compile for full reflection, where we start doesn't matter
        // CheckedArray<TypeInfo*> typeInfos = resolveMap.GetExportedTypes(GetThreadLocalAllocator()->MakeAllocator());

        CheckedArray<TypeInfo*> typeInfos = resolveMap.GetConcreteTypes(GetThreadLocalAllocator()->MakeAllocator());

        // foreach type
            // foreach method
                // introspect & codegen  & write to output somewhere

        jobSystem.Execute(Jobs::Parallel::Batch(typeInfos.size, 3), ScheduleIntrospectScopesJob(typeInfos, &resolveMap));

    }

    void Compiler::SetupCompilationRun(TempAllocator* tempAllocator, CheckedArray<VirtualFileInfo> includedSourceFiles) {

        TempAllocator::ScopedMarker m(tempAllocator);

        // init file state
        for (int32 i = 0; i < fileInfos.size; i++) {
            SourceFileInfo* fileInfo = fileInfos.Get(i);
            fileInfo->dependants.size = 0;
            fileInfo->wasTouched = false;
            fileInfo->wasChanged = false;
            fileInfo->dependantsVisited = false;
        }

        // compute file dependants (could be done at the end of compilation instead)

        for (int32 i = 0; i < fileInfos.size; i++) {

            SourceFileInfo* fileInfo = fileInfos.Get(i);

            for (int32 d = 0; d < fileInfo->dependencies.size; d++) {
                fileInfo->dependencies.Get(d)->dependants.Add(fileInfo);
            }

        }

        // build a lookup table for our files to see if they were created this run or not
        int32 max = fileInfos.size > includedSourceFiles.size ? fileInfos.size : includedSourceFiles.size;
        int32 pow2Size = MathUtil::CeilPow2(max * 2); // *2 for a 50% threshold, it's all temp memory anyway
        if (pow2Size < 16) pow2Size = 16;
        int32 exponent = MathUtil::LogPow2(pow2Size);

        SourceFileInfo** table = tempAllocator->Allocate<SourceFileInfo*>(pow2Size);

        for (int32 i = 0; i < fileInfos.size; i++) {
            SourceFileInfo* file = fileInfos.Get(i);
            int32 h = MsiHash::FNV1a(file->path);
            int32 idx = h;

            while (true) {
                idx = MsiHash::Lookup32(h, exponent, idx);
                if (table[idx] == nullptr) {
                    table[idx] = file;
                    break;
                }
            }

        }

        for (int32 i = 0; i < includedSourceFiles.size; i++) {
            VirtualFileInfo* vFile = includedSourceFiles.GetPointer(i);
            int32 h = MsiHash::FNV1a(vFile->absolutePath, vFile->absolutePathSize);
            int32 idx = h;
            while (true) {
                idx = MsiHash::Lookup32(h, exponent, idx);

                SourceFileInfo* fileInfo = table[idx];

                if (fileInfo == nullptr) {
                    // new file
                    fileInfo = new(fileAllocator.Allocate()) SourceFileInfo();
                    fileInfo->wasTouched = true;
                    fileInfo->wasChanged = true;
                    fileInfo->dependantsVisited = false;
                    fileInfo->lastEditTime = vFile->lastEditTime;
                    fileInfo->path = vFile->GetAbsolutePath();
                    fileInfo->assemblyName = vFile->GetPackageName();
                    table[idx] = fileInfo;
                    fileInfos.Add(fileInfo);
                    break;
                }

                if (fileInfo->path == vFile->GetAbsolutePath()) {

                    // found it
                    fileInfo->wasTouched = true;

                    if (fileInfo->assemblyName != vFile->GetPackageName()) {
                        // this would change fully qualified names so we need to treat this as though it were invalidated
                        fileInfo->assemblyName = vFile->GetPackageName();
                        fileInfo->wasChanged = true;
                    }

                    if (vFile->lastEditTime != fileInfo->lastEditTime) {
                        fileInfo->lastEditTime = vFile->lastEditTime;
                        fileInfo->wasChanged = true;
                    }

                    break;
                }

                // keep looping, paths were not the same

            }
        }

        for (int32 i = 0; i < fileInfos.size; i++) {

            SourceFileInfo* fileInfo = fileInfos.Get(i);

            // if we changed a file or didn't touch a file (ie removed it) then we need to tell dependants it's gone
            if ((fileInfo->wasChanged || !fileInfo->wasTouched) && !fileInfo->dependantsVisited) {
                RecurseChangedDependants(fileInfo);
            }

        }

        // We know what files are dead / invalidated, update our type resolution map to remove those types
        // that originate from a dead / invalidated file. We need to do this before the files are invalidated
        // because the type infos are owned by their declaring file.
        CheckedArray<TypeInfo*> typeInfos = resolveMap.GetValues(GetThreadLocalAllocator()->MakeAllocator());
        FixedPodList list(typeInfos.array, typeInfos.size);
        for (int32 i = 0; i < list.size; i++) {
            SourceFileInfo* fileInfo = list[i]->declaringFile;

            if (fileInfo->wasChanged || !fileInfo->wasTouched) {
                list.SwapRemoveAt(i);
                i--;
            }

        }

        resolveMap.ReplaceValues(list.ToCheckedArray());

        // we need to remove dead files and invalidate changed ones now
        for (int32 i = 0; i < fileInfos.size; i++) {

            SourceFileInfo* fileInfo = fileInfos.Get(i);

            if(fileInfo->isBuiltIn) {
                continue;
            }

            if (!fileInfo->wasTouched) {
                fileAllocator.Free(fileInfo);
                fileInfo->~SourceFileInfo();
                fileInfos.SwapRemoveAt(i);
                i--;
            }
            else if (fileInfo->wasChanged) {
                fileInfo->Invalidate();
            }

        }
    }


}