#pragma once

#include "PooledBlockAllocator.h"

namespace Alchemy::Compilation {

    struct ExpressionVisitor;

    enum class ExpressionType : uint8 {
        FieldAccess,
        Parameter,
        DebugInfo,
        Scope,
        ByRef,
        VEP,
        Comparison,
        NumericLiteral,
        BoolLiteral,
        DirectCast,
        PropertyAccess,
        IndexerAccess,
        Arithmetic,
        VectorArithmetic,
        Equality,
        NullLiteral,
        DefaultLiteral,
        IfStatement,
        MethodGroupAccess,
        SemanticError,
        Argument,
        StaticMethodCall,
        InstanceMethodCall,
    };


    struct Expression {

        TokenRange tokenRange;
        ExpressionType expressionType;
        StorageClass storageClass; // todo -- probably find this a new home

    private:
        uint16 fileId;

    public:
        explicit Expression(ExpressionType expressionType, int32 fileId, TokenRange range)
                : expressionType(expressionType)
                  , tokenRange(range)
                  , storageClass(StorageClass::Default)
                  , fileId(fileId) {
            // this->fileId[0] = (fileId >> 16) & 0xFF;
            // this->fileId[1] = (fileId >> 8) & 0xFF;
            // this->fileId[2] = fileId & 0xFF;
        }

        int32 GetFileId() const {
            // int32 fileId32 = 0;
            // fileId32 |= fileId[0] << 16;
            // fileId32 |= fileId[1] << 8;
            // fileId32 |= fileId[2];
            // return fileId32;
            return fileId;
        }

        virtual ResolvedType GetType() {
            return ResolvedType(BuiltInTypeName::Void);
        }

        virtual int32 ToString(char* buffer) {
            return 0;
        }

        virtual Expression* Reduce() {
            return this;
        }

        virtual Expression* VisitChildren(ExpressionVisitor* visitor) {
            return this;
        }

        virtual Expression* Accept(ExpressionVisitor* visitor) {
            return this;
        }

    };

//    struct DebugInfoExpression : Expression {
//
//        Expression* proxy;
//        FileInfo* fileInfo;
//        int32 lineStart;
//        int32 columnStart;
//        int32 lineEnd;
//        int32 columnEnd;
//
//        DebugInfoExpression(Expression* proxy, FileInfo* fileInfo, TokenRange tokenRange)
//            : Expression(ExpressionType::DebugInfo)
//            , proxy(proxy)
//            , fileInfo(fileInfo)
//            , lineStart(lineStart)
//            , lineEnd(lineEnd)
//            , columnStart(columnStart)
//            , columnEnd(columnEnd) {}
//
//        ResolvedType* GetType() override {
//            return proxy->GetType();
//        }
//
//    };

    enum class DeclarationSource : uint8 {

        LocalVariable,
        Field,
        Property,
        Remember,
        LocalConst,
        Const,
        StaticField,
        StaticProperty,
        Parameter,
        ContextListParameter

    };


    struct VEP : Expression {

        char* name;
        ResolvedType resolvedType;
        uint16 nameLength;
        DeclarationSource declarationSource;
        bool isClosedOver;
        bool isAssignable;
        ArgumentPassByModifier passByModifier; // I'm not sure we need this, defer to wrapper expressions & variable declaration probably

        VEP(FixedCharSpan name, ResolvedType resolvedType, DeclarationSource declarationSource, ArgumentPassByModifier passByModifier, bool isAssignable, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::VEP, fileId, tokenRange)
                  , name(name.ptr)
                  , nameLength(name.size)
                  , resolvedType(resolvedType)
                  , declarationSource(declarationSource)
                  , isClosedOver(false)
                  , isAssignable(isAssignable)
                  , passByModifier(passByModifier) {}

        FixedCharSpan GetName() const {
            return FixedCharSpan(name, nameLength);
        }

    };

    struct Scope : Expression {

        VEP** vepList;
        Expression** statements;
        Scope* parent{};
        ResolvedType returnType;
        VEP* instanceExpression;
        StorageClass returnStorage;
        TypeAccessModifier returnAccessModifier;
        uint16 vepCount{};
        uint16 vepCapacity{};
        uint16 statementCount{};
        uint16 statementCapacity{};
        bool isStaticBoundary;
        bool isClosureContextBoundary;

        Scope(ResolvedType returnType, StorageClass returnStorage, TypeAccessModifier returnAccessModifier, bool isStatic, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::Scope, fileId, tokenRange)
                  , isStaticBoundary(isStatic)
                  , returnAccessModifier(returnAccessModifier)
                  , returnStorage(returnStorage)
                  , returnType(returnType) {
        }

        Scope(int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::Scope, fileId, tokenRange)
                  , isStaticBoundary(false)
                  , returnAccessModifier(TypeAccessModifier::Default)
                  , returnStorage(StorageClass::Default)
                  , returnType(nullptr) {}

        bool IsRootScope() const {
            return parent == nullptr;
        }

