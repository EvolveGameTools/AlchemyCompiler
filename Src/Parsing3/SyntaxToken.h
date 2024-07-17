#pragma once

#include "./TokenKind.h"

namespace Alchemy::Compilation {

    DEFINE_ENUM_FLAGS(SyntaxTokenFlags, uint8, {
        None = 0,
        Error = 1 << 1,
        LeadingTrivia = 1 << 2,
        TrailingTrivia = 1 << 3,
        Missing = 1 << 4,
        Skipped = 1 << 5,
        Omitted = 1 << 6
    })

    struct SyntaxToken {

        TokenKind kind{};
        TokenKind contextualKind{};
        uint16 textSize{};
        uint32 id_flags{};

        inline FixedCharSpan GetText(CheckedArray<char*> texts) {
            if (IsMissing() || !IsValid()) {
                return FixedCharSpan(nullptr, 0);
            }
            char* text = texts[GetId()];
            return FixedCharSpan(text, textSize);
        }

        inline int32 GetId() const {
            return static_cast<int32>((id_flags >> 8) & 0xffffff);
        }

        inline SyntaxTokenFlags GetFlags() const {
            return static_cast<SyntaxTokenFlags>(id_flags & 0xff);
        }

        inline void SetId(int32 id) {
            id_flags &= ~(0xFFFFFF << 8); // Clear the ID bits
            id_flags |= (static_cast<uint32>(id) << 8); // Set new ID
        }

        inline void SetFlags(SyntaxTokenFlags flags) {
            id_flags &= ~(0xFF); // Clear the flags bits
            id_flags |= (uint8) flags; // Set new flags
        }

        inline bool ContainsDiagnostics() {
            return (GetFlags() & SyntaxTokenFlags::Error) != 0;
        }

        inline bool IsValid() {
            return kind != TokenKind::None;
        }

        inline bool IsMissing() {
            return (GetFlags() & SyntaxTokenFlags::Missing) != 0;
        }

        inline void AddFlag(SyntaxTokenFlags flags) {
            SetFlags(GetFlags() | flags);
        }

    };

    struct PendingSyntaxToken : SyntaxToken  {
        char * text {};

        inline void SetFlags(SyntaxTokenFlags flags) {
            id_flags &= ~(0xFF); // Clear the flags bits
            id_flags |= (uint8) flags; // Set new flags
        }
    };

    static_assert(sizeof(SyntaxToken) == 8);
}