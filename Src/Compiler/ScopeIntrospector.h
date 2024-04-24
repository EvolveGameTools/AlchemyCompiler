#pragma once

#include "./PooledBlockAllocator.h"
#include "./TypeContext.h"
#include "./TypeUtil.h"

#define Unreachable assert(false && "unreachable")
#define NotImplemented assert(false && "not yet implemented")

namespace Alchemy::Compilation {

    struct ScopeIntrospector {

        FileInfo* fileInfo;
        Diagnostics* diagnostics;
        ExpressionTree* et;
        TypeResolver typeResolver;
        TypeContext* typeContext;
        TempAllocator* tempAllocator;

        // todo -- try not to use podlist
        PodList<Scope*> scopeStack;
        GenericTypeCache * genericTypeCache;

        ScopeIntrospector(GenericTypeCache * genericTypeCache, MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap, CheckedArray<TypeInfo*> builtInTypes, Diagnostics* diagnostics, TempAllocator * tempAllocator)
            : diagnostics(diagnostics)
            , tempAllocator(tempAllocator)
            , genericTypeCache(genericTypeCache)
            , typeResolver(nullptr, genericTypeCache, resolveMap, diagnostics, builtInTypes, CheckedArray<TypeInfo*>(), tempAllocator) {}

        template<class T>
        T* GetNode(NodeIndex<T> nodeIndex) {
            if (nodeIndex.index == 0) {
                return nullptr;
            }
            return (T*) fileInfo->parseResult.psiNodes.GetPointer(nodeIndex.index);
        }

        template<class T>
        int32 CountExpressions(NodeIndex<T> nodeIndex) {
            T* ptr = GetNode(nodeIndex);

            int32 cnt = 0;
            while (ptr != nullptr) {
                cnt++;
                ptr = GetNode(ptr->next);
            }

            return cnt;
        }

        void Introspect(DeclaredType declaredType) {

            typeContext = declaredType.typeContext;
            TypeInfo* typeInfo = declaredType.typeInfo;
            fileInfo = typeInfo->declaringFile;

            typeResolver.fileInfo = fileInfo;

            et = tempAllocator->New<ExpressionTree>(fileInfo, declaredType, diagnostics, &fileInfo->poolingAllocator);

            for (int32 f = 0; f < typeInfo->fieldCount; f++) {
                typeContext->fields[f] = et->DefineField(&typeInfo->fields[f]);
            }

            for (int32 p = 0; p < typeInfo->propertyCount; p++) {
                et->DefineProperty(&typeInfo->properties[p]);
            }

            // this is safe because we're assigning the same value to methodinfo as we did on allocation
            for (int32 m = 0; m < typeInfo->methodCount; m++) {
                typeContext->methods[m] = et->DefineMethod(&typeInfo->methods[m]);
            }

            scopeStack.Push(typeContext->rootScope);

            for (int32 m = 0; m < typeContext->methodCount; m++) {
                IntrospectMethod(&typeContext->methods[m]);
            }

        }

        // we'll need a target return type

        void IntrospectMethod(MethodDefinition* methodDefinition) {

            MethodDeclarationNode* methodDeclarationNode = GetNode(methodDefinition->methodInfo->nodeIndex);
            BlockNode* blockNode = GetNode(methodDeclarationNode->body);


            PushScope(methodDefinition->scope);

            VisitBlock(blockNode, false);

            // todo -- make sure all child scope paths return a value properly
            if (!methodDefinition->scope->returnType.IsVoid()) {
                // crawl scope tree looking for leaves
            }

            PopScope();

        }


        Expression* AddLocalVariable(ResolvedType pType, FixedCharSpan name, uint16 nodeIndex, DeclarationSource declarationSource) {
//            VEP* vep = allocator->Allocate<VEP>(1);
//            vep->resolvedType = pType;
//            vep->name = name.ptr;
//            vep->nameLength = name.size;
//            vep->nodeIndex = nodeIndex;
//            vep->declarationSource = declarationSource;
//            scopeStack.Peek()->AddVep(vep, vepAllocator);
//            VariableExpression* variable = allocator->Allocate<VariableExpression>(1);
//            return variable; // todo -- not done yet
            return nullptr;
        }

        Expression* VisitIfStatement(IfStatementNode* ifStatementNode) {

            ExpressionListNode* conditions = GetNode(ifStatementNode->conditions);
            ContextListNode* contextListNode = GetNode(ifStatementNode->context);

            int32 contextCount = 0;
            Expression** contextList = nullptr;
            FixedCharSpan* contextNames = nullptr;

            if (contextListNode != nullptr) {
                contextCount = CountExpressions(contextListNode->first);
                contextList = et->allocator->Allocate<Expression*>(contextCount);
                contextNames = et->allocator->Allocate<FixedCharSpan>(contextCount);
                IdentifierNode* ptr = GetNode(contextListNode->first);
                int32 cnt = 0;

                while (ptr != nullptr) {
                    contextNames[cnt] = ptr->GetIdentifier();
                    cnt++;
                    ptr = GetNode(ptr->next);
                }

            }

            if (conditions != nullptr) {
                int32 conditionCount = CountExpressions(conditions->first);
                Expression** conditionList = et->allocator->AllocateUncleared<Expression*>(conditionCount);
                ExpressionNode* ptr = GetNode(conditions->first);

                int32 cnt = 0;

                ResolvedType boolType(BuiltInTypeName::Bool);

                while (ptr != nullptr) {

                    Expression* expression = VisitExpression(&boolType, ptr);

                    if (expression == nullptr) {
                        // break;
                    }

                    ResolvedType resolvedType = expression->GetType();

                    if (resolvedType.Equals(&boolType)) {
                        // see if it's nullable
                        if (resolvedType.IsNullable()) {

                            if (contextCount > cnt) {
                                Expression* variable = et->AddInternalLocalVariable(&resolvedType);
                                et->AddStatement(et->Assign(variable, expression));
                                conditionList[cnt] = et->FieldAccess(false, variable, "hasValue");
                                contextList[cnt] = et->FieldAccess(false, variable, "value");
                            }
                            else {
                                conditionList[cnt] = expression;
                            }

                        }
                        else {
                            LineColumn lc = fileInfo->parseResult.GetLocationFromNodeIndex(ptr->nodeIndex);
                            diagnostics->LogErrorArgs(fileInfo, lc, "Expression type must be bool or nullable");
                            // we won't codegen if we have errors so I think it's fine to leave this as null
                        }

                    }
                    else {
                        conditionList[cnt] = expression;
                        if (contextCount > cnt) {
                            contextList[cnt] = et->BoolConstant(true);
                        }
                    }

                    cnt++;


                    ptr = GetNode(ptr->next);
                }


            }

            StatementNode* bodyStatement = GetNode(ifStatementNode->body);
            if (bodyStatement != nullptr) {

                PushScope();

                if (bodyStatement->nodeType == NodeType::BlockStatement) {

                    // we can't just visit the block because it pushes its own scope which we don't want in this case

                    for (int32 i = 0; i < contextCount; i++) {
                        // todo -- we probably want the node index of the context
                        Expression* expression = AddLocalVariable(contextList[i]->GetType(), contextNames[i], 0, DeclarationSource::ContextListParameter);
                        et->AddStatement(et->Assign(expression, contextList[i]));
                    }

                    VisitBlock((BlockNode*) bodyStatement, false);

                }
                else {

                    for (int32 i = 0; i < contextCount; i++) {
                        // todo -- we probably want the node index of the context
                        Expression* expression = AddLocalVariable(contextList[i]->GetType(), contextNames[i], 0, DeclarationSource::ContextListParameter);
                        et->AddStatement(et->Assign(expression, contextList[i]));
                    }

                    VisitStatement(bodyStatement);
                }

                PopScope();

            }

            StatementNode* elseNode = GetNode(ifStatementNode->elseCase);

            // we may want to force a scope here, not sure if we need to
            Expression* elseNodeExpr = VisitStatement(elseNode);

            // todo -- fill this in
            return et->allocator->New<IfStatementExpression>(fileInfo->fileId, ifStatementNode->GetTokenRange());

        }

