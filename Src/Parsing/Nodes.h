#pragma once

#include <cstring>
#include "./NodeType.h"
#include "./NodeIndex.h"
#include "./ParseTypes.h"
#include "../Util/FixedCharSpan.h"

namespace Alchemy {

    struct NodeBase {
        NodeType nodeType;
        uint8 padding;
        uint16 nodeIndex;
        uint16 tokenStart;
        uint16 tokenEnd;

        explicit NodeBase(NodeType nodeType) : nodeType(nodeType) {}

        TokenRange GetTokenRange() const {
            return TokenRange(tokenStart, tokenEnd);
        }

    };

    struct DeclarationNode : NodeBase {

        NodeIndex<DeclarationNode> next;

        explicit DeclarationNode(NodeType nodeType) : NodeBase(nodeType) {}

    };

    struct ExpressionNode : NodeBase {
        NodeIndex<ExpressionNode> next;

        explicit ExpressionNode(NodeType nodeType)
            : NodeBase(nodeType)
            , next(NodeIndex<ExpressionNode>(0)) {}

    };

    struct FileNode : NodeBase {

        NodeIndex<DeclarationNode> firstDeclaration;

        FileNode()
            : NodeBase(NodeType::File)
            , firstDeclaration(0) {}

    };

    struct ArrayRankNode : NodeBase {

        bool isNullable;

        explicit ArrayRankNode(bool isNullable) : NodeBase(NodeType::ArrayRank), isNullable(isNullable) {}

    };

    struct TypeArgumentListNode;
    struct NamespaceOrTypeNameNode;
    struct IdentifierNode;

    struct TypePathNode : NodeBase {

        NodeIndex<ArrayRankNode> arrayRank;
        bool isNullable;
        BuiltInTypeName builtInTypeName;
        NodeIndex<IdentifierNode> namePath;
        NodeIndex<TypeArgumentListNode> typeArguments;
        NodeIndex<TypePathNode> next;

        explicit TypePathNode(NodeIndex<IdentifierNode> namePath, BuiltInTypeName builtInTypeName, NodeIndex<TypeArgumentListNode> typeArguments, NodeIndex<ArrayRankNode> arrayRank, bool isNullable)
            : NodeBase(NodeType::TypePath)
            , namePath(namePath)
            , builtInTypeName(builtInTypeName)
            , typeArguments(typeArguments)
            , arrayRank(arrayRank)
            , isNullable(isNullable)
            , next(0) {}

    };

    struct TypeListNode : NodeBase {

        NodeIndex<TypePathNode> first;

        explicit TypeListNode(NodeIndex<TypePathNode> first)
            : NodeBase(NodeType::TypeList)
            , first(first) {}

    };

    struct ErrorNode : NodeBase {

        char* message;
        int32 messageLength;

        explicit ErrorNode(char* message, int32 messageLength) : NodeBase(NodeType::Error), message(message), messageLength(messageLength) {}

    };

    struct AssignmentExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> rhs;
        AssignmentOperator op;
        NodeIndex<ExpressionNode> lhs;

