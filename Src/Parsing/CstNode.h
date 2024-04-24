#pragma once

#include "../PrimitiveTypes.h"

namespace Alchemy {

    struct CstNodeIndex {

        uint16 index {0};

        CstNodeIndex() = default;

        explicit CstNodeIndex(uint16 index) : index(index) {}

    };

    // Concrete syntax tree nodes
    struct CstNode {

        CstNodeIndex parentIndex;
        CstNodeIndex nextSiblingIndex;
        CstNodeIndex prevSiblingIndex;
        CstNodeIndex firstChildIndex;
        CstNodeIndex lastChildIndex;
        uint16 depth;
        int32 psiIndexOrTokenIndex {}; // this will be negative if leaf, positive if psi

        int32 GetPsiIndex() {
            return psiIndexOrTokenIndex < 0 ? 0 : psiIndexOrTokenIndex;
        }

        int32 GetTokenIndex() {
            return psiIndexOrTokenIndex > 0 ? 0 : -psiIndexOrTokenIndex;
        }

        bool IsPsiNode() const {
            return psiIndexOrTokenIndex > 0;
        }

        bool IsTokenNode() const {
            return psiIndexOrTokenIndex < 0;
        }

    };

}