        void PushScope(bool isStaticContext = false) {
            Scope* scope = et->MakeBlockScope(isStaticContext);
            scope->parent = scopeStack.Peek();
            scopeStack.Push(scope);
        }

        Scope* PopScope() {
            return scopeStack.Pop();
        }


        bool TryResolveLocalOrParameter(FixedCharSpan name, TokenRange tokenRange, VEP** retn) {

            // we should still return the vep even if it breaks static access rules

            bool passedClosureBoundary = false;
            bool passedStaticBoundary = false;

            for (int32 i = scopeStack.size - 1; i >= 0; i--) {

                Scope* scope = scopeStack[i];

                for (int32 v = scope->vepCount - 1; v >= 0; v--) {

                    VEP* vep = scope->vepList[v];

                    if (vep->GetName() == name) {

                        // closure! the scope should know if it needs to create closure instances by checking its veps
                        if (passedClosureBoundary) {
                            vep->isClosedOver = true;
                        }

                        if (passedStaticBoundary) {
                            diagnostics->LogErrorConst(fileInfo, tokenRange, "cannot access this variable across a static boundary");
                        }

                        *retn = vep;
                        return true;
                    }

                }

                if (scope->isStaticBoundary) {
                    passedStaticBoundary = true;
                }

                if (scope->isClosureContextBoundary) {
                    passedClosureBoundary = true;
                }

            }

            return false;

        }

        bool TryResolveProperty(TypeInfo* typeInfo, FixedCharSpan name, TokenRange tokenRange, PropertyInfo* propertyInfo) {
            if (TypeInfoTryGetProperty(typeInfo, name, &propertyInfo)) {

                if (!propertyInfo->IsStatic()) {

                    Scope* p = scopeStack.Peek();

                    VEP* thisInstance = p->instanceExpression;

                    if (thisInstance == nullptr) {
                        diagnostics->LogErrorArgs(fileInfo, tokenRange, "`%.*s` is an instance property and cannot be used in a static context", name.size, name.ptr);
                        return true;
                    }

                    while (p != nullptr) {

                        // need to capture 'this'
                        if (p->isClosureContextBoundary) {
                            // todo -- I think we need more here, the closure boundary needs to get the 'this' capture if it didn't already
                            // the scope stack should handle nested closure of 'this' (or other parameters) so we can stop here
                            break;
                        }

                        p = p->parent;
                    }

                }

                return true;
            }

            return false;
        }

        bool TryResolveField(TypeInfo* typeInfo, FixedCharSpan name, TokenRange tokenRange, FieldInfo* fieldInfo) {

            if (TypeInfoTryGetField(typeInfo, name, &fieldInfo)) {

                if (!fieldInfo->IsStatic()) {

                    Scope* p = scopeStack.Peek();

                    VEP* thisInstance = p->instanceExpression;

                    if (thisInstance == nullptr) {
                        diagnostics->LogErrorArgs(fileInfo, tokenRange, "`%.*s` is an instance field and cannot be used in a static context", name.size, name.ptr);
                        return true;
                    }

                    while (p != nullptr) {

                        // need to capture 'this'
                        if (p->isClosureContextBoundary) {
                            // todo -- I think we need more here, the closure boundary needs to get the 'this' capture if it didn't already
                            // the scope stack should handle nested closure of 'this' (or other parameters) so we can stop here
                            break;
                        }

                        p = p->parent;
                    }

                }

                return true;
            }

            return false;
        }

        struct AccessChecks {
            bool canBePrivate {};
            bool canBeProtected {};
            bool canBeInternal {};
            bool mustBeInstance {};
            bool canBeStatic {};
        };

