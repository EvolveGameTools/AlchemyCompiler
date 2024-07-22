#pragma once

#include "../../JobSystem/Job.h"
#include "../../JobSystem/JobSystem.h"
#include "../FullyQualifiedName.h"
#include "../ResolvedType.h"
#include "../TypeResolver.h"
#include "../MemberInfo.h"
#include "../../PrimitiveTypes.h"
#include "../../Allocation/ThreadLocalTemp.h"
#include "../Expression.h"

namespace Alchemy::Compilation {


    struct ClosureDefinition {
        // fields + assignment expressions if applicable
        // method body pointer
        // maybe signature, might be encoded elsewhere
    };

    struct Scope;

    struct LocalValue {

        FixedCharSpan name;
        ResolvedType resolvedType;
        BlitPointerField(Scope, DeclaringScope);
        BlitPointerField(Expression, Expression);

    };

    struct Scope {

        SyntaxBase* pSyntaxNode {};
        BlitPointerField(Scope, Parent);
        BlitPointerField(Scope, FirstChild);
        BlitPointerField(Scope, NextSibling);
        BlitPointerField(Scope, PrevSibling);
        BlitPointerField(Scope, LastChild);
        int32 depth {};
        CheckedArray<LocalValue> valueDeclarations;

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
                Scope* nextSibling = GetLastChild();
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

    struct Introspector {

        LinearAllocator* allocator;

        // temp allocated w/o fixed sizes
        FixedPodList<Scope*>* scopeStack;
        FixedPodList<Scope*>* scopeList;
        FixedPodList<Range32> declarationsByScopeDepth;
        FixedPodList<LocalValue*> valueBuffer;
        int32 internalVarId;
        Diagnostics * diagnostics;
        SourceFileInfo * file;

        template<class T, typename... Args>
        T* CreateExpression(Args &&... args) {
            T* retn = (T*) allocator->AllocateUncleared<T>(1);
            new(retn) T(std::forward<Args>(args)...);
            assert(retn->kind != ExpressionKind::Invalid);
            return retn;
        }

        void PushScope() {
            Scope* scope = allocator->New<Scope>();
            scope->SetParent(scopeStack->Peek());
            scope->GetParent()->AddChild(scope);
            scopeStack->Push(scope);
            scopeList->Add(scope);
            declarationsByScopeDepth[scopeStack->size - 1] = Range32(valueBuffer.size, 0);
        }

        void PopScope() {
            declarationsByScopeDepth[scopeStack->size - 1] = Range32(0, 0);
            scopeStack->Pop();
        }

        void AddError(ErrorCode errorCode, FixedCharSpan sourceSpan, FixedCharSpan message) {
            // todo
        }

        LocalValue* AddLocal(FixedCharSpan name, SyntaxBase * syntaxBase) {

            for (int32 i = valueBuffer.size - 1; i >= 0; i--) {
                if (name == valueBuffer[i]->name) {
                    AddError(ErrorCode::ERR_DuplicateIdentifierInScope, file->GetText(syntaxBase), name);
                    break;
                }
            }

            LocalValue* value = allocator->New<LocalValue>();
            declarationsByScopeDepth[scopeStack->size - 1].count++;
            valueBuffer.Add(value);
            return value;

        }

        LocalValue* AddInternalLocal(FixedCharSpan name) {
            LocalValue* value = allocator->New<LocalValue>();
            declarationsByScopeDepth[scopeStack->size - 1].count++;
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

        LocalValue* ResolveIdentifier(FixedCharSpan identifier) {
            Scope* currentScope = scopeStack->Peek();

            Range32 values = declarationsByScopeDepth[currentScope->depth];

            int32 end = values.End();
            for (int32 i = values.start; i < end; i++) {
                LocalValue * value = valueBuffer.Get(i);
                // got it, it's local
                if (value->name == identifier) {
                    return value;
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
                        return value;
                    }
                }
                scope = scope->GetParent();
            }

            // search fields
            // search properties
            // search methods

            return nullptr;

        }

