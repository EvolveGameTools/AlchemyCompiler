#pragma once

#include "../Allocation/LinearAllocator.h"
#include "./IntrospectorBase.h"
#include "../Collections/FixedPodList.h"
#include "SourceFileInfo.h"
#include "Expression.h"
#include "TypeResolutionMap.h"

namespace Alchemy::Compilation {

    struct Introspector : IntrospectorBase {

        // temp allocated w/o fixed sizes

        Introspector(SourceFileInfo* fileInfo, TypeResolutionMap* typeResolver)
            : IntrospectorBase(fileInfo, typeResolver) {}

        void Introspect() {
            for (int32 i = 0; i < file->declaredTypes.size; i++) {
                IntrospectType(file->declaredTypes[i]);
            }
        }

        void IntrospectType(TypeInfo* pTypeInfo) {
            typeInfo = pTypeInfo;

            for (int32 i = 0; i < typeInfo->methodCount; i++) {
                MethodInfo* method = &typeInfo->methods[i];
                VisitMethodDeclarationSyntax(method, method->syntaxNode);
            }

        }

        // add init statement = insert into statement list + offset?
        // scope -> variables etc
        // basic_block -> no control flow except out
        // section has an init and main body
        // each are independent blocks with their own variables etc
        // scopes are for user variables i guess
        // scope contains multiple sections
        // section is basically basic block i guess? or a pair of them

        void IntrospectMethod(SourceFileInfo* fileInfo, MethodInfo* methodInfo) {
//            ts_LocalExpressionBase = allocator->GetBase();
//            MethodDeclarationSyntax* methodDeclarationSyntax = methodInfo->syntaxNode;
//            scopeStack->size = 0;
//            PushScope();
//
//            if (methodInfo->isDefaultParameterOverload) {
//
//            }
//            else {
//                for (int32 i = 0; i < methodInfo->parameterCount; i++) {
//                    ParameterInfo* parameterInfo = &methodInfo->parameters[i];
//                }
//            }
//            PopScope();
        }

        void VisitStatement(StatementSyntax* pSyntax) {
            switch (pSyntax->GetKind()) {

                case SyntaxKind::EmptyStatement: {
                    VisitEmptyStatement((EmptyStatementSyntax*) pSyntax);
                }
                default: {
                    NOT_IMPLEMENTED(SyntaxKindToString(pSyntax->GetKind()));
                    break;
                }
            }
        }

        void VisitEmptyStatement(EmptyStatementSyntax* emptyStatement) {
            return;
        }

        void VisitBlockSyntax(BlockSyntax* blockSyntax) {
            if (blockSyntax == nullptr) {
                return;
            }

            PushScope();

            for (int32 i = 0; i < blockSyntax->statements->size; i++) {
                VisitStatement(blockSyntax->statements->array[i]);
            }

            PopScope();

        }

        void VisitArrowExpressionClauseSyntax(ArrowExpressionClauseSyntax* arrowExpressionClauseSyntax) {

        }

        void VisitMethodDeclarationSyntax(MethodInfo* methodInfo, MethodDeclarationSyntax* methodDeclarationSyntax) {

            Setup();

            targetReturnType = methodInfo->returnType;

            PushScope();

            // out parameters will need to be definitely assigned
            for (int32 i = 0; i < methodInfo->parameterCount; i++) {
                AddParameter(&methodInfo->parameters[i]);
            }

            if (methodDeclarationSyntax->body != nullptr) {
                VisitBlockSyntax(methodDeclarationSyntax->body);
            }
            else if (methodDeclarationSyntax->expressionBody != nullptr) {
                VisitArrowExpressionClauseSyntax(methodDeclarationSyntax->expressionBody);
            }

            PopScope();

        }

    };

}
