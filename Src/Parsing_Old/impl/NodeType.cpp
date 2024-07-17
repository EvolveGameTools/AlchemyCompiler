#include "../NodeType.h"

namespace Alchemy::Parsing {

    // --- Generate NodeTypeToString Start
    const char* NodeTypeToString(NodeType e) {
        switch(e) {
            case NodeType::File: return "File";
            case NodeType::Error: return "Error";
            case NodeType::ArrayRank: return "ArrayRank";
            case NodeType::UsingStaticDeclaration: return "UsingStaticDeclaration";
            case NodeType::UsingNamespaceDeclaration: return "UsingNamespaceDeclaration";
            case NodeType::UsingAliasDeclaration: return "UsingAliasDeclaration";
            case NodeType::MethodDeclaration: return "MethodDeclaration";
            case NodeType::LocalVariableDeclaration: return "LocalVariableDeclaration";
            case NodeType::InvalidDeclaration: return "InvalidDeclaration";
            case NodeType::PropertyDeclaration: return "PropertyDeclaration";
            case NodeType::FieldDeclaration: return "FieldDeclaration";
            case NodeType::ClassDeclaration: return "ClassDeclaration";
            case NodeType::LocalConstantDeclaration: return "LocalConstantDeclaration";
            case NodeType::StructDeclaration: return "StructDeclaration";
            case NodeType::EnumMemberDeclaration: return "EnumMemberDeclaration";
            case NodeType::EnumDeclaration: return "EnumDeclaration";
            case NodeType::ConstantDeclaration: return "ConstantDeclaration";
            case NodeType::ConstructorDeclaration: return "ConstructorDeclaration";
            case NodeType::DelegateDeclaration: return "DelegateDeclaration";
            case NodeType::IndexerDeclaration: return "IndexerDeclaration";
            case NodeType::NamespaceDeclaration: return "NamespaceDeclaration";
            case NodeType::InterfaceDeclaration: return "InterfaceDeclaration";
            case NodeType::TypeArgumentList: return "TypeArgumentList";
            case NodeType::FormalParameterList: return "FormalParameterList";
            case NodeType::FormalParameter: return "FormalParameter";
            case NodeType::TypeConstraintList: return "TypeConstraintList";
            case NodeType::TypeConstraint: return "TypeConstraint";
            case NodeType::TypePath: return "TypePath";
            case NodeType::MemberAccess: return "MemberAccess";
            case NodeType::IncrementDecrement: return "IncrementDecrement";
            case NodeType::Argument: return "Argument";
            case NodeType::ArgumentList: return "ArgumentList";
            case NodeType::MethodInvocation: return "MethodInvocation";
            case NodeType::Identifier: return "Identifier";
            case NodeType::BuiltInTypeAccess: return "BuiltInTypeAccess";
            case NodeType::NullableDereference: return "NullableDereference";
            case NodeType::SwitchExpressionArm: return "SwitchExpressionArm";
            case NodeType::ExpressionList: return "ExpressionList";
            case NodeType::ContextList: return "ContextList";
            case NodeType::SwitchSection: return "SwitchSection";
            case NodeType::SwitchLabel: return "SwitchLabel";
            case NodeType::CatchClause: return "CatchClause";
            case NodeType::MemberInitializer: return "MemberInitializer";
            case NodeType::IndexedInitializer: return "IndexedInitializer";
            case NodeType::CollectionInitializer: return "CollectionInitializer";
            case NodeType::ListInitializer: return "ListInitializer";
            case NodeType::DynamicKeyValue: return "DynamicKeyValue";
            case NodeType::DynamicKey: return "DynamicKey";
            case NodeType::DynamicObjectLiteral: return "DynamicObjectLiteral";
            case NodeType::DynamicArrayLiteral: return "DynamicArrayLiteral";
            case NodeType::StringPart: return "StringPart";
            case NodeType::TypeParameterListNode: return "TypeParameterListNode";
            case NodeType::ClassBody: return "ClassBody";
            case NodeType::PropertyGetter: return "PropertyGetter";
            case NodeType::PropertySetter: return "PropertySetter";
            case NodeType::LambdaParameter: return "LambdaParameter";
            case NodeType::InitializerList: return "InitializerList";
            case NodeType::ForLoopIterator: return "ForLoopIterator";
            case NodeType::ScopeMemberAccess: return "ScopeMemberAccess";
            case NodeType::ConditionalMemberAccess: return "ConditionalMemberAccess";
            case NodeType::ModifierList: return "ModifierList";
            case NodeType::ForLoopInitializer: return "ForLoopInitializer";
            case NodeType::ForInitializerList: return "ForInitializerList";
            case NodeType::IndexerParameterList: return "IndexerParameterList";
            case NodeType::TypeList: return "TypeList";
            case NodeType::NamespacePath: return "NamespacePath";
            case NodeType::EnumMemberList: return "EnumMemberList";
            case NodeType::NamespaceOrTypeName: return "NamespaceOrTypeName";
            case NodeType::LocalVariableType: return "LocalVariableType";
            case NodeType::TypeCast: return "TypeCast";
            case NodeType::Literal: return "Literal";
            case NodeType::TernaryExpression: return "TernaryExpression";
            case NodeType::ThrowExpression: return "ThrowExpression";
            case NodeType::NullCoalescingExpression: return "NullCoalescingExpression";
            case NodeType::BinaryExpression: return "BinaryExpression";
            case NodeType::UnaryExpression: return "UnaryExpression";
            case NodeType::BracketExpression: return "BracketExpression";
            case NodeType::PrimaryExpression: return "PrimaryExpression";
            case NodeType::PrimaryExpressionTail: return "PrimaryExpressionTail";
            case NodeType::ThisReferenceExpression: return "ThisReferenceExpression";
            case NodeType::BaseReferenceExpression: return "BaseReferenceExpression";
            case NodeType::ParenExpression: return "ParenExpression";
            case NodeType::TypeOfExpression: return "TypeOfExpression";
            case NodeType::AssignmentExpression: return "AssignmentExpression";
            case NodeType::IsExpression: return "IsExpression";
            case NodeType::AsExpression: return "AsExpression";
            case NodeType::SwitchExpression: return "SwitchExpression";
            case NodeType::TryExpression: return "TryExpression";
            case NodeType::NewExpression: return "NewExpression";
            case NodeType::LambdaExpression: return "LambdaExpression";
            case NodeType::RefExpression: return "RefExpression";
            case NodeType::NewDynamicObjectLiteralExpression: return "NewDynamicObjectLiteralExpression";
            case NodeType::NewDynamicArrayLiteralExpression: return "NewDynamicArrayLiteralExpression";
            case NodeType::BlockStatement: return "BlockStatement";
            case NodeType::TryBlock: return "TryBlock";
            case NodeType::EmptyStatement: return "EmptyStatement";
            case NodeType::BreakStatement: return "BreakStatement";
            case NodeType::ContinueStatement: return "ContinueStatement";
            case NodeType::ReturnStatement: return "ReturnStatement";
            case NodeType::UsingStatementBlock: return "UsingStatementBlock";
            case NodeType::IfStatement: return "IfStatement";
            case NodeType::WhileStatement: return "WhileStatement";
            case NodeType::DoWhileStatement: return "DoWhileStatement";
            case NodeType::ForLoopStatement: return "ForLoopStatement";
            case NodeType::ForEachLoopStatement: return "ForEachLoopStatement";
            case NodeType::SwitchStatement: return "SwitchStatement";
            case NodeType::ThrowStatement: return "ThrowStatement";
            case NodeType::ExpressionStatement: return "ExpressionStatement";
            case NodeType::TrailingLambdaStatement: return "TrailingLambdaStatement";
            case NodeType::WithStatement: return "WithStatement";
            case NodeType::UsingStatementScope: return "UsingStatementScope";
            case NodeType::PrimaryMethodInvocation: return "PrimaryMethodInvocation";
            default: return "";
        }
    }

// --- Generate NodeTypeToString End

}