        Expression* VisitPrimaryExpression(PrimaryExpressionNode* pNode) {

            ExpressionNode* start = GetNode(pNode->start);

            if (start == nullptr) {
                return nullptr;
            }

            Expression* expression;

            Expression* instance = scopeStack.Peek()->instanceExpression;

            if (start->nodeType == NodeType::Identifier) {
                FixedCharSpan name = ((IdentifierNode*) start)->GetIdentifier();

                VEP* vep;
                FieldInfo* fieldInfo = nullptr;
                PropertyInfo* propertyInfo = nullptr;
                MethodGroup methodGroup;

                if (TryResolveLocalOrParameter(name, start->GetTokenRange(), &vep)) {
                    expression = vep;
                }
                else if (TryResolveField(typeContext->typeInfo, name, start->GetTokenRange(), fieldInfo)) {

                    expression = et->allocator->New<FieldAccessExpression>(instance, fieldInfo, fileInfo->fileId, start->GetTokenRange());

                }
                else if (TryResolveProperty(typeContext->typeInfo, name, start->GetTokenRange(), propertyInfo)) {

                    expression = et->allocator->New<PropertyAccessExpression>(instance, propertyInfo, fileInfo->fileId, start->GetTokenRange());

                }
                else if (TypeInfoTryGetMethodGroup(typeContext->typeInfo, name, et->allocator, &methodGroup)) { // todo -- we could consider releasing this memory again once we resolve the method we want to call
                    // todo -- we still need to know if this got captured in a closure, we can only know this once we figure out which method to call
                    // we don't know which method to call yet, just know a method with this name exists
                    expression = et->allocator->New<MethodGroupAccessExpression>(nullptr, methodGroup, fileInfo->fileId, start->GetTokenRange());
                }
                else {
                    diagnostics->LogErrorArgs(fileInfo, start->GetTokenRange(), "Unable to resolve identifier `%.*s`", name.size, name.ptr);
                    expression = et->allocator->New<SemanticErrorExpression>(fileInfo->fileId, start->GetTokenRange());
                }

            }
            else if (start->nodeType == NodeType::PrimaryMethodInvocation) {
                PrimaryMethodInvocationNode* methodInvocationNode = GetNode(NodeIndex<PrimaryMethodInvocationNode>(start->nodeIndex));
                IdentifierNode* methodNameNode = GetNode(methodInvocationNode->identifier);

                ArgumentListNode* argumentListNode = GetNode(methodInvocationNode->arguments);

                FixedCharSpan methodName = methodNameNode->GetIdentifier();

                int32 methodCount = 0;
                int32 parameterCount = CountExpressions(argumentListNode->first);

                // todo -- we probably need to handle instance differently if we have closures etc

                TypeInfo * typeInfo = typeContext->typeInfo;

                if (!TypeInfoGetMethodCountWithParameterCount(typeInfo, methodName, parameterCount, &methodCount)) {
                    if (HasAnyMethodWithName(typeInfo, methodName)) {
                        diagnostics->LogErrorArgs(fileInfo, start->GetTokenRange(), "Incorrect number of arguments");
                        expression = nullptr; // todo -- still return a method maybe? skip to analyzing next statement?
                        // exception maybe? we can't continue here
                    }
                    else {
                        diagnostics->LogErrorConst(fileInfo, start->GetTokenRange(), "No method with this name");
                        expression = nullptr;
                    }
                }
                else {

                    MethodGroup methodGroup;
                    TypeInfo* instanceTypeInfo = typeContext->typeInfo;

                    AccessChecks accessChecks;
                    accessChecks.mustBeInstance = true;
                    accessChecks.canBeStatic = true;
                    accessChecks.canBePrivate = instanceTypeInfo == typeContext->typeInfo;
                    accessChecks.canBeProtected = instanceTypeInfo->IsSubclassOf(typeContext->typeInfo); // or the reverse?
                    accessChecks.canBeInternal = instanceTypeInfo->GetPackageName() == typeContext->typeInfo->GetPackageName();

                    TypeInfoTryGetMethodGroupWithParameterCount(typeInfo, methodName, tempAllocator, parameterCount, &methodGroup);

                    CheckedArray<ArgumentNode*> arguments(et->allocator->AllocateUncleared<ArgumentNode*>(parameterCount), parameterCount);
                    ArgumentNode* argumentNode = GetNode(argumentListNode->first);

                    int32 a = 0;
                    while (argumentNode != nullptr) {
                        arguments[a] = argumentNode;
                        argumentNode = GetNode(argumentNode->next);
                    }

                    expression = SelectEligibleMethod(instance, methodGroup.methodInfos, arguments, accessChecks, start->GetTokenRange());

                }

            }
            else {
                expression = VisitExpression(nullptr, start);
            }

            expression = VisitBracketExpression(expression, GetNode(pNode->startBracketExpression));

            return VisitPrimaryExpressionTail(expression, GetNode(pNode->tail));

        }

        bool TryGetNextPartAsMethodCall(int32 idx, CheckedArray<PrimaryExpressionTailNode*> partList, MethodInvocationNode** invocationNode) {

            int32 nextIndex = idx + 1;
            if (nextIndex >= partList.size) {
                return false;
            }

            PrimaryExpressionTailNode* nextTail = partList[nextIndex];

            if (nextTail->part.IsValid()) {
                AbstractPsiNode* partNode = fileInfo->parseResult.psiNodes.GetPointer(nextTail->part.index);
                *invocationNode = (MethodInvocationNode*) partNode;
                return partNode->nodeBase.nodeType == NodeType::MethodInvocation;
            }

            return false;

        }

        bool TrySolveGenericArgumentList(TypeArgumentListNode* typeArgumentListNode, CheckedArray<ResolvedType>* retn) {
            if (typeArgumentListNode == nullptr) {
                return false;
            }
            int32 cnt = CountExpressions(typeArgumentListNode->first);
            retn->array = et->allocator->AllocateUncleared<ResolvedType>(cnt);
            retn->size = cnt;

            TypePathNode* ptr = GetNode(typeArgumentListNode->first);
            for (int32 i = 0; i < cnt; i++) {
                // we don't care if we actually resolve or not
                typeResolver.TryResolveTypePath(ptr, &retn->array[i]);
                ptr = GetNode(ptr->next);
            }
            return true;
        }

