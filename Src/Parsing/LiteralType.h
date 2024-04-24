#pragma once

namespace Alchemy::Parsing {

    enum class LiteralType : uint8 {

        None = 0,
        UInt64,
        UInt32,
        Int32,
        Int64,
        Float,
        Double,
        Bool,
        Null,
        Default,
        String

    };

}