#pragma once

#include "./PooledBlockAllocator.h"
#include "./TypeContext.h"

namespace Alchemy::Compilation {

    namespace TypeUtil {
        bool IsImplicitNumericConversion(ResolvedType& leftType, ResolvedType& rightType);
    }

    struct ExpressionTree {

        Scope* currentScope;
        PoolingAllocator* poolingAllocator;
        LinearAllocator* allocator;
        TempAllocator* tempAllocator;
        FileInfo* fileInfo;
        Diagnostics* diagnostics;

        TypeInfo* typeInfo;
        TypeContext* typeContext;

        CheckedArray<AbstractPsiNode> nodes;
        int32 variableNameGen;

        ExpressionTree(FileInfo* fileInfo, DeclaredType declaredType, Diagnostics* diagnostics, PoolingAllocator* poolingAllocator)
            : fileInfo(fileInfo)
            , diagnostics(diagnostics)
            , poolingAllocator(poolingAllocator)
            , allocator(poolingAllocator->linearAllocator)
            , typeContext(declaredType.typeContext)
            , typeInfo(declaredType.typeInfo)
            , variableNameGen(0) {

            nodes = fileInfo->parseResult.psiNodes;
            currentScope = allocator->New<Scope>(fileInfo->fileId, GetTokenRange(typeInfo->nodeIndex));
            typeContext->rootScope = currentScope;
        }

        template<class T>
        TokenRange GetTokenRange(NodeIndex<T> index) {
            if (index.index == 0) {
                return TokenRange();
            }
            return nodes[index.index].GetTokenRange();
        }

        TokenRange GetTokenRange(UntypedNodeIndex index) {
            if (index.index == 0) {
                return TokenRange();
            }
            return nodes[index.index].GetTokenRange();
        }

        TokenRange GetTokenRange(uint16 index) {
            if (index == 0) {
                return TokenRange();
            }
            return nodes[index].GetTokenRange();
        }

        void AddStatement(Expression* statement) {
            currentScope->AddStatement(statement, poolingAllocator);
        }

        FieldAccessExpression* MakeFieldAccess(Expression* instance, FixedCharSpan fieldName, TokenRange tokenRange) {

            ResolvedType resolveType = instance->GetType();

            int32 fieldIdx = -1;
            for (int32 i = 0; i < resolveType.typeInfo->fieldCount; i++) {
                if (resolveType.typeInfo->fields[i].GetName() == fieldName) {
                    fieldIdx = i;
                    break;
                }
            }

            if (fieldIdx == -1) {
                LineColumn lc = fileInfo->parseResult.GetLocationFromToken(tokenRange.start);
                diagnostics->LogErrorArgs(fileInfo, lc, "Type `%.*s` does not define a field `%.*s`",
                    resolveType.typeInfo->GetFullyQualifiedTypeName().size,
                    resolveType.typeInfo->GetFullyQualifiedTypeName().ptr,
                    fieldName.size,
                    fieldName.ptr
                );
                resolveType = ResolvedType(BuiltInTypeName::Object);
            }
            else {

                DeclarationModifier modifier = resolveType.typeInfo->fields[fieldIdx].modifiers;

                if (resolveType.typeInfo != typeInfo && (modifier & DeclarationModifier::Public) == 0) {

                    if ((modifier & DeclarationModifier::Private) != 0) {
                        if (resolveType.typeInfo->declaringFile->packageName != typeInfo->declaringFile->packageName) {
                            LineColumn lc = fileInfo->parseResult.GetLocationFromToken(tokenRange.start);
                            diagnostics->LogErrorArgs(fileInfo, lc, "Field `%.*s` is marked private and is not accessible here",
                                fieldName.size,
                                fieldName.ptr
                            );
                        }
                    }
                    else if ((modifier & DeclarationModifier::Protected) != 0) {
                        if (!resolveType.typeInfo->IsSubclassOf(typeInfo)) {
                            LineColumn lc = fileInfo->parseResult.GetLocationFromToken(tokenRange.start);
                            diagnostics->LogErrorArgs(fileInfo, lc, "Field `%.*s` is marked protected and is not accessible here",
                                fieldName.size,
                                fieldName.ptr
                            );
                        }
                    }
                    else if ((modifier & DeclarationModifier::Internal) != 0) {
                        if (resolveType.typeInfo->declaringFile->packageName != typeInfo->declaringFile->packageName) {
                            LineColumn lc = fileInfo->parseResult.GetLocationFromToken(tokenRange.start);
                            diagnostics->LogErrorArgs(fileInfo, lc, "Field `%.*s` is marked internal and is not accessible here",
                                fieldName.size,
                                fieldName.ptr
                            );
                        }
                    }

                }

            }

            // todo -- fieldinfo
            return allocator->New<FieldAccessExpression>(instance, nullptr, fileInfo->fileId, tokenRange);
        }