        Expression * SelectEligibleMethod(Expression* instance, CheckedArray<MethodInfo*> candidates, CheckedArray<ArgumentNode*> arguments, AccessChecks accessChecks, TokenRange tokenRange) {

            // we've already checked argument count

            assert(candidates.size > 0);

            TempAllocator::ScopedMarker marker(tempAllocator);

            TypeUtil::ParameterConversion* pConversions = tempAllocator->Allocate<TypeUtil::ParameterConversion>(arguments.size);
            CheckedArray<TypeUtil::ParameterConversion> conversions(pConversions, arguments.size);

            // reject candidates where storage or pass by don't match.
            // Keep them around for errors

            struct RejectedMethodCandidate {
                MethodInfo* methodInfo;
                char* rejectionReason;
                int32 points;
                TokenRange tokenRange;

                RejectedMethodCandidate(MethodInfo* methodInfo, char* rejectionReason, TokenRange tokenRange, int32 points)
                    : methodInfo(methodInfo)
                    , rejectionReason(rejectionReason)
                    , points(points)
                    , tokenRange(tokenRange) {}

            };

            FixedPodList<MethodInfo*> possible(tempAllocator->AllocateUncleared<MethodInfo*>(candidates.size), candidates.size);
            FixedPodList<RejectedMethodCandidate> rejected(tempAllocator->AllocateUncleared<RejectedMethodCandidate>(candidates.size), candidates.size);

            for (int32 i = 0; i < candidates.size; i++) {

                MethodInfo* candidate = candidates[i];
                if (candidate->IsStatic() && !accessChecks.canBeStatic) {
                    rejected.Add(RejectedMethodCandidate(candidate, (char*) "Cannot access static method from an instance context", tokenRange, 0));
                    continue;
                }

                if (candidate->IsStatic() && accessChecks.mustBeInstance) {
                    rejected.Add(RejectedMethodCandidate(candidate, (char*) "Cannot access instance method from a static context", tokenRange, 0));
                    continue;
                }

                if ((candidate->modifiers & DeclarationModifier::Private) != 0 && !accessChecks.canBePrivate) {
                    rejected.Add(RejectedMethodCandidate(candidate, (char*) "Cannot access private method here", tokenRange, 0));
                    continue;
                }

                if ((candidate->modifiers & DeclarationModifier::Internal) != 0 && !accessChecks.canBeInternal) {
                    rejected.Add(RejectedMethodCandidate(candidate, (char*) "Cannot access internal method here", tokenRange, 0));
                    continue;
                }

                if ((candidate->modifiers & DeclarationModifier::Protected) != 0 && !accessChecks.canBeProtected) {
                    rejected.Add(RejectedMethodCandidate(candidate, (char*) "Cannot access protected method here", tokenRange, 0));
                    continue;
                }

                ParameterInfo* parameterInfos = candidate->parameters;
                bool parameterSetupMatch = true;

                for (int32 p = 0; p < candidate->parameterCount; p++) {
                    ParameterInfo* parameterInfo = &parameterInfos[p];
                    // todo -- storage class
                    if (parameterInfo->passByModifier != arguments[p]->passBy) {
                        rejected.Add(RejectedMethodCandidate(candidate, (char*) "Parameter modifier type does not match", arguments[p]->GetTokenRange(), 0));
                        parameterSetupMatch = false;
                        break;
                    }
                }

                if (parameterSetupMatch) {
                    possible.Add(candidate);
                }

            }


            CheckedArray<ArgumentExpression*> argumentExpressions(et->allocator->Allocate<ArgumentExpression*>(arguments.size), arguments.size);

            if (possible.size != 0) {

                // figure out the target types that are common across all candidates

                for (int32 i = 0; i < arguments.size; i++) {

                    bool isCommon = true;
                    ResolvedType type = possible[0]->parameters[i].type;

                    for (int j = 1; j < possible.size; j++) {

                        ParameterInfo* parameterInfo = &possible[j]->parameters[i];
                        if (!type.EqualsRef(parameterInfo->type)) {
                            isCommon = false;
                            break;
                        }

                    }

                    if (isCommon) {
                        argumentExpressions[i] = VisitArgumentNode(&type, arguments[i]);
                    }
                    else {
                        argumentExpressions[i] = VisitArgumentNode(nullptr, arguments[i]);
                    }

                }

                int32 winnerIndex = -1;
                int32 winnerPoints = -10000;
                bool isAmbiguous = false;

                TypeUtil::ParameterConversion* winningConversions = tempAllocator->Allocate<TypeUtil::ParameterConversion>(arguments.size);

                // score the possible candidates
                for (int32 candidateIndex = 0; candidateIndex < possible.size; candidateIndex++) {
                    int32 points = 0;

                    bool valid = true;

                    MethodInfo * candidate = possible[candidateIndex];

                    for (int32 p = 0; p < arguments.size; p++) {

                        if (TypeUtil::TryConvertExpressionScored(argumentExpressions[p], candidate->parameters[p].type, &points, &conversions[p])) {
                            continue;
                        }

                        valid = false;

                        if (winnerIndex == -1) {
                            // if we don't have a winner yet, add to rejected
                            rejected.Add(RejectedMethodCandidate(candidate, (char*) "Cannot convert parameter type", arguments[p]->GetTokenRange(), points));
                        }

                        break;

                    }

                    if (valid && points > winnerPoints) {
                        winnerPoints = points;
                        winnerIndex = candidateIndex;
                        memcpy(winningConversions, conversions.array, sizeof(TypeUtil::ParameterConversion) * arguments.size);
                    }
                    else if (valid && points == winnerPoints) {
                        isAmbiguous = true; // todo -- do something with this
                    }

                }

                if (winnerIndex != -1) {

                    MethodInfo* winner = candidates[winnerIndex];

                    for (int32 i = 0; i < arguments.size; i++) {
                        argumentExpressions[i]->expression = winningConversions[i].Convert(et);
                    }

                    if (winner->IsStatic()) {
                        return et->allocator->New<StaticMethodCall>(winner, argumentExpressions, fileInfo->fileId, tokenRange);
                    }
                    else {
                        return et->allocator->New<InstanceMethodCall>(instance, winner, argumentExpressions, fileInfo->fileId, tokenRange);
                    }

                }

            }

            else {
                // if we have no possible candidates we visit without a type assumption
                for (int32 i = 0; i < arguments.size; i++) {
                    argumentExpressions[i] = VisitArgumentNode(nullptr, arguments[i]);
                }

            }
            // if we haven't found a match we want to show an error message
            // we need to find the highest scoring candidate and figure out why it was rejected

            int32 winnerIndex = 0;
            int32 winnerPoints = -10000;
            for (int32 i = 0; i < rejected.size; i++) {
                // score these to return whichever one is most likely for an error message

                if(rejected[i].points > 0) {
                    if(rejected[i].points > winnerPoints) {
                        winnerPoints = rejected[i].points;
                        winnerIndex = i;
                    }
                    continue;
                }

                int32 points = 0;

                for (int32 p = 0; p < arguments.size; p++) {

                    if (!TypeUtil::TryConvertExpressionScored(argumentExpressions[p], possible[i]->parameters[p].type, &points, &conversions[p])) {
                        break;
                    }

                }

                rejected[i].points = points;

                if(rejected[i].points > winnerPoints) {
                    winnerPoints = rejected[i].points;
                    winnerIndex = i;
                }

            }

            // find highest point total, use that for error display. We don't need to do any conversion of argument types

            MethodInfo* winner = candidates[winnerIndex];

            if (winner->IsStatic()) {
                return et->allocator->New<StaticMethodCall>(winner, argumentExpressions, fileInfo->fileId, tokenRange);
            }
            else {
                return et->allocator->New<InstanceMethodCall>(instance, winner, argumentExpressions, fileInfo->fileId, tokenRange);
            }

        }

