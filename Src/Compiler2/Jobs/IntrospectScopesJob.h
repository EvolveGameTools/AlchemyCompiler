#pragma once

#include "../../JobSystem/Job.h"
#include "../../JobSystem/JobSystem.h"
#include "../FullyQualifiedName.h"
#include "../ResolvedType.h"
#include "../TypeResolver.h"
#include "../MemberInfo.h"
#include "../../PrimitiveTypes.h"
#include "../../Allocation/ThreadLocalTemp.h"

namespace Alchemy::Compilation {

    thread_local uint8* allocPtr;

    struct GenericMethodHandler {
        std::mutex * schedulingMutex;
        std::mutex * allocatorMutex;
        LinearAllocator * allocator;
        PagedList<TypeInfo*> createdTypes;
        PagedList<MethodInfo*> createdMethods;
    };

    struct IntrospectScopesJob : Jobs::IJob {

        TypeResolutionMap* resolutionMap;
        CheckedArray<SourceFileInfo*> files;

        ResolvedType returnType;
        Diagnostics * diagnostics;
        SourceFileInfo * file;
        std::mutex * schedulingMutex;
        GenericMethodHandler* genericMethodHandler;

        IntrospectScopesJob(CheckedArray<SourceFileInfo*> files, TypeResolutionMap* resolutionMap)
            : files(files)
            , resolutionMap(resolutionMap) {}


        void Execute(int32 index) override {

            MethodInfo* methodInfo;

            if (methodInfo->syntaxNode->expressionBody != nullptr) {

            }
            else if (methodInfo->syntaxNode->body != nullptr) {
                Visit(methodInfo->syntaxNode->body);
            }
        }

        // PodList<Scope> scopeStack;
        // PodList<BlockExpression> blockStack;

        #define BlitPointerField(x, y) int32 y##_offset {}; \
        x * Get##y() {                                      \
            return (x*)(allocPtr + y##_offset);             \
        }                                                   \
        void Set##y(x * value) {                            \
            if(value == nullptr) y##_offset = 0;            \
            else y##_offset = (uint8*)value - allocPtr;     \
        }                                                   \

        struct Expression {

            BlitPointerField(Expression, Name);
            LineColumn location;

            explicit Expression(Expression * a, LineColumn location) {
                SetName(a);
            }

        };

        struct ClosureDefinition {
            // fields + assignment expressions if applicable
            // method body pointer
            // maybe signature, might be encoded elsewhere
        };

        struct Scope {

            SyntaxBase * pSyntaxNode {};
            BlitPointerField(Scope, Parent);
            BlitPointerField(Scope, FirstChild);
            BlitPointerField(Scope, NextSibling);
            BlitPointerField(Scope, LastChild);

        };

        struct IntrospectionDiagnostic {
            Diagnostic diagnostic;
            BlitPointerField(Diagnostic, next);
        };

        struct IntrospectionInfo {
            uint8* buffer;
            size_t bufferSize;
            // diagnostics
            // referenced type infos // needed for typeof() etc
            // referenced member infos // probably needed for indexing, won't store when compiling just when doing lsp?
        };

        // this pass does all desugaring, definite assignment

        Expression* Visit(ResolvedType * targetType, ExpressionSyntax * expressionSyntax) {

            switch(expressionSyntax->GetKind()) {
                case SyntaxKind::LocalDeclarationStatement: {
                    break;
                }
                case SyntaxKind::InvocationExpression: {
                    InvocationExpressionSyntax * invocationExpressionSyntax = (InvocationExpressionSyntax*)expressionSyntax;
                    Expression * target = Visit(nullptr, invocationExpressionSyntax->expression);

                    for(int32 i = 0; i < invocationExpressionSyntax->argumentList->arguments->itemCount; i++) {
                        ArgumentSyntax * argument = invocationExpressionSyntax->argumentList->arguments->items[i];
                    }

                    MethodInfo * methodInfo = nullptr;

                    bool expected = false;
                    if (methodInfo->isEnqueued.compare_exchange_strong(expected, true)) {
                        std::unique_lock<std::mutex> scheduleLock(*schedulingMutex);
                        // probably one point of allocation here
                        // Schedule();
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

            return nullptr;
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
                        else if(returnStatementSyntax->expressionSyntax == nullptr) {
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
                        ExpressionStatementSyntax * expressionStatementSyntax = (ExpressionStatementSyntax*)statementSyntax;
                        Visit(nullptr, expressionStatementSyntax->expression);
                        break;
                    }
                    default: {
                        NOT_IMPLEMENTED(SyntaxKindToString(statementSyntax->GetKind()));
                        break;
                    }
                }
            }

        }



    };

}