#include "../Nodes.h"

namespace Alchemy::Parsing {

    NodeBase::NodeBase(NodeType nodeType) : nodeType(nodeType) {}

    TokenRange NodeBase::GetTokenRange() const {
        return TokenRange(tokenStart, tokenEnd);
    }

    DeclarationNode::DeclarationNode(NodeType nodeType) : NodeBase(nodeType) {}

    ExpressionNode::ExpressionNode(NodeType nodeType)
        : NodeBase(nodeType)
        , next(NodeIndex<ExpressionNode>(0)) {}

    FileNode::FileNode()
        : NodeBase(NodeType::File)
        , firstDeclaration(0) {}

    ArrayRankNode::ArrayRankNode(bool isNullable) : NodeBase(NodeType::ArrayRank), isNullable(isNullable) {}

    TypePathNode::TypePathNode(NodeIndex<IdentifierNode> namePath, BuiltInTypeName builtInTypeName, NodeIndex<TypeArgumentListNode> typeArguments, NodeIndex<ArrayRankNode> arrayRank, bool isNullable)
        : NodeBase(NodeType::TypePath)
        , namePath(namePath)
        , builtInTypeName(builtInTypeName)
        , typeArguments(typeArguments)
        , arrayRank(arrayRank)
        , isNullable(isNullable)
        , next(0) {}

    TypeListNode::TypeListNode(NodeIndex<TypePathNode> first)
        : NodeBase(NodeType::TypeList)
        , first(first) {}

    ErrorNode::ErrorNode(char* message, int32 messageLength) : NodeBase(NodeType::Error), message(message), messageLength(messageLength) {}

    AssignmentExpressionNode::AssignmentExpressionNode(NodeIndex<ExpressionNode> rhs, AssignmentOperator op, NodeIndex<ExpressionNode> lhs)
        : ExpressionNode(NodeType::AssignmentExpression)
        , op(op)
        , lhs(lhs)
        , rhs(rhs) {}

    FormalParameterNode::FormalParameterNode(StorageClass storage, ArgumentPassByModifier passByModifier, NodeIndex<TypePathNode> type, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> defaultValue)
        : NodeBase(NodeType::FormalParameter)
        , storage(storage)
        , passByModifier(passByModifier)
        , type(type)
        , identifier(identifier)
        , defaultValue(defaultValue) {}

    FormalParameterListNode::FormalParameterListNode(NodeIndex<FormalParameterNode> listStart)
        : NodeBase(NodeType::FormalParameterList)
        , listStart(listStart) {}

    StatementNode::StatementNode(NodeType nodeType) : NodeBase(nodeType) {}

    ReturnStatementNode::ReturnStatementNode(NodeIndex<ExpressionNode> expression) : StatementNode(NodeType::ReturnStatement), expression(expression) {}

    ContinueStatementNode::ContinueStatementNode() : StatementNode(NodeType::ContinueStatement) {}

    BreakStatementNode::BreakStatementNode() : StatementNode(NodeType::BreakStatement) {}

    EmptyStatementNode::EmptyStatementNode() : StatementNode(NodeType::EmptyStatement) {}

    BlockNode::BlockNode(NodeIndex<StatementNode> firstStatement)
        : StatementNode(NodeType::BlockStatement)
        , firstStatement(firstStatement) {}

    ExpressionListNode::ExpressionListNode(NodeIndex<ExpressionNode> first)
        : NodeBase(NodeType::ExpressionList)
        , first(first) {}

    ContextListNode::ContextListNode(NodeIndex<IdentifierNode> first)
        : NodeBase(NodeType::ContextList)
        , first(first) {}

    ForLoopIteratorNode::ForLoopIteratorNode(NodeIndex<ExpressionNode> first)
        : NodeBase(NodeType::ForLoopIterator)
        , first(first) {}

    CatchClauseNode::CatchClauseNode(NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> blockNode)
        : NodeBase(NodeType::CatchClause)
        , contextList(contextList)
        , block(blockNode) {}

    TryBlockNode::TryBlockNode(NodeIndex<BlockNode> block, NodeIndex<CatchClauseNode> catchClause, NodeIndex<BlockNode> finallyBlock)
        : StatementNode(NodeType::TryBlock)
        , block(block)
        , catchClause(catchClause)
        , finallyBlock(finallyBlock) {}

