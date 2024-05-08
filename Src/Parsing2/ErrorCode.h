#pragma once

namespace Alchemy::Compilation {

    enum class ErrorCode {
        None,
        IllegalEscape,
        ERR_OpenEndedComment,
        ERR_UnexpectedCharacter,
        ERR_InvalidReal,
        ERR_InvalidNumber,
        ERR_IntOverflow,
        ERR_InvalidRealNaN,
        ERR_InvalidRealInfinite,

        ERR_IdentifierExpectedKW,
        ERR_IdentifierExpected,
        ERR_SemicolonExpected,
        ERR_CloseParenExpected,
        ERR_LbraceExpected,
        ERR_RbraceExpected,
        ERR_SyntaxError,
    };

}