        FieldAccessExpression* MakeFieldAccessInternal(Expression* instance, FixedCharSpan fieldName) {
            return MakeFieldAccess(instance, fieldName, TokenRange());
        }

        VEP* DefineField(FieldInfo* fieldInfo) {

            assert(currentScope->parent == nullptr && "fields can only be added to root scopes");

            VEP* vep;
            if ((fieldInfo->modifiers & DeclarationModifier::Static) != 0) {
                vep = allocator->New<VEP>(
                    fieldInfo->GetName(),
                    fieldInfo->type,
                    DeclarationSource::StaticField,
                    ArgumentPassByModifier::None,
                    true,
                    fileInfo->fileId,
                    GetTokenRange(fieldInfo->nodeIndex)
                );
            }
            else {
                vep = allocator->New<VEP>(
                    fieldInfo->GetName(),
                    fieldInfo->type,
                    DeclarationSource::Field,
                    ArgumentPassByModifier::None,
                    true,
                    fileInfo->fileId,
                    GetTokenRange(fieldInfo->nodeIndex)
                );
            }

            currentScope->AddVep(vep, poolingAllocator);

            return vep;

        }

        // Veps are always local so we can check if assignable
        // readonly, no setter, or const -> not writable
        // veps are always readable, we won't support setter only properties

        PropertyDefinition DefineProperty(PropertyInfo* propertyInfo) {

            assert(currentScope->parent == nullptr && "properties can only be added to root scopes");
            bool isStatic = (propertyInfo->modifiers & DeclarationModifier::Static) != 0;

            ArgumentPassByModifier passByModifier = ArgumentPassByModifier::None;
            TypeAccessModifier typeAccessModifier = TypeAccessModifier::Default;
            StorageClass storageClass = StorageClass::Default; // todo

            if ((propertyInfo->modifiers & DeclarationModifier::Ref) != 0) {
                passByModifier = ArgumentPassByModifier::Ref;
                typeAccessModifier = TypeAccessModifier::Ref;
            }

            VEP* vep;
            if (isStatic) {
                vep = allocator->New<VEP>(
                    propertyInfo->GetName(),
                    propertyInfo->type,
                    DeclarationSource::StaticProperty,
                    passByModifier,
                    propertyInfo->setterNodeIndex.IsValid(),
                    fileInfo->fileId,
                    GetTokenRange(propertyInfo->nodeIndex)
                );
            }
            else {
                vep = allocator->New<VEP>(
                    propertyInfo->GetName(),
                    propertyInfo->type,
                    DeclarationSource::Property,
                    passByModifier,
                    propertyInfo->setterNodeIndex.IsValid(),
                    fileInfo->fileId,
                    GetTokenRange(propertyInfo->nodeIndex)
                );
            }

            currentScope->AddVep(vep, poolingAllocator);

            PropertyDefinition retn;

            retn.propertyInfo = propertyInfo;

            if (propertyInfo->getterNodeIndex.IsValid()) {
                retn.getterScope = allocator->New<Scope>(
                    propertyInfo->type,
                    storageClass,
                    typeAccessModifier,
                    isStatic,
                    fileInfo->fileId,
                    GetTokenRange(propertyInfo->getterNodeIndex)
                );
            }

            if (propertyInfo->setterNodeIndex.IsValid()) {
                retn.setterScope = allocator->New<Scope>(
                    propertyInfo->type,
                    storageClass,
                    typeAccessModifier,
                    isStatic,
                    fileInfo->fileId,
                    GetTokenRange(propertyInfo->setterNodeIndex)
                );
            }

            return retn;
        }