    TryExpressionNode::TryExpressionNode(NodeIndex<ExpressionNode> expression, NodeIndex<CatchClauseNode> catchClause)
        : StatementNode(NodeType::TryExpression)
        , expression(expression)
        , catchClause(catchClause) {}

    SwitchSectionLabelNode::SwitchSectionLabelNode(NodeIndex<ExpressionNode> expression, NodeIndex<ExpressionNode> caseGuard)
        : NodeBase(NodeType::SwitchLabel)
        , isDefault(false)
        , expression(expression)
        , caseGuard(caseGuard) {}

    SwitchSectionLabelNode::SwitchSectionLabelNode(bool isDefault)
        : NodeBase(NodeType::SwitchLabel)
        , isDefault(isDefault)
        , expression(NodeIndex<ExpressionNode>(0))
        , caseGuard(NodeIndex<ExpressionNode>(0)) {}

    SwitchSectionNode::SwitchSectionNode(NodeIndex<SwitchSectionLabelNode> firstLabel, NodeIndex<StatementNode> firstStatement)
        : NodeBase(NodeType::SwitchSection)
        , firstLabel(firstLabel)
        , firstStatement(firstStatement) {}

    SwitchStatementNode::SwitchStatementNode(NodeIndex<ExpressionNode> expression, NodeIndex<ContextListNode> contextList, NodeIndex<SwitchSectionNode> firstSection)
        : StatementNode(NodeType::SwitchStatement)
        , expression(expression)
        , contextList(contextList)
        , firstSection(firstSection) {}

    ForEachLoopStatementNode::ForEachLoopStatementNode(NodeIndex<ExpressionNode> expression, NodeIndex<ContextListNode> context, NodeIndex<StatementNode> body)
        : StatementNode(NodeType::ForEachLoopStatement)
        , expression(expression)
        , context(context)
        , body(body) {}

    ForLoopStatementNode::ForLoopStatementNode(NodeIndex<ForInitializerListNode> initializer, NodeIndex<ExpressionNode> condition, NodeIndex<ForLoopIteratorNode> iterator, NodeIndex<StatementNode> body)
        : StatementNode(NodeType::ForLoopStatement)
        , initializer(initializer)
        , condition(condition)
        , iterator(iterator)
        , body(body) {}

    WhileStatementNode::WhileStatementNode(NodeIndex<ExpressionListNode> conditions, NodeIndex<ContextListNode> context, NodeIndex<StatementNode> body)
        : StatementNode(NodeType::WhileStatement)
        , conditions(conditions)
        , context(context)
        , body(body) {}

    DoWhileStatementNode::DoWhileStatementNode(NodeIndex<StatementNode> body, NodeIndex<ExpressionNode> condition)
        : StatementNode(NodeType::DoWhileStatement)
        , body(body)
        , condition(condition) {}

    IfStatementNode::IfStatementNode(NodeIndex<ExpressionListNode> conditions, NodeIndex<ContextListNode> context, NodeIndex<StatementNode> body, NodeIndex<StatementNode> elseCase)
        : StatementNode(NodeType::IfStatement)
        , conditions(conditions)
        , context(context)
        , body(body)
        , elseCase(elseCase) {}

    UsingStatementBlockNode::UsingStatementBlockNode(NodeIndex<ExpressionListNode> expressions, NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> block)
        : StatementNode(NodeType::UsingStatementBlock)
        , expressions(expressions)
        , block(block)
        , contextList(contextList) {}

    UsingStatementScopeNode::UsingStatementScopeNode(NodeIndex<ExpressionNode> expression)
        : StatementNode(NodeType::UsingStatementScope)
        , expression(expression) {}

    UsingStatementNode::UsingStatementNode(NodeIndex<ExpressionListNode> expressions, NodeIndex<BlockNode> block, NodeIndex<ContextListNode> contextList)
        : StatementNode(NodeType::UsingStatementBlock)
        , expressions(expressions)
        , block(block)
        , contextList(contextList) {}

    WithStatementNode::WithStatementNode(NodeIndex<ExpressionListNode> expressionList, NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> block)
        : StatementNode(NodeType::WithStatement)
        , expressionList(expressionList)
        , contextList(contextList)
        , block(block) {}