        ArgumentExpression* VisitArgumentNode(ResolvedType* targetType, ArgumentNode* argumentNode) {

            switch (argumentNode->passBy) {

                case ArgumentPassByModifier::None: {
                    Expression* expression = VisitInternal(targetType, argumentNode->expression);
                    return et->allocator->New<ArgumentExpression>(ArgumentPassByModifier::None, expression, fileInfo->fileId, argumentNode->GetTokenRange());
                }
                case ArgumentPassByModifier::Ref: {
                    Expression* expression = VisitInternal(targetType, argumentNode->expression);
                    return et->allocator->New<ArgumentExpression>(ArgumentPassByModifier::Ref, expression, fileInfo->fileId, argumentNode->GetTokenRange());
                }
                case ArgumentPassByModifier::Out: {

                    FixedCharSpan variableName = argumentNode->GetVariableName();
                    TypePathNode* typePathNode = GetNode(argumentNode->typePath);

                    if (typePathNode != nullptr) {
                        ResolvedType resolvedType;
                        typeResolver.TryResolveTypePath(typePathNode, &resolvedType);

                        VEP* vep = et->AddLocalVariable(resolvedType, variableName, argumentNode->GetTokenRange());

                        return et->allocator->New<ArgumentExpression>(ArgumentPassByModifier::Out, vep, fileInfo->fileId, argumentNode->GetTokenRange());
                    }
                    else if (argumentNode->isVarType) {
                        ResolvedType varType = ResolvedType::Var();
                        VEP* vep = et->AddLocalVariable(varType, variableName, argumentNode->GetTokenRange());
                        return et->allocator->New<ArgumentExpression>(ArgumentPassByModifier::Out, vep, fileInfo->fileId, argumentNode->GetTokenRange());
                    }
                    else {
                        Expression* expression = VisitInternal(targetType, argumentNode->expression);
                        return et->allocator->New<ArgumentExpression>(ArgumentPassByModifier::Out, expression, fileInfo->fileId, argumentNode->GetTokenRange());
                    }
                }
                default: {
                    Unreachable;
                    return nullptr;
                }
            }

        }

        void CheckSingleNonGenericMethodCandidate(MethodInfo* methodInfo, CheckedArray<ArgumentExpression*> arguments) const {
            assert(!methodInfo->isOptionalParameterPrototype);
            assert(arguments.size == methodInfo->parameterCount);

            for (int32 i = 0; i < arguments.size; i++) {

                if (arguments[i]->passByModifier != methodInfo->parameters[i].passByModifier) {
                    diagnostics->LogErrorConst(fileInfo, arguments[i]->tokenRange, "Argument modifier types do not match");
                }

                Expression* expr;
                if (TypeUtil::TryConvertExpression(arguments[i], methodInfo->parameters[i].type, et, &expr)) {
                    arguments[i]->expression = expr;
                }
                else {
                    diagnostics->LogErrorConst(fileInfo, arguments[i]->tokenRange, "Argument types do not match");
                }

            }

        }