        MethodDefinition DefineMethod(MethodInfo* methodInfo) {
            // somehow we need to handle 'ref' and storage

            assert(currentScope->IsRootScope());

            StorageClass storageClass = StorageClass::Default;
            TypeAccessModifier accessModifier = TypeAccessModifier::Default;

            if ((methodInfo->modifiers & DeclarationModifier::Ref) != 0) {
                accessModifier = TypeAccessModifier::Ref;
            }

            // todo -- storage class
            // if((methodInfo->modifiers & DeclarationModifier::Temp) != 0) {
            //
            // }

            bool isStatic = (methodInfo->modifiers & DeclarationModifier::Static) != 0;

            Scope* scope = allocator->New<Scope>(methodInfo->returnType, storageClass, accessModifier, isStatic, fileInfo->fileId, GetTokenRange(methodInfo->nodeIndex));

            scope->parent = currentScope;

            MethodDefinition retn;
            retn.methodInfo = methodInfo;
            retn.scope = scope;
            retn.parameters = poolingAllocator->Allocate<VEP*>(methodInfo->parameterCount);

            for (int32 p = 0; p < methodInfo->parameterCount; p++) {
                retn.parameters[p] = MakeParameter(&methodInfo->parameters[p]);
            }

            return retn;

        }

        VEP* DefineVariable(ResolvedType resolvedType, FixedCharSpan name, UntypedNodeIndex nodeIndex) {
            VEP* vep = allocator->New<VEP>(
                name,
                resolvedType,
                DeclarationSource::LocalVariable,
                ArgumentPassByModifier::None,
                true,
                fileInfo->fileId,
                GetTokenRange(nodeIndex)
            );
            currentScope->AddVep(vep, poolingAllocator);
            return vep;
        }

        FixedCharSpan MakeVariableName() {
            int32 cnt = IntToAsciiCount(variableNameGen);
            char* c = poolingAllocator->Allocate<char>(5 + cnt + 1);
            const char* name = "__var";
            memcpy(c, name, 5);
            IntToAscii(variableNameGen, c + 5);
            c[5 + cnt] = '\0';
            variableNameGen++;

            return FixedCharSpan(c, 5 + cnt);

        }

        VEP* DefineInternalVariable(ResolvedType resolvedType) {
            VEP* vep = allocator->New<VEP>(
                MakeVariableName(),
                resolvedType,
                DeclarationSource::LocalVariable,
                ArgumentPassByModifier::None,
                true,
                fileInfo->fileId,
                TokenRange()
            );
            currentScope->AddVep(vep, poolingAllocator);
            return vep;
        }

        Expression* Assign(Expression* lhs, Expression* rhs) {

            if (lhs->GetType().IsVoid()) {
                // bad
            }

            // if !assignable
            return nullptr;
        }

        Expression* FieldAccess(bool isByRef, Expression* pExpression, const char* fieldName) {
            return nullptr;
        }

        Expression* FieldAccess(bool isByRef, Expression* pExpression, FieldInfo* fieldInfo) {
            return nullptr;
        }

        Expression* BoolConstant(bool b) {
            return nullptr;
        }

        Expression* AddInternalLocalVariable(ResolvedType* pType) {
            return nullptr;
        }

        Expression* MakeConditionalExpression(Expression* left, ConditionalOperator op, Expression* right, TokenRange tokenRange) {
            return nullptr;
        }

