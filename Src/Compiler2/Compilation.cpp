
#include "../PrimitiveTypes.h"
#include "../Util/FixedCharSpan.h"
#include "../Parsing2/Tokenizer.h"
#include "../Util/Hash.h"
#include "../Parsing2/SyntaxNodes.h"
#include "../Allocation/ThreadLocalTemp.h"
#include "../Collections/FixedPodList.h"

namespace Alchemy::Compilation {

    struct SymbolTable {

    };

    struct InputFile {

        FixedCharSpan src;
        FixedCharSpan pathFromProjectRoot;
        uint64 lastWrite {};

        InputFile() = default;

        InputFile(FixedCharSpan pathFromProjectRoot, FixedCharSpan src, uint64 lastWrite)
            : src(src)
            , pathFromProjectRoot(pathFromProjectRoot)
            , lastWrite(lastWrite)
        {}

    };

    struct CompilationFile {

        LinearAllocator allocator;
        CheckedArray<LineColumn> lineInfos;
        CheckedArray<SyntaxToken> tokens;
        CompilationUnitSyntax * compilationUnit;
        FixedCharSpan pathFromRoot;
        FixedCharSpan src;
        uint64 lastEditTime;
        uint32 passId;

        CompilationFile(FixedCharSpan pathFromRoot, FixedCharSpan src, uint64 lastEditTime, int32 passId)
            : pathFromRoot(pathFromRoot)
            , src(src)
            , lastEditTime(lastEditTime)
            , allocator(GIGABYTES(4), MEGABYTES(1))
            , lineInfos()
            , tokens()
            , compilationUnit(nullptr)
            , passId(passId)
        {}

        int32 GetHashCode() {
            return MsiHash::FNV1a(pathFromRoot.ptr, pathFromRoot.size);
        }

    };

    struct Assembly {
        FixedCharSpan name;
        FixedCharSpan path;
        // ... options ...
    };

    struct CompilationUnit {};