        Expression* VisitPrimaryExpressionTailPart(int32* iterator, CheckedArray<PrimaryExpressionTailNode*> partList, Expression* last) {

            PrimaryExpressionTailNode* tailNode = partList[*iterator];

            AbstractPsiNode* partNode = fileInfo->parseResult.psiNodes.GetPointer(tailNode->part.index);

            ResolvedType lastType = last->GetType();

            bool canIncludePrivate = lastType.typeInfo == typeContext->typeInfo;
            bool canIncludeProtected = lastType.typeInfo->IsSubclassOf(typeContext->typeInfo); // or the reverse?
            bool canIncludeInternal = lastType.typeInfo->GetPackageName() == typeContext->typeInfo->GetPackageName();
            DeclarationModifier allowedModifiers = DeclarationModifier::Public | DeclarationModifier::Export;
            DeclarationModifier mask = DeclarationModifier::VisibilityMask | DeclarationModifier::Static;

            Expression* result = nullptr;

            if (canIncludeInternal) {
                allowedModifiers |= DeclarationModifier::Internal;
            }

            if (canIncludeProtected) {
                allowedModifiers |= DeclarationModifier::Protected;
            }

            if (canIncludePrivate) {
                allowedModifiers |= DeclarationModifier::Private;
            }

            switch (partNode->nodeBase.nodeType) {

                case NodeType::MemberAccess: {
                    MemberAccessNode* memberAccessNode = (MemberAccessNode*) partNode;
                    IdentifierNode* memberName = GetNode(memberAccessNode->identifier);

                    if (memberName == nullptr) {
                        // we already issued a parse error
                        result = et->allocator->New<SemanticErrorExpression>(fileInfo->fileId, memberAccessNode->GetTokenRange());
                        break;
                    }

                    FieldInfo* fieldInfo = nullptr;
                    PropertyInfo* propertyInfo = nullptr;

                    // Thing? x;
                    // x.value // error
                    // Thing y = x!; // bang will explode if nullable didn't have a value


                    CheckedArray<ResolvedType> resolvedGenerics;
                    TrySolveGenericArgumentList(GetNode(memberName->typeArguments), &resolvedGenerics);

                    // I'm not sure we have generics in any case other than method call since we don't support nested types w/ generics

                    if (TypeInfoTryGetField(lastType.typeInfo, memberName->GetIdentifier(), &fieldInfo)) {

                        if (fieldInfo->IsConst()) {
                            diagnostics->LogErrorArgs(fileInfo, memberName->GetTokenRange(), "`%.*s` is a const field and cannot be accessed with an instance", fieldInfo->nameLength, fieldInfo->name);
                        }

                        if (fieldInfo->IsStatic()) {
                            diagnostics->LogErrorArgs(fileInfo, memberName->GetTokenRange(), "`%.*s` is a static field and cannot be accessed with an instance", fieldInfo->nameLength, fieldInfo->name);
                        }

                        if (lastType.IsNullable() || lastType.IsNullableArray()) {
                            diagnostics->LogErrorConst(fileInfo, memberName->GetTokenRange(), "Nullable types do not allow access to fields, you must first convert to a non nullable type");
                        }

                        // todo -- access checks

                        if ((fieldInfo->modifiers & DeclarationModifier::Private) != 0) {

                        }

                        if ((fieldInfo->modifiers & DeclarationModifier::Protected) != 0) {

                        }

                        if ((fieldInfo->modifiers & DeclarationModifier::Internal) != 0) {

                        }

                        result = et->allocator->New<FieldAccessExpression>(last, fieldInfo, fileInfo->fileId, memberName->GetTokenRange());
                        break;

                    }
                    else if (TypeInfoTryGetProperty(lastType.typeInfo, memberName->GetIdentifier(), &propertyInfo)) {

                        if (propertyInfo->IsStatic()) {
                            diagnostics->LogErrorArgs(fileInfo, memberName->GetTokenRange(), "`%.*s` is a static field and cannot be accessed with an instance", propertyInfo->nameLength, propertyInfo->name);
                        }

                        if (lastType.IsNullable() || lastType.IsNullableArray()) {
                            diagnostics->LogErrorConst(fileInfo, memberName->GetTokenRange(), "Nullable types do not allow access to fields, you must first convert to a non nullable type");
                        }

                        // todo -- access checks

                        if ((propertyInfo->modifiers & DeclarationModifier::Private) != 0) {

                        }

                        if ((propertyInfo->modifiers & DeclarationModifier::Protected) != 0) {

                        }

                        if ((propertyInfo->modifiers & DeclarationModifier::Internal) != 0) {

                        }

                        result = et->allocator->New<PropertyAccessExpression>(last, propertyInfo, fileInfo->fileId, memberName->GetTokenRange());
                        break;
                    }
                    else if (HasAnyMethodWithName(lastType.typeInfo, memberName->GetIdentifier())) {

                        MethodGroup methodGroup;

                        MethodInvocationNode* invocationNode;
                        if (TryGetNextPartAsMethodCall(*iterator, partList, &invocationNode)) {
                            *iterator = *iterator + 1; // bump the iterator since we consume it here

                            // remove methods that are not callable (static, private)

                            ArgumentListNode* argumentListNode = GetNode(invocationNode->arguments);

                            int32 parameterCount = CountExpressions(argumentListNode->first);

                            if (TypeInfoTryGetMethodGroupWithParameterCount(lastType.typeInfo, memberName->GetIdentifier(), tempAllocator, parameterCount, &methodGroup)) {
                                // figure out which method to call, then figure out if it's static/instance and if we can access it

                                if (methodGroup.methodInfos.size == 1 && !methodGroup.methodInfos[0]->isGenericDefinition) {
                                    // visit arguments trying to convert them

                                    CheckedArray<ArgumentExpression*> arguments(et->allocator->AllocateUncleared<ArgumentExpression*>(parameterCount), parameterCount);
                                    ArgumentNode* argumentNode = GetNode(argumentListNode->first);

                                    int32 a = 0;
                                    while (argumentNode != nullptr) {
                                        arguments[a] = VisitArgumentNode(&methodGroup.methodInfos[0]->parameters[a].type, argumentNode);
                                        argumentNode = GetNode(argumentNode->next);
                                    }

                                    CheckSingleNonGenericMethodCandidate(methodGroup.methodInfos[0], arguments);

                                    result = et->allocator->New<InstanceMethodCall>(last, methodGroup.methodInfos[0], arguments, et->fileInfo->fileId, invocationNode->GetTokenRange());
                                    break;

                                }
                                else {

                                    CheckedArray<ArgumentNode*> arguments(et->allocator->Allocate<ArgumentNode*>(parameterCount), parameterCount);
                                    ArgumentNode* argumentNode = GetNode(argumentListNode->first);

                                    int32 a = 0;
                                    while (argumentNode != nullptr) {
                                        arguments[a] = argumentNode;
                                        argumentNode = GetNode(argumentNode->next);
                                    }

                                }

                            }
                            else {
                                // no method with that name with that number of arguments
                                //
                            }

                        }
                        else if (TypeInfoTryGetMethodGroup(lastType.typeInfo, memberName->GetIdentifier(), tempAllocator, &methodGroup)) {
                            // I don't know what to do with this yet, we don't know which method is being targeted

                            if (!tailNode->bracketListStart.IsValid()) {
                                // can't use an indexer on a method group
                                diagnostics->LogErrorArgs(fileInfo, GetNode(tailNode->bracketListStart)->GetTokenRange(), "Cannot use an indexer on a method group");
                                break;
                            }

                            // otherwise we check if our next part is a method invocation
                            if (TryGetNextPartAsMethodCall(*iterator, partList, &invocationNode)) {
                                *iterator = *iterator + 1; // bump the iterator since we consume it here

                                // remove methods that are not callable (static, private)

                                for (int32 m = 0; m < methodGroup.methodInfos.size; m++) {
                                    MethodInfo* methodInfo = methodGroup.methodInfos[m];
                                    DeclarationModifier declarationModifier = methodInfo->modifiers & mask;
                                    if ((declarationModifier & allowedModifiers) != 0) {

                                    }

                                }

                            }
                            else if (*iterator + 1 >= partList.size) {

                                // we'll need to respect access controls here and possibly static as well

                                // if this is the last part and our target type is a nameof return the method name
                                // if this is the last part and our target type is a typeof return the type MethodGroup(typeInfo, name)
                                // if this is the last part and our target type is a delegate, find method matching the delegate type

                                // we might exclude non accessible methods

                            }
                            else {

                            }

                        }
                        else {
                            diagnostics->LogErrorArgs(fileInfo, memberAccessNode->GetTokenRange(), "Unable to resolve member `%.*s`", memberName->GetIdentifier().size, memberName->GetIdentifier().ptr);
                            result = et->allocator->New<SemanticErrorExpression>(fileInfo->fileId, memberName->GetTokenRange());
                            break;
                        }

                    }

                    break;
                }
                case NodeType::ConditionalMemberAccess: {
                    NotImplemented;
                    break;
                }
                case NodeType::ScopeMemberAccess: {
                    NotImplemented;
                    break;
                }
                case NodeType::MethodInvocation: {
                    NotImplemented;
                    break;
                }
                case NodeType::IncrementDecrement: {
                    NotImplemented;
                    break;
                }
                case NodeType::NullableDereference: {
                    NotImplemented;
                    break;
                }
                default: {
                    Unreachable;
                    break;
                }
            }

            assert(result != nullptr && "result should have been assigned");
            return VisitBracketExpression(result, GetNode(tailNode->bracketListStart));

        }

