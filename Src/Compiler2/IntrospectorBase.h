#pragma once

namespace Alchemy::Compilation {


    struct ClosureDefinition {
        // fields + assignment expressions if applicable
        // method body pointer
        // maybe signature, might be encoded elsewhere
    };

    struct GenericMethodHandler {
        std::mutex* schedulingMutex;
        std::mutex* allocatorMutex;
        LinearAllocator* allocator;
        PagedList<TypeInfo*> createdTypes;
        PagedList<MethodInfo*> createdMethods;
    };

    struct Scope;

    struct LocalValue {

        FixedCharSpan name;
        ResolvedType resolvedType;
        BlitPointerField(Scope, DeclaringScope);
        BlitPointerField(Expression, Expression);

    };

    struct Section {
        CheckedArray<LocalValue> variables;
        CheckedArray<Expression*> statements;
        CheckedArray<LocalValue*> initVariables;
        CheckedArray<Expression*> initStatements;
    };

    struct Scope {

        SyntaxBase* pSyntaxNode {};
        BlitPointerField(Scope, Parent);
        BlitPointerField(Scope, FirstChild);
        BlitPointerField(Scope, NextSibling);
        BlitPointerField(Scope, PrevSibling);
        BlitPointerField(Scope, LastChild);
        int32 depth {};

        void AddChild(Scope* scope) {

            assert(scope->GetNextSibling() == nullptr);
            assert(scope->GetPrevSibling() == nullptr);
            assert(scope->GetFirstChild() == nullptr);
            assert(scope->GetLastChild() == nullptr);
            assert(scope->GetParent() == nullptr);

            scope->depth = depth + 1;

            if (GetFirstChild() == nullptr) {
                assert(GetLastChild() == nullptr);
                SetFirstChild(scope);
                SetLastChild(scope);
            }
            else {
                Scope * nextSibling = GetLastChild();
                scope->SetPrevSibling(nextSibling);
                nextSibling->SetNextSibling(scope);
                SetLastChild(scope);
            }
            scope->SetParent(this);
        }

    };

    struct IntrospectionDiagnostic {
        Diagnostic diagnostic;
        BlitPointerField(Diagnostic, next);
    };

    // scope is clearly a { }
    // block is subsection of a scope
    // we make a new block when we hit some piece of control flow within a scope
    // or when we come out of it

    // if(xyz) return; block or scope?

    struct Range32 {

        int32 start {};
        int32 count {};

        Range32(int32 start, int32 count)
            : start(start)
            , count(count) {}

        int32 End() {
            return start + count;
        }

    };


    struct IntrospectorBase {

        LinearAllocator* allocator;
        TypeResolutionMap* typeResolver;
        SourceFileInfo* file;
        FixedPodList<Scope*> scopeStack {};
        FixedPodList<Scope*> scopeList {};
        FixedPodList<Range32> declarationsByScopeDepth;
        FixedPodList<LocalValue*> valueBuffer;
        int32 internalVarId {};
        TypeInfo* typeInfo {};
        Expression* thisInstance {};

        ResolvedType targetReturnType;

        explicit IntrospectorBase(SourceFileInfo* fileInfo, TypeResolutionMap* typeResolver)
            : file(fileInfo)
            , typeResolver(typeResolver)
            , allocator(&fileInfo->allocator)
            {}

        void Setup() {
            internalVarId = 0;
            scopeStack = FixedPodList<Scope*>(GetThreadLocalAllocator()->AllocateUncleared<Scope*>(4096), 4096);
        }


        PodList<Expression*> statementBuffer;

        Section * currentSection;

        void PushSection() {
            // finalizes the last section if present


        }

        void PopSection() {
            // makes a new section
        }

        void AddStatement(Expression * expression) {}

        void AddInitStatement(Expression * expression) {}

        void AddParameter(ParameterInfo * parameterInfo) {
            Scope* rootScope = scopeStack.Peek();
            ParameterExpression * parameterExpression = CreateExpression<ParameterExpression>(parameterInfo);
            // rootScope->AddLocal(parameterExpression);

        }

        template<class T, typename... Args>
        T* CreateExpression(Args&& ... args) {
            T* retn = (T*) allocator->AllocateUncleared<T>(1);
            new(retn) T(std::forward<Args>(args)...);
            assert(retn->kind != ExpressionKind::Invalid);
            return retn;
        }

        void PushScope() {
            Scope* scope = allocator->New<Scope>();
            scope->SetParent(scopeStack.Peek());
            scope->GetParent()->AddChild(scope);
            scopeStack.Push(scope);
            scopeList.Add(scope);
            declarationsByScopeDepth[scopeStack.size - 1] = Range32(valueBuffer.size, 0);
        }