    void ProcessCompilationUnit(CompilationFile * compilationFile, Diagnostics * diagnostics, CompilationUnitSyntax * compilationUnit) {

        // find our type signatures
        // update the symbol table

        TempAllocator * tempAllocator = GetThreadLocalAllocator();
        TempAllocator::ScopedMarker m(tempAllocator);


        // not sure if we care to multithread the symbol table updates
        // except for the first compile we only have a few files updating at once
        // parsing we can multi-thread easily

        // do we track symbols by type
        // does each type have its own symbol table?
        // or each namespace?
        // are fields/methods symbols?

        // seems like we have a symbol table per type
        // which we can choose if we want it to be a map or a flat list
        // easy ish to expand it just by controlling threshold via exponent

        struct Symbol {
            char* fullyQualifiedName;
        };

        // symbol table by type
        // multiple files can contribute to the same type symbol table via partial


        // when reloading a file we invalidate the symbol table for that file?
        // we need to track dependencies between methods

        struct TypeSymbol : Symbol {};

        struct MethodInfo : Symbol {
            // list of method & fields it depends on
            // after symbol table refresh
            // any deleted symbols get their shit redone
            // delete a field symbol if type changed or removed entirely
            // delete a method symbol if signature changed or removed entirely

            // track references to each symbol
            CheckedArray<Symbol*> dependsOnThis;
            CheckedArray<Symbol*> dependsOn;

        };

        struct TypeInfo {

            int32 sourceCount;
            int32 sourceCapacity;
            TypeSymbol * sources;
            // allocators? who owns the memory? do files track which symbols they contribute? if so how do we control duplicates?

            void AddMethod() {}

            void RemoveMethod() {}

            // each method needs c source & bytecode, who owns that?
            // maybe the declaring file? fine but then we need a write + copy scheme / a mutex
            // files have mutexes & allocators

            MethodInfo * FindMethod(FixedCharSpan fullyQualifiedName) {
                for(int32 i = 0; i < members.size; i++) {
                    if(members[i].type == method && members[i].fullyQualifiedName == fullyQualifiedName) {
                        return members[i];
                    }
                }
            }

        };

        // parse files that changed
        // for each parsed file figure out it's type declarations
        // resolve those types

        // we know what symbols a file contributes
        // we know what uses those symbols
        // we should be able to tell when updating a file if the symbols we used to have are the same or not
        // if they are the same we don't invalidate dependencies

        // symbolTable.GetSymbolId(fullyQualifiedName)
        // Symbol * symbol = symbolTable.GetSymbol(symbolId);
        // symbolTable.UpdateSymbol(fullyQualifiedName, impl);

        // we can store file local symbols to de-dupe them to know if we need to hit the main table w/ updates or not
        // or we can read them out before / during parsing for repeated files
        // probably also helps w/ single symbol table mutex, because the parse passes end at different rates
        // then the main table just points at the file infos for location info i guess

        // type::fieldOrPropertyOrMethodOrClass

        // how do we handle generics? ghost files? we don't re-parse per use but we do re-use the parse tree and dependencies
        // when that file changes we need to invalidate all uses of generics & re-validate if signatures changed

        // i think symbol granularity is correct
        // a symbol has usages and has depdencies
        // if a symbol was deleted or changed after a re-parse its usages get invalidated
        // if a symbol was unchanged, no work to do for it's usages

        // I'm not totally committed to partial methods/classes but they are super nice to have
        // do we need types to be resolved for symbols? they are pretty inter-dependant right?

        // are symbols just scoped names at this point? I dunno
        // for methods we need to know their parameter types & constraints etc, that's part of the signature
        // we should be able to first pass to resolve method parameters because we just need types for that?
        // so add type symbols but not method/field symbols yet



        // symboltable.findSymbolsFromFile(fileId);
        // just an msi multi map right?

        // symbol->Depend(symbolId, fileId, tokenRange, isWrite, isRead);
        // symbol->DependOn(symbolId);

        // we don't need to update dependencies during main compile pass
        // we don't need to do it all for one off compilations
        // we completely rebuild the map after each compilation i guess

        // types go first
        // record list of file's type declarations
        // foreach type declaration in file parse result
        // if not in previous declaration list, add it to decl list
        // foreach type in prev declaration list, if not in new list, remove it

        // add to symbol table



        for(int32 i = 0; i < compilationUnit->members->size; i++) {

            TempAllocator::ScopedMarker m2(tempAllocator);

            MemberDeclarationSyntax * member = compilationUnit->members->array[i];
            switch (member->GetKind()) {

                case SyntaxKind::ClassDeclaration: {

                    ClassDeclarationSyntax * classDeclaration = (ClassDeclarationSyntax*)member;
                    SyntaxToken identifier = classDeclaration->identifier;

                    // foreach method in class
                    // we can't resolve signatures yet but we can update the symbol table
                    break;
                }
                default: {
                    UNREACHABLE("");
                    break;
                }
            }
        }

    }

    struct TypeDeclaration {
        FixedCharSpan fullyQualifiedName;
        FixedCharSpan name;
    };

    FixedPodList<TypeDeclaration> declarations;

