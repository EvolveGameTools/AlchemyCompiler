#pragma once

#include "./SyntaxKind.h"
#include "./Scanning.h"

namespace Alchemy::Compilation {

    DEFINE_ENUM_FLAGS(NodeEqualityOptions, uint8, {
        None = 0,
        IgnoreTrivia = 1 << 0,
        Default = IgnoreTrivia,
    })

    struct SyntaxBase {

        // todo -- merge start & kind & end & flags into 2 ints
        SyntaxKind _kind;
        SyntaxTokenFlags flags;
        int32 startTokenId;
        int32 endTokenId;

        explicit SyntaxBase(SyntaxKind kind)
            : _kind(kind)
            , flags(SyntaxTokenFlags::None)
            , startTokenId(0)
            , endTokenId(0) {}

        inline int32 GetStartTokenId() {
            return startTokenId;
        }

        inline int32 GetEndTokenId() {
            return endTokenId;
        }

        inline SyntaxKind GetKind() {
            return _kind;
        }

        inline bool IsMissing() {
            return false; // (flags & SyntaxTokenFlags::IsNotMissing) == 0;
        }

//        #if true // shows better debugger output if polymorphic
        virtual void dummy() {}
//        #endif

    };


    struct SyntaxListUntyped : SyntaxBase {
        // DO NOT CHANGE THIS LAYOUT WITHOUT MIRRORING IN SyntaxList<T>
        int32 size;
        SyntaxBase** array;

        SyntaxListUntyped(SyntaxBase** array, int32 size) : SyntaxBase(SyntaxKind::ListKind), array(array), size(size) {}

    };

    template<typename T>
    struct SyntaxList : SyntaxBase {
        // DO NOT CHANGE THIS LAYOUT WITHOUT MIRRORING IN SyntaxListUntyped
        int32 size;
        T** array;

        SyntaxList(T** array, int32 size) : SyntaxBase(SyntaxKind::ListKind), array(array), size(size) {}
    };

    struct SeparatedSyntaxListUntyped : SyntaxBase {
        // DO NOT CHANGE THIS LAYOUT WITHOUT MIRRORING IN SeparatedSyntaxList<T>
        int32 itemCount {};
        int32 separatorCount {};
        SyntaxBase** items {};
        SyntaxToken* separators {};

        explicit SeparatedSyntaxListUntyped() : SyntaxBase(SyntaxKind::ListKind) {}

    };

    template<typename T>
    struct SeparatedSyntaxList : SyntaxBase {
        // DO NOT CHANGE THIS LAYOUT WITHOUT MIRRORING IN SeparatedSyntaxListUntyped
        int32 itemCount;
        int32 separatorCount;
        T** items;
        SyntaxToken* separators;

        SeparatedSyntaxList(int32 itemCount, T** items, int32 separatorCount, SyntaxToken* separators)
            : SyntaxBase(SyntaxKind::ListKind)
            , itemCount(itemCount)
            , items(items)
            , separatorCount(separatorCount)
            , separators(separators) {}

        SeparatedSyntaxListUntyped* ToUntypedList() {
            return (SeparatedSyntaxListUntyped*) this;
        }

    };


    struct TokenList {
        SyntaxToken* array;
        int32 size;

        TokenList(SyntaxToken* array, int32 size)
            : array(array)
            , size(size) {}

        SyntaxToken& operator[](int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

    };

    struct VariableDesignationSyntax : SyntaxBase {

        explicit VariableDesignationSyntax(SyntaxKind kind) : SyntaxBase(kind) {}

    };

    struct ExpressionSyntax : SyntaxBase {

        explicit ExpressionSyntax(SyntaxKind kind) : SyntaxBase(kind) {}

    };

    struct TypeSyntax : ExpressionSyntax {

        explicit TypeSyntax(SyntaxKind kind) : ExpressionSyntax(kind) {}

    };

    struct NameSyntax : TypeSyntax {

        explicit NameSyntax(SyntaxKind kind) : TypeSyntax(kind) {}

    };

    struct SimpleNameSyntax : NameSyntax {

        explicit SimpleNameSyntax(SyntaxKind kind) : NameSyntax(kind) {}

    };

    struct BaseExpressionColonSyntax : SyntaxBase {
        explicit BaseExpressionColonSyntax(SyntaxKind kind)
            : SyntaxBase(kind) {}
    };

    struct PatternSyntax : SyntaxBase {
        explicit PatternSyntax(SyntaxKind kind)
            : SyntaxBase(kind) {}
    };

    struct MemberDeclarationSyntax : SyntaxBase {

        explicit MemberDeclarationSyntax(SyntaxKind kind)
            : SyntaxBase(kind) {}
    };

    struct ConstructorInitializerSyntax : SyntaxBase {

        explicit ConstructorInitializerSyntax(SyntaxKind kind)
            : SyntaxBase(kind) {}

    };

    struct StatementSyntax : SyntaxBase {
        explicit StatementSyntax(SyntaxKind kind)
            : SyntaxBase(kind) {}
    };

    #define VALID_SYNTAX_KINDS static constexpr SyntaxKind kValidSyntaxKinds[]
    #define ASSERT_VALID_SYNTAX_KIND(k)  assert(IsValidSyntaxKind(k, (SyntaxKind*)kValidSyntaxKinds, sizeof(kValidSyntaxKinds) / sizeof(kValidSyntaxKinds[0])))

    inline bool IsValidSyntaxKind(SyntaxKind kind, SyntaxKind* items, int32 size) {
        for (int32 i = 0; i < size; i++) {
            if (kind == items[i]) {
                return true;
            }
        }
        return false;
    }
}