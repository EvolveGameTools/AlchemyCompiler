#include "../Allocation/LinearAllocator.h"
#include "./Scanning.h"
#include "./Parser.h"
#include "./Parsing.h"
#include "./SyntaxFacts.h"
#include "../Collections/FixedPodList.h"

namespace Alchemy::Compilation {

    struct ResetPoint {

        bool resetOnDispose;
        size_t allocatorOffset;
        size_t tempAllocatorOffset;
        Parser* originalParser;
        Parser copyParser;
        Diagnostics diagnosticsCopy;

        explicit ResetPoint(Parser* parser, bool resetOnDispose = true)
            : originalParser(parser)
            , copyParser(*parser)
            , allocatorOffset(parser->allocator->offset)
            , tempAllocatorOffset(parser->tempAllocator->offset)
            , diagnosticsCopy(*parser->diagnostics)
            , resetOnDispose(resetOnDispose) {}

        ~ResetPoint() {
            if (resetOnDispose) {
                Reset();
            }
        }

        void Reset() {
            *originalParser = copyParser;
            *originalParser->diagnostics = diagnosticsCopy;
            originalParser->tempAllocator->offset = tempAllocatorOffset;
            originalParser->allocator->offset = allocatorOffset;
        }

    };

    template<typename T>
    struct SyntaxListBuilder {
        TempAllocator* allocator;
        T** array;
        int32 capacity;
        int32 size;

        explicit SyntaxListBuilder(TempAllocator* allocator)
            : allocator(allocator)
            , size(0)
            , capacity(8)
            , array(allocator->AllocateUncleared<T*>(8)) {
        }

        void Add(T* item) {
            if (size + 1 > capacity) {
                T** ptr = allocator->AllocateUncleared<T*>(capacity * 2);
                memcpy(ptr, array, sizeof(T*) * size);
                array = ptr;
                capacity *= 2;
            }
            array[size] = item;
            size++;
        }

        SyntaxList<T>* ToSyntaxList(LinearAllocator* pAllocator) {
            SyntaxList<T>* retn = pAllocator->AllocateUncleared<SyntaxList<T>>(1);
            T** ptr = pAllocator->AllocateUncleared<T*>(size);
            memcpy(ptr, array, sizeof(T*) * size);
            new(retn) SyntaxList<T>(ptr, size);
            return retn;
        }

        T* Get(int32 i) {
            assert(i > 0 && i < size);
            return array[i];
        }

    };

    bool IsPossibleType(Parser* parser) {
        TokenKind tk = parser->currentToken.kind;
        return SyntaxFacts::IsPredefinedType(tk) || IsTrueIdentifier(parser);
    }

    ScanTypeFlags ScanType(Parser* parser, SyntaxToken* lastTokenOfType, bool forPattern) {
        return ScanType(parser, forPattern ? ParseTypeMode::DefinitePattern : ParseTypeMode::Normal, lastTokenOfType);
    }

    bool IsEndOfNamespace(Parser* parser) {
        return parser->currentToken.kind == TokenKind::CloseBraceToken;
    }

    bool IsNamespaceMemberStartOrStop(Parser* parser) {
        return IsEndOfNamespace(parser) || IsPossibleNamespaceMemberDeclaration(parser);
    }

    bool IsAttributeDeclarationTerminator() {
        return false;
    }

    bool IsPossibleAggregateClauseStartOrStop() {
        return false;
    }

    bool CanStartMember(TokenKind kind) {
        switch (kind) {
            case TokenKind::BoolKeyword:
            case TokenKind::ByteKeyword:
            case TokenKind::FloatKeyword:
            case TokenKind::IntKeyword:
            case TokenKind::LongKeyword:
            case TokenKind::SByteKeyword:
            case TokenKind::ShortKeyword:
            case TokenKind::UIntKeyword:
            case TokenKind::ULongKeyword:
            case TokenKind::UShortKeyword:
            case TokenKind::CharKeyword:

            case TokenKind::AbstractKeyword:
            case TokenKind::ClassKeyword:
            case TokenKind::ConstKeyword:
            case TokenKind::DelegateKeyword:
            case TokenKind::DoubleKeyword:
            case TokenKind::EnumKeyword:
            case TokenKind::ExternKeyword:
            case TokenKind::FixedKeyword:
            case TokenKind::InterfaceKeyword:
            case TokenKind::InternalKeyword:
//            case TokenKind::NewKeyword: // not sure that we want this as a name override
            case TokenKind::ObjectKeyword:
            case TokenKind::OverrideKeyword:
            case TokenKind::PrivateKeyword:
            case TokenKind::ProtectedKeyword:
            case TokenKind::PublicKeyword:
            case TokenKind::ReadOnlyKeyword:
            case TokenKind::SealedKeyword:
            case TokenKind::StaticKeyword:
            case TokenKind::StringKeyword:
            case TokenKind::StructKeyword:
            case TokenKind::VirtualKeyword:
            case TokenKind::VoidKeyword:
            case TokenKind::IdentifierToken:
//            case TokenKind::TildeToken:     I think we don't support destructors
            case TokenKind::OpenBracketToken:
            case TokenKind::ImplicitKeyword: // maybe?
            case TokenKind::ExplicitKeyword: // maybe?
//            case TokenKind::OpenParenToken:    //tuple -- if we end up supporting that
            case TokenKind::RefKeyword:
                return true;

            default:
                return false;
        }
    }

    bool IsPossibleMemberStart(Parser* parser) {
        return CanStartMember(parser->currentToken.kind);
    }

    bool IsPossibleMemberStartOrStop(Parser* parser) {
        return IsPossibleMemberStart(parser) || parser->currentToken.kind == TokenKind::CloseBraceToken;
    }

    bool IsEndOfReturnType(Parser* parser) {
        switch (parser->currentToken.kind) {
            case TokenKind::OpenParenToken:
            case TokenKind::OpenBraceToken:
            case TokenKind::SemicolonToken:
                return true;
            default:
                return false;
        }
    }

    bool IsEndOfParameterList(Parser* parser) {
        TokenKind kind = parser->currentToken.kind;
        return kind == TokenKind::CloseParenToken || kind == TokenKind::CloseBracketToken || kind == TokenKind::SemicolonToken;
    }

    bool IsEndOfFieldDeclaration(Parser* parser) {
        return parser->currentToken.kind == TokenKind::SemicolonToken;
    }

    bool IsPossibleEndOfVariableDeclaration(Parser* parser) {
        switch (parser->currentToken.kind) {
            case TokenKind::CommaToken:
            case TokenKind::SemicolonToken:
                return true;
            default:
                return false;
        }
    }

    bool IsEndOfTypeArgumentList(Parser* parser) {
        return parser->currentToken.kind == TokenKind::GreaterThanToken;
    }

    bool IsPartialMember(Parser* parser) {
        // partials must be void return type
        return parser->currentToken.contextualKind == TokenKind::PartialKeyword && parser->PeekToken(1).kind == TokenKind::VoidKeyword;
    }

    bool IsCurrentTokenPartialKeywordOfPartialMethodOrType(Parser* parser) {
        if (parser->currentToken.contextualKind == TokenKind::PartialKeyword) {
            if (IsPartialType(parser) || IsPartialMember(parser)) {
                return true;
            }
        }

        return false;
    }

    bool IsCurrentTokenWhereOfConstraintClause(Parser* parser) {
        return
            parser->currentToken.contextualKind == TokenKind::WhereKeyword &&
                parser->PeekToken(1).kind == TokenKind::IdentifierToken &&
                parser->PeekToken(2).kind == TokenKind::ColonToken;
    }

    bool IsTrueIdentifier(Parser* parser) {
        if (parser->currentToken.kind == TokenKind::IdentifierToken
            && !IsCurrentTokenPartialKeywordOfPartialMethodOrType(parser)
            && !IsCurrentTokenWhereOfConstraintClause(parser)) {
            return true;
        }

        return false;
    }

    bool IsPossibleAnonymousMethodExpression(Parser* parser) {
        // Skip past any static keywords

        int32 tokenIndex = 0;

        if (parser->PeekToken(tokenIndex).kind == TokenKind::StaticKeyword) {
            tokenIndex++;
        }

        return parser->PeekToken(tokenIndex).kind == TokenKind::DelegateKeyword;
    }

    ScanTypeFlags ScanPossibleTypeArgumentList(Parser* parser, SyntaxToken* greaterThanToken, bool* pIsDefinitelyTypeArgumentList) {
        *pIsDefinitelyTypeArgumentList = false;
        assert(parser->currentToken.kind == TokenKind::LessThanToken);

        ScanTypeFlags result = ScanTypeFlags::GenericTypeOrExpression;
        ScanTypeFlags lastScannedType;

        do {
            parser->EatToken();

            // Type arguments cannot contain attributes, so if this is an open square, we early out and assume it is not a type argument
            if (parser->currentToken.kind == TokenKind::OpenBracketToken) {
                return ScanTypeFlags::NotType;
            }

            if (parser->currentToken.kind == TokenKind::GreaterThanToken) {
                *greaterThanToken = parser->EatToken();
                return result;
            }

            SyntaxToken unused = SyntaxToken();
            lastScannedType = ScanType(parser, &unused, false);

            switch (lastScannedType) {
                case ScanTypeFlags::NotType:
                    return ScanTypeFlags::NotType;

                case ScanTypeFlags::MustBeType:
                    // We're currently scanning a possible type-argument list.  But we're
                    // not sure if this is actually a type argument list, or is maybe some
                    // complex relational expression with <'s and >'s.  One thing we can
                    // tell though is that if we have a predefined type (like 'int' or 'string')
                    // before a comma or > then this is definitely a type argument list. i.e.
                    // if you have:
                    //
                    //      var v = ImmutableDictionary<int,
                    //
                    // then there's no legal interpretation of this as an expression (since a
                    // standalone predefined type is not a valid simple term.  Contrast that
                    // with :
                    //
                    //  var v = ImmutableDictionary<Int32,
                    //
                    // Here this might actually be a relational expression and the comma is meant
                    // to separate out the variable declarator 'v' from the next variable.
                    //
                    // Note: we check if we got 'MustBeType' which triggers for predefined types,
                    // (int, string, etc.), or array types (Goo[], A<T>[][] etc.), or pointer types
                    // of things that must be types (int*, void**, etc.).
                    *pIsDefinitelyTypeArgumentList = *pIsDefinitelyTypeArgumentList || parser->currentToken.kind == TokenKind::CommaToken || parser->currentToken.kind == TokenKind::GreaterThanToken;
                    result = ScanTypeFlags::GenericTypeOrMethod;
                    break;

                case ScanTypeFlags::NullableType:
                    // See above.  If we have `X<Y?,` or `X<Y?>` then this is definitely a type argument list.
                    *pIsDefinitelyTypeArgumentList = *pIsDefinitelyTypeArgumentList || parser->currentToken.kind == TokenKind::CommaToken || parser->currentToken.kind == TokenKind::GreaterThanToken;
                    if (*pIsDefinitelyTypeArgumentList) {
                        result = ScanTypeFlags::GenericTypeOrMethod;
                    }

                    // Note: we intentionally fall out without setting 'result'.
                    // Seeing a nullable type (not followed by a , or > ) is not enough
                    // information for us to determine what this is yet.  i.e. the user may have:
                    //
                    //      X < Y ? Z : W
                    //
                    // We'd see a nullable type here, but this is definitely not a type arg list.

                    break;

                case ScanTypeFlags::GenericTypeOrExpression:
                    // See above.  If we have  X<Y<Z>,  then this would definitely be a type argument list.
                    // However, if we have  X<Y<Z>> then this might not be type argument list.  This could just
                    // be some sort of expression where we're comparing, and then shifting values.
                    if (!*pIsDefinitelyTypeArgumentList) {
                        *pIsDefinitelyTypeArgumentList = parser->currentToken.kind == TokenKind::CommaToken;
                        result = ScanTypeFlags::GenericTypeOrMethod;
                    }
                    break;

                case ScanTypeFlags::GenericTypeOrMethod:
                    result = ScanTypeFlags::GenericTypeOrMethod;
                    break;

                case ScanTypeFlags::NonGenericTypeOrExpression:
                    // Explicitly keeping this case in the switch for clarity.  We parsed out another portion of the
                    // type argument list that looks like it's a non-generic-type-or-expr (the simplest case just
                    // being "X").  That changes nothing here wrt determining what type of entity we have here, so
                    // just fall through and see if we're followed by a "," (in which case keep going), or a ">", in
                    // which case we're done.
                    break;
                case ScanTypeFlags::PointerOrMultiplication:
                case ScanTypeFlags::TupleType:
                    break;
            }
        } while (parser->currentToken.kind == TokenKind::CommaToken);

        if (parser->currentToken.kind != TokenKind::GreaterThanToken) {
            // Error recovery after missing > token:

            // In the case of an identifier, we assume that there could be a missing > token
            // For example, we have reached C in X<A, B C
            if (parser->currentToken.kind == TokenKind::IdentifierToken) {
                *greaterThanToken = parser->EatToken(TokenKind::GreaterThanToken);
                return result;
            }

            // As for tuples, we do not expect direct invocation right after the parenthesis
            // EXAMPLE: X<(string, string)(), where we imply a missing > token between )(
            // as the user probably wants to invoke X by X<(string, string)>()
            if (lastScannedType == ScanTypeFlags::TupleType && parser->currentToken.kind == TokenKind::OpenParenToken) {
                *greaterThanToken = parser->EatToken(TokenKind::GreaterThanToken);
                return result;
            }

            return ScanTypeFlags::NotType;
        }

        *greaterThanToken = parser->EatToken();

        // If we have `X<Y>)` then this would definitely be a type argument list.
        *pIsDefinitelyTypeArgumentList = *pIsDefinitelyTypeArgumentList || parser->currentToken.kind == TokenKind::CloseParenToken;
        if (*pIsDefinitelyTypeArgumentList) {
            result = ScanTypeFlags::GenericTypeOrMethod;
        }

        return result;
    }

    ScanTypeFlags ScanNamedTypePart(Parser* parser, SyntaxToken* lastTokenOfType) {

        if (parser->currentToken.kind != TokenKind::IdentifierToken || !IsTrueIdentifier(parser)) {
            return ScanTypeFlags::NotType;
        }

        *lastTokenOfType = parser->EatToken();
        if (parser->currentToken.kind == TokenKind::LessThanToken) {
            bool unused = false;
            return ScanPossibleTypeArgumentList(parser, lastTokenOfType, &unused);
        }
        else {
            return ScanTypeFlags::NonGenericTypeOrExpression;
        }

    }

    ScanTypeFlags ScanTupleType(Parser* parser, SyntaxToken* lastTokenOfType) {
        ScanTypeFlags tupleElementType = ScanType(parser, lastTokenOfType, false);
        if (tupleElementType != ScanTypeFlags::NotType) {
            if (IsTrueIdentifier(parser)) {
                *lastTokenOfType = parser->EatToken();
            }

            if (parser->currentToken.kind == TokenKind::CommaToken) {
                do {
                    *lastTokenOfType = parser->EatToken();
                    tupleElementType = ScanType(parser, lastTokenOfType, false);

                    if (tupleElementType == ScanTypeFlags::NotType) {
                        *lastTokenOfType = parser->EatToken();
                        return ScanTypeFlags::NotType;
                    }

                    if (IsTrueIdentifier(parser)) {
                        *lastTokenOfType = parser->EatToken();
                    }
                } while (parser->currentToken.kind == TokenKind::CommaToken);

                if (parser->currentToken.kind == TokenKind::CloseParenToken) {
                    *lastTokenOfType = parser->EatToken();
                    return ScanTypeFlags::TupleType;
                }
            }
        }

        // Can't be a type!
        return ScanTypeFlags::NotType;
    }

    bool IsMakingProgress(Parser* parser, int* lastTokenPosition, bool assertIfFalse) {
        int32 pos = parser->ptr;

        if (pos > *lastTokenPosition) {
            *lastTokenPosition = pos;
            return true;
        }

        assert(!assertIfFalse);
        return false;
    }

    ScanTypeFlags ScanType(Parser* parser, bool forPattern) {
        SyntaxToken lastTokenOfType;
        return ScanType(parser, &lastTokenOfType, forPattern);
    }

    ScanTypeFlags ScanType(Parser* parser, ParseTypeMode mode, SyntaxToken* lastTokenOfType) {
        assert(mode != ParseTypeMode::NewExpression);
        ScanTypeFlags result;

        if (parser->currentToken.kind == TokenKind::RefKeyword) {
            // todo -- extend this for 'temp', 'scoped' etc
            // in a ref local or ref return, we treat "ref" and "ref readonly" as part of the type
            parser->EatToken();

            if (parser->currentToken.kind == TokenKind::ReadOnlyKeyword) {
                parser->EatToken();
            }

        }

        if (parser->currentToken.kind == TokenKind::IdentifierToken) {

            assert(parser->currentToken.kind == TokenKind::IdentifierToken);

            result = ScanNamedTypePart(parser, lastTokenOfType);
            if (result == ScanTypeFlags::NotType) {
                return ScanTypeFlags::NotType;
            }

            assert(result == ScanTypeFlags::GenericTypeOrExpression || result == ScanTypeFlags::GenericTypeOrMethod || result == ScanTypeFlags::NonGenericTypeOrExpression);

            // Scan a name
            for (bool firstLoop = true; parser->currentToken.kind == TokenKind::DotToken || parser->currentToken.kind == TokenKind::DotDotToken; firstLoop = false) {

                parser->EatToken();
                result = ScanNamedTypePart(parser, lastTokenOfType);
                if (result == ScanTypeFlags::NotType) {
                    return ScanTypeFlags::NotType;
                }

                assert(result == ScanTypeFlags::GenericTypeOrExpression || result == ScanTypeFlags::GenericTypeOrMethod || result == ScanTypeFlags::NonGenericTypeOrExpression);
            }

        }
        else if (SyntaxFacts::IsPredefinedType(parser->currentToken.kind)) {
            // Simple type...
            *lastTokenOfType = parser->EatToken();
            result = ScanTypeFlags::MustBeType;
        }
        else if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            *lastTokenOfType = parser->EatToken();

            result = ScanTupleType(parser, lastTokenOfType);
            if (result == ScanTypeFlags::NotType || mode == ParseTypeMode::DefinitePattern && parser->currentToken.kind != TokenKind::OpenBracketToken) {
                // A tuple type can appear in a pattern only if it is the element type of an array type.
                return ScanTypeFlags::NotType;
            }
        }
        else {
            // Can't be a type!
            return ScanTypeFlags::NotType;
        }

        int32 lastTokenPosition = -1;

        while (IsMakingProgress(parser, &lastTokenPosition)) {

            if (parser->currentToken.kind == TokenKind::QuestionToken) {

                if (lastTokenOfType->kind == TokenKind::QuestionToken && lastTokenOfType->kind != TokenKind::AsteriskToken) {
                    // don't allow `Type??`
                    // don't allow `Type*?`
                    *lastTokenOfType = parser->EatToken();
                    result = ScanTypeFlags::NullableType;
                    continue;
                }

            }

            switch (parser->currentToken.kind) {
                case TokenKind::OpenBracketToken:
                    // Check for array types.
                    parser->EatToken();
                    while (parser->currentToken.kind == TokenKind::CommaToken) {
                        parser->EatToken();
                    }

                    if (parser->currentToken.kind != TokenKind::CloseBracketToken) {
                        return ScanTypeFlags::NotType;
                    }

                    *lastTokenOfType = parser->EatToken();
                    result = ScanTypeFlags::MustBeType;
                    break;
                default:
                    goto done;
            }
        }

