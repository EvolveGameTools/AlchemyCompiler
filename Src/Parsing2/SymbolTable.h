#pragma once

#include "../Util/Hash.h"

namespace Alchemy::Compilation {

    struct TypeInfo {
        TypeInfo* elementType;
        TypeInfo** baseTypes;
        FixedCharSpan fullyQualifiedName;
    };

    struct ResolvedTypeInfo {

        TypeInfo* typeInfo;

    };

    struct SymbolTable {

        std::mutex mtx;

        // how do we organize types?
        // between compilation actions we schedule a job to clean up / merge the type tables

        PodList<TypeInfo> typeInfos;

        struct TypeId {
            int32 id;
        };

        struct Type {
            CheckedArray<TypeId> dependencies;
        };

        struct Node;


        struct Symbol {

            uint8 symbolType;
            TypeId typeId;

            Node* definingNode;

            // type = field
            // typeId id;
            // origin id;
            // visibility
            // type flags (const, readonly, ref, out, static) maybe
            // file id, token range?
            // fieldId id; // unique id or what?
            // dependents
            // Node* definingNode
        };

        // update parse
        // update symbols

        // foreach type in parsed files
        // see if fields were added
        // see if fields were removed
        // see if fields were changed
        // see if method signatures were changed
        // see if types were added/removed

        // while doing sema on a method we add references to symbols
        // we don't need the updates to write until we're done

        struct MethodInfo {
        };

        // for each changed method signature
        // find it's dependencies
        // writes are solved / easy

        // removal is trickier
        // whenever a method body changes
        // we have a few cases
        // method body changed & added a reference
        // method body changed & removed a reference
        // method was entirely deleted (also happens on signature changed)

        // we only really need to synchronize dependencies between updates
        // ie after returning an lsp response or after compilation

        // where do we store these?
        // paged buffers of bytes i guess
        // we may need a generation or something to figure out if still valid

        struct SymbolTableUpdate {
            bool isRemoval;
            Symbol* symbol;

        };

        void SemanticAnalyze(MethodInfo* methodInfo) {
            // hashset(symbol)
            // visit(methodInfo)
            // hashset.Add(methodSymbol)
            // visit fieldAccess(fieldInfo)
            // hashset.Add(fieldSymbol)
            // CheckedArray<Symbol> dependencies = hashset.ToList(allocator);
            // we could diff this w/ previous dependencies (which we store anyway)
            // can hold adds & removes
            // lock { symbolTableWrites.Add(methodInfo, dependencies); }

        }

        ResolvedTypeInfo* ResolveType(FixedCharSpan typeName) {
            // when compiling we only care about what we depend on right?
            // do we need the inverse? ie what depends on us?

            // change comes in
            // we need to do analysis
            // for each type that changed in structure
            // notify all dependents
            // what granularity?
            // fields? methods? full types?
            return nullptr;
        }

    };

}