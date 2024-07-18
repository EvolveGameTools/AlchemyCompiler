#pragma once

#include "./SyntaxKind.h"
#include "./Scanning.h"
#include "./Tokenizer.h"

namespace Alchemy::Compilation {

    DEFINE_ENUM_FLAGS(NodeEqualityOptions, uint8, {
        None = 0,
        IgnoreTrivia = 1 << 0,
        Default = IgnoreTrivia,
    })

    struct SyntaxBase {

    private:
        uint32_t startId_kind_combined;
        uint32_t endId_flags_combined;

    public:
        explicit SyntaxBase(SyntaxKind kind)
            : startId_kind_combined(0)
            , endId_flags_combined(0) {
            SetKind(kind);
        }

        inline FixedCharSpan GetText(TokenizerResult result) {
            SyntaxToken start = result.tokens[GetStartTokenId()];
            SyntaxToken end = result.tokens[GetEndTokenId()];
            FixedCharSpan startText = start.GetText(result.texts);
            FixedCharSpan endText = end.GetText(result.texts);
            char * pEnd = endText.ptr + endText.size;
            return FixedCharSpan(startText.ptr, pEnd - startText.ptr);
        }

        inline void SetKind(SyntaxKind kind) {
            startId_kind_combined = (startId_kind_combined & 0x00FFFFFF) | (static_cast<int32>(kind) << 24);
        }

        inline SyntaxKind GetKind() {
            return static_cast<SyntaxKind>((startId_kind_combined >> 24) & 0xFF);
        }

        inline void SetFlags(SyntaxTokenFlags flags) {
            endId_flags_combined = (endId_flags_combined & 0x00FFFFFF) | (static_cast<int32>(flags) << 24);
        }

        inline SyntaxTokenFlags GetFlags() {
            return static_cast<SyntaxTokenFlags>((endId_flags_combined >> 24) & 0xFF);
        }

        inline int32 GetEndTokenId() {
            return (int32)(endId_flags_combined & 0x00FFFFFF);
        }

        inline void SetEndTokenId(int32 endId) {
            endId_flags_combined = (endId_flags_combined & 0xFF000000) | (endId & 0x00FFFFFF);
        }

        inline int32 GetStartTokenId() {
            return (int32)(startId_kind_combined & 0x00FFFFFF);
        }

        inline void SetStartTokenId(int32 startId) {
            startId_kind_combined = (startId_kind_combined & 0xFF000000) | (startId & 0x00FFFFFF);
        }

        inline bool IsMissing(CheckedArray<SyntaxToken> tokens) {
            int32 endTokenId = GetEndTokenId();
            for(int32 i = GetStartTokenId(); i < endTokenId; i++) {
                SyntaxToken * pToken = &tokens[i];
                if(pToken->kind == TokenKind::Trivia) {
                    continue;
                }
                if(pToken->IsMissing()) {
                    return true;
                }
            }
            return false;
        }

//        #if true // shows better debugger output if polymorphic
//        virtual void dummy() {}
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

    struct LambdaExpressionSyntax : ExpressionSyntax {

        explicit LambdaExpressionSyntax(SyntaxKind kind) : ExpressionSyntax(kind) {}

    };

    struct NameSyntax : TypeSyntax {

        explicit NameSyntax(SyntaxKind kind) : TypeSyntax(kind) {}

    };

    struct SimpleNameSyntax_Abstract : NameSyntax {

        explicit SimpleNameSyntax_Abstract(SyntaxKind kind) : NameSyntax(kind) {}

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

    struct CollectionElementSyntax : SyntaxBase {

        explicit CollectionElementSyntax(SyntaxKind kind) : SyntaxBase(kind) {}

    };

    struct CommonForEachStatementSyntax : StatementSyntax {

        explicit CommonForEachStatementSyntax(SyntaxKind kind) : StatementSyntax(kind) {}

    };

    struct SwitchLabelSyntax : SyntaxBase {

        explicit SwitchLabelSyntax(SyntaxKind kind) : SyntaxBase(kind) {}

    };

    struct StringPartSyntax : SyntaxBase {

        explicit StringPartSyntax(SyntaxKind kind) : SyntaxBase(kind) {}

    };

    struct TypeDeclarationSyntax : MemberDeclarationSyntax {

        explicit TypeDeclarationSyntax(SyntaxKind kind) : MemberDeclarationSyntax(kind) {}

    };
    struct TypeParameterConstraintSyntax : MemberDeclarationSyntax {

        explicit TypeParameterConstraintSyntax(SyntaxKind kind) : MemberDeclarationSyntax(kind) {}

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