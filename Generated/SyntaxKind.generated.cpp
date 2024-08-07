#include "../Src/Parsing3/SyntaxKind.h"
namespace Alchemy::Compilation {
    const char* SyntaxKindToString(SyntaxKind e) {
        switch(e) {
            case SyntaxKind::None: return "None";
            case SyntaxKind::ListKind: return "ListKind";
            case SyntaxKind::UnaryPlusExpression: return "UnaryPlusExpression";
            case SyntaxKind::UnaryMinusExpression: return "UnaryMinusExpression";
            case SyntaxKind::BitwiseNotExpression: return "BitwiseNotExpression";
            case SyntaxKind::LogicalNotExpression: return "LogicalNotExpression";
            case SyntaxKind::PreIncrementExpression: return "PreIncrementExpression";
            case SyntaxKind::PreDecrementExpression: return "PreDecrementExpression";
            case SyntaxKind::IndexExpression: return "IndexExpression";
            case SyntaxKind::PostIncrementExpression: return "PostIncrementExpression";
            case SyntaxKind::PostDecrementExpression: return "PostDecrementExpression";
            case SyntaxKind::CoalesceExpression: return "CoalesceExpression";
            case SyntaxKind::IsExpression: return "IsExpression";
            case SyntaxKind::AsExpression: return "AsExpression";
            case SyntaxKind::BitwiseOrExpression: return "BitwiseOrExpression";
            case SyntaxKind::ExclusiveOrExpression: return "ExclusiveOrExpression";
            case SyntaxKind::BitwiseAndExpression: return "BitwiseAndExpression";
            case SyntaxKind::EqualsExpression: return "EqualsExpression";
            case SyntaxKind::NotEqualsExpression: return "NotEqualsExpression";
            case SyntaxKind::LessThanExpression: return "LessThanExpression";
            case SyntaxKind::LessThanOrEqualExpression: return "LessThanOrEqualExpression";
            case SyntaxKind::GreaterThanExpression: return "GreaterThanExpression";
            case SyntaxKind::GreaterThanOrEqualExpression: return "GreaterThanOrEqualExpression";
            case SyntaxKind::LeftShiftExpression: return "LeftShiftExpression";
            case SyntaxKind::RightShiftExpression: return "RightShiftExpression";
            case SyntaxKind::UnsignedRightShiftExpression: return "UnsignedRightShiftExpression";
            case SyntaxKind::AddExpression: return "AddExpression";
            case SyntaxKind::SubtractExpression: return "SubtractExpression";
            case SyntaxKind::MultiplyExpression: return "MultiplyExpression";
            case SyntaxKind::DivideExpression: return "DivideExpression";
            case SyntaxKind::ModuloExpression: return "ModuloExpression";
            case SyntaxKind::LogicalAndExpression: return "LogicalAndExpression";
            case SyntaxKind::LogicalOrExpression: return "LogicalOrExpression";
            case SyntaxKind::RefExpression: return "RefExpression";
            case SyntaxKind::SwitchExpression: return "SwitchExpression";
            case SyntaxKind::IdentifierName: return "IdentifierName";
            case SyntaxKind::QualifiedName: return "QualifiedName";
            case SyntaxKind::GenericName: return "GenericName";
            case SyntaxKind::ArrayRankSpecifier: return "ArrayRankSpecifier";
            case SyntaxKind::TupleElement: return "TupleElement";
            case SyntaxKind::MemberDeclaration: return "MemberDeclaration";
            case SyntaxKind::GetAccessorDeclaration: return "GetAccessorDeclaration";
            case SyntaxKind::SetAccessorDeclaration: return "SetAccessorDeclaration";
            case SyntaxKind::InitAccessorDeclaration: return "InitAccessorDeclaration";
            case SyntaxKind::TypeArgumentList: return "TypeArgumentList";
            case SyntaxKind::VariableDeclarator: return "VariableDeclarator";
            case SyntaxKind::__PATTERN_START__: return "__PATTERN_START__";
            case SyntaxKind::PatternSyntax: return "PatternSyntax";
            case SyntaxKind::DeclarationPattern: return "DeclarationPattern";
            case SyntaxKind::ConstantPattern: return "ConstantPattern";
            case SyntaxKind::CasePatternSwitchLabel: return "CasePatternSwitchLabel";
            case SyntaxKind::WhenClause: return "WhenClause";
            case SyntaxKind::DiscardDesignation: return "DiscardDesignation";
            case SyntaxKind::RecursivePattern: return "RecursivePattern";
            case SyntaxKind::PropertyPatternClause: return "PropertyPatternClause";
            case SyntaxKind::Subpattern: return "Subpattern";
            case SyntaxKind::PositionalPatternClause: return "PositionalPatternClause";
            case SyntaxKind::DiscardPattern: return "DiscardPattern";
            case SyntaxKind::SwitchExpressionArm: return "SwitchExpressionArm";
            case SyntaxKind::VarPattern: return "VarPattern";
            case SyntaxKind::ParenthesizedPattern: return "ParenthesizedPattern";
            case SyntaxKind::RelationalPattern: return "RelationalPattern";
            case SyntaxKind::TypePattern: return "TypePattern";
            case SyntaxKind::OrPattern: return "OrPattern";
            case SyntaxKind::AndPattern: return "AndPattern";
            case SyntaxKind::NotPattern: return "NotPattern";
            case SyntaxKind::SlicePattern: return "SlicePattern";
            case SyntaxKind::ListPattern: return "ListPattern";
            case SyntaxKind::__PATTERN_END__: return "__PATTERN_END__";
            case SyntaxKind::__TYPE_START__: return "__TYPE_START__";
            case SyntaxKind::TupleType: return "TupleType";
            case SyntaxKind::NullableType: return "NullableType";
            case SyntaxKind::RefType: return "RefType";
            case SyntaxKind::PredefinedType: return "PredefinedType";
            case SyntaxKind::__TYPE_END__: return "__TYPE_END__";
            case SyntaxKind::NamespaceDeclaration: return "NamespaceDeclaration";
            case SyntaxKind::EnumDeclaration: return "EnumDeclaration";
            case SyntaxKind::StructDeclaration: return "StructDeclaration";
            case SyntaxKind::ClassDeclaration: return "ClassDeclaration";
            case SyntaxKind::InterfaceDeclaration: return "InterfaceDeclaration";
            case SyntaxKind::DelegateDeclaration: return "DelegateDeclaration";
            case SyntaxKind::WidgetDeclaration: return "WidgetDeclaration";
            case SyntaxKind::GlobalStatement: return "GlobalStatement";
            case SyntaxKind::IncompleteMember: return "IncompleteMember";
            case SyntaxKind::CompilationUnit: return "CompilationUnit";
            case SyntaxKind::LocalFunctionStatement: return "LocalFunctionStatement";
            case SyntaxKind::TypeParameterList: return "TypeParameterList";
            case SyntaxKind::ParameterList: return "ParameterList";
            case SyntaxKind::EqualsValueClause: return "EqualsValueClause";
            case SyntaxKind::Argument: return "Argument";
            case SyntaxKind::BracketedArgumentList: return "BracketedArgumentList";
            case SyntaxKind::VariableDeclaration: return "VariableDeclaration";
            case SyntaxKind::FieldDeclaration: return "FieldDeclaration";
            case SyntaxKind::TypeParameter: return "TypeParameter";
            case SyntaxKind::Parameter: return "Parameter";
            case SyntaxKind::ConstraintClauses: return "ConstraintClauses";
            case SyntaxKind::ArgumentList: return "ArgumentList";
            case SyntaxKind::ConstructorInitializer: return "ConstructorInitializer";
            case SyntaxKind::ThisConstructorInitializer: return "ThisConstructorInitializer";
            case SyntaxKind::BaseConstructorInitializer: return "BaseConstructorInitializer";
            case SyntaxKind::NamedConstructorInitializer: return "NamedConstructorInitializer";
            case SyntaxKind::NameColon: return "NameColon";
            case SyntaxKind::Block: return "Block";
            case SyntaxKind::ArrowExpressionClause: return "ArrowExpressionClause";
            case SyntaxKind::ParenthesizedLambdaExpression: return "ParenthesizedLambdaExpression";
            case SyntaxKind::SimpleLambdaExpression: return "SimpleLambdaExpression";
            case SyntaxKind::AnonymousMethodExpression: return "AnonymousMethodExpression";
            case SyntaxKind::SimpleAssignmentExpression: return "SimpleAssignmentExpression";
            case SyntaxKind::AddAssignmentExpression: return "AddAssignmentExpression";
            case SyntaxKind::SubtractAssignmentExpression: return "SubtractAssignmentExpression";
            case SyntaxKind::MultiplyAssignmentExpression: return "MultiplyAssignmentExpression";
            case SyntaxKind::DivideAssignmentExpression: return "DivideAssignmentExpression";
            case SyntaxKind::ModuloAssignmentExpression: return "ModuloAssignmentExpression";
            case SyntaxKind::AndAssignmentExpression: return "AndAssignmentExpression";
            case SyntaxKind::ExclusiveOrAssignmentExpression: return "ExclusiveOrAssignmentExpression";
            case SyntaxKind::OrAssignmentExpression: return "OrAssignmentExpression";
            case SyntaxKind::LeftShiftAssignmentExpression: return "LeftShiftAssignmentExpression";
            case SyntaxKind::RightShiftAssignmentExpression: return "RightShiftAssignmentExpression";
            case SyntaxKind::UnsignedRightShiftAssignmentExpression: return "UnsignedRightShiftAssignmentExpression";
            case SyntaxKind::CoalesceAssignmentExpression: return "CoalesceAssignmentExpression";
            case SyntaxKind::ThrowExpression: return "ThrowExpression";
            case SyntaxKind::IsPatternExpression: return "IsPatternExpression";
            case SyntaxKind::WithExpression: return "WithExpression";
            case SyntaxKind::TypeOfExpression: return "TypeOfExpression";
            case SyntaxKind::SizeOfExpression: return "SizeOfExpression";
            case SyntaxKind::CastExpression: return "CastExpression";
            case SyntaxKind::RangeExpression: return "RangeExpression";
            case SyntaxKind::ConditionalExpression: return "ConditionalExpression";
            case SyntaxKind::AnonymousObjectCreationExpression: return "AnonymousObjectCreationExpression";
            case SyntaxKind::ArrayCreationExpression: return "ArrayCreationExpression";
            case SyntaxKind::BaseExpression: return "BaseExpression";
            case SyntaxKind::CollectionExpression: return "CollectionExpression";
            case SyntaxKind::ConditionalAccessExpression: return "ConditionalAccessExpression";
            case SyntaxKind::DeclarationExpression: return "DeclarationExpression";
            case SyntaxKind::DefaultExpression: return "DefaultExpression";
            case SyntaxKind::CharacterLiteralExpression: return "CharacterLiteralExpression";
            case SyntaxKind::DefaultLiteralExpression: return "DefaultLiteralExpression";
            case SyntaxKind::FalseLiteralExpression: return "FalseLiteralExpression";
            case SyntaxKind::NullLiteralExpression: return "NullLiteralExpression";
            case SyntaxKind::NumericLiteralExpression: return "NumericLiteralExpression";
            case SyntaxKind::StringLiteralExpression: return "StringLiteralExpression";
            case SyntaxKind::TrueLiteralExpression: return "TrueLiteralExpression";
            case SyntaxKind::ElementAccessExpression: return "ElementAccessExpression";
            case SyntaxKind::ImplicitArrayCreationExpression: return "ImplicitArrayCreationExpression";
            case SyntaxKind::ImplicitStackAllocArrayCreationExpression: return "ImplicitStackAllocArrayCreationExpression";
            case SyntaxKind::ImplicitObjectCreationExpression: return "ImplicitObjectCreationExpression";
            case SyntaxKind::InterpolatedStringExpression: return "InterpolatedStringExpression";
            case SyntaxKind::InvocationExpression: return "InvocationExpression";
            case SyntaxKind::ObjectCreationExpression: return "ObjectCreationExpression";
            case SyntaxKind::ParenthesizedExpression: return "ParenthesizedExpression";
            case SyntaxKind::PointerMemberAccessExpression: return "PointerMemberAccessExpression";
            case SyntaxKind::SimpleMemberAccessExpression: return "SimpleMemberAccessExpression";
            case SyntaxKind::StackAllocArrayCreationExpression: return "StackAllocArrayCreationExpression";
            case SyntaxKind::ThisExpression: return "ThisExpression";
            case SyntaxKind::TupleExpression: return "TupleExpression";
            case SyntaxKind::ParenthesizedVariableDesignation: return "ParenthesizedVariableDesignation";
            case SyntaxKind::SingleVariableDesignation: return "SingleVariableDesignation";
            case SyntaxKind::NameEquals: return "NameEquals";
            case SyntaxKind::AnonymousObjectMemberDeclarator: return "AnonymousObjectMemberDeclarator";
            case SyntaxKind::ExpressionColon: return "ExpressionColon";
            case SyntaxKind::SimpleName: return "SimpleName";
            case SyntaxKind::BangExpression: return "BangExpression";
            case SyntaxKind::MemberBindingExpression: return "MemberBindingExpression";
            case SyntaxKind::ElementBindingExpression: return "ElementBindingExpression";
            case SyntaxKind::ArrayInitializerExpression: return "ArrayInitializerExpression";
            case SyntaxKind::ObjectInitializerExpression: return "ObjectInitializerExpression";
            case SyntaxKind::CollectionInitializerExpression: return "CollectionInitializerExpression";
            case SyntaxKind::ComplexElementInitializerExpression: return "ComplexElementInitializerExpression";
            case SyntaxKind::WithInitializerExpression: return "WithInitializerExpression";
            case SyntaxKind::SpreadElement: return "SpreadElement";
            case SyntaxKind::ExpressionElement: return "ExpressionElement";
            case SyntaxKind::ImplicitElementAccess: return "ImplicitElementAccess";
            case SyntaxKind::EmptyStatement: return "EmptyStatement";
            case SyntaxKind::BreakStatement: return "BreakStatement";
            case SyntaxKind::ContinueStatement: return "ContinueStatement";
            case SyntaxKind::DoStatement: return "DoStatement";
            case SyntaxKind::ExpressionStatement: return "ExpressionStatement";
            case SyntaxKind::ForStatement: return "ForStatement";
            case SyntaxKind::ForEachStatement: return "ForEachStatement";
            case SyntaxKind::ForEachVariableStatement: return "ForEachVariableStatement";
            case SyntaxKind::GotoCaseStatement: return "GotoCaseStatement";
            case SyntaxKind::GotoDefaultStatement: return "GotoDefaultStatement";
            case SyntaxKind::GotoStatement: return "GotoStatement";
            case SyntaxKind::ElseClause: return "ElseClause";
            case SyntaxKind::IfStatement: return "IfStatement";
            case SyntaxKind::ReturnStatement: return "ReturnStatement";
            case SyntaxKind::LocalDeclarationStatement: return "LocalDeclarationStatement";
            case SyntaxKind::WhileStatement: return "WhileStatement";
            case SyntaxKind::ThrowStatement: return "ThrowStatement";
            case SyntaxKind::CatchClause: return "CatchClause";
            case SyntaxKind::CatchDeclaration: return "CatchDeclaration";
            case SyntaxKind::CatchFilterClause: return "CatchFilterClause";
            case SyntaxKind::FinallyClause: return "FinallyClause";
            case SyntaxKind::TryStatement: return "TryStatement";
            case SyntaxKind::SwitchStatement: return "SwitchStatement";
            case SyntaxKind::SwitchSection: return "SwitchSection";
            case SyntaxKind::CaseSwitchLabel: return "CaseSwitchLabel";
            case SyntaxKind::DefaultSwitchLabel: return "DefaultSwitchLabel";
            case SyntaxKind::UsingStatement: return "UsingStatement";
            case SyntaxKind::LabeledStatement: return "LabeledStatement";
            case SyntaxKind::BaseList: return "BaseList";
            case SyntaxKind::Attribute: return "Attribute";
            case SyntaxKind::AttributeList: return "AttributeList";
            case SyntaxKind::EnumMemberDeclaration: return "EnumMemberDeclaration";
            case SyntaxKind::TypeConstraint: return "TypeConstraint";
            case SyntaxKind::TypeParameterConstraintClause: return "TypeParameterConstraintClause";
            case SyntaxKind::ConstructorConstraint: return "ConstructorConstraint";
            case SyntaxKind::StructConstraint: return "StructConstraint";
            case SyntaxKind::ClassConstraint: return "ClassConstraint";
            case SyntaxKind::ConstructorDeclaration: return "ConstructorDeclaration";
            case SyntaxKind::EmptyStringLiteralExpression: return "EmptyStringLiteralExpression";
            case SyntaxKind::StringLiteralPart: return "StringLiteralPart";
            case SyntaxKind::InterpolatedIdentifierPart: return "InterpolatedIdentifierPart";
            case SyntaxKind::RawStringLiteralExpression: return "RawStringLiteralExpression";
            case SyntaxKind::BracketedParameterList: return "BracketedParameterList";
            case SyntaxKind::IndexerDeclaration: return "IndexerDeclaration";
            case SyntaxKind::PropertyDeclaration: return "PropertyDeclaration";
            case SyntaxKind::AccessorList: return "AccessorList";
            case SyntaxKind::MethodDeclaration: return "MethodDeclaration";
            case SyntaxKind::UsingDeclaration: return "UsingDeclaration";
            case SyntaxKind::ExternDeclaration: return "ExternDeclaration";
            case SyntaxKind::UsingNamespaceDeclaration: return "UsingNamespaceDeclaration";
            case SyntaxKind::BaseType: return "BaseType";
            default: return "";
        }
    }

}