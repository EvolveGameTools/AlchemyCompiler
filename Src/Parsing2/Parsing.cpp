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

                while ((immediatelyAbort == nullptr || !immediatelyAbort(parser, node)) && IsMakingProgress(parser, &lastTokenPosition)) {
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
        return parser->currentToken.contextualKind == TokenKind::WhereKeyword &&
            parser->PeekToken(1).kind == TokenKind::IdentifierToken &&
            parser->PeekToken(2).kind == TokenKind::ColonToken;
    }

    bool IsTrueIdentifier(SyntaxToken token) {
        return token.kind == TokenKind::IdentifierToken;
    }

    bool IsTrueIdentifier(Parser* parser) {
        if (parser->currentToken.kind == TokenKind::IdentifierToken
            && !IsCurrentTokenPartialKeywordOfPartialMethodOrType(parser)
            && !IsCurrentTokenWhereOfConstraintClause(parser)) {
            return true;
        }

        return false;
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
        NOT_IMPLEMENTED("ScanParenthesizedLambda");
        return false;
    }

    struct NamespaceBodyBuilder {

        SyntaxListBuilder<MemberDeclarationSyntax>* members;

        explicit NamespaceBodyBuilder(SyntaxListBuilder<MemberDeclarationSyntax>* members)
            : members(members)
        {}
//    public SyntaxListBuilder<ExternAliasDirectiveSyntax> Externs;
//    public SyntaxListBuilder<UsingDirectiveSyntax> Usings;
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
//            case TokenKind::ExternKeyword:
//                return DeclarationModifiers.Extern;
//            case TokenKind::NewKeyword:
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

    DelegateDeclarationSyntax* ParseDelegateDeclaration(Parser* parser, SyntaxList<AttributeListSyntax>* attributes, TokenListBuffer* modifiers) {
        assert(parser->currentToken.kind == TokenKind::DelegateKeyword);

        SyntaxToken delegateToken = parser->EatToken(TokenKind::DelegateKeyword);
        TypeSyntax* type = ParseReturnType(parser);
        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfMethodSignature;
        SyntaxToken name = ParseIdentifierToken(parser);
        TypeParameterListSyntax* typeParameters = ParseTypeParameterList(parser);
        ParameterListSyntax* parameterList = ParseParenthesizedParameterList(parser);

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<TypeParameterConstraintClauseSyntax> constraints(parser->tempAllocator);

        if (parser->currentToken.contextualKind == TokenKind::WhereKeyword) {
            ParseTypeParameterConstraintClauses(parser, &constraints);
        }

        parser->termState = saveTerm;

        SyntaxToken semicolon = parser->EatToken(TokenKind::SemicolonToken);

        return parser->CreateNode<DelegateDeclarationSyntax>(
            attributes,
            modifiers->Persist(parser->allocator),
            delegateToken,
            type,
            name,
            typeParameters,
            parameterList,
            constraints.ToSyntaxList(parser->allocator),
            semicolon
        );

    }

    bool IsPossibleEnumMemberDeclaration(Parser* parser) {
        return parser->currentToken.kind == TokenKind::OpenBracketToken || IsTrueIdentifier(parser);
    }

    PostSkipAction SkipBadEnumMemberListTokens(Parser* parser, TokenKind expected, TokenKind closeKind) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* parser) { return parser->currentToken.kind != TokenKind::CommaToken && parser->currentToken.kind != TokenKind::SemicolonToken && !IsPossibleEnumMemberDeclaration(parser); },
            [](Parser* parser, TokenKind closeKind) { return parser->currentToken.kind == closeKind; },
            expected,
            closeKind
        );
    }

    EnumMemberDeclarationSyntax* ParseEnumMemberDeclaration(Parser* parser) {

        SyntaxList<AttributeListSyntax>* memberAttrs = ParseAttributeDeclarations(parser);
        SyntaxToken memberName = ParseIdentifierToken(parser);
        EqualsValueClauseSyntax* equalsValue = nullptr;
        if (parser->currentToken.kind == TokenKind::EqualsToken) {

            SyntaxToken equalsToken = parser->EatToken();

            ExpressionSyntax* value = parser->currentToken.kind == TokenKind::CommaToken || parser->currentToken.kind == TokenKind::CloseBraceToken
                ? ParseIdentifierName(parser, ErrorCode::ERR_ConstantExpected)
                : ParseExpression(parser);

            // an identifier is a valid expression
            equalsValue = parser->CreateNode<EqualsValueClauseSyntax>(equalsToken, value);
        }

        return parser->CreateNode<EnumMemberDeclarationSyntax>(memberAttrs, memberName, equalsValue);
    }

    EnumDeclarationSyntax* ParseEnumDeclaration(Parser* parser, SyntaxList<AttributeListSyntax>* attributes, TokenListBuffer* modifiers) {
        assert(parser->currentToken.kind == TokenKind::EnumKeyword);

        SyntaxToken enumToken = parser->EatToken();
        SyntaxToken name = ParseIdentifierToken(parser);

        // check to see if the user tried to create a generic enum.
        TypeParameterListSyntax* typeParameters = ParseTypeParameterList(parser);

        if (typeParameters != nullptr) {
            parser->AddError(typeParameters, ErrorCode::ERR_UnexpectedGenericName);
        }

        BaseListSyntax* baseList = nullptr;
        if (parser->currentToken.kind == TokenKind::ColonToken) {
            SyntaxToken colon = parser->EatToken(TokenKind::ColonToken);
            TypeSyntax* type = ParseType(parser);
            TempAllocator::ScopedMarker m(parser->tempAllocator);
            SeparatedSyntaxListBuilder<BaseTypeSyntax> tmpList(parser->tempAllocator);

            tmpList.Add(parser->CreateNode<SimpleBaseTypeSyntax>(type));
            baseList = parser->CreateNode<BaseListSyntax>(colon, tmpList.ToList(parser->allocator));
        }

        SeparatedSyntaxList<EnumMemberDeclarationSyntax>* members = nullptr;
        SyntaxToken semicolon;
        SyntaxToken openBrace;
        SyntaxToken closeBrace;

        if (parser->currentToken.kind == TokenKind::SemicolonToken) {
            semicolon = parser->EatToken();
        }
        else {
            openBrace = parser->EatToken(TokenKind::OpenBraceToken);

            if (!openBrace.IsMissing()) {
                // It's not uncommon for people to use semicolons to separate out enum members.  So be resilient to
                // that, successfully consuming them as separators, while telling the user it needs to be a comma
                // instead.
                members = ParseCommaSeparatedSyntaxList<EnumMemberDeclarationSyntax>(
                    parser,
                    &openBrace,
                    TokenKind::CloseBraceToken,
                    [](Parser* p) { return IsPossibleEnumMemberDeclaration(p); },
                    [](Parser* p) { return (SyntaxBase*) ParseEnumMemberDeclaration(p); },
                    &SkipBadEnumMemberListTokens,
                    true,
                    false,
                    true
                );
            }

            closeBrace = parser->EatToken(TokenKind::CloseBraceToken);
            if (parser->currentToken.kind == TokenKind::SemicolonToken) {
                semicolon = parser->EatToken();
            }

        }

        return parser->CreateNode<EnumDeclarationSyntax>(
            attributes,
            modifiers->Persist(parser->allocator),
            enumToken,
            name,
            baseList,
            openBrace,
            members,
            closeBrace,
            semicolon
        );

    }

    bool IsPossibleAttribute(Parser* parser) {
        return IsTrueIdentifier(parser);
    }

    PostSkipAction SkipBadBaseListTokens(Parser* parser, TokenKind expected) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleAttribute(p); }, // todo -- I'm not sure we can accept attributes here
            [](Parser* p, TokenKind unused) { return p->currentToken.kind == TokenKind::OpenBraceToken || IsCurrentTokenWhereOfConstraintClause(p); },
            expected,
            TokenKind::None
        );
    }

    BaseListSyntax* ParseBaseList(Parser* parser) {
        if (parser->currentToken.kind != TokenKind::ColonToken) {
            return nullptr;
        }

        SyntaxToken colon = parser->EatToken();

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SeparatedSyntaxListBuilder<BaseTypeSyntax> list(parser->tempAllocator);

        TypeSyntax* firstType = ParseType(parser);

        ArgumentListSyntax* argumentList = parser->currentToken.kind == TokenKind::OpenParenToken
            ? ParseParenthesizedArgumentList(parser)
            : nullptr;

        list.Add(argumentList != nullptr
            ? (BaseTypeSyntax*) parser->CreateNode<PrimaryConstructorBaseTypeSyntax>(firstType, argumentList)
            : (BaseTypeSyntax*) parser->CreateNode<SimpleBaseTypeSyntax>(firstType)
        );

        // any additional types
        while (true) {
            // todo -- we will likely want to change the where : condition to support our version of generics
            if (parser->currentToken.kind == TokenKind::OpenBraceToken ||
                ((parser->termState & TerminatorState::IsEndOfClassOrStructOrInterfaceSignature) != 0 && parser->currentToken.kind == TokenKind::SemicolonToken) ||
                IsCurrentTokenWhereOfConstraintClause(parser)) {
                break;
            }
            else if (parser->currentToken.kind == TokenKind::CommaToken || IsPossibleType(parser)) {
                list.AddSeparator(parser->EatToken(TokenKind::CommaToken));
                list.Add(parser->CreateNode<SimpleBaseTypeSyntax>(ParseType(parser)));
                continue;
            }
            else if (SkipBadBaseListTokens(parser, TokenKind::CommaToken) == PostSkipAction::Abort) {
                break;
            }
        }

        return parser->CreateNode<BaseListSyntax>(colon, list.ToList(parser->allocator));

    }

    bool IsPossibleTypeParameterConstraint(Parser* parser) {
        switch (parser->currentToken.kind) {
            case TokenKind::NewKeyword:
            case TokenKind::ClassKeyword:
            case TokenKind::StructKeyword:
                return true;
            case TokenKind::IdentifierToken:
                return IsTrueIdentifier(parser);
            default:
                return SyntaxFacts::IsPredefinedType(parser->currentToken.kind);
        }
    }

    TypeParameterConstraintSyntax* ParseTypeParameterConstraint(Parser* parser) {
        switch (parser->currentToken.kind) {
            case TokenKind::NewKeyword: {
                SyntaxToken newKeyword = parser->EatToken();
                SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);
                SyntaxToken closeParen = parser->EatToken(TokenKind::CloseParenToken);
                return parser->CreateNode<ConstructorConstraintSyntax>(newKeyword, openParen, closeParen);
            }
            case TokenKind::StructKeyword: {
                SyntaxToken keyword = parser->EatToken();
                SyntaxToken question;
                if (parser->currentToken.kind == TokenKind::QuestionToken) {
                    question = parser->EatToken();
                    parser->AddError(question, ErrorCode::ERR_UnexpectedToken);
                }
                return parser->CreateNode<ClassOrStructConstraintSyntax>(SyntaxKind::StructConstraint, keyword, question);
            }
            case TokenKind::ClassKeyword: {
                SyntaxToken keyword = parser->EatToken();
                SyntaxToken question;
                if (parser->currentToken.kind == TokenKind::QuestionToken) {
                    question = parser->EatToken();
                }
                return parser->CreateNode<ClassOrStructConstraintSyntax>(SyntaxKind::ClassConstraint, keyword, question);
            }
            case TokenKind::DelegateKeyword: {
                IdentifierNameSyntax* missing = CreateMissingIdentifierName(parser);
                parser->AddError(missing, ErrorCode::ERR_NoDelegateConstraint);
                parser->EatToken();
                return parser->CreateNode<TypeConstraintSyntax>(missing);
            }
                // todo -- maybe add constraint for enum keyword
            default: {
                return parser->CreateNode<TypeConstraintSyntax>(ParseType(parser));
            }
        }
    }

    PostSkipAction SkipBadTypeParameterConstraintTokens(Parser* parser, TokenKind expected) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* parser) { return parser->currentToken.kind != TokenKind::CommaToken && !IsPossibleTypeParameterConstraint(parser); },
            [](Parser* parser, TokenKind unused) { return parser->currentToken.kind == TokenKind::OpenBraceToken || IsCurrentTokenWhereOfConstraintClause(parser); },
            expected,
            TokenKind::None
        );
    }

    TypeParameterConstraintClauseSyntax* ParseTypeParameterConstraintClause(Parser* parser) {
        SyntaxToken where = parser->EatContextualToken(TokenKind::WhereKeyword);
        IdentifierNameSyntax* name = nullptr;
        if (!IsTrueIdentifier(parser)) {
            name = CreateMissingIdentifierName(parser);
            parser->AddError(name, ErrorCode::ERR_IdentifierExpected);
        }
        else {
            name = ParseIdentifierName(parser);
        }

        SyntaxToken colon = parser->EatToken(TokenKind::ColonToken);

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SeparatedSyntaxListBuilder<TypeParameterConstraintSyntax> bounds(parser->tempAllocator);

        if (parser->currentToken.kind == TokenKind::OpenBraceToken || IsCurrentTokenWhereOfConstraintClause(parser)) {
            IdentifierNameSyntax* missing = CreateMissingIdentifierName(parser);
            parser->AddError(missing, ErrorCode::ERR_TypeExpected);
            bounds.Add(parser->CreateNode<TypeConstraintSyntax>(missing));
        }
        else {
            bounds.Add(ParseTypeParameterConstraint(parser));
            while (true) {
                if (parser->currentToken.kind == TokenKind::OpenBraceToken
                    || ((parser->termState & TerminatorState::IsEndOfClassOrStructOrInterfaceSignature) != 0 && parser->currentToken.kind == TokenKind::SemicolonToken)
                    || parser->currentToken.kind == TokenKind::EqualsGreaterThanToken
                    || parser->currentToken.contextualKind == TokenKind::WhereKeyword) {
                    break;
                }
                else if (parser->currentToken.kind == TokenKind::CommaToken || IsPossibleTypeParameterConstraint(parser)) {
                    bounds.AddSeparator(parser->EatToken(TokenKind::CommaToken));
                    if (IsCurrentTokenWhereOfConstraintClause(parser)) {
                        IdentifierNameSyntax* missing = CreateMissingIdentifierName(parser);
                        parser->AddError(missing, ErrorCode::ERR_TypeExpected);
                        bounds.Add(parser->CreateNode<TypeConstraintSyntax>(missing));
                        break;
                    }
                    else {
                        bounds.Add(ParseTypeParameterConstraint(parser));
                    }
                }
                else if (SkipBadTypeParameterConstraintTokens(parser, TokenKind::CommaToken) == PostSkipAction::Abort) {
                    break;
                }
            }
        }

        return parser->CreateNode<TypeParameterConstraintClauseSyntax>(
            where,
            name,
            colon,
            bounds.ToList(parser->allocator)
        );

    }

    void ParseTypeParameterConstraintClauses(Parser* parser, SyntaxListBuilder<TypeParameterConstraintClauseSyntax>* list) {
        while (parser->currentToken.contextualKind == TokenKind::WhereKeyword) {
            list->Add(ParseTypeParameterConstraintClause(parser));
        }
    }

    void SkipBadMemberListTokens(Parser* parser) {
        int32 curlyCount = 0;

        bool done = false;

        // always consume at least one token.
        SyntaxToken token = parser->EatToken();
        parser->AddError(token, ErrorCode::ERR_InvalidMemberDecl);

        while (!done) {
            TokenKind kind = parser->currentToken.kind;

            // If this token can start a member, we're done
            if (CanStartMember(kind) && !(kind == TokenKind::DelegateKeyword && parser->PeekToken(1).kind == TokenKind::OpenBraceToken || parser->PeekToken(1).kind == TokenKind::OpenParenToken)) {
                done = true;
                continue;
            }

            // Watch curlies and look for end of file/close curly
            switch (kind) {
                case TokenKind::OpenBraceToken:
                    curlyCount++;
                    break;

                case TokenKind::CloseBraceToken:
                    if (curlyCount-- == 0) {
                        done = true;
                        continue;
                    }

                    break;

                case TokenKind::EndOfFileToken:
                    done = true;
                    continue;

                default:
                    break;
            }

            parser->EatToken();
        }

    }

    MemberDeclarationSyntax* ParseClassOrStructOrInterfaceDeclaration(Parser* parser, SyntaxList<AttributeListSyntax>* attributes, TokenListBuffer* modifiers) {

        SyntaxToken keyword = parser->EatToken();
        SyntaxKind topLevelKind;
        switch (keyword.kind) {
            case TokenKind::ClassKeyword: {
                topLevelKind = SyntaxKind::ClassDeclaration;
                break;
            }
            case TokenKind::StructKeyword: {
                topLevelKind = SyntaxKind::StructDeclaration;
                break;
            }
            case TokenKind::InterfaceKeyword: {
                topLevelKind = SyntaxKind::InterfaceDeclaration;
                break;
            }
            default: {
                UNREACHABLE("ParseClassOrStructOrInterfaceDeclaration");
                topLevelKind = SyntaxKind::ClassDeclaration;
            }
        }

        TerminatorState outerSaveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfClassOrStructOrInterfaceSignature;

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsPossibleAggregateClauseStartOrStop;

        SyntaxToken name = ParseIdentifierToken(parser);
        TypeParameterListSyntax* typeParameters = ParseTypeParameterList(parser);

        // for primary constructors
        ParameterListSyntax* paramList = parser->currentToken.kind == TokenKind::OpenParenToken
            ? ParseParenthesizedParameterList(parser)
            : nullptr;

        BaseListSyntax* baseList = ParseBaseList(parser);
        parser->termState = saveTerm;

        TempAllocator::ScopedMarker m(parser->tempAllocator);

        bool parseMembers = true;
        SyntaxListBuilder<MemberDeclarationSyntax> members(parser->tempAllocator);
        SyntaxListBuilder<TypeParameterConstraintClauseSyntax> constraints(parser->tempAllocator);

        if (parser->currentToken.contextualKind == TokenKind::WhereKeyword) {
            ParseTypeParameterConstraintClauses(parser, &constraints);
        }

        parser->termState = outerSaveTerm;

        SyntaxToken semicolon;
        SyntaxToken openBrace;
        SyntaxToken closeBrace;
        if (parser->currentToken.kind == TokenKind::SemicolonToken) {
            semicolon = parser->EatToken(TokenKind::SemicolonToken);
        }
        else {
            openBrace = parser->EatToken(TokenKind::OpenBraceToken);
            // ignore members if missing open curly
            if (openBrace.IsMissing()) {
                parseMembers = false;
            }
            // even if we saw a { or think we should parse members bail out early since
            // we know namespaces can't be nested inside types
            if (parseMembers) {

                while (true) {
                    TokenKind kind = parser->currentToken.kind;

                    if (CanStartMember(kind)) {
                        // This token can start a member -- go parse it
                        TerminatorState saveTerm2 = parser->termState;
                        parser->termState |= TerminatorState::IsPossibleMemberStartOrStop;

                        MemberDeclarationSyntax* member = ParseMemberDeclaration(parser, topLevelKind);
                        if (member != nullptr) {
                            // statements are accepted here, a semantic error will be reported later
                            members.Add(member);
                        }
                        else {
                            // we get here if we couldn't parse the lookahead as a statement or a declaration (we haven't consumed any tokens):
                            SkipBadMemberListTokens(parser);
                        }

                        parser->termState = saveTerm2;
                    }
                    else if (kind == TokenKind::CloseBraceToken || kind == TokenKind::EndOfFileToken || IsTerminator(parser)) {
                        // This marks the end of members of this class
                        break;
                    }
                    else {
                        // Error -- try to sync up with intended reality
                        SkipBadMemberListTokens(parser);
                    }
                }
            }

            if (openBrace.IsMissing()) {
                closeBrace = MakeMissingToken(TokenKind::CloseBraceToken, parser->ptr);
                ErrorCode errorCode = parser->GetExpectedTokenErrorCode(TokenKind::CloseBraceToken, parser->currentToken.kind);
                parser->AddError(closeBrace, errorCode);
            }
            else {
                closeBrace = parser->EatToken(TokenKind::CloseBraceToken);
            }

            if (parser->currentToken.kind == TokenKind::SemicolonToken) {
                semicolon = parser->EatToken();
            }

        }

        switch (topLevelKind) {
            case SyntaxKind::ClassDeclaration: {
                return parser->CreateNode<ClassDeclarationSyntax>(
                    attributes,
                    modifiers->Persist(parser->allocator),
                    keyword,
                    name,
                    typeParameters,
                    paramList,
                    baseList,
                    constraints.ToSyntaxList(parser->allocator),
                    openBrace,
                    members.ToSyntaxList(parser->allocator),
                    closeBrace,
                    semicolon
                );
            }

            case SyntaxKind::StructDeclaration: {
                return parser->CreateNode<StructDeclarationSyntax>(
                    attributes,
                    modifiers->Persist(parser->allocator),
                    keyword,
                    name,
                    typeParameters,
                    paramList,
                    baseList,
                    constraints.ToSyntaxList(parser->allocator),
                    openBrace,
                    members.ToSyntaxList(parser->allocator),
                    closeBrace,
                    semicolon
                );
            }

            case SyntaxKind::InterfaceDeclaration: {
                return parser->CreateNode<InterfaceDeclarationSyntax>(
                    attributes,
                    modifiers->Persist(parser->allocator),
                    keyword,
                    name,
                    typeParameters,
                    paramList,
                    baseList,
                    constraints.ToSyntaxList(parser->allocator),
                    openBrace,
                    members.ToSyntaxList(parser->allocator),
                    closeBrace,
                    semicolon
                );
            }
            default: {
                UNREACHABLE("ParseClassOrStructOrInterfaceDeclaration");
                return nullptr;
            }
        }

    }

    MemberDeclarationSyntax* ParseTypeDeclaration(Parser* parser, SyntaxList<AttributeListSyntax>* attributes, TokenListBuffer* modifiers) {
        switch (parser->currentToken.kind) {
            case TokenKind::ClassKeyword:
            case TokenKind::StructKeyword:
            case TokenKind::InterfaceKeyword:
                return ParseClassOrStructOrInterfaceDeclaration(parser, attributes, modifiers);

            case TokenKind::DelegateKeyword:
                return ParseDelegateDeclaration(parser, attributes, modifiers);

            case TokenKind::EnumKeyword:
                return ParseEnumDeclaration(parser, attributes, modifiers);

            default:
                UNREACHABLE("ParseTypeDeclaration");
                break;
        }
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


    bool IsVarType(Parser* parser) {
        // todo -- we might just always return true for this
        if (parser->currentToken.kind != TokenKind::VarKeyword) {
            return false;
        }

        switch (parser->PeekToken(1).kind) {
            case TokenKind::DotToken:
            case TokenKind::ColonColonToken:
            case TokenKind::OpenBracketToken:
            case TokenKind::AsteriskToken:
            case TokenKind::QuestionToken:
            case TokenKind::LessThanToken:
                return false;
            default:
                return true;
        }
    }

    bool IsPossibleArgumentExpression(Parser* parser) {
        return SyntaxFacts::IsValidArgumentRefKindKeyword(parser->currentToken.kind) || IsPossibleExpression(parser);
    }

    bool ScanDesignation(Parser* parser, bool permitTuple) {
        switch (parser->currentToken.kind) {

            case TokenKind::IdentifierToken: {
                bool result = IsTrueIdentifier(parser);
                parser->EatToken();
                return result;
            }
            case TokenKind::OpenParenToken: {
                if (!permitTuple) {
                    return false;
                }

                bool sawComma = false;
                while (true) {
                    parser->EatToken(); // consume the `(` or `,`
                    if (!ScanDesignation(parser, true)) {
                        return false;
                    }
                    switch (parser->currentToken.kind) {
                        case TokenKind::CloseParenToken:
                            parser->EatToken();
                            return sawComma;
                        case TokenKind::CommaToken:
                            sawComma = true;
                            continue;
                        default:
                            return false;
                    }
                }
            }
            default: {
                return false;
            }
        }
    }

    VariableDesignationSyntax* ParseDesignation(Parser* parser, bool forPattern) {
        // the two forms of designation are
        // (1) identifier
        // (2) ( designation ... )
        // for pattern-matching, we permit the designation list to be empty
        VariableDesignationSyntax* result;
        if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);

            TempAllocator::ScopedMarker m(parser->tempAllocator);
            SeparatedSyntaxListBuilder<VariableDesignationSyntax> listOfDesignations(parser->tempAllocator);

            bool done = false;
            if (forPattern) {
                done = (parser->currentToken.kind == TokenKind::CloseParenToken);
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

            result = parser->CreateNode<ParenthesizedVariableDesignationSyntax>(
                openParen,
                listOfDesignations.ToList(parser->allocator),
                parser->EatToken(TokenKind::CloseParenToken)
            );
        }
        else {
            result = ParseSimpleDesignation(parser);
        }

        return result;
    }

    bool IsPossibleDeclarationExpression(Parser* parser, ParseTypeMode mode, bool permitTupleDesignation, bool* pIsScoped) {
        assert(mode == ParseTypeMode::Normal || mode == ParseTypeMode::FirstElementOfPossibleTupleLiteral || mode == ParseTypeMode::AfterTupleComma);
        *pIsScoped = false;

        ResetPoint resetPoint(parser);

        // todo -- we aren't really supported 'scoped' in the same way as C#
//        if (parser->currentToken.contextualKind == TokenKind::ScopedKeyword) {
//            parser->EatToken();
//            if (ScanType(parser, false) != ScanTypeFlags::NotType && parser->currentToken.kind == TokenKind::IdentifierToken) {
//                switch (mode)
//                {
//                    case ParseTypeMode::FirstElementOfPossibleTupleLiteral:
//                        if (parser->PeekToken(1).kind == TokenKind::CommaToken) {
//                            *pIsScoped = true;
//                            return true;
//                        }
//                        break;
//
//                    case ParseTypeMode::AfterTupleComma:
//                        if (parser->PeekToken(1).kind == TokenKind::CommaToken || parser->PeekToken(1).kind == TokenKind::CloseParenToken) {
//                            *pIsScoped = true;
//                            return true;
//                        }
//                        break;
//
//                    default:
//                        // The other case where we disambiguate between a declaration and expression is before the `in` of a foreach loop.
//                        // There we err on the side of accepting a declaration.
//                        *pIsScoped = true;
//                        return true;
//                }
//            }
//
//            resetPoint.Reset();
//        }

        bool typeIsVar = IsVarType(parser);
        SyntaxToken lastTokenOfType;
        if (ScanType(parser, mode, &lastTokenOfType) == ScanTypeFlags::NotType) {
            return false;
        }

        // check for a designation
        if (!ScanDesignation(parser, permitTupleDesignation && (typeIsVar || SyntaxFacts::IsPredefinedType(lastTokenOfType.kind)))) {
            return false;
        }

        switch (mode) {
            case ParseTypeMode::FirstElementOfPossibleTupleLiteral:
                return parser->currentToken.kind == TokenKind::CommaToken;
            case ParseTypeMode::AfterTupleComma:
                return parser->currentToken.kind == TokenKind::CommaToken || parser->currentToken.kind == TokenKind::CloseParenToken;
            default:
                // The other case where we disambiguate between a declaration and expression is before the `in` of a foreach loop.
                // There we err on the side of accepting a declaration. (Matt) Alchemy probably doesn't have an 'in' keyword like C#
                return true;
        }
    }

    // Parse an expression where a declaration expression would be permitted. This is suitable for use after
    // the `out` keyword in an argument list, or in the elements of a tuple literal (because they may
    // be on the left-hand-side of a positional subpattern). The first element of a tuple is handled slightly
    // differently, as we check for the comma before concluding that the identifier should cause a
    // disambiguation. For example, for the input `(A < B , C > D)`, we treat this as a tuple with
    // two elements, because if we considered the `A<B,C>` to be a type, it wouldn't be a tuple at
    // all. Since we don't have such a thing as a one-element tuple (even for positional subpattern), the
    // absence of the comma after the `D` means we don't treat the `D` as contributing to the
    // disambiguation of the expression/type. More formally, ...
    //
    // If a sequence of tokens can be parsed(in context) as a* simple-name* (§7.6.3), *member-access* (§7.6.5),
    // or* pointer-member-access* (§18.5.2) ending with a* type-argument-list* (§4.4.1), the token immediately
    // following the closing `>` token is examined, to see if it is
    // - One of `(  )  ]  }  :  ;  ,  .  ?  ==  !=  |  ^  &&  ||  &  [`; or
    // - One of the relational operators `<  >  <=  >=  is as`; or
    // - A contextual query keyword appearing inside a query expression; or
    // - In certain contexts, we treat *identifier* as a disambiguating token.Those contexts are where the
    //   sequence of tokens being disambiguated is immediately preceded by one of the keywords `is`, `case`
    //   or `out`, or arises while parsing the first element of a tuple literal(in which case the tokens are
    //   preceded by `(` or `:` and the identifier is followed by a `,`) or a subsequent element of a tuple literal.
    //
    // If the following token is among this list, or an identifier in such a context, then the *type-argument-list* is
    // retained as part of the *simple-name*, *member-access* or  *pointer-member-access* and any other possible parse
    // of the sequence of tokens is discarded.Otherwise, the *type-argument-list* is not considered to be part of the
    // *simple-name*, *member-access* or *pointer-member-access*, even if there is no other possible parse of the
    // sequence of tokens.Note that these rules are not applied when parsing a *type-argument-list* in a *namespace-or-type-name* (§3.8).
    //
    // See also ScanTypeArgumentList where these disambiguation rules are encoded.
    ExpressionSyntax* ParseExpressionOrDeclaration(Parser* parser, ParseTypeMode mode, bool permitTupleDesignation) {
        bool isScoped = false;
        return IsPossibleDeclarationExpression(parser, mode, permitTupleDesignation, &isScoped)
            ? ParseDeclarationExpression(parser, mode, isScoped)
            : ParseSubExpression(parser, Precedence::Expression);
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

    bool IsPossibleCollectionElement(Parser* parser) {
        return IsPossibleExpression(parser);
    }

    TokenList* ParseAnonymousFunctionModifiers(Parser* parser) {
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        TokenListBuffer modifiers(parser->tempAllocator);

        while (parser->currentToken.kind == TokenKind::StaticKeyword) {
            modifiers.Add(parser->EatToken(TokenKind::StaticKeyword));
        }

        return modifiers.Persist(parser->allocator);
    }

    bool IsPossibleLambdaParameter(Parser* parser) {
        switch (parser->currentToken.kind) {
            case TokenKind::ParamsKeyword:
            case TokenKind::ReadOnlyKeyword:
            case TokenKind::RefKeyword:
            case TokenKind::OutKeyword:
//            case TokenKind::InKeyword:
            case TokenKind::OpenParenToken:   // tuple
//            case TokenKind::OpenBracketToken: // attribute
                return true;

            case TokenKind::IdentifierToken:
                return IsTrueIdentifier(parser);

            default:
                return SyntaxFacts::IsPredefinedType(parser->currentToken.kind);
        }
    }

    PostSkipAction SkipBadLambdaParameterListTokens(Parser* parser, TokenKind expectedKind, TokenKind closeKind) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleLambdaParameter(p); },
            [](Parser* p, TokenKind closeKind) { return p->currentToken.kind == closeKind; },
            expectedKind,
            closeKind
        );
    }

    bool ShouldParseLambdaParameterType(Parser* parser) {
        // If we have "int/string/etc." always parse out a type.
        if (SyntaxFacts::IsPredefinedType(parser->currentToken.kind)) {
            return true;
        }

        // if we have a tuple type in a lambda.
        if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            return true;
        }

        // Don't parse out a type if we see:
        //
        //      (a,
        //      (a)
        //      (a =>
        //      (a {
        //      (a !!    or    (a !!= -- alchemy doesn't use null surpression
        //      (a =
        //
        // In all other cases, parse out a type.
        if (IsTrueIdentifier(parser)) {
            SyntaxToken peek1 = parser->PeekToken(1);
            if (peek1.kind != TokenKind::CommaToken &&
                peek1.kind != TokenKind::CloseParenToken &&
                peek1.kind != TokenKind::EqualsGreaterThanToken &&
                peek1.kind != TokenKind::OpenBraceToken &&
//                peek1.kind != TokenKind::ExclamationToken &&
                peek1.kind != TokenKind::EqualsToken) {
                return true;
            }
        }

        return false;
    }

    ParameterSyntax* ParseLambdaParameter(Parser* parser) {

        // Params are actually illegal in a lambda, but we'll allow it for error recovery purposes and
        // give the "params unexpected" error at semantic analysis time.
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        TokenListBuffer modifiers(parser->tempAllocator);

        if (IsParameterModifierExcludingScoped(parser->currentToken)) {
            ParseParameterModifiers(parser, &modifiers, false);
        }

        // If we have "scoped/ref/out/in/params" always try to parse out a type.
        TypeSyntax* paramType = modifiers.size != 0 || ShouldParseLambdaParameterType(parser)
            ? ParseType(parser, ParseTypeMode::Parameter)
            : nullptr;

        SyntaxToken identifier = ParseIdentifierToken(parser);

        // Parse default value if any
        SyntaxToken equalsToken;
        if (parser->currentToken.kind == TokenKind::EqualsToken) {
            equalsToken = parser->EatToken();
        }

        return parser->CreateNode<ParameterSyntax>(
            modifiers.Persist(parser->allocator),
            paramType,
            identifier,
            equalsToken.IsValid()
                ? parser->CreateNode<EqualsValueClauseSyntax>(equalsToken, ParseExpression(parser))
                : nullptr
        );
    }

    ParameterListSyntax* ParseLambdaParameterList(Parser* parser) {
        SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);
        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfParameterList;

        SeparatedSyntaxList<ParameterSyntax>* nodes = ParseCommaSeparatedSyntaxList<ParameterSyntax>(
            parser,
            &openParen,
            TokenKind::CloseParenToken,
            [](Parser* p) { return IsPossibleLambdaParameter(p); },
            [](Parser* p) { return (SyntaxBase*) ParseLambdaParameter(p); },
            &SkipBadLambdaParameterListTokens,
            false,
            false,
            false
        );

        parser->termState = saveTerm;

        return parser->CreateNode<ParameterListSyntax>(
            openParen,
            nodes,
            parser->EatToken(TokenKind::CloseParenToken)
        );

    }

    StatementSyntax* ParseEmbeddedStatementRest(Parser* parser, StatementSyntax* statement) {
        if (statement == nullptr) {
            // The consumers of embedded statements are expecting to receive a non-null statement
            // yet there are several error conditions that can lead ParseStatement to return
            // null.  When that occurs create an error empty Statement and return it to the caller.
            return parser->CreateNode<EmptyStatementSyntax>(parser->EatToken(TokenKind::SemicolonToken));
        }

        return statement;
    }

    StatementSyntax* ParseEmbeddedStatement(Parser* parser) {
        return ParseEmbeddedStatementRest(parser, ParseStatement(parser));
    }

    DoStatementSyntax* ParseDoStatement(Parser* parser) {
        SyntaxToken doToken = parser->EatToken(TokenKind::DoKeyword);
        StatementSyntax* statement = ParseEmbeddedStatement(parser);
        SyntaxToken whileToken = parser->EatToken(TokenKind::WhileKeyword);
        SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfDoWhileExpression;
        ExpressionSyntax* expression = ParseExpression(parser);
        parser->termState = saveTerm;

        return parser->CreateNode<DoStatementSyntax>(
            doToken,
            statement,
            whileToken,
            openParen,
            expression,
            parser->EatToken(TokenKind::CloseParenToken),
            parser->EatToken(TokenKind::SemicolonToken)
        );
    }

    void ParseLocalDeclaration(Parser* parser, SeparatedSyntaxListBuilder<VariableDeclaratorSyntax>* variables, bool allowLocalFunctions, bool stopOnCloseParen, TokenListBuffer* mods, TypeSyntax** outType, LocalFunctionStatementSyntax** localFunction) {
        TypeSyntax* type = allowLocalFunctions
            ? ParseReturnType(parser)
            : ParseType(parser);

        VariableFlags flags = VariableFlags::LocalOrField;

        if (mods != nullptr) {
            for (int32 i = 0; i < mods->size; i++) {
                if (mods->array[i].kind == TokenKind::ConstKeyword) {
                    flags |= VariableFlags::Const;
                    break;
                }
            }
        }

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfDeclarationClause;
        ParseVariableDeclarators(
            parser,
            type,
            flags,
            variables,
            true,
            allowLocalFunctions,
            stopOnCloseParen,
            mods,
            localFunction
        );

        parser->termState = saveTerm;

        if (allowLocalFunctions && *localFunction == nullptr && type->GetKind() == SyntaxKind::PredefinedType) {
            PredefinedTypeSyntax* predefinedTypeSyntax = (PredefinedTypeSyntax*) type;
            if (predefinedTypeSyntax->typeToken.kind == TokenKind::VoidKeyword) {
                parser->AddError(type, ErrorCode::ERR_NoVoidHere);
            }
        }

        *outType = type;

    }

    // Parse a local variable declaration for constructs where the variable declaration is enclosed in parentheses.
    // Specifically, only for the `fixed (...)` `for(...)` or `using (...)` statements.
    VariableDeclarationSyntax* ParseParenthesizedVariableDeclaration(Parser* parser) {
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SeparatedSyntaxListBuilder<VariableDeclaratorSyntax> variables(parser->tempAllocator);
        LocalFunctionStatementSyntax* localFn = nullptr;
        TypeSyntax* type = nullptr;
        ParseLocalDeclaration(
            parser,
            &variables,
            false, // allowLocalFunctions, bool stopOnCloseParen,
            true, // stopOnCloseParen -- Always stop on a close paren as the parent `fixed(...)/for(...)/using(...)` statement wants to consume it.
            nullptr,
            &type,
            &localFn
        );

        assert(localFn == nullptr);

        return parser->CreateNode<VariableDeclarationSyntax>(type, variables.ToList(parser->allocator));
    }

    static PostSkipAction SkipBadForStatementExpressionListTokens(Parser* parser, TokenKind expectedKind, TokenKind closeKind) {
        if (parser->currentToken.kind == TokenKind::CloseParenToken || parser->currentToken.kind == TokenKind::SemicolonToken) {
            return PostSkipAction::Abort;
        }

        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleExpression(p); },
            [](Parser* p, TokenKind closeKind) { return p->currentToken.kind == closeKind || p->currentToken.kind == TokenKind::SemicolonToken; },
            expectedKind,
            closeKind
        );

    }

    SeparatedSyntaxList<ExpressionSyntax>* ParseForStatementExpressionList(Parser* parser, SyntaxToken* startToken) {
        return ParseCommaSeparatedSyntaxList<ExpressionSyntax>(
            parser,
            startToken,
            TokenKind::CloseParenToken,
            [](Parser* parser) { return IsPossibleExpression(parser); },
            [](Parser* parser) { return (SyntaxBase*) ParseExpression(parser); },
            &SkipBadForStatementExpressionListTokens,
            false,
            false,
            false
        );

    }

    bool IsValidForeachVariable(ExpressionSyntax* variable) {
        switch (variable->GetKind()) {
            case SyntaxKind::DeclarationExpression:
                // e.g. `foreach (var (x, y) in e)`
                return true;
            case SyntaxKind::TupleExpression:
                // e.g. `foreach ((var x, var y) in e)`
                return true;
            case SyntaxKind::IdentifierName:
                // e.g. `foreach (_ in e)`
                return ((IdentifierNameSyntax*) variable)->identifier.contextualKind == TokenKind::UnderscoreToken;
            default:
                return false;
        }
    }

    CommonForEachStatementSyntax* ParseForEachStatement(Parser* parser) {
        // Can be a 'for' keyword if the user typed: 'for (SomeType t in'
        assert(parser->currentToken.kind == TokenKind::ForEachKeyword || parser->currentToken.kind == TokenKind::ForKeyword);
        // Syntax for foreach is either:
        //  foreach ( <type> <identifier> in <expr> ) <embedded-statement>
        // or
        //  foreach ( <deconstruction-declaration> in <expr> ) <embedded-statement>

        SyntaxToken foreach;

        // If we're at a 'for', then consume it and attach
        // it as skipped text to the missing 'foreach' token.
        if (parser->currentToken.kind == TokenKind::ForKeyword) {
            SyntaxToken skippedForToken = parser->EatToken();
            parser->AddError(skippedForToken, ErrorCode::ERR_ExpectedForeachKeyword);
            foreach = MakeMissingToken(TokenKind::ForEachKeyword, skippedForToken.GetId());
        }
        else {
            foreach = parser->EatToken(TokenKind::ForEachKeyword);
        }

        SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);

        ExpressionSyntax* variable = ParseExpressionOrDeclaration(parser, ParseTypeMode::Normal, true);
        SyntaxToken inKeyword = parser->EatToken(TokenKind::InKeyword, ErrorCode::ERR_InExpected);
        if (!IsValidForeachVariable(variable)) {
            parser->AddError(inKeyword, ErrorCode::ERR_BadForeachDecl);
        }

        ExpressionSyntax* expression = ParseExpression(parser);
        SyntaxToken closeParen = parser->EatToken(TokenKind::CloseParenToken);
        StatementSyntax* statement = ParseEmbeddedStatement(parser);

        if (variable->GetKind() == SyntaxKind::DeclarationExpression) {
            DeclarationExpressionSyntax* decl = (DeclarationExpressionSyntax*) variable;
            if (decl->designation->GetKind() != SyntaxKind::ParenthesizedVariableDesignation) {
                // if we see a foreach declaration that isn't a deconstruction, we use the old form of foreach syntax node.
                SyntaxToken identifier;
                switch (decl->designation->GetKind()) {
                    case SyntaxKind::SingleVariableDesignation:
                        identifier = ((SingleVariableDesignationSyntax*) decl->designation)->identifier;
                        break;
                    case SyntaxKind::DiscardDesignation: {
                        // revert the identifier from its contextual underscore back to an identifier.
                        DiscardDesignationSyntax* discard = (DiscardDesignationSyntax*) decl;
                        identifier = discard->underscoreToken;
                        break;
                    }
                    default:
                        UNREACHABLE("ParseForEachStatement");
                        return nullptr;
                }

                return parser->CreateNode<ForEachStatementSyntax>(foreach, openParen, decl->type, identifier, inKeyword, expression, closeParen, statement);
            }
        }
        return parser->CreateNode<ForEachVariableStatementSyntax>(foreach, openParen, variable, inKeyword, expression, closeParen, statement);


    }

    ForStatementSyntax* ParseForStatement(Parser* parser) {
        SyntaxToken forToken = parser->EatToken(TokenKind::ForKeyword);
        SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfForStatementArgument;

        ResetPoint resetPoint(parser, false);
        TempAllocator::ScopedMarker m(parser->tempAllocator);

        SeparatedSyntaxList<ExpressionSyntax>* initializers = nullptr;
        SeparatedSyntaxList<ExpressionSyntax>* incrementors = nullptr;

        // Here can be either a declaration or an expression statement list. Scan for a declaration first.
        VariableDeclarationSyntax* decl = nullptr;
        bool isDeclaration = false;

        if (parser->currentToken.kind == TokenKind::RefKeyword) {
            isDeclaration = true;
        }

        if (!isDeclaration) {
            isDeclaration = ScanType(parser, false) != ScanTypeFlags::NotType && IsTrueIdentifier(parser);
            resetPoint.Reset();
        }

        if (isDeclaration) {
            decl = ParseParenthesizedVariableDeclaration(parser);
        }
        else if (parser->currentToken.kind != TokenKind::SemicolonToken) {
            // Not a type followed by an identifier, so it must be an expression list.
            initializers = ParseForStatementExpressionList(parser, &openParen);
        }
        SyntaxToken semi = parser->EatToken(TokenKind::SemicolonToken);
        ExpressionSyntax* condition = nullptr;

        if (parser->currentToken.kind != TokenKind::SemicolonToken) {
            condition = ParseExpression(parser);
        }

        SyntaxToken semi2 = parser->EatToken(TokenKind::SemicolonToken);
        if (parser->currentToken.kind != TokenKind::CloseParenToken) {
            incrementors = ParseForStatementExpressionList(parser, &semi2);
        }

        ForStatementSyntax* retn = parser->CreateNode<ForStatementSyntax>(
            forToken,
            openParen,
            decl,
            initializers,
            semi,
            condition,
            semi2,
            incrementors,
            parser->EatToken(TokenKind::CloseParenToken),
            ParseEmbeddedStatement(parser)
        );

        parser->termState = saveTerm;
        return retn;

    }

    StatementSyntax* ParseForOrForEachStatement(Parser* parser) {
        // Check if the user wrote the following accidentally:
        //
        // for (SomeType t in
        //
        // instead of
        //
        // foreach (SomeType t in
        //
        // In that case, parse it as a foreach, but given the appropriate message that a
        // 'foreach' keyword was expected.
        ResetPoint resetPoint(parser, false);

        assert(parser->currentToken.kind == TokenKind::ForKeyword);

        parser->EatToken();

        if (parser->EatToken().kind == TokenKind::OpenParenToken &&
            ScanType(parser, false) != ScanTypeFlags::NotType &&
            parser->EatToken().kind == TokenKind::IdentifierToken &&
            parser->EatToken().kind == TokenKind::InKeyword) {
            // Looks like a foreach statement.  Parse it that way instead
            resetPoint.Reset();
            return ParseForEachStatement(parser);
        }
        else {
            // Normal for statement.
            resetPoint.Reset();
            return ParseForStatement(parser);
        }
    }

    GotoStatementSyntax* ParseGotoStatement(Parser* parser) {

        SyntaxToken gotoToken = parser->EatToken(TokenKind::GotoKeyword);

        SyntaxToken caseOrDefault;
        ExpressionSyntax* arg = nullptr;
        SyntaxKind kind;

        if (parser->currentToken.kind == TokenKind::CaseKeyword || parser->currentToken.kind == TokenKind::DefaultKeyword) {
            caseOrDefault = parser->EatToken();
            if (caseOrDefault.kind == TokenKind::CaseKeyword) {
                kind = SyntaxKind::GotoCaseStatement;
                arg = ParseExpression(parser);
            }
            else {
                kind = SyntaxKind::GotoDefaultStatement;
            }
        }
        else {
            kind = SyntaxKind::GotoStatement;
            arg = ParseIdentifierName(parser);
        }

        return parser->CreateNode<GotoStatementSyntax>(kind, gotoToken, caseOrDefault, arg, parser->EatToken(TokenKind::SemicolonToken));
    }

    ElseClauseSyntax* ParseElseClauseOpt(Parser* parser) {
        return parser->currentToken.kind != TokenKind::ElseKeyword
            ? nullptr
            : parser->CreateNode<ElseClauseSyntax>(
                parser->EatToken(TokenKind::ElseKeyword),
                ParseEmbeddedStatement(parser)
            );
    }

    ExpressionStatementSyntax* ParseExpressionStatement(Parser* parser, ExpressionSyntax* expression) {
        // Do not report an error if the expression is not a statement expression.
        // The error is reported in semantic analysis.
        return parser->CreateNode<ExpressionStatementSyntax>(expression, parser->EatToken(TokenKind::SemicolonToken));
    }

    ExpressionStatementSyntax* ParseExpressionStatement(Parser* parser) {
        return ParseExpressionStatement(parser, ParseExpression(parser));
    }

    IfStatementSyntax* ParseIfStatement(Parser* parser) {
        return parser->CreateNode<IfStatementSyntax>(
            parser->EatToken(TokenKind::IfKeyword),
            parser->EatToken(TokenKind::OpenParenToken),
            ParseExpression(parser),
            parser->EatToken(TokenKind::CloseParenToken),
            ParseEmbeddedStatement(parser),
            ParseElseClauseOpt(parser)
        );
    }

    IfStatementSyntax* ParseMisplacedElse(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::ElseKeyword);

        return parser->CreateNode<IfStatementSyntax>(
            parser->EatToken(TokenKind::IfKeyword, ErrorCode::ERR_ElseCannotStartStatement),
            parser->EatToken(TokenKind::OpenParenToken),
            ParseExpression(parser),
            parser->EatToken(TokenKind::CloseParenToken),
            ParseExpressionStatement(parser),
            ParseElseClauseOpt(parser)
        );

    }

    bool IsNonContextualModifier(SyntaxToken nextToken) {
        return !SyntaxFacts::IsContextualKeyword(nextToken.contextualKind) && GetModifier(nextToken) != DeclarationModifiers::None;
    }

    bool IsPossibleMemberName(Parser* parser) {
        switch (parser->currentToken.kind) {
            case TokenKind::IdentifierToken:
            case TokenKind::ThisKeyword:
                return true;
            default:
                return false;
        }
    }

    bool ShouldContextualKeywordBeTreatedAsModifier(Parser* parser, bool parsingStatementNotDeclaration) {
        assert(parser->currentToken.kind == TokenKind::IdentifierToken && GetModifier(parser->currentToken) != DeclarationModifiers::None);

        // If the next token is a (non-contextual) modifier keyword, then this token is definitely a modifier
        if (IsNonContextualModifier(parser->PeekToken(1))) {
            return true;
        }

        ResetPoint resetPoint(parser);
        parser->EatToken(); // consume contextual token

        if (!parsingStatementNotDeclaration && (parser->currentToken.contextualKind == TokenKind::PartialKeyword)) {
            parser->EatToken(); // "partial" doesn't affect our decision, so look past it.
        }

        // ... 'TOKEN' [partial] <typedecl> ...
        // ... 'TOKEN' [partial] <implicit> <operator> ...
        // ... 'TOKEN' [partial] <explicit> <operator> ...
        // ... 'TOKEN' [partial] <typename> <operator> ...
        // ... 'TOKEN' [partial] <typename> <membername> ...
        // DEVNOTE: Although we parse async user defined conversions, operators, etc. here,
        // anything other than async methods are detected as erroneous later, during the define phase
        // Generally wherever we refer to 'async' here, it can also be 'required' or 'file'.

        if (!parsingStatementNotDeclaration) {

            TokenKind tk = parser->currentToken.kind;

            if (SyntaxFacts::IsTypeModifierOrTypeKeyword(tk) || (tk == TokenKind::ExplicitKeyword || tk == TokenKind::ImplicitKeyword && parser->PeekToken(1).kind == TokenKind::OperatorKeyword)) {
                return true;
            }

        }

        if (ScanType(parser, false) != ScanTypeFlags::NotType) {
            // We've seen "TOKEN TypeName".  Now we have to determine if we should we treat
            // 'TOKEN' as a modifier.  Or is the user actually writing something like
            // "public TOKEN Goo" where 'TOKEN' is actually the return type.

            if (IsPossibleMemberName(parser)) {
                // we have: "TOKEN Type X" or "TOKEN Type this", 'TOKEN' is definitely a
                // modifier here.
                return true;
            }

            TokenKind currentTokenKind = parser->currentToken.kind;

            // The file ends with "TOKEN TypeName", it's not legal code, and it's much
            // more likely that this is meant to be a modifier.
            if (currentTokenKind == TokenKind::EndOfFileToken) {
                return true;
            }

            // "TOKEN TypeName }".  In this case, we just have an incomplete member, and
            // we should definitely default to 'TOKEN' being considered a return type here.
            if (currentTokenKind == TokenKind::CloseBraceToken) {
                return true;
            }

            // "TOKEN TypeName void". In this case, we just have an incomplete member before
            // an existing member.  Treat this 'TOKEN' as a keyword.
            if (SyntaxFacts::IsPredefinedType(currentTokenKind)) {
                return true;
            }

            // "TOKEN TypeName public".  In this case, we just have an incomplete member before
            // an existing member.  Treat this 'TOKEN' as a keyword.
            if (IsNonContextualModifier(parser->currentToken)) {
                return true;
            }

            // "TOKEN TypeName class". In this case, we just have an incomplete member before
            // an existing type declaration. Treat this 'TOKEN' as a keyword.
            if (IsTypeDeclarationStart(parser)) {
                return true;
            }

            // "TOKEN TypeName namespace". In this case, we just have an incomplete member before
            // an existing namespace declaration.  Treat this 'TOKEN' as a keyword.
            if (currentTokenKind == TokenKind::NamespaceKeyword) {
                return true;
            }

            if (!parsingStatementNotDeclaration && currentTokenKind == TokenKind::OperatorKeyword) {
                return true;
            }
        }

        return false;

    }

    enum class TypeIdentifierStartState {
        Yes,
        No,
        Maybe
    };

    TypeIdentifierStartState IsPossibleTypedIdentifierStart(SyntaxToken current, SyntaxToken next, bool allowThisKeyword) {
        if (IsTrueIdentifier(current)) {
            switch (next.kind) {
                // tokens that can be in type names...
                case TokenKind::DotToken:
                case TokenKind::QuestionToken:
                case TokenKind::OpenBracketToken:
                case TokenKind::LessThanToken:
//                case TokenKind::AsteriskToken:
//                case TokenKind::ColonColonToken:
                    return TypeIdentifierStartState::Maybe;

                case TokenKind::OpenParenToken:
                    if (current.contextualKind == TokenKind::VarKeyword) {
                        // potentially either a tuple type in a local declaration (true), or
                        // a tuple lvalue in a deconstruction assignment (false).
                        return TypeIdentifierStartState::Maybe;
                    }
                    else {
                        return TypeIdentifierStartState::No;
                    }

                case TokenKind::IdentifierToken:
                    return IsTrueIdentifier(next) ? TypeIdentifierStartState::Yes : TypeIdentifierStartState::No;

                case TokenKind::ThisKeyword:
                    return allowThisKeyword ? TypeIdentifierStartState::Yes : TypeIdentifierStartState::No;

                default:
                    return TypeIdentifierStartState::No;
            }
        }

        return TypeIdentifierStartState::Maybe;
    }

    bool IsPossibleFirstTypedIdentifierInLocaDeclarationStatement(Parser* parser) {
        TypeIdentifierStartState typedIdentifier = IsPossibleTypedIdentifierStart(parser->currentToken, parser->PeekToken(1), false);
        switch (typedIdentifier) {
            case TypeIdentifierStartState::Yes: {
                return true;
            }
            case TypeIdentifierStartState::No: {
                return false;
            }
            default:
            case TypeIdentifierStartState::Maybe: {
                break;
            }
        }

        // It's common to have code like the following:
        //
        //      Task.
        //      await Task.Delay()
        //
        // In this case we don't want to parse this as a local declaration like:
        //
        //      Task.await Task
        //
        // This does not represent user intent, and it causes all sorts of problems to higher
        // layers.  This is because both the parse tree is strange, and the symbol tables have
        // entries that throw things off (like a bogus 'Task' local).
        //
        // Note that we explicitly do this check when we see that the code spreads over multiple
        // lines.  We don't want this if the user has actually written "X.Y z"
        TokenKind tk = parser->currentToken.contextualKind;

        if (tk == TokenKind::IdentifierToken) {
            SyntaxToken token1 = parser->PeekToken(1);
            if (token1.kind == TokenKind::DotToken && parser->HasTrailingNewLine(token1)) {
                if (parser->PeekToken(2).kind == TokenKind::IdentifierToken && parser->PeekToken(3).kind == TokenKind::IdentifierToken) {
                    // We have something like:
                    //
                    //      X.
                    //      Y z
                    //
                    // This is only a local declaration if we have:
                    //
                    //      X.Y z;
                    //      X.Y z = ...
                    //      X.Y z, ...
                    //      X.Y z( ...      (local function)
                    //      X.Y z<W...      (local function)
                    //
                    TokenKind token4Kind = parser->PeekToken(4).kind;
                    if (token4Kind != TokenKind::SemicolonToken &&
                        token4Kind != TokenKind::EqualsToken &&
                        token4Kind != TokenKind::CommaToken &&
                        token4Kind != TokenKind::OpenParenToken &&
                        token4Kind != TokenKind::LessThanToken) {
                        return false;
                    }
                }
            }
        }

        ResetPoint resetPoint(parser);

        ScanTypeFlags st = ScanType(parser, false);

        // We could always return true for st == AliasQualName in addition to MustBeType on the first line, however, we want it to return false in the case where
        // CurrentToken.Kind != TokenKind::Identifier so that error cases, like: A::N(), are not parsed as variable declarations and instead are parsed as A.N() where we can give
        // a better error message saying "did you meant to use a '.'?"
        if (st == ScanTypeFlags::MustBeType && parser->currentToken.kind != TokenKind::DotToken && parser->currentToken.kind != TokenKind::OpenParenToken) {
            return true;
        }

        if (st == ScanTypeFlags::NotType || parser->currentToken.kind != TokenKind::IdentifierToken) {
            return false;
        }

        return true;
    }

    bool IsPossibleLocalDeclarationStatement(Parser* parser) {
        // This method decides whether to parse a statement as a
        // declaration or as an expression statement.

        TokenKind tk = parser->currentToken.kind;
        if (tk == TokenKind::RefKeyword || SyntaxFacts::IsDeclarationModifier(tk) || // treat `static int x = 2;` as a local variable declaration
            (SyntaxFacts::IsPredefinedType(tk) && parser->PeekToken(1).kind != TokenKind::DotToken // e.g. `int.Parse()` is an expression
                && parser->PeekToken(1).kind != TokenKind::OpenParenToken)) // e.g. `int (x, y)` is an error decl expression
        {
            return true;
        }

        // note: `using(` is already handled in ParseStatementCore.
        if (tk == TokenKind::UsingKeyword) {
            assert(parser->PeekToken(1).kind != TokenKind::OpenParenToken);
            return true;
        }

        tk = parser->currentToken.contextualKind;

        bool isPossibleModifier = SyntaxFacts::IsAdditionalLocalFunctionModifier(tk) || ShouldContextualKeywordBeTreatedAsModifier(parser, true);

        if (isPossibleModifier) {
            return true;
        }

        return IsPossibleFirstTypedIdentifierInLocaDeclarationStatement(parser);
    }

    void ParseDeclarationModifiers(Parser* parser, TokenListBuffer* list, bool isUsingDeclaration) {
        TokenKind k;
        while (SyntaxFacts::IsDeclarationModifier(k = parser->currentToken.contextualKind) || SyntaxFacts::IsAdditionalLocalFunctionModifier(k)) {
            SyntaxToken mod = parser->EatToken();

            if (isUsingDeclaration) {
                parser->AddError(mod, ErrorCode::ERR_NoModifiersOnUsing);
            }
            else if (k == TokenKind::ReadOnlyKeyword) {
                parser->AddError(mod, ErrorCode::ERR_BadMemberFlag);
            }
            else {

                for (int32 i = 0; i < list->size; i++) {
                    if (list->array[i].kind == mod.kind) {
                        // check for duplicates, can only be const
                        parser->AddError(mod, ErrorCode::ERR_TypeExpected);
                        break;
                    }
                }

            }

            list->Add(mod);
        }
    }

    // Parses any kind of local declaration statement: local variable or local function.
    StatementSyntax* ParseLocalDeclarationStatement(Parser* parser) {

        SyntaxToken usingKeyword;
        bool canParseAsLocalFunction = false;

        if (parser->currentToken.kind == TokenKind::UsingKeyword) {
            usingKeyword = parser->EatToken();
        }
        else {
            canParseAsLocalFunction = true;
        }

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        TokenListBuffer mods(parser->tempAllocator);
        ParseDeclarationModifiers(parser, &mods, usingKeyword.IsValid());
        SeparatedSyntaxListBuilder<VariableDeclaratorSyntax> variables(parser->tempAllocator);

        TypeSyntax* type = nullptr;
        LocalFunctionStatementSyntax* localFunction = nullptr;

        ParseLocalDeclaration(
            parser,
            &variables,
            canParseAsLocalFunction,
            // A local declaration doesn't have a `(...)` construct.  So no need to stop if we hit a close paren
            // after a declarator.  Let normal error recovery kick in.
            false,
            &mods,
            &type,
            &localFunction
        );

        if (localFunction != nullptr) {
            assert(variables.itemCount == 0);
            return localFunction;
        }

        if (canParseAsLocalFunction) {
            // If we find an accessibility modifier but no local function it's likely
            // the user forgot a closing brace. Let's back out of statement parsing.
            // We check just for a leading accessibility modifier in the syntax because
            // SkipBadStatementListTokens will not skip attribute lists.
            if (mods.size > 0 && SyntaxFacts::IsAccessibilityModifier(mods.array[0].contextualKind)) {
                return nullptr;
            }
        }

        if (!usingKeyword.IsValid()) {
            for (int32 i = 0; i < mods.size; i++) {
                SyntaxToken mod = mods.array[i];

                if (SyntaxFacts::IsAdditionalLocalFunctionModifier(mod.contextualKind)) {
                    parser->AddError(mod, ErrorCode::ERR_BadMemberFlag);
                }
            }
        }

        return parser->CreateNode<LocalDeclarationStatementSyntax>(
            usingKeyword,
            mods.Persist(parser->allocator),
            parser->CreateNode<VariableDeclarationSyntax>(type, variables.ToList(parser->allocator)),
            parser->EatToken(TokenKind::SemicolonToken)
        );

    }

    StatementSyntax* ParseStatementRest(Parser* parser, ResetPoint* resetPoint) {
        if (!IsPossibleLocalDeclarationStatement(parser)) {
            return ParseExpressionStatement(parser);
        }

        StatementSyntax* result = ParseLocalDeclarationStatement(parser);
        // didn't get any sort of statement.  This was something else entirely
        // (like just a `}`).  No need to retry anything here.  Just reset back
        // to where we started from and bail entirely from parsing a statement.
        if (result == nullptr) {
            resetPoint->Reset();
            return nullptr;
        }

        return result;
    }

    WhileStatementSyntax* ParseWhileStatement(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::WhileKeyword);
        return parser->CreateNode<WhileStatementSyntax>(
            parser->EatToken(TokenKind::WhileKeyword),
            parser->EatToken(TokenKind::OpenParenToken),
            ParseExpression(parser),
            parser->EatToken(TokenKind::CloseParenToken),
            ParseEmbeddedStatement(parser)
        );
    }

    ThrowStatementSyntax* ParseThrowStatement(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::ThrowKeyword);
        return parser->CreateNode<ThrowStatementSyntax>(
            parser->EatToken(TokenKind::ThrowKeyword),
            parser->currentToken.kind != TokenKind::SemicolonToken ? ParseExpression(parser) : nullptr,
            parser->EatToken(TokenKind::SemicolonToken)
        );
    }

    BlockSyntax* MissingBlock(Parser* parser) {
        return parser->CreateNode<BlockSyntax>(
            MakeMissingToken(TokenKind::OpenBraceToken, parser->ptr),
            nullptr,
            MakeMissingToken(TokenKind::CloseBraceToken, parser->ptr)
        );
    }

    CatchClauseSyntax* ParseCatchClause(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::CatchKeyword);

        SyntaxToken catchKeyword = parser->EatToken();

        CatchDeclarationSyntax* decl = nullptr;
        TerminatorState saveTerm = parser->termState;

        if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            SyntaxToken openParen = parser->EatToken();

            parser->termState |= TerminatorState::IsEndOfCatchClause;
            TypeSyntax* type = ParseType(parser);
            SyntaxToken name;
            if (IsTrueIdentifier(parser)) {
                name = ParseIdentifierToken(parser);
            }

            parser->termState = saveTerm;

            SyntaxToken closeParen = parser->EatToken(TokenKind::CloseParenToken);
            decl = parser->CreateNode<CatchDeclarationSyntax>(openParen, type, name, closeParen);
        }

        CatchFilterClauseSyntax* filter = nullptr;

        TokenKind keywordKind = parser->currentToken.contextualKind;
        if (keywordKind == TokenKind::WhenKeyword) {
            SyntaxToken whenKeyword = parser->EatContextualToken(TokenKind::WhenKeyword);

            parser->termState |= TerminatorState::IsEndOfFilterClause;
            SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);
            ExpressionSyntax* filterExpression = ParseExpression(parser);

            parser->termState = saveTerm;
            SyntaxToken closeParen = parser->EatToken(TokenKind::CloseParenToken);
            filter = parser->CreateNode<CatchFilterClauseSyntax>(whenKeyword, openParen, filterExpression, closeParen);
        }

        parser->termState |= TerminatorState::IsEndOfCatchBlock;
        BlockSyntax* block = ParseBlock(parser);
        parser->termState = saveTerm;

        return parser->CreateNode<CatchClauseSyntax>(catchKeyword, decl, filter, block);
    }

    TryStatementSyntax* ParseTryStatement(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::TryKeyword || parser->currentToken.kind == TokenKind::CatchKeyword || parser->currentToken.kind == TokenKind::FinallyKeyword);
        // We are called into on try/catch/finally, so eating the try may actually fail.

        SyntaxToken tryKeyword = parser->EatToken(TokenKind::TryKeyword);
        BlockSyntax* tryBlock = nullptr;

        if (tryKeyword.IsMissing()) {
            // If there was no actual `try`, then we got here because of a misplaced `catch`/`finally`.  In that
            // case just synthesize a fully missing try-block.  We will already have issued a diagnostic on the
            // `try` keyword, so we don't need to issue any more.

            assert(parser->currentToken.kind == TokenKind::CatchKeyword || parser->currentToken.kind == TokenKind::FinallyKeyword);

            tryBlock = MissingBlock(parser);
        }
        else {
            TerminatorState saveTerm = parser->termState;
            parser->termState |= TerminatorState::IsEndOfTryBlock;
            tryBlock = ParseBlock(parser);
            parser->termState = saveTerm;
        }

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<CatchClauseSyntax> catchClauses(parser->tempAllocator);
        FinallyClauseSyntax* finallyClause = nullptr;

        while (parser->currentToken.kind == TokenKind::CatchKeyword) {
            catchClauses.Add(ParseCatchClause(parser));
        }

        if (parser->currentToken.kind == TokenKind::FinallyKeyword) {
            finallyClause = parser->CreateNode<FinallyClauseSyntax>(
                parser->EatToken(),
                ParseBlock(parser)
            );
        }

        if (catchClauses.size == 0 && finallyClause == nullptr) {
            parser->AddError(tryBlock, ErrorCode::ERR_ExpectedEndTry);
            // synthesize missing tokens for "finally { }":
            finallyClause = parser->CreateNode<FinallyClauseSyntax>(
                MakeMissingToken(TokenKind::FinallyKeyword, parser->ptr),
                MissingBlock(parser)
            );
        }

        return parser->CreateNode<TryStatementSyntax>(
            tryKeyword,
            tryBlock,
            catchClauses.ToSyntaxList(parser->allocator),
            finallyClause
        );

    }

    void ParseSwitchHeader(Parser* parser, SyntaxToken* switchKeyword, SyntaxToken* openParen, ExpressionSyntax** expression, SyntaxToken* closeParen, SyntaxToken* openBrace) {
        if (parser->currentToken.kind == TokenKind::CaseKeyword) {
            // try to eat a 'switch' so the user gets a good error message about what's wrong. then directly
            // creating missing tokens for the rest so they don't get cascading errors.
            *switchKeyword = parser->EatToken(TokenKind::SwitchKeyword);
            *openParen = MakeMissingToken(TokenKind::OpenParenToken, parser->ptr);
            *expression = CreateMissingIdentifierName(parser);
            *closeParen = MakeMissingToken(TokenKind::CloseParenToken, parser->ptr);
            *openBrace = MakeMissingToken(TokenKind::OpenBraceToken, parser->ptr);
        }
        else {
            *switchKeyword = parser->EatToken(TokenKind::SwitchKeyword);
            ExpressionSyntax* expr = ParseExpression(parser);
            *expression = expr;
            if (expr->GetKind() == SyntaxKind::ParenthesizedExpression) {
                ParenthesizedExpressionSyntax* parenExpression = (ParenthesizedExpressionSyntax*) expr;
                *openParen = parenExpression->openToken;
                *expression = parenExpression->expression;
                *closeParen = parenExpression->closeToken;
            }
            else if (expr->GetKind() == SyntaxKind::TupleExpression) {
                // As a special case, when a tuple literal is the governing expression of
                // a switch statement we permit the switch statement's own parentheses to be omitted.
                *openParen = SyntaxToken();
                *closeParen = SyntaxToken();
            }
            else {
                // Some other expression has appeared without parens. Give a syntax error.
                *openParen = MakeMissingToken(TokenKind::OpenParenToken, parser->ptr);
                parser->AddError(expr, ErrorCode::ERR_SwitchGoverningExpressionRequiresParens);
                *closeParen = MakeMissingToken(TokenKind::CloseParenToken, parser->ptr);
            }

            *openBrace = parser->EatToken(TokenKind::OpenBraceToken);
        }
    }

    SyntaxBase* ParseExpressionOrPatternForSwitchStatement(Parser* parser) {
        TerminatorState savedState = parser->termState;
        parser->termState |= TerminatorState::IsExpressionOrPatternInCaseLabelOfSwitchStatement;
        PatternSyntax* pattern = ParsePattern(parser, Precedence::Conditional, true);
        parser->termState = savedState;
        return ConvertPatternToExpressionIfPossible(parser, pattern);
    }

    SwitchSectionSyntax* ParseSwitchSection(Parser* parser) {
        // First, parse case label(s)
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<SwitchLabelSyntax> labels(parser->tempAllocator);
        SyntaxListBuilder<StatementSyntax> statements(parser->tempAllocator);

        do {
            SwitchLabelSyntax* label = nullptr;
            if (parser->currentToken.kind == TokenKind::CaseKeyword) {
                SyntaxToken caseKeyword = parser->EatToken();

                if (parser->currentToken.kind == TokenKind::ColonToken) {
                    label = parser->CreateNode<CaseSwitchLabelSyntax>(
                        caseKeyword,
                        ParseIdentifierName(parser, ErrorCode::ERR_ConstantExpected),
                        parser->EatToken(TokenKind::ColonToken)
                    );
                }
                else {
                    SyntaxBase* node = ParseExpressionOrPatternForSwitchStatement(parser);

                    // if there is a 'when' token, we treat a case expression as a constant pattern.
                    if (parser->currentToken.contextualKind == TokenKind::WhenKeyword && SyntaxFacts::IsExpressionSyntax(node->GetKind())) {
                        node = parser->CreateNode<ConstantPatternSyntax>((ExpressionSyntax*) node);
                    }

                    if (node->GetKind() == SyntaxKind::DiscardPattern) {
                        parser->AddError(node, ErrorCode::ERR_DiscardPatternInSwitchStatement);
                    }

                    if (SyntaxFacts::IsPatternSyntax(node->GetKind())) {
                        label = parser->CreateNode<CasePatternSwitchLabelSyntax>(
                            caseKeyword,
                            (PatternSyntax*) node,
                            ParseWhenClause(parser, Precedence::Expression),
                            parser->EatToken(TokenKind::ColonToken)
                        );
                    }
                    else {
                        label = parser->CreateNode<CaseSwitchLabelSyntax>(
                            caseKeyword,
                            (ExpressionSyntax*) node,
                            parser->EatToken(TokenKind::ColonToken)
                        );
                    }
                }
            }
            else {
                assert(parser->currentToken.kind == TokenKind::DefaultKeyword);
                label = parser->CreateNode<DefaultSwitchLabelSyntax>(
                    parser->EatToken(TokenKind::DefaultKeyword),
                    parser->EatToken(TokenKind::ColonToken)
                );
            }

            labels.Add(label);
        } while (IsPossibleSwitchSection(parser));

        // Next, parse statement list stopping for new sections
        ParseStatements(parser, &statements, true);

        return parser->CreateNode<SwitchSectionSyntax>(
            labels.ToSyntaxList(parser->allocator),
            statements.ToSyntaxList(parser->allocator)
        );
    }

    SwitchStatementSyntax* ParseSwitchStatement(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::SwitchKeyword || parser->currentToken.kind == TokenKind::CaseKeyword);

        SyntaxToken switchKeyword;
        SyntaxToken openParen;
        ExpressionSyntax* expression = nullptr;
        SyntaxToken closeParen;
        SyntaxToken openBrace;

        ParseSwitchHeader(parser, &switchKeyword, &openParen, &expression, &closeParen, &openBrace);

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<SwitchSectionSyntax> sections(parser->tempAllocator);

        while (IsPossibleSwitchSection(parser)) {
            sections.Add(ParseSwitchSection(parser));
        }

        return parser->CreateNode<SwitchStatementSyntax>(
            switchKeyword,
            openParen,
            expression,
            closeParen,
            openBrace,
            sections.ToSyntaxList(parser->allocator),
            parser->EatToken(TokenKind::CloseBraceToken)
        );
    }

    bool IsUsingStatementVariableDeclaration(Parser* parser, ScanTypeFlags st) {
        assert(st != ScanTypeFlags::NullableType);

        bool condition1 = st == ScanTypeFlags::MustBeType && parser->currentToken.kind != TokenKind::DotToken;
        bool condition2 = st != ScanTypeFlags::NotType && parser->currentToken.kind == TokenKind::IdentifierToken;
        bool condition3 = st == ScanTypeFlags::NonGenericTypeOrExpression || parser->PeekToken(1).kind == TokenKind::EqualsToken;

        return condition1 || (condition2 && condition3);
    }

    void ParseUsingExpression(Parser* parser, VariableDeclarationSyntax** declaration, ExpressionSyntax** expression, ResetPoint* resetPoint) {

        // Now, this can be either an expression or a decl list

        ScanTypeFlags st = ScanType(parser, false);

        if (st == ScanTypeFlags::NullableType) {
            // We need to handle:
            // * using (f ? x = a : x = b)
            // * using (f ? x = a)
            // * using (f ? x, y)

            if (parser->currentToken.kind != TokenKind::IdentifierToken) {
                resetPoint->Reset();
                *expression = ParseExpression(parser);
            }
            else {
                switch (parser->PeekToken(1).kind) {
                    default:
                        resetPoint->Reset();
                        *expression = ParseExpression(parser);
                        break;

                    case TokenKind::CommaToken:
                    case TokenKind::CloseParenToken:
                        resetPoint->Reset();
                        *declaration = ParseParenthesizedVariableDeclaration(parser);
                        break;

                    case TokenKind::EqualsToken:
                        // Parse it as a decl. If the next token is a : and only one variable was parsed,
                        // convert the whole thing to ?: expression.
                        resetPoint->Reset();
                        VariableDeclarationSyntax* decl = ParseParenthesizedVariableDeclaration(parser);
                        *declaration = decl;
                        // We may have non-nullable types in error scenarios.
                        if (parser->currentToken.kind == TokenKind::ColonToken &&
                            decl->type->GetKind() == SyntaxKind::NullableType &&
                            SyntaxFacts::IsName(((NullableTypeSyntax*) decl->type)->elementType->GetKind()) &&
                            decl->variables->itemCount == 1) {
                            // We have "name? id = expr :" so need to convert to a ?: expression.
                            resetPoint->Reset();
                            *declaration = nullptr;
                            *expression = ParseExpression(parser);
                        }

                        break;
                }
            }
        }
        else if (IsUsingStatementVariableDeclaration(parser, st)) {
            resetPoint->Reset();
            *declaration = ParseParenthesizedVariableDeclaration(parser);
        }
        else {
            // Must be an expression statement
            resetPoint->Reset();
            *expression = ParseExpression(parser);
        }
    }

    UsingStatementSyntax* ParseUsingStatement(Parser* parser) {
        SyntaxToken usingKeyword = parser->EatToken(TokenKind::UsingKeyword);
        SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);

        VariableDeclarationSyntax* declaration = nullptr;
        ExpressionSyntax* expression = nullptr;

        ResetPoint resetPoint(parser, false);
        ParseUsingExpression(parser, &declaration, &expression, &resetPoint);

        return parser->CreateNode<UsingStatementSyntax>(
            usingKeyword,
            openParen,
            declaration,
            expression,
            parser->EatToken(TokenKind::CloseParenToken),
            ParseEmbeddedStatement(parser)
        );
    }

    StatementSyntax* ParseStatementStartingWithUsing(Parser* parser) {
        return parser->PeekToken(1).kind == TokenKind::OpenParenToken
            ? ParseUsingStatement(parser)
            : ParseLocalDeclarationStatement(parser);
    }

    bool IsPossibleLabeledStatement(Parser* parser) {
        return parser->PeekToken(1).kind == TokenKind::ColonToken && IsTrueIdentifier(parser);
    }

    LabeledStatementSyntax* ParseLabeledStatement(Parser* parser) {
        assert(IsTrueIdentifier(parser) && parser->PeekToken(1).kind == TokenKind::ColonToken);

        SyntaxToken identifier = ParseIdentifierToken(parser);
        SyntaxToken colon = parser->EatToken(TokenKind::ColonToken);
        StatementSyntax* statement = ParseStatement(parser);
        if (statement == nullptr) {
            statement = parser->CreateNode<EmptyStatementSyntax>(parser->EatToken(TokenKind::SemicolonToken));
        }
        return parser->CreateNode<LabeledStatementSyntax>(
            identifier,
            colon,
            statement
        );
    }

    StatementSyntax* TryParseStatementStartingWithIdentifier(Parser* parser) {
        if (IsPossibleLabeledStatement(parser)) {
            return ParseLabeledStatement(parser);
        }
        // we don't support yield right now but would add it here if we did
        // else if (IsPossibleYieldStatement(parser)) {
        //     return ParseYieldStatement(parser);
        // }

        return nullptr;
    }

    StatementSyntax* ParseStatement(Parser* parser) {
        ResetPoint resetPointBeforeStatement(parser, false);

        StatementSyntax* result = nullptr;

        switch (parser->currentToken.kind) {
//            case TokenKind::FixedKeyword:
//                return ParseFixedStatement();
//            case TokenKind::LockKeyword:
//                return ParseLockStatement(parser);
//            case TokenKind::UnsafeKeyword:
//                result = TryParseStatementStartingWithUnsafe();
//                if (result != nullptr)
//                    return result;
//                break;
            default:
                break;
            case TokenKind::BreakKeyword:
                return parser->CreateNode<BreakStatementSyntax>(parser->EatToken(TokenKind::BreakKeyword), parser->EatToken(TokenKind::SemicolonToken));
            case TokenKind::ContinueKeyword:
                return parser->CreateNode<ContinueStatementSyntax>(parser->EatToken(TokenKind::ContinueKeyword), parser->EatToken(TokenKind::SemicolonToken));
            case TokenKind::TryKeyword:
            case TokenKind::CatchKeyword:
            case TokenKind::FinallyKeyword:
                return ParseTryStatement(parser);
            case TokenKind::DoKeyword:
                return ParseDoStatement(parser);
            case TokenKind::ForKeyword:
                return ParseForOrForEachStatement(parser);
            case TokenKind::ForEachKeyword:
                return ParseForEachStatement(parser);
            case TokenKind::GotoKeyword:
                return ParseGotoStatement(parser);
            case TokenKind::IfKeyword:
                return ParseIfStatement(parser);
            case TokenKind::ElseKeyword:
                // Including 'else' keyword to handle 'else without if' error cases
                return ParseMisplacedElse(parser);
            case TokenKind::ReturnKeyword: {
                return parser->CreateNode<ReturnStatementSyntax>(
                    parser->EatToken(TokenKind::ReturnKeyword),
                    parser->currentToken.kind != TokenKind::SemicolonToken ? ParsePossibleRefExpression(parser) : nullptr,
                    parser->EatToken(TokenKind::SemicolonToken)
                );
            }
            case TokenKind::SwitchKeyword:
            case TokenKind::CaseKeyword: // error recovery case.
                return ParseSwitchStatement(parser);
            case TokenKind::ThrowKeyword:
                return ParseThrowStatement(parser);

            case TokenKind::UsingKeyword:
                return ParseStatementStartingWithUsing(parser);
            case TokenKind::WhileKeyword:
                return ParseWhileStatement(parser);
            case TokenKind::OpenBraceToken:
                return ParseBlock(parser);
            case TokenKind::SemicolonToken:
                return parser->CreateNode<EmptyStatementSyntax>(parser->EatToken());
            case TokenKind::IdentifierToken:
                result = TryParseStatementStartingWithIdentifier(parser);
                if (result != nullptr)
                    return result;
                break;
        }

        return ParseStatementRest(parser, &resetPointBeforeStatement);

    }

    PostSkipAction SkipBadStatementListTokens(Parser* parser, TokenKind expected) {
        // We know we have a bad statement, so it can't be a local
        // function, meaning we shouldn't consider accessibility
        // modifiers to be the start of a statement
        return SkipBadTokensWithExpectedKind(
            parser,
            [](Parser* parser) { return !IsPossibleStatement(parser, false); },
            [](Parser* parser, TokenKind closeKind) { return parser->currentToken.kind == TokenKind::CloseBraceToken; },
            expected,
            TokenKind::None
        );
    }

    void ParseStatements(Parser* parser, SyntaxListBuilder<StatementSyntax>* statements, bool stopOnSwitchSections) {
        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsPossibleStatementStartOrStop; // partial statements can abort if a new statement starts
        if (stopOnSwitchSections) {
            parser->termState |= TerminatorState::IsSwitchSectionStart;
        }

        int32 lastTokenPosition = -1;
        while (parser->currentToken.kind != TokenKind::CloseBraceToken && parser->currentToken.kind != TokenKind::EndOfFileToken
            && !(stopOnSwitchSections && IsPossibleSwitchSection(parser))
            && IsMakingProgress(parser, &lastTokenPosition)) {
            if (IsPossibleStatement(parser, true)) {
                StatementSyntax* statement = ParseStatement(parser);
                if (statement != nullptr) {
                    statements->Add(statement);
                    continue;
                }
            }

            PostSkipAction action = SkipBadStatementListTokens(parser, TokenKind::CloseBraceToken);

            if (action == PostSkipAction::Abort) {
                break;
            }
        }

        parser->termState = saveTerm;
    }

    // Used to parse normal blocks that appear inside method bodies. For the top level block
    // of a method/accessor useParseMethodOrAccessorBodyBlock.
    BlockSyntax* ParseBlock(Parser* parser) {

        SyntaxToken openBrace = parser->EatToken(TokenKind::OpenBraceToken);

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<StatementSyntax> statements(parser->tempAllocator);

        ParseStatements(parser, &statements, false);

        return parser->CreateNode<BlockSyntax>(
            openBrace,
            statements.ToSyntaxList(parser->allocator),
            parser->EatToken(TokenKind::CloseBraceToken)
        );
    }

    void ParseLambdaBody(Parser* parser, BlockSyntax** block, ExpressionSyntax** expression) {
        if (parser->currentToken.kind == TokenKind::OpenBraceToken) {
            *block = ParseBlock(parser);
            *expression = nullptr;
        }
        else {
            *expression = ParsePossibleRefExpression(parser);
            *block = nullptr;
        }
    }

    LambdaExpressionSyntax* ParseLambdaExpressionWorker(Parser* parser) {
        TokenList* modifiers = ParseAnonymousFunctionModifiers(parser);

        TypeSyntax* returnType = nullptr;
        {
            ResetPoint resetPoint(parser, false);
            returnType = ParseReturnType(parser);
            if (parser->currentToken.kind != TokenKind::OpenParenToken) {
                resetPoint.Reset();
                returnType = nullptr;
            }
        }

        BlockSyntax* block = nullptr;
        ExpressionSyntax* expression = nullptr;

        if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            ParameterListSyntax* paramList = ParseLambdaParameterList(parser);
            SyntaxToken arrow = parser->EatToken(TokenKind::EqualsGreaterThanToken);

            ParseLambdaBody(parser, &block, &expression);

            ParenthesizedLambdaExpressionSyntax m(
                modifiers,
                returnType,
                paramList,
                arrow,
                block,
                expression
            );

            return parser->CreateNode<ParenthesizedLambdaExpressionSyntax>(
                modifiers,
                returnType,
                paramList,
                arrow,
                block,
                expression
            );
        }
        else {
            // Unparenthesized lambda case
            // x => ...
            SyntaxToken identifier = (parser->currentToken.kind != TokenKind::IdentifierToken && parser->PeekToken(1).kind == TokenKind::EqualsGreaterThanToken)
                ? parser->EatTokenAsKind(TokenKind::IdentifierToken)
                : ParseIdentifierToken(parser);


            // Case x=>, x =>
            SyntaxToken arrow = parser->EatToken(TokenKind::EqualsGreaterThanToken);

            ParameterSyntax* parameter = parser->CreateNode<ParameterSyntax>(nullptr, nullptr, identifier, nullptr);

            ParseLambdaBody(parser, &block, &expression);

            return parser->CreateNode<SimpleLambdaExpressionSyntax>(
                modifiers,
                parameter,
                arrow,
                block,
                expression
            );
        }
    }

    LambdaExpressionSyntax* ParseLambdaExpression(Parser* parser) {

        bool parentScopeForceConditionalAccess = parser->forceConditionalAccessExpression;
        parser->forceConditionalAccessExpression = false;

        LambdaExpressionSyntax* result = ParseLambdaExpressionWorker(parser);

        parser->forceConditionalAccessExpression = parentScopeForceConditionalAccess;

        return result;
    }

    // Parse expected lambda expression but assume `x ? () => y :` is a conditional
    // expression rather than a lambda expression with an explicit return type and
    // return null in that case only.
    ExpressionSyntax* TryParseLambdaExpression(Parser* parser) {
        ResetPoint resetPoint(parser, false);
        LambdaExpressionSyntax* result = ParseLambdaExpression(parser);

        if (parser->currentToken.kind == TokenKind::ColonToken && result->GetKind() == SyntaxKind::ParenthesizedLambdaExpression) {
            ParenthesizedLambdaExpressionSyntax* typedResult = (ParenthesizedLambdaExpressionSyntax*) result;
            if (typedResult->returnType->GetKind() == SyntaxKind::NullableType) {
                resetPoint.Reset();
                return nullptr;
            }
        }

        return result;
    }

    static PostSkipAction SkipBadCollectionElementTokens(Parser* parser, TokenKind expected, TokenKind closeKind) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleCollectionElement(p); },
            [](Parser* p, TokenKind closeKind) { return p->currentToken.kind == closeKind; },
            expected,
            closeKind
        );

    }

    CollectionElementSyntax* ParseCollectionElement(Parser* parser) {

        if (parser->currentToken.kind == TokenKind::DotDotToken) {
            return parser->CreateNode<SpreadElementSyntax>(parser->EatToken(), ParseExpression(parser));
        }

        return parser->CreateNode<ExpressionElementSyntax>(ParseExpression(parser));
    }

    CollectionExpressionSyntax* ParseCollectionExpression(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::OpenBracketToken);
        SyntaxToken openBracket = parser->EatToken(TokenKind::OpenBracketToken);
        SeparatedSyntaxList<CollectionElementSyntax>* list = ParseCommaSeparatedSyntaxList<CollectionElementSyntax>(
            parser,
            &openBracket,
            TokenKind::CloseBracketToken,
            [](Parser* p) { return IsPossibleCollectionElement(p); },
            [](Parser* p) { return (SyntaxBase*) ParseCollectionElement(p); },
            &SkipBadCollectionElementTokens,
            true,
            false,
            false
        );

        return parser->CreateNode<CollectionExpressionSyntax>(openBracket, list, parser->EatToken(TokenKind::CloseBracketToken));

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

    bool IsObjectInitializer(SeparatedSyntaxList<ExpressionSyntax>* initializers) {
        // Empty initializer list must be parsed as an object initializer.
        if (initializers->itemCount == 0) {
            return true;
        }

        // We have at least one initializer expression. If at least one initializer expression is a named
        // assignment, this is an object initializer. Otherwise, this is a collection initializer.
        for (int32 i = 0; i < initializers->itemCount; i++) {
            ExpressionSyntax* item = initializers->items[i];

            if (item->GetKind() == SyntaxKind::SimpleAssignmentExpression) {
                AssignmentExpressionSyntax* assignment = (AssignmentExpressionSyntax*) item;
                if ((assignment->left->GetKind() == SyntaxKind::IdentifierName || assignment->left->GetKind() == SyntaxKind::ImplicitElementAccess)) {
                    return true;
                }
            }

        }

        return false;
    }

    bool IsComplexElementInitializer(Parser* parser) {
        return parser->currentToken.kind == TokenKind::OpenBraceToken;
    }

    bool IsDictionaryInitializer(Parser* parser) {
        return parser->currentToken.kind == TokenKind::OpenBracketToken;
    }

    bool IsInitializerMember(Parser* parser) {
        return IsComplexElementInitializer(parser) ||
            IsNamedAssignment(parser) ||
            IsDictionaryInitializer(parser) ||
            IsPossibleExpression(parser);
    }

    InitializerExpressionSyntax* ParseComplexElementInitializer(Parser* parser) {
        SyntaxToken openBrace = parser->EatToken(TokenKind::OpenBraceToken);

        SeparatedSyntaxList<ExpressionSyntax>* initializers = ParseCommaSeparatedSyntaxList<ExpressionSyntax>(
            parser,
            &openBrace,
            TokenKind::CloseBraceToken,
            [](Parser* p) { return IsPossibleExpression(p); },
            [](Parser* p) { return (SyntaxBase*) ParseExpression(p); },
            &SkipBadInitializerListTokens,
            false,
            false,
            false
        );

        return parser->CreateNode<InitializerExpressionSyntax>(
            SyntaxKind::ComplexElementInitializerExpression,
            openBrace,
            initializers,
            parser->EatToken(TokenKind::CloseBraceToken)
        );
    }

    AssignmentExpressionSyntax* ParseDictionaryInitializer(Parser* parser) {
        return parser->CreateNode<AssignmentExpressionSyntax>(
            SyntaxKind::SimpleAssignmentExpression,
            parser->CreateNode<ImplicitElementAccessSyntax>(ParseBracketedArgumentList(parser)),
            parser->EatToken(TokenKind::EqualsToken),
            parser->currentToken.kind == TokenKind::OpenBraceToken
                ? ParseObjectOrCollectionInitializer(parser)
                : ParsePossibleRefExpression(parser)
        );
    }

    AssignmentExpressionSyntax* ParseObjectInitializerNamedAssignment(Parser* parser) {
        return parser->CreateNode<AssignmentExpressionSyntax>(
            SyntaxKind::SimpleAssignmentExpression,
            ParseIdentifierName(parser),
            parser->EatToken(TokenKind::EqualsToken),
            parser->currentToken.kind == TokenKind::OpenBraceToken
                ? ParseObjectOrCollectionInitializer(parser)
                : ParsePossibleRefExpression(parser)
        );
    }

    ExpressionSyntax* ParseObjectOrCollectionInitializerMember(Parser* parser) {
        if (IsComplexElementInitializer(parser)) {
            // { ... }
            return ParseComplexElementInitializer(parser);
        }
        else if (IsDictionaryInitializer(parser)) {
            // [...] = { ... }
            // [...] = ref <expr>
            // [...] = <expr>
            return ParseDictionaryInitializer(parser);
        }
        else if (IsNamedAssignment(parser)) {
            // Name = { ... }
            // Name = ref <expr>
            // Name =  <expr>
            return ParseObjectInitializerNamedAssignment(parser);
        }
        else {
            // <expr>
            // ref <expr>
            return ParsePossibleRefExpression(parser);
        }
    }

    InitializerExpressionSyntax* ParseObjectOrCollectionInitializer(Parser* parser) {
        SyntaxToken openBrace = parser->EatToken(TokenKind::OpenBraceToken);

        SeparatedSyntaxList<ExpressionSyntax>* initializers = ParseCommaSeparatedSyntaxList<ExpressionSyntax>(
            parser,
            &openBrace,
            TokenKind::CloseBraceToken,
            [](Parser* p) { return IsInitializerMember(p); },
            [](Parser* p) { return (SyntaxBase*) ParseObjectOrCollectionInitializerMember(p); },
            &SkipBadInitializerListTokens,
            true,
            false,
            true
        );

        SyntaxKind kind = IsObjectInitializer(initializers) ? SyntaxKind::ObjectInitializerExpression : SyntaxKind::CollectionInitializerExpression;

        return parser->CreateNode<InitializerExpressionSyntax>(
            kind,
            openBrace,
            initializers,
            parser->EatToken(TokenKind::CloseBraceToken)
        );

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
                parser->allocator->New<SeparatedSyntaxList<ArgumentSyntax >>(0, nullptr, 0, nullptr),
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

    StringPartSyntax* ParseInterpolatedStringExpression(Parser * parser) {
        assert(parser->currentToken.kind == TokenKind::InterpolatedExpressionStart);
        SyntaxToken start = parser->EatToken();
        ExpressionSyntax * expression = ParseExpression(parser);
        SyntaxToken end = parser->EatToken(TokenKind::InterpolatedExpressionEnd);
        return parser->CreateNode<InterpolatedStringExpressionSyntax>(start, expression, end);
    }

    ExpressionSyntax* ParseRawStringLiteral(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::RawStringLiteralStart);
        SyntaxToken start = parser->EatToken();
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<StringPartSyntax> builder(parser->tempAllocator);

        while(true) {
            switch (parser->currentToken.kind) {
                case TokenKind::StringLiteralPart: {
                    builder.Add(parser->CreateNode<StringLiteralPartSyntax>(parser->EatToken()));
                    break;
                }
                case TokenKind::InterpolatedIdentifier: {
                    // todo -- do we need to check that the identifier is not reserved?
                    builder.Add(parser->CreateNode<InterpolatedIdentifierPartSyntax>(parser->EatToken()));
                    break;
                }
                case TokenKind::InterpolatedExpressionStart: {
                    builder.Add(ParseInterpolatedStringExpression(parser));
                    break;
                }
                default: {
                    break;
                }
            }
        }

        SyntaxList<StringPartSyntax> * parts = builder.ToSyntaxList(parser->allocator);
        SyntaxToken end = parser->EatToken(TokenKind::RawStringLiteralEnd);
        return parser->CreateNode<RawStringLiteralExpression>(start, parts, end);
    }

    ExpressionSyntax* ParseStringLiteral(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::StringLiteralStart);
        SyntaxToken start = parser->EatToken();
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<StringPartSyntax> builder(parser->tempAllocator);

        while(true) {
            switch (parser->currentToken.kind) {
                case TokenKind::StringLiteralPart: {
                    builder.Add(parser->CreateNode<StringLiteralPartSyntax>(parser->EatToken()));
                    break;
                }
                case TokenKind::InterpolatedIdentifier: {
                    // todo -- do we need to check that the identifier is not reserved?
                    builder.Add(parser->CreateNode<InterpolatedIdentifierPartSyntax>(parser->EatToken()));
                    break;
                }
                case TokenKind::InterpolatedExpressionStart: {
                    builder.Add(ParseInterpolatedStringExpression(parser));
                    break;
                }
                default: {
                    goto end;
                }
            }
        }
