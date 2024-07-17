#pragma once
#include "../PrimitiveTypes.h"

namespace Alchemy::Parsing {

    struct AstNodeIndex {

        uint16 index { 0 };

        AstNodeIndex() = default;

        explicit AstNodeIndex(uint16 index) : index(index) {}

        bool IsValid() const {
            return index > 0;
        }
    };

    struct AstNode {

        uint16 psiIndex {}; // will be 0 for leaf nodes probably
        uint16 depth {};
        uint16 firstChild {};
        uint16 lastChild {};
        uint16 parentIndex {};
        uint16 nextIndex {};
        uint32 cstIndex {};

    };

}