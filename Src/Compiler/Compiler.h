#pragma once

#include <chrono>
#include <filesystem>
#include <iostream>
#include "../PrimitiveTypes.h"
#include "../Util/MathUtil.h"
#include "../JobSystem/JobSystem.h"
#include "./GatherTypeInfoJob.h"

#include "./ParseFilesJob.h"
#include "./Diagnostics.h"
#include "./ResolveMemberTypesJob.h"
#include "../Allocation/PoolAllocator.h"
#include "./FileInfo.h"
#include "./ConstructExpressionTrees.h"
#include "./FindEntryPointsJob.h"
#include "./EntryPoint.h"
#include "./AnalyzeTypes.h"
#include "./GenericTypeCache.h"

namespace Alchemy::Compilation {

    namespace fs = std::filesystem;

    struct Compiler {

        Diagnostics diagnostics;
        Alchemy::Jobs::JobSystem jobSystem;
        MSIDictionary<FixedCharSpan, TypeInfo*> resolveMap;
        MSIDictionary<FixedCharSpan, FileInfo*> fileInfoByAbsolutePath;
        Alchemy::PoolAllocator<FileInfo> fileAllocator;

        int32 fileIdAllocator;

        PodList<int32> fileIdFreeList;
        PodList<FileInfo*> fileInfos;
        PodList<FileInfo*> touchedFiles;
        PodList<FileInfo*> changedFiles;
        PodList<FileInfo*> addedFiles;
        PodList<FixedCharSpan> entryPointPatterns;

        LinearAllocator linearAllocator;
        GenericTypeCache genericTypeCache;

        explicit Compiler(int32 threads)
                : jobSystem(threads)
                  , linearAllocator(GIGABYTES(1), KILOBYTES(32))
                  , resolveMap(512)
                  , fileInfos(128)
                  , touchedFiles(128)
                  , addedFiles(128)
                  , changedFiles(16)
                  , fileIdFreeList(16)
                  , fileIdAllocator(0)
                  , genericTypeCache() {}

        ~Compiler() {
            for (int32 i = 0; i < fileInfos.size; i++) {
                fileInfos[i]->Dispose();
            }
        }

        void CompleteCompilationRun() {

            for (int32 i = 0; i < fileInfos.size; i++) {
                fileInfos[i]->wasChanged = false;
            }

        }

        void SetupCompilationRun(TempAllocator * tempAllocator) {

            // todo -- we only need to invalidate dependencies when a file's structure changes
            // ie type declarations change (new member, changed member, changed access, etc)

            TempAllocator::ScopedMarker m(tempAllocator);

            int32 max = fileInfos.size > touchedFiles.size ? fileInfos.size : touchedFiles.size;

            int32 mapSize = LongBoolMap::GetMapSize(max);

            LongBoolMap touchedMap(linearAllocator.Allocate<uint64>(mapSize), mapSize);
            LongBoolMap changedMap(linearAllocator.Allocate<uint64>(mapSize), mapSize);

            for (int32 i = 0; i < touchedFiles.size; i++) {
                touchedMap.Set(touchedFiles[i]->fileId);
            }

            for (int32 i = 0; i < changedFiles.size; i++) {
                changedMap.Set(changedFiles[i]->fileId);
                changedFiles[i]->wasChanged = true;
            }

            for (int32 i = 0; i < fileInfos.size; i++) {

                if (!touchedMap.Get(fileInfos[i]->fileId)) {
                    changedMap.Set(fileInfos[i]->fileId);
                    fileIdFreeList.Push(fileInfos[i]->fileId);

                    fileInfos[i]->Dispose();
                    fileAllocator.Free(fileInfos[i]);
                }

            }

            // go through the files that survived the reload (ie not moved / deleted)
            // if any dependencies were invalidated, we need to mark that file as also
            // being invalid. We loop until nothing was invalidated
            fileInfos.size = 0;
            fileInfos.AddRange(touchedFiles.array, touchedFiles.size);

            while (true) {
                bool invalidated = false;

                for (int32 i = 0; i < fileInfos.size; i++) {
                    LongBoolMap dependencies = fileInfos[i]->GetDependencyMap();

                    if (dependencies.ContainsAny(changedMap)) {
                        changedMap.Set(fileInfos[i]->fileId);
                        fileInfos[i]->Invalidate();
                        invalidated = true;
                    }
                }

                if (!invalidated) {
                    break;
                }

            }

            GenericTypeCache_Invalidate(&genericTypeCache, changedMap, tempAllocator);

            // now add the new files
            fileInfos.AddRange(addedFiles.array, addedFiles.size);
            for (int32 i = 0; i < addedFiles.size; i++) {
                if (fileIdFreeList.size > 0) {
                    addedFiles[i]->fileId = fileIdFreeList.Pop();
                } else {
                    addedFiles[i]->fileId = fileIdAllocator++;
                }
            }

            fileInfoByAbsolutePath.Clear();
            resolveMap.Clear();

            // update the internal file count maps as needed
            for (int32 i = 0; i < fileInfos.size; i++) {
                fileInfos[i]->EnsureFileCount(fileInfos.size);
            }

            for (int32 i = 0; i < fileInfos.size; i++) {
                fileInfoByAbsolutePath.TryAdd(fileInfos[i]->filePath, fileInfos[i]);
            }

            changedFiles.size = 0;
            addedFiles.size = 0;
            touchedFiles.size = 0;

        }