    LocalVariableInitializerNode::LocalVariableInitializerNode(bool isByRef, NodeIndex<ExpressionNode> expression)
        : NodeBase(NodeType::LocalVariableDeclaration)
        , isByRef(isByRef)
        , expression(expression) {}

    LocalVariableTypeNode::LocalVariableTypeNode(StorageClass storageClass, bool isByRef, bool isVarTyped, NodeIndex<TypePathNode> typePath)
        : NodeBase(NodeType::LocalVariableType)
        , storageClass(storageClass)
        , isByRef(isByRef)
        , isVarTyped(isVarTyped)
        , typePath(typePath) {}

    ForInitializerNode::ForInitializerNode(NodeIndex<StatementNode> variableDeclaration)
        : NodeBase(NodeType::ForLoopInitializer)
        , variableDeclaration(variableDeclaration)
        , expression(NodeIndex<ExpressionNode>(0))
        , next(NodeIndex<ForInitializerNode>(0)) {}

    ForInitializerNode::ForInitializerNode(NodeIndex<ExpressionNode> expression)
        : NodeBase(NodeType::ForLoopInitializer)
        , variableDeclaration(NodeIndex<StatementNode>(0))
        , expression(expression)
        , next(NodeIndex<ForInitializerNode>(0)) {}

    ForInitializerListNode::ForInitializerListNode(NodeIndex<ForInitializerNode> first)
        : NodeBase(NodeType::ForInitializerList)
        , first(first) {}

    LocalVariableDeclarationNode::LocalVariableDeclarationNode(NodeIndex<LocalVariableTypeNode> variableType, NodeIndex<IdentifierNode> identifier, NodeIndex<LocalVariableInitializerNode> initializer)
        : StatementNode(NodeType::LocalVariableDeclaration)
        , variableType(variableType)
        , identifier(identifier)
        , initializer(initializer) {}

    LocalConstantDeclarationNode::LocalConstantDeclarationNode(NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> expression)
        : StatementNode(NodeType::LocalConstantDeclaration)
        , typePath(typePath)
        , identifier(identifier)
        , expression(expression) {}

    ModifierListNode::ModifierListNode(DeclarationModifier modifiers)
        : NodeBase(NodeType::ModifierList)
        , modifiers(modifiers) {}

