#pragma once

#include <cstring>
#include "./NodeType.h"
#include "./NodeIndex.h"
#include "./ParseTypes.h"
#include "../Util/FixedCharSpan.h"
#include "./LiteralType.h"

namespace Alchemy::Parsing {


    struct NodeBase {
        NodeType nodeType;
        uint8 padding {};
        uint16 nodeIndex {};
        uint16 tokenStart {};
        uint16 tokenEnd {};

        explicit NodeBase(NodeType nodeType);

        TokenRange GetTokenRange() const;

    };

    struct AbstractPsiNode {

        NodeBase nodeBase;

        char bytes[24] { };

        TokenRange GetTokenRange() {
            return nodeBase.GetTokenRange();
        }

    };

    struct DeclarationNode : NodeBase {

        NodeIndex<DeclarationNode> next;

        explicit DeclarationNode(NodeType nodeType);

    };

    struct ExpressionNode : NodeBase {
        NodeIndex<ExpressionNode> next;

        explicit ExpressionNode(NodeType nodeType);

    };

    struct FileNode : NodeBase {

        NodeIndex<DeclarationNode> firstDeclaration;

        FileNode();

    };

    struct ArrayRankNode : NodeBase {

        bool isNullable;

        explicit ArrayRankNode(bool isNullable);

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

        explicit TypePathNode(NodeIndex<IdentifierNode> namePath, BuiltInTypeName builtInTypeName, NodeIndex<TypeArgumentListNode> typeArguments, NodeIndex<ArrayRankNode> arrayRank, bool isNullable);

    };

    struct TypeListNode : NodeBase {

        NodeIndex<TypePathNode> first;

        explicit TypeListNode(NodeIndex<TypePathNode> first);

    };

    struct ErrorNode : NodeBase {

        char* message;
        int32 messageLength;

        explicit ErrorNode(char* message, int32 messageLength);

    };

    struct AssignmentExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> rhs;
        AssignmentOperator op;
        NodeIndex<ExpressionNode> lhs;

