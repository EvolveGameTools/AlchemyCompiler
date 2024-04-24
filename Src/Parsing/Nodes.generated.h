#pragma once
#include <algorithm> // for std::max
#include "./Nodes.h"

namespace {

    template<typename... Ts>
    constexpr size_t MaxStructSize() {
        return (std::max)({sizeof(Ts)...});
    }

    constexpr size_t kMaxNodeSize = MaxStructSize<
        Alchemy::NodeBase,
        Alchemy::DeclarationNode,
        Alchemy::ExpressionNode,
        Alchemy::FileNode,
        Alchemy::ArrayRankNode,
        Alchemy::TypePathNode,
        Alchemy::TypeListNode,
        Alchemy::ErrorNode,
        Alchemy::AssignmentExpressionNode,
        Alchemy::FormalParameterNode,
        Alchemy::FormalParameterListNode,
        Alchemy::StatementNode,
        Alchemy::ReturnStatementNode,
        Alchemy::ContinueStatementNode,
        Alchemy::BreakStatementNode,
        Alchemy::EmptyStatementNode,
        Alchemy::BlockNode,
        Alchemy::ExpressionListNode,
        Alchemy::ContextListNode,
        Alchemy::ForLoopIteratorNode,
        Alchemy::CatchClauseNode,
        Alchemy::TryBlockNode,
        Alchemy::TryExpressionNode,
        Alchemy::SwitchSectionLabelNode,
        Alchemy::SwitchSectionNode,
        Alchemy::SwitchStatementNode,
        Alchemy::ForEachLoopStatementNode,
        Alchemy::ForLoopStatementNode,
        Alchemy::WhileStatementNode,
        Alchemy::DoWhileStatementNode,
        Alchemy::IfStatementNode,
        Alchemy::UsingStatementBlockNode,
        Alchemy::UsingStatementScopeNode,
        Alchemy::UsingStatementNode,
        Alchemy::WithStatementNode,
        Alchemy::LocalVariableInitializerNode,
        Alchemy::LocalVariableTypeNode,
        Alchemy::ForInitializerNode,
        Alchemy::ForInitializerListNode,
        Alchemy::LocalVariableDeclarationNode,
        Alchemy::LocalConstantDeclarationNode,
        Alchemy::ModifierListNode,
        Alchemy::ConstantDeclarationNode,
        Alchemy::SwitchExpressionArmNode,
        Alchemy::SwitchExpressionNode,
        Alchemy::AsExpressionNode,
        Alchemy::IsExpressionNode,
        Alchemy::ExpressionStatementNode,
        Alchemy::ThrowStatementNode,
        Alchemy::ThrowExpressionNode,
        Alchemy::TernaryExpressionNode,
        Alchemy::NullCoalescingExpressionNode,
        Alchemy::TypeArgumentListNode,
        Alchemy::NamespaceOrTypeNameNode,
        Alchemy::TypeConstraintListNode,
        Alchemy::RefExpressionNode,
        Alchemy::LambdaParameterNode,
        Alchemy::LambdaExpressionNode,
        Alchemy::IdentifierNode,
        Alchemy::TypeParameterListNode,
        Alchemy::PropertySetterNode,
        Alchemy::PropertyGetterNode,
        Alchemy::PropertyDeclarationNode,
        Alchemy::EnumMemberDeclarationNode,
        Alchemy::EnumMemberListNode,
        Alchemy::EnumDeclarationNode,
        Alchemy::MethodDeclarationNode,
        Alchemy::TypeConstraintNode,
        Alchemy::BinaryExpressionNode,
        Alchemy::UnaryExpressionNode,
        Alchemy::TypeCastExpressionNode,
        Alchemy::BracketExpressionNode,
        Alchemy::PrimaryExpressionTailNode,
        Alchemy::NullableDereferenceNode,
        Alchemy::IncrementDecrementNode,
        Alchemy::InitializerNode,
        Alchemy::IndexedInitializerNode,
        Alchemy::CollectionInitializerNode,
        Alchemy::ListInitializerNode,
        Alchemy::MemberInitializerNode,
        Alchemy::ArgumentNode,
        Alchemy::ArgumentListNode,
        Alchemy::TrailingLambdaStatementNode,
        Alchemy::MethodInvocationNode,
        Alchemy::PrimaryMethodInvocationNode,
        Alchemy::PrimaryExpressionNode,
        Alchemy::ScopeMemberAccessNode,
        Alchemy::ConditionalMemberAccessNode,
        Alchemy::MemberAccessNode,
        Alchemy::ThisReferenceExpressionNode,
        Alchemy::BaseReferenceExpressionNode,
        Alchemy::ParenExpressionNode,
        Alchemy::BuiltInTypeAccessExpressionNode,
        Alchemy::TypeOfExpressionNode,
        Alchemy::StringPartNode,
        Alchemy::DynamicArrayLiteralNode,
        Alchemy::DynamicKeyNode,
        Alchemy::DynamicValueNode,
        Alchemy::DynamicKeyValueNode,
        Alchemy::DynamicObjectLiteralNode,
        Alchemy::NewDynamicObjectLiteralNode,
        Alchemy::NewDynamicArrayLiteralNode,
        Alchemy::InitializerListNode,
        Alchemy::NewExpressionNode,
        Alchemy::LiteralExpressionNode,
        Alchemy::NamespacePathNode,
        Alchemy::UsingNamespaceNode,
        Alchemy::UsingStaticNode,
        Alchemy::UsingAliasNode,
        Alchemy::ClassBodyNode,
        Alchemy::StructDeclarationNode,
        Alchemy::InterfaceDeclarationNode,
        Alchemy::NamespaceDeclarationNode,
        Alchemy::ClassDeclarationNode,
        Alchemy::InvalidDeclarationNode,
        Alchemy::ErrorDeclarationNode,
        Alchemy::IndexerParameterListNode,
        Alchemy::IndexerDeclarationNode,
        Alchemy::DelegateDeclarationNode,
        Alchemy::ConstructorDeclarationNode,
        Alchemy::FieldDeclarationNode    
    >() - sizeof(Alchemy::NodeBase);
    