        Expression* MakeEqualityExpression(Expression* left, EqualityOperator op, Expression* right, TokenRange tokenRange) {
            ResolvedType leftType = left->GetType();
            ResolvedType rightType = right->GetType();

            if (leftType.Equals(&rightType)) {

                //if(leftType.IsArithmetic() || leftType.IsEnum() || leftType.IsReferenceType()) {
                return allocator->New<EqualityExpression>(left, op, right, fileInfo->fileId, tokenRange);
                // }

                // todo -- look for an op_Equal and op_NotEqual

                // i kind of like memcmp for struct equality automatically

            }

            bool leftIsReference = leftType.IsReferenceType();
            bool rightIsReference = rightType.IsReferenceType();

            if (leftIsReference && rightIsReference) {

                // see if they are the assignable
                if (leftType.IsAssignableFrom(rightType)) {
                    // cast + equal (but we can't compare pointers probably need to find the root using vtable)
                }

            }

            else if (leftIsReference || rightIsReference) {
                // todo -- look for an operator
            }

            if (leftType.IsArithmetic() && rightType.IsArithmetic()) {

                // If either operand is of type double, the other operand is converted to double.
                if (leftType.builtInTypeName == BuiltInTypeName::Double || rightType.builtInTypeName == BuiltInTypeName::Double) {
                    return MakeImplicitEquality<BuiltInTypeName::Double>(left, right, op, tokenRange);
                }

                // If either operand is of type float, the other operand is converted to float.
                if (leftType.builtInTypeName == BuiltInTypeName::Float || rightType.builtInTypeName == BuiltInTypeName::Float) {
                    return MakeImplicitEquality<BuiltInTypeName::Float>(left, right, op, tokenRange);
                }

                // If either operand is of type ulong, the other operand is converted to ulong.
                if ((leftType.builtInTypeName == BuiltInTypeName::Ulong || rightType.builtInTypeName == BuiltInTypeName::Ulong)) {
                    return MakeImplicitEquality<BuiltInTypeName::Ulong>(left, right, op, tokenRange);
                }

                // If either operand is of type long, the other operand is converted to long.
                if ((leftType.builtInTypeName == BuiltInTypeName::Long || rightType.builtInTypeName == BuiltInTypeName::Long)) {
                    return MakeImplicitEquality<BuiltInTypeName::Long>(left, right, op, tokenRange);
                }

                // If either operand is of type uint, the other operand is converted to uint.
                if ((leftType.builtInTypeName == BuiltInTypeName::UInt32 || rightType.builtInTypeName == BuiltInTypeName::UInt32)) {
                    return MakeImplicitEquality<BuiltInTypeName::UInt32>(left, right, op, tokenRange);
                }

                // If either operand is of type int, the other operand is converted to int.
                if ((leftType.builtInTypeName == BuiltInTypeName::Int32 || rightType.builtInTypeName == BuiltInTypeName::Int32)) {
                    return MakeImplicitEquality<BuiltInTypeName::Int32>(left, right, op, tokenRange);
                }

                // If either operand is of type uint16, the other operand is converted to uint16.
                if ((leftType.builtInTypeName == BuiltInTypeName::UInt16 || rightType.builtInTypeName == BuiltInTypeName::UInt16)) {
                    return MakeImplicitEquality<BuiltInTypeName::UInt16>(left, right, op, tokenRange);
                }

                // If either operand is of type int16, the other operand is converted to int16.
                if ((leftType.builtInTypeName == BuiltInTypeName::Int16 || rightType.builtInTypeName == BuiltInTypeName::Int16)) {
                    return MakeImplicitEquality<BuiltInTypeName::Int16>(left, right, op, tokenRange);
                }

                // If either operand is of type uint8, the other operand is converted to uint8.
                if ((leftType.builtInTypeName == BuiltInTypeName::UInt8 || rightType.builtInTypeName == BuiltInTypeName::UInt8)) {
                    return MakeImplicitEquality<BuiltInTypeName::UInt8>(left, right, op, tokenRange);
                }

                // If either operand is of type int8, the other operand is converted to int8.
                if ((leftType.builtInTypeName == BuiltInTypeName::Int8 || rightType.builtInTypeName == BuiltInTypeName::Int8)) {
                    return MakeImplicitEquality<BuiltInTypeName::Int8>(left, right, op, tokenRange);
                }

                diagnostics->LogErrorConst(fileInfo, GetLocation(tokenRange), "Unreachable, arithmetic type conversion failed");

                return allocator->New<BoolLiteralExpression>(false, fileInfo->fileId, tokenRange);
            }


            return nullptr;
        }