        void AddStatement(Expression* expression, PoolingAllocator* allocator) {
            if (statementCount + 1 > statementCapacity) {
                int32 oldCapacity = statementCapacity;
                if (statementCapacity == 0) {
                    statementCount = 2;
                }
                Expression** newList = allocator->AllocatePooledUncleared<Expression*>(statementCapacity * 2);
                memcpy(newList, statements, statementCount);
                allocator->Free(statements, oldCapacity);
                statements = newList;
                statementCapacity *= 2;
            }

            statements[statementCount++] = expression;
        }

        void AddVep(VEP* vep, PoolingAllocator* allocator) {

            if (vepCount + 1 > vepCapacity) {
                int32 oldCapacity = vepCapacity;
                if (vepCapacity == 0) {
                    vepCapacity = 2;
                }
                VEP** newList = allocator->AllocatePooledUncleared<VEP*>(vepCapacity * 2);
                memcpy(newList, vepList, vepCount);
                allocator->Free(vepList, oldCapacity);
                vepList = newList;
                vepCapacity *= 2;
            }

            vepList[vepCount++] = vep;

        }

    };

    struct IfStatementExpression : Expression {

        IfStatementExpression(int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::IfStatement, fileId, tokenRange) {}

        ResolvedType GetType() override {
            return ResolvedType(BuiltInTypeName::Void);
        }

    };

    struct ByRefExpression : Expression {

        ArgumentPassByModifier modifier;
        Expression* expression;

        ByRefExpression(ArgumentPassByModifier modifier, Expression* expression, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::ByRef, fileId, tokenRange)
                  , modifier(modifier)
                  , expression(expression) {}

        ResolvedType GetType() override {
            return expression->GetType(); // todo -- need modify this to be ref/out i think
        }

    };


    struct IndexerExpression : Expression {

        Expression* instance;
        IndexerInfo* indexerInfo;

        IndexerExpression(Expression* instance, IndexerInfo* indexerInfo, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::IndexerAccess, fileId, tokenRange)
                  , instance(instance)
                  , indexerInfo(indexerInfo) {}

        bool IsReadable() const {
            return indexerInfo->getterNodeIndex.IsValid();
        }

        ResolvedType GetType() override {
            return indexerInfo->type;
        }
    };

    struct PropertyAccessExpression : Expression {

        Expression* instance;
        PropertyInfo* propertyInfo;

        PropertyAccessExpression(Expression* instance, PropertyInfo* propertyInfo, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::PropertyAccess, fileId, tokenRange)
                  , instance(instance)
                  , propertyInfo(propertyInfo) {}

        bool IsReadable() const {
            return propertyInfo->getterNodeIndex.IsValid();
        }

        ResolvedType GetType() override {
            return propertyInfo->type;
        }
    };

    struct SemanticErrorExpression : Expression {

        SemanticErrorExpression(int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::SemanticError, fileId, tokenRange) {}

        ResolvedType GetType() override {
            return ResolvedType::Null();
        }

    };

    struct MethodGroupAccessExpression : Expression {

        Expression* instance;
        MethodGroup methodGroup;

        MethodGroupAccessExpression(Expression* instance, MethodGroup methodGroup, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::MethodGroupAccess, fileId, tokenRange)
                  , instance(instance)
                  , methodGroup(methodGroup) {}

        ResolvedType GetType() override {
            return ResolvedType::MethodGroup();
        }

    };

    struct FieldAccessExpression : Expression {

        Expression* instance;
        FieldInfo* fieldInfo;

        FieldAccessExpression(Expression* instance, FieldInfo* fieldInfo, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::FieldAccess, fileId, tokenRange)
                  , instance(instance)
                  , fieldInfo(fieldInfo) {}

        ResolvedType GetType() override {
            return fieldInfo->type;
        }

    };


    struct DirectCastExpression : Expression {

        ResolvedType castType;
        Expression* expression;

        DirectCastExpression(Expression* expression, ResolvedType castType, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::DirectCast, fileId, tokenRange)
                  , castType(castType)
                  , expression(expression) {}

        ResolvedType GetType() override {
            return castType;
        }

    };

    struct NullLiteralExpression : Expression {

        ResolvedType nullType;

        NullLiteralExpression(ResolvedType nullType, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::NullLiteral, fileId, tokenRange)
                  , nullType(nullType) {}

        ResolvedType GetType() {
            return nullType;
        }

    };

    struct DefaultLiteralExpression : Expression {

        ResolvedType defaultType;

        DefaultLiteralExpression(ResolvedType defaultType, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::DefaultLiteral, fileId, tokenRange)
                  , defaultType(defaultType) {}

        ResolvedType GetType() {
            return defaultType;
        }

    };

    struct ArgumentExpression : Expression {

        ArgumentPassByModifier passByModifier;

        Expression* expression;

        ArgumentExpression(ArgumentPassByModifier passByModifier, Expression* expression, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::Argument, fileId, tokenRange)
                  , passByModifier(passByModifier)
                  , expression(expression) {}

