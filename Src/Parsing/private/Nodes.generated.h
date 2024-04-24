#pragma once
#include <algorithm> // for std::max
#include "../Nodes.h"

namespace {

    template<typename... Ts>
    constexpr size_t MaxStructSize() {
        return (std::max)({sizeof(Ts)...});
    }

    constexpr size_t kMaxNodeSize = MaxStructSize<
        Alchemy::Parsing::NodeBase,
        Alchemy::Parsing::DeclarationNode,
        Alchemy::Parsing::ExpressionNode,
        Alchemy::Parsing::FileNode,
        Alchemy::Parsing::ArrayRankNode,
        Alchemy::Parsing::TypePathNode,
        Alchemy::Parsing::TypeListNode,
        Alchemy::Parsing::ErrorNode,
        Alchemy::Parsing::AssignmentExpressionNode,
        Alchemy::Parsing::FormalParameterNode,
        Alchemy::Parsing::FormalParameterListNode,
        Alchemy::Parsing::StatementNode,
        Alchemy::Parsing::ReturnStatementNode,
        Alchemy::Parsing::ContinueStatementNode,
        Alchemy::Parsing::BreakStatementNode,
        Alchemy::Parsing::EmptyStatementNode,
        Alchemy::Parsing::BlockNode,
        Alchemy::Parsing::ExpressionListNode,
        Alchemy::Parsing::ContextListNode,
        Alchemy::Parsing::ForLoopIteratorNode,
        Alchemy::Parsing::CatchClauseNode,
        Alchemy::Parsing::TryBlockNode,
        Alchemy::Parsing::TryExpressionNode,
        Alchemy::Parsing::SwitchSectionLabelNode,
        Alchemy::Parsing::SwitchSectionNode,
        Alchemy::Parsing::SwitchStatementNode,
        Alchemy::Parsing::ForEachLoopStatementNode,
        Alchemy::Parsing::ForLoopStatementNode,
        Alchemy::Parsing::WhileStatementNode,
        Alchemy::Parsing::DoWhileStatementNode,
        Alchemy::Parsing::IfStatementNode,
        Alchemy::Parsing::UsingStatementBlockNode,
        Alchemy::Parsing::UsingStatementScopeNode,
        Alchemy::Parsing::UsingStatementNode,
        Alchemy::Parsing::WithStatementNode,
        Alchemy::Parsing::LocalVariableInitializerNode,
        Alchemy::Parsing::LocalVariableTypeNode,
        Alchemy::Parsing::ForInitializerNode,
        Alchemy::Parsing::ForInitializerListNode,
        Alchemy::Parsing::LocalVariableDeclarationNode,
        Alchemy::Parsing::LocalConstantDeclarationNode,
        Alchemy::Parsing::ModifierListNode,
        Alchemy::Parsing::ConstantDeclarationNode,
        Alchemy::Parsing::SwitchExpressionArmNode,
        Alchemy::Parsing::SwitchExpressionNode,
        Alchemy::Parsing::AsExpressionNode,
        Alchemy::Parsing::IsExpressionNode,
        Alchemy::Parsing::ExpressionStatementNode,
        Alchemy::Parsing::ThrowStatementNode,
        Alchemy::Parsing::ThrowExpressionNode,
        Alchemy::Parsing::TernaryExpressionNode,
        Alchemy::Parsing::NullCoalescingExpressionNode,
        Alchemy::Parsing::TypeArgumentListNode,
        Alchemy::Parsing::NamespaceOrTypeNameNode,
        Alchemy::Parsing::TypeConstraintListNode,
        Alchemy::Parsing::RefExpressionNode,
        Alchemy::Parsing::LambdaParameterNode,
        Alchemy::Parsing::LambdaExpressionNode,
        Alchemy::Parsing::IdentifierNode,
        Alchemy::Parsing::TypeParameterListNode,
        Alchemy::Parsing::PropertySetterNode,
        Alchemy::Parsing::PropertyGetterNode,
        Alchemy::Parsing::PropertyDeclarationNode,
        Alchemy::Parsing::EnumMemberDeclarationNode,
        Alchemy::Parsing::EnumMemberListNode,
        Alchemy::Parsing::EnumDeclarationNode,
        Alchemy::Parsing::MethodDeclarationNode,
        Alchemy::Parsing::TypeConstraintNode,
        Alchemy::Parsing::BinaryExpressionNode,
        Alchemy::Parsing::UnaryExpressionNode,
        Alchemy::Parsing::TypeCastExpressionNode,
        Alchemy::Parsing::BracketExpressionNode,
        Alchemy::Parsing::PrimaryExpressionTailNode,
        Alchemy::Parsing::NullableDereferenceNode,
        Alchemy::Parsing::IncrementDecrementNode,
        Alchemy::Parsing::InitializerNode,
        Alchemy::Parsing::IndexedInitializerNode,
        Alchemy::Parsing::CollectionInitializerNode,
        Alchemy::Parsing::ListInitializerNode,
        Alchemy::Parsing::MemberInitializerNode,
        Alchemy::Parsing::ArgumentNode,
        Alchemy::Parsing::ArgumentListNode,
        Alchemy::Parsing::TrailingLambdaStatementNode,
        Alchemy::Parsing::MethodInvocationNode,
        Alchemy::Parsing::PrimaryMethodInvocationNode,
        Alchemy::Parsing::PrimaryExpressionNode,
        Alchemy::Parsing::ScopeMemberAccessNode,
        Alchemy::Parsing::ConditionalMemberAccessNode,
        Alchemy::Parsing::MemberAccessNode,
        Alchemy::Parsing::ThisReferenceExpressionNode,
        Alchemy::Parsing::BaseReferenceExpressionNode,
        Alchemy::Parsing::ParenExpressionNode,
        Alchemy::Parsing::BuiltInTypeAccessExpressionNode,
        Alchemy::Parsing::TypeOfExpressionNode,
        Alchemy::Parsing::StringPartNode,
        Alchemy::Parsing::DynamicArrayLiteralNode,
        Alchemy::Parsing::DynamicKeyNode,
        Alchemy::Parsing::DynamicValueNode,
        Alchemy::Parsing::DynamicKeyValueNode,
        Alchemy::Parsing::DynamicObjectLiteralNode,
        Alchemy::Parsing::NewDynamicObjectLiteralNode,
        Alchemy::Parsing::NewDynamicArrayLiteralNode,
        Alchemy::Parsing::InitializerListNode,
        Alchemy::Parsing::NewExpressionNode,
        Alchemy::Parsing::LiteralExpressionNode,
        Alchemy::Parsing::NamespacePathNode,
        Alchemy::Parsing::UsingNamespaceNode,
        Alchemy::Parsing::UsingStaticNode,
        Alchemy::Parsing::UsingAliasNode,
        Alchemy::Parsing::ClassBodyNode,
        Alchemy::Parsing::StructDeclarationNode,
        Alchemy::Parsing::InterfaceDeclarationNode,
        Alchemy::Parsing::NamespaceDeclarationNode,
        Alchemy::Parsing::ClassDeclarationNode,
        Alchemy::Parsing::InvalidDeclarationNode,
        Alchemy::Parsing::ErrorDeclarationNode,
        Alchemy::Parsing::IndexerParameterListNode,
        Alchemy::Parsing::IndexerDeclarationNode,
        Alchemy::Parsing::DelegateDeclarationNode,
        Alchemy::Parsing::ConstructorDeclarationNode,
        Alchemy::Parsing::FieldDeclarationNode
    >() - sizeof(Alchemy::Parsing::NodeBase);
    