    ConstantDeclarationNode::ConstantDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> expression)
        : DeclarationNode(NodeType::ConstantDeclaration)
        , modifiers(modifiers)
        , typePath(typePath)
        , identifier(identifier)
        , expression(expression) {}

    SwitchExpressionArmNode::SwitchExpressionArmNode(NodeIndex<ExpressionNode> condition, NodeIndex<ExpressionNode> caseGuard, NodeIndex<ExpressionNode> action)
        : NodeBase(NodeType::SwitchExpressionArm)
        , condition(condition)
        , caseGuard(caseGuard)
        , action(action)
        , next(NodeIndex<SwitchExpressionArmNode>(0)) {}

    SwitchExpressionNode::SwitchExpressionNode(NodeIndex<ExpressionNode> switchValue, NodeIndex<SwitchExpressionArmNode> firstArm)
        : ExpressionNode(NodeType::SwitchExpression)
        , switchValue(switchValue)
        , firstArm(firstArm) {}

    AsExpressionNode::AsExpressionNode(bool panicOnFailure, NodeIndex<TypePathNode> type)
        : ExpressionNode(NodeType::AsExpression)
        , panicOnFailure(panicOnFailure)
        , type(type) {}

    IsExpressionNode::IsExpressionNode(NodeIndex<TypePathNode> type, FixedCharSpan name)
        : ExpressionNode(NodeType::IsExpression)
        , type(type)
        , name(name) {}

    ExpressionStatementNode::ExpressionStatementNode(NodeIndex<ExpressionNode> expression)
        : StatementNode(NodeType::ExpressionStatement)
        , expression(expression) {}

    ThrowStatementNode::ThrowStatementNode(NodeIndex<ExpressionNode> expression) : StatementNode(NodeType::ThrowStatement), expression(expression) {}

    ThrowExpressionNode::ThrowExpressionNode(NodeIndex<ExpressionNode> expression)
        : ExpressionNode(NodeType::ThrowExpression)
        , expression(expression) {}

    TernaryExpressionNode::TernaryExpressionNode(NodeIndex<ExpressionNode> conditionNodeIndex, NodeIndex<ExpressionNode> trueNodeIndex, NodeIndex<ExpressionNode> falseNodeIndex)
        : ExpressionNode(NodeType::TernaryExpression)
        , conditionNodeIndex(conditionNodeIndex)
        , trueNodeIndex(trueNodeIndex)
        , falseNodeIndex(falseNodeIndex) {}

    NullCoalescingExpressionNode::NullCoalescingExpressionNode(NodeIndex<ExpressionNode> lhs, NodeIndex<ExpressionNode> rhs)
        : ExpressionNode(NodeType::NullCoalescingExpression)
        , lhs(lhs)
        , rhs(rhs) {}

    TypeArgumentListNode::TypeArgumentListNode(NodeIndex<TypePathNode> first)
        : NodeBase(NodeType::TypeArgumentList)
        , first(first) {}

    NamespaceOrTypeNameNode::NamespaceOrTypeNameNode(FixedCharSpan name, BuiltInTypeName builtInTypeName, NodeIndex<TypeArgumentListNode> typeArguments, NodeIndex<NamespaceOrTypeNameNode> next)
        : NodeBase(NodeType::NamespaceOrTypeName)
        , name(name.ptr)
        , nameLength(name.size)
        , builtInTypeName(builtInTypeName)
        , typeArguments(typeArguments)
        , next(next) {}

    RefExpressionNode::RefExpressionNode(NodeIndex<ExpressionNode> expression)
        : ExpressionNode(NodeType::RefExpression)
        , expression(expression) {}

    LambdaParameterNode::LambdaParameterNode(StorageClass storageClass, ArgumentPassByModifier passByModifier, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier)
        : NodeBase(NodeType::LambdaParameter)
        , storageClass(storageClass)
        , passByModifier(passByModifier)
        , typePath(typePath)
        , identifier(identifier) {}

    LambdaExpressionNode::LambdaExpressionNode(NodeIndex<LambdaParameterNode> firstParameter, NodeIndex<BlockNode> block, NodeIndex<ExpressionNode> expression)
        : ExpressionNode(NodeType::LambdaExpression)
        , firstParameter(firstParameter)
        , block(block)
        , expression(expression) {}

    IdentifierNode::IdentifierNode(FixedCharSpan identifier, NodeIndex<TypeArgumentListNode> typeArguments)
        : ExpressionNode(NodeType::Identifier)
        , name(identifier.ptr)
        , nameLength(identifier.size)
        , typeArguments(typeArguments)
        , next(0) {}

    TypeParameterListNode::TypeParameterListNode(NodeIndex<IdentifierNode> first)
        : NodeBase(NodeType::TypeParameterListNode)
        , first(first) {}

    PropertySetterNode::PropertySetterNode(DefinitionVisibility visibility)
        : NodeBase(NodeType::PropertySetter)
        , visibility(visibility)
        , valueIdentifier(NodeIndex<IdentifierNode>(0))
        , expression(NodeIndex<ExpressionNode>(0))
        , block(NodeIndex<BlockNode>(0)) {}

    PropertySetterNode::PropertySetterNode(DefinitionVisibility visibility, NodeIndex<IdentifierNode> valueIdentifier, NodeIndex<ExpressionNode> expression)
        : NodeBase(NodeType::PropertySetter)
        , visibility(visibility)
        , valueIdentifier(valueIdentifier)
        , expression(expression)
        , block(NodeIndex<BlockNode>(0)) {}

    PropertySetterNode::PropertySetterNode(DefinitionVisibility visibility, NodeIndex<IdentifierNode> valueIdentifier, NodeIndex<BlockNode> block)
        : NodeBase(NodeType::PropertySetter)
        , visibility(visibility)
        , valueIdentifier(valueIdentifier)
        , expression(NodeIndex<ExpressionNode>(0))
        , block(block) {}

    PropertyGetterNode::PropertyGetterNode(DefinitionVisibility visibility)
        : NodeBase(NodeType::PropertyGetter)
        , visibility(visibility)
        , expression(NodeIndex<ExpressionNode>(0))
        , block(NodeIndex<BlockNode>(0)) {}

    PropertyGetterNode::PropertyGetterNode(DefinitionVisibility visibility, NodeIndex<BlockNode> block)
        : NodeBase(NodeType::PropertyGetter)
        , visibility(visibility)
        , expression(NodeIndex<ExpressionNode>(0))
        , block(block) {}

    PropertyGetterNode::PropertyGetterNode(DefinitionVisibility visibility, NodeIndex<ExpressionNode> expression)
        : NodeBase(NodeType::PropertyGetter)
        , visibility(visibility)
        , expression(expression)
        , block(NodeIndex<BlockNode>(0)) {}

    PropertyDeclarationNode::PropertyDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePathNode, NodeIndex<IdentifierNode> identifier, NodeIndex<PropertyGetterNode> getterNode, NodeIndex<PropertySetterNode> setterNode)
        : DeclarationNode(NodeType::PropertyDeclaration)
        , modifiers(modifiers)
        , typePath(typePathNode)
        , identifier(identifier)
        , getterNode(getterNode)
        , setterNode(setterNode) {}

    EnumMemberDeclarationNode::EnumMemberDeclarationNode(NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> expression)
        : DeclarationNode(NodeType::EnumMemberDeclaration)
        , identifier(identifier)
        , expression(expression)
        , next(0) {}

    EnumMemberListNode::EnumMemberListNode(NodeIndex<EnumMemberDeclarationNode> first)
        : NodeBase(NodeType::EnumMemberList)
        , first(first) {}

    EnumDeclarationNode::EnumDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<TypePathNode> baseType, NodeIndex<EnumMemberListNode> members)
        : DeclarationNode(NodeType::EnumDeclaration)
        , modifiers(modifiers)
        , identifier(identifier)
        , baseType(baseType)
        , members(members) {}

    MethodDeclarationNode::MethodDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePathNode, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<FormalParameterListNode> parameters, NodeIndex<BlockNode> body, NodeIndex<ExpressionNode> arrowBody)
        : DeclarationNode(NodeType::MethodDeclaration)
        , modifiers(modifiers)
        , returnType(typePathNode)
        , identifier(identifier)
        , typeParameters(typeParameters)
        , parameters(parameters)
        , body(body)
        , arrowBody(arrowBody) {}

    TypeConstraintNode::TypeConstraintNode() : NodeBase(NodeType::TypeConstraint) {}

    BinaryExpressionNode::BinaryExpressionNode(BinaryExpressionType type, NodeIndex<ExpressionNode> lhs, NodeIndex<ExpressionNode> rhs)
        : ExpressionNode(NodeType::BinaryExpression)
        , lhs(lhs)
        , rhs(rhs)
        , type(type) {}

    UnaryExpressionNode::UnaryExpressionNode(UnaryExpressionType type, NodeIndex<ExpressionNode> expression)
        : ExpressionNode(NodeType::UnaryExpression)
        , type(type)
        , expression(expression) {}

    TypeCastExpressionNode::TypeCastExpressionNode(NodeIndex<TypePathNode> type, NodeIndex<ExpressionNode> expression)
        : ExpressionNode(NodeType::TypeCast)
        , type(type)
        , expression(expression) {}

    BracketExpressionNode::BracketExpressionNode(bool isNullable, NodeIndex<ExpressionNode> expression)
        : NodeBase(NodeType::BracketExpression)
        , isNullable(isNullable)
        , expression(expression)
        , next(NodeIndex<BracketExpressionNode>(0)) {}

    PrimaryExpressionTailNode::PrimaryExpressionTailNode(UntypedNodeIndex part, NodeIndex<BracketExpressionNode> bracketListStart)
        : NodeBase(NodeType::PrimaryExpressionTail)
        , part(part)
        , bracketListStart(bracketListStart)
        , next(0) // assigned later
    {}

    NullableDereferenceNode::NullableDereferenceNode()
        : NodeBase(NodeType::NullableDereference) {}

    IncrementDecrementNode::IncrementDecrementNode(bool isIncrement) : NodeBase(NodeType::IncrementDecrement), isIncrement(isIncrement) {}

    InitializerNode::InitializerNode(NodeType nodeType)
        : NodeBase(nodeType)
        , next(NodeIndex<InitializerNode>(0)) {}

    IndexedInitializerNode::IndexedInitializerNode(NodeIndex<ArgumentListNode> argumentList, NodeIndex<ExpressionNode> value)
        : InitializerNode(NodeType::IndexedInitializer)
        , argumentList(argumentList)
        , value(value) {}

    CollectionInitializerNode::CollectionInitializerNode(NodeIndex<ExpressionNode> first)
        : InitializerNode(NodeType::CollectionInitializer)
        , first(first) {}

    ListInitializerNode::ListInitializerNode(NodeIndex<ExpressionNode> value) : InitializerNode(NodeType::ListInitializer) {}

    MemberInitializerNode::MemberInitializerNode(FixedCharSpan memberName, NodeIndex<ExpressionNode> value)
        : InitializerNode(NodeType::MemberInitializer)
        , memberName(memberName.ptr)
        , memberNameLength(memberName.size)
        , value(value) {}

    ArgumentNode::ArgumentNode(ArgumentPassByModifier passBy, bool isVarType, NodeIndex<TypePathNode> typePath, NodeIndex<ExpressionNode> expression)
        : NodeBase(NodeType::Argument)
        , passBy(passBy)
        , isVarType(isVarType)
        , typePath(typePath)
        , expression(expression)
        , variableName(nullptr)
        , variableNameLength(0)
        , next(NodeIndex<ArgumentNode>(0)) // set explicitly later
    {}

    ArgumentNode::ArgumentNode(ArgumentPassByModifier passBy, bool isVarType, NodeIndex<TypePathNode> typePath, FixedCharSpan variableName)
        : NodeBase(NodeType::Argument)
        , passBy(passBy)
        , isVarType(isVarType)
        , typePath(typePath)
        , expression(NodeIndex<ExpressionNode>(0))
        , variableName(variableName.ptr)
        , variableNameLength(variableName.size)
        , next(NodeIndex<ArgumentNode>(0)) // set explicitly later
    {}

    ArgumentListNode::ArgumentListNode(NodeIndex<ArgumentNode> first)
        : NodeBase(NodeType::ArgumentList)
        , first(first) {}

    TrailingLambdaStatementNode::TrailingLambdaStatementNode(NodeIndex<ExpressionNode> expression, NodeIndex<ContextListNode> contextList, NodeIndex<BlockNode> block)
        : StatementNode(NodeType::TrailingLambdaStatement)
        , expression(expression)
        , contextList(contextList)
        , block(block) {}

    MethodInvocationNode::MethodInvocationNode(NodeIndex<ArgumentListNode> arguments)
        : NodeBase(NodeType::MethodInvocation)
        , arguments(arguments) {}

    PrimaryMethodInvocationNode::PrimaryMethodInvocationNode(NodeIndex<IdentifierNode> identifier, NodeIndex<ArgumentListNode> arguments)
        : NodeBase(NodeType::PrimaryMethodInvocation)
        , identifier(identifier)
        , arguments(arguments) {}

    PrimaryExpressionNode::PrimaryExpressionNode(NodeIndex<ExpressionNode> start, NodeIndex<BracketExpressionNode> startBracketExpression, NodeIndex<PrimaryExpressionTailNode> tail)
        : ExpressionNode(NodeType::PrimaryExpression)
        , start(start)
        , startBracketExpression(startBracketExpression)
        , tail(tail) {}

    ScopeMemberAccessNode::ScopeMemberAccessNode(NodeIndex<IdentifierNode> identifier)
        : NodeBase(NodeType::ScopeMemberAccess)
        , identifier(identifier) {}

    ConditionalMemberAccessNode::ConditionalMemberAccessNode(NodeIndex<IdentifierNode> identifier)
        : NodeBase(NodeType::ConditionalMemberAccess)
        , identifier(identifier) {}

    MemberAccessNode::MemberAccessNode(NodeIndex<IdentifierNode> identifier)
        : NodeBase(NodeType::MemberAccess)
        , identifier(identifier) {}

    ThisReferenceExpressionNode::ThisReferenceExpressionNode() : ExpressionNode(NodeType::ThisReferenceExpression) {}

    BaseReferenceExpressionNode::BaseReferenceExpressionNode() : ExpressionNode(NodeType::BaseReferenceExpression) {}

    ParenExpressionNode::ParenExpressionNode(NodeIndex<ExpressionNode> expression)
        : ExpressionNode(NodeType::ParenExpression)
        , expression(expression) {}

    BuiltInTypeAccessExpressionNode::BuiltInTypeAccessExpressionNode(BuiltInTypeName typeName)
        : ExpressionNode(NodeType::BuiltInTypeAccess)
        , typeName(typeName) {}

    TypeOfExpressionNode::TypeOfExpressionNode(NodeIndex<TypePathNode> type)
        : ExpressionNode(NodeType::TypeOfExpression)
        , type(type) {}

    StringPartNode::StringPartNode(StringPartType partType, NodeIndex<ExpressionNode> expression)
        : NodeBase(NodeType::StringPart)
        , partType(partType)
        , expression(expression)
        , source(nullptr)
        , sourceLength(0)
        , next(NodeIndex<StringPartNode>(0)) {}

    StringPartNode::StringPartNode(StringPartType partType, FixedCharSpan source)
        : NodeBase(NodeType::StringPart)
        , partType(partType)
        , source(source.ptr)
        , sourceLength(source.size)
        , expression(NodeIndex<ExpressionNode>(0))
        , next(NodeIndex<StringPartNode>(0)) {}

    DynamicArrayLiteralNode::DynamicArrayLiteralNode(NodeIndex<ExpressionNode> firstExpression)
        : ExpressionNode(NodeType::DynamicArrayLiteral)
        , firstExpression(firstExpression) {}

    DynamicKeyNode::DynamicKeyNode(FixedCharSpan identifier)
        : NodeBase(NodeType::DynamicKey)
        , identifier(identifier)
        , expression(NodeIndex<ExpressionNode>(0)) {}

    DynamicKeyNode::DynamicKeyNode(NodeIndex<ExpressionNode> expression)
        : NodeBase(NodeType::DynamicKey)
        , identifier(FixedCharSpan())
        , expression(expression) {}

    DynamicValueNode::DynamicValueNode(NodeType nodeType)
        : NodeBase(nodeType) {}

    DynamicKeyValueNode::DynamicKeyValueNode(NodeIndex<DynamicKeyNode> key, NodeIndex<ExpressionNode> value)
        : NodeBase(NodeType::DynamicKeyValue)
        , key(key)
        , value(value) {}

    DynamicObjectLiteralNode::DynamicObjectLiteralNode(NodeIndex<DynamicKeyValueNode> first)
        : ExpressionNode(NodeType::DynamicObjectLiteral)
        , first(first) {
    }

    NewDynamicObjectLiteralNode::NewDynamicObjectLiteralNode(AllocatorType allocatorType, NodeIndex<DynamicObjectLiteralNode> objectLiteral)
        : ExpressionNode(NodeType::NewDynamicObjectLiteralExpression)
        , allocatorType(allocatorType)
        , objectLiteral(objectLiteral) {}

    NewDynamicArrayLiteralNode::NewDynamicArrayLiteralNode(AllocatorType allocatorType, NodeIndex<DynamicArrayLiteralNode> arrayLiteral)
        : ExpressionNode(NodeType::NewDynamicArrayLiteralExpression)
        , allocatorType(allocatorType)
        , arrayLiteral(arrayLiteral) {}

    InitializerListNode::InitializerListNode(NodeIndex<InitializerNode> first)
        : NodeBase(NodeType::InitializerList)
        , first(first) {}

    NewExpressionNode::NewExpressionNode(NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> constructorName, NodeIndex<ArgumentListNode> argumentList, NodeIndex<InitializerListNode> initializerList)
        : ExpressionNode(NodeType::NewExpression)
        , constructorName(constructorName)
        , typePath(typePath)
        , argumentList(argumentList)
        , initializerList(initializerList) {}

    LiteralExpressionNode::LiteralExpressionNode(LiteralType literalType, LiteralValue literalValue)
        : ExpressionNode(NodeType::Literal)
        , literalType(literalType)
        , literalValue(literalValue)
        , defaultType(0) {}

    LiteralExpressionNode::LiteralExpressionNode(LiteralType literalType, NodeIndex<TypePathNode> defaultType)
        : ExpressionNode(NodeType::Literal)
        , literalType(literalType)
        , literalValue()
        , defaultType(defaultType) {}

    LiteralExpressionNode::LiteralExpressionNode(LiteralType literalType)
        : ExpressionNode(NodeType::Literal)
        , literalType(literalType)
        , literalValue()
        , defaultType(0) {}

    NamespacePathNode::NamespacePathNode(FixedCharSpan namespacePath)
        : NodeBase(NodeType::NamespacePath)
        , namespaceName(namespacePath.ptr)
        , namespaceNameLength(namespacePath.size) {}

    UsingNamespaceNode::UsingNamespaceNode(NodeIndex<NamespacePathNode> namespacePath)
        : DeclarationNode(NodeType::UsingNamespaceDeclaration)
        , namespacePath(namespacePath) {}

    UsingStaticNode::UsingStaticNode(NodeIndex<TypePathNode> type)
        : DeclarationNode(NodeType::UsingStaticDeclaration)
        , type(type) {}

    UsingAliasNode::UsingAliasNode(FixedCharSpan alias, NodeIndex<TypePathNode> type)
        : DeclarationNode(NodeType::UsingAliasDeclaration)
        , alias(alias.ptr)
        , length(alias.size)
        , type(type) {}

    ClassBodyNode::ClassBodyNode(NodeIndex<DeclarationNode> first)
        : NodeBase(NodeType::ClassBody)
        , first(first) {}

    StructDeclarationNode::StructDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<TypeListNode> baseType, NodeIndex<ClassBodyNode> classBody)
        : DeclarationNode(NodeType::StructDeclaration)
        , modifiers(modifiers)
        , identifier(identifier)
        , typeParameters(typeParameters)
        , baseType(baseType)
        , classBody(classBody) {}

    InterfaceDeclarationNode::InterfaceDeclarationNode()
        : DeclarationNode(NodeType::InterfaceDeclaration) {}

    NamespaceDeclarationNode::NamespaceDeclarationNode(FixedCharSpan name, NodeIndex<DeclarationNode> firstDeclaration)
        : DeclarationNode(NodeType::NamespaceDeclaration)
        , name(name.ptr)
        , nameLength(name.size)
        , firstDeclaration(firstDeclaration) {}

    ClassDeclarationNode::ClassDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<TypeListNode> baseType, NodeIndex<ClassBodyNode> classBody)
        : DeclarationNode(NodeType::ClassDeclaration)
        , modifiers(modifiers)
        , identifier(identifier)
        , typeParameters(typeParameters)
        , baseType(baseType)
        , classBody(classBody) {}

    InvalidDeclarationNode::InvalidDeclarationNode(NodeIndex<BlockNode> block)
        : DeclarationNode(NodeType::InvalidDeclaration)
        , block(block) {}

    ErrorDeclarationNode::ErrorDeclarationNode() : DeclarationNode(NodeType::Error) {}

    IndexerParameterListNode::IndexerParameterListNode(NodeIndex<FormalParameterNode> first)
        : NodeBase(NodeType::IndexerParameterList)
        , first(first) {}

    IndexerDeclarationNode::IndexerDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IndexerParameterListNode> parameterList, NodeIndex<PropertyGetterNode> getter, NodeIndex<PropertySetterNode> setter)
        : DeclarationNode(NodeType::IndexerDeclaration)
        , modifiers(modifiers)
        , typePath(typePath)
        , parameterList(parameterList)
        , setter(setter)
        , getter(getter) {}

    DelegateDeclarationNode::DelegateDeclarationNode(NodeIndex<ModifierListNode> modifiers, bool isVoid, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<TypeParameterListNode> typeParameters, NodeIndex<FormalParameterListNode> parameters)
        : DeclarationNode(NodeType::DelegateDeclaration)
        , modifiers(modifiers)
        , isVoid(isVoid)
        , typePath(typePath)
        , identifier(identifier)
        , typeParameters(typeParameters)
        , parameters(parameters) {}

    ConstructorDeclarationNode::ConstructorDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<IdentifierNode> identifier, NodeIndex<FormalParameterListNode> parameters, NodeIndex<BlockNode> body)
        : DeclarationNode(NodeType::ConstructorDeclaration)
        , modifiers(modifiers)
        , identifier(identifier)
        , parameters(parameters)
        , body(body) {}

    FieldDeclarationNode::FieldDeclarationNode(NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<ExpressionNode> initExpression)
        : DeclarationNode(NodeType::FieldDeclaration)
        , modifiers(modifiers)
        , typePath(typePath)
        , identifier(identifier)
        , initExpression(initExpression) {}
}