        ResolvedType GetType() override {
            return expression->GetType();
        }

    };

    struct StaticMethodCall : Expression {

        MethodInfo * methodInfo;
        CheckedArray<ArgumentExpression*> arguments;

        StaticMethodCall(MethodInfo * methodInfo, CheckedArray< ArgumentExpression*> arguments, int32 fileId, TokenRange tokenRange)
            : Expression(ExpressionType::StaticMethodCall, fileId, tokenRange)
            , methodInfo(methodInfo)
            , arguments(arguments)
        {}

    };

    struct InstanceMethodCall final : Expression {

        Expression * instance;
        MethodInfo * methodInfo;
        CheckedArray<ArgumentExpression*> arguments;

        InstanceMethodCall(Expression * instance, MethodInfo * methodInfo, CheckedArray<ArgumentExpression*> arguments, int32 fileId, TokenRange tokenRange)
        : Expression(ExpressionType::InstanceMethodCall, fileId, tokenRange)
        , instance(instance)
        , methodInfo(methodInfo)
        , arguments(arguments)
        {}

    };

    struct BoolLiteralExpression : Expression {

        bool value;

        BoolLiteralExpression(bool value, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::BoolLiteral, fileId, tokenRange)
                  , value(value) {}

        ResolvedType GetType() {
            return ResolvedType(BuiltInTypeName::Bool);
        }

    };

    struct NumericLiteralExpression : Expression {

        NumericLiteralTypeName numericType;
        NumericLiteralValue value;

        NumericLiteralExpression(NumericLiteralTypeName numericType, NumericLiteralValue value, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::NumericLiteral, fileId, tokenRange)
                  , numericType(numericType)
                  , value(value) {}

        ResolvedType GetType() {

            switch (numericType) {

                case NumericLiteralTypeName::Int32:
                    return ResolvedType(BuiltInTypeName::Int32);

                case NumericLiteralTypeName::Int64:
                    return ResolvedType(BuiltInTypeName::Int64);

                case NumericLiteralTypeName::UInt32:
                    return ResolvedType(BuiltInTypeName::UInt32);

                case NumericLiteralTypeName::UInt64:
                    return ResolvedType(BuiltInTypeName::UInt64);

                case NumericLiteralTypeName::Float:
                    return ResolvedType(BuiltInTypeName::Float);

                case NumericLiteralTypeName::Double:
                    return ResolvedType(BuiltInTypeName::Double);

                default: {
                    return ResolvedType(BuiltInTypeName::Int32);
                }
            }

        }

    };

    enum class ArithmeticOperator {
        Add,
        Subtract,
        Multiply,
        Divide,
        Modulus
    };

    enum class ConditionalOperator {
        Or,
        And
    };

    enum class EqualityOperator {
        Equal,
        NotEqual
    };

    enum class ComparisonOperator {
        GreaterThan,
        LessThan,
        GreaterThanOrEqualTo,
        LessThanOrEqualTo
    };

    struct ArithmeticExpression : Expression {

        Expression* left;
        Expression* right;
        ArithmeticOperator op;

        ArithmeticExpression(Expression* left, ArithmeticOperator op, Expression* right, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::Arithmetic, fileId, tokenRange)
                  , left(left)
                  , right(right)
                  , op(op) {}

        ResolvedType GetType() override {
            return left->GetType();
        }

    };

    struct VectorArithmeticExpression : Expression {

        Expression* left;
        Expression* right;
        ArithmeticOperator op;

        VectorArithmeticExpression(Expression* left, ArithmeticOperator op, Expression* right, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::VectorArithmetic, fileId, tokenRange)
                  , left(left)
                  , right(right)
                  , op(op) {}

        ResolvedType GetType() override {
            return left->GetType();
        }

    };

    struct EqualityExpression : Expression {

        Expression* lhs;
        Expression* rhs;
        EqualityOperator operatorType;

        EqualityExpression(Expression* lhs, EqualityOperator operatorType, Expression* rhs, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::Equality, fileId, tokenRange)
                  , lhs(lhs)
                  , rhs(rhs)
                  , operatorType(operatorType) {}

        ResolvedType GetType() override {
            return ResolvedType(BuiltInTypeName::Bool);
        }
    };

    struct ComparisonExpression : Expression {
        Expression* lhs;
        Expression* rhs;
        ComparisonOperator operatorType;

        ComparisonExpression(Expression* lhs, ComparisonOperator operatorType, Expression* rhs, int32 fileId, TokenRange tokenRange)
                : Expression(ExpressionType::Comparison, fileId, tokenRange)
                  , lhs(lhs)
                  , rhs(rhs)
                  , operatorType(operatorType) {}

        ResolvedType GetType() override {
            return ResolvedType(BuiltInTypeName::Bool);
        }

    };

    struct ExpressionVisitor {

        Expression* Visit(Expression* node) {
            if (node != nullptr) {
                node->Accept(this);
            }
            return nullptr;
        }

    };

}