end:
        SyntaxList<StringPartSyntax> * parts = builder.ToSyntaxList(parser->allocator);
        SyntaxToken end = parser->EatToken(TokenKind::StringLiteralEnd);
        return parser->CreateNode<StringLiteralExpression>(start, parts, end);
    }

    CharacterLiteralExpressionSyntax* ParseCharacterLiteral(Parser* parser) {
        assert(parser->currentToken.kind == TokenKind::CharLiteralStart);
        SyntaxToken start = parser->EatToken();
        SyntaxToken content;
        if(parser->currentToken.kind != TokenKind::CharLiteralContent) {
            content = parser->CreateMissingToken(TokenKind::CharLiteralContent, ErrorCode::ERR_EmptyCharLiteral, true);
        }
        else {
            content = parser->EatToken();
        }
        SyntaxToken end = parser->EatToken(TokenKind::CharLiteralEnd);
        return parser->CreateNode<CharacterLiteralExpressionSyntax>(start, content, end);
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

                if (IsPossibleLambdaExpression(parser, precedence)) {
                    return ParseLambdaExpression(parser);
                }
                else {
                    ExpressionSyntax* missing = CreateMissingIdentifierName(parser);
                    parser->AddError(missing, ErrorCode::ERR_InvalidExprTerm);
                    return missing;
                }
            case TokenKind::IdentifierToken: {
                if (IsTrueIdentifier(parser)) {

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
                    ? (ExpressionSyntax*) ParseLambdaExpression(parser)
                    : (ExpressionSyntax*) ParseCollectionExpression(parser);

            case TokenKind::ThisKeyword:
                return parser->CreateNode<ThisExpressionSyntax>(parser->EatToken());

            case TokenKind::BaseKeyword: {
                return parser->CreateNode<BaseExpressionSyntax>(parser->EatToken());
            }

            case TokenKind::FalseKeyword:
            case TokenKind::TrueKeyword:
            case TokenKind::NullKeyword:
            case TokenKind::NumericLiteralToken:
            case TokenKind::StringLiteralEmpty:
                return parser->CreateNode<LiteralExpressionSyntax>(SyntaxFacts::GetLiteralExpression(tk), parser->EatToken());
            case TokenKind::RawStringLiteralStart: {
                return ParseRawStringLiteral(parser);
            }
            case TokenKind::StringLiteralStart: {
                return ParseStringLiteral(parser);
            }
            case TokenKind::CharLiteralStart: {
                return ParseCharacterLiteral(parser);
            }
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

//            case TokenKind::DelegateKeyword:
//                // check for lambda expression with explicit function pointer return type
//                return IsPossibleLambdaExpression(parser, precedence)
//                    ? ParseLambdaExpression(parser)
//                    : ParseAnonymousMethodExpression(parser);
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

    ListPatternSyntax* ParseListPattern(Parser* parser, bool whenIsKeyword) {
        SyntaxToken openBracket = parser->EatToken(TokenKind::OpenBracketToken);
        SeparatedSyntaxList<PatternSyntax>* list = ParseCommaSeparatedSyntaxList<PatternSyntax>(
            parser,
            &openBracket,
            TokenKind::CloseBracketToken,
            [](Parser* p) { return IsPossibleSubpatternElement(p); },
            [](Parser* p) { return (SyntaxBase*) ParsePattern(p, Precedence::Conditional); },
            &SkipBadPatternListTokens,
            true,
            false,
            false
        );
        return parser->CreateNode<ListPatternSyntax>(
            openBracket,
            list,
            parser->EatToken(TokenKind::CloseBracketToken),
            TryParseSimpleDesignation(parser, whenIsKeyword)
        );
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

    SyntaxBase* ConvertPatternToExpressionIfPossible(Parser* parser, PatternSyntax* pattern, bool permitTypeArguments) {
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
            default:
                return false;
        }

    }

    bool IsPossibleParameter(Parser* parser) {
        switch (parser->currentToken.kind) {
//            case TokenKind::OpenBracketToken: // attribute
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
        if ((parser->termState & TerminatorState::IsExpressionOrPatternInCaseLabelOfSwitchStatement) != 0 && parser->currentToken.kind == TokenKind::ColonToken) {
            return PostSkipAction::Abort;
        }

        // This is pretty much the same as above, but for switch expressions and `=>` and `:` tokens.
        // The reason why we cannot use single flag for both cases is because we want `=>` to be the "exit" token only for switch expressions.
        // Consider the following example: `case (() => 0):`. Normally `=>` is treated as bad separator, so we parse this basically the same as `case ((), 1):`, which is syntactically valid.
        // However, if we treated `=>` as "exit" token, parsing wouldn't consume full case label properly and would produce a lot of noise errors.
        // We can afford `:` to be the exit token for switch expressions because error recovery is already good enough and treats `:` as bad `=>`,
        // meaning that switch expression arm `{ : 1` can be recovered to `{ } => 1` where the closing `}` is missing and instead of `=>` we have `:`.
        if ((parser->termState & TerminatorState::IsPatternInSwitchExpressionArm) != 0 && parser->currentToken.kind == TokenKind::EqualsGreaterThanToken || parser->currentToken.kind == TokenKind::ColonToken) {
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

    WhenClauseSyntax* ParseWhenClause(Parser* parser, Precedence precedence) {
        if (parser->currentToken.contextualKind != TokenKind::WhenKeyword) {
            return nullptr;
        }

        SyntaxToken whenKeyword = parser->EatContextualToken(TokenKind::WhenKeyword);
        ExpressionSyntax* expression = ParseSubExpression(parser, precedence);
        return parser->CreateNode<WhenClauseSyntax>(whenKeyword, expression);
    }

    SeparatedSyntaxList<SwitchExpressionArmSyntax>* ParseSwitchExpressionArms(Parser* parser) {
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SeparatedSyntaxListBuilder<SwitchExpressionArmSyntax> arms(parser->tempAllocator);
        while (parser->currentToken.kind != TokenKind::CloseBraceToken) {
            // Help out in the case where a user is converting a switch statement to a switch expression. Note:
            // `default(...)` and `default` will also be consumed as a legal syntactic patterns (though the
            // latter will fail during binding).  So if the user has `default:` we will recover fine as we
            // handle the errant colon below.

            SyntaxToken errantCase;
            if (parser->currentToken.kind == TokenKind::CaseKeyword) {
                errantCase = parser->EatToken();
                parser->AddError(errantCase, ErrorCode::ERR_BadCaseInSwitchArm);
            }

            TerminatorState savedState = parser->termState;
            parser->termState |= TerminatorState::IsPatternInSwitchExpressionArm;
            PatternSyntax* pattern = ParsePattern(parser, Precedence::Coalescing, true);
            parser->termState = savedState;

            // We use a precedence that excludes lambdas, assignments, and a conditional which could have a
            // lambda on the right, because we need the parser to leave the EqualsGreaterThanToken to be
            // consumed by the switch arm. The strange side-effect of that is that the conditional expression is
            // not permitted as a constant expression here; it would have to be parenthesized.

            int32 ptr = parser->ptr;
            WhenClauseSyntax* whenClause = ParseWhenClause(parser, Precedence::Coalescing);
            // Help out in the case where a user is converting a switch statement to a switch expression.
            // Consume the `:` as a `=>` and report an error.
            SyntaxToken gteToken = parser->currentToken.kind == TokenKind::ColonToken
                ? parser->EatTokenAsKind(TokenKind::EqualsGreaterThanToken)
                : parser->EatToken(TokenKind::EqualsGreaterThanToken);

            ExpressionSyntax* expression = ParseExpression(parser);
            SwitchExpressionArmSyntax* switchExpressionCase = parser->CreateNode<SwitchExpressionArmSyntax>(
                pattern,
                whenClause,
                gteToken,
                expression
            );

            // If we're not making progress, abort
            if (!errantCase.IsValid() && ptr == parser->ptr && parser->currentToken.kind != TokenKind::CommaToken) {
                break;
            }

            arms.Add(switchExpressionCase);
            if (parser->currentToken.kind != TokenKind::CloseBraceToken) {
                SyntaxToken commaToken = parser->currentToken.kind == TokenKind::SemicolonToken
                    ? parser->EatTokenAsKind(TokenKind::CommaToken)
                    : parser->EatToken(TokenKind::CommaToken);
                arms.AddSeparator(commaToken);
            }
        }

        return arms.ToList(parser->allocator);

    }

    SwitchExpressionSyntax* ParseSwitchExpression(Parser* parser, ExpressionSyntax* governingExpression, SyntaxToken switchKeyword) {
        // For better error recovery when an expression is typed on a line before a switch statement,
        // the caller checks if the switch keyword is followed by an open curly brace. Only if it is
        // would we attempt to parse it as a switch expression here.

        SyntaxToken open = parser->EatToken(TokenKind::OpenBraceToken);
        SeparatedSyntaxList<SwitchExpressionArmSyntax>* arms = ParseSwitchExpressionArms(parser);
        SyntaxToken close = parser->EatToken(TokenKind::CloseBraceToken);

        return parser->CreateNode<SwitchExpressionSyntax>(
            governingExpression,
            switchKeyword,
            open,
            arms,
            close
        );

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
//            if (current is ConditionalExpressionSyntax conditionalExpression && conditionalExpression.WhenTrue.GetFirstToken().Kind == TokenKind::OpenBracketToken) {
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
//                SyntaxFactory.MissingToken(TokenKind::ColonToken),
//                _syntaxFactory.IdentifierName(SyntaxFactory.MissingToken(TokenKind::IdentifierToken)));
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
        NameColonSyntax* nameColon;

        if (parser->currentToken.kind == TokenKind::IdentifierToken && parser->PeekToken(1).kind == TokenKind::ColonToken) {
            IdentifierNameSyntax* identifierName = ParseIdentifierName(parser);
            SyntaxToken colonToken = parser->EatToken(TokenKind::ColonToken);
            nameColon = parser->CreateNode<NameColonSyntax>(
                identifierName,
                colonToken
            );
        }
        else {
            nameColon = nullptr;
        }

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

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfArgumentList;
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

        parser->termState = saveTerm;

        // convert `]` into `)` or vice versa for error recovery
        if(parser->currentToken.kind == TokenKind::CloseParenToken || parser->currentToken.kind == TokenKind::CloseBracketToken) {
            *closeToken = parser->EatTokenAsKind(closeKind);
        }
        else {
            *closeToken = parser->EatToken(closeKind);
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

        ArgumentListSyntax* argumentList;
        if (parser->currentToken.kind == TokenKind::OpenParenToken) {
            argumentList = ParseParenthesizedArgumentList(parser);
        }
        else {
            SyntaxToken openParen = parser->EatToken(TokenKind::OpenParenToken);
            SyntaxToken closeParen = parser->CreateMissingToken(TokenKind::CloseParenToken);
            argumentList = parser->CreateNode<ArgumentListSyntax>(openParen, nullptr, closeParen);
        }

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
        SyntaxToken gteToken = parser->EatToken(TokenKind::EqualsGreaterThanToken);
        ExpressionSyntax* expression = ParsePossibleRefExpression(parser);
        return parser->CreateNode<ArrowExpressionClauseSyntax>(gteToken, expression);
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

        SyntaxList<AttributeListSyntax>* attributes = nullptr;

        SyntaxToken name = ParseIdentifierToken(parser);
        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfMethodSignature;
        ParameterListSyntax* paramList = ParseParenthesizedParameterList(parser);
        ConstructorInitializerSyntax* initializer = parser->currentToken.kind == TokenKind::ColonToken
            ? ParseConstructorInitializer(parser)
            : nullptr;

        BlockSyntax* blockBody = nullptr;
        ArrowExpressionClauseSyntax* expressionBody = nullptr;
        SyntaxToken semicolon;

        ParseBlockAndExpressionBodiesWithSemicolon(parser, &blockBody, &expressionBody, &semicolon);

        parser->termState = saveTerm;
        return parser->CreateNode<ConstructorDeclarationSyntax>(
            attributes,
            modifiers->Persist(parser->allocator),
            name, paramList,
            initializer,
            blockBody,
            expressionBody,
            semicolon
        );
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

        // implicit conversion float(Thing t) {}
        // implicit conversion
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
        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfReturnType;
        TypeSyntax* type = ParseTypeOrVoid(parser);
        parser->termState = saveTerm;
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

    bool IsStartOfTypeParameter(Parser* parser) {
        if (IsCurrentTokenWhereOfConstraintClause(parser)) {
            return false;
        }

        // possible attributes
        // if (parser->currentToken.kind == TokenKind::OpenBracketToken && parser->PeekToken(1).kind != TokenKind::CloseBracketToken) {
        //     return true;
        // }

        return IsTrueIdentifier(parser);
    }

    TypeParameterSyntax* ParseTypeParameter(Parser* parser) {
        if (IsCurrentTokenWhereOfConstraintClause(parser)) {
            SyntaxToken missing = MakeMissingToken(TokenKind::IdentifierToken, parser->ptr);
            parser->AddError(missing, ErrorCode::ERR_IdentifierExpected);
            return parser->CreateNode<TypeParameterSyntax>(missing);
        }

        // SyntaxListBuilder<sAttributeListSyntax> attrList(parser->tempAllocator);
        // if (parser->currentToken.kind == TokenKind::OpenBracketToken && parser->PeekToken(1).kind != TokenKind::CloseBracketToken) {
        //     TerminatorState saveTerm = parser->_termState;
        //     parser->_termState = TerminatorState::IsEndOfTypeArgumentList;
        //     attrs = ParseAttributeDeclarations(parser, false);
        //     parser->_termState = saveTerm;
        // }

        return parser->CreateNode<TypeParameterSyntax>(ParseIdentifierToken(parser));
    }

    PostSkipAction SkipBadTypeParameterListTokens(Parser* parser, TokenKind expected, TokenKind closeKind) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* p) { return p->currentToken.kind != TokenKind::CommaToken && !IsPossibleType(p); },
            [](Parser* p, TokenKind closeKind) { return p->currentToken.kind == TokenKind::GreaterThanToken; },
            expected
        );
    }

    TypeParameterListSyntax* ParseTypeParameterList(Parser* parser) {
        if (parser->currentToken.kind != TokenKind::LessThanToken) {
            return nullptr;
        }

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfTypeParameterList;
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

    void ParseParameterModifiers(Parser* parser, TokenListBuffer* modifiers, bool isFunctionPointerParameter) {
//        bool tryScoped = true;

        while (IsParameterModifierExcludingScoped(parser->currentToken)) {
            // TokenKind tk = parser->currentToken.kind;
            //
            // if (tk == TokenKind::RefKeyword || tk == TokenKind:: OutKeyword || tk == TokenKind:: ReadOnlyKeyword) {
            //     tryScoped = false;
            // }

            modifiers->Add(parser->EatToken());
        }

//        if (tryScoped) {
//            SyntaxToken scopedKeyword = ParsePossibleScopedKeyword(isFunctionPointerParameter);
//
//            if (scopedKeyword.IsValid()) {
//                modifiers.Add(scopedKeyword);
//
//                // Look if ref/out/in/readonly are next
//                while (par is (TokenKind::RefKeyword or TokenKind::OutKeyword or TokenKind::InKeyword or TokenKind::ReadOnlyKeyword)) {
//                    modifiers.Add(this.EatToken());
//                }
//            }
//        }
    }

    ParameterSyntax* ParseParameter(Parser* parser) {

        // var attributes = this.ParseAttributeDeclarations(inExpressionContext: false);

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        TokenListBuffer modifiers(parser->tempAllocator);

        ParseParameterModifiers(parser, &modifiers, false);

        TypeSyntax* type = ParseType(parser, ParseTypeMode::Parameter);
        SyntaxToken identifier;

        if (parser->currentToken.kind == TokenKind::IdentifierToken && IsCurrentTokenWhereOfConstraintClause(parser)) {
            identifier = MakeMissingToken(TokenKind::IdentifierToken, parser->ptr);
            parser->AddError(identifier, ErrorCode::ERR_IdentifierExpected);
        }
        else {
            identifier = ParseIdentifierToken(parser);
        }

        // When the user type "int goo[]", give them a useful error
        if (parser->currentToken.kind == TokenKind::OpenBracketToken && parser->PeekToken(1).kind == TokenKind::CloseBracketToken) {
            SyntaxToken open = parser->EatToken();
            parser->EatToken(); // skip ]
            parser->AddError(open, ErrorCode::ERR_BadArraySyntax);
        }


        // If we didn't already consume an equals sign as part of !!=, then try to scan one out now.

        SyntaxToken equalsToken;
        if (parser->currentToken.kind == TokenKind::EqualsToken) {
            equalsToken = parser->EatToken();
        }

        return parser->CreateNode<ParameterSyntax>(
            modifiers.Persist(parser->allocator),
            type,
            identifier,
            equalsToken.IsValid()
                ? parser->CreateNode<EqualsValueClauseSyntax>(equalsToken, ParseExpression(parser))
                : nullptr
        );

    }

    SeparatedSyntaxList<ParameterSyntax>* ParseParameterList(Parser* parser, SyntaxToken* open, SyntaxToken* close, TokenKind openKind, TokenKind closeKind) {
        *open = parser->EatToken(openKind);

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfParameterList;

        SeparatedSyntaxList<ParameterSyntax>* parameters = ParseCommaSeparatedSyntaxList<ParameterSyntax>(
            parser,
            open,
            closeKind,
            [](Parser* p) { return IsPossibleParameter(p); },
            [](Parser* p) { return (SyntaxBase*) ParseParameter(p); },
            &SkipBadParameterListTokens,
            false,
            false,
            false
        );

        parser->termState = saveTerm;
        *close = parser->EatToken(closeKind);

        return parameters;

    }

    ParameterListSyntax* ParseParenthesizedParameterList(Parser* parser) {
        SyntaxToken open;
        SyntaxToken close;
        SeparatedSyntaxList<ParameterSyntax>* parameters = ParseParameterList(parser, &open, &close, TokenKind::OpenParenToken, TokenKind::CloseParenToken);
        return parser->CreateNode<ParameterListSyntax>(open, parameters, close);
    }

    LocalFunctionStatementSyntax* TryParseLocalFunctionStatementBody(Parser* parser, TokenListBuffer* modifiers, TypeSyntax* returnType, SyntaxToken name) {
        // we've already got the return type
        TypeParameterListSyntax* typeParameterListOpt = ParseTypeParameterList(parser);
        ParameterListSyntax* paramList = ParseParenthesizedParameterList(parser);

        SyntaxList<TypeParameterConstraintClauseSyntax>* constraints = nullptr;
        if (parser->currentToken.contextualKind == TokenKind::WhereKeyword) {
            TempAllocator::ScopedMarker m(parser->tempAllocator);
            SyntaxListBuilder<TypeParameterConstraintClauseSyntax> builder(parser->tempAllocator);
            ParseTypeParameterConstraintClauses(parser, &builder);
            constraints = builder.ToSyntaxList(parser->allocator);
        }

        BlockSyntax* blockBody = nullptr;
        ArrowExpressionClauseSyntax* expressionBody = nullptr;
        SyntaxToken semicolon;

        ParseBlockAndExpressionBodiesWithSemicolon(parser, &blockBody, &expressionBody, &semicolon, false);

        return parser->CreateNode<LocalFunctionStatementSyntax>(
            modifiers->Persist(parser->allocator),
            returnType,
            name,
            typeParameterListOpt,
            paramList,
            constraints,
            blockBody,
            expressionBody,
            semicolon
        );
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

        SyntaxToken openToken;
        SyntaxToken closeToken;
        SeparatedSyntaxList<ArgumentSyntax>* argumentList = nullptr;

        ParseArgumentList(parser, TokenKind::OpenBracketToken, TokenKind::CloseBracketToken, &openToken, &argumentList, &closeToken);

        return parser->CreateNode<BracketedArgumentListSyntax>(openToken, argumentList, closeToken);

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
        TerminatorState saveTerm = parser->termState;
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
                parser->termState |= TerminatorState::IsPossibleEndOfVariableDeclaration;
                argumentList = ParseBracketedArgumentList(parser);
                parser->termState = saveTerm;
                parser->AddError(argumentList, ErrorCode::ERR_BadVarDecl);
                break;
            }
            case TokenKind::OpenBracketToken: {
                open_bracket_label:
                bool sawNonOmittedSize;
                parser->termState |= TerminatorState::IsPossibleEndOfVariableDeclaration;
                ArrayRankSpecifierSyntax* specifier = ParseArrayRankSpecifier(parser, &sawNonOmittedSize);
                parser->termState = saveTerm;
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

                SeparatedSyntaxList<ArgumentSyntax>* argList = parser->allocator->New<SeparatedSyntaxList<ArgumentSyntax >>(sizes->itemCount, args, sizes->separatorCount, sizes->separators);

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
        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsEndOfFieldDeclaration;

        LocalFunctionStatementSyntax* local = nullptr;
        ParseVariableDeclarators(parser, type, flags, &variables, variableDeclarationsExpected, false, false, nullptr, &local);

        parser->termState = saveTerm;
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

    bool IsPossibleAttributeDeclaration(Parser* parser) {
        // Have to at least start with `[` to be an attribute
        if (parser->currentToken.kind != TokenKind::OpenBracketToken) {
            return false;
        }

        ResetPoint resetPoint(parser);

        parser->EatToken();

        if (IsTrueIdentifier(parser)) {
            return true;
        }

        if (SyntaxFacts::IsLiteralExpression(parser->currentToken.kind)) {
            return false;
        }

        return true;

    }

    AttributeSyntax* ParseAttribute(Parser* parser) {
        NameSyntax* nameSyntax = ParseQualifiedName(parser, NameOptions::None);
        ArgumentListSyntax* argumentList = ParseParenthesizedArgumentList(parser);
        return parser->CreateNode<AttributeSyntax>(nameSyntax, argumentList);
    }

    PostSkipAction SkipBadAttributeListTokens(Parser* parser, TokenKind expectedKind, TokenKind closeKind) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            [](Parser* parser) { return parser->currentToken.kind != TokenKind::CommaToken && !IsPossibleAttribute(parser); },
            [](Parser* parser, TokenKind closeKind) { return parser->currentToken.kind == closeKind; },
            expectedKind,
            closeKind
        );
    }

    AttributeListSyntax* TryParseAttributeDeclaration(Parser* parser) {

        if (parser->currentToken.kind != TokenKind::OpenBracketToken) {
            return nullptr;
        }

        SyntaxToken openBracket = parser->EatToken(TokenKind::OpenBracketToken);

        SeparatedSyntaxList<AttributeSyntax>* attributes = ParseCommaSeparatedSyntaxList<AttributeSyntax>(
            parser,
            &openBracket,
            TokenKind::CloseBracketToken,
            [](Parser* parser) { return IsPossibleAttribute(parser); },
            [](Parser* parser) { return (SyntaxBase*) ParseAttribute(parser); },
            &SkipBadAttributeListTokens,
            true,
            true,
            false
        );

        SyntaxToken closeBracket = parser->EatToken(TokenKind::CloseBracketToken);

        return parser->CreateNode<AttributeListSyntax>(openBracket, attributes, closeBracket);

    }

    SyntaxList<AttributeListSyntax>* ParseAttributeDeclarations(Parser* parser) {

        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<AttributeListSyntax> attributes(parser->tempAllocator);

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsAttributeDeclarationTerminator;

        while (IsPossibleAttributeDeclaration(parser)) {
            AttributeListSyntax* attributeDeclaration = TryParseAttributeDeclaration(parser);
            if (attributeDeclaration == nullptr) {
                break;
            }

            attributes.Add(attributeDeclaration);
        }

        parser->termState = saveTerm;

        return attributes.ToSyntaxList(parser->allocator);

    }

    struct TermStateGuard {
        Parser * parser;
        TerminatorState resetState;

        explicit TermStateGuard(Parser * parser)
            : parser(parser)
            , resetState(parser->termState)
            {}

        ~TermStateGuard() {
            parser->termState = resetState;
        }

    };

    #define TERM_STATE_GUARD(x) TermStateGuard xyz_guard(x)

    BracketedParameterListSyntax* ParseBracketedParameterList(Parser * parser) {
        SyntaxToken open;
        SyntaxToken close;
        SeparatedSyntaxList<ParameterSyntax>* parameters = ParseParameterList(parser, &open,&close, TokenKind::OpenBracketToken, TokenKind::CloseBracketToken);
        return parser->CreateNode<BracketedParameterListSyntax>(open, parameters, close);
    }

    bool IsPossibleAccessor(Parser * parser) {
        return parser->currentToken.kind == TokenKind::IdentifierToken
               || SyntaxFacts::GetAccessorDeclarationKind(parser->currentToken.contextualKind) != SyntaxKind::None
               || parser->currentToken.kind== TokenKind::OpenBraceToken  // for accessor blocks w/ missing keyword
               || parser->currentToken.kind == TokenKind::SemicolonToken // for empty body accessors w/ missing keyword
               || IsPossibleAccessorModifier(parser);
    }

    PostSkipAction SkipBadTokensWithErrorCode(Parser * parser, notExpectedFn isNotExpectedFunction, abortFn abortFunction, ErrorCode errorCode) {
        bool first = true;
        PostSkipAction action = PostSkipAction::Continue;
        while (isNotExpectedFunction(parser))  {

            if (abortFunction(parser, TokenKind::None)) {
                action = PostSkipAction::Abort;
                break;
            }

            SyntaxToken token = parser->EatToken();

            if(first) {
                parser->AddError(token, errorCode);
                first = false;
            }
        }

        return action;
    }

    PostSkipAction SkipBadAccessorListTokens(Parser * parser, ErrorCode error) {
        return SkipBadTokensWithErrorCode(
            parser,
            [](Parser * p) { return p->currentToken.kind != TokenKind::CloseBraceToken && !IsPossibleAccessor(p); },
            [](Parser * p, TokenKind unused) { return IsTerminator(p); },
            error
        );
    }

    AccessorDeclarationSyntax* ParseAccessorDeclaration(Parser * parser) {
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        TokenListBuffer modifiers(parser->tempAllocator);

        bool unused = false;
        ParseModifiers(parser, &modifiers, true, &unused);

        SyntaxToken accessorName = parser->EatToken(TokenKind::IdentifierToken, ErrorCode::ERR_GetOrSetExpected);
        SyntaxKind accessorKind = SyntaxFacts::GetAccessorDeclarationKind(accessorName.contextualKind);

        // Only convert the identifier to a keyword if it's a valid one.  Otherwise any
        // other contextual keyword (like 'partial') will be converted into a keyword
        // and will be invalid.
        if (accessorKind == SyntaxKind::None) {
            // We'll have an UnknownAccessorDeclaration either because we didn't have
            // an IdentifierToken or because we have an IdentifierToken which is not
            // add/remove/get/set.  In the former case, we'll already have reported
            // an error and will have a missing token.  But in the latter case we need
            // to report that the identifier is incorrect.
            if (!accessorName.IsMissing())  {
                parser->AddError(accessorName, ErrorCode::ERR_GetOrSetExpected);
            }
            else {
                assert(accessorName.ContainsDiagnostics());
            }
        }
        else {
            accessorName = ConvertToKeyword(accessorName);
        }

        BlockSyntax * blockBody = nullptr;
        ArrowExpressionClauseSyntax * expressionBody = nullptr;
        SyntaxToken semicolon;

        bool currentTokenIsSemicolon = parser->currentToken.kind == TokenKind::SemicolonToken;
        bool currentTokenIsArrow = parser->currentToken.kind == TokenKind::EqualsGreaterThanToken;
        bool currentTokenIsOpenBraceToken = parser->currentToken.kind == TokenKind::OpenBraceToken;

        if (currentTokenIsOpenBraceToken || currentTokenIsArrow)  {
            ParseBlockAndExpressionBodiesWithSemicolon(parser, &blockBody, &expressionBody, &semicolon);
        }
        else if (currentTokenIsSemicolon) {
            semicolon = parser->EatToken(TokenKind::SemicolonToken, ErrorCode::ERR_SemiOrLBraceOrArrowExpected);
        }
        else {
            // We didn't get something we recognized.  If we got an accessor type we
            // recognized (i.e. get/set/init) then try to parse out a block.
            // Only do this if it doesn't seem like we're at the end of the accessor/property.
            // for example, if we have "get set", don't actually try to parse out the
            // block.  Otherwise we'll consume the 'set'.  In that case, just end the
            // current accessor with a semicolon so we can properly consume the next
            // in the calling method's loop.
            if (accessorKind != SyntaxKind::None) {
                if (!IsTerminator(parser)) {
                    blockBody = ParseMethodOrAccessorBodyBlock(parser, true);
                }
                else {
                    semicolon = parser->EatToken(TokenKind::SemicolonToken, ErrorCode::ERR_SemiOrLBraceOrArrowExpected);
                }
            }
            else {
                // Don't bother eating anything if we didn't even have a valid accessor.
                // It will just lead to more errors.  Note: we will have already produced
                // a good error by now.
                assert(accessorName.ContainsDiagnostics());
            }
        }

        return parser->CreateNode<AccessorDeclarationSyntax>(
            accessorKind,
            modifiers.Persist(parser->allocator),
            accessorName,
            blockBody,
            expressionBody,
            semicolon
        );
    }

    AccessorListSyntax * ParseAccessorList(Parser * parser) {
        SyntaxToken openBrace = parser->EatToken(TokenKind::OpenBraceToken);
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SyntaxListBuilder<AccessorDeclarationSyntax> accessors(parser->tempAllocator);

        if (!openBrace.IsMissing() || !IsTerminator(parser)) {
            // parse property accessors

            while (true) {
                if (parser->currentToken.kind == TokenKind::CloseBraceToken) {
                    break;
                }
                else if (IsPossibleAccessor(parser)) {
                    accessors.Add(ParseAccessorDeclaration(parser));
                }
                else if (SkipBadAccessorListTokens(parser, ErrorCode::ERR_GetOrSetExpected) == PostSkipAction::Abort) {
                    break;
                }
            }

        }

        SyntaxToken closeBrace = parser->EatToken(TokenKind::CloseBraceToken);
        return parser->CreateNode<AccessorListSyntax>(openBrace, accessors.ToSyntaxList(parser->allocator), closeBrace);
    }

    IndexerDeclarationSyntax* ParseIndexerDeclaration(Parser * parser, SyntaxList<AttributeListSyntax> * attributes, TokenListBuffer * modifiers, TypeSyntax * type, SyntaxToken thisKeyword, TypeParameterListSyntax * typeParameterList) {

        // check to see if the user tried to create a generic indexer.
        if (typeParameterList != nullptr) {
            parser->AddError(typeParameterList, ErrorCode::ERR_UnexpectedGenericName);
        }

        BracketedParameterListSyntax * parameterList = ParseBracketedParameterList(parser);

        AccessorListSyntax* accessorList = nullptr;
        ArrowExpressionClauseSyntax * expressionBody = nullptr;
        SyntaxToken semicolon ;
        // Try to parse accessor list unless there is an expression
        // body and no accessor list
        if (parser->currentToken.kind == TokenKind::EqualsGreaterThanToken) {
            expressionBody = ParseArrowExpressionClause(parser);
            semicolon = parser->EatToken(TokenKind::SemicolonToken);
        }
        else {
            accessorList = ParseAccessorList(parser);
            if (parser->currentToken.kind == TokenKind::SemicolonToken) {
                SyntaxToken skip = parser->EatToken(); // todo -- skip this
                parser->AddError(skip, ErrorCode::ERR_UnexpectedSemicolon);
            }
        }

        // If the user has erroneously provided both an accessor list
        // and an expression body, but no semicolon, we want to parse
        // the expression body and report the error (which is done later)
        if (parser->currentToken.kind == TokenKind::EqualsGreaterThanToken && !semicolon.IsValid()) {
            expressionBody = ParseArrowExpressionClause(parser);
            semicolon = parser->EatToken(TokenKind::SemicolonToken);
        }

        return parser->CreateNode<IndexerDeclarationSyntax>(
            attributes,
            modifiers->Persist(parser->allocator),
            type,
            thisKeyword,
            parameterList,
            accessorList,
            expressionBody,
            semicolon
        );
    }

    MemberDeclarationSyntax* ParseMemberDeclaration(Parser* parser, SyntaxKind parentKind) {
        assert(parentKind != SyntaxKind::CompilationUnit);

        TERM_STATE_GUARD(parser);

        TerminatorState saveTermState = parser->termState;

        SyntaxList<AttributeListSyntax>* attributes = ParseAttributeDeclarations(parser);

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
            return ParseTypeDeclaration(parser, attributes, &modifiers);
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

        // At this point we can either have indexers, methods, or properties or something invalid

        if (!IsPossibleMemberName(parser)) {
            // we haven't advanced, the caller needs to consume the tokens ahead
            if (attributes->size == 0 && modifiers.size == 0 && type->IsMissing() && type->GetKind() != SyntaxKind::RefType) {
                return nullptr;
            }
            // otherwise return an incomplete member
            return parser->CreateNode<IncompleteMemberSyntax>(attributes, modifiers.Persist(parser->allocator), type);
        }

        bool isThisKeyword = parser->currentToken.kind == TokenKind::ThisKeyword;
        SyntaxToken identifierOrThisOpt = parser->EatToken();
        TypeParameterListSyntax *typeParameterListOpt = ParseTypeParameterList(parser);

        if(isThisKeyword) {
            return ParseIndexerDeclaration(parser, attributes, modifiers, type, identifierOrThisOpt, typeParameterListOpt);
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

    CompilationUnitSyntax* ParseCompilationUnit(Parser * parser)
    {
        SyntaxToken tmp;

        NamespaceBodyBuilder body;

        ParseNamespaceBody(parser, &tmp, &body, ref initialBadNodes, SyntaxKind.CompilationUnit);

        SyntaxToken eof = parser->EatToken(TokenKind::EndOfFileToken);

        return parser->CreateNode<CompilationUnitSyntax>(body.Externs, body.Usings, body.Attributes, body.Members, eof);

    }

    void ParseNamespaceBody(Parser* parser, SyntaxToken openBraceOrSemiColon, NamespaceBodyBuilder* bodyBuilder,  SyntaxKind parentKind) {

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsNamespaceMemberStartOrStop;
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
                    NOT_IMPLEMENTED("using");
//                    ParseUsingDirective(parser, openBraceOrSemiColon, bodyBuilder, initialBadNodes, &seen, &pendingIncompleteMembers);
                    reportUnexpectedToken = true;
                    break;
                }

                default: {
                    MemberDeclarationSyntax* memberOrStatement = ParseMemberDeclaration(parser, parentKind);
                    if (memberOrStatement == nullptr) {
                        // incomplete members must be processed before we add any nodes to the body:
                        ReduceIncompleteMembers(parser, &pendingIncompleteMembers, openBraceOrSemiColon, bodyBuilder);

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
        parser->termState = saveTerm;
        // adds pending incomplete nodes:
        AddIncompleteMembers(pendingIncompleteMembers, body);

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
            case TokenKind::RawStringLiteralStart:
            case TokenKind::StringLiteralStart:
            case TokenKind::CharLiteralStart:
            case TokenKind::NewKeyword:
            case TokenKind::DelegateKeyword:
            case TokenKind::ThrowKeyword:
//            case TokenKind::StackAllocKeyword: probably allow something like this
            case TokenKind::DotDotToken:
            case TokenKind::RefKeyword:
            case TokenKind::OpenBracketToken: // collection expression
                return true;
            case TokenKind::StaticKeyword:
                return IsPossibleLambdaExpression(parser, Precedence::Expression);
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

    bool IsEndOfFixedStatement(Parser* parser) {
        TokenKind tk = parser->currentToken.kind;
        return tk == TokenKind::CloseParenToken || tk == TokenKind::OpenBraceToken || tk == TokenKind::SemicolonToken;
    }

    bool IsEndOfTryBlock(Parser* parser) {
        TokenKind tk = parser->currentToken.kind;
        return tk == TokenKind::CloseBraceToken || tk == TokenKind::CatchKeyword || tk == TokenKind::FinallyKeyword;
    }

    bool IsEndOfCatchClause(Parser* parser) {
        TokenKind tk = parser->currentToken.kind;
        return tk == TokenKind::CloseParenToken ||
            tk == TokenKind::OpenBraceToken ||
            tk == TokenKind::CloseBraceToken ||
            tk == TokenKind::CatchKeyword ||
            tk == TokenKind::FinallyKeyword;
    }

    bool IsEndOfFilterClause(Parser* parser) {
        TokenKind tk = parser->currentToken.kind;

        return tk == TokenKind::CloseParenToken ||
            tk == TokenKind::OpenBraceToken ||
            tk == TokenKind::CloseBraceToken ||
            tk == TokenKind::CatchKeyword ||
            tk == TokenKind::FinallyKeyword;

    }

    bool IsEndOfCatchBlock(Parser* parser) {
        TokenKind tk = parser->currentToken.kind;

        return tk == TokenKind::CloseBraceToken ||
            tk == TokenKind::CatchKeyword ||
            tk == TokenKind::FinallyKeyword;
    }

    bool IsEndOfDoWhileExpression(Parser* parser) {
        return parser->currentToken.kind == TokenKind::CloseParenToken || parser->currentToken.kind == TokenKind::SemicolonToken;
    }

    bool IsEndOfForStatementArgument(Parser* parser) {
        TokenKind tk = parser->currentToken.kind;
        return tk == TokenKind::SemicolonToken || tk == TokenKind::CloseParenToken || tk == TokenKind::OpenBraceToken;
    }

    bool IsEndOfDeclarationClause(Parser* parser) {
        return parser->currentToken.kind == TokenKind::SemicolonToken || parser->currentToken.kind == TokenKind::ColonToken;
    }

    bool IsEndOfArgumentList(Parser* parser) {
        return parser->currentToken.kind == TokenKind::CloseParenToken || parser->currentToken.kind == TokenKind::CloseBracketToken;
    }

    bool IsPossibleSwitchSection(Parser* parser) {
        return parser->currentToken.kind == TokenKind::CaseKeyword || (parser->currentToken.kind == TokenKind::DefaultKeyword && parser->PeekToken(1).kind != TokenKind::OpenParenToken);
    }

    bool IsEndOfTypeParameterList(Parser* parser) {
        TokenKind tk = parser->currentToken.kind;
        if (tk == TokenKind::OpenParenToken) {
            // void Goo<T (
            return true;
        }

        if (tk == TokenKind::ColonToken) {
            // class C<T :
            return true;
        }

        if (tk == TokenKind::OpenBraceToken) {
            // class C<T {
            return true;
        }

        if (IsCurrentTokenWhereOfConstraintClause(parser)) {
            // class C<T where T :
            return true;
        }

        return false;
    }

    bool IsEndOfMethodSignature(Parser* parser) {
        return parser->currentToken.kind == TokenKind::SemicolonToken || parser->currentToken.kind == TokenKind::OpenBraceToken;
    }

    bool IsEndOfNameInExplicitInterface(Parser* parser) {
        return parser->currentToken.kind == TokenKind::DotToken || parser->currentToken.kind == TokenKind::ColonColonToken;
    }

    bool IsEndOfClassOrStructOrInterfaceSignature(Parser* parser) {
        return parser->currentToken.kind == TokenKind::SemicolonToken || parser->currentToken.kind == TokenKind::OpenBraceToken;
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

        int32 num = (int32) parser->termState;

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
                    if (IsEndOfFixedStatement(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfTryBlock: {
                    if (IsEndOfTryBlock(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfCatchClause: {
                    if (IsEndOfCatchClause(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfFilterClause: {
                    if (IsEndOfFilterClause(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfCatchBlock: {
                    if (IsEndOfCatchBlock(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfDoWhileExpression: {
                    if (IsEndOfDoWhileExpression(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfForStatementArgument: {
                    if (IsEndOfForStatementArgument(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfDeclarationClause: {
                    if (IsEndOfDeclarationClause(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfArgumentList: {
                    if (IsEndOfArgumentList(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsSwitchSectionStart: {
                    if (IsPossibleSwitchSection(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfTypeParameterList: {
                    if (IsEndOfTypeParameterList(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfMethodSignature: {
                    if (IsEndOfMethodSignature(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfNameInExplicitInterface: {
                    if (IsEndOfNameInExplicitInterface(parser)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfClassOrStructOrInterfaceSignature: {
                    if (IsEndOfClassOrStructOrInterfaceSignature(parser)) {
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

        TerminatorState saveTerm = parser->termState;
        parser->termState |= TerminatorState::IsNamespaceMemberStartOrStop;
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

    NameSyntax* ParseQualifiedName(Parser* parser, NameOptions options) {
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
                UNREACHABLE("ParseTypeCore");
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
                            type = parser->CreateNode<NullableTypeSyntax>(type, question);
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

            SyntaxToken refKeyword = parser->EatToken();
            SyntaxToken readonlyKeyword = parser->currentToken.kind == TokenKind::ReadOnlyKeyword ? parser->EatToken() : SyntaxToken();
            TypeSyntax* type = ParseTypeCore(parser, ParseTypeMode::AfterRef);
            return parser->CreateNode<RefTypeSyntax>(
                parser->EatToken(),
                readonlyKeyword,
                type
            );

        }

        return ParseTypeCore(parser, mode);
    }

}