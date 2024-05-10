#pragma once

#include "./SyntaxBase.h"
#include "./SyntaxNodes.h"

namespace Alchemy::Compilation {

    bool NodesEqual(SyntaxBase* a, SyntaxBase* b, NodeEqualityOptions options);

    inline bool TokensEqual(SyntaxToken a, SyntaxToken b, NodeEqualityOptions options) {
        if (a.kind != b.kind) {
            return false;
        }

        if (a.contextualKind != b.contextualKind) {
            return false;
        }

        SyntaxTokenFlags a1 = a.flags;
        SyntaxTokenFlags b1 = b.flags;

        if ((options & NodeEqualityOptions::IgnoreTrivia) != 0) {
            a1 = a1 & ~(SyntaxTokenFlags::LeadingTrivia | SyntaxTokenFlags::TrailingTrivia);
            b1 = b1 & ~(SyntaxTokenFlags::LeadingTrivia | SyntaxTokenFlags::TrailingTrivia);
        }

        if (a1 != b1) {
            return false;
        }

        if (a.literalType != b.literalType) {
            return false;
        }

        #if ALCHEMY_DEBUG != 0
        if (a.text != b.text) {
            return false;
        }
        #endif

        return true;
    }

    inline bool SeparatedSyntaxListEqual(SeparatedSyntaxListUntyped* a, SeparatedSyntaxListUntyped* b, NodeEqualityOptions options) {
        if (a == nullptr && b == nullptr) {
            return true;
        }

        if (a == nullptr || b == nullptr) {
            return false;
        }
        if (a->itemCount != b->itemCount) {
            return false;
        }
        if (a->separatorCount != b->separatorCount) {
            return false;
        }

        for (int32 i = 0; i < a->separatorCount; i++) {
            if (!TokensEqual(a->separators[i], b->separators[i], options)) {
                return false;
            }
        }

        for (int32 i = 0; i < a->itemCount; i++) {
            if (!NodesEqual(a->items[i], b->items[i], options)) {
                return false;
            }
        }

        return true;
    }

    inline bool SyntaxListEqual(SyntaxListUntyped* a, SyntaxListUntyped* b, NodeEqualityOptions options) {
        if (a == nullptr && b == nullptr) {
            return true;
        }

        if (a == nullptr || b == nullptr) {
            return false;
        }

        if (a->size != b->size) {
            return false;
        }

        for (int32 i = 0; i < a->size; i++) {
            if (!NodesEqual(a->array[i], b->array[i], options)) {
                return false;
            }
        }

        return true;
    }

}