        Expression* MakeComparisonExpression(Expression* left, ComparisonOperator op, Expression* right, TokenRange tokenRange) {
            ResolvedType leftType = left->GetType();
            ResolvedType rightType = right->GetType();

            AssertCanRead(left);
            AssertCanRead(right);

            if (leftType.Equals(&rightType)) {

                if (!leftType.IsEnum() && !leftType.IsArithmetic() && leftType.builtInTypeName != BuiltInTypeName::Char) {

                    diagnostics->LogErrorConst(fileInfo, fileInfo->parseResult.GetLocationFromToken(tokenRange.start), "Comparison operands must be numeric or char type");

                    NumericLiteralValue v;
                    v.i32 = 0;
                    return allocator->New<ComparisonExpression>(
                        allocator->New<NumericLiteralExpression>(NumericLiteralTypeName::Int32, v, fileInfo->fileId, left->tokenRange),
                        op,
                        allocator->New<NumericLiteralExpression>(NumericLiteralTypeName::Int32, v, fileInfo->fileId, right->tokenRange),
                        fileInfo->fileId,
                        tokenRange
                    );

                }

                return allocator->New<ComparisonExpression>(left, op, right, fileInfo->fileId, tokenRange);
            }

            // we need to try to convert
            bool lhsNumeric = leftType.IsArithmetic();
            bool rhsNumeric = rightType.IsArithmetic();

            if (lhsNumeric && rhsNumeric) {

                if (leftType.IsUnsignedInteger() != rightType.IsUnsignedInteger()) {
                    // we can't compare these
                    LineColumn lc = fileInfo->parseResult.GetLocationFromToken(tokenRange.start);
                    diagnostics->LogErrorConst(fileInfo, lc, "Cannot compare signed and unsigned types, you need a cast here because data would be lost ");
                    return allocator->New<BoolLiteralExpression>(false, fileInfo->fileId, tokenRange);
                }

                // If either operand is of type double, the other operand is converted to double.
                if (leftType.builtInTypeName == BuiltInTypeName::Double || rightType.builtInTypeName == BuiltInTypeName::Double) {
                    return MakeImplicitComparison<BuiltInTypeName::Double>(left, right, op, tokenRange);
                }

                // If either operand is of type float, the other operand is converted to float.
                if (leftType.builtInTypeName == BuiltInTypeName::Float || rightType.builtInTypeName == BuiltInTypeName::Float) {
                    return MakeImplicitComparison<BuiltInTypeName::Float>(left, right, op, tokenRange);
                }

                // If either operand is of type ulong, the other operand is converted to ulong.
                if ((leftType.builtInTypeName == BuiltInTypeName::Ulong || rightType.builtInTypeName == BuiltInTypeName::Ulong)) {
                    return MakeImplicitComparison<BuiltInTypeName::Ulong>(left, right, op, tokenRange);
                }

                // If either operand is of type long, the other operand is converted to long.
                if ((leftType.builtInTypeName == BuiltInTypeName::Long || rightType.builtInTypeName == BuiltInTypeName::Long)) {
                    return MakeImplicitComparison<BuiltInTypeName::Long>(left, right, op, tokenRange);
                }

                // If either operand is of type uint, the other operand is converted to uint.
                if ((leftType.builtInTypeName == BuiltInTypeName::UInt32 || rightType.builtInTypeName == BuiltInTypeName::UInt32)) {
                    return MakeImplicitComparison<BuiltInTypeName::UInt32>(left, right, op, tokenRange);
                }

                // If either operand is of type int, the other operand is converted to int.
                if ((leftType.builtInTypeName == BuiltInTypeName::Int32 || rightType.builtInTypeName == BuiltInTypeName::Int32)) {
                    return MakeImplicitComparison<BuiltInTypeName::Int32>(left, right, op, tokenRange);
                }

                // If either operand is of type uint16, the other operand is converted to uint16.
                if ((leftType.builtInTypeName == BuiltInTypeName::UInt16 || rightType.builtInTypeName == BuiltInTypeName::UInt16)) {
                    return MakeImplicitComparison<BuiltInTypeName::UInt16>(left, right, op, tokenRange);
                }

                // If either operand is of type int16, the other operand is converted to int16.
                if ((leftType.builtInTypeName == BuiltInTypeName::Int16 || rightType.builtInTypeName == BuiltInTypeName::Int16)) {
                    return MakeImplicitComparison<BuiltInTypeName::Int16>(left, right, op, tokenRange);
                }

                // If either operand is of type uint8, the other operand is converted to uint8.
                if ((leftType.builtInTypeName == BuiltInTypeName::UInt8 || rightType.builtInTypeName == BuiltInTypeName::UInt8)) {
                    return MakeImplicitComparison<BuiltInTypeName::UInt8>(left, right, op, tokenRange);
                }

                // If either operand is of type int8, the other operand is converted to int8.
                if ((leftType.builtInTypeName == BuiltInTypeName::Int8 || rightType.builtInTypeName == BuiltInTypeName::Int8)) {
                    return MakeImplicitComparison<BuiltInTypeName::Int8>(left, right, op, tokenRange);
                }

                diagnostics->LogErrorConst(fileInfo, GetLocation(tokenRange), "Unreachable, arithmetic type conversion failed");

                return allocator->New<BoolLiteralExpression>(false, fileInfo->fileId, tokenRange);
            }

            // one or the other isn't numeric
            // todo -- figure out vector comparisons, probably not done with operators?
            diagnostics->LogErrorConst(fileInfo, fileInfo->parseResult.GetLocationFromToken(tokenRange.start), "Comparison operands must both be numeric, an enum, or the char type");
            return allocator->New<BoolLiteralExpression>(false, fileInfo->fileId, tokenRange);
        }

