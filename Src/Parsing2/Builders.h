//#pragma once
//
//#include "./SyntaxBase.h"
//#include "./SyntaxNodes.h"
//#include "SyntaxFacts.h"
//
//namespace Alchemy::Compilation {
//
//    struct SyntaxBaseBuilder {
//
//        LinearAllocator* allocator;
//
//        virtual SyntaxBase* Build() = 0;
//
//    };
//
//    struct ExpressionSyntaxBuilder : SyntaxBaseBuilder {
//
//    };
//
//    struct TypeSyntaxBuilder : ExpressionSyntaxBuilder {
//
//    };
//
//    struct NameSyntaxBuilder : TypeSyntaxBuilder {
//
//    };
//
//    struct SimpleNameSyntaxBuilder : NameSyntaxBuilder {
//
//    };
//
//    struct MemberDeclarationSyntaxBuilder : SyntaxBaseBuilder {
//    };
//
//    struct SyntaxListBuilder {
//
//        LinearAllocator* allocator;
//
//        SyntaxListUntyped* Build() {
//            SyntaxListUntyped* retn = allocator->Allocate<SyntaxListUntyped>(1);
//            return retn;
//        }
//
//    };
//
//    struct SeparatedSyntaxListBuilder {
//        LinearAllocator* allocator;
//
//        SyntaxBaseBuilder ** items;
//        int32 itemCount;
//        SyntaxToken * separators;
//        int32 separatorCount;
//        int32 itemCapacity;
//        int32 separatorCapacity;
//
//        explicit SeparatedSyntaxListBuilder(LinearAllocator* allocator)
//            : allocator(allocator)
//            , itemCount(0)
//            , separatorCount(0)
//            , itemCapacity(4)
//            , separatorCapacity(4)
//            , items(allocator->AllocateUncleared<SyntaxBaseBuilder*>(4))
//            , separators(allocator->AllocateUncleared<SyntaxToken>(4))
//            {}
//
//        SeparatedSyntaxListBuilder* AddItem(SyntaxBaseBuilder* item) {
//            items = allocator->ResizeList(items, itemCount, &itemCapacity);
//            items[itemCount++] = item;
//            return this;
//        }
//
//        SeparatedSyntaxListBuilder* AddSeparator(SyntaxToken token) {
//            separators = allocator->ResizeList(separators, separatorCount, &separatorCapacity);
//            separators[separatorCount++] = token;
//            return this;
//        }
//
//        SeparatedSyntaxListUntyped* Build() {
//            SeparatedSyntaxListUntyped* retn = allocator->Allocate<SeparatedSyntaxListUntyped>(1);
//            retn->separatorCount = separatorCount;
//            retn->itemCount = itemCount;
//            retn->items = allocator->AllocateUncleared<SyntaxBase*>(itemCount);
//            for(int32 i = 0; i < itemCount; i++) {
//                retn->items[i] = items[i]->Build();
//            }
//            retn->separators = separators;
//            return retn;
//        }
//    };
//
//    struct TokenListBuilder {
//
//        LinearAllocator* allocator;
//        SyntaxToken* array;
//        int32 size;
//        int32 capacity;
//
//        explicit TokenListBuilder(LinearAllocator* allocator)
//            : allocator(allocator)
//            , array(allocator->Allocate<SyntaxToken>(4))
//            , size(0)
//            , capacity(4) {}
//
//        TokenListBuilder* AddToken(SyntaxToken token) {
//            array = allocator->ResizeList<SyntaxToken>(array, size, &capacity);
//            array[size] = token;
//            size++;
//            return this;
//        }
//
//        TokenList* Build() {
//            TokenList* retn = allocator->Allocate<TokenList>(1);
//            retn->array = allocator->AllocateUncleared<SyntaxToken>(size);
//            retn->size = size;
//            memcpy(retn->array, array, sizeof(SyntaxToken) * size);
//            return retn;
//        }
//    };
//
//
//    struct BuilderBase {
//
//        LinearAllocator* allocator;
//
//        explicit BuilderBase(LinearAllocator* allocator)
//            : allocator(allocator) {}
//
//        struct SyntaxToken MakeIdentifier(const char* identifier, SyntaxTokenFlags flags = SyntaxTokenFlags::None) {
//            struct SyntaxToken retn;
//            retn.kind = SyntaxKind::IdentifierToken;
//            retn.contextualKind = SyntaxKind::IdentifierToken;
//            retn.SetFlags(flags);
//            retn.text = (char*) identifier;
//            retn.textSize = (uint16) strlen(identifier);
//            SyntaxKind keyword;
//
//            if (TryMatchKeyword_Generated((char*) identifier, (int32) strlen(identifier), &keyword)) {
//                if (SyntaxFacts::IsReservedKeyword(keyword)) {
//                    retn.kind = keyword;
//                    retn.contextualKind = SyntaxKind::None;
//                }
//                else {
//                    // it's contextual
//                    assert(SyntaxFacts::IsContextualKeyword(keyword));
//                    retn.kind = SyntaxKind::IdentifierToken;
//                    retn.contextualKind = keyword;
//
//                }
//
//            }
//
//            return retn;
//        }
//
//        struct SyntaxToken SyntaxToken(TokenKind kind, SyntaxTokenFlags flags = SyntaxTokenFlags::None) {
//            struct SyntaxToken retn;
//            retn.kind = kind;
//            retn.SetFlags(flags);
//
//            assert(kind != TokenKind::IdentifierToken && "use MakeIdentifier instead");
//            retn.text = (char*) SyntaxFacts::GetText(kind);
//            retn.textSize = (uint16) strlen(retn.text);
//            return retn;
//        }
//
//        struct SyntaxToken SyntaxToken(SyntaxKind kind, SyntaxKind contextualKind, SyntaxTokenFlags flags = SyntaxTokenFlags::None) {
//            struct SyntaxToken retn;
//            retn.kind = kind;
//            retn.contextualKind = contextualKind;
//            assert(kind != SyntaxKind::IdentifierToken && "use MakeIdentifier instead");
//            retn.SetFlags(flags);
//
//            retn.text = (char*) SyntaxFacts::GetText(contextualKind);
//            retn.textSize = (uint16) strlen(retn.text);
//
//            return retn;
//        }
//
//        TokenListBuilder* TokenList() {
//            return allocator->New<TokenListBuilder>(allocator);
//        }
//
//        SeparatedSyntaxListBuilder* SeparatedSyntaxList() {
//            return allocator->New<SeparatedSyntaxListBuilder>(allocator);
//        }
//
//    };
//
//
//}