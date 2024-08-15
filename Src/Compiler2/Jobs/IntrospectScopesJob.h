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

    // introspect by type maybe makes some sense to reduce repeated work
    // introspect by method would mean we need better storage solutions for partial data right?
    // introspect by type lets me add fields dynamically w/o issue which is important for
    // widgets but maybe not for regular classes / structs
    // introspect by type maybe forces a full instantiate for generic types instead of just the used methods
    // but also enables reflection?
    // is it easier? yes.
    // we can introspect everything and only output code for used methods, likely a better compromise

    // we probably want to emit expressions and then lower them based on our output target
    // maybe we need partial lowering just to get to

    // we want a separate object for handling mutex locked generic types & generic methods since we want those to
    // live in a different allocator.

    // we want expression trees for all methods at the end of this pass
    // we should also know which methods are ever called. (not sure about interface methods & overrides yet)



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
                default:
                    UNREACHABLE("Visit");
                    break;

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

                    MethodInfo * methodInfo = nullptr;

                    // when we hit a generic type we enqueue it
                    // when we hit a generic method, we enqueue it
                    // we just forget about them when done?
                    // we have our static generic builder
                    // we have our dynamic generic builder

                    // i don't want to ref count anything
                    // how badly do we need to actually trace everything?
                    // can't we just go through entry point methods & fan out
                    // then while compiling we are done when no jobs are left in the queues
                    // we can decide if we generate all methods or touched methods
                    // we can easily to lsp on just one file
                    // or choose how we enqueue it right?
                    // which probably means we don't care about generics that much

                    bool expected = false;
                    if (methodInfo->isEnqueued.compare_exchange_strong(expected, true)) {
                        // std::unique_lock<std::mutex> scheduleLock();
                        // probably one point of allocation here
                        // Schedule();
                    }

                    // ResolveMethodToCall()
                    // enqueue method call creation if it is generic or if it's instance type is generic (and doesn't already exist)
                    // we could lock the resolver before doing this so it becomes reads only
                    // and hold a separate resolver for dynamic types w/ locks
                    // so we don't contend there
                    // also offers a solution for nuking ephemeral types, we literally don't store them

                    // expression system works on baseptr + offset w/ methods for accessing

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



    };

}