        VEP* MakeParameter(ParameterInfo* parameterInfo) {

            return allocator->New<VEP>(
                parameterInfo->GetName(),
                parameterInfo->type,
                DeclarationSource::Parameter,
                parameterInfo->passByModifier,
                true, // todo -- can support readonly parameters
                fileInfo->fileId,
                GetTokenRange(parameterInfo->nodeIndex)
            );

        }

        Scope* MakeBlockScope(bool isStatic) {
            Scope* retn = allocator->New<Scope>(fileInfo->fileId, TokenRange());
            retn->isStaticBoundary = isStatic;
            return retn;
        }

        void AssertCanRead(Expression* expression) {

            if (expression == nullptr) {
                return;
            }

            if (expression->expressionType == ExpressionType::PropertyAccess) {
                PropertyAccessExpression* propertyAccessExpression = (PropertyAccessExpression*) expression;
                if (!propertyAccessExpression->IsReadable()) {
                    diagnostics->LogErrorConst(fileInfo, fileInfo->parseResult.GetLocationFromToken(expression->tokenRange.start), "Property is not readable");
                }
            }
            else if (expression->expressionType == ExpressionType::IndexerAccess) {
                IndexerExpression* indexer = (IndexerExpression*) expression;
                if (!indexer->IsReadable()) {
                    diagnostics->LogErrorConst(fileInfo, fileInfo->parseResult.GetLocationFromToken(expression->tokenRange.start), "Indexer is not readable");
                }
            }

        }

        LineColumn GetLocation(TokenRange range) {
            return fileInfo->parseResult.GetLocationFromToken(range.start);
        }