        Expression* VisitPrimaryExpressionTail(Expression* head, PrimaryExpressionTailNode* primaryExpressionTailNode) {

            if(head == nullptr) {
                return nullptr;
            }

            if (primaryExpressionTailNode == nullptr) {
                return head;
            }

            if (!primaryExpressionTailNode->part.IsValid()) {
                return VisitBracketExpression(head, GetNode(primaryExpressionTailNode->bracketListStart));
            }

            // push all parts into a list
            int32 cnt = CountExpressions(NodeIndex<PrimaryExpressionTailNode>(primaryExpressionTailNode->nodeIndex));
            FixedPodList<PrimaryExpressionTailNode*> tailNodes(tempAllocator->AllocateUncleared<PrimaryExpressionTailNode*>(cnt), cnt);
            PrimaryExpressionTailNode* ptr = primaryExpressionTailNode;

            while (ptr != nullptr) {
                tailNodes.Add(ptr);
                ptr = GetNode(ptr->next);
            }

            Expression* last = head;
            for (int32 i = 0; i < tailNodes.size; i++) {
                last = VisitPrimaryExpressionTailPart(&i, tailNodes.ToCheckedArray(), last);
            }

            return head;

        }

        Expression* VisitBracketExpression(Expression* head, BracketExpressionNode* bracket) {

            if(head == nullptr) {
                return nullptr;
            }

            if (bracket == nullptr) {
                return head;
            }

            NotImplemented;

            bool isIndexable = head->GetType().IsIndexable();

            if (!isIndexable) {
                diagnostics->LogErrorConst(fileInfo, bracket->GetTokenRange(), "type is not indexable");
            }

            int32 cnt = CountExpressions(bracket->next);
            CheckedArray<Expression*> bracketExpressions(nullptr, cnt); // todo
            CheckedArray<ResolvedType> bracketExpressionTypes(nullptr, cnt);

            BracketExpressionNode* ptr = bracket;

            // todo -- special case array
            // todo -- we only support single argument indexers? probably can use the same resolution as methods so we can handle this for free basically

            cnt = 0;
            while (ptr != nullptr) {

                Expression* brackExpr = VisitExpression(nullptr, GetNode(ptr->expression));
                // todo -- handle nullable
                bracketExpressions[cnt++] = brackExpr;
                ptr = GetNode(ptr->next);

            }

            // todo -- there  are maybe some implicit conversions to do here
            IndexerInfo* indexerInfo = head->GetType().GetIndexerInfo(bracketExpressionTypes);

            return head;

        }

        Expression* VisitLiteralCore(LiteralExpressionNode* literalNode) {

            NumericLiteralValue v;

            switch (literalNode->literalType) {

                case LiteralType::UInt64:
                    v.u64 = literalNode->literalValue.uint64Value;
                    return et->allocator->New<NumericLiteralExpression>(NumericLiteralTypeName::UInt64, v, fileInfo->fileId, literalNode->GetTokenRange());

                case LiteralType::UInt32:
                    return et->allocator->New<NumericLiteralExpression>(NumericLiteralTypeName::UInt32, v, fileInfo->fileId, literalNode->GetTokenRange());

                case LiteralType::Int32:
                    return et->allocator->New<NumericLiteralExpression>(NumericLiteralTypeName::Int32, v, fileInfo->fileId, literalNode->GetTokenRange());

                case LiteralType::Int64:
                    return et->allocator->New<NumericLiteralExpression>(NumericLiteralTypeName::Int64, v, fileInfo->fileId, literalNode->GetTokenRange());

                case LiteralType::Float:
                    return et->allocator->New<NumericLiteralExpression>(NumericLiteralTypeName::Float, v, fileInfo->fileId, literalNode->GetTokenRange());

                case LiteralType::Double:
                    return et->allocator->New<NumericLiteralExpression>(NumericLiteralTypeName::Double, v, fileInfo->fileId, literalNode->GetTokenRange());

                case LiteralType::Bool:
                    return et->allocator->New<BoolLiteralExpression>(literalNode->literalValue.boolValue, fileInfo->fileId, literalNode->GetTokenRange());

                case LiteralType::String:
                    NotImplemented;
                    return nullptr;

                case LiteralType::None:
                case LiteralType::Null:
                case LiteralType::Default:
                default:
                    Unreachable;
                    return nullptr;
            }

        }

        LineColumn GetLocation(TokenRange range) {
            return fileInfo->parseResult.GetLocationFromToken(range.start);
        }

        Expression* VisitLiteral(ResolvedType* targetType, LiteralExpressionNode* literalNode) {

            // null & default can be typeless, we'll validate usage later, sometimes we can associate their types later

            if (literalNode->literalType == LiteralType::Null) {

                if (targetType == nullptr) {
                    return et->allocator->New<NullLiteralExpression>(ResolvedType::Null(), fileInfo->fileId, literalNode->GetTokenRange());
                }
                else {
                    return et->allocator->New<NullLiteralExpression>(*targetType, fileInfo->fileId, literalNode->GetTokenRange());
                }

            }

            if (literalNode->literalType == LiteralType::Default) {

                if (literalNode->defaultType.IsValid()) {

                    ResolvedType resolvedType;
                    if (typeResolver.TryResolveTypePath(GetNode(literalNode->defaultType), &resolvedType)) {

                        if (targetType != nullptr && !targetType->Equals(&resolvedType) && !targetType->IsAssignableFrom(resolvedType)) {

                            TempAllocator::ScopedMarker mark(tempAllocator);
                            diagnostics->LogErrorArgs(fileInfo, literalNode->GetTokenRange(), "default type `%s` is not assignable to `%s`",
                                resolvedType.ToCString(tempAllocator),
                                targetType->ToCString(tempAllocator)
                            );

                        }

                        return et->allocator->New<DefaultLiteralExpression>(resolvedType, fileInfo->fileId, literalNode->GetTokenRange());

                    }
                    else {
                        return et->allocator->New<DefaultLiteralExpression>(ResolvedType::Null(), fileInfo->fileId, literalNode->GetTokenRange());
                    }

                }
                else if (targetType == nullptr) {
                    return et->allocator->New<DefaultLiteralExpression>(ResolvedType::Null(), fileInfo->fileId, literalNode->GetTokenRange());
                }
                else {
                    return et->allocator->New<DefaultLiteralExpression>(*targetType, fileInfo->fileId, literalNode->GetTokenRange());
                }

            }

            Expression* retn = VisitLiteralCore(literalNode);

            // I think casting / conversion gets handled by the caller, right?

            return retn;

        }

