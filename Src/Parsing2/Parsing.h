#pragma once

#include "./SyntaxNodes.h"

namespace Alchemy::Compilation {

    enum class PostSkipAction {
        Continue,
        Abort
    };

    enum class NamespaceParts {
        None = 0,
        ExternAliases,
        Usings,
        GlobalAttributes,
        MembersAndStatements,
        TypesAndNamespaces,
        TopLevelStatementsAfterTypesAndNamespaces,
    };

    enum class ScanTypeArgumentListKind {
        NotTypeArgumentList,
        PossibleTypeArgumentList,
        DefiniteTypeArgumentList
    };

    DEFINE_ENUM_FLAGS(NameOptions, uint16, {
        None = 0,
        InExpression = 1 << 0, // Used to influence parser ambiguity around "<" and generics vs. expressions. Used in ParseSimpleName.
        InTypeList = 1 << 1, // Allows attributes to appear within the generic type argument list. Used during ParseInstantiation.
        PossiblePattern = 1 << 2, // Used to influence parser ambiguity around "<" and generics vs. expressions on the right of 'is'
        AfterIs = 1 << 3,
        DefinitePattern = 1 << 4,
        AfterOut = 1 << 5,
        AfterTupleComma = 1 << 6,
        FirstElementOfPossibleTupleLiteral = 1 << 7
    })

    DEFINE_ENUM_FLAGS(VariableFlags, int32, {
        Fixed = 0x01,
        Const = 0x02,
        LocalOrField = 0x04
    })

    struct TokenListBuffer {
        TempAllocator* allocator;
        SyntaxToken* array;
        int32 capacity;
        int32 size;

        explicit TokenListBuffer(TempAllocator* allocator)
                : allocator(allocator)
                  , size(0)
                  , capacity(8)
                  , array(allocator->AllocateUncleared<SyntaxToken>(8)) {}

        void Add(SyntaxToken item) {
            if (size + 1 > capacity) {
                SyntaxToken* ptr = allocator->AllocateUncleared<SyntaxToken>(capacity * 2);
                memcpy(ptr, array, sizeof(SyntaxToken) * size);
                array = ptr;
                capacity *= 2;
            }
            array[size] = item;
            size++;
        }

        TokenList* Persist(LinearAllocator * pAllocator) {
            SyntaxToken * p = pAllocator->AllocateUncleared<SyntaxToken>(size);
            memcpy(p, array, sizeof(SyntaxToken) * size);
            return pAllocator->New<TokenList>(p, size);
        }

    };

    typedef bool (* abortFn)(Parser* parser, SyntaxKind kind);

    typedef bool (* notExpectedFn)(Parser* parser);

    bool IsTrueIdentifier(Parser* parser);

    bool IsPossibleType(Parser* parser);

    bool IsPossibleNamespaceMemberDeclaration(Parser* parser);

    bool IsPartialType(Parser* parser);

    TypeSyntax* ParseType(Parser* parser, ParseTypeMode mode = ParseTypeMode::Normal);

    IdentifierNameSyntax* CreateMissingIdentifierName(Parser* parser);

    TypeSyntax* ParseQualifiedName(Parser* parser, NameOptions options);

    NameSyntax* ParseQualifiedNameRight(Parser* parser, NameOptions options, NameSyntax* left, SyntaxToken separator);

    VariableDeclaratorSyntax* ParseVariableDeclarator(Parser* parser, TypeSyntax* parentType, VariableFlags flags, bool isFirst, bool allowLocalFunctions, TokenListBuffer* mods, LocalFunctionStatementSyntax** localFunction, bool isExpressionContext = false);
    ExpressionSyntax* ParseExpression(Parser* parser);
    ExpressionSyntax* ParseArrayInitializer(Parser * parser);
    ArrayRankSpecifierSyntax* ParseArrayRankSpecifier(Parser* parser, bool* pSawNonOmittedSize);
    SimpleNameSyntax* ParseSimpleName(Parser* parser, NameOptions options = NameOptions::None);
    MemberDeclarationSyntax* ParseMemberDeclaration(Parser* parser, SyntaxKind parentKind);