        template<BuiltInTypeName typeName>
        Expression* MakeImplicitArithmetic(Expression* left, Expression* right, ArithmeticOperator op, TokenRange tokenRange) {
            ResolvedType leftType = left->GetType();
            ResolvedType rightType = right->GetType();

            if (rightType.builtInTypeName == typeName) {
                left = allocator->New<DirectCastExpression>(left, rightType, fileInfo->fileId, left->tokenRange);
            }
            else {
                right = allocator->New<DirectCastExpression>(right, leftType, fileInfo->fileId, right->tokenRange);
            }
            return allocator->New<ArithmeticExpression>(left, op, right, fileInfo->fileId, tokenRange);
        }

        template<BuiltInTypeName typeName>
        Expression* MakeImplicitComparison(Expression* left, Expression* right, ComparisonOperator op, TokenRange tokenRange) {
            ResolvedType leftType = left->GetType();
            ResolvedType rightType = right->GetType();

            if (rightType.builtInTypeName == typeName) {
                left = allocator->New<DirectCastExpression>(left, rightType, fileInfo->fileId, left->tokenRange);
            }
            else {
                right = allocator->New<DirectCastExpression>(right, leftType, fileInfo->fileId, right->tokenRange);
            }
            return allocator->New<ComparisonExpression>(left, op, right, fileInfo->fileId, tokenRange);
        }

        template<BuiltInTypeName typeName>
        Expression* MakeImplicitEquality(Expression* left, Expression* right, EqualityOperator op, TokenRange tokenRange) {
            ResolvedType leftType = left->GetType();
            ResolvedType rightType = right->GetType();

            if (rightType.builtInTypeName == typeName) {
                left = allocator->New<DirectCastExpression>(left, rightType, fileInfo->fileId, left->tokenRange);
            }
            else {
                right = allocator->New<DirectCastExpression>(right, leftType, fileInfo->fileId, right->tokenRange);
            }
            return allocator->New<EqualityExpression>(left, op, right, fileInfo->fileId, tokenRange);
        }

