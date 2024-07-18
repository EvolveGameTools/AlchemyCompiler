#pragma once

#include "../PrimitiveTypes.h"

namespace Alchemy::Compilation {

    enum class SyntaxKind : uint8 {
        None,
        ListKind, // for holding lists, not a real usable type in the language

        // -- Start Expressions
        UnaryPlusExpression,
        UnaryMinusExpression,
        BitwiseNotExpression,
        LogicalNotExpression,
        PreIncrementExpression,
        PreDecrementExpression,
        IndexExpression,
        PostIncrementExpression,
        PostDecrementExpression,
        CoalesceExpression,
        IsExpression,
        AsExpression,
        BitwiseOrExpression,
        ExclusiveOrExpression,
        BitwiseAndExpression,
        EqualsExpression,
        NotEqualsExpression,
        LessThanExpression,
        LessThanOrEqualExpression,
        GreaterThanExpression,
        GreaterThanOrEqualExpression,
        LeftShiftExpression,
        RightShiftExpression,
        UnsignedRightShiftExpression,
        AddExpression,
        SubtractExpression,
        MultiplyExpression,
        DivideExpression,
        ModuloExpression,
        LogicalAndExpression,
        LogicalOrExpression,
        RefExpression,
        SwitchExpression,

        // -- End Expressions

        IdentifierName,
        QualifiedName,
        GenericName,
        ArrayRankSpecifier,
        TupleElement,
        MemberDeclaration,
        GetAccessorDeclaration,
        SetAccessorDeclaration,
        InitAccessorDeclaration,
        TypeArgumentList,
        VariableDeclarator,

        __PATTERN_START__,
        PatternSyntax,
        DeclarationPattern,
        ConstantPattern,
        CasePatternSwitchLabel,
        WhenClause,
        DiscardDesignation,
        RecursivePattern,
        PropertyPatternClause,
        Subpattern,
        PositionalPatternClause,
        DiscardPattern,
        SwitchExpressionArm,
        VarPattern,
        ParenthesizedPattern,
        RelationalPattern,
        TypePattern,
        OrPattern,
        AndPattern,
        NotPattern,
        SlicePattern,
        ListPattern,
        __PATTERN_END__,

        __TYPE_START__,
        TupleType,
        NullableType,
        RefType,
        PredefinedType,
        __TYPE_END__,

        NamespaceDeclaration,
        EnumDeclaration,
        StructDeclaration,
        ClassDeclaration,
        InterfaceDeclaration,
        DelegateDeclaration,
        WidgetDeclaration,
        GlobalStatement,
        IncompleteMember,
        CompilationUnit,

        LocalFunctionStatement,
        TypeParameterList,
        ParameterList,
        EqualsValueClause,
        Argument,
        BracketedArgumentList,
        VariableDeclaration,
        FieldDeclaration,
        TypeParameter,
        Parameter,
        ConstraintClauses,
        ArgumentList,
        ConstructorInitializer,
        ThisConstructorInitializer,
        BaseConstructorInitializer,
        NamedConstructorInitializer,
        NameColon,
        Block,
        ArrowExpressionClause,
        ParenthesizedLambdaExpression,
        SimpleLambdaExpression,
        AnonymousMethodExpression,
        SimpleAssignmentExpression,
        AddAssignmentExpression,
        SubtractAssignmentExpression,
        MultiplyAssignmentExpression,
        DivideAssignmentExpression,
        ModuloAssignmentExpression,
        AndAssignmentExpression,
        ExclusiveOrAssignmentExpression,
        OrAssignmentExpression,
        LeftShiftAssignmentExpression,
        RightShiftAssignmentExpression,
        UnsignedRightShiftAssignmentExpression,
        CoalesceAssignmentExpression,
        ThrowExpression,
        IsPatternExpression,
        WithExpression,
        TypeOfExpression,
        SizeOfExpression,
        CastExpression,
        RangeExpression,
        ConditionalExpression,
        AnonymousObjectCreationExpression,
        ArrayCreationExpression,
        BaseExpression,
        CollectionExpression,
        ConditionalAccessExpression,
        DeclarationExpression,
        DefaultExpression,
        CharacterLiteralExpression,
        DefaultLiteralExpression,
        FalseLiteralExpression,
        NullLiteralExpression,
        NumericLiteralExpression,
        StringLiteralExpression,
        TrueLiteralExpression,
        ElementAccessExpression,
        ImplicitArrayCreationExpression,
        ImplicitStackAllocArrayCreationExpression,
        ImplicitObjectCreationExpression,
        InterpolatedStringExpression,
        InvocationExpression,
        ObjectCreationExpression,
        ParenthesizedExpression,
        PointerMemberAccessExpression,
        SimpleMemberAccessExpression,
        StackAllocArrayCreationExpression,
        ThisExpression,
        TupleExpression,
        ParenthesizedVariableDesignation,
        SingleVariableDesignation,
        NameEquals,
        AnonymousObjectMemberDeclarator,
        ExpressionColon,
        SimpleName,
        BangExpression,
        MemberBindingExpression,
        ElementBindingExpression,

        ArrayInitializerExpression,
        ObjectInitializerExpression,
        CollectionInitializerExpression,
        ComplexElementInitializerExpression,
        WithInitializerExpression,

        SpreadElement,
        ExpressionElement,
        ImplicitElementAccess,
        EmptyStatement,
        BreakStatement,
        ContinueStatement,
        DoStatement,

        ExpressionStatement,
        ForStatement,
        ForEachStatement,
        ForEachVariableStatement,
        GotoCaseStatement,
        GotoDefaultStatement,
        GotoStatement,
        ElseClause,
        IfStatement,
        ReturnStatement,
        LocalDeclarationStatement,
        WhileStatement,
        ThrowStatement,
        CatchClause,
        CatchDeclaration,
        CatchFilterClause,
        FinallyClause,
        TryStatement,
        SwitchStatement,
        SwitchSection,
        CaseSwitchLabel,
        DefaultSwitchLabel,
        UsingStatement,
        LabeledStatement,
        BaseList,
        Attribute,
        AttributeList,
        EnumMemberDeclaration,
        TypeConstraint,
        TypeParameterConstraintClause,
        ConstructorConstraint,
        StructConstraint,
        ClassConstraint,
        ConstructorDeclaration,
        EmptyStringLiteralExpression,
        StringLiteralPart,
        InterpolatedIdentifierPart,
        RawStringLiteralExpression,
        BracketedParameterList,
        IndexerDeclaration,
        PropertyDeclaration,
        AccessorList,
        MethodDeclaration,
        UsingDeclaration,
        ExternDeclaration,

        UsingNamespaceDeclaration,
        BaseType,
    };

    const char* SyntaxKindToString(SyntaxKind kind);

}