        void PopScope() {
            declarationsByScopeDepth[scopeStack.size - 1] = Range32(0, 0);
            scopeStack.Pop();
        }

        void AddError(ErrorCode errorCode, FixedCharSpan sourceSpan, FixedCharSpan message) {
            // todo
        }

        LocalValue* AddLocal(FixedCharSpan name, SyntaxBase* syntaxBase) {

            for (int32 i = valueBuffer.size - 1; i >= 0; i--) {
                if (name == valueBuffer[i]->name) {
                    AddError(ErrorCode::ERR_DuplicateIdentifierInScope, file->GetText(syntaxBase), name);
                    break;
                }
            }

            LocalValue* value = allocator->New<LocalValue>();
            declarationsByScopeDepth[scopeStack.size - 1].count++;
            valueBuffer.Add(value);
            return value;

        }

        LocalValue* AddInternalLocal(FixedCharSpan name) {
            LocalValue* value = allocator->New<LocalValue>();
            declarationsByScopeDepth[scopeStack.size - 1].count++;
            valueBuffer.Add(value);
            char* nameBuffer = allocator->AllocateUncleared<char>(name.size + 4);
            char* ptr = nameBuffer;
            ptr[0] = '_';
            ptr++;
            memcpy(ptr, name.ptr, name.size);
            ptr += name.size;
            ptr[0] = '_';
            ptr += IntToAscii(internalVarId, ptr);
            value->name = FixedCharSpan(nameBuffer, ptr - nameBuffer);
            return value;
        }

        Expression* ResolveIdentifier(FixedCharSpan identifier) {
            Scope* currentScope = scopeStack.Peek();

            Range32 values = declarationsByScopeDepth[currentScope->depth];

            int32 end = values.End();
            for (int32 i = values.start; i < end; i++) {
                LocalValue* value = valueBuffer.Get(i);
                // got it, it's local
                if (value->name == identifier) {
                    return value->GetExpression();
                }
            }

            // we may need to mark locals with a variable type: loop iterator, yield, etc
            // reserve a slot per scope for closure instance creation, skip if not used

            // wasn't in local scope, try parent scopes and watch for closure boundaries
            // if we pass a closure boundary we need to promote the local to a closure

            // we probably need to know when we hit a loop scope
            // we need to know when we hit a yield scope
            // we need to know when we hit a lambda

            Scope* scope = currentScope->GetParent();
            while (scope != nullptr) {
                values = declarationsByScopeDepth[scope->depth];
                end = values.End();
                for (int32 i = values.start; i < end; i++) {
                    LocalValue* value = valueBuffer.Get(i);
                    if (value->name == identifier) {
                        return value->GetExpression();
                    }
                }
                scope = scope->GetParent();
            }

            // todo -- profile here, this is likely kind of a hot code section & does a lot of searching / pointer tracking

            TypeInfo* ptr = typeInfo;

            bool isStatic = thisInstance == nullptr;

            while (ptr != nullptr) {

                for (int32 i = 0; i < ptr->fieldCount; i++) {

                    FieldInfo* fieldInfo = &ptr->fields[i];

                    if (fieldInfo->identifier != identifier) {
                        continue;
                    }

                    // todo -- const
                    if ((fieldInfo->modifiers & FieldModifiers::Static) == 0) {
                        if (isStatic) {
                            AddError(ErrorCode::ERR_InstanceFieldAccessInStaticContext, identifier, FixedCharSpan());
                        }
                        return CreateExpression<FieldAccessExpression>(thisInstance, fieldInfo);
                    }
                    else {
                        return CreateExpression<FieldAccessExpression>(nullptr, fieldInfo);
                    }
                }

                if (!isStatic) {
                    ptr = ptr->GetBaseClass();
                }

            }

            ptr = typeInfo;
            while (ptr != nullptr) {

                for (int32 i = 0; i < ptr->propertyCount; i++) {
                    PropertyInfo* propertyInfo = &ptr->properties[i];
                    if (propertyInfo->name != identifier) {
                        continue;
                    }

                    if ((propertyInfo->modifiers & PropertyModifiers::Static) == 0) {
                        if (isStatic) {
                            AddError(ErrorCode::ERR_InstanceFieldAccessInStaticContext, identifier, FixedCharSpan());
                        }
                        return CreateExpression<PropertyAccessExpression>(thisInstance, propertyInfo);
                    }
                    else {
                        return CreateExpression<PropertyAccessExpression>(nullptr, propertyInfo);
                    }

                }

                if (!isStatic) {
                    ptr = ptr->GetBaseClass();
                }

            }
            // search properties
            // search methods

            return nullptr;

        }
    };

}