        void IntrospectMethod(SourceFileInfo* fileInfo, MethodInfo* methodInfo) {
            ts_LocalExpressionBase = allocator->GetBase();
            MethodDeclarationSyntax* methodDeclarationSyntax = methodInfo->syntaxNode;
            scopeStack->size = 0;
            PushScope();

            if (methodInfo->isDefaultParameterOverload) {

            }
            else {
                for (int32 i = 0; i < methodInfo->parameterCount; i++) {
                    ParameterInfo* parameterInfo = &methodInfo->parameters[i];
                }
            }
            PopScope();
        }

    };

    struct IntrospectScopesJob : Jobs::IJob {

        TypeResolutionMap* resolutionMap;
        CheckedArray<SourceFileInfo*> files;

        IntrospectScopesJob(CheckedArray<SourceFileInfo*> files, TypeResolutionMap* resolutionMap)
            : files(files)
            , resolutionMap(resolutionMap) {}

        ResolvedType returnType;
        Diagnostics* diagnostics;
        SourceFileInfo* file;

        // PodList<Scope> scopeStack;
        // PodList<BlockExpression> blockStack;



        struct IntrospectionInfo {
            uint8* buffer;
            size_t bufferSize;
            // diagnostics
            // referenced type infos // needed for typeof() etc
            // referenced member infos // probably needed for indexing, won't store when compiling just when doing lsp?
        };

        // this pass does all desugaring, definite assignment analys
        Expression* Visit(ExpressionSyntax* expressionSyntax) {

            switch (expressionSyntax->GetKind()) {
                case SyntaxKind::LocalDeclarationStatement: {

                    break;
                }
                case SyntaxKind::InvocationExpression: {
                    InvocationExpressionSyntax* invocationExpressionSyntax = (InvocationExpressionSyntax*) expressionSyntax;
                    Expression* target = Visit(invocationExpressionSyntax->expression);

                    for (int32 i = 0; i < invocationExpressionSyntax->argumentList->arguments->itemCount; i++) {
                        ArgumentSyntax* argument = invocationExpressionSyntax->argumentList->arguments->items[i];
//                        ArgumentExpression arg = Visit(argument);
                    }

                    // ResolveMethodToCall()
                    // enqueue method call creation if it is generic or if it's instance type is generic (and doesn't already exist)
                    // we could lock the resolver before doing this so it becomes reads only
                    // and hold a separate resolver for dynamic types w/ locks
                    // so we don't contend there
                    // also offers a solution for nuking ephemeral types, we literally don't store them

                    // expression system works on baseptr + offet w/ methods for accessing

                    break;
                }
            }

//            return Expression();
        }

        void Visit(BlockSyntax* pSyntax) {

            for (int32 i = 0; i < pSyntax->statements->size; i++) {
                StatementSyntax* statementSyntax = pSyntax->statements->array[i];
                switch (statementSyntax->GetKind()) {
                    case SyntaxKind::EmptyStatement: {
                        break;
                    }
                    case SyntaxKind::ReturnStatement: {
                        ReturnStatementSyntax* returnStatementSyntax = (ReturnStatementSyntax*) statementSyntax;

                        if (returnStatementSyntax->expressionSyntax == nullptr && returnType.typeInfo != resolutionMap->voidType) {
                            diagnostics->AddError(ErrorCode::ERR_ExpectedReturnType, file->GetText(returnStatementSyntax));
                            break;
                        }
                        else if (returnStatementSyntax->expressionSyntax == nullptr) {
                            // handle return void
                        }
                        else {
                            // handle typed return
                            // Expression retnExpression = VisitExpression(returnStatementSyntax->expressionSyntax);
                            // tree->CreateNode<ReturnStatementExpression>(retnExpression);
                        }

                        break;
                    }
                    case SyntaxKind::ExpressionStatement: {
                        ExpressionStatementSyntax* expressionStatementSyntax = (ExpressionStatementSyntax*) statementSyntax;
                        Visit(expressionStatementSyntax->expression);
                        break;
                    }
                    default: {
                        NOT_IMPLEMENTED(SyntaxKindToString(statementSyntax->GetKind()));
                        break;
                    }
                }
            }

        }

        void Execute() override {

            MethodInfo* methodInfo;

            if (methodInfo->syntaxNode->expressionBody != nullptr) {

            }
            else if (methodInfo->syntaxNode->body != nullptr) {
                Visit(methodInfo->syntaxNode->body);
            }
        }

    };

}