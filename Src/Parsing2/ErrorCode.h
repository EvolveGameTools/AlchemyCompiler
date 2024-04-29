#pragma once

namespace Alchemy::Parsing {

    enum class ErrorCode {
        None,
        IllegalEscape,
        ERR_OpenEndedComment,
        ERR_UnexpectedCharacter,
        ERR_InvalidReal,
        ERR_InvalidNumber,
        ERR_IntOverflow
    };

}