        explicit AssignmentExpressionNode(NodeIndex<ExpressionNode> rhs, AssignmentOperator op, NodeIndex<ExpressionNode> lhs)
            : ExpressionNode(NodeType::AssignmentExpression)
            , op(op)
            , lhs(lhs)
            , rhs(rhs) {}
    };

    struct FormalParameterNode : NodeBase {

        StorageClass storage;
        ArgumentPassByModifier passByModifier;
        NodeIndex<TypePathNode> type;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> defaultValue;
        NodeIndex<FormalParameterNode> next;

        explicit FormalParameterNode(StorageClass storage, ArgumentPassByModifier passByModifier, NodeIndex<TypePathNode> type, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> defaultValue)
            : NodeBase(NodeType::FormalParameter)
            , storage(storage)
            , passByModifier(passByModifier)
            , type(type)
            , identifier(identifier)
            , defaultValue(defaultValue) {}

    };

    struct FormalParameterListNode : NodeBase {

        NodeIndex<FormalParameterNode> listStart;

        explicit FormalParameterListNode(NodeIndex<FormalParameterNode> listStart)
            : NodeBase(NodeType::FormalParameterList)
            , listStart(listStart) {}

    };

    struct StatementNode : NodeBase {

        NodeIndex<StatementNode> next;

        explicit StatementNode(NodeType nodeType) : NodeBase(nodeType) {}

    };

    struct ReturnStatementNode : StatementNode {

        NodeIndex<ExpressionNode> expression;

        explicit ReturnStatementNode(NodeIndex<ExpressionNode> expression) : StatementNode(NodeType::ReturnStatement), expression(expression) {}

    };

    struct ContinueStatementNode : StatementNode {

        explicit ContinueStatementNode() : StatementNode(NodeType::ContinueStatement) {}

    };

    struct BreakStatementNode : StatementNode {

        explicit BreakStatementNode() : StatementNode(NodeType::BreakStatement) {}

    };

    struct EmptyStatementNode : StatementNode {
        explicit EmptyStatementNode() : StatementNode(NodeType::EmptyStatement) {}
    };


    struct BlockNode : StatementNode {

        NodeIndex<StatementNode> firstStatement;

        explicit BlockNode(NodeIndex<StatementNode> firstStatement)
            : StatementNode(NodeType::BlockStatement)
            , firstStatement(firstStatement) {}

    };

    struct IdentifierNode;

    struct ExpressionListNode : NodeBase {

        NodeIndex<ExpressionNode> first;

        explicit ExpressionListNode(NodeIndex<ExpressionNode> first)
            : NodeBase(NodeType::ExpressionList)
            , first(first) {}

    };

    struct ContextListNode : NodeBase {

        NodeIndex<IdentifierNode> first;

        explicit ContextListNode(NodeIndex<IdentifierNode> first)
            : NodeBase(NodeType::ContextList)
            , first(first) {}

    };

    struct ForLoopIteratorNode : NodeBase {

        NodeIndex<ExpressionNode> first;

        explicit ForLoopIteratorNode(NodeIndex<ExpressionNode> first)
            : NodeBase(NodeType::ForLoopIterator)
            , first(first) {}

    };

    struct CatchClauseNode : NodeBase {
        NodeIndex<BlockNode> block;
        NodeIndex<ContextListNode> contextList;

        CatchClauseNode(NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> blockNode)
            : NodeBase(NodeType::CatchClause)
            , contextList(contextList)
            , block(blockNode) {}

    };

    struct TryBlockNode : StatementNode {

        NodeIndex<BlockNode> block;
        NodeIndex<CatchClauseNode> catchClause;
        NodeIndex<BlockNode> finallyBlock;

        explicit TryBlockNode(NodeIndex<BlockNode> block, NodeIndex<CatchClauseNode> catchClause, NodeIndex<BlockNode> finallyBlock)
            : StatementNode(NodeType::TryBlock)
            , block(block)
            , catchClause(catchClause)
            , finallyBlock(finallyBlock) {}

    };

    struct TryExpressionNode : StatementNode {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<CatchClauseNode> catchClause;

        explicit TryExpressionNode(NodeIndex<ExpressionNode> expression, NodeIndex<CatchClauseNode> catchClause)
            : StatementNode(NodeType::TryExpression)
            , expression(expression)
            , catchClause(catchClause) {}

    };

    struct SwitchSectionLabelNode : NodeBase {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<ExpressionNode> caseGuard;
        bool isDefault;
        NodeIndex<SwitchSectionLabelNode> next;

        explicit SwitchSectionLabelNode(NodeIndex<ExpressionNode> expression, NodeIndex<ExpressionNode> caseGuard)
            : NodeBase(NodeType::SwitchLabel)
            , isDefault(false)
            , expression(expression)
            , caseGuard(caseGuard) {}

        explicit SwitchSectionLabelNode(bool isDefault)
            : NodeBase(NodeType::SwitchLabel)
            , isDefault(isDefault)
            , expression(NodeIndex<ExpressionNode>(0))
            , caseGuard(NodeIndex<ExpressionNode>(0)) {}


    };

    struct SwitchSectionNode : NodeBase {

        NodeIndex<SwitchSectionLabelNode> firstLabel;
        NodeIndex<StatementNode> firstStatement;

        explicit SwitchSectionNode(NodeIndex<SwitchSectionLabelNode> firstLabel, NodeIndex<StatementNode> firstStatement)
            : NodeBase(NodeType::SwitchSection)
            , firstLabel(firstLabel)
            , firstStatement(firstStatement) {}

    };

    struct SwitchStatementNode : StatementNode {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<ContextListNode> contextList;
        NodeIndex<SwitchSectionNode> firstSection;

        explicit SwitchStatementNode(NodeIndex<ExpressionNode> expression, NodeIndex<ContextListNode> contextList, NodeIndex<SwitchSectionNode> firstSection)
            : StatementNode(NodeType::SwitchStatement)
            , expression(expression)
            , contextList(contextList)
            , firstSection(firstSection) {}

    };

    struct ForEachLoopStatementNode : StatementNode {

        NodeIndex<ContextListNode> context;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<StatementNode> body;

        explicit ForEachLoopStatementNode(NodeIndex<ExpressionNode> expression, NodeIndex<ContextListNode> context, NodeIndex<StatementNode> body)
            : StatementNode(NodeType::ForEachLoopStatement)
            , expression(expression)
            , context(context)
            , body(body) {}

    };

    struct ForInitializerListNode;

    struct ForLoopStatementNode : StatementNode {
        NodeIndex<ForInitializerListNode> initializer;
        NodeIndex<ExpressionNode> condition;
        NodeIndex<ForLoopIteratorNode> iterator;
        NodeIndex<StatementNode> body;

        explicit ForLoopStatementNode(NodeIndex<ForInitializerListNode> initializer, NodeIndex<ExpressionNode> condition, NodeIndex<ForLoopIteratorNode> iterator, NodeIndex<StatementNode> body)
            : StatementNode(NodeType::ForLoopStatement)
            , initializer(initializer)
            , condition(condition)
            , iterator(iterator)
            , body(body) {}

    };

    struct WhileStatementNode : StatementNode {
        NodeIndex<ExpressionListNode> conditions;
        NodeIndex<ContextListNode> context;
        NodeIndex<StatementNode> body;

        explicit WhileStatementNode(NodeIndex<ExpressionListNode> conditions, NodeIndex<ContextListNode> context, NodeIndex<StatementNode> body)
            : StatementNode(NodeType::WhileStatement)
            , conditions(conditions)
            , context(context)
            , body(body) {}

    };

    struct DoWhileStatementNode : StatementNode {
        NodeIndex<StatementNode> body;
        NodeIndex<ExpressionNode> condition;

        explicit DoWhileStatementNode(NodeIndex<StatementNode> body, NodeIndex<ExpressionNode> condition)
            : StatementNode(NodeType::DoWhileStatement)
            , body(body)
            , condition(condition) {}

    };

    struct IfStatementNode : StatementNode {

        NodeIndex<ExpressionListNode> conditions;
        NodeIndex<ContextListNode> context;
        NodeIndex<StatementNode> body;
        NodeIndex<StatementNode> elseCase;

        IfStatementNode(NodeIndex<ExpressionListNode> conditions, NodeIndex<ContextListNode> context, NodeIndex<StatementNode> body, NodeIndex<StatementNode> elseCase)
            : StatementNode(NodeType::IfStatement)
            , conditions(conditions)
            , context(context)
            , body(body)
            , elseCase(elseCase) {}

    };

    struct UsingStatementBlockNode : StatementNode {

        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionListNode> expressions;
        NodeIndex<ContextListNode> contextList;

        explicit UsingStatementBlockNode(NodeIndex<ExpressionListNode> expressions, NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> block)
            : StatementNode(NodeType::UsingStatementBlock)
            , expressions(expressions)
            , block(block)
            , contextList(contextList) {}

    };

    struct UsingStatementScopeNode : StatementNode {

        NodeIndex<ExpressionNode> expression;

        explicit UsingStatementScopeNode(NodeIndex<ExpressionNode> expression)
            : StatementNode(NodeType::UsingStatementScope)
            , expression(expression) {}

    };

    struct UsingStatementNode : StatementNode {

        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionListNode> expressions;
        NodeIndex<ContextListNode> contextList;

        explicit UsingStatementNode(NodeIndex<ExpressionListNode> expressions, NodeIndex<BlockNode> block, NodeIndex<ContextListNode> contextList)
            : StatementNode(NodeType::UsingStatementBlock)
            , expressions(expressions)
            , block(block)
            , contextList(contextList) {}

    };

    struct WithStatementNode : StatementNode {

        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionListNode> expressionList;
        NodeIndex<ContextListNode> contextList;

        explicit WithStatementNode(NodeIndex<ExpressionListNode> expressionList, NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> block)
            : StatementNode(NodeType::WithStatement)
            , expressionList(expressionList)
            , contextList(contextList)
            , block(block) {}

    };

    struct LocalVariableInitializerNode : NodeBase {

        bool isByRef;
        NodeIndex<ExpressionNode> expression;

        explicit LocalVariableInitializerNode(bool isByRef, NodeIndex<ExpressionNode> expression)
            : NodeBase(NodeType::LocalVariableDeclaration)
            , isByRef(isByRef)
            , expression(expression) {}

    };

    struct LocalVariableTypeNode : NodeBase {

        StorageClass storageClass;
        NodeIndex<TypePathNode> typePath;
        bool isByRef;
        bool isVarTyped;

        explicit LocalVariableTypeNode(StorageClass storageClass, bool isByRef, bool isVarTyped, NodeIndex<TypePathNode> typePath)
            : NodeBase(NodeType::LocalVariableType)
            , storageClass(storageClass)
            , isByRef(isByRef)
            , isVarTyped(isVarTyped)
            , typePath(typePath) {}

    };

    struct ForInitializerNode : NodeBase {
        NodeIndex<ForInitializerNode> next;
        NodeIndex<StatementNode> variableDeclaration;
        NodeIndex<ExpressionNode> expression;

        explicit ForInitializerNode(NodeIndex<StatementNode> variableDeclaration)
            : NodeBase(NodeType::ForLoopInitializer)
            , variableDeclaration(variableDeclaration)
            , expression(NodeIndex<ExpressionNode>(0))
            , next(NodeIndex<ForInitializerNode>(0)) {}

        explicit ForInitializerNode(NodeIndex<ExpressionNode> expression)
            : NodeBase(NodeType::ForLoopInitializer)
            , variableDeclaration(NodeIndex<StatementNode>(0))
            , expression(expression)
            , next(NodeIndex<ForInitializerNode>(0)) {}

    };

    struct ForInitializerListNode : NodeBase {
        NodeIndex<ForInitializerNode> first;

        explicit ForInitializerListNode(NodeIndex<ForInitializerNode> first)
            : NodeBase(NodeType::ForInitializerList)
            , first(first) {}

    };

    struct LocalVariableDeclarationNode : StatementNode {

        NodeIndex<LocalVariableTypeNode> variableType;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<LocalVariableInitializerNode> initializer;

        explicit LocalVariableDeclarationNode(NodeIndex<LocalVariableTypeNode> variableType, NodeIndex<IdentifierNode> identifier, NodeIndex<LocalVariableInitializerNode> initializer)
            : StatementNode(NodeType::LocalVariableDeclaration)
            , variableType(variableType)
            , identifier(identifier)
            , initializer(initializer) {}

    };

    struct LocalConstantDeclarationNode : StatementNode {

        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> expression;

        explicit LocalConstantDeclarationNode(NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> expression)
            : StatementNode(NodeType::LocalConstantDeclaration)
            , typePath(typePath)
            , identifier(identifier)
            , expression(expression) {}

    };

    struct ModifierListNode : NodeBase {

        DeclarationModifier modifiers;

        explicit ModifierListNode(DeclarationModifier modifiers)
            : NodeBase(NodeType::ModifierList)
            , modifiers(modifiers) {}

    };

    struct ConstantDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> expression;

        explicit ConstantDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> expression)
            : DeclarationNode(NodeType::ConstantDeclaration)
            , modifiers(modifiers)
            , typePath(typePath)
            , identifier(identifier)
            , expression(expression) {}

    };

    struct SwitchExpressionArmNode : NodeBase {

        NodeIndex<ExpressionNode> condition;
        NodeIndex<ExpressionNode> caseGuard;
        NodeIndex<ExpressionNode> action;
        NodeIndex<SwitchExpressionArmNode> next;

        explicit SwitchExpressionArmNode(NodeIndex<ExpressionNode> condition, NodeIndex<ExpressionNode> caseGuard, NodeIndex<ExpressionNode> action)
            : NodeBase(NodeType::SwitchExpressionArm)
            , condition(condition)
            , caseGuard(caseGuard)
            , action(action)
            , next(NodeIndex<SwitchExpressionArmNode>(0)) {}

    };

    struct SwitchExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> switchValue;
        NodeIndex<SwitchExpressionArmNode> firstArm;

        explicit SwitchExpressionNode(NodeIndex<ExpressionNode> switchValue, NodeIndex<SwitchExpressionArmNode> firstArm)
            : ExpressionNode(NodeType::SwitchExpression)
            , switchValue(switchValue)
            , firstArm(firstArm) {}

    };

    struct AsExpressionNode : ExpressionNode {

        NodeIndex<TypePathNode> type;
        bool panicOnFailure;

        explicit AsExpressionNode(bool panicOnFailure, NodeIndex<TypePathNode> type)
            : ExpressionNode(NodeType::AsExpression)
            , panicOnFailure(panicOnFailure)
            , type(type) {}

    };

    struct IsExpressionNode : ExpressionNode {

        NodeIndex<TypePathNode> type;
        FixedCharSpan name;

        explicit IsExpressionNode(NodeIndex<TypePathNode> type, FixedCharSpan name)
            : ExpressionNode(NodeType::IsExpression)
            , type(type)
            , name(name) {}


    };

    struct ExpressionStatementNode : StatementNode {
        NodeIndex<ExpressionNode> expression;

        explicit ExpressionStatementNode(NodeIndex<ExpressionNode> expression)
            : StatementNode(NodeType::ExpressionStatement)
            , expression(expression) {}

    };

    struct ThrowStatementNode : StatementNode {

        NodeIndex<ExpressionNode> expression;

        explicit ThrowStatementNode(NodeIndex<ExpressionNode> expression) : StatementNode(NodeType::ThrowStatement), expression(expression) {}

    };

    struct ThrowExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> expression;

        explicit ThrowExpressionNode(NodeIndex<ExpressionNode> expression)
            : ExpressionNode(NodeType::ThrowExpression)
            , expression(expression) {}

    };

    struct TernaryExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> conditionNodeIndex;
        NodeIndex<ExpressionNode> trueNodeIndex;
        NodeIndex<ExpressionNode> falseNodeIndex;

        explicit TernaryExpressionNode(
            NodeIndex<ExpressionNode> conditionNodeIndex,
            NodeIndex<ExpressionNode> trueNodeIndex,
            NodeIndex<ExpressionNode> falseNodeIndex
        )
            : ExpressionNode(NodeType::TernaryExpression)
            , conditionNodeIndex(conditionNodeIndex)
            , trueNodeIndex(trueNodeIndex)
            , falseNodeIndex(falseNodeIndex) {}

    };

    struct NullCoalescingExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> lhs;
        NodeIndex<ExpressionNode> rhs;

        explicit NullCoalescingExpressionNode(NodeIndex<ExpressionNode> lhs, NodeIndex<ExpressionNode> rhs)
            : ExpressionNode(NodeType::NullCoalescingExpression)
            , lhs(lhs)
            , rhs(rhs) {}

    };


    struct TypeArgumentListNode : NodeBase {

        NodeIndex<TypePathNode> first;

        explicit TypeArgumentListNode(NodeIndex<TypePathNode> first)
            : NodeBase(NodeType::TypeArgumentList)
            , first(first) {}

    };

    struct NamespaceOrTypeNameNode : NodeBase {

        char* name;
        uint16 nameLength;
        BuiltInTypeName builtInTypeName;
        NodeIndex<TypeArgumentListNode> typeArguments;
        NodeIndex<NamespaceOrTypeNameNode> next;

        explicit NamespaceOrTypeNameNode(FixedCharSpan name, BuiltInTypeName builtInTypeName, NodeIndex<TypeArgumentListNode> typeArguments, NodeIndex<NamespaceOrTypeNameNode> next)
            : NodeBase(NodeType::NamespaceOrTypeName)
            , name(name.ptr)
            , nameLength(name.size)
            , builtInTypeName(builtInTypeName)
            , typeArguments(typeArguments)
            , next(next) {}

    };

    struct TypeConstraintListNode : NodeBase {

        explicit TypeConstraintListNode() : NodeBase(NodeType::TypeConstraintList) {}

    };

    struct RefExpressionNode : ExpressionNode {
        NodeIndex<ExpressionNode> expression;

        explicit RefExpressionNode(NodeIndex<ExpressionNode> expression)
            : ExpressionNode(NodeType::RefExpression)
            , expression(expression) {}

    };

    struct LambdaParameterNode : NodeBase {

        NodeIndex<LambdaParameterNode> next;

        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        StorageClass storageClass;
        ArgumentPassByModifier passByModifier;

        explicit LambdaParameterNode(
            StorageClass storageClass,
            ArgumentPassByModifier passByModifier,
            NodeIndex<TypePathNode> typePath,
            NodeIndex<IdentifierNode> identifier
        )
            : NodeBase(NodeType::LambdaParameter)
            , storageClass(storageClass)
            , passByModifier(passByModifier)
            , typePath(typePath)
            , identifier(identifier) {}


    };

    struct LambdaExpressionNode : ExpressionNode {

        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<LambdaParameterNode> firstParameter;

        LambdaExpressionNode(NodeIndex<LambdaParameterNode> firstParameter, NodeIndex<BlockNode> block, NodeIndex<ExpressionNode> expression)
            : ExpressionNode(NodeType::LambdaExpression)
            , firstParameter(firstParameter)
            , block(block)
            , expression(expression) {}

    };

    struct IdentifierNode : ExpressionNode {
        char* name;
        int32 nameLength;
        NodeIndex<TypeArgumentListNode> typeArguments;
        NodeIndex<IdentifierNode> next;

        explicit IdentifierNode(FixedCharSpan identifier, NodeIndex<TypeArgumentListNode> typeArguments)
            : ExpressionNode(NodeType::Identifier)
            , name(identifier.ptr)
            , nameLength(identifier.size)
            , typeArguments(typeArguments)
            , next(0) {}

        FixedCharSpan GetIdentifier() const {
            return FixedCharSpan(name, nameLength);
        }

    };

    struct TypeParameterListNode : NodeBase {

        NodeIndex<IdentifierNode> first;

        explicit TypeParameterListNode(NodeIndex<IdentifierNode> first)
            : NodeBase(NodeType::TypeParameterListNode)
            , first(first) {}

    };

    struct PropertySetterNode : NodeBase {

        DefinitionVisibility visibility;
        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<IdentifierNode> valueIdentifier;

        explicit PropertySetterNode(DefinitionVisibility visibility)
            : NodeBase(NodeType::PropertySetter)
            , visibility(visibility)
            , valueIdentifier(NodeIndex<IdentifierNode>(0))
            , expression(NodeIndex<ExpressionNode>(0))
            , block(NodeIndex<BlockNode>(0)) {}

        PropertySetterNode(DefinitionVisibility visibility, NodeIndex<IdentifierNode> valueIdentifier, NodeIndex<ExpressionNode> expression)
            : NodeBase(NodeType::PropertySetter)
            , visibility(visibility)
            , valueIdentifier(valueIdentifier)
            , expression(expression)
            , block(NodeIndex<BlockNode>(0)) {}

        PropertySetterNode(DefinitionVisibility visibility, NodeIndex<IdentifierNode> valueIdentifier, NodeIndex<BlockNode> block)
            : NodeBase(NodeType::PropertySetter)
            , visibility(visibility)
            , valueIdentifier(valueIdentifier)
            , expression(NodeIndex<ExpressionNode>(0))
            , block(block) {}

    };

    struct PropertyGetterNode : NodeBase {

        DefinitionVisibility visibility;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<BlockNode> block;

        explicit PropertyGetterNode(DefinitionVisibility visibility)
            : NodeBase(NodeType::PropertyGetter)
            , visibility(visibility)
            , expression(NodeIndex<ExpressionNode>(0))
            , block(NodeIndex<BlockNode>(0)) {}

        explicit PropertyGetterNode(DefinitionVisibility visibility, NodeIndex<BlockNode> block)
            : NodeBase(NodeType::PropertyGetter)
            , expression(NodeIndex<ExpressionNode>(0))
            , block(block) {}

        explicit PropertyGetterNode(DefinitionVisibility visibility, NodeIndex<ExpressionNode> expression)
            : NodeBase(NodeType::PropertyGetter)
            , expression(expression)
            , block(NodeIndex<BlockNode>(0)) {}

    };

    struct PropertyDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<PropertyGetterNode> getterNode;
        NodeIndex<PropertySetterNode> setterNode;

        PropertyDeclarationNode(
            NodeIndex<ModifierListNode> modifiers,
            NodeIndex<TypePathNode> typePathNode,
            NodeIndex<IdentifierNode> identifier,
            NodeIndex<PropertyGetterNode> getterNode,
            NodeIndex<PropertySetterNode> setterNode
        )
            : DeclarationNode(NodeType::PropertyDeclaration)
            , modifiers(modifiers)
            , typePath(typePathNode)
            , identifier(identifier)
            , getterNode(getterNode)
            , setterNode(setterNode) {}

    };

    struct EnumMemberDeclarationNode : DeclarationNode {

        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<EnumMemberDeclarationNode> next;

        EnumMemberDeclarationNode(NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> expression)
            : DeclarationNode(NodeType::EnumMemberDeclaration)
            , identifier(identifier)
            , expression(expression)
            , next(0) {}

    };

    struct EnumMemberListNode : NodeBase {

        NodeIndex<EnumMemberDeclarationNode> first;

        explicit EnumMemberListNode(NodeIndex<EnumMemberDeclarationNode> first)
            : NodeBase(NodeType::EnumMemberList)
            , first(first) {}

    };

    struct EnumDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypePathNode> baseType;
        NodeIndex<EnumMemberListNode> members;

        EnumDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<TypePathNode> baseType, NodeIndex<EnumMemberListNode> members)
            : DeclarationNode(NodeType::EnumDeclaration)
            , modifiers(modifiers)
            , identifier(identifier)
            , baseType(baseType)
            , members(members) {}

    };

    struct MethodDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> returnType;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypeParameterListNode> typeParameters;
        NodeIndex<FormalParameterListNode> parameters;
        // NodeIndex<ParameterTypeConstraintNode> typeConstraints;
        NodeIndex<BlockNode> body;
        NodeIndex<ExpressionNode> arrowBody;

        explicit MethodDeclarationNode(
            NodeIndex<ModifierListNode> modifiers,
            NodeIndex<TypePathNode> typePathNode,
            NodeIndex<IdentifierNode> identifier,
            NodeIndex<TypeParameterListNode> typeParameters,
            NodeIndex<FormalParameterListNode> parameters,
            NodeIndex<BlockNode> body,
            NodeIndex<ExpressionNode> arrowBody
        )
            : DeclarationNode(NodeType::MethodDeclaration)
            , modifiers(modifiers)
            , returnType(typePathNode)
            , identifier(identifier)
            , typeParameters(typeParameters)
            , parameters(parameters)
            , body(body)
            , arrowBody(arrowBody) {}


    };

    struct TypeConstraintNode : NodeBase {

        NodeIndex<TypeConstraintNode> next;

        explicit TypeConstraintNode() : NodeBase(NodeType::TypeConstraint) {}

    };

    struct BinaryExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> lhs;
        NodeIndex<ExpressionNode> rhs;
        BinaryExpressionType type;

        explicit BinaryExpressionNode(BinaryExpressionType type, NodeIndex<ExpressionNode> lhs, NodeIndex<ExpressionNode> rhs)
            : ExpressionNode(NodeType::BinaryExpression)
            , lhs(lhs)
            , rhs(rhs)
            , type(type) {}

    };

    struct UnaryExpressionNode : ExpressionNode {

        UnaryExpressionType type;
        NodeIndex<ExpressionNode> expression;

        explicit UnaryExpressionNode(UnaryExpressionType type, NodeIndex<ExpressionNode> expression)
            : ExpressionNode(NodeType::UnaryExpression)
            , type(type)
            , expression(expression) {}

    };

    struct TypeCastExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<TypePathNode> type;

        explicit TypeCastExpressionNode(NodeIndex<TypePathNode> type, NodeIndex<ExpressionNode> expression)
            : ExpressionNode(NodeType::TypeCast)
            , type(type)
            , expression(expression) {}

    };

    struct BracketExpressionNode : NodeBase {

        NodeIndex<BracketExpressionNode> next;
        NodeIndex<ExpressionNode> expression;
        bool isNullable;

        explicit BracketExpressionNode(bool isNullable, NodeIndex<ExpressionNode> expression)
            : NodeBase(NodeType::BracketExpression)
            , isNullable(isNullable)
            , expression(expression)
            , next(NodeIndex<BracketExpressionNode>(0)) {}

    };

    struct PrimaryExpressionTailNode : NodeBase {

        NodeIndex<PrimaryExpressionTailNode> next;
        NodeIndex<BracketExpressionNode> bracketListStart;
        UntypedNodeIndex part;

        explicit PrimaryExpressionTailNode(UntypedNodeIndex part, NodeIndex<BracketExpressionNode> bracketListStart)
            : NodeBase(NodeType::PrimaryExpressionTail)
            , part(part)
            , bracketListStart(bracketListStart)
            , next(0) // assigned later
        {}

    };

    struct NullableDereferenceNode : NodeBase {

        NullableDereferenceNode()
            : NodeBase(NodeType::NullableDereference) {}

    };

    struct IncrementDecrementNode : NodeBase {

        bool isIncrement;

        explicit IncrementDecrementNode(bool isIncrement) : NodeBase(NodeType::IncrementDecrement), isIncrement(isIncrement) {}

    };

    enum class InitializerNodeType : uint8 {

        Member, // assignment
        Indexer, // call indexer
        Collection, // call Add(n)
        Expression // call Add(1)

    };

    struct InitializerNode : NodeBase {

        NodeIndex<InitializerNode> next;

        explicit InitializerNode(NodeType nodeType)
            : NodeBase(nodeType)
            , next(NodeIndex<InitializerNode>(0)) {}

    };

    struct ArgumentListNode;

    struct IndexedInitializerNode : InitializerNode {
        NodeIndex<ArgumentListNode> argumentList;
        NodeIndex<ExpressionNode> value;

        explicit IndexedInitializerNode(NodeIndex<ArgumentListNode> argumentList, NodeIndex<ExpressionNode> value)
            : InitializerNode(NodeType::IndexedInitializer)
            , argumentList(argumentList)
            , value(value) {}

    };

    struct CollectionInitializerNode : InitializerNode {

        NodeIndex<ExpressionNode> first;

        explicit CollectionInitializerNode(NodeIndex<ExpressionNode> first)
            : InitializerNode(NodeType::CollectionInitializer)
            , first(first) {}

    };

    struct ListInitializerNode : InitializerNode {
        NodeIndex<ExpressionNode> value;

        explicit ListInitializerNode(NodeIndex<ExpressionNode> value) : InitializerNode(NodeType::ListInitializer) {}

    };

    struct MemberInitializerNode : InitializerNode {

        char* memberName;
        uint16 memberNameLength;
        NodeIndex<ExpressionNode> value;

        explicit MemberInitializerNode(FixedCharSpan memberName, NodeIndex<ExpressionNode> value)
            : InitializerNode(NodeType::MemberInitializer)
            , memberName(memberName.ptr)
            , memberNameLength(memberName.size)
            , value(value) {}

        FixedCharSpan GetMemberName() const {
            return FixedCharSpan(memberName, memberNameLength);
        }

    };

    struct ArgumentNode : NodeBase {

        char* variableName;
        bool isVarType;
        ArgumentPassByModifier passBy;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<ArgumentNode> next;
        uint16 variableNameLength;

        explicit ArgumentNode(ArgumentPassByModifier passBy, bool isVarType, NodeIndex<TypePathNode> typePath, NodeIndex<ExpressionNode> expression)
            : NodeBase(NodeType::Argument)
            , passBy(passBy)
            , isVarType(isVarType)
            , typePath(typePath)
            , expression(expression)
            , variableName(nullptr)
            , variableNameLength(0)
            , next(NodeIndex<ArgumentNode>(0)) // set explicitly later
        {}

        explicit ArgumentNode(ArgumentPassByModifier passBy, bool isVarType, NodeIndex<TypePathNode> typePath, FixedCharSpan variableName)
            : NodeBase(NodeType::Argument)
            , passBy(passBy)
            , isVarType(isVarType)
            , typePath(typePath)
            , expression(NodeIndex<ExpressionNode>(0))
            , variableName(variableName.ptr)
            , variableNameLength(variableName.size)
            , next(NodeIndex<ArgumentNode>(0)) // set explicitly later
        {}

        FixedCharSpan GetVariableName() {
            return FixedCharSpan(variableName, variableNameLength);
        }

    };


    struct ArgumentListNode : NodeBase {

        NodeIndex<ArgumentNode> first;

        explicit ArgumentListNode(NodeIndex<ArgumentNode> first)
            : NodeBase(NodeType::ArgumentList)
            , first(first) {}

    };

    struct TrailingLambdaStatementNode : StatementNode {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<ContextListNode> contextList;
        NodeIndex<BlockNode> block;

        explicit TrailingLambdaStatementNode(NodeIndex<ExpressionNode> expression, NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> block)
            : StatementNode(NodeType::TrailingLambdaStatement)
            , expression(expression)
            , contextList(contextList)
            , block(block) {}

    };

    struct MethodInvocationNode : NodeBase {

        NodeIndex<ArgumentListNode> arguments;

        explicit MethodInvocationNode(NodeIndex<ArgumentListNode> arguments)
            : NodeBase(NodeType::MethodInvocation)
            , arguments(arguments) {}

    };

    struct PrimaryMethodInvocationNode : NodeBase {

        NodeIndex<ArgumentListNode> arguments;
        NodeIndex<IdentifierNode> identifier;

        explicit PrimaryMethodInvocationNode(NodeIndex<IdentifierNode> identifier, NodeIndex<ArgumentListNode> arguments)
            : NodeBase(NodeType::PrimaryMethodInvocation)
            , identifier(identifier)
            , arguments(arguments) {}

    };

    struct PrimaryExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> start;
        NodeIndex<BracketExpressionNode> startBracketExpression;
        NodeIndex<PrimaryExpressionTailNode> tail;

        explicit PrimaryExpressionNode(NodeIndex<ExpressionNode> start, NodeIndex<BracketExpressionNode> startBracketExpression, NodeIndex<PrimaryExpressionTailNode> tail)
            : ExpressionNode(NodeType::PrimaryExpression)
            , start(start)
            , startBracketExpression(startBracketExpression)
            , tail(tail) {}

    };

    struct ScopeMemberAccessNode : NodeBase {

        NodeIndex<IdentifierNode> identifier;

        explicit ScopeMemberAccessNode(NodeIndex<IdentifierNode> identifier)
            : NodeBase(NodeType::ScopeMemberAccess)
            , identifier(identifier) {}


    };

    struct ConditionalMemberAccessNode : NodeBase {

        NodeIndex<IdentifierNode> identifier;

        explicit ConditionalMemberAccessNode(NodeIndex<IdentifierNode> identifier)
            : NodeBase(NodeType::ConditionalMemberAccess)
            , identifier(identifier) {}

    };

    struct MemberAccessNode : NodeBase {

        NodeIndex<IdentifierNode> identifier;

        explicit MemberAccessNode(NodeIndex<IdentifierNode> identifier)
            : NodeBase(NodeType::MemberAccess)
            , identifier(identifier) {}

    };

    struct ThisReferenceExpressionNode : ExpressionNode {

        explicit ThisReferenceExpressionNode() : ExpressionNode(NodeType::ThisReferenceExpression) {}

    };

    struct BaseReferenceExpressionNode : ExpressionNode {

        explicit BaseReferenceExpressionNode() : ExpressionNode(NodeType::BaseReferenceExpression) {}

    };

    struct ParenExpressionNode : ExpressionNode {
        NodeIndex<ExpressionNode> expression;

        explicit ParenExpressionNode(NodeIndex<ExpressionNode> expression)
            : ExpressionNode(NodeType::ParenExpression)
            , expression(expression) {}

    };

    struct BuiltInTypeAccessExpressionNode : ExpressionNode {

        BuiltInTypeName typeName;

        explicit BuiltInTypeAccessExpressionNode(BuiltInTypeName typeName)
            : ExpressionNode(NodeType::BuiltInTypeAccess)
            , typeName(typeName) {}

    };


    struct TypeOfExpressionNode : ExpressionNode {

        NodeIndex<TypePathNode> type;

        explicit TypeOfExpressionNode(NodeIndex<TypePathNode> type)
            : ExpressionNode(NodeType::TypeOfExpression)
            , type(type) {}

    };


    struct StringPartNode : NodeBase {
        NodeIndex<StringPartNode> next;
        char* source;
        uint16 sourceLength;
        NodeIndex<ExpressionNode> expression;
        StringPartType partType;

        explicit StringPartNode(StringPartType partType, NodeIndex<ExpressionNode> expression)
            : NodeBase(NodeType::StringPart)
            , partType(partType)
            , expression(expression)
            , source(nullptr)
            , sourceLength(0)
            , next(NodeIndex<StringPartNode>(0)) {}

        explicit StringPartNode(StringPartType partType, FixedCharSpan source)
            : NodeBase(NodeType::StringPart)
            , partType(partType)
            , source(source.ptr)
            , sourceLength(source.size)
            , expression(NodeIndex<ExpressionNode>(0))
            , next(NodeIndex<StringPartNode>(0)) {}

        FixedCharSpan GetSource() const {
            return FixedCharSpan(source, sourceLength);
        }

    };


    struct DynamicArrayLiteralNode : ExpressionNode {

        NodeIndex<ExpressionNode> firstExpression;

        explicit DynamicArrayLiteralNode(NodeIndex<ExpressionNode> firstExpression)
            : ExpressionNode(NodeType::DynamicArrayLiteral)
            , firstExpression(firstExpression) {}

    };

    struct DynamicKeyNode : NodeBase {

        FixedCharSpan identifier;
        NodeIndex<ExpressionNode> expression;

        explicit DynamicKeyNode(FixedCharSpan identifier)
            : NodeBase(NodeType::DynamicKey)
            , identifier(identifier)
            , expression(NodeIndex<ExpressionNode>(0)) {}


        explicit DynamicKeyNode(NodeIndex<ExpressionNode> expression)
            : NodeBase(NodeType::DynamicKey)
            , identifier(FixedCharSpan())
            , expression(expression) {}

    };

    struct DynamicValueNode : NodeBase {

        explicit DynamicValueNode(NodeType nodeType)
            : NodeBase(nodeType) {}

    };

    struct DynamicKeyValueNode : NodeBase {

        NodeIndex<DynamicKeyValueNode> next;
        NodeIndex<DynamicKeyNode> key;
        NodeIndex<ExpressionNode> value;

        explicit DynamicKeyValueNode(NodeIndex<DynamicKeyNode> key, NodeIndex<ExpressionNode> value)
            : NodeBase(NodeType::DynamicKeyValue)
            , key(key)
            , value(value) {}

    };

    struct DynamicObjectLiteralNode : ExpressionNode {

        NodeIndex<DynamicKeyValueNode> first;

        explicit DynamicObjectLiteralNode(NodeIndex<DynamicKeyValueNode> first)
            : ExpressionNode(NodeType::DynamicObjectLiteral)
            , first(first) {
        }

    };

    struct NewDynamicObjectLiteralNode : ExpressionNode {

        AllocatorType allocatorType;
        NodeIndex<DynamicObjectLiteralNode> objectLiteral;

        explicit NewDynamicObjectLiteralNode(AllocatorType allocatorType, NodeIndex<DynamicObjectLiteralNode> objectLiteral)
            : ExpressionNode(NodeType::NewDynamicObjectLiteralExpression)
            , allocatorType(allocatorType)
            , objectLiteral(objectLiteral) {}

    };

    struct NewDynamicArrayLiteralNode : ExpressionNode {

        AllocatorType allocatorType;
        NodeIndex<DynamicArrayLiteralNode> arrayLiteral;

        explicit NewDynamicArrayLiteralNode(AllocatorType allocatorType, NodeIndex<DynamicArrayLiteralNode> arrayLiteral)
            : ExpressionNode(NodeType::NewDynamicArrayLiteralExpression)
            , allocatorType(allocatorType)
            , arrayLiteral(arrayLiteral) {}

    };

    struct InitializerListNode : NodeBase {

        NodeIndex<InitializerNode> first;

        explicit InitializerListNode(NodeIndex<InitializerNode> first)
            : NodeBase(NodeType::InitializerList)
            , first(first) {}

    };

    struct NewExpressionNode : ExpressionNode {

        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> constructorName;
        NodeIndex<ArgumentListNode> argumentList;
        NodeIndex<InitializerListNode> initializerList;

        explicit NewExpressionNode(NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> constructorName, NodeIndex<ArgumentListNode> argumentList, NodeIndex<InitializerListNode> initializerList)
            : ExpressionNode(NodeType::NewExpression)
            , constructorName(constructorName)
            , typePath(typePath)
            , argumentList(argumentList)
            , initializerList(initializerList) {}

    };

    struct LiteralExpressionNode : ExpressionNode {

        LiteralType literalType;
        LiteralValue literalValue;
        NodeIndex<TypePathNode> defaultType;

        explicit LiteralExpressionNode(LiteralType literalType, LiteralValue literalValue)
            : ExpressionNode(NodeType::Literal)
            , literalType(literalType)
            , literalValue(literalValue)
            , defaultType(0) {}

        explicit LiteralExpressionNode(LiteralType literalType, NodeIndex<TypePathNode> defaultType)
            : ExpressionNode(NodeType::Literal)
            , literalType(literalType)
            , literalValue()
            , defaultType(defaultType) {}

        explicit LiteralExpressionNode(LiteralType literalType)
            : ExpressionNode(NodeType::Literal)
            , literalType(literalType)
            , literalValue()
            , defaultType(0) {}

    };

    struct NamespacePathNode : NodeBase {

        char* namespaceName;
        int32 namespaceNameLength;

        explicit NamespacePathNode(FixedCharSpan namespacePath)
            : NodeBase(NodeType::NamespacePath)
            , namespaceName(namespacePath.ptr)
            , namespaceNameLength(namespacePath.size) {}

        FixedCharSpan GetNamespacePath() const {
            return FixedCharSpan(namespaceName, namespaceNameLength);
        }

    };

    struct UsingNamespaceNode : DeclarationNode {

        NodeIndex<NamespacePathNode> namespacePath;

        explicit UsingNamespaceNode(NodeIndex<NamespacePathNode> namespacePath)
            : DeclarationNode(NodeType::UsingNamespaceDeclaration)
            , namespacePath(namespacePath) {}


    };

    struct UsingStaticNode : DeclarationNode {

        NodeIndex<TypePathNode> type;

        explicit UsingStaticNode(NodeIndex<TypePathNode> type)
            : DeclarationNode(NodeType::UsingStaticDeclaration)
            , type(type) {}

    };

    struct UsingAliasNode : DeclarationNode {

        char* alias;
        int32 length;
        NodeIndex<TypePathNode> type;

        UsingAliasNode(FixedCharSpan alias, NodeIndex<TypePathNode> type)
            : DeclarationNode(NodeType::UsingAliasDeclaration)
            , alias(alias.ptr)
            , length(alias.size)
            , type(type) {}

        FixedCharSpan GetAliasName() const {
            return FixedCharSpan(alias, length);
        }

    };

    struct ClassBodyNode : NodeBase {

        NodeIndex<DeclarationNode> first;

        explicit ClassBodyNode(NodeIndex<DeclarationNode> first)
            : NodeBase(NodeType::ClassBody)
            , first(first) {}

    };

    struct StructDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypeListNode> baseType;
        NodeIndex<TypeParameterListNode> typeParameters;
        NodeIndex<ClassBodyNode> classBody;

        StructDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<TypeListNode> baseType, NodeIndex<ClassBodyNode> classBody)
            : DeclarationNode(NodeType::StructDeclaration)
            , modifiers(modifiers)
            , identifier(identifier)
            , typeParameters(typeParameters)
            , baseType(baseType)
            , classBody(classBody) {}

    };

    struct InterfaceDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypeListNode> baseType;
        NodeIndex<TypeParameterListNode> typeParameters;
        NodeIndex<ClassBodyNode> classBody;

        explicit InterfaceDeclarationNode()
            : DeclarationNode(NodeType::InterfaceDeclaration) {}

    };

    struct NamespaceDeclarationNode : DeclarationNode {

        char* name;
        int32 nameLength;

        NodeIndex<DeclarationNode> firstDeclaration;

        explicit NamespaceDeclarationNode(FixedCharSpan name, NodeIndex<DeclarationNode> firstDeclaration)
            : DeclarationNode(NodeType::NamespaceDeclaration)
            , name(name.ptr)
            , nameLength(name.size)
            , firstDeclaration(firstDeclaration) {}

        FixedCharSpan GetName() const {
            return FixedCharSpan(name, nameLength);
        }

    };

    struct ClassDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypeListNode> baseType;
        NodeIndex<TypeParameterListNode> typeParameters;
        NodeIndex<ClassBodyNode> classBody;

        ClassDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<TypeListNode> baseType, NodeIndex<ClassBodyNode> classBody)
            : DeclarationNode(NodeType::ClassDeclaration)
            , modifiers(modifiers)
            , identifier(identifier)
            , typeParameters(typeParameters)
            , baseType(baseType)
            , classBody(classBody) {}

    };

    struct InvalidDeclarationNode : DeclarationNode {

        NodeIndex<BlockNode> block;

        explicit InvalidDeclarationNode(NodeIndex<BlockNode> block)
            : DeclarationNode(NodeType::InvalidDeclaration)
            , block(block) {}

    };

    struct ErrorDeclarationNode : DeclarationNode {

        ErrorDeclarationNode() : DeclarationNode(NodeType::Error) {}

    };

    struct IndexerParameterListNode : NodeBase {

        NodeIndex<FormalParameterNode> first;

        explicit IndexerParameterListNode(NodeIndex<FormalParameterNode> first)
            : NodeBase(NodeType::IndexerParameterList)
            , first(first) {}

    };

    struct IndexerDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<IndexerParameterListNode> parameterList;
        NodeIndex<PropertyGetterNode> getter;
        NodeIndex<PropertySetterNode> setter;

        IndexerDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IndexerParameterListNode> parameterList, NodeIndex<PropertyGetterNode> getter, NodeIndex<PropertySetterNode> setter)
            : DeclarationNode(NodeType::IndexerDeclaration)
            , modifiers(modifiers)
            , typePath(typePath)
            , parameterList(parameterList)
            , setter(setter)
            , getter(getter) {}

    };

    struct DelegateDeclarationNode : DeclarationNode {

        bool isVoid;
        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypeParameterListNode> typeParameters;
        NodeIndex<FormalParameterListNode> parameters;

        DelegateDeclarationNode(NodeIndex<ModifierListNode> modifiers, bool isVoid, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<FormalParameterListNode> parameters)
            : DeclarationNode(NodeType::DelegateDeclaration)
            , modifiers(modifiers)
            , isVoid(isVoid)
            , typePath(typePath)
            , identifier(identifier)
            , typeParameters(typeParameters)
            , parameters(parameters) {}

    };

    struct ConstructorDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<FormalParameterListNode> parameters;
        NodeIndex<BlockNode> body;

        ConstructorDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<FormalParameterListNode> parameters, NodeIndex<BlockNode> body)
            : DeclarationNode(NodeType::ConstructorDeclaration)
            , modifiers(modifiers)
            , identifier(identifier)
            , parameters(parameters)
            , body(body) {}

    };

    struct FieldDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> initExpression;

        FieldDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> initExpression)
            : DeclarationNode(NodeType::FieldDeclaration)
            , modifiers(modifiers)
            , typePath(typePath)
            , identifier(identifier)
            , initExpression(initExpression) {}

    };

}