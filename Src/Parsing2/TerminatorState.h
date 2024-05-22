#pragma once

namespace Alchemy::Compilation {

    DEFINE_ENUM_FLAGS(TerminatorState, uint32, {
        EndOfFile = 0,
        IsNamespaceMemberStartOrStop = 1 << 0,
        IsAttributeDeclarationTerminator = 1 << 1,
        IsPossibleAggregateClauseStartOrStop = 1 << 2,
        IsPossibleMemberStartOrStop = 1 << 3,
        IsEndOfReturnType = 1 << 4,
        IsEndOfParameterList = 1 << 5,
        IsEndOfFieldDeclaration = 1 << 6,
        IsPossibleEndOfVariableDeclaration = 1 << 7,
        IsEndOfTypeArgumentList = 1 << 8,
        IsPossibleStatementStartOrStop = 1 << 9,
        IsEndOfFixedStatement = 1 << 10,
        IsEndOfTryBlock = 1 << 11,
        IsEndOfCatchClause = 1 << 12,
        IsEndOfFilterClause = 1 << 13,
        IsEndOfCatchBlock = 1 << 14,
        IsEndOfDoWhileExpression = 1 << 15,
        IsEndOfForStatementArgument = 1 << 16,
        IsEndOfDeclarationClause = 1 << 17,
        IsEndOfArgumentList = 1 << 18,
        IsSwitchSectionStart = 1 << 19,
        IsEndOfTypeParameterList = 1 << 20,
        IsEndOfMethodSignature = 1 << 21,
        IsEndOfNameInExplicitInterface = 1 << 22,
        IsEndOfClassOrStructOrInterfaceSignature = 1 << 23,
        IsExpressionOrPatternInCaseLabelOfSwitchStatement = 1 << 24,
        IsPatternInSwitchExpressionArm = 1 << 25,
        __LAST__ = 1 << 26
    })
}