        done:
        return result;
    }

    bool ScanParenthesizedLambda(Parser* parser, Precedence precedence) {
        return false;
    }


    struct NamespaceBodyBuilder {
        SyntaxListBuilder<MemberDeclarationSyntax>* members;
    };

    void AddIncompleteMembers(SyntaxListBuilder<MemberDeclarationSyntax>* incompleteMembers, NamespaceBodyBuilder* body) {
        for (int32 i = 0; i < incompleteMembers->size; i++) {
            body->members->Add(incompleteMembers->array[i]);

        }
        incompleteMembers->size = 0;
    }

    MemberDeclarationSyntax* adjustStateAndReportStatementOutOfOrder(Parser* parser, NamespaceParts* seen, MemberDeclarationSyntax* memberOrStatement) {
        switch (memberOrStatement->GetKind()) {
            case SyntaxKind::GlobalStatement:
                if (*seen < NamespaceParts::MembersAndStatements) {
                    *seen = NamespaceParts::MembersAndStatements;
                }
                else if (*seen == NamespaceParts::TypesAndNamespaces) {
                    *seen = NamespaceParts::TopLevelStatementsAfterTypesAndNamespaces;
                    parser->AddError(memberOrStatement, ErrorCode::ERR_TopLevelStatementAfterNamespaceOrType);
                }

                break;

            case SyntaxKind::NamespaceDeclaration:
            case SyntaxKind::EnumDeclaration:
            case SyntaxKind::StructDeclaration:
            case SyntaxKind::ClassDeclaration:
            case SyntaxKind::InterfaceDeclaration:
            case SyntaxKind::DelegateDeclaration:
                if (*seen < NamespaceParts::TypesAndNamespaces) {
                    *seen = NamespaceParts::TypesAndNamespaces;
                }
                break;

            default:
                if (*seen < NamespaceParts::MembersAndStatements) {
                    *seen = NamespaceParts::MembersAndStatements;
                }
                break;
        }

        return memberOrStatement;
    }

    MemberDeclarationSyntax* ParseNamespaceDeclaration(Parser* parser, SyntaxListBuilder<SyntaxBase>* modifiers) {
        return nullptr;
    }

    enum class DeclarationModifiers {
        None,
        Public,
        Internal,
        Protected,
        Private,
        Sealed,
        Abstract,

        Static,
        Virtual,
        Override,
        ReadOnly,
        Partial,
        Ref,
    };


    DeclarationModifiers GetModifier(TokenKind kind, TokenKind contextualKind) {
        switch (kind) {
            case TokenKind::PublicKeyword:
                return DeclarationModifiers::Public;
            case TokenKind::InternalKeyword:
                return DeclarationModifiers::Internal;
            case TokenKind::ProtectedKeyword:
                return DeclarationModifiers::Protected;
            case TokenKind::PrivateKeyword:
                return DeclarationModifiers::Private;
            case TokenKind::SealedKeyword:
                return DeclarationModifiers::Sealed;
            case TokenKind::AbstractKeyword:
                return DeclarationModifiers::Abstract;
            case TokenKind::StaticKeyword:
                return DeclarationModifiers::Static;
            case TokenKind::VirtualKeyword:
                return DeclarationModifiers::Virtual;
//            case SyntaxKind.ExternKeyword:
//                return DeclarationModifiers.Extern;
//            case SyntaxKind.NewKeyword:
//                return DeclarationModifiers.New;
            case TokenKind::OverrideKeyword:
                return DeclarationModifiers::Override;
            case TokenKind::ReadOnlyKeyword:
                return DeclarationModifiers::ReadOnly;
            case TokenKind::PartialKeyword:
                return DeclarationModifiers::Partial;
            case TokenKind::RefKeyword:
                return DeclarationModifiers::Ref;
            case TokenKind::IdentifierToken:
                switch (contextualKind) {
                    case TokenKind::PartialKeyword:
                        return DeclarationModifiers::Partial;
                    default:
                        return DeclarationModifiers::None;
                }
            default:
                return DeclarationModifiers::None;
        }
    }

    DeclarationModifiers GetModifier(SyntaxToken token) {
        return GetModifier(token.kind, token.contextualKind);
    }

    SyntaxToken ConvertToKeyword(SyntaxToken token) {
        token.kind = token.contextualKind;
        return token;
    }

    void ParseModifiers(Parser* parser, TokenListBuffer* tokens, bool forAccessors, bool* isPossibleTypeDeclaration) {

        *isPossibleTypeDeclaration = true;

        while (true) {
            DeclarationModifiers newMod = GetModifier(parser->currentToken.kind, parser->currentToken.contextualKind);

            if (newMod == DeclarationModifiers::None) {
                break;
            }

            SyntaxToken modTok;

            switch (newMod) {
                case DeclarationModifiers::Partial: {
                    SyntaxToken nextToken = parser->PeekToken(1);
                    if (IsPartialType(parser) || IsPartialMember(parser)) {
                        // Standard legal cases.
                        modTok = ConvertToKeyword(parser->EatToken());
                    }
                    else if (nextToken.kind == TokenKind::NamespaceKeyword) {
                        // todo -- report error, can't have a partial namespace
                        modTok = ConvertToKeyword(parser->EatToken());
                    }
                    else if (nextToken.kind == TokenKind::EnumKeyword || nextToken.kind == TokenKind::DelegateKeyword || (IsPossibleStartOfTypeDeclaration(nextToken.kind) && GetModifier(nextToken) != DeclarationModifiers::None)) {
                        // todo -- report error, can't have a partial here
                        modTok = ConvertToKeyword(parser->EatToken());
                    }
                    else {
                        return;
                    }
                }
                case DeclarationModifiers::Ref: {
                    // 'ref' is only a modifier if used on a ref struct
                    // it must be either immediately before the 'struct'
                    // keyword, or immediately before 'partial struct' if
                    // this is a partial ref struct declaration

                    if (forAccessors && IsPossibleAccessorModifier(parser)) {
                        modTok = parser->EatToken();
                    }
                    else {
                        return;
                    }

                    break;
                }
                default: {
                    modTok = parser->EatToken();
                    break;
                }

            }

            tokens->Add(modTok);

        }

    }

    MemberDeclarationSyntax* ParseTypeDeclaration(Parser* parser, TokenListBuffer* modifiers) {
        NOT_IMPLEMENTED("ParseTypeDeclaration");
        return nullptr;
    }

    bool ShouldParseSeparatorOrElement(Parser* parser, TokenKind separatorTokenKind, bool allowSemicolonAsSeparator, isPossibleElementFn isPossibleElement) {
        // if we're on a separator, we def should parse it out as such.
        if (parser->currentToken.kind == separatorTokenKind) {
            return true;
        }

        // We're not on a valid separator, but we want to be resilient for the user accidentally using the wrong
        // one in common cases.
        if (allowSemicolonAsSeparator && parser->currentToken.kind == TokenKind::SemicolonToken) {
            return true;
        }

        if (isPossibleElement(parser)) {
            return true;
        }

        return false;
    }

    template<typename TNode>
    SeparatedSyntaxList<TNode>* ParseCommaSeparatedSyntaxList(
        Parser* parser,
        SyntaxToken* openToken,
        TokenKind closeTokenKind,
        isPossibleElementFn isPossibleElement,
        parseElementFn parseElement,
        immediatelyAbortFn immediatelyAbort,
        skipBadTokensFn skipBadTokens,
        bool allowTrailingSeparator,
        bool requireOneElement,
        bool allowSemicolonAsSeparator) {

        TokenKind separatorTokenKind = TokenKind::CommaToken;
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SeparatedSyntaxListBuilder<TNode> nodes(parser->tempAllocator);
        tryAgain:
        if (requireOneElement || parser->currentToken.kind != closeTokenKind) {
            if (requireOneElement || ShouldParseSeparatorOrElement(parser, separatorTokenKind, allowSemicolonAsSeparator, isPossibleElement)) {
                // first argument
                TNode* node = (TNode*) parseElement(parser);
                nodes.Add(node);

                // now that we've gotten one element, we don't require any more.
                requireOneElement = false;

                // Ensure that if parsing separators/elements doesn't move us forward, that we always bail out from parsing this list.
                int32 lastTokenPosition = -1;

                while (!immediatelyAbort(parser, node) && IsMakingProgress(parser, &lastTokenPosition)) {
                    if (parser->currentToken.kind == closeTokenKind)
                        break;

                    if (ShouldParseSeparatorOrElement(parser, separatorTokenKind, allowSemicolonAsSeparator, isPossibleElement)) {
                        // If we got a semicolon instead of comma, consume it with error and act as if it were a comma.
                        nodes.AddSeparator(parser->currentToken.kind == TokenKind::SemicolonToken
                            ? parser->EatTokenWithPrejudice(separatorTokenKind)
                            : parser->EatToken(separatorTokenKind)
                        );

                        if (allowTrailingSeparator) {
                            // check for exit case after legal trailing comma
                            if (parser->currentToken.kind == closeTokenKind) {
                                break;
                            }
                            else if (!isPossibleElement(parser)) {
                                goto tryAgain;
                            }
                        }

                        node = (TNode*) parseElement(parser);
                        nodes.Add(node);
                        continue;
                    }

                    // Something we didn't recognize, try to skip tokens, reporting that we expected a separator here.
                    if (skipBadTokens(parser, separatorTokenKind, closeTokenKind) == PostSkipAction::Abort) {
                        break;
                    }
                }
            }
            else if (skipBadTokens(parser, TokenKind::IdentifierToken, closeTokenKind) == PostSkipAction::Continue) {
                // Something we didn't recognize, try to skip tokens, reporting that we expected an identifier here.
                // While 'identifier' may not be completely accurate in terms of what the list needs, it's a
                // generally good 'catch all' indicating that some name/expr was needed, where something else
                // invalid was found.
                goto tryAgain;
            }
        }
        return nodes.ToList(parser->allocator);
    }

    template<typename TNode>
    SeparatedSyntaxList<TNode>* ParseCommaSeparatedSyntaxList(
        Parser* parser,
        SyntaxToken* openToken,
        TokenKind closeTokenKind,
        isPossibleElementFn isPossibleElement,
        parseElementFn parseElement,
        skipBadTokensFn skipBadTokens,
        bool allowTrailingSeparator,
        bool requireOneElement,
        bool allowSemicolonAsSeparator) {
        return ParseCommaSeparatedSyntaxList<TNode>(
            parser,
            openToken,
            closeTokenKind,
            isPossibleElement,
            parseElement,
            nullptr,
            skipBadTokens,
            allowTrailingSeparator,
            requireOneElement,
            allowSemicolonAsSeparator
        );
    }


    bool IsPossibleArgumentExpression(Parser* parser) {
        return SyntaxFacts::IsValidArgumentRefKindKeyword(parser->currentToken.kind) || IsPossibleExpression(parser);
    }

    ExpressionSyntax* ParseExpressionOrDeclaration(Parser* parser, ParseTypeMode mode, bool permitTupleDesignation) {
        NOT_IMPLEMENTED("ParseExpressionOrDeclaration");
        return nullptr;
    }

    ExpressionSyntax* ParseThrowExpression(Parser* parser) {
        return parser->CreateNode<ThrowExpressionSyntax>(
            parser->EatToken(TokenKind::ThrowKeyword),
            ParseSubExpression(parser, Precedence::Coalescing)
        );
    }

    bool ScanDesignator(Parser* parser) {
        switch (parser->currentToken.kind) {
            case TokenKind::IdentifierToken: {
                if (!IsTrueIdentifier(parser)) {
                    return false;
                }

                parser->EatToken(); // eat the identifier
                return true;
            }
            case TokenKind::OpenParenToken:
                while (true) {
                    parser->EatToken(); // eat the open paren or comma
                    if (!ScanDesignator(parser)) {
                        return false;
                    }

                    switch (parser->currentToken.kind) {
                        case TokenKind::CommaToken:
                            continue;
                        case TokenKind::CloseParenToken:
                            parser->EatToken(); // eat the close paren
                            return true;
                        default:
                            return false;
                    }
                }
            default:
                return false;
        }
    }

    bool IsPossibleDeconstructionLeft(Parser* parser, Precedence precedence) {
        if (precedence > Precedence::Assignment || !(parser->currentToken.contextualKind == TokenKind::VarKeyword || SyntaxFacts::IsPredefinedType(parser->currentToken.kind))) {
            return false;
        }

        ResetPoint resetPoint(parser);

        parser->EatToken(); // `var`
        return parser->currentToken.kind == TokenKind::OpenParenToken && ScanDesignator(parser) && parser->currentToken.kind == TokenKind::EqualsToken;
    }

    VariableDesignationSyntax* ParseSimpleDesignation(Parser* parser) {
        return parser->currentToken.contextualKind == TokenKind::UnderscoreToken
            ? (VariableDesignationSyntax*) parser->CreateNode<DiscardDesignationSyntax>(parser->EatToken())
            : (VariableDesignationSyntax*) parser->CreateNode<SingleVariableDesignationSyntax>(parser->EatToken(TokenKind::IdentifierToken));
    }

    VariableDesignationSyntax* ParseDesignation(Parser* parser, bool forPattern) {
        VariableDesignationSyntax* result = nullptr;
        if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            SyntaxToken openParen = parser->EatToken();
            TempAllocator::ScopedMarker m(parser->tempAllocator);
            SeparatedSyntaxListBuilder<VariableDesignationSyntax> listOfDesignations(parser->tempAllocator);

            bool done = false;
            if (forPattern) {
                done = parser->currentToken.kind == TokenKind::CloseParenToken;
            }
            else {
                listOfDesignations.Add(ParseDesignation(parser, forPattern));
                listOfDesignations.AddSeparator(parser->EatToken(TokenKind::CommaToken));
            }

            if (!done) {
                while (true) {
                    listOfDesignations.Add(ParseDesignation(parser, forPattern));
                    if (parser->currentToken.kind == TokenKind::CommaToken) {
                        listOfDesignations.AddSeparator(parser->EatToken(TokenKind::CommaToken));
                    }
                    else {
                        break;
                    }
                }
            }

            result = parser->CreateNode<ParenthesizedVariableDesignationSyntax>(openParen, listOfDesignations.ToList(parser->allocator), parser->EatToken(TokenKind::CloseParenToken));
        }
        else {
            result = ParseSimpleDesignation(parser);
        }

        return result;
    }

    ExpressionSyntax* ParseDeclarationExpression(Parser* parser, ParseTypeMode mode, bool isScoped) {
        // todo -- this is probably where we add 'temp' and other lifetime keywords
        TypeSyntax* type = ParseType(parser, mode);
        return parser->CreateNode<DeclarationExpressionSyntax>(type, ParseDesignation(parser, false));
    }

    ExpressionSyntax* ParseTypeOfExpression(Parser* parser) {
        return parser->CreateNode<TypeOfExpressionSyntax>(
            parser->EatToken(),
            parser->EatToken(TokenKind::OpenParenToken),
            ParseTypeOrVoid(parser),
            parser->EatToken(TokenKind::CloseParenToken)
        );
    }

    ExpressionSyntax* ParseDefaultExpression(Parser* parser) {
        SyntaxToken keyword = parser->EatToken();
        if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            return parser->CreateNode<DefaultExpressionSyntax>(
                keyword,
                parser->EatToken(TokenKind::OpenParenToken),
                ParseType(parser),
                parser->EatToken(TokenKind::CloseParenToken));
        }
        else {
            return parser->CreateNode<LiteralExpressionSyntax>(SyntaxKind::DefaultLiteralExpression, keyword);
        }
    }

    ExpressionSyntax* ParseSizeOfExpression(Parser* parser) {
        return parser->CreateNode<SizeOfExpressionSyntax>(
            parser->EatToken(),
            parser->EatToken(TokenKind::OpenParenToken),
            ParseTypeOrVoid(parser),
            parser->EatToken(TokenKind::CloseParenToken)
        );
    }

    ExpressionSyntax* ParseLambdaExpression(Parser* parser) {
        NOT_IMPLEMENTED("ParseLambdaExpression");
        return nullptr;
    }

    ExpressionSyntax* ParseAnonymousMethodExpression(Parser* parser) {
        NOT_IMPLEMENTED("ParseAnonymousMethodExpression");
        return nullptr;
    }

    ExpressionSyntax* TryParseLambdaExpression(Parser* parser) {
        NOT_IMPLEMENTED("TryParseLambdaExpression");
        return nullptr;
    }

    ExpressionSyntax* ParseCollectionExpression(Parser* Parser) {
        NOT_IMPLEMENTED("ParseCollectionExpression");
        return nullptr;
    }

    bool isBinaryPatternKeyword(TokenKind contextualKind) {
        return contextualKind == TokenKind::OrKeyword || contextualKind == TokenKind::AndKeyword;
    }

    bool IsPossibleSubpatternElement(Parser* parser) {

        if (CanStartExpression(parser)) {
            return true;
        }

        TokenKind tk = parser->currentToken.kind;

        return tk == TokenKind::OpenBraceToken ||
            tk == TokenKind::OpenBracketToken ||
            tk == TokenKind::LessThanToken ||
            tk == TokenKind::LessThanEqualsToken ||
            tk == TokenKind::GreaterThanToken ||
            tk == TokenKind::GreaterThanEqualsToken;
    }

    bool IsBinaryPattern(Parser* parser) {
        if (!isBinaryPatternKeyword(parser->currentToken.contextualKind)) {
            return false;
        }
        bool lastTokenIsBinaryOperator = true;

        parser->EatToken();
        while (isBinaryPatternKeyword(parser->currentToken.contextualKind)) {
            // If we see a subsequent binary pattern token, it can't be an operator.
            // Later, it will be parsed as an identifier.
            lastTokenIsBinaryOperator = !lastTokenIsBinaryOperator;
            parser->EatToken();
        }

        // In case a combinator token is used as a constant, we explicitly check that a pattern is NOT followed.
        // Such as `(e is (int)or or >= 0)` versus `(e is (int) or or)`
        return lastTokenIsBinaryOperator == IsPossibleSubpatternElement(parser);
    }

    bool ScanCast(Parser* parser, bool forPattern) {
        if (parser->currentToken.kind != TokenKind::OpenParenToken) {
            return false;
        }
        parser->EatToken();

        ScanTypeFlags type = ScanType(parser, false);

        if (type == ScanTypeFlags::NotType) {
            return false;
        }

        if (parser->currentToken.kind != TokenKind::CloseParenToken) {
            return false;
        }

        parser->EatToken();

        if (forPattern && parser->currentToken.kind == TokenKind::IdentifierToken) {
            // In a pattern, an identifier can follow a cast unless it's a binary pattern token.
            return !IsBinaryPattern(parser);
        }

        switch (type) {
            // If we have any of the following, we know it must be a cast:
            // 1) (Goo*)bar; -- we don't support pointers atm
            // 2) (Goo?)bar;
            // 3) "(int)bar" or "(int[])bar"
            // 4) (G::Goo)bar
            case ScanTypeFlags::PointerOrMultiplication:
            case ScanTypeFlags::NullableType:
            case ScanTypeFlags::MustBeType: {
                if (!forPattern) {
                    return true;
                }
                switch (parser->currentToken.kind) {
                    case TokenKind::PlusToken:
                    case TokenKind::MinusToken:
                    case TokenKind::AmpersandToken: // -- pointers aren't supported but we can parse it I guess
                    case TokenKind::AsteriskToken:
                    case TokenKind::DotDotToken:
                        return true;
                    default:
                        return SyntaxFacts::CanFollowCast(parser->currentToken.kind);
                }
            }
            case ScanTypeFlags::GenericTypeOrMethod:
            case ScanTypeFlags::TupleType:
                // If we have `(X<Y>)[...` then we know this must be a cast of a collection expression, not an index
                // into some expr. As most collections are generic, the common case is not ambiguous.
                //
                // Things are still ambiguous if you have `(X)[...` and we still parse that as
                // indexing into an expression.  The user can still write `(X)([...` in this case though to get cast
                // parsing. As non-generic casts are the rare case for collection expressions, this gives a good balance
                return parser->currentToken.kind == TokenKind::OpenBracketToken || SyntaxFacts::CanFollowCast(parser->currentToken.kind);
            case ScanTypeFlags::GenericTypeOrExpression:
            case ScanTypeFlags::NonGenericTypeOrExpression:
                // if we have `(A)[]` then treat that always as a cast of an empty collection expression.  `[]` is not
                // legal on the RHS in any other circumstances for a parenthesized expr.
                if (parser->currentToken.kind == TokenKind::OpenBracketToken && parser->PeekToken(1).kind == TokenKind::CloseBracketToken) {
                    return true;
                }

                // check for ambiguous type or expression followed by disambiguating token.  i.e.
                //
                // "(A)b" is a cast.  But "(A)+b" is not a cast.
                return SyntaxFacts::CanFollowCast(parser->currentToken.kind);
            default:
                UNREACHABLE("");
                return false;
        }

    }

    ExpressionSyntax* ParseTupleExpressionTail(Parser* parser, SyntaxToken openToken, ArgumentSyntax* firstArg) {
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SeparatedSyntaxListBuilder<ArgumentSyntax> list(parser->tempAllocator);
        list.Add(firstArg);

        while (parser->currentToken.kind == TokenKind::CommaToken) {
            list.AddSeparator(parser->EatToken());

            ExpressionSyntax* expression = ParseExpressionOrDeclaration(parser, ParseTypeMode::AfterTupleComma, true);
            ArgumentSyntax* argument = expression->GetKind() != SyntaxKind::IdentifierName || parser->currentToken.kind != TokenKind::ColonToken
                ? parser->CreateNode<ArgumentSyntax>(nullptr, SyntaxToken(), expression)
                : parser->CreateNode<ArgumentSyntax>(
                    parser->CreateNode<NameColonSyntax>((IdentifierNameSyntax*) expression, parser->EatToken()),
                    SyntaxToken(),
                    ParseExpressionOrDeclaration(parser, ParseTypeMode::AfterTupleComma, true)
                );

            list.Add(argument);
        }

        if (list.itemCount < 2) {
            ExpressionSyntax* expressionSyntax = CreateMissingIdentifierName(parser);

            list.AddSeparator(parser->CreateMissingToken(TokenKind::CommaToken));
            list.Add(parser->CreateNode<ArgumentSyntax>(nullptr, SyntaxToken(), expressionSyntax));
            parser->AddError(expressionSyntax, ErrorCode::ERR_TupleTooFewElements);

        }

        return parser->CreateNode<TupleExpressionSyntax>(openToken, list.ToList(parser->allocator), parser->EatToken(TokenKind::CloseParenToken));
    }

    ExpressionSyntax* ParseCastOrParenExpressionOrTuple(Parser* parser) {
        ResetPoint resetPoint(parser, false);
        // We have a decision to make -- is this a cast, or is it a parenthesized
        // expression?  Because look-ahead is cheap with our token stream, we check
        // to see if this "looks like" a cast (without constructing any parse trees)
        // to help us make the decision.
        if (ScanCast(parser, false)) {

            // Looks like a cast, so parse it as one.
            resetPoint.Reset();

            return parser->CreateNode<CastExpressionSyntax>(
                parser->EatToken(TokenKind::OpenParenToken),
                ParseType(parser),
                parser->EatToken(TokenKind::CloseParenToken),
                ParseSubExpression(parser, Precedence::Cast)
            );

        }
        // Doesn't look like a cast, so parse this as a parenthesized expression or tuple.
        resetPoint.Reset();
        SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);
        ExpressionSyntax* expression = ParseExpressionOrDeclaration(parser, ParseTypeMode::FirstElementOfPossibleTupleLiteral, true);

        //  ( <expr>,    must be a tuple
        if (parser->currentToken.kind == TokenKind::CommaToken) {
            return ParseTupleExpressionTail(parser, openParen, parser->CreateNode<ArgumentSyntax>(nullptr, SyntaxToken(), expression));
        }

        // ( name:
        if (expression->GetKind() == SyntaxKind::IdentifierName && parser->currentToken.kind == TokenKind::ColonToken) {
            NameColonSyntax* nameColonSyntax = parser->CreateNode<NameColonSyntax>((IdentifierNameSyntax*) expression, parser->EatToken());
            return ParseTupleExpressionTail(parser, openParen, parser->CreateNode<ArgumentSyntax>(nameColonSyntax, SyntaxToken(), ParseExpressionOrDeclaration(parser, ParseTypeMode::FirstElementOfPossibleTupleLiteral, true)));
        }

        return parser->CreateNode<ParenthesizedExpressionSyntax>(openParen, expression, parser->EatToken(TokenKind::CloseParenToken));
    }

    bool IsAnonymousType(Parser* parser) {
        return parser->currentToken.kind == TokenKind::NewKeyword && parser->PeekToken(1).kind == TokenKind::OpenBraceToken;
    }

    bool IsNamedAssignment(Parser* parser) {
        return IsTrueIdentifier(parser) && parser->PeekToken(1).kind == TokenKind::EqualsToken;
    }

    NameEqualsSyntax* ParseNameEquals(Parser* parser) {
        return parser->CreateNode<NameEqualsSyntax>(
            parser->CreateNode<IdentifierNameSyntax>(ParseIdentifierToken(parser)),
            parser->EatToken(TokenKind::EqualsToken)
        );
    }

    AnonymousObjectMemberDeclaratorSyntax* ParseAnonymousTypeMemberInitializer(Parser* parser) {
        return parser->CreateNode<AnonymousObjectMemberDeclaratorSyntax>(
            IsNamedAssignment(parser) ? ParseNameEquals(parser) : nullptr,
            ParseExpression(parser)
        );
    }

    PostSkipAction SkipBadInitializerListTokens(Parser* parser, TokenKind expectedKind, TokenKind closeKind) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleExpression(p); },
            [](Parser* p, TokenKind closeKind) { return p->currentToken.kind == closeKind; },
            expectedKind,
            closeKind
        );
    }

    ExpressionSyntax* ParseAnonymousTypeExpression(Parser* parser) {
        SyntaxToken newToken = parser->EatToken(TokenKind::NewKeyword);
        SyntaxToken openBrace = parser->EatToken(TokenKind::OpenBraceToken);

        SeparatedSyntaxList<AnonymousObjectMemberDeclaratorSyntax>* expressions = ParseCommaSeparatedSyntaxList<AnonymousObjectMemberDeclaratorSyntax>(
            parser,
            &openBrace,
            TokenKind::CloseBraceToken,
            [](Parser* p) { return IsPossibleExpression(p); },
            [](Parser* p) { return (SyntaxBase*) ParseAnonymousTypeMemberInitializer(p); },
            &SkipBadInitializerListTokens,
            true, // allowTrailingSeparator
            false, // requireOneElement
            false  // allowSemicolonAsSeparator
        );

        return parser->CreateNode<AnonymousObjectCreationExpressionSyntax>(
            newToken,
            openBrace,
            expressions,
            parser->EatToken(TokenKind::CloseBraceToken)
        );

    }

    bool IsImplicitlyTypedArray(Parser* parser) {
        return parser->PeekToken(1).kind == TokenKind::OpenBracketToken;
    }

    ExpressionSyntax* ParseImplicitlyTypedArrayCreation(Parser* parser) {

        // var x = new[,] { { 1 }, { 2 } };

        SyntaxToken newKeyword = parser->EatToken(TokenKind::NewKeyword);
        SyntaxToken openBracket = parser->EatToken(TokenKind::OpenBracketToken);
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        TokenListBuffer commas(parser->tempAllocator);
        int lastTokenPosition = -1;

        while (IsMakingProgress(parser, &lastTokenPosition)) {

            if (IsPossibleExpression(parser)) {
                ExpressionSyntax* size = ParseExpression(parser);
                parser->AddError(size, ErrorCode::ERR_InvalidArray);
            }

            if (parser->currentToken.kind == TokenKind::CommaToken) {
                commas.Add(parser->EatToken());
                continue;
            }

            break;
        }

        return parser->CreateNode<ImplicitArrayCreationExpressionSyntax>(
            newKeyword,
            openBracket,
            commas.Persist(parser->allocator),
            parser->EatToken(TokenKind::CloseBracketToken),
            ParseArrayInitializer(parser)
        );

    }

    bool IsImplicitObjectCreation(Parser* parser) {
        // The caller is expected to have consumed the new keyword.
        if (parser->currentToken.kind != TokenKind::OpenParenToken) {
            return false;
        }

        ResetPoint resetPoint(parser);
        parser->EatToken(); // open paren

        SyntaxToken unused;
        ScanTypeFlags scanTypeFlags = ScanTupleType(parser, &unused);

        if (scanTypeFlags != ScanTypeFlags::NotType) {
            switch (parser->currentToken.kind) {
                case TokenKind::QuestionToken:    // e.g. `new(a, b)?()`
                case TokenKind::OpenBracketToken: // e.g. `new(a, b)[]`
                case TokenKind::OpenParenToken:   // e.g. `new(a, b)()` for better error recovery
                    return false;
                default:
                    break;
            }
        }

        return true;
    }

    InitializerExpressionSyntax* ParseObjectOrCollectionInitializer(Parser* parser) {
        NOT_IMPLEMENTED("ParseObjectOrCollectionInitializer");
        return nullptr;
    }

    ExpressionSyntax* ParseArrayOrObjectCreationExpression(Parser* parser) {
        SyntaxToken newKeyword = parser->EatToken(TokenKind::NewKeyword);

        TypeSyntax* type = nullptr;
        InitializerExpressionSyntax* initializer = nullptr;

        if (!IsImplicitObjectCreation(parser)) {
            type = ParseType(parser, ParseTypeMode::NewExpression);
            if (type->GetKind() == SyntaxKind::ArrayType) {

                // Check for an initializer.
                if (parser->currentToken.kind == TokenKind::OpenBraceToken) {
                    initializer = ParseArrayInitializer(parser);
                }

                return parser->CreateNode<ArrayCreationExpressionSyntax>(newKeyword, (ArrayTypeSyntax*) type, initializer);
            }
        }

        ArgumentListSyntax* argumentList = nullptr;
        if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            argumentList = ParseParenthesizedArgumentList(parser);
        }

        if (parser->currentToken.kind == TokenKind::OpenBraceToken) {
            initializer = ParseObjectOrCollectionInitializer(parser);
        }
        // we need one or the other. todo -- don't bother reporting this if we already complained about the new type.
        if (argumentList == nullptr && initializer == nullptr) {

            argumentList = parser->CreateNode<ArgumentListSyntax>(
                parser->EatToken(TokenKind::OpenParenToken, ErrorCode::ERR_BadNewExpr),
                parser->allocator->New<SeparatedSyntaxList<ArgumentSyntax>>(0, nullptr, 0, nullptr),
                parser->CreateMissingToken(TokenKind::CloseParenToken)
            );
        }

        return type == nullptr
            ? (ExpressionSyntax*) parser->CreateNode<ImplicitObjectCreationExpressionSyntax>(newKeyword, argumentList, initializer)
            : (ExpressionSyntax*) parser->CreateNode<ObjectCreationExpressionSyntax>(newKeyword, type, argumentList, initializer);

    }

    ExpressionSyntax* ParseNewExpression(Parser* parser) {
        // todo -- some changes needed here for alchemy & dynamic etc
        if (IsAnonymousType(parser)) {
            return ParseAnonymousTypeExpression(parser);
        }
        else if (IsImplicitlyTypedArray(parser)) {
            return ParseImplicitlyTypedArrayCreation(parser);
        }
        else {
            // assume object creation as default case
            return ParseArrayOrObjectCreationExpression(parser);
        }
    }

    ExpressionSyntax* ParseRegularStackAllocExpression(Parser* parser) {
        return parser->CreateNode<StackAllocArrayCreationExpressionSyntax>(
            parser->EatToken(TokenKind::StackAllocKeyword),
            ParseType(parser),
            parser->currentToken.kind == TokenKind::OpenBraceToken ? ParseArrayInitializer(parser) : nullptr
        );
    }

    ExpressionSyntax* ParseImplicitlyTypedStackAllocExpression(Parser* parser) {
        //  var x = stackalloc [] { 1, 2, 3 }
        SyntaxToken stackalloc = parser->EatToken(TokenKind::StackAllocKeyword);
        SyntaxToken openBracket = parser->EatToken(TokenKind::OpenBracketToken);

        int32 lastTokenPosition = -1;
        while (IsMakingProgress(parser, &lastTokenPosition)) {
            if (IsPossibleExpression(parser)) {
                ExpressionSyntax* size = ParseExpression(parser);
                parser->AddError(size, ErrorCode::ERR_InvalidStackAllocArray);
            }

            if (parser->currentToken.kind == TokenKind::CommaToken) {
                SyntaxToken comma = parser->EatToken();
                parser->AddError(comma, ErrorCode::ERR_InvalidStackAllocArray);
                continue;
            }

            break;
        }

        return parser->CreateNode<ImplicitStackAllocArrayCreationExpressionSyntax>(
            stackalloc,
            openBracket,
            parser->EatToken(TokenKind::CloseBracketToken),
            ParseArrayInitializer(parser)
        );

    }

    ExpressionSyntax* ParseStackAllocExpression(Parser* parser) {
        return IsImplicitlyTypedArray(parser)
            ? ParseImplicitlyTypedStackAllocExpression(parser)
            : ParseRegularStackAllocExpression(parser);
    }

    ExpressionSyntax* ParseTermWithoutPostfix(Parser* parser, Precedence precedence) {
        TokenKind tk = parser->currentToken.kind;

        switch (tk) {
            case TokenKind::TypeOfKeyword:
                return ParseTypeOfExpression(parser);
            case TokenKind::DefaultKeyword:
                return ParseDefaultExpression(parser);
            case TokenKind::SizeOfKeyword:
                return ParseSizeOfExpression(parser);
            case TokenKind::EqualsGreaterThanToken:
                return ParseLambdaExpression(parser);
            case TokenKind::StaticKeyword:
                if (IsPossibleAnonymousMethodExpression(parser)) {
                    return ParseAnonymousMethodExpression(parser);
                }
                else if (IsPossibleLambdaExpression(parser, precedence)) {
                    return ParseLambdaExpression(parser);
                }
                else {
                    ExpressionSyntax* missing = CreateMissingIdentifierName(parser);
                    parser->AddError(missing, ErrorCode::ERR_InvalidExprTerm);
                    return missing;
                }
            case TokenKind::IdentifierToken: {
                if (IsTrueIdentifier(parser)) {

                    if (IsPossibleAnonymousMethodExpression(parser)) {
                        return ParseAnonymousMethodExpression(parser);
                    }

                    if (IsPossibleLambdaExpression(parser, precedence)) {
                        ExpressionSyntax* lambda = TryParseLambdaExpression(parser);
                        if (lambda) {
                            return lambda;
                        }
                    }

                    if (IsPossibleDeconstructionLeft(parser, precedence)) {
                        return ParseDeclarationExpression(parser, ParseTypeMode::Normal, false);
                    }

                    return ParseSimpleName(parser, NameOptions::InExpression);

                }
                else {
                    ExpressionSyntax* missing = CreateMissingIdentifierName(parser);
                    parser->AddError(missing, ErrorCode::ERR_InvalidExprTerm);
                    return missing;
                }
            }
            case TokenKind::OpenBracketToken:
                return IsPossibleLambdaExpression(parser, precedence)
                    ? ParseLambdaExpression(parser)
                    : ParseCollectionExpression(parser);

            case TokenKind::ThisKeyword:
                return parser->CreateNode<ThisExpressionSyntax>(parser->EatToken());

            case TokenKind::BaseKeyword: {
                return parser->CreateNode<BaseExpressionSyntax>(parser->EatToken());
            }

            case TokenKind::FalseKeyword:
            case TokenKind::TrueKeyword:
            case TokenKind::NullKeyword:
            case TokenKind::NumericLiteralToken:
            case TokenKind::StringLiteralToken:
            case TokenKind::CharacterLiteralToken:
                return parser->CreateNode<LiteralExpressionSyntax>(SyntaxFacts::GetLiteralExpression(tk), parser->EatToken());
                // todo -- string interpolation
            case TokenKind::OpenParenToken: {
                if (IsPossibleLambdaExpression(parser, precedence)) {
                    ExpressionSyntax* lambda = TryParseLambdaExpression(parser);
                    if (lambda != nullptr) {
                        return lambda;
                    }
                    return ParseCastOrParenExpressionOrTuple(parser);
                }
            }
            case TokenKind::NewKeyword:
                return ParseNewExpression(parser);

            case TokenKind::StackAllocKeyword:
                return ParseStackAllocExpression(parser);

            case TokenKind::DelegateKeyword:
                // check for lambda expression with explicit function pointer return type
                return IsPossibleLambdaExpression(parser, precedence)
                    ? ParseLambdaExpression(parser)
                    : ParseAnonymousMethodExpression(parser);
            case TokenKind::RefKeyword: {
                if (IsPossibleLambdaExpression(parser, precedence)) {
                    return ParseLambdaExpression(parser);
                }
                // ref is not expected to appear in this position.
                SyntaxToken refKeyword = parser->EatToken();
                ExpressionSyntax* expression = parser->CreateNode<RefExpressionSyntax>(refKeyword, ParseExpression(parser));
                parser->AddError(expression, ErrorCode::ERR_InvalidExprTerm);
                return expression;
            }
            default: {
                if (SyntaxFacts::IsPredefinedType(tk)) {
                    if (IsPossibleLambdaExpression(parser, precedence)) {
                        return ParseLambdaExpression(parser);
                    }

                    // check for intrinsic type followed by '.'
                    ExpressionSyntax* expr = parser->CreateNode<PredefinedTypeSyntax>(parser->EatToken());

                    if (parser->currentToken.kind != TokenKind::DotToken || tk == TokenKind::VoidKeyword) {
                        parser->AddError(expr, ErrorCode::ERR_InvalidExprTerm);
                    }

                    return expr;
                }
                else {
                    ExpressionSyntax* expr = CreateMissingIdentifierName(parser);

                    if (tk == TokenKind::EndOfFileToken) {
                        parser->AddError(expr, ErrorCode::ERR_ExpressionExpected);
                    }
                    else {
                        parser->AddError(expr, ErrorCode::ERR_InvalidExprTerm);
                    }

                    return expr;
                }
            }

        }

    }

    bool CanStartConsequenceExpression(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::QuestionToken);
        TokenKind nextTokenKind = parser->PeekToken(1).kind;

        // ?. is always the start of of a consequence expression.
        if (nextTokenKind == TokenKind::DotToken) {
            return true;
        }

        if (nextTokenKind == TokenKind::OpenBracketToken) {
            // could simply be `x?[0]`, or could be `x ? [0] : [1]`.

            // Caller only wants us to parse ?[ how it was originally parsed before collection expressions.
            if (parser->forceConditionalAccessExpression) {
                return true;
            }

            ResetPoint resetPoint(parser);

            // Move past the '?'. Parse what comes next the same way that conditional expressions are parsed.
            parser->EatToken();
            ParsePossibleRefExpression(parser);

            // If we see a colon, then do not parse this as a conditional-access-expression, pop up to the caller
            // and have it reparse this as a conditional-expression instead.
            return parser->currentToken.kind != TokenKind::ColonToken;
        }

        // Anything else is just a normal ? and indicates the start of a ternary expression.
        return false;
    }

    ExpressionSyntax* ParseConsequenceSyntax(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::DotToken || parser->currentToken.kind == TokenKind::OpenBracketToken);
        ExpressionSyntax* expr = nullptr;
        if (parser->currentToken.kind == TokenKind::DotToken) {
            expr = parser->CreateNode<MemberBindingExpressionSyntax>(parser->EatToken(), ParseSimpleName(parser, NameOptions::InExpression));
        }
        else if (parser->currentToken.kind == TokenKind::OpenBracketToken) {
            expr = parser->CreateNode<ElementBindingExpressionSyntax>(ParseBracketedArgumentList(parser));
        }
        else {
            UNREACHABLE("ParseConsequenceSyntax");
            return nullptr;
        }
        while (true) {

            switch (parser->currentToken.kind) {
                case TokenKind::OpenParenToken:
                    expr = parser->CreateNode<InvocationExpressionSyntax>(expr, ParseParenthesizedArgumentList(parser));
                    continue;

                case TokenKind::OpenBracketToken:
                    expr = parser->CreateNode<ElementAccessExpressionSyntax>(expr, ParseBracketedArgumentList(parser));
                    continue;

                case TokenKind::DotToken:
                    expr = parser->CreateNode<MemberAccessExpressionSyntax>(SyntaxKind::SimpleMemberAccessExpression, expr, parser->EatToken(), ParseSimpleName(parser, NameOptions::InExpression));
                    continue;

                case TokenKind::QuestionToken:
                    return !CanStartConsequenceExpression(parser)
                        ? expr
                        : parser->CreateNode<ConditionalAccessExpressionSyntax>(
                            expr,
                            parser->EatToken(),
                            ParseConsequenceSyntax(parser)
                        );

                default:
                    return expr;
            }
        }
    }

    ExpressionSyntax* ParsePostFixExpression(Parser* parser, ExpressionSyntax* expr) {
        while (true) {
            switch (parser->currentToken.kind) {
                case TokenKind::OpenParenToken:
                    expr = parser->CreateNode<InvocationExpressionSyntax>(expr, ParseParenthesizedArgumentList(parser));
                    continue;

                case TokenKind::OpenBracketToken:
                    expr = parser->CreateNode<ElementAccessExpressionSyntax>(expr, ParseBracketedArgumentList(parser));
                    continue;

                case TokenKind::PlusPlusToken:
                case TokenKind::MinusMinusToken:
                    expr = parser->CreateNode<PostfixUnaryExpressionSyntax>(SyntaxFacts::GetPostfixUnaryExpression(parser->currentToken.kind), expr, parser->EatToken());
                    continue;

                case TokenKind::ColonColonToken:
                    if (parser->PeekToken(1).kind == TokenKind::IdentifierToken) {
                        SyntaxToken operatorToken = parser->EatToken();

                        parser->AddError(operatorToken, ErrorCode::ERR_UnexpectedDoubleColon);

                        // replace :: with missing dot and annotate with skipped text "::" and error
                        expr = parser->CreateNode<MemberAccessExpressionSyntax>(
                            SyntaxKind::SimpleMemberAccessExpression,
                            expr,
                            MakeMissingToken(TokenKind::DotToken, parser->currentToken.GetId()),
                            ParseSimpleName(parser, NameOptions::InExpression)
                        );
                    }
                    else {
                        // just some random trailing :: ?
                        parser->EatTokenWithPrejudice(TokenKind::DotToken);
                    }

                    continue;

                case TokenKind::MinusGreaterThanToken:
                    expr = parser->CreateNode<MemberAccessExpressionSyntax>(SyntaxKind::PointerMemberAccessExpression, expr, parser->EatToken(), ParseSimpleName(parser, NameOptions::InExpression));
                    continue;

                case TokenKind::DotToken:
                    // if we have the error situation:
                    //
                    //      expr.
                    //      X Y
                    //
                    // Then we don't want to parse this out as "Expr.X"
                    //
                    // It's far more likely the member access expression is simply incomplete and
                    // there is a new declaration on the next line.
                    if (parser->HasTrailingNewLine(parser->currentToken) &&
                        parser->PeekToken(1).kind == TokenKind::IdentifierToken &&
                        parser->PeekToken(2).contextualKind == TokenKind::IdentifierToken) {
                        SyntaxToken dot = parser->EatToken();
                        IdentifierNameSyntax* missing = CreateMissingIdentifierName(parser);
                        parser->AddError(missing, ErrorCode::ERR_IdentifierExpected);

                        return parser->CreateNode<MemberAccessExpressionSyntax>(
                            SyntaxKind::SimpleMemberAccessExpression,
                            expr,
                            parser->EatToken(),
                            missing
                        );
                    }

                    expr = parser->CreateNode<MemberAccessExpressionSyntax>(SyntaxKind::SimpleMemberAccessExpression, expr, parser->EatToken(), ParseSimpleName(parser, NameOptions::InExpression));
                    continue;

                case TokenKind::QuestionToken:
                    if (CanStartConsequenceExpression(parser)) {
                        expr = parser->CreateNode<ConditionalAccessExpressionSyntax>(
                            expr,
                            parser->EatToken(),
                            ParseConsequenceSyntax(parser)
                        );
                        continue;
                    }

                    return expr;

                case TokenKind::ExclamationToken:
                    expr = parser->CreateNode<PostfixUnaryExpressionSyntax>(SyntaxKind::BangExpression, expr, parser->EatToken());
                    continue;

                default:
                    return expr;
            }
        }
    }

    ExpressionSyntax* ParseTerm(Parser* parser, Precedence precedence) {
        return ParsePostFixExpression(parser, ParseTermWithoutPostfix(parser, precedence));
    }

    PatternSyntax* ParseListPattern(Parser* parser, bool keyword) {
        NOT_IMPLEMENTED("ParseListPattern");
        return nullptr;
    }

    bool LooksLikeTupleArrayType(Parser* parser) {

        if (parser->currentToken.kind != TokenKind::OpenParenToken) {
            return false;
        }

        ResetPoint resetPoint(parser);

        return ScanType(parser, true) != ScanTypeFlags::NotType;
    }

    bool LooksLikeTypeOfPattern(Parser* parser) {
        TokenKind tk = parser->currentToken.kind;
        if (SyntaxFacts::IsPredefinedType(tk)) {
            return true;
        }

        if (tk == TokenKind::IdentifierToken && parser->currentToken.contextualKind != TokenKind::UnderscoreToken &&
            (parser->currentToken.contextualKind != TokenKind::NameOfKeyword || parser->PeekToken(1).kind != TokenKind::OpenParenToken)) {
            return true;
        }

        if (LooksLikeTupleArrayType(parser)) {
            return true;
        }

        // We'll parse the function pointer, but issue an error in semantic analysis
        // if (IsFunctionPointerStart()) {
        //     return true;
        // }

        return false;
    }

    bool IsValidPatternDesignation(Parser* parser, bool whenIsKeyword) {

        if (parser->currentToken.kind != TokenKind::IdentifierToken) {
            return false;
        }

        switch (parser->currentToken.contextualKind) {
            case TokenKind::WhenKeyword:
                return !whenIsKeyword;
            case TokenKind::AndKeyword:
            case TokenKind::OrKeyword: {
                TokenKind tk = parser->PeekToken(1).kind;
                switch (tk) {
                    case TokenKind::CloseBraceToken:
                    case TokenKind::CloseBracketToken:
                    case TokenKind::CloseParenToken:
                    case TokenKind::CommaToken:
                    case TokenKind::SemicolonToken:
                    case TokenKind::QuestionToken:
                    case TokenKind::ColonToken: {
                        return true;
                    }
                    case TokenKind::LessThanEqualsToken:
                    case TokenKind::LessThanToken:
                    case TokenKind::GreaterThanEqualsToken:
                    case TokenKind::GreaterThanToken:
                    case TokenKind::IdentifierToken:
                    case TokenKind::OpenBraceToken:
                    case TokenKind::OpenParenToken:
                    case TokenKind::OpenBracketToken: {
                        // these all can start a pattern
                        return false;
                    }
                    default: {
                        if (SyntaxFacts::IsBinaryExpression(tk)) {
                            return true; // `e is int and && true` is valid code with `and` being a designator
                        }

                        // If the following token could start an expression, it may be a constant pattern after a combinator.
                        ResetPoint resetPoint(parser);
                        parser->EatToken();
                        return CanStartExpression(parser);
                    }
                }
            }
            case TokenKind::UnderscoreToken: // discard is a valid pattern designation
            default:
                return true;
        }

    }

    bool LooksLikeCast(Parser* parser) {
        ResetPoint resetPoint(parser);
        return ScanCast(parser, true);
    }

    bool ConvertTypeToExpression(Parser* parser, TypeSyntax* type, ExpressionSyntax** expr, bool permitTypeArguments = false) {
        // todo -- remove dynamic casting
        if (auto g = dynamic_cast<GenericNameSyntax*>(type)) {
            *expr = g;
            return permitTypeArguments;
        }
        else if (auto s = dynamic_cast<SimpleNameSyntax*>(type)) {
            *expr = s;
            return true;
        }
        else if (auto q = dynamic_cast<QualifiedNameSyntax*>(type)) {
            auto left = q->left;
            auto dotToken = q->dotToken;
            auto right = q->right;

            if (permitTypeArguments || !dynamic_cast<GenericNameSyntax*>(right)) {
                ExpressionSyntax* leftExpr;
                bool leftConverted = ConvertTypeToExpression(parser, left, &leftExpr, true);
                auto newLeft = leftConverted ? leftExpr : left;
                *expr = parser->CreateNode<MemberAccessExpressionSyntax>(SyntaxKind::SimpleMemberAccessExpression, newLeft, dotToken, right);
                return true;
            }
        }

        *expr = nullptr;
        return false;
    }

    SyntaxBase* ConvertPatternToExpressionIfPossible(Parser* parser, PatternSyntax* pattern, bool permitTypeArguments = false) {
        if (pattern->GetKind() == SyntaxKind::ConstantPattern) {
            ConstantPatternSyntax* cp = (ConstantPatternSyntax*) pattern;
            return cp->expression;
        }

        if (pattern->GetKind() == SyntaxKind::TypePattern) {
            TypePatternSyntax* typePattern = (TypePatternSyntax*) pattern;
            ExpressionSyntax* expr = nullptr;
            if (ConvertTypeToExpression(parser, typePattern->type, &expr, permitTypeArguments)) {
                return expr;
            }

        }

        if (pattern->GetKind() == SyntaxKind::DiscardPattern) {
            DiscardPatternSyntax* dp = (DiscardPatternSyntax*) pattern;
            // convert to an identifier
            SyntaxToken id = dp->underscore;
            id.kind = TokenKind::IdentifierToken;
            id.contextualKind = TokenKind::IdentifierToken;
            return parser->CreateNode<IdentifierNameSyntax>(id);
        }

        return pattern;

    }

    SubpatternSyntax* ParseSubpatternElement(Parser* parser) {
        BaseExpressionColonSyntax* exprColon = nullptr;
        PatternSyntax* pattern = ParsePattern(parser, Precedence::Conditional);
        // If there is a colon but it's not preceded by a valid expression, leave it out to parse it as a missing comma
        if (parser->currentToken.kind == TokenKind::ColonToken) {

            SyntaxBase* baseExpr = ConvertPatternToExpressionIfPossible(parser, pattern, true);

            if (dynamic_cast<ExpressionSyntax*>(baseExpr)) {
                ExpressionSyntax* expr = (ExpressionSyntax*) baseExpr;
                SyntaxToken colon = parser->EatToken();
                exprColon = expr->GetKind() == SyntaxKind::IdentifierName
                    ? (BaseExpressionColonSyntax*) parser->CreateNode<NameColonSyntax>((IdentifierNameSyntax*) expr, colon)
                    : (BaseExpressionColonSyntax*) parser->CreateNode<ExpressionColonSyntax>(expr, colon);
                pattern = ParsePattern(parser, Precedence::Conditional);

            }
        }

        return parser->CreateNode<SubpatternSyntax>(exprColon, pattern);

    }

    bool IsParameterModifierExcludingScoped(SyntaxToken token) {
        switch (token.kind) {
            // todo -- temp etc
            case TokenKind::ThisKeyword:
            case TokenKind::RefKeyword:
            case TokenKind::OutKeyword:
            case TokenKind::ParamsKeyword:
            case TokenKind::ReadOnlyKeyword:
                return true;
        }

        return false;
    }

    bool IsPossibleParameter(Parser* parser) {
        switch (parser->currentToken.kind) {
//            case SyntaxKind.OpenBracketToken: // attribute
            case TokenKind::OpenParenToken:   // tuple
                return true;

            case TokenKind::IdentifierToken:
                return IsTrueIdentifier(parser);

            default:
                return IsParameterModifierExcludingScoped(parser->currentToken) || SyntaxFacts::IsPredefinedType(parser->currentToken.kind);
        }
    }

    PostSkipAction SkipBadParameterListTokens(Parser* parser, TokenKind expectedKind, TokenKind closeKind) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleParameter(p); },
            [](Parser* p, TokenKind closeKind) { return p->currentToken.kind == closeKind; },
            expectedKind,
            closeKind
        );
    }

    PostSkipAction SkipBadPatternListTokens(Parser* parser, TokenKind expectedKind, TokenKind closeKind) {
        TokenKind tk = parser->currentToken.kind;

        if (tk == TokenKind::CloseParenToken || tk == TokenKind::CloseBraceToken || tk == TokenKind::CloseBracketToken || tk == TokenKind::SemicolonToken) {
            return PostSkipAction::Abort;
        }

        // `:` is usually treated as incorrect separation token. This helps for error recovery in basic typing scenarios like `{ Prop:$$ Prop1: { ... } }`.
        // However, such behavior isn't much desirable when parsing pattern of a case label in a switch statement. For instance, consider the following example: `case { Prop: { }: case ...`.
        // Normally we would skip second `:` and `case` keyword after it as bad tokens and continue parsing pattern, which produces a lot of noise errors.
        // In order to avoid that and produce single error of missing `}` we exit on unexpected `:` in such cases.
        if ((parser->_termState & TerminatorState::IsExpressionOrPatternInCaseLabelOfSwitchStatement) != 0 && parser->currentToken.kind == TokenKind::ColonToken) {
            return PostSkipAction::Abort;
        }

        // This is pretty much the same as above, but for switch expressions and `=>` and `:` tokens.
        // The reason why we cannot use single flag for both cases is because we want `=>` to be the "exit" token only for switch expressions.
        // Consider the following example: `case (() => 0):`. Normally `=>` is treated as bad separator, so we parse this basically the same as `case ((), 1):`, which is syntactically valid.
        // However, if we treated `=>` as "exit" token, parsing wouldn't consume full case label properly and would produce a lot of noise errors.
        // We can afford `:` to be the exit token for switch expressions because error recovery is already good enough and treats `:` as bad `=>`,
        // meaning that switch expression arm `{ : 1` can be recovered to `{ } => 1` where the closing `}` is missing and instead of `=>` we have `:`.
        if ((parser->_termState & TerminatorState::IsPatternInSwitchExpressionArm) != 0 && parser->currentToken.kind == TokenKind::EqualsGreaterThanToken || parser->currentToken.kind == TokenKind::ColonToken) {
            return PostSkipAction::Abort;
        }

        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleSubpatternElement(p); },
            [](Parser* p, TokenKind closeKind) { return p->currentToken.kind == closeKind || p->currentToken.kind == TokenKind::SemicolonToken; },
            expectedKind,
            closeKind
        );

    }

    PropertyPatternClauseSyntax* ParsePropertyPatternClause(Parser* parser) {
        SyntaxToken openBraceToken = parser->EatToken(TokenKind::OpenBraceToken);
        SeparatedSyntaxList<SubpatternSyntax>* subPatterns = ParseCommaSeparatedSyntaxList<SubpatternSyntax>(
            parser,
            &openBraceToken,
            TokenKind::CloseBraceToken,
            [](Parser* p) { return IsPossibleSubpatternElement(p); },
            [](Parser* p) { return (SyntaxBase*) ParseSubpatternElement(p); },
            &SkipBadPatternListTokens,
            true,
            false,
            false
        );

        return parser->CreateNode<PropertyPatternClauseSyntax>(
            openBraceToken,
            subPatterns,
            parser->EatToken(TokenKind::CloseBraceToken)
        );
    }

    bool TryParsePropertyPatternClause(Parser* parser, PropertyPatternClauseSyntax** propertyPatternClauseResult) {
        if (parser->currentToken.kind == TokenKind::OpenBraceToken) {
            *propertyPatternClauseResult = ParsePropertyPatternClause(parser);
            return true;
        }

        *propertyPatternClauseResult = nullptr;
        return false;
    }

    VariableDesignationSyntax* TryParseSimpleDesignation(Parser* parser, bool whenIsKeyword) {
        return IsTrueIdentifier(parser) && IsValidPatternDesignation(parser, whenIsKeyword)
            ? ParseSimpleDesignation(parser)
            : nullptr;
    }

    PatternSyntax* ParsePatternContinued(Parser* parser, TypeSyntax* type, Precedence precedence, bool whenIsKeyword) {
        if (type != nullptr && type->GetKind() == SyntaxKind::IdentifierName) {
            IdentifierNameSyntax* typeIdentifier = (IdentifierNameSyntax*) type;
            SyntaxToken typeIdentifierToken = typeIdentifier->identifier;
            if (typeIdentifierToken.contextualKind == TokenKind::VarKeyword && (parser->currentToken.kind == TokenKind::OpenParenToken || IsValidPatternDesignation(parser, whenIsKeyword))) {
                // we have a "var" pattern; "var" is not permitted to be a stand-in for a type (or a constant) in a pattern.
                SyntaxToken varToken = ConvertToKeyword(typeIdentifierToken);
                VariableDesignationSyntax* varDesignation = ParseDesignation(parser, true);
                return parser->CreateNode<VarPatternSyntax>(varToken, varDesignation);
            }
        }

        if (parser->currentToken.kind == TokenKind::OpenParenToken && (type != nullptr || !LooksLikeCast(parser))) {
            // It is possible this is a parenthesized (constant) expression.
            // We normalize later.
            SyntaxToken openParenToken = parser->EatToken(TokenKind::OpenParenToken);
            SeparatedSyntaxList<SubpatternSyntax>* subPatterns = ParseCommaSeparatedSyntaxList<SubpatternSyntax>(
                parser,
                &openParenToken,
                TokenKind::CloseParenToken,
                [](Parser* p) { return IsPossibleSubpatternElement(p); },
                [](Parser* p) { return (SyntaxBase*) ParseSubpatternElement(p); },
                &SkipBadPatternListTokens,
                false,
                false,
                false
            );

            SyntaxToken closeParenToken = parser->EatToken(TokenKind::CloseParenToken);
            PropertyPatternClauseSyntax* propertyPatternClause0 = nullptr;
            TryParsePropertyPatternClause(parser, &propertyPatternClause0);
            VariableDesignationSyntax* designation0 = TryParseSimpleDesignation(parser, whenIsKeyword);

            if (type == nullptr && propertyPatternClause0 == nullptr && designation0 == nullptr && subPatterns->itemCount == 1 && subPatterns->separatorCount == 0) {
                SubpatternSyntax* firstSubPattern = subPatterns->items[0];
                assert(firstSubPattern != nullptr);

                // There is an ambiguity between a positional pattern `(` pattern `)`
                // and a constant expression pattern that happens to be parenthesized.
                // we treat such syntax as a parenthesized expression always.
                if (firstSubPattern->expressionColon == nullptr) {
                    PatternSyntax* subpattern = firstSubPattern->pattern;
                    if (subpattern->GetKind() == SyntaxKind::ConstantPattern) {
                        ConstantPatternSyntax* cp = (ConstantPatternSyntax*) subpattern;
                        ExpressionSyntax* expression = parser->CreateNode<ParenthesizedExpressionSyntax>(openParenToken, cp->expression, closeParenToken);
                        expression = ParseExpressionContinued(parser, expression, precedence);
                        return parser->CreateNode<ConstantPatternSyntax>(expression);
                    }
                    else {
                        return parser->CreateNode<ParenthesizedPatternSyntax>(openParenToken, subpattern, closeParenToken);

                    }

                }

            }

            PositionalPatternClauseSyntax* positionalPatternClause = parser->CreateNode<PositionalPatternClauseSyntax>(openParenToken, subPatterns, closeParenToken);
            return parser->CreateNode<RecursivePatternSyntax>(type, positionalPatternClause, propertyPatternClause0, designation0);
        }

        PropertyPatternClauseSyntax* propertyPatternClause = nullptr;
        if (TryParsePropertyPatternClause(parser, &propertyPatternClause)) {
            return parser->CreateNode<RecursivePatternSyntax>(type, nullptr, propertyPatternClause, TryParseSimpleDesignation(parser, whenIsKeyword));
        }

        if (type != nullptr) {
            VariableDesignationSyntax* designation = TryParseSimpleDesignation(parser, whenIsKeyword);
            if (designation != nullptr) {
                return parser->CreateNode<DeclarationPatternSyntax>(type, designation);
            }

            // We normally prefer an expression rather than a type in a pattern.
            ExpressionSyntax* expression = nullptr;
            return ConvertTypeToExpression(parser, type, &expression)
                ? (PatternSyntax*) parser->CreateNode<ConstantPatternSyntax>(ParseExpressionContinued(parser, expression, precedence))
                : (PatternSyntax*) parser->CreateNode<TypePatternSyntax>(type);
        }

        // let the caller fall back to parsing an expression
        return nullptr;

    }

    PatternSyntax* ParsePrimaryPattern(Parser* parser, Precedence precedence, bool afterIs, bool whenIsKeyword) {
        // handle common error recovery situations during typing
        switch (parser->currentToken.kind) {
            case TokenKind::CommaToken:
            case TokenKind::SemicolonToken:
            case TokenKind::CloseBraceToken:
            case TokenKind::CloseParenToken:
            case TokenKind::CloseBracketToken:
            case TokenKind::EqualsGreaterThanToken:
                return parser->CreateNode<ConstantPatternSyntax>(ParseIdentifierName(parser, ErrorCode::ERR_MissingPattern));
            default:
                break;
        }

        if (parser->currentToken.contextualKind == TokenKind::UnderscoreToken) {
            return parser->CreateNode<DiscardPatternSyntax>(parser->EatContextualToken(TokenKind::UnderscoreToken));
        }

        switch (parser->currentToken.kind) {
            default:
                break;
            case TokenKind::OpenBracketToken:
                return ParseListPattern(parser, whenIsKeyword);
            case TokenKind::DotDotToken:
                return parser->CreateNode<SlicePatternSyntax>(
                    parser->EatToken(),
                    IsPossibleSubpatternElement(parser)
                        ? ParsePattern(parser, precedence, false, whenIsKeyword)
                        : nullptr
                );
            case TokenKind::LessThanToken:
            case TokenKind::LessThanEqualsToken:
            case TokenKind::GreaterThanToken:
            case TokenKind::GreaterThanEqualsToken:
            case TokenKind::EqualsEqualsToken:
            case TokenKind::ExclamationEqualsToken:
                // this is a relational pattern.
                assert(precedence < Precedence::Shift);
                return parser->CreateNode<RelationalPatternSyntax>(
                    parser->EatToken(),
                    ParseSubExpression(parser, Precedence::Relational)
                );
        }
        ResetPoint resetPoint(parser, false);

        TypeSyntax* type = nullptr;
        if (LooksLikeTypeOfPattern(parser)) {
            type = ParseType(parser, afterIs ? ParseTypeMode::AfterIs : ParseTypeMode::DefinitePattern);
            if (type->IsMissing() || !SyntaxFacts::CanTokenFollowTypeInPattern(parser->currentToken.kind, precedence)) {
                // either it is not shaped like a type, or it is a constant expression.
                resetPoint.Reset();
                type = nullptr;
            }
        }

        PatternSyntax* pattern = ParsePatternContinued(parser, type, precedence, whenIsKeyword);

        if (pattern != nullptr) {
            return pattern;
        }

        resetPoint.Reset();
        ExpressionSyntax* value = ParseSubExpression(parser, precedence);
        return parser->CreateNode<ConstantPatternSyntax>(value);
    }

    PatternSyntax* ParseNegatedPattern(Parser* parser, Precedence precedence, bool afterIs, bool whenIsKeyword) {
        if (parser->currentToken.contextualKind == TokenKind::NotKeyword) {
            return parser->CreateNode<UnaryPatternSyntax>(
                SyntaxKind::NotPattern,
                ConvertToKeyword(parser->EatToken()),
                ParseNegatedPattern(parser, precedence, afterIs, whenIsKeyword)
            );
        }
        else {
            return ParsePrimaryPattern(parser, precedence, afterIs, whenIsKeyword);
        }
    }

    PatternSyntax* ParseConjunctivePattern(Parser* parser, Precedence precedence, bool afterIs, bool whenIsKeyword) {
        PatternSyntax* result = ParseNegatedPattern(parser, precedence, afterIs, whenIsKeyword);
        while (parser->currentToken.contextualKind == TokenKind::AndKeyword) {
            result = parser->CreateNode<BinaryPatternSyntax>(
                SyntaxKind::AndPattern,
                result,
                ConvertToKeyword(parser->EatToken()),
                ParseNegatedPattern(parser, precedence, afterIs, whenIsKeyword)
            );
        }

        return result;
    }

    PatternSyntax* ParseDisjunctivePattern(Parser* parser, Precedence precedence, bool afterIs, bool whenIsKeyword) {
        PatternSyntax* result = ParseConjunctivePattern(parser, precedence, afterIs, whenIsKeyword);
        // todo -- just make and & or reserved keywords
        while (parser->currentToken.contextualKind == TokenKind::OrKeyword) {
            result = parser->CreateNode<BinaryPatternSyntax>(
                SyntaxKind::OrPattern,
                result,
                ConvertToKeyword(parser->EatToken()),
                ParseConjunctivePattern(parser, precedence, afterIs, whenIsKeyword)
            );
        }

        return result;
    }

    PatternSyntax* ParsePattern(Parser* parser, Precedence precedence, bool afterIs, bool whenIsKeyword) {
        return ParseDisjunctivePattern(parser, precedence, afterIs, whenIsKeyword);
    }

    ExpressionSyntax* ParseTypeOrPatternForIsOperator(Parser* parser) {
        PatternSyntax* pattern = ParsePattern(parser, SyntaxFacts::GetPrecedence(SyntaxKind::IsPatternExpression), true);

        // return pattern switch
        // {
        //     ConstantPatternSyntax cp when ConvertExpressionToType(cp.Expression, out NameSyntax? type) => type,
        //         TypePatternSyntax tp => tp.Type,
        //         DiscardPatternSyntax dp => _syntaxFactory.IdentifierName(ConvertToIdentifier(dp.UnderscoreToken)),
        //         var p => p,
        // };
        return nullptr;
    }

    ExpressionSyntax* ParseIsExpression(Parser* parser, ExpressionSyntax* leftOperand, SyntaxToken opToken) {

        ExpressionSyntax* node = ParseTypeOrPatternForIsOperator(parser);

        if (SyntaxFacts::IsPatternSyntax(node->GetKind())) {
            return parser->CreateNode<IsPatternExpressionSyntax>(leftOperand, opToken, (PatternSyntax*) node);
        }

        if (SyntaxFacts::IsTypeSyntax(node->GetKind())) {
            return parser->CreateNode<BinaryExpressionSyntax>(SyntaxKind::IsExpression, leftOperand, opToken, (TypeSyntax*) node);
        }

        UNREACHABLE("ParseIsExpression");
        return nullptr;
    }

    ExpressionSyntax* ParseSwitchExpression(Parser* parser, ExpressionSyntax* governingExpression, SyntaxToken switchKeyword) {
        NOT_IMPLEMENTED("ParseSwitchExpression");
        return nullptr;
    }

    ExpressionSyntax* ParseWithExpression(Parser* parser, ExpressionSyntax* receiverExpression, SyntaxToken withKeyword) {
        NOT_IMPLEMENTED("ParseWithExpression");
        return nullptr;
    }

    bool ContainsTernaryCollectionToReinterpret(Parser* parser, ExpressionSyntax* expression) {

        Alchemy::FixedPodList<SyntaxBase*> stack;

        stack.Add(expression);

        while (stack.size > 0) {

            SyntaxBase* current = stack.Pop();

            NOT_IMPLEMENTED("ContainsTernaryCollectionToReinterpret");
//            if (current is ConditionalExpressionSyntax conditionalExpression && conditionalExpression.WhenTrue.GetFirstToken().Kind == SyntaxKind.OpenBracketToken) {
//                return true;
//            }
//
//            // Note: we could consider not recursing into anonymous-methods/lambdas (since we reset the
//            // ForceConditionalAccessExpression flag when we go into that).  However, that adds a bit of
//            // fragile coupling between these different code blocks that i'd prefer to avoid.  In practice
//            // the extra cost here will almost never occur, so the simplicity is worth it.
//            foreach (var child in current.ChildNodesAndTokens()) {
//                stack.Push(child);
//            }

        }

        return false;
    }

    ExpressionSyntax* ParseExpressionContinued(Parser* parser, ExpressionSyntax* leftOperand, Precedence precedence) {
        while (true) {
            // We either have a binary or assignment operator here, or we're finished.
            TokenKind tk = parser->currentToken.contextualKind;

            bool isAssignmentOperator = false;
            SyntaxKind opKind;

            if (SyntaxFacts::IsBinaryExpression(tk)) {
                opKind = SyntaxFacts::GetBinaryExpression(tk);
            }
            else if (SyntaxFacts::IsAssignmentExpressionOperatorToken(tk)) {
                opKind = SyntaxFacts::GetAssignmentExpression(tk);
                isAssignmentOperator = true;
            }
            else if (tk == TokenKind::DotDotToken) {
                opKind = SyntaxKind::RangeExpression;
            }
            else if (tk == TokenKind::SwitchKeyword && parser->PeekToken(1).kind == TokenKind::OpenBraceToken) {
                opKind = SyntaxKind::SwitchExpression;
            }
            else if (tk == TokenKind::WithKeyword && parser->PeekToken(1).kind == TokenKind::OpenBraceToken) {
                //https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/operators/with-expression
                // todo -- we'll keep this feature but change the keyword used to 'new' maybe
                opKind = SyntaxKind::WithExpression;
            }
            else {
                break;
            }

            Precedence newPrecedence = SyntaxFacts::GetPrecedence(opKind);
            // check for >>, >>=, >>> or >>>=, // check to see if they really are adjacent
            int32 tokensToCombine = 1;
            SyntaxToken peek1 = parser->PeekToken(1);
            SyntaxToken peek2 = parser->PeekToken(2);
            if (tk == TokenKind::GreaterThanToken && peek1.kind == TokenKind::GreaterThanToken || peek1.kind == TokenKind::GreaterThanEqualsToken && parser->NoTriviaBetween(parser->currentToken, peek1)) {
                if (peek1.kind == TokenKind::GreaterThanToken) {
                    if (peek2.kind == TokenKind::GreaterThanToken || peek2.kind == TokenKind::GreaterThanEqualsToken && parser->NoTriviaBetween(peek1, peek2)) {
                        if (peek2.kind == TokenKind::GreaterThanToken) {
                            opKind = SyntaxFacts::GetBinaryExpression(TokenKind::GreaterThanGreaterThanGreaterThanToken);
                        }
                        else {
                            opKind = SyntaxFacts::GetAssignmentExpression(TokenKind::GreaterThanGreaterThanGreaterThanEqualsToken);
                            isAssignmentOperator = true;
                        }

                        tokensToCombine = 3;
                    }
                    else {
                        opKind = SyntaxFacts::GetBinaryExpression(TokenKind::GreaterThanGreaterThanToken);
                        tokensToCombine = 2;
                    }
                }
                else {
                    opKind = SyntaxFacts::GetAssignmentExpression(TokenKind::GreaterThanGreaterThanEqualsToken);
                    isAssignmentOperator = true;
                    tokensToCombine = 2;
                }

                newPrecedence = SyntaxFacts::GetPrecedence(opKind);
            }
            // Check the precedence to see if we should "take" this operator
            if (newPrecedence < precedence) {
                break;
            }

            // Same precedence, but not right-associative -- deal with this "later"
            if ((newPrecedence == precedence) && !SyntaxFacts::IsRightAssociative(opKind)) {
                break;
            }

            // We'll "take" this operator, as precedence is tentatively OK.
            SyntaxToken opToken = parser->EatContextualToken(tk);
            Precedence leftPrecedence = SyntaxFacts::GetPrecedence(leftOperand->GetKind());
            if (newPrecedence > leftPrecedence) {
                // Normally, a left operand with a looser precedence will consume all right operands that
                // have a tighter precedence.  For example, in the expression `a + b * c`, the `* c` part
                // will be consumed as part of the right operand of the addition.  However, there are a
                // few circumstances in which a tighter precedence is not consumed: that occurs when the
                // left hand operator does not have an expression as its right operand.  This occurs for
                // the is-type operator and the is-pattern operator.  Source text such as
                // `a is {} + b` should produce a syntax error, as parsing the `+` with an `is`
                // expression as its left operand would be a precedence inversion.  Similarly, it occurs
                // with an anonymous method expression or a lambda expression with a block body.  No
                // further parsing will find a way to fix things up, so we accept the operator but issue
                // a diagnostic.
                ErrorCode errorCode = leftOperand->GetKind() == SyntaxKind::IsPatternExpression ? ErrorCode::ERR_UnexpectedToken : ErrorCode::WRN_PrecedenceInversion;
                parser->AddError(opToken, errorCode);
            }

            // Combine tokens into a single token if needed
            if (tokensToCombine == 2) {
                SyntaxToken opToken2 = parser->EatToken();
                TokenKind kind = opToken2.kind == TokenKind::GreaterThanToken ? TokenKind::GreaterThanGreaterThanToken : TokenKind::GreaterThanGreaterThanEqualsToken;
                // todo -- we're basically upgrading this token, and skipping the following one
                // we likely want to flag it as combined somehow but I'm not sure how.
                // maybe we just check later for kind == GreaterThanGreaterThanToken ||  GreaterThanGreaterThanEqualsToken
                // and do a post process on it. the token in our token array won't match our copy here
                opToken.kind = kind;
            }
            else if (tokensToCombine == 3) {
                SyntaxToken opToken2 = parser->EatToken();
                assert(opToken2.kind == TokenKind::GreaterThanToken);
                opToken2 = parser->EatToken();
                TokenKind kind = opToken2.kind == TokenKind::GreaterThanToken ? TokenKind::GreaterThanGreaterThanGreaterThanToken : TokenKind::GreaterThanGreaterThanGreaterThanEqualsToken;
                // see above comment
                opToken.kind = kind;
            }
            else if (tokensToCombine != 1) {
                UNREACHABLE("unexpected tokens to combine");
            }
            if (opKind == SyntaxKind::AsExpression) {
                TypeSyntax* type = ParseType(parser, ParseTypeMode::AsExpression);
                leftOperand = parser->CreateNode<BinaryExpressionSyntax>(opKind, leftOperand, opToken, type);
            }
            else if (opKind == SyntaxKind::IsExpression) {
                leftOperand = ParseIsExpression(parser, leftOperand, opToken);
            }
            else if (isAssignmentOperator) {
                ExpressionSyntax* rhs = nullptr;

                // check for lambda expression with explicit ref return type: `ref int () => { ... }`
                if (opKind == SyntaxKind::SimpleAssignmentExpression && parser->currentToken.kind == TokenKind::RefKeyword && !IsPossibleLambdaExpression(parser, newPrecedence)) {
                    rhs = parser->CreateNode<RefExpressionSyntax>(
                        parser->EatToken(),
                        ParseExpression(parser)
                    );
                }
                else {
                    rhs = ParseSubExpression(parser, newPrecedence);
                }

                leftOperand = parser->CreateNode<AssignmentExpressionSyntax>(opKind, leftOperand, opToken, rhs);
            }
            else if (opKind == SyntaxKind::SwitchExpression) {
                leftOperand = ParseSwitchExpression(parser, leftOperand, opToken);
            }
            else if (opKind == SyntaxKind::WithExpression) { // todo -- change this keyword name
                leftOperand = ParseWithExpression(parser, leftOperand, opToken);
            }
            else if (tk == TokenKind::DotDotToken) {
                // Operator ".." here can either be a binary or a postfix unary operator:
                assert(opKind == SyntaxKind::RangeExpression);

                ExpressionSyntax* rightOperand;
                if (CanStartExpression(parser)) {
                    newPrecedence = SyntaxFacts::GetPrecedence(opKind);
                    rightOperand = ParseSubExpression(parser, newPrecedence);
                }
                else {
                    rightOperand = nullptr;
                }

                leftOperand = parser->CreateNode<RangeExpressionSyntax>(leftOperand, opToken, rightOperand);
            }
            else {
                assert(SyntaxFacts::IsBinaryExpression(tk));
                leftOperand = parser->CreateNode<BinaryExpressionSyntax>(opKind, leftOperand, opToken, ParseSubExpression(parser, newPrecedence));
            }
        }

        // From the grammar spec:
        //
        // conditional-expression:
        //  null-coalescing-expression
        //  null-coalescing-expression   ?   expression   :   expression
        //
        // Only take the conditional if we're at or below its precedence.
        if (parser->currentToken.kind != TokenKind::QuestionToken || precedence > Precedence::Conditional) {
            return leftOperand;
        }

        // Complex ambiguity with `?` and collection-expressions.  Specifically: b?[c]:d
        //
        // On its own, we want that to be a conditional expression with a collection expression in it.  However, for
        // back compat, we need to make sure that `a ? b?[c] : d` sees the inner `b?[c]` as a
        // conditional-access-expression.  So, if after consuming the portion after the initial `?` if we do not
        // have the `:` we need, and we can see a `?[` in that portion of the parse, then we retry consuming the
        // when-true portion, but this time forcing the prior way of handling `?[`.
        SyntaxToken questionToken = parser->EatToken();

        ResetPoint afterQuestionToken(parser, false);
        ExpressionSyntax* whenTrue = ParsePossibleRefExpression(parser);

        if (parser->currentToken.kind != TokenKind::ColonToken && !parser->forceConditionalAccessExpression && ContainsTernaryCollectionToReinterpret(parser, whenTrue)) {
            // Keep track of where we are right now in case the new parse doesn't make things better.
            ResetPoint originalAfterWhenTrue(parser, false);

            // Go back to right after the `?`
            afterQuestionToken.Reset();

            // try reparsing with `?[` as a conditional access, not a ternary+collection
            parser->forceConditionalAccessExpression = true;
            ExpressionSyntax* newWhenTrue = ParsePossibleRefExpression(parser);
            parser->forceConditionalAccessExpression = false;

            if (parser->currentToken.kind == TokenKind::ColonToken) {
                // if we now are at a colon, this was preferred parse.
                whenTrue = newWhenTrue;
            }
            else {
                // retrying the parse didn't help.  Use the original interpretation.
                originalAfterWhenTrue.Reset();
            }
        }

        // " $something#f3" " ${xyz#f3}
        // todo -- we don't do the same thing with end of file here, we pre-tokenize the whole file up front. we also probably pick a better token then : for formatting like #
//        if (parser->currentToken.kind == TokenKind::EndOfFileToken && this.lexer.InterpolationFollowedByColon) {
//            // We have an interpolated string with an interpolation that contains a conditional expression.
//            // Unfortunately, the precedence demands that the colon is considered to signal the start of the
//            // format string. Without this code, the compiler would complain about a missing colon, and point
//            // to the colon that is present, which would be confusing. We aim to give a better error message.
//            leftOperand = _syntaxFactory.ConditionalExpression(
//                leftOperand,
//                questionToken,
//                whenTrue,
//                SyntaxFactory.MissingToken(SyntaxKind.ColonToken),
//                _syntaxFactory.IdentifierName(SyntaxFactory.MissingToken(SyntaxKind.IdentifierToken)));
//            return this.AddError(leftOperand, ErrorCode.ERR_ConditionalInInterpolation);
//        }
//        else {
        return parser->CreateNode<ConditionalExpressionSyntax>(
            leftOperand,
            questionToken,
            whenTrue,
            parser->EatToken(TokenKind::ColonToken),
            ParsePossibleRefExpression(parser)
        );
//        }
    }

    ExpressionSyntax* ParseSubExpression(Parser* parser, Precedence precedence) {
        ExpressionSyntax* leftOperand;
        Precedence newPrecedence = Precedence::Expression;
        // all of these are tokens that start statements and are invalid
        // to start a expression with. if we see one, then we must have
        // something like:
        //
        // return
        // if (...
        // parse out a missing name node for the expression, and keep on going
        TokenKind tk = parser->currentToken.kind;
        if (SyntaxFacts::IsInvalidSubExpression(tk)) {
            ExpressionSyntax* missing = CreateMissingIdentifierName(parser);
            parser->AddError(missing, ErrorCode::ERR_InvalidExprTerm);
            return missing;
        }

        // Parse a left operand -- possibly preceded by a unary operator.
        if (SyntaxFacts::IsExpectedPrefixUnaryOperator(tk)) {
            SyntaxKind opKind = SyntaxFacts::GetPrefixUnaryExpression(tk);
            newPrecedence = SyntaxFacts::GetPrecedence(opKind);
            SyntaxToken opToken = parser->EatToken();
            ExpressionSyntax* operand = ParseSubExpression(parser, newPrecedence);
            leftOperand = parser->CreateNode<PrefixUnaryExpressionSyntax>(opKind, opToken, operand);
        }
        else if (tk == TokenKind::DotDotToken) {
            // Operator ".." here can either be a prefix unary operator or a stand alone empty range:
            SyntaxToken opToken = parser->EatToken();
            newPrecedence = SyntaxFacts::GetPrecedence(SyntaxKind::RangeExpression);

            ExpressionSyntax* rightOperand = nullptr;
            if (CanStartExpression(parser)) {
                rightOperand = ParseSubExpression(parser, newPrecedence);
            }

            leftOperand = parser->CreateNode<RangeExpressionSyntax>(nullptr, opToken, rightOperand);
        }
        else if (tk == TokenKind::ThrowKeyword) {
            ExpressionSyntax* result = ParseThrowExpression(parser);
            // we parse a throw expression even at the wrong precedence for better recovery
            if (precedence > Precedence::Coalescing) {
                parser->AddError(result, ErrorCode::ERR_InvalidExprTerm);
            }

            return result;
        }
        if (IsPossibleDeconstructionLeft(parser, precedence)) {
            leftOperand = ParseDeclarationExpression(parser, ParseTypeMode::Normal, false);
        }
        else {
            // Not a unary operator - get a primary expression.
            leftOperand = ParseTerm(parser, precedence);
        }

        return ParseExpressionContinued(parser, leftOperand, precedence);
    }

    ArgumentSyntax* ParseArgumentExpression(Parser* parser, bool isIndexer) {
        NameColonSyntax* nameColon = parser->currentToken.kind == TokenKind::IdentifierToken && parser->PeekToken(1).kind == TokenKind::ColonToken
            ? parser->CreateNode<NameColonSyntax>(
                ParseIdentifierName(parser),
                parser->EatToken(TokenKind::ColonToken))
            : nullptr;

        SyntaxToken refKindKeyword;

        // check for lambda expression with explicit ref return type: `ref int () => { ... }`
        if (SyntaxFacts::IsValidArgumentRefKindKeyword(parser->currentToken.kind) && !(parser->currentToken.kind == TokenKind::RefKeyword && IsPossibleLambdaExpression(parser, Precedence::Expression))) {
            refKindKeyword = parser->EatToken();
        }

        ExpressionSyntax* expression;

        if (isIndexer && (parser->currentToken.kind == TokenKind::CommaToken || parser->currentToken.kind == TokenKind::CloseBracketToken)) {
            expression = ParseIdentifierName(parser, ErrorCode::ERR_ValueExpected);
        }
        else if (parser->currentToken.kind == TokenKind::CommaToken) {
            expression = ParseIdentifierName(parser, ErrorCode::ERR_MissingArgument);
        }
        else {

            // C# doesn't support out/ref for indexers, we do though
            expression = refKindKeyword.IsValid() && refKindKeyword.kind == TokenKind::OutKeyword
                ? ParseExpressionOrDeclaration(parser, ParseTypeMode::Normal, false)
                : ParseSubExpression(parser, Precedence::Expression);
        }

        return parser->CreateNode<ArgumentSyntax>(nameColon, refKindKeyword, expression);

    }

    PostSkipAction SkipBadArgumentListTokens(Parser* parser, TokenKind expectedKind, TokenKind closeKind) {
        TokenKind tk = parser->currentToken.kind;

        if (tk == TokenKind::CloseParenToken || tk == TokenKind::CloseBracketToken || tk == TokenKind::SemicolonToken) {
            return PostSkipAction::Abort;
        }

        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleArgumentExpression(p); },
            [](Parser* p, TokenKind closeKind) { return p->currentToken.kind == closeKind || p->currentToken.kind == TokenKind::SemicolonToken; },
            expectedKind,
            closeKind
        );
    }

    void ParseArgumentList(Parser* parser, TokenKind openKind, TokenKind closeKind, SyntaxToken* openToken, SeparatedSyntaxList<ArgumentSyntax>** arguments, SyntaxToken* closeToken) {
        assert(openKind == TokenKind::OpenParenToken || openKind == TokenKind::OpenBracketToken);
        assert(closeKind == TokenKind::CloseParenToken || openKind == TokenKind::CloseBracketToken);
        assert((openKind == TokenKind::OpenParenToken) == (closeKind == TokenKind::CloseParenToken));

        // convert `[` into `(` or vice versa for error recovery
        *openToken = parser->currentToken.kind == TokenKind::OpenParenToken || parser->currentToken.kind == TokenKind::OpenBracketToken
            ? parser->EatTokenAsKind(openKind)
            : parser->EatToken(openKind);

        TerminatorState saveTerm = parser->_termState;
        parser->_termState |= TerminatorState::IsEndOfArgumentList;
        bool isIndexer = openKind == TokenKind::OpenBracketToken;

        if (parser->currentToken.kind != closeKind && parser->currentToken.kind != TokenKind::SemicolonToken) {
            if (isIndexer) {
                // An indexer always expects at least one value.
                *arguments = ParseCommaSeparatedSyntaxList<ArgumentSyntax>(
                    parser,
                    openToken,
                    TokenKind::CloseBracketToken,
                    &IsPossibleArgumentExpression,
                    [](Parser* p) { return (SyntaxBase*) ParseArgumentExpression(p, true); },
                    &SkipBadArgumentListTokens,
                    false, // allowTrailingSeparator
                    false, // requireOneElement
                    false  // allowSemicolonAsSeparator
                );
            }
            else {
                *arguments = ParseCommaSeparatedSyntaxList<ArgumentSyntax>(
                    parser,
                    openToken,
                    TokenKind::CloseParenToken,
                    &IsPossibleArgumentExpression,
                    [](Parser* p) { return (SyntaxBase*) ParseArgumentExpression(p, false); },
                    &SkipBadArgumentListTokens,
                    false, // allowTrailingSeparator
                    false, // requireOneElement
                    false  // allowSemicolonAsSeparator
                );
            }
        }
        else if (isIndexer && parser->currentToken.kind == closeKind) {
            // An indexer always expects at least one value. And so we need to give an error
            // for the case where we see only "[]". ParseArgumentExpression gives it.

            SeparatedSyntaxListBuilder<ArgumentSyntax> list(parser->tempAllocator);
            list.Add(ParseArgumentExpression(parser, isIndexer));
            *arguments = list.ToList(parser->allocator);

        }
        else {
            *arguments = nullptr;
        }
    }

    ArgumentListSyntax* ParseParenthesizedArgumentList(Parser* parser) {

        SyntaxToken openToken;
        SyntaxToken closeToken;
        SeparatedSyntaxList<ArgumentSyntax>* argumentList = nullptr;

        ParseArgumentList(parser, TokenKind::OpenParenToken, TokenKind::CloseParenToken, &openToken, &argumentList, &closeToken);

        return parser->CreateNode<ArgumentListSyntax>(openToken, argumentList, closeToken);
    }

    ConstructorInitializerSyntax* ParseConstructorInitializer(Parser* parser) {
        SyntaxToken colon = parser->EatToken(TokenKind::ColonToken);

        bool reportError = true;
        SyntaxKind kind = parser->currentToken.kind == TokenKind::BaseKeyword
            ? SyntaxKind::BaseConstructorInitializer
            : SyntaxKind::ThisConstructorInitializer;

        SyntaxToken token;
        if (parser->currentToken.kind == TokenKind::BaseKeyword) {
            kind = SyntaxKind::BaseConstructorInitializer;
            token = parser->EatToken();
        }
        else if (parser->currentToken.kind == TokenKind::ThisKeyword) {
            kind = SyntaxKind::ThisConstructorInitializer;
            token = parser->EatToken();
        }
        else if (parser->currentToken.kind == TokenKind::IdentifierToken) {
            kind = SyntaxKind::NamedConstructorInitializer;
            if (SyntaxFacts::IsReservedKeyword(parser->currentToken.kind)) {
                // todo -- error
            }

            token = parser->EatToken();
        }
        else {
            token = parser->EatToken(TokenKind::ThisKeyword, ErrorCode::ERR_ThisOrBaseOrConstructorNameExpected);
            // No need to report further errors at this point:
            reportError = false;
        }

        ArgumentListSyntax* argumentList = parser->currentToken.kind == TokenKind::OpenParenToken
            ? ParseParenthesizedArgumentList(parser)
            : parser->CreateNode<ArgumentListSyntax>(
                parser->EatToken(TokenKind::OpenParenToken),
                nullptr,
                parser->CreateMissingToken(TokenKind::CloseParenToken)
            );

        if (kind == SyntaxKind::BaseConstructorInitializer) {
            return parser->CreateNode<BaseConstructorInitializerSyntax>(colon, token, argumentList);
        }
        else if (kind == SyntaxKind::ThisConstructorInitializer) {
            return parser->CreateNode<ThisConstructorInitializerSyntax>(colon, token, argumentList);
        }
        else {
            return parser->CreateNode<NamedConstructorInitializerSyntax>(colon, token, argumentList);
        }

    }

    void ParseStatements(Parser* Parser, SyntaxListBuilder<StatementSyntax>* statements, bool stopOnSwitchSections) {
        NOT_IMPLEMENTED("ParseStatements");
    }

    // Used to parse the block-body for a method or accessor.  For blocks that appear *inside* method bodies, call ParseBlock
    BlockSyntax* ParseMethodOrAccessorBodyBlock(Parser* parser, bool isAccessorBody) {
        // There's a special error code for a missing token after an accessor keyword
        SyntaxToken openBrace;
        if (isAccessorBody && parser->currentToken.kind != TokenKind::OpenBraceToken) {
            openBrace = MakeMissingToken(TokenKind::OpenBraceToken, parser->currentToken.GetId());
            parser->AddError(openBrace, ErrorCode::ERR_SemiOrLBraceOrArrowExpected);
        }
        else {
            parser->EatToken(TokenKind::OpenBraceToken);
        }

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<StatementSyntax> statements(parser->tempAllocator);
        ParseStatements(parser, &statements, false);

        return parser->CreateNode<BlockSyntax>(openBrace, statements.ToSyntaxList(parser->allocator), parser->EatToken(TokenKind::CloseBraceToken));
    }

    ExpressionSyntax* ParsePossibleRefExpression(Parser* parser) {
        // check for lambda expression with explicit ref return type: `ref int () => { ... }`
        SyntaxToken refKeyword = parser->currentToken.kind == TokenKind::RefKeyword && !IsPossibleLambdaExpression(parser, Precedence::Expression)
            ? parser->EatToken()
            : SyntaxToken();

        ExpressionSyntax* expression = ParseExpression(parser);
        return !refKeyword.IsValid()
            ? expression
            : parser->CreateNode<RefExpressionSyntax>(refKeyword, expression);
    }

    ArrowExpressionClauseSyntax* ParseArrowExpressionClause(Parser* parser) {
        return parser->CreateNode<ArrowExpressionClauseSyntax>(parser->EatToken(TokenKind::EqualsGreaterThanToken), ParsePossibleRefExpression(parser));
    }

    void ParseBlockAndExpressionBodiesWithSemicolon(Parser* parser, BlockSyntax** blockBody, ArrowExpressionClauseSyntax** expressionBody, SyntaxToken* semicolon, bool parseSemicolonAfterBlock = true) {
        // Check for 'forward' declarations with no block of any kind
        if (parser->currentToken.kind == TokenKind::SemicolonToken) {
            *blockBody = nullptr;
            *expressionBody = nullptr;
            *semicolon = parser->EatToken(TokenKind::SemicolonToken);
            return;
        }

        *blockBody = parser->currentToken.kind == TokenKind::OpenBraceToken
            ? ParseMethodOrAccessorBodyBlock(parser, false)
            : nullptr;

        *expressionBody = parser->currentToken.kind == TokenKind::EqualsGreaterThanToken
            ? ParseArrowExpressionClause(parser)
            : nullptr;

        // Expression-bodies need semicolons and native behavior
        // expects a semicolon if there is no body
        if (*expressionBody != nullptr || *blockBody == nullptr) {
            *semicolon = parser->EatToken(TokenKind::SemicolonToken);
        }
            // Check for bad semicolon after block body
        else if (parseSemicolonAfterBlock && parser->currentToken.kind == TokenKind::SemicolonToken) {
            parser->AddError(parser->currentToken, ErrorCode::ERR_UnexpectedSemicolon);
            *semicolon = parser->EatToken();
        }
        else {
            *semicolon = SyntaxToken();
        }
    }

    MemberDeclarationSyntax* ParseConstructorDeclaration(Parser* parser, TokenListBuffer* modifiers) {

        // we may end up supporting both C# style construction as well as 'constructor' name syntax

        bool isCtorKeyword = parser->currentToken.kind == TokenKind::ConstructorKeyword;

        if (isCtorKeyword) {
            parser->EatToken();
            // look for a name or a (
            if (parser->currentToken.kind == TokenKind::IdentifierToken) {

            }
            else if (parser->currentToken.kind == TokenKind::OpenParenToken) {

            }
            else {
                // missing ( maybe? not sure
            }
        }
        else {
            SyntaxToken name = ParseIdentifierToken(parser);

        }

        TerminatorState saveTerm = parser->_termState;
        parser->_termState |= TerminatorState::IsEndOfMethodSignature;
        ParameterListSyntax* paramList = ParseParenthesizedParameterList(parser);
        ConstructorInitializerSyntax* initializer = parser->currentToken.kind == TokenKind::ColonToken
            ? ParseConstructorInitializer(parser)
            : nullptr;

//        ParseBlockAndExpressionBodiesWithSemicolon(out
//        var
//            body, out
//        var
//            expressionBody, out
//        var
//        semicolon);

        parser->_termState = saveTerm;
        return nullptr;
//        return _syntaxFactory.ConstructorDeclaration(attributes, modifiers.ToList(), name, paramList, initializer, body, expressionBody, semicolon);

    }

    FieldDeclarationSyntax* ParseConstantFieldDeclaration(Parser* parser, TokenListBuffer* modifiers, SyntaxKind parentKind) {
        modifiers->Add(parser->EatToken(TokenKind::ConstKeyword));

        TypeSyntax* type = ParseType(parser, ParseTypeMode::Normal);
        SeparatedSyntaxList<VariableDeclaratorSyntax>* variableDeclarators = ParseFieldDeclarationVariableDeclarators(parser, type, VariableFlags::Const, parentKind);
        VariableDeclarationSyntax* declaration = parser->CreateNode<VariableDeclarationSyntax>(type, variableDeclarators);
        return parser->CreateNode<FieldDeclarationSyntax>(modifiers->Persist(parser->allocator), declaration, parser->EatToken(TokenKind::SemicolonToken));
    }

    FieldDeclarationSyntax* ParseFixedFieldDeclaration(Parser* parser, TokenListBuffer* modifiers, SyntaxKind parentKind) {
        modifiers->Add(parser->EatToken());

        TypeSyntax* type = ParseType(parser, ParseTypeMode::Normal);

        SeparatedSyntaxList<VariableDeclaratorSyntax>* variableDeclarators = ParseFieldDeclarationVariableDeclarators(parser, type, VariableFlags::Fixed, parentKind);
        VariableDeclarationSyntax* declaration = parser->CreateNode<VariableDeclarationSyntax>(type, variableDeclarators);

        return parser->CreateNode<FieldDeclarationSyntax>(modifiers->Persist(parser->allocator), declaration, parser->EatToken(TokenKind::SemicolonToken));

    }

    MemberDeclarationSyntax* TryParseConversionOperatorDeclaration(Parser* parser, TokenListBuffer* modifiers) {
        // implicit conversion toType (from type) {}
        if (parser->currentToken.kind != TokenKind::ImplicitKeyword && parser->currentToken.kind != TokenKind::ExplicitKeyword) {
            return nullptr;
        }

        NOT_IMPLEMENTED("TryParseConversionOperatorDeclaration");
        return nullptr;
    }

    MemberDeclarationSyntax* ParseOperatorDeclaration(Parser* parser, TokenListBuffer* modifiers, TypeSyntax* typeSyntax) {
        NOT_IMPLEMENTED("ParseOperatorDeclaration");
        return nullptr;
    }

    TypeSyntax* ParseTypeOrVoid(Parser* parser) {
        if (parser->currentToken.kind == TokenKind::VoidKeyword) {
            return parser->CreateNode<PredefinedTypeSyntax>(parser->EatToken());
        }

        return ParseType(parser, ParseTypeMode::Normal);
    }

    TypeSyntax* ParseReturnType(Parser* parser) {
        TerminatorState saveTerm = parser->_termState;
        parser->_termState |= TerminatorState::IsEndOfReturnType;
        TypeSyntax* type = ParseTypeOrVoid(parser);
        parser->_termState = saveTerm;
        return type;
    }

    bool unexpectedVariableToken(Parser* parser) {
        return parser->currentToken.kind != TokenKind::CommaToken;
    }

    bool abortVariable(Parser* parser, TokenKind kind) {
        return parser->currentToken.kind == TokenKind::SemicolonToken;
    }

    PostSkipAction SkipBadVariableListTokens(Parser* parser, TokenKind expected) {
        return SkipBadSeparatedListTokensWithExpectedKind(parser, &unexpectedVariableToken, &abortVariable, expected);
    }

    TypeParameterListSyntax* ParseTypeParameterList(Parser* parser) {
        if (parser->currentToken.kind != TokenKind::LessThanToken) {
            return nullptr;
        }

        TerminatorState saveTerm = parser->_termState;
        parser->_termState |= TerminatorState::IsEndOfTypeParameterList;
        SyntaxToken open = parser->EatToken(TokenKind::LessThanToken);

        SeparatedSyntaxList<TypeParameterSyntax>* parameters = ParseCommaSeparatedSyntaxList<TypeParameterSyntax>(
            parser,
            &open,
            TokenKind::GreaterThanToken,
            [](Parser* p) { return IsStartOfTypeParameter(p); },
            [](Parser* p) { return (SyntaxBase*) ParseTypeParameter(p); },
            &SkipBadTypeParameterListTokens,
            false,
            true,
            false
        );

        return nullptr;
    }

    ParameterListSyntax* ParseParenthesizedParameterList(Parser* parser) {
        NOT_IMPLEMENTED("ParameterListSyntax");
        return nullptr;
    }

    LocalFunctionStatementSyntax* TryParseLocalFunctionStatementBody(Parser* parser, TokenListBuffer* mods, TypeSyntax* parentType, SyntaxToken name) {
        NOT_IMPLEMENTED("LocalFunctionStatementSyntax");
        return nullptr;
    }

    bool IsPossibleVariableInitializer(Parser* parser) {
        return parser->currentToken.kind == TokenKind::OpenBraceToken || IsPossibleExpression(parser);
    }

    PostSkipAction SkipBadArrayInitializerTokens(Parser* parser, TokenKind expectedKind, TokenKind closeKind) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleVariableInitializer(p); },
            [](Parser* p, TokenKind closeKind) { return p->currentToken.kind == closeKind; },
            expectedKind,
            closeKind
        );
    }

    InitializerExpressionSyntax* ParseArrayInitializer(Parser* parser) {
        SyntaxToken openBrace = parser->EatToken(TokenKind::OpenBraceToken);
        SeparatedSyntaxList<ExpressionSyntax>* list = ParseCommaSeparatedSyntaxList<ExpressionSyntax>(
            parser,
            &openBrace,
            TokenKind::CloseBraceToken,
            [](Parser* p) { return IsPossibleVariableInitializer(p); },
            [](Parser* p) { return (SyntaxBase*) ParseVariableInitializer(p); },
            &SkipBadArrayInitializerTokens,
            true,
            false,
            false
        );

        return parser->CreateNode<InitializerExpressionSyntax>(
            SyntaxKind::ArrayInitializerExpression,
            openBrace,
            list,
            parser->EatToken(TokenKind::CloseBraceToken)
        );

    }

    bool IsLocalFunctionAfterIdentifier(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::OpenParenToken || parser->currentToken.kind == TokenKind::LessThanToken);

        ResetPoint resetPoint(parser);

        TypeParameterListSyntax* typeParameterListOpt = ParseTypeParameterList(parser);
        ParameterListSyntax* paramList = ParseParenthesizedParameterList(parser);

        TokenKind k = parser->currentToken.kind;

        if (!paramList->IsMissing() && (k == TokenKind::OpenBraceToken || k == TokenKind::EqualsGreaterThanToken || parser->currentToken.contextualKind == TokenKind::WhereKeyword)) {
            return true;
        }

        return false;
    }

    ExpressionSyntax* ParseVariableInitializer(Parser* parser) {
        return parser->currentToken.kind == TokenKind::OpenBraceToken
            ? ParseArrayInitializer(parser)
            : ParseExpression(parser);
    }

    BracketedArgumentListSyntax* ParseBracketedArgumentList(Parser* parser) {
        NOT_IMPLEMENTED("ParseBracketedArgumentList");
        return nullptr;
    }

    VariableDeclaratorSyntax* ParseVariableDeclarator(Parser* parser, TypeSyntax* parentType, VariableFlags flags, bool isFirst, bool allowLocalFunctions, TokenListBuffer* mods, LocalFunctionStatementSyntax** localFunction, bool isExpressionContext) {
        if (!isExpressionContext) {
            // Check for the common pattern of:
            //
            // C                    //<-- here
            // Console.WriteLine();
            //
            // Standard greedy parsing will assume that this should be parsed as a variable
            // declaration: "C Console".  We want to avoid that as it can confused parts of the
            // system further up.  So, if we see certain things following the identifier, then we can
            // assume it's not the actual name.
            //
            // So, if we're after a newline and we see a name followed by the list below, then we
            // assume that we're accidentally consuming too far into the next statement.
            //
            // <dot>, <arrow>, any binary operator (except =), <question>.  None of these characters
            // are allowed in a normal variable declaration.  This also provides a more useful error
            // message to the user.  Instead of telling them that a semicolon is expected after the
            // following token, then instead get a useful message about an identifier being missing.
            // The above list prevents:
            //
            // C                    //<-- here
            // Console.WriteLine();
            //
            // C                    //<-- here
            // Console->WriteLine();
            //
            // C
            // A + B;
            //
            // C
            // A ? B : D;
            //
            // C
            // A()

            ResetPoint reset(parser);

            TokenKind currentTokenKind = parser->currentToken.kind;
            if (currentTokenKind == TokenKind::IdentifierToken && !parentType->IsMissing()) {

                bool isAfterNewLine = parser->IsAfterNewLine(parentType->endTokenId);

                if (isAfterNewLine) {

                    parser->EatToken();
                    currentTokenKind = parser->currentToken.kind;

                    bool isNonEqualsBinaryToken = currentTokenKind != TokenKind::EqualsToken && SyntaxFacts::IsBinaryExpressionOperatorToken(currentTokenKind);

                    if (currentTokenKind == TokenKind::DotToken || currentTokenKind == TokenKind::OpenParenToken || currentTokenKind == TokenKind::MinusGreaterThanToken || isNonEqualsBinaryToken) {
                        bool isPossibleLocalFunctionToken = currentTokenKind == TokenKind::OpenParenToken || currentTokenKind == TokenKind::LessThanToken;

                        // Make sure this isn't a local function
                        if (!isPossibleLocalFunctionToken || !IsLocalFunctionAfterIdentifier(parser)) {
                            SyntaxToken missingIdentifier = MakeMissingToken(TokenKind::IdentifierToken, parser->ptr);
                            parser->AddError(missingIdentifier, ErrorCode::ERR_IdentifierExpected);

                            *localFunction = nullptr;
                            return parser->CreateNode<VariableDeclaratorSyntax>(missingIdentifier, nullptr, nullptr);
                        }
                    }
                }
            }
        }

        SyntaxToken name = ParseIdentifierToken(parser);
        BracketedArgumentListSyntax* argumentList = nullptr;
        EqualsValueClauseSyntax* initializer = nullptr;
        TerminatorState saveTerm = parser->_termState;
        bool isFixed = (flags & VariableFlags::Fixed) != 0;
        bool isConst = (flags & VariableFlags::Const) != 0;
        bool isLocalOrField = (flags & VariableFlags::LocalOrField) != 0;

        // Give better error message in the case where the user did something like:
        //
        // X x = 1, Y y = 2;
        // using (X x = expr1, Y y = expr2) ...
        //
        // The superfluous type name is treated as variable (it is an identifier) and a missing ',' is injected after it.
        if (!isFirst && IsTrueIdentifier(parser)) {
            parser->AddError(name, ErrorCode::ERR_MultiTypeInDeclaration);
        }
        switch (parser->currentToken.kind) {
            case TokenKind::EqualsToken: {
                if (isFixed) {
                    goto default_label;
                }
                equals_label:
                SyntaxToken equals = parser->EatToken();
                // check for lambda expression with explicit ref return type: `ref int () => { ... }`
                SyntaxToken refKeyword = isLocalOrField && !isConst && parser->currentToken.kind == TokenKind::RefKeyword && !IsPossibleLambdaExpression(parser, Precedence::Expression)
                    ? parser->EatToken()
                    : SyntaxToken();
                ExpressionSyntax* init = ParseVariableInitializer(parser);
                initializer = parser->CreateNode<EqualsValueClauseSyntax>(equals, !refKeyword.IsValid() ? init : parser->CreateNode<RefExpressionSyntax>(refKeyword, init));
                break;
            }
            case TokenKind::LessThanToken: {
                if (allowLocalFunctions && isFirst) {
                    *localFunction = TryParseLocalFunctionStatementBody(parser, mods, parentType, name);
                    if (*localFunction != nullptr) {
                        return nullptr;
                    }
                }
                goto default_label;
            }
            case TokenKind::OpenParenToken: {
                if (allowLocalFunctions && isFirst) {
                    *localFunction = TryParseLocalFunctionStatementBody(parser, mods, parentType, name);
                    if (*localFunction != nullptr) {
                        return nullptr;
                    }
                }
                // Special case for accidental use of C-style constructors
                // Fake up something to hold the arguments.
                parser->_termState |= TerminatorState::IsPossibleEndOfVariableDeclaration;
                argumentList = ParseBracketedArgumentList(parser);
                parser->_termState = saveTerm;
                parser->AddError(argumentList, ErrorCode::ERR_BadVarDecl);
                break;
            }
            case TokenKind::OpenBracketToken: {
                open_bracket_label:
                bool sawNonOmittedSize;
                parser->_termState |= TerminatorState::IsPossibleEndOfVariableDeclaration;
                ArrayRankSpecifierSyntax* specifier = ParseArrayRankSpecifier(parser, &sawNonOmittedSize);
                parser->_termState = saveTerm;
                SyntaxToken open = specifier->open;
                SeparatedSyntaxList<ExpressionSyntax>* sizes = specifier->ranks;
                SyntaxToken close = specifier->close;

                if (isFixed && !sawNonOmittedSize) {
                    parser->AddError(close, ErrorCode::ERR_ValueExpected);
                }

                // int[] values;
                // int[,] values;

                ArgumentSyntax** args = parser->allocator->AllocateUncleared<ArgumentSyntax*>(sizes->itemCount);

                for (int32 i = 0; i < sizes->itemCount; i++) {

                    bool isOmitted = sizes->items[i]->GetKind() == SyntaxKind::OmittedArraySizeExpression;
                    if (!isFixed && !isOmitted) {
                        parser->AddError(sizes->items[i], ErrorCode::ERR_ArraySizeInDeclaration);
                    }

                    args[i] = parser->CreateNode<ArgumentSyntax>(nullptr, SyntaxToken(), sizes->items[i]);
                }

                SeparatedSyntaxList<ArgumentSyntax>* argList = parser->allocator->New<SeparatedSyntaxList<ArgumentSyntax>>(sizes->itemCount, args, sizes->separatorCount, sizes->separators);

                argumentList = parser->CreateNode<BracketedArgumentListSyntax>(open, argList, close);

                if (!isFixed) {
                    parser->AddError(argumentList, ErrorCode::ERR_CStyleArray);
                    if (parser->currentToken.kind == TokenKind::EqualsToken) {
                        goto equals_label;
                    }
                }

                break;
            }
            default: {
                default_label:
                if (isConst) {
                    parser->AddError(name, ErrorCode::ERR_ConstValueRequired);
                }
                else if (isFixed) {
                    if (parentType->GetKind() == SyntaxKind::ArrayType) {
                        // They accidentally put the array before the identifier
                        parser->AddError(name, ErrorCode::ERR_FixedDimsRequired);
                    }
                    else {
                        goto open_bracket_label;
                    }
                }
                break;
            }
        }
        *localFunction = nullptr;
        return parser->CreateNode<VariableDeclaratorSyntax>(name, argumentList, initializer);
    }

    void ParseVariableDeclarators(Parser* parser, TypeSyntax* type, VariableFlags flags, SeparatedSyntaxListBuilder<VariableDeclaratorSyntax>* variables, bool variableDeclarationsExpected, bool allowLocalFunctions, bool stopOnCloseParen, TokenListBuffer* mods, LocalFunctionStatementSyntax** localFunction) {

        *localFunction = nullptr;

        variables->Add(ParseVariableDeclarator(parser, type, flags, true, allowLocalFunctions, mods, localFunction));

        if (*localFunction != nullptr) {
            assert(variables->itemCount == 0);
            return;
        }

        while (true) {
            if (parser->currentToken.kind == TokenKind::SemicolonToken) {
                break;
            }

            if (stopOnCloseParen && parser->currentToken.kind == TokenKind::CloseParenToken) {
                break;
            }

            if (parser->currentToken.kind == TokenKind::CommaToken) {
                variables->AddSeparator(parser->EatToken());
                variables->Add(ParseVariableDeclarator(parser, type, flags, false, allowLocalFunctions, mods, localFunction));
            }
            else if (!variableDeclarationsExpected || SkipBadVariableListTokens(parser, TokenKind::CommaToken) == PostSkipAction::Abort) {
                break;
            }
        }
    }

    SeparatedSyntaxList<VariableDeclaratorSyntax>* ParseFieldDeclarationVariableDeclarators(Parser* parser, TypeSyntax* type, VariableFlags flags, SyntaxKind parentKind) {
        bool variableDeclarationsExpected = parentKind != SyntaxKind::NamespaceDeclaration && parentKind != SyntaxKind::CompilationUnit;
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SeparatedSyntaxListBuilder<VariableDeclaratorSyntax> variables(parser->tempAllocator);
        TerminatorState saveTerm = parser->_termState;
        parser->_termState |= TerminatorState::IsEndOfFieldDeclaration;

        LocalFunctionStatementSyntax* local = nullptr;
        ParseVariableDeclarators(parser, type, flags, &variables, variableDeclarationsExpected, false, false, nullptr, &local);

        parser->_termState = saveTerm;
        return variables.ToList(parser->allocator);
    }

    MemberDeclarationSyntax* ParseNormalFieldDeclaration(Parser* parser, TokenListBuffer* modifiers, TypeSyntax* type, SyntaxKind parentKind) {
        SeparatedSyntaxList<VariableDeclaratorSyntax>* variables = ParseFieldDeclarationVariableDeclarators(parser, type, VariableFlags::LocalOrField, parentKind);
        VariableDeclarationSyntax* variableDeclaration = parser->CreateNode<VariableDeclarationSyntax>(type, variables);
        return parser->CreateNode<FieldDeclarationSyntax>(modifiers->Persist(parser->allocator), variableDeclaration, parser->EatToken(TokenKind::SemicolonToken));
    }

    bool IsFieldDeclaration(Parser* parser) {
        if (parser->currentToken.kind != TokenKind::IdentifierToken) {
            return false;
        }

        // Treat this as a field, unless we have anything following that
        // makes us:
        //   a) explicit
        //   b) generic
        //   c) a property
        //   d) a method (unless we already know we're parsing an event)
        TokenKind kind = parser->PeekToken(1).kind;

        // Error recovery, don't allow a misplaced semicolon after the name in a property to throw off the entire parse.
        //
        // e.g. `public int MyProperty; { get; set; }` should still be parsed as a property with a skipped token.
        if (kind == TokenKind::SemicolonToken && IsStartOfPropertyBody(parser->PeekToken(2).kind)) {
            return false;
        }

        switch (kind) {
            case TokenKind::LessThanToken:              // Goo<     explicit or generic method
            case TokenKind::OpenBraceToken:             // Goo {    property
            case TokenKind::EqualsGreaterThanToken:     // Goo =>   property
            case TokenKind::OpenParenToken:             // Goo(     method
                return false;
            default:
                return true;
        }
    }

    bool IsOperatorStart(Parser* parser) {
        return parser->currentToken.kind == TokenKind::OperatorKeyword;
    }

    bool IsStartOfPropertyBody(TokenKind kind) {
        return kind == TokenKind::OpenBraceToken || kind == TokenKind::EqualsGreaterThanToken;
    }

    MemberDeclarationSyntax* ParseMemberDeclaration(Parser* parser, SyntaxKind parentKind) {
        assert(parentKind != SyntaxKind::CompilationUnit);

        TerminatorState saveTermState = parser->_termState;

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        TokenListBuffer modifiers(parser->tempAllocator);

        bool isPossibleTypeDeclaration = false;
        ParseModifiers(parser, &modifiers, false, &isPossibleTypeDeclaration);

        if (parser->currentToken.kind == TokenKind::ConstructorKeyword || parser->currentToken.kind == TokenKind::IdentifierToken && parser->PeekToken(1).kind == TokenKind::OpenParenToken) {
            return ParseConstructorDeclaration(parser, &modifiers);
        }

        if (parser->currentToken.kind == TokenKind::ConstKeyword) {
            return ParseConstantFieldDeclaration(parser, &modifiers, parentKind);
        }

        if (parser->currentToken.kind == TokenKind::FixedKeyword) {
            return ParseFixedFieldDeclaration(parser, &modifiers, parentKind);
        }

        // Check for conversion operators (implicit/explicit)
        MemberDeclarationSyntax* result = TryParseConversionOperatorDeclaration(parser, &modifiers);
        if (result != nullptr) {
            return result;
        }

        // It's valid to have a type declaration here -- check for those
        if (isPossibleTypeDeclaration && IsTypeDeclarationStart(parser)) {
            return ParseTypeDeclaration(parser, &modifiers);
        }

        // Everything that's left -- methods, fields, properties,
        // indexers, and non-conversion operators -- starts with a type (possibly void).
        TypeSyntax* type = ParseReturnType(parser);

        ResetPoint afterTypeResetPoint(parser);

        // Check for misplaced modifiers. if we see any, then consider this member terminated and restart parsing.
        // if (IsMisplacedModifier(&modifiers, type, result)) {
        //     return result;
        // }
        parse_member_name:

        // If we've seen the ref keyword, we know we must have an indexer, method, field, or property.
        if (type->GetKind() != SyntaxKind::RefType) {
            // Check here for operators
            if (IsOperatorStart(parser)) {
                return ParseOperatorDeclaration(parser, &modifiers, type);
            }

        }

        if (IsFieldDeclaration(parser)) {
            return ParseNormalFieldDeclaration(parser, &modifiers, type, parentKind);
        }

        return nullptr;
    }

    void AddSkippedNamespaceText(Parser* parser, SyntaxToken token, NamespaceBodyBuilder* body, SyntaxListBuilder<SyntaxBase>* initialBadNodes, SyntaxBase* skippedSyntax) {
        // todo -- just flag the token as skipped
        return;
    }

    void ReduceIncompleteMembers(Parser* parser, SyntaxListBuilder<MemberDeclarationSyntax>* incompleteMembers, SyntaxToken token, NamespaceBodyBuilder* body, SyntaxListBuilder<SyntaxBase>* initialBadNodes) {
        for (int32 i = 0; i < incompleteMembers->size; i++) {
            AddSkippedNamespaceText(parser, token, body, initialBadNodes, incompleteMembers->Get(i));
        }
        incompleteMembers->size = 0;
    }

    void ParseNamespaceBody(Parser* parser, SyntaxToken openBraceOrSemiColon, NamespaceBodyBuilder* bodyBuilder, SyntaxListBuilder<SyntaxBase>* initialBadNodes, SyntaxKind parentKind) {

        TerminatorState saveTerm = parser->_termState;
        parser->_termState |= TerminatorState::IsNamespaceMemberStartOrStop;
        NamespaceParts seen = NamespaceParts::None;

        TempAllocator::ScopedMarker marker(parser->tempAllocator);
        SyntaxListBuilder<MemberDeclarationSyntax> pendingIncompleteMembers(parser->tempAllocator);
        bool reportUnexpectedToken = true;

        while (true) {
            switch (parser->currentToken.kind) {
                case TokenKind::NamespaceKeyword: {
                    AddIncompleteMembers(&pendingIncompleteMembers, bodyBuilder);

                    SyntaxListBuilder<SyntaxBase> modifiers(parser->tempAllocator);
                    bodyBuilder->members->Add(adjustStateAndReportStatementOutOfOrder(parser, &seen, ParseNamespaceDeclaration(parser, &modifiers)));
                    reportUnexpectedToken = true;

                    break;
                }
                case TokenKind::EndOfFileToken:
                case TokenKind::CloseBraceToken: {
                    // This token marks the end of a namespace body
                    return;
                }
                case TokenKind::ExternKeyword: {
                    NOT_IMPLEMENTED("extern");
                    break; // todo
                }

                case TokenKind::UsingKeyword: {
//                    ParseUsingDirective(parser, openBraceOrSemiColon, bodyBuilder, initialBadNodes, &seen, &pendingIncompleteMembers);
                    reportUnexpectedToken = true;
                    break;
                }

                default: {
                    MemberDeclarationSyntax* memberOrStatement = ParseMemberDeclaration(parser, parentKind);
                    if (memberOrStatement == nullptr) {
                        // incomplete members must be processed before we add any nodes to the body:
                        ReduceIncompleteMembers(parser, &pendingIncompleteMembers, openBraceOrSemiColon, bodyBuilder, initialBadNodes);

                        // eat one token and try to parse declaration or statement again:
                        SyntaxToken skippedToken = parser->EatToken();
                        if (reportUnexpectedToken && !skippedToken.ContainsDiagnostics()) {
                            parser->AddError(skippedToken, ErrorCode::ERR_EOFExpected);

                            // do not report the error multiple times for subsequent tokens:
                            reportUnexpectedToken = false;
                        }


                        // I think we just skip these tokens, everything from the last valid / used token to this point
                        // AddSkippedNamespaceText(parser, openBraceOrSemiColon, bodyBuilder, initialBadNodes, skippedToken);

                    }
                    else if (memberOrStatement->GetKind() == SyntaxKind::IncompleteMember && seen < NamespaceParts::MembersAndStatements) {
                        pendingIncompleteMembers.Add(memberOrStatement);
                        reportUnexpectedToken = true;
                    }
                    else {
                        // incomplete members must be processed before we add any nodes to the body:
                        AddIncompleteMembers(&pendingIncompleteMembers, bodyBuilder);

                        bodyBuilder->members->Add(adjustStateAndReportStatementOutOfOrder(parser, &seen, memberOrStatement));
                        reportUnexpectedToken = true;
                    }
                }

            }
        }

    }

    // For better error recovery 'static =>' is also considered a possible lambda expression.
    bool IsPossibleLambdaExpression(Parser* parser, Precedence precedence) {
        if (precedence > Precedence::Lambda) {
            return false;
        }

        SyntaxToken token1 = parser->PeekToken(1);

        // x =>
        if (token1.kind == TokenKind::EqualsGreaterThanToken) {
            return true;
        }

        ResetPoint resetPoint(parser);

        bool seenStatic = false;
        if (parser->currentToken.kind == TokenKind::StaticKeyword) {
            parser->EatToken();
            seenStatic = true;
        }

        if (seenStatic) {
            if (parser->currentToken.kind == TokenKind::EqualsGreaterThanToken) {
                // `static =>`
                // This is an error case, but we have enough code in front of us to be certain
                // the user was trying to write a static lambda.
                return true;
            }

            if (parser->currentToken.kind == TokenKind::OpenParenToken) {
                // static (...
                return true;
            }
        }

        if (parser->currentToken.kind == TokenKind::IdentifierToken && parser->PeekToken(1).kind == TokenKind::EqualsGreaterThanToken) {
            // 1. `a => ...`
            // 2. `static a => ...`
            return true;
        }

        // Have checked all the static forms.  And have checked for the basic `a => a` form.
        // At this point we have must be on an explicit return type for this to still be a lambda.
        //        if (Parser.currentToken.contextualKind == TokenKind::AsyncKeyword &&            IsAnonymousFunctionAsyncModifier()) {
        //            EatToken();
        //        }

        SyntaxToken k;
        ScanTypeFlags st = ScanType(parser, &k, false);
        if (st == ScanTypeFlags::NotType || parser->currentToken.kind != TokenKind::OpenParenToken) {
            // reset
        }

        // However, just because we're on `async` doesn't mean we're a lambda.  We might have
        // something lambda-like like:
        //
        //      async a => ...  // or
        //      async (a) => ...
        //
        // Or we could have something that isn't a lambda like:
        //
        //      async ();

        // ' <identifier> => ...' looks like an async simple lambda
        if (parser->currentToken.kind == TokenKind::IdentifierToken && parser->PeekToken(1).kind == TokenKind::EqualsGreaterThanToken) {
            //  a => ...
            return true;
        }

        // Non-simple async lambda must be of the form 'async (...'
        if (parser->currentToken.kind != TokenKind::OpenParenToken) {
            return false;
        }

        // Check whether looks like implicitly or explicitly typed lambda
        return ScanParenthesizedLambda(parser, precedence);
    }

    bool IsPossibleExpression(Parser* parser, bool allowBinaryExpressions, bool allowAssignmentExpressions) {
        TokenKind tk = parser->currentToken.kind;
        switch (tk) {
            case TokenKind::TypeOfKeyword:
            case TokenKind::DefaultKeyword:
            case TokenKind::SizeOfKeyword:
            case TokenKind::BaseKeyword:
            case TokenKind::FalseKeyword:
            case TokenKind::ThisKeyword:
            case TokenKind::TrueKeyword:
            case TokenKind::NullKeyword:
            case TokenKind::OpenParenToken:
            case TokenKind::NumericLiteralToken:
            case TokenKind::StringLiteralToken:
                // todo -- strings
//            case TokenKind::SingleLineRawStringLiteralToken:
//            case TokenKind::MultiLineRawStringLiteralToken:
//            case TokenKind::InterpolatedStringToken:
//            case TokenKind::InterpolatedStringStartToken:
//            case TokenKind::InterpolatedVerbatimStringStartToken:
//            case TokenKind::InterpolatedSingleLineRawStringStartToken:
//            case TokenKind::InterpolatedMultiLineRawStringStartToken:
            case TokenKind::CharacterLiteralToken:
            case TokenKind::NewKeyword:
            case TokenKind::DelegateKeyword:
            case TokenKind::ThrowKeyword:
//            case TokenKind::StackAllocKeyword: probably allow something like this
            case TokenKind::DotDotToken:
            case TokenKind::RefKeyword:
            case TokenKind::OpenBracketToken: // collection expression
                return true;
            case TokenKind::StaticKeyword:
                return IsPossibleAnonymousMethodExpression(parser) || IsPossibleLambdaExpression(parser, Precedence::Expression);
            case TokenKind::IdentifierToken:
                return IsTrueIdentifier(parser);
            default:
                return SyntaxFacts::IsPredefinedType(tk)
                    || SyntaxFacts::IsAnyUnaryExpression(tk)
                    || (allowBinaryExpressions && SyntaxFacts::IsBinaryExpression(tk))
                    || (allowAssignmentExpressions && SyntaxFacts::IsAssignmentExpressionOperatorToken(tk));
        }
    }

    bool IsPossibleAccessorModifier(Parser* parser) {
        // We only want to accept a modifier as the start of an accessor if the modifiers are
        // actually followed by "get/set/add/remove".  Otherwise, we might thing think we're
        // starting an accessor when we're actually starting a normal class member.  For example:
        //
        //      class C {
        //          public int Prop { get { this.
        //          private DateTime x;
        //
        // We don't want to think of the "private" in "private DateTime x" as starting an accessor
        // here.  If we do, we'll get totally thrown off in parsing the remainder and that will
        // throw off the rest of the features that depend on a good syntax tree.
        //
        // Note: we allow all modifiers here.  That's because we want to parse things like
        // "abstract get" as an accessor.  This way we can provide a good error message
        // to the user that this is not allowed.

        if (GetModifier(parser->currentToken) == DeclarationModifiers::None) {
            return false;
        }

        int32 peekIndex = 1;
        while (GetModifier(parser->PeekToken(peekIndex)) != DeclarationModifiers::None) {
            peekIndex++;
        }

        SyntaxToken token = parser->PeekToken(peekIndex);

        if (token.kind == TokenKind::CloseBraceToken || token.kind == TokenKind::EndOfFileToken) {
            // If we see "{ get { } public }
            // then we will think that "public" likely starts an accessor.
            return true;
        }

        switch (token.contextualKind) {
            case TokenKind::GetKeyword:
            case TokenKind::SetKeyword:
            case TokenKind::InitKeyword:
                return true;
            default:
                return false;
        }

    }

    bool IsPossibleStatement(Parser* parser, bool acceptAccessibilityMods) {
        TokenKind tk = parser->currentToken.kind;
        switch (tk) {
            case TokenKind::BreakKeyword:
            case TokenKind::ContinueKeyword:
            case TokenKind::TryKeyword:
            case TokenKind::ConstKeyword:
            case TokenKind::DoKeyword:
            case TokenKind::ForKeyword:
            case TokenKind::ForEachKeyword:
            case TokenKind::IfKeyword:
            case TokenKind::ElseKeyword:
            case TokenKind::ReturnKeyword:
            case TokenKind::SwitchKeyword:
            case TokenKind::ThrowKeyword:
            case TokenKind::UsingKeyword:
            case TokenKind::WhileKeyword:
            case TokenKind::OpenBraceToken:
            case TokenKind::SemicolonToken:
            case TokenKind::StaticKeyword:
            case TokenKind::ReadOnlyKeyword: // not sure about this one
            case TokenKind::RefKeyword:
            case TokenKind::ExternKeyword: // not sure this could be right here
            case TokenKind::OpenBracketToken:
            case TokenKind::CaseKeyword: // for parsing an errant case without a switch.
                return true;

            case TokenKind::IdentifierToken:
                return IsTrueIdentifier(parser);

                // Accessibility modifiers are not legal in a statement,
                // but a common mistake for local functions. Parse to give a
                // better error message.
            case TokenKind::PublicKeyword:
            case TokenKind::InternalKeyword:
            case TokenKind::ProtectedKeyword:
            case TokenKind::PrivateKeyword:
                return acceptAccessibilityMods;
            default:
                return SyntaxFacts::IsPredefinedType(tk) || IsPossibleExpression(parser);
        }
    }

    bool IsPossibleStatementStartOrStop(Parser* parser) {
        return parser->currentToken.kind == TokenKind::SemicolonToken || IsPossibleStatement(parser, true);
    }

    bool IsEndOfFixedStatement() {
        return false;
    }

    bool IsEndOfTryBlock() {
        return false;
    }

    bool IsEndOfCatchClause() {
        return false;
    }

    bool IsEndOfFilterClause() {
        return false;
    }

    bool IsEndOfCatchBlock() {
        return false;
    }

    bool IsEndOfDoWhileExpression() {
        return false;
    }

    bool IsEndOfForStatementArgument() {
        return false;
    }

    bool IsEndOfDeclarationClause() {
        return false;
    }

    bool IsEndOfArgumentList() {
        return false;
    }

    bool IsPossibleSwitchSection() {
        return false;
    }

    bool IsEndOfTypeParameterList() {
        return false;
    }

    bool IsEndOfMethodSignature() {
        return false;
    }

    bool IsEndOfNameInExplicitInterface() {
        return false;
    }

    bool IsEndOfFunctionPointerParameterList(bool b) {
        return false;
    }

    bool IsEndOfFunctionPointerCallingConvention() {
        return false;
    }

    bool IsEndOfRecordOrClassOrStructOrInterfaceSignature() {
        return false;
    }

    bool IsPartialType(Parser* parser) {
        assert(parser->currentToken.contextualKind == TokenKind::PartialKeyword);
        switch (parser->PeekToken(1).kind) {
            case TokenKind::StructKeyword:
            case TokenKind::ClassKeyword:
            case TokenKind::InterfaceKeyword:
                return true;
            default:
                return false;
        }
    }

    bool IsPartialInNamespaceMemberDeclaration(Parser* parser) {
        if (parser->currentToken.contextualKind == TokenKind::PartialKeyword) {
            if (IsPartialType(parser)) {
                return true;
            }
            else if (parser->PeekToken(1).kind == TokenKind::NamespaceKeyword) {
                return true;
            }
        }

        return false;
    }

    bool IsTypeModifierOrTypeKeyword(TokenKind kind) {
        switch (kind) {
            case TokenKind::EnumKeyword:
            case TokenKind::DelegateKeyword:
            case TokenKind::ClassKeyword:
            case TokenKind::InterfaceKeyword:
            case TokenKind::StructKeyword:
            case TokenKind::AbstractKeyword:
            case TokenKind::InternalKeyword:
            case TokenKind::PrivateKeyword:
            case TokenKind::ProtectedKeyword:
            case TokenKind::PublicKeyword:
            case TokenKind::SealedKeyword:
            case TokenKind::StaticKeyword:
                // case TokenKind::NewKeyword: // might remove?
                return true;
            default:
                return false;
        }
    }

    bool IsTypeDeclarationStart(Parser* parser) {
        switch (parser->currentToken.kind) {
            case TokenKind::ClassKeyword:
            case TokenKind::DelegateKeyword:
            case TokenKind::EnumKeyword:
            case TokenKind::InterfaceKeyword:
            case TokenKind::StructKeyword:
                return true;

            default:
                return false;
        }
    }

    bool IsPossibleStartOfTypeDeclaration(TokenKind kind) {
        return IsTypeModifierOrTypeKeyword(kind) || kind == TokenKind::OpenBracketToken;
    }

    bool IsPossibleNamespaceMemberDeclaration(Parser* parser) {
        switch (parser->currentToken.kind) {
            case TokenKind::ExternKeyword:
            case TokenKind::UsingKeyword:
            case TokenKind::NamespaceKeyword:
                return true;
            case TokenKind::IdentifierToken:
                return IsPartialInNamespaceMemberDeclaration(parser);
            default:
                return IsPossibleStartOfTypeDeclaration(parser->currentToken.kind);
        }
    }

    bool IsTerminator(Parser* parser) {

        if (parser->currentToken.kind == TokenKind::EndOfFileToken) {
            return true;
        }

        int32 num = (int32) parser->_termState;

        while (num) {
            int32 position = tzcnt32(num);
            num &= num - 1; // Clear the least significant bit

            switch ((TerminatorState) (1 << position)) {
                case TerminatorState::IsPatternInSwitchExpressionArm:
                case TerminatorState::IsExpressionOrPatternInCaseLabelOfSwitchStatement:
                case TerminatorState::__LAST__:
                case TerminatorState::EndOfFile: {
                    continue;
                }
                case TerminatorState::IsNamespaceMemberStartOrStop: {
                    if (IsNamespaceMemberStartOrStop(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsAttributeDeclarationTerminator: {
                    if (IsAttributeDeclarationTerminator()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsPossibleAggregateClauseStartOrStop: {
                    if (IsPossibleAggregateClauseStartOrStop()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsPossibleMemberStartOrStop: {
                    if (IsPossibleMemberStartOrStop(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfReturnType: {
                    if (IsEndOfReturnType(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfParameterList: {
                    if (IsEndOfParameterList(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfFieldDeclaration: {
                    if (IsEndOfFieldDeclaration(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsPossibleEndOfVariableDeclaration: {
                    if (IsPossibleEndOfVariableDeclaration(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfTypeArgumentList: {
                    if (IsEndOfTypeArgumentList(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsPossibleStatementStartOrStop: {
                    if (IsPossibleStatementStartOrStop(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfFixedStatement: {
                    if (IsEndOfFixedStatement()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfTryBlock: {
                    if (IsEndOfTryBlock()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfCatchClause: {
                    if (IsEndOfCatchClause()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfFilterClause: {
                    if (IsEndOfFilterClause()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfCatchBlock: {
                    if (IsEndOfCatchBlock()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfDoWhileExpression: {
                    if (IsEndOfDoWhileExpression()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfForStatementArgument: {
                    if (IsEndOfForStatementArgument()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfDeclarationClause: {
                    if (IsEndOfDeclarationClause()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfArgumentList: {
                    if (IsEndOfArgumentList()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsSwitchSectionStart: {
                    if (IsPossibleSwitchSection()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfTypeParameterList: {
                    if (IsEndOfTypeParameterList()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfMethodSignature: {
                    if (IsEndOfMethodSignature()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfNameInExplicitInterface: {
                    if (IsEndOfNameInExplicitInterface()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfFunctionPointerParameterList: {
                    if (IsEndOfFunctionPointerParameterList(false)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfFunctionPointerParameterListErrored: {
                    if (IsEndOfFunctionPointerParameterList(true)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfFunctionPointerCallingConvention: {
                    if (IsEndOfFunctionPointerCallingConvention()) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfRecordOrClassOrStructOrInterfaceSignature: {
                    if (IsEndOfRecordOrClassOrStructOrInterfaceSignature()) {
                        return true;
                    }
                    continue;
                }
            }
        }

        return false;
    }

    SyntaxToken ParseIdentifierToken(Parser* parser, ErrorCode errorCode) {
        if (parser->currentToken.kind == TokenKind::IdentifierToken) {
            return parser->EatToken();
        }
        else {
            SyntaxToken token = parser->currentToken;
            Diagnostic diagnostic(errorCode, token.text, token.text + token.textSize);
            parser->AddError(token, diagnostic);
            return parser->CreateMissingToken(TokenKind::IdentifierToken);
        }
    }

    void ParseNamespaceBody(Parser* parser, SyntaxToken* openBraceOrSemicolon) {
        bool isGlobal = openBraceOrSemicolon == nullptr;

        TerminatorState saveTerm = parser->_termState;
        parser->_termState |= TerminatorState::IsNamespaceMemberStartOrStop;
        NamespaceParts seen = NamespaceParts::None;

        SyntaxListBuilder<MemberDeclarationSyntax> pendingIncompleteMembers(parser->tempAllocator);

        bool reportUnexpectedToken = true;

        while (true) {
            switch (parser->currentToken.kind) {
                case TokenKind::NamespaceKeyword: {

                    break;
                }
                default: {
                    break;
                }
            }

        }

    }

    IdentifierNameSyntax* ParseIdentifierName(Parser* parser, ErrorCode code) {
        return parser->CreateNode<IdentifierNameSyntax>(parser->EatToken(TokenKind::IdentifierToken));
    }

    ScanTypeArgumentListKind ScanTypeArgumentList(Parser* parser, NameOptions options) {
        if (parser->currentToken.kind != TokenKind::LessThanToken) {
            return ScanTypeArgumentListKind::NotTypeArgumentList;
        }

        if ((options & NameOptions::InExpression) == 0) {
            return ScanTypeArgumentListKind::DefiniteTypeArgumentList;
        }

        // We're in an expression context, and we have a < token.  This could be a
        // type argument list, or it could just be a relational expression.
        //
        // Scan just the type argument list portion (i.e. the part from < to > ) to
        // see what we think it could be.  This will give us one of three possibilities:
        //
        //      result == ScanTypeFlags.NotType.
        //
        // This is absolutely not a type-argument-list.  Just return that result immediately.
        //
        //      result != ScanTypeFlags.NotType && isDefinitelyTypeArgumentList.
        //
        // This is absolutely a type-argument-list.  Just return that result immediately
        //
        //      result != ScanTypeFlags.NotType && !isDefinitelyTypeArgumentList.
        //
        // This could be a type-argument list, or it could be an expression.  Need to see
        // what came after the last '>' to find out which it is.

        // Scan for a type argument list. If we think it's a type argument list
        // then assume it is unless we see specific tokens following it.
        SyntaxToken greaterThanToken;
        bool isDefinitelyTypeArgumentList = false;
        ScanTypeFlags possibleTypeArgumentFlags = ScanPossibleTypeArgumentList(parser, &greaterThanToken, &isDefinitelyTypeArgumentList);
        if (possibleTypeArgumentFlags == ScanTypeFlags::NotType) {
            return ScanTypeArgumentListKind::NotTypeArgumentList;
        }

        if (isDefinitelyTypeArgumentList) {
            return ScanTypeArgumentListKind::DefiniteTypeArgumentList;
        }

        // If we did not definitively determine from immediate syntax that it was or
        // was not a type argument list, we must have scanned the entire thing up through
        // the closing greater-than token. In that case we will disambiguate based on the
        // token that follows it.

        assert(greaterThanToken.kind == TokenKind::GreaterThanToken);
        switch (parser->currentToken.kind) {
            case TokenKind::OpenParenToken:
            case TokenKind::CloseParenToken:
            case TokenKind::CloseBracketToken:
            case TokenKind::CloseBraceToken:
            case TokenKind::ColonToken:
            case TokenKind::SemicolonToken:
            case TokenKind::CommaToken:
            case TokenKind::DotToken:
            case TokenKind::QuestionToken:
            case TokenKind::EqualsEqualsToken:
            case TokenKind::ExclamationEqualsToken:
            case TokenKind::BarToken:
            case TokenKind::CaretToken:
                // These tokens are from 7.5.4.2 Grammar Ambiguities
                return ScanTypeArgumentListKind::DefiniteTypeArgumentList;

            case TokenKind::AmpersandAmpersandToken: // e.g. `e is A<B> && e`
            case TokenKind::BarBarToken:             // e.g. `e is A<B> || e`
            case TokenKind::AmpersandToken:          // e.g. `e is A<B> & e`
            case TokenKind::OpenBracketToken:        // e.g. `e is A<B>[]`
            case TokenKind::LessThanToken:           // e.g. `e is A<B> < C`
            case TokenKind::LessThanEqualsToken:     // e.g. `e is A<B> <= C`
            case TokenKind::GreaterThanEqualsToken:  // e.g. `e is A<B> >= C`
            case TokenKind::IsKeyword:               // e.g. `e is A<B> is bool`
            case TokenKind::AsKeyword:               // e.g. `e is A<B> as bool`
                // These tokens were added to 7.5.4.2 Grammar Ambiguities in C# 7.0
                return ScanTypeArgumentListKind::DefiniteTypeArgumentList;

            case TokenKind::OpenBraceToken: // e.g. `e is A<B> {}`
                // This token was added to 7.5.4.2 Grammar Ambiguities in C# 8.0
                return ScanTypeArgumentListKind::DefiniteTypeArgumentList;

            case TokenKind::GreaterThanToken: {
                // This token is added to 7.5.4.2 Grammar Ambiguities in C#7 for the special case in which
                // the possible generic is following an `is` keyword, e.g. `e is A<B> > C`.
                // We test one further token ahead because a right-shift operator `>>` looks like a pair of greater-than
                // tokens at this stage, but we don't intend to be handling the right-shift operator.
                // The upshot is that we retain compatibility with the two previous behaviors:
                // `(x is A<B>>C)` is parsed as `(x is A<B>) > C`
                // `A<B>>C` elsewhere is parsed as `A < (B >> C)`
                if (((options & NameOptions::AfterIs) != 0) && parser->PeekToken(1).kind != TokenKind::GreaterThanToken) {
                    return ScanTypeArgumentListKind::DefiniteTypeArgumentList;
                }

                return ScanTypeArgumentListKind::PossibleTypeArgumentList;
            }

            case TokenKind::IdentifierToken:
                // C#7: In certain contexts, we treat *identifier* as a disambiguating token. Those
                // contexts are where the sequence of tokens being disambiguated is immediately preceded by one
                // of the keywords is, case, or out, or arises while parsing the first element of a tuple literal
                // (in which case the tokens are preceded by `(` and the identifier is followed by a `,`) or a
                // subsequent element of a tuple literal (in which case the tokens are preceded by `,` and the
                // identifier is followed by a `,` or `)`).
                // In C#8 (or whenever recursive patterns are introduced) we also treat an identifier as a
                // disambiguating token if we're parsing the type of a pattern.
                // Note that we treat query contextual keywords (which appear here as identifiers) as disambiguating tokens as well.
                if ((options & (NameOptions::AfterIs | NameOptions::DefinitePattern | NameOptions::AfterOut)) != 0 ||
                    (options & NameOptions::AfterTupleComma) != 0 && parser->PeekToken(1).kind == TokenKind::CommaToken || parser->PeekToken(1).kind == TokenKind::CloseParenToken ||
                    (options & NameOptions::FirstElementOfPossibleTupleLiteral) != 0 && parser->PeekToken(1).kind == TokenKind::CommaToken) {
                    // we allow 'G<T,U> x' as a pattern-matching operation and a declaration expression in a tuple.
                    return ScanTypeArgumentListKind::DefiniteTypeArgumentList;
                }

                return ScanTypeArgumentListKind::PossibleTypeArgumentList;

            case TokenKind::EndOfFileToken:          // e.g. `e is A<B>`
                // This is useful for parsing expressions in isolation
                return ScanTypeArgumentListKind::DefiniteTypeArgumentList;

            case TokenKind::EqualsGreaterThanToken:  // e.g. `e switch { A<B> => 1 }`
                // This token was added to 7.5.4.2 Grammar Ambiguities in C# 9.0
                return ScanTypeArgumentListKind::DefiniteTypeArgumentList;

            default:
                return ScanTypeArgumentListKind::PossibleTypeArgumentList;
        }
    }

    bool TokenBreaksTypeArgumentList(SyntaxToken token) {

//        SyntaxKind contextualKind = token.contextualKind; //SyntaxFacts::GetContextualKeywordKind(parser->GetTokenText(token));

        switch (token.contextualKind) {
            // Example: x is IEnumerable<string or IList<int>
            case TokenKind::OrKeyword:
                // Example: x is IEnumerable<string and IDisposable
            case TokenKind::AndKeyword:
                return true;
            default:
                break;
        }

        switch (token.kind) {
            // Example: Method<string(argument)
            // Note: We would do a bad job handling a tuple argument with a missing comma,
            //       like: Method<string (int x, int y)>
            //       but since we do not look as far as possible to determine whether it is
            //       a tuple type or an argument list, we resort to considering it as an
            //       argument list
            case TokenKind::OpenParenToken:
                // Example: IEnumerable<string Method<T>() --- (< in <T>)
            case TokenKind::LessThanToken:
                // Example: Method(IEnumerable<string parameter)
            case TokenKind::CloseParenToken:
                // Example: IEnumerable<string field;
            case TokenKind::SemicolonToken:
                // Example: IEnumerable<string Property { get; set; }
            case TokenKind::OpenBraceToken:
                // Example:
                // {
                //     IEnumerable<string field
                // }
            case TokenKind::CloseBraceToken:
                // Examples:
                // - IEnumerable<string field = null;
                // - Method(IEnumerable<string parameter = null)
            case TokenKind::EqualsToken:
                // Example: IEnumerable<string Property => null;
            case TokenKind::EqualsGreaterThanToken:
                // Example: IEnumerable<string this[string key] { get; set; }
            case TokenKind::ThisKeyword:
                // Example: static IEnumerable<string operator +(A left, A right);
            case TokenKind::OperatorKeyword:
                return true;
            default:
                return false;
        }

    }

    TypeSyntax* ParseTypeArgument(Parser* parser) {

        // todo -- I think type arguments are just identifiers right? Or could they by TList<TValue>?
        TypeSyntax* result = ParseType(parser, ParseTypeMode::Normal);

        // Consider the case where someone supplies an invalid type argument
        // Such as Action<0> or Action<static>.  In this case we generate a missing
        // identifier in ParseType, but if we continue as is we'll immediately start to
        // interpret 0 as the start of a new expression when we can tell it's most likely
        // meant to be part of the type list.
        //
        // To solve this we check if the current token is not comma or greater than and
        // the next token is a comma or greater than. If so we assume that the found
        // token is part of this expression and we attempt to recover. This does open
        // the door for cases where we have an  incomplete line to be interpreted as
        // a single expression.  For example:
        //
        // Action< // Incomplete line
        // a>b;
        //
        // However, this only happens when the following expression is of the form a>...
        // or a,... which  means this case should happen less frequently than what we're
        // trying to solve here so we err on the side of better error messages
        // for the majority of cases.

        SyntaxToken current = parser->currentToken;
        SyntaxToken next = parser->PeekToken(1);

        if (result->IsMissing() && current.kind != TokenKind::CommaToken && current.kind != TokenKind::GreaterThanToken && next.kind == TokenKind::CommaToken || next.kind == TokenKind::GreaterThanToken) {
            // skip the current token so we can recover
            parser->SkipToken();
        }

        return result;
    }

    bool typeArgumentListNotExpectedFn(Parser* parser) {
        return parser->currentToken.kind != TokenKind::CommaToken && !IsPossibleType(parser);
    }

    bool typeArgumentListAbortFn(Parser* parser, TokenKind kind) {
        return parser->currentToken.kind == TokenKind::GreaterThanToken;
    }

    PostSkipAction SkipBadTypeArgumentListTokens(Parser* parser, TokenKind kind) {
        return SkipBadSeparatedListTokensWithExpectedKind(parser, &typeArgumentListNotExpectedFn, &typeArgumentListAbortFn, kind);
    }

    void ParseTypeArgumentList(Parser* parser, SyntaxToken* open, SeparatedSyntaxListBuilder<TypeSyntax>* builder, SyntaxToken* close) {
        assert(parser->currentToken.kind == TokenKind::LessThanToken);
        *open = parser->EatToken();

        builder->Add(ParseTypeArgument(parser));
        // remaining types & commas
        while (true) {
            if (parser->currentToken.kind == TokenKind::GreaterThanToken) {
                break;
            }

            // We prefer early terminating the argument list over parsing until exhaustion for better error recovery
            if (TokenBreaksTypeArgumentList(parser->currentToken)) {
                break;
            }

            // We are currently past parsing a type and we encounter an unexpected identifier token
            // followed by tokens that are not part of a type argument list
            // Example: List<(string a, string b) Method() { }
            //                 current token:     ^^^^^^
            if (parser->currentToken.kind == TokenKind::IdentifierToken && TokenBreaksTypeArgumentList(parser->PeekToken(1))) {
                break;
            }

            // This is for the case where we are in a this[] accessor, and the last one of the parameters in the parameter list
            // is missing a > on its type
            // Example: X this[IEnumerable<string parameter] =>
            //                 current token:     ^^^^^^^^^
            if (parser->currentToken.kind == TokenKind::IdentifierToken && parser->PeekToken(1).kind == TokenKind::CloseBracketToken) {
                break;
            }

            if (parser->currentToken.kind == TokenKind::CommaToken || IsPossibleType(parser)) {
                builder->AddSeparator(parser->EatToken(TokenKind::CommaToken));
                builder->Add(ParseTypeArgument(parser));
            }
            else if (SkipBadTypeArgumentListTokens(parser, TokenKind::CommaToken) == PostSkipAction::Abort) {
                break;
            }

        }

        *close = parser->EatToken(TokenKind::GreaterThanToken);

    }

    ExpressionSyntax* ParseExpression(Parser* parser) {
        return ParseSubExpression(parser, Precedence::Expression);
    }

    SimpleNameSyntax* ParseSimpleName(Parser* parser, NameOptions options) {
        IdentifierNameSyntax* id = ParseIdentifierName(parser);

        if (id->identifier.IsMissing()) {
            return id;
        }

        SimpleNameSyntax* name = id;
        ScanTypeArgumentListKind kind;
        if (parser->currentToken.kind == TokenKind::LessThanToken) {
            {
                ResetPoint resetPoint(parser);
                kind = ScanTypeArgumentList(parser, options);
            }

            if (kind == ScanTypeArgumentListKind::DefiniteTypeArgumentList || (kind == ScanTypeArgumentListKind::PossibleTypeArgumentList && (options & NameOptions::InTypeList) != 0)) {
                assert(parser->currentToken.kind == TokenKind::LessThanToken);
                SeparatedSyntaxListBuilder<TypeSyntax> builder(parser->tempAllocator);
                SyntaxToken open;
                SyntaxToken close;
                ParseTypeArgumentList(parser, &open, &builder, &close);

                name = parser->CreateNode<GenericNameSyntax>(
                    id->identifier,
                    parser->CreateNode<TypeArgumentListSyntax>(open, builder.ToList(parser->allocator), close)
                );
            }

        }

        return name;

    }

    NameSyntax* RecoverFromDotDot(Parser* parser, NameSyntax* left, SyntaxToken* separator) {
        assert(separator->kind == TokenKind::DotDotToken);

        IdentifierNameSyntax* missingName = CreateMissingIdentifierName(parser);
        SyntaxToken leftDot = MakeMissingToken(TokenKind::DotToken, parser->ptr);
        parser->AddError(*separator, Diagnostic(ErrorCode::ERR_IdentifierExpected, separator->GetText()));
        *separator = MakeMissingToken(TokenKind::DotToken, parser->ptr);
        return parser->CreateNode<QualifiedNameSyntax>(left, leftDot, missingName);
    }

    NameSyntax* ParseQualifiedNameRight(Parser* parser, NameOptions options, NameSyntax* left, SyntaxToken separator) {
        // assert(separator.Kind is SyntaxKind.DotToken or SyntaxKind.DotDotToken or SyntaxKind.ColonColonToken);
        SimpleNameSyntax* right = ParseSimpleName(parser, options);

        switch (separator.kind) {
            case TokenKind::DotToken:
                return parser->CreateNode<QualifiedNameSyntax>(left, separator, right);

            case TokenKind::DotDotToken:
                // Error recovery.  If we have `X..Y` break that into `X.<missing-id>.Y`
                return parser->CreateNode<QualifiedNameSyntax>(RecoverFromDotDot(parser, left, &separator), separator, right);

                // removing :: support for now, doesn't seem super helpful and is maybe just confusing

//            case TokenKind::ColonColonToken:
//
//                if (left->kind != TokenKind::IdentifierName) {
//                    separator = this.AddError(separator, ErrorCode::ERR_UnexpectedAliasedName);
//                }
//
//                // If the left hand side is not an identifier name then the user has done
//                // something like Goo.Bar::Blah. We've already made an error node for the
//                // ::, so just pretend that they typed Goo.Bar.Blah and continue on.
//
//                if (left->kind != TokenKind::IdentifierName) {
//                    separator = this.ConvertToMissingWithTrailingTrivia(separator, TokenKind::DotToken);
//                    return _syntaxFactory.QualifiedName(left, separator, right);
//                }
//                else {
//
////                    if (identifierLeft.Identifier.contextualKind == TokenKind::GlobalKeyword) {
////                        identifierLeft = _syntaxFactory.IdentifierName(ConvertToKeyword(identifierLeft.Identifier));
////                    }
//
//                    // If the name on the right had errors or warnings then we need to preserve them in the tree.
//                    return WithAdditionalDiagnostics(_syntaxFactory.AliasQualifiedName(identifierLeft, separator, right), left.GetDiagnostics());
//                }

            default:
                UNREACHABLE("ParseQualifiedNameRight");
                return nullptr;
        }
    }

    TypeSyntax* ParseQualifiedName(Parser* parser, NameOptions options) {
        NameSyntax* name = ParseSimpleName(parser, options);

        // Handle .. tokens for error recovery purposes.
        while (parser->currentToken.kind == TokenKind::DotToken || parser->currentToken.kind == TokenKind::DotDotToken) {

            if (parser->PeekToken(1).kind == TokenKind::ThisKeyword) {
                break;
            }

            SyntaxToken separator = parser->EatToken();
            name = ParseQualifiedNameRight(parser, options, name, separator);
        }

        return name;

    }

    SyntaxToken MakeMissingToken(TokenKind kind, int32 id) {
        assert(SyntaxFacts::IsToken(kind));
        SyntaxToken retn;
        retn.kind = kind;
        retn.SetId(id);
        retn.SetFlags(SyntaxTokenFlags::Missing);
        return retn;
    }

    IdentifierNameSyntax* CreateMissingIdentifierName(Parser* parser) {
        return parser->CreateNode<IdentifierNameSyntax>(MakeMissingToken(TokenKind::IdentifierToken, parser->ptr));
    }

    TupleElementSyntax* ParseTupleElement(Parser* parser) {
        return parser->CreateNode<TupleElementSyntax>(
            ParseType(parser, ParseTypeMode::Normal),
            IsTrueIdentifier(parser)
                ? ParseIdentifierToken(parser)
                : SyntaxToken()
        );
    }

    TypeSyntax* ParseTupleType(Parser* parser) {
        SyntaxToken open = parser->EatToken(TokenKind::OpenParenToken);
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SeparatedSyntaxListBuilder<TupleElementSyntax> list(parser->tempAllocator);

        if (parser->currentToken.kind != TokenKind::CloseParenToken) {
            list.Add(ParseTupleElement(parser));

            while (parser->currentToken.kind == TokenKind::CommaToken) {
                list.AddSeparator(parser->EatToken(TokenKind::CommaToken));
                list.Add(ParseTupleElement(parser));
            }
        }

        // todo -- not sure about these two checks, validate
        if (list.itemCount < 2) {

            if (list.itemCount == 0) {
                list.Add(parser->CreateNode<TupleElementSyntax>(CreateMissingIdentifierName(parser), SyntaxToken()));
            }

            IdentifierNameSyntax* missing = CreateMissingIdentifierName(parser);

            list.AddSeparator(MakeMissingToken(TokenKind::CommaToken, parser->ptr));
            list.Add(parser->CreateNode<TupleElementSyntax>(missing, SyntaxToken()));

            FixedCharSpan span = parser->currentToken.GetText();
            parser->diagnostics->AddError(Diagnostic(ErrorCode::ERR_TupleTooFewElements, span));
        }

        return parser->CreateNode<TupleTypeSyntax>(open, list.ToList(parser->allocator), parser->EatToken(TokenKind::CloseParenToken));

    }

    TypeSyntax* ParseUnderlyingType(Parser* parser, ParseTypeMode mode, NameOptions options = NameOptions::None) {
        if (SyntaxFacts::IsPredefinedType(parser->currentToken.kind)) {
            SyntaxToken token = parser->EatToken();
            if (token.kind == TokenKind::VoidKeyword) {
                ErrorCode code = mode == ParseTypeMode::Parameter ? ErrorCode::ERR_NoVoidParameter : ErrorCode::ERR_NoVoidHere;
                parser->AddError(token, parser->MakeDiagnostic(code, token));
            }

            return parser->CreateNode<PredefinedTypeSyntax>(token);
        }

        if (IsTrueIdentifier(parser)) {
            return ParseQualifiedName(parser, options);
        }

        if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            return ParseTupleType(parser);
        }

        // we don't advance in the error case, is that correct?
        IdentifierNameSyntax* retn = CreateMissingIdentifierName(parser);
        ErrorCode errorCode = mode == ParseTypeMode::NewExpression ? ErrorCode::ERR_BadNewExpr : ErrorCode::ERR_TypeExpected;
        FixedCharSpan span = parser->currentToken.GetText();
        parser->AddError(parser->currentToken, Diagnostic(errorCode, span.ptr, span.ptr + span.size));
        return retn;

    }

    bool CanStartExpression(Parser* parser) {
        return IsPossibleExpression(parser, false, false);
    }

    bool CanFollowNullableType(Parser* parser, ParseTypeMode mode) {
        switch (mode) {
            case ParseTypeMode::AfterIs:
            case ParseTypeMode::DefinitePattern:
            case ParseTypeMode::AsExpression: {
                // These contexts might be a type that is at the end of an expression. In these contexts we only
                // permit the nullable qualifier if it is followed by a token that could not start an
                // expression, because we want to consider a `?` token as part of the
                // `?:` operator if possible.
                //
                // Similarly, if we have `T?[]` we do want to treat that as an array of nullables not a conditional that returns a list.
                return !CanStartExpression(parser) || parser->currentToken.kind == TokenKind::OpenBracketToken;
            }
            case ParseTypeMode::NewExpression: {
                // A nullable qualifier is permitted as part of the type in a `new` expression.
                // e.g. `new int?()` is allowed.  It creates a null value of type `Nullable<int>`.
                // Similarly `new int? {}` is allowed.
                TokenKind kind = parser->currentToken.kind;

                return
                    kind == TokenKind::OpenParenToken ||   // ctor parameters
                        kind == TokenKind::OpenBracketToken ||   // array type
                        kind == TokenKind::OpenBraceToken;   // object initializer
            }
            default: {
                return true;
            }
        }
    }

    bool TryEatNullableQualifierIfApplicable(Parser* parser, ParseTypeMode mode, SyntaxToken* questionToken) {
        ResetPoint resetPoint(parser, false);
        *questionToken = parser->EatToken();
        if (!CanFollowNullableType(parser, mode)) {
            resetPoint.Reset();
            return false;
        }
        return true;
    }

    bool CanBeNullableType(Parser* parser, TypeSyntax* typeSyntax, ParseTypeMode mode) {
        // These are the fast tests for (in)applicability.
        // More expensive tests are in `TryEatNullableQualifierIfApplicable`
        if (typeSyntax->GetKind() == SyntaxKind::NullableType) {
            return false;
        }
        if (parser->PeekToken(1).kind == TokenKind::OpenBracketToken) {
            return true;
        }
        if (mode == ParseTypeMode::DefinitePattern) {
            return true; // Permit nullable type parsing and report while binding for a better error message
        }
        if (mode == ParseTypeMode::NewExpression && typeSyntax->GetKind() == SyntaxKind::TupleType && parser->PeekToken(1).kind != TokenKind::OpenParenToken && parser->PeekToken(1).kind != TokenKind::OpenBraceToken) {
            return false; // Permit `new (int, int)?(t)` (creation) and `new (int, int) ? x : y` (conditional)
        }

        return true;
    }

    PostSkipAction SkipBadTokensWithExpectedKind(Parser* parser, notExpectedFn isNotExpectedFunction, abortFn abortFunction, TokenKind expected, TokenKind closeKind) {
        int32 startToken = parser->ptr;
        bool first = true;
        PostSkipAction action = PostSkipAction::Continue;

        while (isNotExpectedFunction(parser)) {
            if (abortFunction(parser, closeKind) || IsTerminator(parser)) {
                action = PostSkipAction::Abort;
                break;
            }

            // if this token already contains diagnostics we don't want to add another one
            if (first && !parser->currentToken.ContainsDiagnostics()) {
                parser->SkipTokenWithPrejudice(expected);
            }
            else {
                parser->SkipToken();
            }

            first = false;

        }

        return action;
    }

    PostSkipAction SkipBadSeparatedListTokensWithExpectedKind(Parser* parser, notExpectedFn isNotExpectedFunction, abortFn abortFunction, TokenKind expected, TokenKind closeKind) {
        return SkipBadTokensWithExpectedKind(parser, isNotExpectedFunction, abortFunction, expected, closeKind);
    }

    bool arrayRankNotExpectedFn(Parser* parser) {
        return parser->currentToken.kind != TokenKind::CommaToken && !IsPossibleExpression(parser);
    }

    bool arrayRankAbortFn(Parser* parser, TokenKind kind) {
        return parser->currentToken.kind == TokenKind::CloseBracketToken;
    }

    PostSkipAction SkipBadArrayRankSpecifierTokens(Parser* parser, TokenKind expected) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            &arrayRankNotExpectedFn,
            &arrayRankAbortFn,
            expected
        );
    }

    SyntaxToken MakeOmittedToken(TokenKind kind, int32 position) {
        SyntaxToken omittedToken;
        omittedToken.kind = kind;
        omittedToken.SetId(position);
        omittedToken.SetFlags(SyntaxTokenFlags::Omitted);
        return omittedToken;
    }

    ArrayRankSpecifierSyntax* ParseArrayRankSpecifier(Parser* parser, bool* pSawNonOmittedSize) {
        *pSawNonOmittedSize = false;
        bool sawOmittedSize = false;
        SyntaxToken open = parser->EatToken(TokenKind::OpenBracketToken);
        TempAllocator::ScopedMarker scopedMarker(parser->tempAllocator);
        SeparatedSyntaxListBuilder<ExpressionSyntax> list(parser->tempAllocator);

        int32 lastTokenPosition = -1;
        while (IsMakingProgress(parser, &lastTokenPosition) && parser->currentToken.kind != TokenKind::CloseBracketToken) {

            if (parser->currentToken.kind == TokenKind::CommaToken) {
                sawOmittedSize = true;
                list.Add(parser->CreateNode<OmittedArraySizeExpressionSyntax>(MakeOmittedToken(TokenKind::OmittedArraySizeExpressionToken, parser->ptr)));
                list.AddSeparator(parser->EatToken());
            }
            else if (IsPossibleExpression(parser)) {
                ExpressionSyntax* size = ParseExpression(parser);
                *pSawNonOmittedSize = true;
                list.Add(size);
                if (parser->currentToken.kind != TokenKind::CloseBracketToken) {
                    list.AddSeparator(parser->EatToken(TokenKind::CommaToken));
                }
            }
            else if (SkipBadArrayRankSpecifierTokens(parser, TokenKind::CommaToken) == PostSkipAction::Abort) {
                break;
            }

        }

        // Don't end on a comma.
        // If the omitted size would be the only element, then skip it unless sizes were expected.
        if (list.separatorCount == list.itemCount + 1) {
            sawOmittedSize = true;
            list.Add(parser->CreateNode<OmittedArraySizeExpressionSyntax>(MakeOmittedToken(TokenKind::OmittedArraySizeExpressionToken, parser->ptr)));
        }

        // Never mix omitted and non-omitted array sizes.  If there were non-omitted array sizes,
        // then convert all of the omitted array sizes to missing identifiers.
        if (sawOmittedSize && *pSawNonOmittedSize) {
            for (int32 i = 0; i < list.itemCount; i++) {
                if (list.GetItem(i)->GetKind() == SyntaxKind::OmittedArraySizeExpression) {
                    SyntaxToken separator = list.GetSeparator(i);
                    FixedCharSpan text = separator.GetText(); // we need a text range for the error. try to use the next separator. todo -- probably fails on the end
                    parser->AddError(separator, Diagnostic(ErrorCode::ERR_ValueExpected, text.ptr, text.ptr + text.size));
                    list.items[i] = parser->CreateNode<IdentifierNameSyntax>(parser->CreateMissingToken(TokenKind::IdentifierToken));
                }
            }
        }

        return parser->CreateNode<ArrayRankSpecifierSyntax>(open, list.ToList(parser->allocator), parser->EatToken(TokenKind::CloseBracketToken));

    }

    TypeSyntax* ParseTypeCore(Parser* parser, ParseTypeMode mode) {
        NameOptions nameOptions;
        switch (mode) {
            case ParseTypeMode::AfterIs:
                nameOptions = NameOptions::InExpression | NameOptions::AfterIs | NameOptions::PossiblePattern;
                break;
            case ParseTypeMode::DefinitePattern:
                nameOptions = NameOptions::InExpression | NameOptions::DefinitePattern | NameOptions::PossiblePattern;
                break;
            case ParseTypeMode::AfterOut:
                nameOptions = NameOptions::InExpression | NameOptions::AfterOut;
                break;
            case ParseTypeMode::AfterTupleComma:
                nameOptions = NameOptions::InExpression | NameOptions::AfterTupleComma;
                break;
            case ParseTypeMode::FirstElementOfPossibleTupleLiteral:
                nameOptions = NameOptions::InExpression | NameOptions::FirstElementOfPossibleTupleLiteral;
                break;
            case ParseTypeMode::NewExpression:
            case ParseTypeMode::AsExpression:
            case ParseTypeMode::Normal:
            case ParseTypeMode::Parameter:
            case ParseTypeMode::AfterRef:
                nameOptions = NameOptions::None;
                break;
            default:
                NOT_IMPLEMENTED("invalid value");
                break;
        }

        TypeSyntax* type = ParseUnderlyingType(parser, mode, nameOptions);
        int32 lastTokenPosition = -1;
        while (IsMakingProgress(parser, &lastTokenPosition)) {
            switch (parser->currentToken.kind) {
                case TokenKind::QuestionToken: {
                    if (CanBeNullableType(parser, type, mode)) {
                        SyntaxToken question;
                        if (TryEatNullableQualifierIfApplicable(parser, mode, &question)) {
                            type = parser->CreateNode<NullableType>(type, question);
                            continue;
                        }
                    }
                    goto done; // token not consumed

                }
                case TokenKind::OpenBracketToken: {
                    // Now check for arrays.
                    TempAllocator::ScopedMarker marker(parser->tempAllocator);
                    SyntaxListBuilder<ArrayRankSpecifierSyntax> ranks(parser->tempAllocator);
                    do {
                        bool unused = false;
                        ranks.Add(ParseArrayRankSpecifier(parser, &unused));
                    } while (parser->currentToken.kind == TokenKind::OpenBracketToken);

                    type = parser->CreateNode<ArrayTypeSyntax>(type, ranks.ToSyntaxList(parser->allocator));
                    continue;
                }
                default:
                    goto done; // token not consumed
            }
        }
        done:
        return type;
    }

    TypeSyntax* ParseType(Parser* parser, ParseTypeMode mode) {

        if (parser->currentToken.kind == TokenKind::RefKeyword) {

            return parser->CreateNode<RefTypeSyntax>(
                parser->EatToken(),
                parser->currentToken.kind == TokenKind::ReadOnlyKeyword ? parser->EatToken() : SyntaxToken(),
                ParseTypeCore(parser, ParseTypeMode::AfterRef)
            );

        }

        return ParseTypeCore(parser, mode);
    }
}