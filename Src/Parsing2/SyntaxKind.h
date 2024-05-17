#pragma once

#include "../PrimitiveTypes.h"

namespace Alchemy::Compilation {

    enum class TokenKind : uint8 {
        None,
        // --- Literal Types ---
        __FirstLiteral__,
        BoolLiteral,
        CharLiteral,
        FloatLiteral,
        DoubleLiteral,
        Int32Literal,
        Int64Literal,
        UInt32Literal,
        UInt64Literal,
        __LastLiteral__,

        // --- Reserved Keyword
        __FirstKeyword__,
        FixedKeyword,
        // todo -- int32 etc
        BoolKeyword,
        ByteKeyword,
        SByteKeyword,
        ShortKeyword,
        UShortKeyword,
        IntKeyword,
        UIntKeyword,
        LongKeyword,
        ULongKeyword,
        DoubleKeyword,
        FloatKeyword,
        StringKeyword,
        CharKeyword,
        VoidKeyword,
        ObjectKeyword,
        TypeOfKeyword,
        SizeOfKeyword,
        NullKeyword,
        TrueKeyword,
        FalseKeyword,
        IfKeyword,
        ElseKeyword,
        WhileKeyword,
        ForKeyword,
        ForEachKeyword,
        DoKeyword,
        SwitchKeyword,
        CaseKeyword,
        DefaultKeyword,
        TryKeyword,
        CatchKeyword,
        FinallyKeyword,
        BreakKeyword,
        ContinueKeyword,
        ReturnKeyword,
        ThrowKeyword,
        PublicKeyword,
        PrivateKeyword,
        InternalKeyword,
        ProtectedKeyword,
        StaticKeyword,
        ReadOnlyKeyword,
        SealedKeyword,
        ConstKeyword,
        NewKeyword,
        OverrideKeyword,
        AbstractKeyword,
        VirtualKeyword,
        ExternKeyword,
        RefKeyword,
        OutKeyword,
        InKeyword,
        IsKeyword,
        AsKeyword,
        ParamsKeyword,
        ThisKeyword,
        BaseKeyword,
        NamespaceKeyword,
        UsingKeyword,
        ClassKeyword,
        StructKeyword,
        InterfaceKeyword,
        EnumKeyword,
        DelegateKeyword,
        OperatorKeyword,
        ExplicitKeyword,
        NameofKeyword,
        ImplicitKeyword,
        VarKeyword,
        StackAllocKeyword,
        ElifKeyword,
        EndIfKeyword,
        NameOfKeyword,
        ConstructorKeyword,
        GotoKeyword, // reserved but not used atm
        LockKeyword, // reserved but not used atm

        __LastKeyword__,
        // --- End Reserved Keywords

        // --- Contextual Keywords
        __FirstContextualKeyword__,
        PartialKeyword,
        AliasKeyword,
//        GlobalKeyword,
        GetKeyword,
        SetKeyword,
        WhereKeyword,
        FromKeyword,
        WhenKeyword,
        OrKeyword,
        AndKeyword,
        NotKeyword,
        WithKeyword,
        InitKeyword,
        RequiredKeyword,
        __LastContextualKeyword__,
        // --- End Contextual Keywords

        // --- Tokens
        __FirstToken__,
        IdentifierToken,
        SlashEqualsToken,
        SlashToken,
        DotToken,
        DotDotToken,
        DotDotDotToken,
        CommaToken,
        ColonColonToken,
        ColonToken,
        SemicolonToken,
        TildeToken,
        ExclamationToken,
        ExclamationEqualsToken,
        EqualsEqualsToken,
        EqualsGreaterThanToken,
        EqualsToken,
        AsteriskEqualsToken,
        AsteriskToken,
        OpenParenToken,
        CloseParenToken,
        OpenBraceToken,
        CloseBraceToken,
        OpenBracketToken,
        CloseBracketToken,
        QuestionQuestionEqualsToken,
        QuestionQuestionToken,
        QuestionToken,
        PlusEqualsToken,
        PlusPlusToken,
        PlusToken,
        MinusEqualsToken,
        MinusMinusToken,
        MinusGreaterThanToken,
        MinusToken,
        PercentToken,
        PercentEqualsToken,
        AmpersandEqualsToken,
        AmpersandAmpersandToken,
        AmpersandToken,
        CaretEqualsToken,
        CaretToken,
        BarEqualsToken,
        BarBarToken,
        BarToken,
        LessThanEqualsToken,
        LessThanLessThanEqualsToken,
        LessThanLessThanToken,
        LessThanToken,
        GreaterThanToken,
        GreaterThanEqualsToken,
        NumericLiteralToken,
        StringLiteralToken,
        EndOfFileToken,
        CharacterLiteralToken,
        GreaterThanGreaterThanToken,
        GreaterThanGreaterThanGreaterThanToken,
        GreaterThanGreaterThanEqualsToken,
        GreaterThanGreaterThanGreaterThanEqualsToken,
        UnderscoreToken,
        DollarToken,
        BackslashToken,
        DoubleQuoteToken,
        SingleQuoteToken,
        HashToken,
        OmittedArraySizeExpressionToken,
        __LastToken__,
        // --- End Tokens


        Trivia,
        MultiLineComment,
        SingleLineComment,
        Whitespace,
        NewLine,

    };

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
        AddressOfExpression,
        PointerIndirectionExpression,
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
        OmittedArraySizeExpression,
        RefExpression,
        SwitchExpression,

        // -- End Expressions

        IdentifierName,
        QualifiedName,
        GenericName,
        ArrayRankSpecifier,
        TupleElement,
        MemberDeclaration,
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
        ArrayType,
        PredefinedType,
        __TYPE_END__,

        NamespaceDeclaration,
        EnumDeclaration,
        StructDeclaration,
        ClassDeclaration,
        InterfaceDeclaration,
        DelegateDeclaration,
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
    };

    const char* SyntaxKindToString(SyntaxKind kind);
    const char* TokenKindToString(TokenKind kind);

}