        static_assert(sizeof(Alchemy::NodeBase) <= 32);
        static_assert(sizeof(Alchemy::DeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::ExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::FileNode) <= 32);
        static_assert(sizeof(Alchemy::ArrayRankNode) <= 32);
        static_assert(sizeof(Alchemy::TypePathNode) <= 32);
        static_assert(sizeof(Alchemy::TypeListNode) <= 32);
        static_assert(sizeof(Alchemy::ErrorNode) <= 32);
        static_assert(sizeof(Alchemy::AssignmentExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::FormalParameterNode) <= 32);
        static_assert(sizeof(Alchemy::FormalParameterListNode) <= 32);
        static_assert(sizeof(Alchemy::StatementNode) <= 32);
        static_assert(sizeof(Alchemy::ReturnStatementNode) <= 32);
        static_assert(sizeof(Alchemy::ContinueStatementNode) <= 32);
        static_assert(sizeof(Alchemy::BreakStatementNode) <= 32);
        static_assert(sizeof(Alchemy::EmptyStatementNode) <= 32);
        static_assert(sizeof(Alchemy::BlockNode) <= 32);
        static_assert(sizeof(Alchemy::ExpressionListNode) <= 32);
        static_assert(sizeof(Alchemy::ContextListNode) <= 32);
        static_assert(sizeof(Alchemy::ForLoopIteratorNode) <= 32);
        static_assert(sizeof(Alchemy::CatchClauseNode) <= 32);
        static_assert(sizeof(Alchemy::TryBlockNode) <= 32);
        static_assert(sizeof(Alchemy::TryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::SwitchSectionLabelNode) <= 32);
        static_assert(sizeof(Alchemy::SwitchSectionNode) <= 32);
        static_assert(sizeof(Alchemy::SwitchStatementNode) <= 32);
        static_assert(sizeof(Alchemy::ForEachLoopStatementNode) <= 32);
        static_assert(sizeof(Alchemy::ForLoopStatementNode) <= 32);
        static_assert(sizeof(Alchemy::WhileStatementNode) <= 32);
        static_assert(sizeof(Alchemy::DoWhileStatementNode) <= 32);
        static_assert(sizeof(Alchemy::IfStatementNode) <= 32);
        static_assert(sizeof(Alchemy::UsingStatementBlockNode) <= 32);
        static_assert(sizeof(Alchemy::UsingStatementScopeNode) <= 32);
        static_assert(sizeof(Alchemy::UsingStatementNode) <= 32);
        static_assert(sizeof(Alchemy::WithStatementNode) <= 32);
        static_assert(sizeof(Alchemy::LocalVariableInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::LocalVariableTypeNode) <= 32);
        static_assert(sizeof(Alchemy::ForInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::ForInitializerListNode) <= 32);
        static_assert(sizeof(Alchemy::LocalVariableDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::LocalConstantDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::ModifierListNode) <= 32);
        static_assert(sizeof(Alchemy::ConstantDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::SwitchExpressionArmNode) <= 32);
        static_assert(sizeof(Alchemy::SwitchExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::AsExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::IsExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::ExpressionStatementNode) <= 32);
        static_assert(sizeof(Alchemy::ThrowStatementNode) <= 32);
        static_assert(sizeof(Alchemy::ThrowExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::TernaryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::NullCoalescingExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::TypeArgumentListNode) <= 32);
        static_assert(sizeof(Alchemy::NamespaceOrTypeNameNode) <= 32);
        static_assert(sizeof(Alchemy::TypeConstraintListNode) <= 32);
        static_assert(sizeof(Alchemy::RefExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::LambdaParameterNode) <= 32);
        static_assert(sizeof(Alchemy::LambdaExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::IdentifierNode) <= 32);
        static_assert(sizeof(Alchemy::TypeParameterListNode) <= 32);
        static_assert(sizeof(Alchemy::PropertySetterNode) <= 32);
        static_assert(sizeof(Alchemy::PropertyGetterNode) <= 32);
        static_assert(sizeof(Alchemy::PropertyDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::EnumMemberDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::EnumMemberListNode) <= 32);
        static_assert(sizeof(Alchemy::EnumDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::MethodDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::TypeConstraintNode) <= 32);
        static_assert(sizeof(Alchemy::BinaryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::UnaryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::TypeCastExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::BracketExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::PrimaryExpressionTailNode) <= 32);
        static_assert(sizeof(Alchemy::NullableDereferenceNode) <= 32);
        static_assert(sizeof(Alchemy::IncrementDecrementNode) <= 32);
        static_assert(sizeof(Alchemy::InitializerNode) <= 32);
        static_assert(sizeof(Alchemy::IndexedInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::CollectionInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::ListInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::MemberInitializerNode) <= 32);
        static_assert(sizeof(Alchemy::ArgumentNode) <= 32);
        static_assert(sizeof(Alchemy::ArgumentListNode) <= 32);
        static_assert(sizeof(Alchemy::TrailingLambdaStatementNode) <= 32);
        static_assert(sizeof(Alchemy::MethodInvocationNode) <= 32);
        static_assert(sizeof(Alchemy::PrimaryMethodInvocationNode) <= 32);
        static_assert(sizeof(Alchemy::PrimaryExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::ScopeMemberAccessNode) <= 32);
        static_assert(sizeof(Alchemy::ConditionalMemberAccessNode) <= 32);
        static_assert(sizeof(Alchemy::MemberAccessNode) <= 32);
        static_assert(sizeof(Alchemy::ThisReferenceExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::BaseReferenceExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::ParenExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::BuiltInTypeAccessExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::TypeOfExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::StringPartNode) <= 32);
        static_assert(sizeof(Alchemy::DynamicArrayLiteralNode) <= 32);
        static_assert(sizeof(Alchemy::DynamicKeyNode) <= 32);
        static_assert(sizeof(Alchemy::DynamicValueNode) <= 32);
        static_assert(sizeof(Alchemy::DynamicKeyValueNode) <= 32);
        static_assert(sizeof(Alchemy::DynamicObjectLiteralNode) <= 32);
        static_assert(sizeof(Alchemy::NewDynamicObjectLiteralNode) <= 32);
        static_assert(sizeof(Alchemy::NewDynamicArrayLiteralNode) <= 32);
        static_assert(sizeof(Alchemy::InitializerListNode) <= 32);
        static_assert(sizeof(Alchemy::NewExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::LiteralExpressionNode) <= 32);
        static_assert(sizeof(Alchemy::NamespacePathNode) <= 32);
        static_assert(sizeof(Alchemy::UsingNamespaceNode) <= 32);
        static_assert(sizeof(Alchemy::UsingStaticNode) <= 32);
        static_assert(sizeof(Alchemy::UsingAliasNode) <= 32);
        static_assert(sizeof(Alchemy::ClassBodyNode) <= 32);
        static_assert(sizeof(Alchemy::StructDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::InterfaceDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::NamespaceDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::ClassDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::InvalidDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::ErrorDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::IndexerParameterListNode) <= 32);
        static_assert(sizeof(Alchemy::IndexerDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::DelegateDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::ConstructorDeclarationNode) <= 32);
        static_assert(sizeof(Alchemy::FieldDeclarationNode) <= 32);


}
    
namespace Alchemy {

    struct AbstractPsiNode {

        NodeBase nodeBase;
        
        char bytes[kMaxNodeSize] { };
        
        TokenRange GetTokenRange() {
            return nodeBase.GetTokenRange();
        }

    };

    static_assert(sizeof(AbstractPsiNode) == kMaxNodeSize + sizeof(NodeBase));
    static_assert(sizeof(AbstractPsiNode) == 32);

}