    void Process() {

        PodList<TypeDeclaration> list(16);

        // Parse(list)

        // rebuilding symbol table might work
        // but we'd need to keep the same symbol ids
        // and gc dead symbols

        for(int32 i = 0; i < parseResult.declarations.size; i++) {
            TypeDeclaration * info = parseResult.declarations[i];
            if(list.Contains(*info)) {
                list.Remove(info);
            }
            else {
                newList.Add(info);
            }
        }

        // new list
        // remove list
        // types don't have a change list, they either are present or not
        // changing constraints or base type results in a new symbol, they must be exactly identical to stay
        // once we remove dead types, we need to find generics and remove those

        // if a base type changes we need to re-scan extending type usages as well as generics

        // foreach dead type
        //   remove

        // foreach live type
            // if any dependencies are now dead, mark dead
            // which kind of tells me that I need a list of generic instantiations as well as usages

        // C# doesn't do incremental semantic analysis
        // does libraries once and caches
        // does min set of sema on demand for ide actions

        // feels like we can do better though right?
        // two levels: types and symbols

        // types need to know their dependencies (generic root, base type, interfaces, constraints, (attributes?) )
        // and inversely the dependencies need to know their dependents right?

        // when any of these change we need to bust the cache for the type

        // maybe file level incremental isn't so bad after all
        // - fixes dependency issue simply
        // - easy to bust dependents, easy to track too
        // - its methods that are most expensive to do sema for, we can still early out there w/ trickery
        // - it at least gives us an indication of where to look for changes, we can later decide updates aren't needed
        // - most of the time a file only has a small number of types in it

        // we just don't want to do sema on everything the file touches w/o reason
        // maybe its the best balance of reuse and dependency tracking though

        // we want to run method sema and codegen in parallel per method

        // 3 levels of sema:
        // - type hierarchies
        // - signatures & fields
        // - method bodies

        // get types in module

        // methods need to know when their signature is impacted by a type change

        // can we compare sema results to decide if we need to re-scan other files or not
        // naive approach is recursive busting, which feels pretty bad
        // when do we need to bust dependencies?
        // when type structure changed really (technically there's a public/private setup here but im not sure how to handle that nicely)
        // foreach changed file
        //      process it
        // we should know if we have one file changed or many, optimize for the single file case
        // if more than one file changed, do a full pass

        // in the single file changed case figure out if any types signatures changed
        // what if we were using a type that didn't exist and then add it?
        // we have a dependency we didn't know about
        // i guess we can track unresolved types
        // and we know when types were added somewhere, so we can then re-scan that file w/ the added type

        // if so then we need to scan all the files that use this file and bust them
        // question is if this is recursive or not
        // It's possible to have two files that depend on each other
        // I think the most reasonable thing we can do is check if a file's external api changed

        // type structure
        // fields/properties/methods/delegates/enums/interfaces/etc

        // if that changed, re-do sema for all impacted files and I guess hope we're fast enough to handle it smoothly
        // this is marginally better than re-doing full sema passes w/o caching
        // for ide actions like find usages do we need a full pass?

        // we want to re-use codegen as much as possible as well
        // we know we've finished a sema pass for sure before we handle any ide requests like goto reference

        // where do we keep sema data?
        // probably in the file allocator w/ a copy-in scheme for threading

        // do we really want a db of find-usages?
        // or can we just serve that by traversing our sema graph?
        // we know the file dependencies where we need to look
        // but if the symbol is a virtual method or something
        // we'd need to additionally scan base types etc

        // doing lookups on demand makes sense, why store everything if we only need to serve once?
        // inlay hints are another issue, but we don't 100% have to support them
        // after sema we can definitely track references if we want to

        // semantic model for a file can be reused if no dependencies changed
        // find references cannot be probably
        // go to impl etc

    }

    struct ht {
        int32 len;
    };

    struct Compiler {

        FixedCharSpan projectRoot;

        int32 exponent;
        CompilationFile** ht;
        int32 * indices;

        explicit Compiler(FixedCharSpan projectRoot)
            : projectRoot(projectRoot)
        {}

        void MapLookup() {

        }

        PodList<CompilationFile*> files;

        void LoadFiles(CheckedArray<InputFile> inputFiles) {

            PodList<CompilationFile*> newFiles;
            PodList<CompilationFile*> changedFiles;
            PodList<CompilationFile*> removedFiles;

            for(int32 idx = 0; idx < inputFiles.size; idx++) {

                InputFile * inputFile = &inputFiles[idx];

                // todo -- maybe normalize slashes to / or expect caller to do that

                int32 idx = MapLookup(inputFile);

                MapAdd(inputFile);
                MapUpdate(inputFile);

                if(isNewFile) {
                    newList.Add(compilationFile);
                }
                else if(isChangedFile) {
                    changedList.Add(compilationFile);
                }

                if(ht[idx] != nullptr) {
                    // we had it already & it didn't change
                }
                else {
                    // this is a new file or a changed on e
                }

                for (int32 i = hash;;) {

                    i = MsiHash::Lookup32(hash, exponent, i);

                    if(ht[i] == nullptr) {

                        if(mapSize + 1 == mapThreshold) {
                            // rehash the whole map
                        }

                        // may need to rehash here
                        // empty, insert here
                        // get from free list or append
                        // or just make one, whatever
                        CompilationFile * file = MallocateTyped(CompilationFile, 1);
                        ht[i] = new (file) CompilationFile(inputFile->pathFromProjectRoot, inputFile->src, inputFile->lastWrite);
                        break;
                    }
                    else {
                        // if path is equal we have a match
                        // otherwise keep looking
                    }

                    if (!t->ht[i]) {
                        // empty, insert here
                        if ((uint32)t->len+1 == (uint32)1<<EXP) {
                            return 0;  // out of memory
                        }
                        t->len++;
                        t->ht[i] = key;
                        return key;
                    } else if (!strcmp(t->ht[i], key)) {
                        // found, return canonical instance
                        return t->ht[i];
                    }
                }
                // see if file is new


            }

        }

    };

    // first pass compiles everything

    void LoadFiles() {

    }

}