        Expression* ArithmeticOperation(Expression* left, ArithmeticOperator op, Expression* right, TokenRange tokenRange) {
            ResolvedType leftType = left->GetType();
            ResolvedType rightType = right->GetType();

            AssertCanRead(left);
            AssertCanRead(right);

            if (leftType.Equals(&rightType)) {

                if (leftType.IsArithmetic()) {
                    return allocator->New<ArithmeticExpression>(left, op, right, fileInfo->fileId, tokenRange);
                }

                if (leftType.IsVector()) {
                    return allocator->New<VectorArithmeticExpression>(left, op, right, fileInfo->fileId, tokenRange);
                }

                diagnostics->LogErrorConst(fileInfo, GetLocation(tokenRange), "Expected operands of arithmetic expression to be vectors or numbers");

                return left;
            }

            if (leftType.IsArithmetic() && rightType.IsArithmetic()) {

                // If either operand is of type double, the other operand is converted to double.
                if (leftType.builtInTypeName == BuiltInTypeName::Double || rightType.builtInTypeName == BuiltInTypeName::Double) {
                    return MakeImplicitArithmetic<BuiltInTypeName::Double>(left, right, op, tokenRange);
                }

                // If either operand is of type float, the other operand is converted to float.
                if (leftType.builtInTypeName == BuiltInTypeName::Float || rightType.builtInTypeName == BuiltInTypeName::Float) {
                    return MakeImplicitArithmetic<BuiltInTypeName::Float>(left, right, op, tokenRange);
                }

                // If either operand is of type ulong, the other operand is converted to ulong.
                if ((leftType.builtInTypeName == BuiltInTypeName::Ulong || rightType.builtInTypeName == BuiltInTypeName::Ulong)) {
                    return MakeImplicitArithmetic<BuiltInTypeName::Ulong>(left, right, op, tokenRange);
                }

                // If either operand is of type long, the other operand is converted to long.
                if ((leftType.builtInTypeName == BuiltInTypeName::Long || rightType.builtInTypeName == BuiltInTypeName::Long)) {
                    return MakeImplicitArithmetic<BuiltInTypeName::Long>(left, right, op, tokenRange);
                }

                // If either operand is of type uint, the other operand is converted to uint.
                if ((leftType.builtInTypeName == BuiltInTypeName::UInt32 || rightType.builtInTypeName == BuiltInTypeName::UInt32)) {
                    return MakeImplicitArithmetic<BuiltInTypeName::UInt32>(left, right, op, tokenRange);
                }

                // If either operand is of type int, the other operand is converted to int.
                if ((leftType.builtInTypeName == BuiltInTypeName::Int32 || rightType.builtInTypeName == BuiltInTypeName::Int32)) {
                    return MakeImplicitArithmetic<BuiltInTypeName::Int32>(left, right, op, tokenRange);
                }

                // If either operand is of type uint16, the other operand is converted to uint16.
                if ((leftType.builtInTypeName == BuiltInTypeName::UInt16 || rightType.builtInTypeName == BuiltInTypeName::UInt16)) {
                    return MakeImplicitArithmetic<BuiltInTypeName::UInt16>(left, right, op, tokenRange);
                }

                // If either operand is of type int16, the other operand is converted to int16.
                if ((leftType.builtInTypeName == BuiltInTypeName::Int16 || rightType.builtInTypeName == BuiltInTypeName::Int16)) {
                    return MakeImplicitArithmetic<BuiltInTypeName::Int16>(left, right, op, tokenRange);
                }

                // If either operand is of type uint8, the other operand is converted to uint8.
                if ((leftType.builtInTypeName == BuiltInTypeName::UInt8 || rightType.builtInTypeName == BuiltInTypeName::UInt8)) {
                    return MakeImplicitArithmetic<BuiltInTypeName::UInt8>(left, right, op, tokenRange);
                }

                // If either operand is of type int8, the other operand is converted to int8.
                if ((leftType.builtInTypeName == BuiltInTypeName::Int8 || rightType.builtInTypeName == BuiltInTypeName::Int8)) {
                    return MakeImplicitArithmetic<BuiltInTypeName::Int8>(left, right, op, tokenRange);
                }

                diagnostics->LogErrorConst(fileInfo, GetLocation(tokenRange), "Unreachable, arithmetic type conversion failed");

                return left;

            }

            // If either operand is of type int, the other operand is converted to int.
            if ((leftType.builtInTypeName == BuiltInTypeName::Int32 || rightType.builtInTypeName == BuiltInTypeName::Int32)) {
                return MakeImplicitArithmetic<BuiltInTypeName::Int32>(left, right, op, tokenRange);
            }

            // If one operand is of type ulong, and the other operand is of any numeric type the ulong operand is converted to the type of the other operand.
            if (!leftType.Equals(&rightType) && TypeUtil::IsImplicitNumericConversion(leftType, rightType)) {
                right = allocator->New<DirectCastExpression>(right, leftType, fileInfo->fileId, right->tokenRange);
                rightType = leftType;
            }

            if (!leftType.Equals(&rightType)) {
                diagnostics->LogErrorConst(fileInfo, GetLocation(tokenRange), "Invalid arithmetic expression, types do not match and are not implicitly convertable");
                return left;
            }

            return allocator->New<ArithmeticExpression>(left, op, right, fileInfo->fileId, tokenRange);

        }

        VEP* AddLocalVariable(ResolvedType & type, FixedCharSpan span, TokenRange tokenRange) {
            // todo -- search in scopes for name conflict, check static as well, we can stop when we hit a static or non static boundary
            VEP* vep = allocator->New<VEP>(span, type, DeclarationSource::LocalVariable, ArgumentPassByModifier::None, true, fileInfo->fileId, tokenRange);
            currentScope->AddVep(vep, poolingAllocator);
            return vep;
        }

    };


}