        Expression* VisitExpressionCore(ResolvedType& targetType, AbstractPsiNode* node) {

            switch (node->nodeBase.nodeType) {
                case NodeType::Literal: {
                    return VisitLiteral(&targetType, (LiteralExpressionNode*)
                        node);
                }
                case NodeType::AssignmentExpression: {

                }
                default: {
                    assert(false && "not implemented or invalid");
                    return nullptr;
                }
            }

        }

        Expression* VisitInternal(ResolvedType* resolvedType, UntypedNodeIndex nodeIndex) {

            if (!nodeIndex.IsValid()) {
                return nullptr;
            }

            AbstractPsiNode* pNode = fileInfo->parseResult.psiNodes.GetPointer(nodeIndex.index);

            switch (pNode->nodeBase.nodeType) {
                case NodeType::PrimaryExpression: {
                    return VisitPrimaryExpression((PrimaryExpressionNode*) pNode);
                }
                default:
                    NotImplemented;
                    return nullptr;
            }

            return nullptr;
        }

        Expression* VisitBinaryExpression(BinaryExpressionNode* pNode) {

            if (pNode->type == BinaryExpressionType::ConditionalOr || pNode->type == BinaryExpressionType::ConditionalAnd) {
                ResolvedType boolType(BuiltInTypeName::Bool);
                Expression* lhs = VisitInternal(&boolType, pNode->lhs);
                Expression* rhs = VisitInternal(&boolType, pNode->rhs);
                return et->MakeConditionalExpression(lhs, pNode->type == BinaryExpressionType::ConditionalOr ? ConditionalOperator::Or : ConditionalOperator::And, rhs, pNode->GetTokenRange());
            }

            Expression* lhs = VisitInternal(nullptr, pNode->lhs);
            Expression* rhs = VisitInternal(nullptr, pNode->rhs);

            switch (pNode->type) {

                case BinaryExpressionType::ConditionalOr:
                case BinaryExpressionType::ConditionalAnd:
                case BinaryExpressionType::BitwiseOr:
                case BinaryExpressionType::BitwiseXor:
                case BinaryExpressionType::BitwiseAnd:
                    NotImplemented;
                    return nullptr;

                case BinaryExpressionType::Equal:
                    return et->MakeEqualityExpression(lhs, EqualityOperator::Equal, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::NotEqual:
                    return et->MakeEqualityExpression(lhs, EqualityOperator::NotEqual, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::LessThan:
                    return et->MakeComparisonExpression(lhs, ComparisonOperator::LessThan, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::GreaterThan:
                    return et->MakeComparisonExpression(lhs, ComparisonOperator::GreaterThan, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::LessThanOrEqualTo:
                    return et->MakeComparisonExpression(lhs, ComparisonOperator::LessThanOrEqualTo, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::GreaterThanOrEqualTo:
                    return et->MakeComparisonExpression(lhs, ComparisonOperator::GreaterThanOrEqualTo, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::ShiftRight:
                    return nullptr;

                case BinaryExpressionType::ShiftLeft:
                    return nullptr;

                case BinaryExpressionType::Add:
                    return et->ArithmeticOperation(lhs, ArithmeticOperator::Add, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::Subtract:
                    return et->ArithmeticOperation(lhs, ArithmeticOperator::Subtract, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::Multiply:
                    return et->ArithmeticOperation(lhs, ArithmeticOperator::Multiply, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::Divide:
                    return et->ArithmeticOperation(lhs, ArithmeticOperator::Divide, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::Modulus:
                    return et->ArithmeticOperation(lhs, ArithmeticOperator::Modulus, rhs, pNode->GetTokenRange());

                case BinaryExpressionType::None:
                    Unreachable;
                    return nullptr;
            }
        }

        Expression* VisitExpression(ResolvedType* targetType, ExpressionNode* pNode) {
            if (pNode == nullptr) {
                return nullptr;
            }

            switch (pNode->nodeType) {

                case NodeType::Literal: {
                    return VisitLiteral(targetType, (LiteralExpressionNode*)pNode);
                }

                case NodeType::PrimaryExpression: {
                    return VisitPrimaryExpression((PrimaryExpressionNode*) pNode);
                }

                case NodeType::BinaryExpression: {
                    return VisitBinaryExpression((BinaryExpressionNode*) pNode);
                }
                default: {
                    char c[1024];
                    int32 size = snprintf(c, 1024, "expression type `%s` was not implemented", NodeTypeToString(pNode->nodeType));
                    c[size] = '\0';
                    assert(false && (const char*) c);
                }
            }

            return nullptr;

        }

        Expression* VisitExpressionStatement(ExpressionStatementNode* pNode) {
            // we might need a node here to handle the ; when printing
            // or we mark the expression as requiring termination
            return VisitExpression(nullptr, GetNode(pNode->expression));
        }

        Expression* VisitStatement(StatementNode* pNode) {

            if (pNode == nullptr) {
                return nullptr;
            }

            switch (pNode->nodeType) {
                case NodeType::IfStatement: {
                    return VisitIfStatement((IfStatementNode*) pNode);
                    break;
                }
                case NodeType::ExpressionStatement: {
                    return VisitExpressionStatement((ExpressionStatementNode*) pNode);
                }
                case NodeType::BlockStatement: {
                    return VisitBlock((BlockNode*) pNode, true);
                }
                default: {
                    char c[1024];
                    int32 size = snprintf(c, 1024, "statement type `%s` was not implemented", NodeTypeToString(pNode->nodeType));
                    c[size] = '\0';
                    assert(false && (const char*) c);
                    return nullptr;
                }
            }

        }

        Scope* VisitBlock(BlockNode* blockNode, bool pushScope) {

            if (blockNode == nullptr) {
                return nullptr;
            }

            if (pushScope) {
                PushScope();
            }

            StatementNode* statementNode = GetNode(blockNode->firstStatement);
            StatementNode* ptr = statementNode;
            while (ptr != nullptr) {
                VisitStatement(ptr);
                ptr = GetNode(ptr->next);
            }

            if (pushScope) {
                return PopScope();
            }

            return nullptr;

        }

    };

}