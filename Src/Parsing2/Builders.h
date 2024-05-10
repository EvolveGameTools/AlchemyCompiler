#pragma once

#include "./SyntaxBase.h"
#include "./SyntaxNodes.h"
#include "SyntaxFacts.h"

namespace Alchemy::Compilation {

    struct SyntaxBaseBuilder {

        LinearAllocator * allocator;

        virtual SyntaxBase* Build() = 0;

    };

    struct ExpressionSyntaxBuilder : SyntaxBaseBuilder {

    };

    struct TypeSyntaxBuilder : ExpressionSyntaxBuilder {

    };

    struct NameSyntaxBuilder : TypeSyntaxBuilder {

    };

    struct SimpleNameSyntaxBuilder : NameSyntaxBuilder {

    };

    struct SyntaxListBuilder {

        LinearAllocator* allocator;

        SyntaxListUntyped* Build() {
            SyntaxListUntyped* retn = allocator->Allocate<SyntaxListUntyped>(1);
            return retn;
        }

    };

    struct SeparatedSyntaxListBuilder {
        LinearAllocator* allocator;
        SeparatedSyntaxListUntyped* Build() {
            SeparatedSyntaxListUntyped* retn = allocator->Allocate<SeparatedSyntaxListUntyped>(1);
            return retn;
        }
    };

    struct BuilderBase {

        LinearAllocator * allocator;

        explicit BuilderBase(LinearAllocator * allocator) : allocator(allocator) {}

        struct SyntaxToken MakeIdentifier(const char * identifier, SyntaxTokenFlags flags = SyntaxTokenFlags:: None) {
            struct SyntaxToken retn;
            retn.kind = SyntaxKind::IdentifierToken;
            retn.contextualKind = SyntaxKind::IdentifierToken;
            retn.SetFlags(flags);
            retn.text = (char*)identifier;
            retn.textSize = (uint16) strlen(identifier);
            SyntaxKind keyword;

            if(TryMatchKeyword_Generated((char*)identifier, (int32)strlen(identifier), & keyword)) {
                if (SyntaxFacts::IsReservedKeyword(keyword)) {
                    retn.kind = keyword;
                    retn.contextualKind = SyntaxKind::None;
                }
                else {
                    // it's contextual
                    assert(SyntaxFacts::IsContextualKeyword(keyword));
                    retn.kind = SyntaxKind::IdentifierToken;
                    retn.contextualKind = keyword;

                }

            }

            return retn;
        }

        struct SyntaxToken SyntaxToken(SyntaxKind kind, SyntaxTokenFlags flags = SyntaxTokenFlags::None) {
            struct SyntaxToken retn;
            retn.kind = kind;
            retn.SetFlags(flags);

            assert(kind != SyntaxKind::IdentifierToken && "use MakeIdentifier instead");
            retn.text = (char*)SyntaxFacts::GetText(kind);
            retn.textSize = (uint16) strlen(retn.text);
            return retn;
        }

        struct SyntaxToken SyntaxToken(SyntaxKind kind, SyntaxKind contextualKind, SyntaxTokenFlags flags = SyntaxTokenFlags::None) {
            struct SyntaxToken retn;
            retn.kind = kind;
            retn.contextualKind = contextualKind;
            assert(kind != SyntaxKind::IdentifierToken && "use MakeIdentifier instead");
            retn.SetFlags(flags);

            retn.text = (char*)SyntaxFacts::GetText(contextualKind);
            retn.textSize = (uint16) strlen(retn.text);

            return retn;
        }

    };

}