        void LoadFilesInPackage(FixedCharSpan &packageName, const fs::path &directory, const std::string &extension) {
            for (const auto &entry: fs::directory_iterator(directory)) {
                if (fs::is_directory(entry)) {
                    LoadFilesInPackage(packageName, entry.path(), extension);
                } else if (entry.path().extension() == extension) {
                    fs::path absolutePath = fs::absolute(entry.path());
                    std::string absolutePathStr = entry.path().string();

                    FixedCharSpan absolute((char*) absolutePathStr.c_str(), (int32) absolutePathStr.length());

                    FileInfo* fileInfo = nullptr;

                    if (fileInfoByAbsolutePath.TryGetValue(absolute, &fileInfo)) {
                        touchedFiles.Add(fileInfo);
                        uint64 lastEditTime = std::chrono::duration_cast<std::chrono::milliseconds>(fs::last_write_time(entry.path()).time_since_epoch()).count();
                        if (lastEditTime != fileInfo->lastEditTime) {
                            fileInfo->lastEditTime = lastEditTime;
                            changedFiles.Add(fileInfo);
                        }
                    }
                    else {
                        addedFiles.Add(new(fileAllocator.Allocate()) FileInfo(packageName, absolute));
                    }

                }
            }
        }

        void AddEntryPointPattern(FixedCharSpan pattern) {
            entryPointPatterns.Add(pattern);
        }

    };

    using namespace Alchemy::Jobs;

    static CheckedArray<TypeInfo*> s_BuiltInTypeInfos;

    TypeInfo * GetBuiltInTypeInfo(BuiltInTypeName typeName) {
        if(typeName == BuiltInTypeName::Invalid) {
            return nullptr;
        }
        return s_BuiltInTypeInfos[(int32)typeName];
    }

    struct CompileRootJob : Jobs::IJob {

        Diagnostics* diagnostics;
        MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap;
        FixedCharSpan rootPackageName;

        CheckedArray<TypeInfo*> builtInTypes;
        Compiler* compiler;

        explicit CompileRootJob(Compiler* compiler)
                  : compiler(compiler)
                  , resolveMap(&compiler->resolveMap)
                  , diagnostics(&compiler->diagnostics) {}

        void AssignPrimitiveType(const char* name, BuiltInTypeName builtInTypeName) {
            TypeInfo* primitive = nullptr;
            assert(resolveMap->TryGetValue(FixedCharSpan(name), &primitive));
            builtInTypes[(int32) builtInTypeName] = primitive;
            primitive->flags |= TypeInfoFlags::IsPrimitive;
        }

        void RunParseJobs(CheckedArray<FileInfo*> fileInfos) {

            TempAllocator::ScopedMarker marker(GetAllocator());

            CheckedArray<Parser*> perThreadParsers(GetAllocator()->Allocate<Parser*>(GetWorkerCount()), GetWorkerCount());

            for(int32 i = 0; i < perThreadParsers.size; i++) {
                perThreadParsers[i] = GetAllocator()->New<Parser>();
            }

            Await(Parallel::Batch(fileInfos.size, 5), ParseFilesJob(fileInfos, perThreadParsers, diagnostics));

            for(int32 i = 0; i < perThreadParsers.size; i++) {
                perThreadParsers[i]->~Parser();
            }

        }