        static_assert(sizeof(Alchemy::Parsing::NodeBase) <= 32);
        static_assert(sizeof(Alchemy::Parsing::DeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::FileNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ArrayRankNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TypePathNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TypeListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ErrorNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::AssignmentExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::FormalParameterNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::FormalParameterListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::StatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ReturnStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ContinueStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::BreakStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::EmptyStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::BlockNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ExpressionListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ContextListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ForLoopIteratorNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::CatchClauseNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TryBlockNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::SwitchSectionLabelNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::SwitchSectionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::SwitchStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ForEachLoopStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ForLoopStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::WhileStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::DoWhileStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::IfStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::UsingStatementBlockNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::UsingStatementScopeNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::UsingStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::WithStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::LocalVariableInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::LocalVariableTypeNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ForInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ForInitializerListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::LocalVariableDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::LocalConstantDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ModifierListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ConstantDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::SwitchExpressionArmNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::SwitchExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::AsExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::IsExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ExpressionStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ThrowStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ThrowExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TernaryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::NullCoalescingExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TypeArgumentListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::NamespaceOrTypeNameNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TypeConstraintListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::RefExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::LambdaParameterNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::LambdaExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::IdentifierNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TypeParameterListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::PropertySetterNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::PropertyGetterNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::PropertyDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::EnumMemberDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::EnumMemberListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::EnumDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::MethodDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TypeConstraintNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::BinaryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::UnaryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TypeCastExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::BracketExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::PrimaryExpressionTailNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::NullableDereferenceNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::IncrementDecrementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::InitializerNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::IndexedInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::CollectionInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ListInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::MemberInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ArgumentNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ArgumentListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TrailingLambdaStatementNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::MethodInvocationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::PrimaryMethodInvocationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::PrimaryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ScopeMemberAccessNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ConditionalMemberAccessNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::MemberAccessNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ThisReferenceExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::BaseReferenceExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ParenExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::BuiltInTypeAccessExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::TypeOfExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::StringPartNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::DynamicArrayLiteralNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::DynamicKeyNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::DynamicValueNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::DynamicKeyValueNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::DynamicObjectLiteralNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::NewDynamicObjectLiteralNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::NewDynamicArrayLiteralNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::InitializerListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::NewExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::LiteralExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::NamespacePathNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::UsingNamespaceNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::UsingStaticNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::UsingAliasNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ClassBodyNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::StructDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::InterfaceDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::NamespaceDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ClassDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::InvalidDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ErrorDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::IndexerParameterListNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::IndexerDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::DelegateDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::ConstructorDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::Parsing::FieldDeclarationNode) <= 32);

}
    
namespace Alchemy::Parsing {

    static_assert(sizeof(AbstractPsiNode) == kMaxNodeSize + sizeof(NodeBase));
    static_assert(sizeof(AbstractPsiNode) == 32);

}