        explicit AssignmentExpressionNode(NodeIndex<ExpressionNode> rhs, AssignmentOperator op, NodeIndex<ExpressionNode> lhs);
    };

    struct FormalParameterNode : NodeBase {

        StorageClass storage;
        ArgumentPassByModifier passByModifier;
        NodeIndex<TypePathNode> type;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> defaultValue;
        NodeIndex<FormalParameterNode> next;

        explicit FormalParameterNode(StorageClass storage, ArgumentPassByModifier passByModifier, NodeIndex<TypePathNode> type, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> defaultValue);

    };

    struct FormalParameterListNode : NodeBase {

        NodeIndex<FormalParameterNode> listStart;

        explicit FormalParameterListNode(NodeIndex<FormalParameterNode> listStart);

    };

    struct StatementNode : NodeBase {

        NodeIndex<StatementNode> next;

        explicit StatementNode(NodeType nodeType);

    };

    struct ReturnStatementNode : StatementNode {

        NodeIndex<ExpressionNode> expression;

        explicit ReturnStatementNode(NodeIndex<ExpressionNode> expression);

    };

    struct ContinueStatementNode : StatementNode {

        explicit ContinueStatementNode();

    };

    struct BreakStatementNode : StatementNode {

        explicit BreakStatementNode();

    };

    struct EmptyStatementNode : StatementNode {
        explicit EmptyStatementNode();
    };

    struct BlockNode : StatementNode {

        NodeIndex<StatementNode> firstStatement;

        explicit BlockNode(NodeIndex<StatementNode> firstStatement);

    };

    struct IdentifierNode;

    struct ExpressionListNode : NodeBase {

        NodeIndex<ExpressionNode> first;

        explicit ExpressionListNode(NodeIndex<ExpressionNode> first);

    };

    struct ContextListNode : NodeBase {

        NodeIndex<IdentifierNode> first;

        explicit ContextListNode(NodeIndex<IdentifierNode> first);

    };

    struct ForLoopIteratorNode : NodeBase {

        NodeIndex<ExpressionNode> first;

        explicit ForLoopIteratorNode(NodeIndex<ExpressionNode> first);

    };

    struct CatchClauseNode : NodeBase {
        NodeIndex<BlockNode> block;
        NodeIndex<ContextListNode> contextList;

        CatchClauseNode(NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> blockNode);

    };

    struct TryBlockNode : StatementNode {

        NodeIndex<BlockNode> block;
        NodeIndex<CatchClauseNode> catchClause;
        NodeIndex<BlockNode> finallyBlock;

        explicit TryBlockNode(NodeIndex<BlockNode> block, NodeIndex<CatchClauseNode> catchClause, NodeIndex<BlockNode> finallyBlock);

    };

    struct TryExpressionNode : StatementNode {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<CatchClauseNode> catchClause;

        explicit TryExpressionNode(NodeIndex<ExpressionNode> expression, NodeIndex<CatchClauseNode> catchClause);

    };

    struct SwitchSectionLabelNode : NodeBase {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<ExpressionNode> caseGuard;
        bool isDefault;
        NodeIndex<SwitchSectionLabelNode> next;

        explicit SwitchSectionLabelNode(NodeIndex<ExpressionNode> expression, NodeIndex<ExpressionNode> caseGuard);

        explicit SwitchSectionLabelNode(bool isDefault);

    };

    struct SwitchSectionNode : NodeBase {

        NodeIndex<SwitchSectionLabelNode> firstLabel;
        NodeIndex<StatementNode> firstStatement;

        explicit SwitchSectionNode(NodeIndex<SwitchSectionLabelNode> firstLabel, NodeIndex<StatementNode> firstStatement);

    };

    struct SwitchStatementNode : StatementNode {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<ContextListNode> contextList;
        NodeIndex<SwitchSectionNode> firstSection;

        explicit SwitchStatementNode(NodeIndex<ExpressionNode> expression, NodeIndex<ContextListNode> contextList, NodeIndex<SwitchSectionNode> firstSection);

    };

    struct ForEachLoopStatementNode : StatementNode {

        NodeIndex<ContextListNode> context;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<StatementNode> body;

        explicit ForEachLoopStatementNode(NodeIndex<ExpressionNode> expression, NodeIndex<ContextListNode> context, NodeIndex<StatementNode> body);

    };

    struct ForInitializerListNode;

    struct ForLoopStatementNode : StatementNode {
        NodeIndex<ForInitializerListNode> initializer;
        NodeIndex<ExpressionNode> condition;
        NodeIndex<ForLoopIteratorNode> iterator;
        NodeIndex<StatementNode> body;

        explicit ForLoopStatementNode(NodeIndex<ForInitializerListNode> initializer, NodeIndex<ExpressionNode> condition, NodeIndex<ForLoopIteratorNode> iterator, NodeIndex<StatementNode> body);

    };

    struct WhileStatementNode : StatementNode {
        NodeIndex<ExpressionListNode> conditions;
        NodeIndex<ContextListNode> context;
        NodeIndex<StatementNode> body;

        explicit WhileStatementNode(NodeIndex<ExpressionListNode> conditions, NodeIndex<ContextListNode> context, NodeIndex<StatementNode> body);

    };

    struct DoWhileStatementNode : StatementNode {
        NodeIndex<StatementNode> body;
        NodeIndex<ExpressionNode> condition;

        explicit DoWhileStatementNode(NodeIndex<StatementNode> body, NodeIndex<ExpressionNode> condition);

    };

    struct IfStatementNode : StatementNode {

        NodeIndex<ExpressionListNode> conditions;
        NodeIndex<ContextListNode> context;
        NodeIndex<StatementNode> body;
        NodeIndex<StatementNode> elseCase;

        IfStatementNode(NodeIndex<ExpressionListNode> conditions, NodeIndex<ContextListNode> context, NodeIndex<StatementNode> body, NodeIndex<StatementNode> elseCase);

    };

    struct UsingStatementBlockNode : StatementNode {

        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionListNode> expressions;
        NodeIndex<ContextListNode> contextList;

        explicit UsingStatementBlockNode(NodeIndex<ExpressionListNode> expressions, NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> block);

    };

    struct UsingStatementScopeNode : StatementNode {

        NodeIndex<ExpressionNode> expression;

        explicit UsingStatementScopeNode(NodeIndex<ExpressionNode> expression);

    };

    struct UsingStatementNode : StatementNode {

        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionListNode> expressions;
        NodeIndex<ContextListNode> contextList;

        explicit UsingStatementNode(NodeIndex<ExpressionListNode> expressions, NodeIndex<BlockNode> block, NodeIndex<ContextListNode> contextList);

    };

    struct WithStatementNode : StatementNode {

        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionListNode> expressionList;
        NodeIndex<ContextListNode> contextList;

        explicit WithStatementNode(NodeIndex<ExpressionListNode> expressionList, NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> block);

    };

    struct LocalVariableInitializerNode : NodeBase {

        bool isByRef;
        NodeIndex<ExpressionNode> expression;

        explicit LocalVariableInitializerNode(bool isByRef, NodeIndex<ExpressionNode> expression);

    };

    struct LocalVariableTypeNode : NodeBase {

        StorageClass storageClass;
        NodeIndex<TypePathNode> typePath;
        bool isByRef;
        bool isVarTyped;

        explicit LocalVariableTypeNode(StorageClass storageClass, bool isByRef, bool isVarTyped, NodeIndex<TypePathNode> typePath);

    };

    struct ForInitializerNode : NodeBase {
        NodeIndex<ForInitializerNode> next;
        NodeIndex<StatementNode> variableDeclaration;
        NodeIndex<ExpressionNode> expression;

        explicit ForInitializerNode(NodeIndex<StatementNode> variableDeclaration);

        explicit ForInitializerNode(NodeIndex<ExpressionNode> expression);

    };

    struct ForInitializerListNode : NodeBase {
        NodeIndex<ForInitializerNode> first;

        explicit ForInitializerListNode(NodeIndex<ForInitializerNode> first);

    };

    struct LocalVariableDeclarationNode : StatementNode {

        NodeIndex<LocalVariableTypeNode> variableType;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<LocalVariableInitializerNode> initializer;

        explicit LocalVariableDeclarationNode(NodeIndex<LocalVariableTypeNode> variableType, NodeIndex<IdentifierNode> identifier, NodeIndex<LocalVariableInitializerNode> initializer);

    };

    struct LocalConstantDeclarationNode : StatementNode {

        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> expression;

        explicit LocalConstantDeclarationNode(NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> expression);

    };

    struct ModifierListNode : NodeBase {

        DeclarationModifier modifiers;

        explicit ModifierListNode(DeclarationModifier modifiers);

    };

    struct ConstantDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> expression;

        explicit ConstantDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> expression);

    };

    struct SwitchExpressionArmNode : NodeBase {

        NodeIndex<ExpressionNode> condition;
        NodeIndex<ExpressionNode> caseGuard;
        NodeIndex<ExpressionNode> action;
        NodeIndex<SwitchExpressionArmNode> next;

        explicit SwitchExpressionArmNode(NodeIndex<ExpressionNode> condition, NodeIndex<ExpressionNode> caseGuard, NodeIndex<ExpressionNode> action);

    };

    struct SwitchExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> switchValue;
        NodeIndex<SwitchExpressionArmNode> firstArm;

        explicit SwitchExpressionNode(NodeIndex<ExpressionNode> switchValue, NodeIndex<SwitchExpressionArmNode> firstArm);

    };

    struct AsExpressionNode : ExpressionNode {

        NodeIndex<TypePathNode> type;
        bool panicOnFailure;

        explicit AsExpressionNode(bool panicOnFailure, NodeIndex<TypePathNode> type);

    };

    struct IsExpressionNode : ExpressionNode {

        NodeIndex<TypePathNode> type;
        FixedCharSpan name;

        explicit IsExpressionNode(NodeIndex<TypePathNode> type, FixedCharSpan name);


    };

    struct ExpressionStatementNode : StatementNode {
        NodeIndex<ExpressionNode> expression;

        explicit ExpressionStatementNode(NodeIndex<ExpressionNode> expression);

    };

    struct ThrowStatementNode : StatementNode {

        NodeIndex<ExpressionNode> expression;

        explicit ThrowStatementNode(NodeIndex<ExpressionNode> expression);

    };

    struct ThrowExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> expression;

        explicit ThrowExpressionNode(NodeIndex<ExpressionNode> expression);

    };

    struct TernaryExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> conditionNodeIndex;
        NodeIndex<ExpressionNode> trueNodeIndex;
        NodeIndex<ExpressionNode> falseNodeIndex;

        explicit TernaryExpressionNode(
            NodeIndex<ExpressionNode> conditionNodeIndex,
            NodeIndex<ExpressionNode> trueNodeIndex,
            NodeIndex<ExpressionNode> falseNodeIndex
        );

    };

    struct NullCoalescingExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> lhs;
        NodeIndex<ExpressionNode> rhs;

        explicit NullCoalescingExpressionNode(NodeIndex<ExpressionNode> lhs, NodeIndex<ExpressionNode> rhs);

    };


    struct TypeArgumentListNode : NodeBase {

        NodeIndex<TypePathNode> first;

        explicit TypeArgumentListNode(NodeIndex<TypePathNode> first);

    };

    struct NamespaceOrTypeNameNode : NodeBase {

        char* name;
        uint16 nameLength;
        BuiltInTypeName builtInTypeName;
        NodeIndex<TypeArgumentListNode> typeArguments;
        NodeIndex<NamespaceOrTypeNameNode> next;

        explicit NamespaceOrTypeNameNode(FixedCharSpan name, BuiltInTypeName builtInTypeName, NodeIndex<TypeArgumentListNode> typeArguments, NodeIndex<NamespaceOrTypeNameNode> next);

    };

    struct TypeConstraintListNode : NodeBase {

        explicit TypeConstraintListNode() : NodeBase(NodeType::TypeConstraintList) {}

    };

    struct RefExpressionNode : ExpressionNode {
        NodeIndex<ExpressionNode> expression;

        explicit RefExpressionNode(NodeIndex<ExpressionNode> expression);

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
        );


    };

    struct LambdaExpressionNode : ExpressionNode {

        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<LambdaParameterNode> firstParameter;

        LambdaExpressionNode(NodeIndex<LambdaParameterNode> firstParameter, NodeIndex<BlockNode> block, NodeIndex<ExpressionNode> expression);

    };

    struct IdentifierNode : ExpressionNode {
        char* name;
        int32 nameLength;
        NodeIndex<TypeArgumentListNode> typeArguments;
        NodeIndex<IdentifierNode> next;

        explicit IdentifierNode(FixedCharSpan identifier, NodeIndex<TypeArgumentListNode> typeArguments);

        inline FixedCharSpan GetIdentifier() const {
            return FixedCharSpan(name, nameLength);
        }

    };

    struct TypeParameterListNode : NodeBase {

        NodeIndex<IdentifierNode> first;

        explicit TypeParameterListNode(NodeIndex<IdentifierNode> first);

    };

    struct PropertySetterNode : NodeBase {

        DefinitionVisibility visibility;
        NodeIndex<BlockNode> block;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<IdentifierNode> valueIdentifier;

        explicit PropertySetterNode(DefinitionVisibility visibility);

        PropertySetterNode(DefinitionVisibility visibility, NodeIndex<IdentifierNode> valueIdentifier, NodeIndex<ExpressionNode> expression);

        PropertySetterNode(DefinitionVisibility visibility, NodeIndex<IdentifierNode> valueIdentifier, NodeIndex<BlockNode> block);

    };

    struct PropertyGetterNode : NodeBase {

        DefinitionVisibility visibility;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<BlockNode> block;

        explicit PropertyGetterNode(DefinitionVisibility visibility);

        explicit PropertyGetterNode(DefinitionVisibility visibility, NodeIndex<BlockNode> block);

        explicit PropertyGetterNode(DefinitionVisibility visibility, NodeIndex<ExpressionNode> expression);

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
        );

    };

    struct EnumMemberDeclarationNode : DeclarationNode {

        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> expression;
        NodeIndex<EnumMemberDeclarationNode> next;

        EnumMemberDeclarationNode(NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> expression);

    };

    struct EnumMemberListNode : NodeBase {

        NodeIndex<EnumMemberDeclarationNode> first;

        explicit EnumMemberListNode(NodeIndex<EnumMemberDeclarationNode> first);

    };

    struct EnumDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypePathNode> baseType;
        NodeIndex<EnumMemberListNode> members;

        EnumDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<TypePathNode> baseType, NodeIndex<EnumMemberListNode> members);

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
        );


    };

    struct TypeConstraintNode : NodeBase {

        NodeIndex<TypeConstraintNode> next;

        explicit TypeConstraintNode();

    };

    struct BinaryExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> lhs;
        NodeIndex<ExpressionNode> rhs;
        BinaryExpressionType type;

        explicit BinaryExpressionNode(BinaryExpressionType type, NodeIndex<ExpressionNode> lhs, NodeIndex<ExpressionNode> rhs);

    };

    struct UnaryExpressionNode : ExpressionNode {

        UnaryExpressionType type;
        NodeIndex<ExpressionNode> expression;

        explicit UnaryExpressionNode(UnaryExpressionType type, NodeIndex<ExpressionNode> expression);

    };

    struct TypeCastExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<TypePathNode> type;

        explicit TypeCastExpressionNode(NodeIndex<TypePathNode> type, NodeIndex<ExpressionNode> expression);

    };

    struct BracketExpressionNode : NodeBase {

        NodeIndex<BracketExpressionNode> next;
        NodeIndex<ExpressionNode> expression;
        bool isNullable;

        explicit BracketExpressionNode(bool isNullable, NodeIndex<ExpressionNode> expression);

    };

    struct PrimaryExpressionTailNode : NodeBase {

        NodeIndex<PrimaryExpressionTailNode> next;
        NodeIndex<BracketExpressionNode> bracketListStart;
        UntypedNodeIndex part;

        explicit PrimaryExpressionTailNode(UntypedNodeIndex part, NodeIndex<BracketExpressionNode> bracketListStart);

    };

    struct NullableDereferenceNode : NodeBase {

        NullableDereferenceNode();

    };

    struct IncrementDecrementNode : NodeBase {

        bool isIncrement;

        explicit IncrementDecrementNode(bool isIncrement);

    };

    enum class InitializerNodeType : uint8 {

        Member, // assignment
        Indexer, // call indexer
        Collection, // call Add(n)
        Expression // call Add(1)

    };

    struct InitializerNode : NodeBase {

        NodeIndex<InitializerNode> next;

        explicit InitializerNode(NodeType nodeType);

    };

    struct ArgumentListNode;

    struct IndexedInitializerNode : InitializerNode {
        NodeIndex<ArgumentListNode> argumentList;
        NodeIndex<ExpressionNode> value;

        explicit IndexedInitializerNode(NodeIndex<ArgumentListNode> argumentList, NodeIndex<ExpressionNode> value);

    };

    struct CollectionInitializerNode : InitializerNode {

        NodeIndex<ExpressionNode> first;

        explicit CollectionInitializerNode(NodeIndex<ExpressionNode> first);

    };

    struct ListInitializerNode : InitializerNode {
        NodeIndex<ExpressionNode> value;

        explicit ListInitializerNode(NodeIndex<ExpressionNode> value);

    };

    struct MemberInitializerNode : InitializerNode {

        char* memberName;
        uint16 memberNameLength;
        NodeIndex<ExpressionNode> value;

        explicit MemberInitializerNode(FixedCharSpan memberName, NodeIndex<ExpressionNode> value);

        inline FixedCharSpan GetMemberName() const {
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

        explicit ArgumentNode(ArgumentPassByModifier passBy, bool isVarType, NodeIndex<TypePathNode> typePath, NodeIndex<ExpressionNode> expression);

        explicit ArgumentNode(ArgumentPassByModifier passBy, bool isVarType, NodeIndex<TypePathNode> typePath, FixedCharSpan variableName);

        inline FixedCharSpan GetVariableName() {
            return FixedCharSpan(variableName, variableNameLength);
        }

    };


    struct ArgumentListNode : NodeBase {

        NodeIndex<ArgumentNode> first;

        explicit ArgumentListNode(NodeIndex<ArgumentNode> first);

    };

    struct TrailingLambdaStatementNode : StatementNode {

        NodeIndex<ExpressionNode> expression;
        NodeIndex<ContextListNode> contextList;
        NodeIndex<BlockNode> block;

        explicit TrailingLambdaStatementNode(NodeIndex<ExpressionNode> expression, NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> block);

    };

    struct MethodInvocationNode : NodeBase {

        NodeIndex<ArgumentListNode> arguments;

        explicit MethodInvocationNode(NodeIndex<ArgumentListNode> arguments);

    };

    struct PrimaryMethodInvocationNode : NodeBase {

        NodeIndex<ArgumentListNode> arguments;
        NodeIndex<IdentifierNode> identifier;

        explicit PrimaryMethodInvocationNode(NodeIndex<IdentifierNode> identifier, NodeIndex<ArgumentListNode> arguments);

    };

    struct PrimaryExpressionNode : ExpressionNode {

        NodeIndex<ExpressionNode> start;
        NodeIndex<BracketExpressionNode> startBracketExpression;
        NodeIndex<PrimaryExpressionTailNode> tail;

        explicit PrimaryExpressionNode(NodeIndex<ExpressionNode> start, NodeIndex<BracketExpressionNode> startBracketExpression, NodeIndex<PrimaryExpressionTailNode> tail);

    };

    struct ScopeMemberAccessNode : NodeBase {

        NodeIndex<IdentifierNode> identifier;

        explicit ScopeMemberAccessNode(NodeIndex<IdentifierNode> identifier);


    };

    struct ConditionalMemberAccessNode : NodeBase {

        NodeIndex<IdentifierNode> identifier;

        explicit ConditionalMemberAccessNode(NodeIndex<IdentifierNode> identifier);

    };

    struct MemberAccessNode : NodeBase {

        NodeIndex<IdentifierNode> identifier;

        explicit MemberAccessNode(NodeIndex<IdentifierNode> identifier);

    };

    struct ThisReferenceExpressionNode : ExpressionNode {

        explicit ThisReferenceExpressionNode();

    };

    struct BaseReferenceExpressionNode : ExpressionNode {

        explicit BaseReferenceExpressionNode();

    };

    struct ParenExpressionNode : ExpressionNode {
        NodeIndex<ExpressionNode> expression;

        explicit ParenExpressionNode(NodeIndex<ExpressionNode> expression);

    };

    struct BuiltInTypeAccessExpressionNode : ExpressionNode {

        BuiltInTypeName typeName;

        explicit BuiltInTypeAccessExpressionNode(BuiltInTypeName typeName);

    };


    struct TypeOfExpressionNode : ExpressionNode {

        NodeIndex<TypePathNode> type;

        explicit TypeOfExpressionNode(NodeIndex<TypePathNode> type);

    };


    struct StringPartNode : NodeBase {
        NodeIndex<StringPartNode> next;
        char* source;
        uint16 sourceLength;
        NodeIndex<ExpressionNode> expression;
        StringPartType partType;

        explicit StringPartNode(StringPartType partType, NodeIndex<ExpressionNode> expression);

        explicit StringPartNode(StringPartType partType, FixedCharSpan source);

        inline FixedCharSpan GetSource() const {
            return FixedCharSpan(source, sourceLength);
        }

    };


    struct DynamicArrayLiteralNode : ExpressionNode {

        NodeIndex<ExpressionNode> firstExpression;

        explicit DynamicArrayLiteralNode(NodeIndex<ExpressionNode> firstExpression);

    };

    struct DynamicKeyNode : NodeBase {

        FixedCharSpan identifier;
        NodeIndex<ExpressionNode> expression;

        explicit DynamicKeyNode(FixedCharSpan identifier);

        explicit DynamicKeyNode(NodeIndex<ExpressionNode> expression);

    };

    struct DynamicValueNode : NodeBase {

        explicit DynamicValueNode(NodeType nodeType);

    };

    struct DynamicKeyValueNode : NodeBase {

        NodeIndex<DynamicKeyValueNode> next;
        NodeIndex<DynamicKeyNode> key;
        NodeIndex<ExpressionNode> value;

        explicit DynamicKeyValueNode(NodeIndex<DynamicKeyNode> key, NodeIndex<ExpressionNode> value);

    };

    struct DynamicObjectLiteralNode : ExpressionNode {

        NodeIndex<DynamicKeyValueNode> first;

        explicit DynamicObjectLiteralNode(NodeIndex<DynamicKeyValueNode> first);

    };

    struct NewDynamicObjectLiteralNode : ExpressionNode {

        AllocatorType allocatorType;
        NodeIndex<DynamicObjectLiteralNode> objectLiteral;

        explicit NewDynamicObjectLiteralNode(AllocatorType allocatorType, NodeIndex<DynamicObjectLiteralNode> objectLiteral);

    };

    struct NewDynamicArrayLiteralNode : ExpressionNode {

        AllocatorType allocatorType;
        NodeIndex<DynamicArrayLiteralNode> arrayLiteral;

        explicit NewDynamicArrayLiteralNode(AllocatorType allocatorType, NodeIndex<DynamicArrayLiteralNode> arrayLiteral);

    };

    struct InitializerListNode : NodeBase {

        NodeIndex<InitializerNode> first;

        explicit InitializerListNode(NodeIndex<InitializerNode> first);

    };

    struct NewExpressionNode : ExpressionNode {

        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> constructorName;
        NodeIndex<ArgumentListNode> argumentList;
        NodeIndex<InitializerListNode> initializerList;

        explicit NewExpressionNode(NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> constructorName, NodeIndex<ArgumentListNode> argumentList, NodeIndex<InitializerListNode> initializerList);

    };

    struct LiteralExpressionNode : ExpressionNode {

        LiteralType literalType;
        LiteralValue literalValue;
        NodeIndex<TypePathNode> defaultType;

        explicit LiteralExpressionNode(LiteralType literalType, LiteralValue literalValue);

        explicit LiteralExpressionNode(LiteralType literalType, NodeIndex<TypePathNode> defaultType);

        explicit LiteralExpressionNode(LiteralType literalType);

    };

    struct NamespacePathNode : NodeBase {

        char* namespaceName;
        int32 namespaceNameLength;

        explicit NamespacePathNode(FixedCharSpan namespacePath);

        inline FixedCharSpan GetNamespacePath() const {
            return FixedCharSpan(namespaceName, namespaceNameLength);
        }

    };

    struct UsingNamespaceNode : DeclarationNode {

        NodeIndex<NamespacePathNode> namespacePath;

        explicit UsingNamespaceNode(NodeIndex<NamespacePathNode> namespacePath);


    };

    struct UsingStaticNode : DeclarationNode {

        NodeIndex<TypePathNode> type;

        explicit UsingStaticNode(NodeIndex<TypePathNode> type);

    };

    struct UsingAliasNode : DeclarationNode {

        char* alias;
        int32 length;
        NodeIndex<TypePathNode> type;

        UsingAliasNode(FixedCharSpan alias, NodeIndex<TypePathNode> type);

        inline FixedCharSpan GetAliasName() const {
            return FixedCharSpan(alias, length);
        }

    };

    struct ClassBodyNode : NodeBase {

        NodeIndex<DeclarationNode> first;

        explicit ClassBodyNode(NodeIndex<DeclarationNode> first);

    };

    struct StructDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypeListNode> baseType;
        NodeIndex<TypeParameterListNode> typeParameters;
        NodeIndex<ClassBodyNode> classBody;

        StructDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<TypeListNode> baseType, NodeIndex<ClassBodyNode> classBody);

    };

    struct InterfaceDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypeListNode> baseType;
        NodeIndex<TypeParameterListNode> typeParameters;
        NodeIndex<ClassBodyNode> classBody;

        explicit InterfaceDeclarationNode();

    };

    struct NamespaceDeclarationNode : DeclarationNode {

        char* name;
        int32 nameLength;

        NodeIndex<DeclarationNode> firstDeclaration;

        explicit NamespaceDeclarationNode(FixedCharSpan name, NodeIndex<DeclarationNode> firstDeclaration);

        inline FixedCharSpan GetName() const {
            return FixedCharSpan(name, nameLength);
        }

    };

    struct ClassDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypeListNode> baseType;
        NodeIndex<TypeParameterListNode> typeParameters;
        NodeIndex<ClassBodyNode> classBody;

        ClassDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<TypeListNode> baseType, NodeIndex<ClassBodyNode> classBody);

    };

    struct InvalidDeclarationNode : DeclarationNode {

        NodeIndex<BlockNode> block;

        explicit InvalidDeclarationNode(NodeIndex<BlockNode> block);

    };

    struct ErrorDeclarationNode : DeclarationNode {

        ErrorDeclarationNode();

    };

    struct IndexerParameterListNode : NodeBase {

        NodeIndex<FormalParameterNode> first;

        explicit IndexerParameterListNode(NodeIndex<FormalParameterNode> first);

    };

    struct IndexerDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<IndexerParameterListNode> parameterList;
        NodeIndex<PropertyGetterNode> getter;
        NodeIndex<PropertySetterNode> setter;

        IndexerDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IndexerParameterListNode> parameterList, NodeIndex<PropertyGetterNode> getter, NodeIndex<PropertySetterNode> setter);

    };

    struct DelegateDeclarationNode : DeclarationNode {

        bool isVoid;
        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<TypeParameterListNode> typeParameters;
        NodeIndex<FormalParameterListNode> parameters;

        DelegateDeclarationNode(NodeIndex<ModifierListNode> modifiers, bool isVoid, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<FormalParameterListNode> parameters);

    };

    struct ConstructorDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<FormalParameterListNode> parameters;
        NodeIndex<BlockNode> body;

        ConstructorDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<FormalParameterListNode> parameters, NodeIndex<BlockNode> body);

    };

    struct FieldDeclarationNode : DeclarationNode {

        NodeIndex<ModifierListNode> modifiers;
        NodeIndex<TypePathNode> typePath;
        NodeIndex<IdentifierNode> identifier;
        NodeIndex<ExpressionNode> initExpression;

        FieldDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> initExpression);

    };

}