        void Execute() override {

            FixedCharSpan system("System");
            FixedCharSpan app("TestApp");
            std::string extension(".ax");

            rootPackageName = app;
            compiler->LoadFilesInPackage(system, "../AlchemySystem", extension);
            compiler->LoadFilesInPackage(app, "../TestApp", extension);

            compiler->SetupCompilationRun(GetAllocator());

            compiler->AddEntryPointPattern(FixedCharSpan("Program"));

            CheckedArray<FixedCharSpan> entryPointPatterns = compiler->entryPointPatterns.ToCheckedArray();

            builtInTypes = CheckedArray<TypeInfo*>(GetAllocator()->Allocate<TypeInfo*>(kBuiltInTypeCount), kBuiltInTypeCount);

            CheckedArray<FileInfo*> fileInfos = compiler->fileInfos.ToCheckedArray();

            RunParseJobs(fileInfos);

            Await(Parallel::Batch(fileInfos.size, 1), GatherTypeInfoJob(fileInfos, resolveMap, diagnostics));

            AssignPrimitiveType("System::Int64", BuiltInTypeName::Int64);
            AssignPrimitiveType("System::Int32", BuiltInTypeName::Int32);
            AssignPrimitiveType("System::Int16", BuiltInTypeName::Int16);
            AssignPrimitiveType("System::Int8", BuiltInTypeName::Int8);

            AssignPrimitiveType("System::UInt64", BuiltInTypeName::UInt64);
            AssignPrimitiveType("System::UInt32", BuiltInTypeName::UInt32);
            AssignPrimitiveType("System::UInt16", BuiltInTypeName::UInt16);
            AssignPrimitiveType("System::UInt8", BuiltInTypeName::UInt8);

            AssignPrimitiveType("System::Float", BuiltInTypeName::Float);

            AssignPrimitiveType("System::Float2", BuiltInTypeName::Float2);
            AssignPrimitiveType("System::Float3", BuiltInTypeName::Float3);
            AssignPrimitiveType("System::Float4", BuiltInTypeName::Float4);

            AssignPrimitiveType("System::Double", BuiltInTypeName::Double);

            AssignPrimitiveType("System::Bool", BuiltInTypeName::Bool);

            // because of this we can't have multiple compilation passes in flight at the same time, which should be fine
            s_BuiltInTypeInfos = builtInTypes;

            PodList<EntryPoint> entryPoints(32); // todo -- not a pod list

            JobHandle resolveMemberTypes = Schedule(Parallel::Batch(fileInfos.size, 1), ResolveMemberTypesJob(fileInfos, &compiler->genericTypeCache, resolveMap, builtInTypes, diagnostics));
            JobHandle findEntryPoints = Schedule(FindEntryPointsJob(rootPackageName, fileInfos, entryPointPatterns, &entryPoints, diagnostics));

            Await(resolveMemberTypes, findEntryPoints);

            Await(ConstructExpressionTrees(fileInfos, &compiler->genericTypeCache, resolveMap, builtInTypes, diagnostics));

            MSISet<VisitEntry> globalLookup(1024);

            LinearAllocator* allocator = GetAllocator(); // this is ok because we Await() the completion of the child job, otherwise this would get wiped

            int32 workerCount = GetWorkerCount();

            CheckedArray<LinearAllocator*> perThreadAllocators(allocator->Allocate<LinearAllocator*>(workerCount), workerCount);
            CheckedArray<PodList<char>*> perThreadOutputBuffers(allocator->Allocate<PodList<char>*>(workerCount), workerCount);
            CheckedArray<PodList<CodeGenOutput>*> perThreadCodeGenOutputs(allocator->Allocate<PodList<CodeGenOutput>*>(workerCount), workerCount);
            CheckedArray<MSISet<ResolvedType>*> perThreadTypeSets(allocator->Allocate<MSISet<ResolvedType>*>(workerCount), workerCount);

            Jobs::JobHandle* handles = allocator->Allocate<Jobs::JobHandle>(entryPoints.size);

            for (int32 i = 0; i < workerCount; i++) {
                perThreadAllocators[i] = allocator->New<LinearAllocator>(0, MEGABYTES(1));
                perThreadOutputBuffers[i] = allocator->New<PodList<char>>(4096);
                perThreadCodeGenOutputs[i] = allocator->New<PodList<CodeGenOutput>>(32);
                perThreadTypeSets[i] = allocator->New<MSISet<ResolvedType>>(512);
            }

            std::mutex sharedLock;
            SharedWorkerData data;
            data.mtx = &sharedLock;
            data.perWorkerAllocators = perThreadAllocators;
            data.perWorkerOutputBuffers = perThreadOutputBuffers;
            data.perWorkerCodeGenOutputs = perThreadCodeGenOutputs;
            data.perWorkerTypeSets = perThreadTypeSets;

            for (int32 i = 0; i < entryPoints.size; i++) {
                handles[i] = Schedule(GatherCodeGenEntries(VisitEntry(VisitEntryType::Method, entryPoints[i].methodInfo), &data));
            }

            AwaitAll(entryPoints.size, handles);

            MSISet<ResolvedType> touchedTypes(1024);

            for (int32 i = 0; i < workerCount; i++) {
                TempAllocator::ScopedMarker temp(GetAllocator());
                MSISet<ResolvedType> * typeSet = perThreadTypeSets[i];
                CheckedArray<ResolvedType> tempTypes(GetAllocator()->AllocateUncleared<ResolvedType>(typeSet->GetSize()), typeSet->GetSize());
                typeSet->GetKeys(tempTypes);

                for(int32 j = 0; j < tempTypes.size; j++) {
                    touchedTypes.TryAdd(tempTypes[j]);
                }

            }

            {
                TempAllocator::ScopedMarker temp(GetAllocator());
                CheckedArray<ResolvedType> tempTypes(GetAllocator()->AllocateUncleared<ResolvedType>(touchedTypes.GetSize()), touchedTypes.GetSize());

                int32 structCount = 0;
                int32 classCount = 0;

                for(int32 j = 0; j < tempTypes.size; j++) {
                    ResolvedType type = tempTypes[j];

                    if(type.IsPrimitive()) {
                        continue;
                    }

                    // todo -- optimize class type nullables to just a nullptr
                    if(type.IsNullable()) {
                        structCount++;
                        continue;
                    }

                    if(type.IsArray()) {
                        classCount++;
                        continue;
                    }

                    if(type.typeInfo->IsClass()) {
                        classCount++;
                    }

                    if(type.typeInfo->IsStruct()) {
                        structCount++;
                    }

                }

                // find the largest worker buffer and use that maybe

                // we output
                // built in primitives
                // forward declarations for classes
                // structs containing only builtins and classe members

                PodList<char> outputBuffer(KILOBYTES(16));

                for(int32 j = 0; j < tempTypes.size; j++) {
                    ResolvedType type = tempTypes[j];

                    if(type.IsPrimitive() || type.IsArray() || type.IsNullable()) {
                        continue;
                    }

                    if(type.typeInfo->IsClass()) {
                        // forward declaration
                        outputBuffer.AddRange("struct ", (int32)strlen("struct "));
//                        outputBuffer->AddRange(resolvedType.typeInfo->typeName, resolvedType.typeInfo->typeNameLength);
//                        outputBuffer->Add('_');
//                        WriteInt(resolvedType.typeInfo->genericArgumentCount);
//                        outputBuffer->Add('_');
//                        EncodePointerBase32(resolvedType.typeInfo, outputBuffer->Reserve(kBase32PointerLength));
                    }

                    if(type.typeInfo->IsStruct()) {
                        // if it contains other non built in, non primitive structs
                        // we need to dependency sort this
                        // otherwise we can emit it w/o issue
                    }


                }

                printf("structs: %d, classes: %d\n", structCount, classCount);

            }

            for (int32 i = 0; i < workerCount; i++) {
                PodList<CodeGenOutput>* outputs = perThreadCodeGenOutputs[i];
                for(int32 x = 0; x < outputs->size; x++) {
                    CodeGenOutput output = outputs->Get(x);
                    switch(output.outputType) {
                        case CodeGenOutputType::MethodSignature: {
                            puts(output.src);
                            break;
                        }
                        case CodeGenOutputType::MethodImplementation: {
                            puts(output.src);
                            break;
                        }
                        case CodeGenOutputType::Type:
                            break;
                    }
                }
            }

            // destructors are not automatically called
            for (int32 i = 0; i < workerCount; i++) {
                perThreadAllocators[i]->~LinearAllocator();
                perThreadOutputBuffers[i]->~PodList<char>();
                perThreadCodeGenOutputs[i]->~PodList<CodeGenOutput>();
                perThreadTypeSets[i]->~MSISet<ResolvedType>();
            }

            CheckedArray<VisitEntry> usedEntries(allocator->Allocate<VisitEntry>(globalLookup.GetSize()), globalLookup.GetSize());

            globalLookup.GetKeys(usedEntries);

            for (int32 i = 0; i < usedEntries.size; i++) {

            }

            compiler->CompleteCompilationRun();
        }

    };

    static void Compile(const char* packagePath) {

        Compiler compiler(0);

        compiler.jobSystem.Execute(CompileRootJob(&compiler));

    }

}