    SyntaxToken MakeMissingToken(SyntaxKind kind, int32 tokenId);

    ScanTypeFlags ScanPossibleTypeArgumentList(Parser* parser, SyntaxToken* greaterThanToken, bool* pIsDefinitelyTypeArgumentList);

    ScanTypeFlags ScanType(Parser* parser, ParseTypeMode mode, SyntaxToken* lastTokenOfType);

    ScanTypeArgumentListKind ScanTypeArgumentList(Parser* parser, NameOptions options);

    PostSkipAction SkipBadSeparatedListTokensWithExpectedKind(Parser* parser, notExpectedFn isNotExpectedFunction, abortFn abortFunction, SyntaxKind expected, SyntaxKind closeKind = SyntaxKind::None);

    PostSkipAction SkipBadTokensWithExpectedKind(Parser* parser, notExpectedFn isNotExpectedFunction, abortFn abortFunction, SyntaxKind expected, SyntaxKind closeKind);

    SyntaxToken ParseIdentifierToken(Parser* parser, ErrorCode errorCode = ErrorCode::ERR_IdentifierExpected);

    bool IsPossibleLambdaExpression(Parser* parser, Precedence precedence);

    bool IsPossibleStartOfTypeDeclaration(SyntaxKind kind);

    bool IsPossibleAccessorModifier(Parser* parser);

    bool IsTypeDeclarationStart(Parser* parser);

    bool IsStartOfPropertyBody(SyntaxKind kind);


    template<typename T>
    struct SeparatedNodeListBuilder {

        TempAllocator* allocator;
        T** items;
        SyntaxToken* separators;

        int32 capacity;
        int32 itemCount;
        int32 separatorCount;

        explicit SeparatedNodeListBuilder(TempAllocator* allocator)
                : allocator(allocator)
                  , itemCount(0)
                  , separatorCount(0)
                  , capacity(8)
                  , items(allocator->AllocateUncleared<T*>(8))
                  , separators(allocator->AllocateUncleared<SyntaxToken>(8)) {
        }

        void EnsureAdditionalCapacity(int32 additional) {
            if (itemCount + additional > capacity || separatorCount + additional > capacity) {
                int32 m = capacity * 2 > capacity + additional ? capacity * 2 : (capacity * 2) + additional;
                T** pItems = allocator->AllocateUncleared<T*>(m);
                SyntaxToken* pTokens = allocator->AllocateUncleared<SyntaxToken>(m);
                memcpy(pItems, items, sizeof(T*) * itemCount);
                memcpy(pTokens, separators, sizeof(SyntaxToken) * separatorCount);
                items = pItems;
                separators = pTokens;
                capacity = m;
            }
        }

        SyntaxToken GetSeparator(int32 idx) {
            assert(idx >= 0 && idx < separatorCount);
            return separators[idx];
        }

        T* GetItem(int32 idx) {
            assert(idx >= 0 && idx < itemCount);
            return items[idx];
        }

        void Add(T* item) {
            assert(itemCount == separatorCount && "expected a separator");
            EnsureAdditionalCapacity(1);
            items[itemCount] = item;
            itemCount++;
        }

        void AddSeparator(SyntaxToken separator) {
            assert(itemCount == separatorCount + 1 && "expected an item");
            EnsureAdditionalCapacity(1);
            separators[separatorCount] = separator;
            separatorCount++;
        }

        SeparatedSyntaxList<T>* ToList(LinearAllocator* pAllocator) {
            SeparatedSyntaxList<T>* retn = pAllocator->AllocateUncleared<SeparatedSyntaxList<T>>(1);

            SyntaxToken* pSeparators = pAllocator->AllocateUncleared<SyntaxToken>(separatorCount);
            T** pItems = pAllocator->AllocateUncleared<T*>(itemCount);

            memcpy(pItems, items, sizeof(T*) * itemCount);
            memcpy(pSeparators, separators, sizeof(SyntaxToken) * separatorCount);
            new(retn) SeparatedSyntaxList<T>(itemCount, pItems, separatorCount, pSeparators);
            return retn;
        }

    };

}