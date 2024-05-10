#include "../Allocation/LinearAllocator.h"
#include "./Scanning.h"
#include "./Parser.h"
#include "./Parsing.h"
#include "./SyntaxFacts.h"

namespace Alchemy::Compilation {

    template<typename T>
    struct NodeListBuilder {
        TempAllocator* allocator;
        T** array;
        int32 capacity;
        int32 size;

        explicit NodeListBuilder(TempAllocator* allocator)
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
    };

    bool IsPossibleType(Parser* parser) {
        SyntaxKind tk = parser->currentToken.kind;
        return SyntaxFacts::IsPredefinedType(tk) || IsTrueIdentifier(parser);
    }

    ScanTypeFlags ScanType(Parser* parser, SyntaxToken* lastTokenOfType, bool forPattern) {
        return ScanType(parser, forPattern ? ParseTypeMode::DefinitePattern : ParseTypeMode::Normal, lastTokenOfType);
    }

    bool IsEndOfNamespace(Parser* parser) {
        return parser->currentToken.kind == SyntaxKind::CloseBraceToken;
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

    bool CanStartMember(SyntaxKind kind) {
        switch (kind) {
            case SyntaxKind::BoolKeyword:
            case SyntaxKind::ByteKeyword:
            case SyntaxKind::FloatKeyword:
            case SyntaxKind::IntKeyword:
            case SyntaxKind::LongKeyword:
            case SyntaxKind::SByteKeyword:
            case SyntaxKind::ShortKeyword:
            case SyntaxKind::UIntKeyword:
            case SyntaxKind::ULongKeyword:
            case SyntaxKind::UShortKeyword:
            case SyntaxKind::CharKeyword:

            case SyntaxKind::AbstractKeyword:
            case SyntaxKind::ClassKeyword:
            case SyntaxKind::ConstKeyword:
            case SyntaxKind::DelegateKeyword:
            case SyntaxKind::DoubleKeyword:
            case SyntaxKind::EnumKeyword:
            case SyntaxKind::ExternKeyword:
            case SyntaxKind::FixedKeyword:
            case SyntaxKind::InterfaceKeyword:
            case SyntaxKind::InternalKeyword:
//            case SyntaxKind::NewKeyword: // not sure that we want this as a name override
            case SyntaxKind::ObjectKeyword:
            case SyntaxKind::OverrideKeyword:
            case SyntaxKind::PrivateKeyword:
            case SyntaxKind::ProtectedKeyword:
            case SyntaxKind::PublicKeyword:
            case SyntaxKind::ReadOnlyKeyword:
            case SyntaxKind::SealedKeyword:
            case SyntaxKind::StaticKeyword:
            case SyntaxKind::StringKeyword:
            case SyntaxKind::StructKeyword:
            case SyntaxKind::VirtualKeyword:
            case SyntaxKind::VoidKeyword:
            case SyntaxKind::IdentifierToken:
//            case SyntaxKind::TildeToken:     I think we don't support destructors
            case SyntaxKind::OpenBracketToken:
            case SyntaxKind::ImplicitKeyword: // maybe?
            case SyntaxKind::ExplicitKeyword: // maybe?
//            case SyntaxKind::OpenParenToken:    //tuple -- if we end up supporting that
            case SyntaxKind::RefKeyword:
                return true;

            default:
                return false;
        }
    }

    bool IsPossibleMemberStart(Parser* parser) {
        return CanStartMember(parser->currentToken.kind);
    }

    bool IsPossibleMemberStartOrStop(Parser* parser) {
        return IsPossibleMemberStart(parser) || parser->currentToken.kind == SyntaxKind::CloseBraceToken;
    }

    bool IsEndOfReturnType(Parser* parser) {
        switch (parser->currentToken.kind) {
            case SyntaxKind::OpenParenToken:
            case SyntaxKind::OpenBraceToken:
            case SyntaxKind::SemicolonToken:
                return true;
            default:
                return false;
        }
    }

    bool IsEndOfParameterList(Parser* parser) {
        SyntaxKind kind = parser->currentToken.kind;
        return kind == SyntaxKind::CloseParenToken || kind == SyntaxKind::CloseBracketToken || kind == SyntaxKind::SemicolonToken;
    }

    bool IsEndOfFieldDeclaration(Parser* parser) {
        return parser->currentToken.kind == SyntaxKind::SemicolonToken;
    }

    bool IsPossibleEndOfVariableDeclaration(Parser* parser) {
        switch (parser->currentToken.kind) {
            case SyntaxKind::CommaToken:
            case SyntaxKind::SemicolonToken:
                return true;
            default:
                return false;
        }
    }

    bool IsEndOfTypeArgumentList(Parser* parser) {
        return parser->currentToken.kind == SyntaxKind::GreaterThanToken;
    }

    bool IsPartialMember(Parser* parser) {
        // partials must be void return type
        return parser->currentToken.contextualKind == SyntaxKind::PartialKeyword && parser->PeekToken(1).kind == SyntaxKind::VoidKeyword;
    }

    bool IsCurrentTokenPartialKeywordOfPartialMethodOrType(Parser* parser) {
        if (parser->currentToken.contextualKind == SyntaxKind::PartialKeyword) {
            if (IsPartialType(parser) || IsPartialMember(parser)) {
                return true;
            }
        }

        return false;
    }

    bool IsCurrentTokenWhereOfConstraintClause(Parser* parser) {
        return
            parser->currentToken.contextualKind == SyntaxKind::WhereKeyword &&
            parser->PeekToken(1).kind == SyntaxKind::IdentifierToken &&
            parser->PeekToken(2).kind == SyntaxKind::ColonToken;
    }

    bool IsTrueIdentifier(Parser* parser) {
        if (parser->currentToken.kind == SyntaxKind::IdentifierToken
            && !IsCurrentTokenPartialKeywordOfPartialMethodOrType(parser)
            && !IsCurrentTokenWhereOfConstraintClause(parser)) {
            return true;
        }

        return false;
    }

    bool IsPossibleAnonymousMethodExpression(Parser* parser) {
        // Skip past any static/async keywords.
        int32 tokenIndex = 0;
        while (parser->PeekToken(tokenIndex).kind == SyntaxKind::StaticKeyword) {
            tokenIndex++;
        }

        return parser->PeekToken(tokenIndex).kind == SyntaxKind::DelegateKeyword;
    }

    ScanTypeFlags ScanPossibleTypeArgumentList(Parser* parser, SyntaxToken* greaterThanToken, bool* pIsDefinitelyTypeArgumentList) {
        *pIsDefinitelyTypeArgumentList = false;
        assert(parser->currentToken.kind == SyntaxKind::LessThanToken);

        ScanTypeFlags result = ScanTypeFlags::GenericTypeOrExpression;
        ScanTypeFlags lastScannedType;

        do {
            parser->EatToken();

            // Type arguments cannot contain attributes, so if this is an open square, we early out and assume it is not a type argument
            if (parser->currentToken.kind == SyntaxKind::OpenBracketToken) {
                return ScanTypeFlags::NotType;
            }

            if (parser->currentToken.kind == SyntaxKind::GreaterThanToken) {
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
                    *pIsDefinitelyTypeArgumentList = *pIsDefinitelyTypeArgumentList || parser->currentToken.kind == SyntaxKind::CommaToken || parser->currentToken.kind == SyntaxKind::GreaterThanToken;
                    result = ScanTypeFlags::GenericTypeOrMethod;
                    break;

                case ScanTypeFlags::NullableType:
                    // See above.  If we have `X<Y?,` or `X<Y?>` then this is definitely a type argument list.
                    *pIsDefinitelyTypeArgumentList = *pIsDefinitelyTypeArgumentList || parser->currentToken.kind == SyntaxKind::CommaToken || parser->currentToken.kind == SyntaxKind::GreaterThanToken;
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
                        *pIsDefinitelyTypeArgumentList = parser->currentToken.kind == SyntaxKind::CommaToken;
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
        } while (parser->currentToken.kind == SyntaxKind::CommaToken);

        if (parser->currentToken.kind != SyntaxKind::GreaterThanToken) {
            // Error recovery after missing > token:

            // In the case of an identifier, we assume that there could be a missing > token
            // For example, we have reached C in X<A, B C
            if (parser->currentToken.kind == SyntaxKind::IdentifierToken) {
                *greaterThanToken = parser->EatToken(SyntaxKind::GreaterThanToken);
                return result;
            }

            // As for tuples, we do not expect direct invocation right after the parenthesis
            // EXAMPLE: X<(string, string)(), where we imply a missing > token between )(
            // as the user probably wants to invoke X by X<(string, string)>()
            if (lastScannedType == ScanTypeFlags::TupleType && parser->currentToken.kind == SyntaxKind::OpenParenToken) {
                *greaterThanToken = parser->EatToken(SyntaxKind::GreaterThanToken);
                return result;
            }

            return ScanTypeFlags::NotType;
        }

        *greaterThanToken = parser->EatToken();

        // If we have `X<Y>)` then this would definitely be a type argument list.
        *pIsDefinitelyTypeArgumentList = *pIsDefinitelyTypeArgumentList || parser->currentToken.kind == SyntaxKind::CloseParenToken;
        if (*pIsDefinitelyTypeArgumentList) {
            result = ScanTypeFlags::GenericTypeOrMethod;
        }

        return result;
    }

    ScanTypeFlags ScanNamedTypePart(Parser* parser, SyntaxToken* lastTokenOfType) {

        if (parser->currentToken.kind != SyntaxKind::IdentifierToken || !IsTrueIdentifier(parser)) {
            return ScanTypeFlags::NotType;
        }

        *lastTokenOfType = parser->EatToken();
        if (parser->currentToken.kind == SyntaxKind::LessThanToken) {
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

            if (parser->currentToken.kind == SyntaxKind::CommaToken) {
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
                } while (parser->currentToken.kind == SyntaxKind::CommaToken);

                if (parser->currentToken.kind == SyntaxKind::CloseParenToken) {
                    *lastTokenOfType = parser->EatToken();
                    return ScanTypeFlags::TupleType;
                }
            }
        }

        // Can't be a type!
        return ScanTypeFlags::NotType;
    }

    bool IsMakingProgress(Parser* parser, int* lastTokenPosition, bool assertIfFalse = true) {
        int32 pos = parser->ptr;

        if (pos > *lastTokenPosition) {
            *lastTokenPosition = pos;
            return true;
        }

        assert(!assertIfFalse);
        return false;
    }

    ScanTypeFlags ScanType(Parser* parser, ParseTypeMode mode, SyntaxToken* lastTokenOfType) {
        assert(mode != ParseTypeMode::NewExpression);
        ScanTypeFlags result;

        if (parser->currentToken.kind == SyntaxKind::RefKeyword) {
            // todo -- extend this for 'temp', 'scoped' etc
            // in a ref local or ref return, we treat "ref" and "ref readonly" as part of the type
            parser->EatToken();

            if (parser->currentToken.kind == SyntaxKind::ReadOnlyKeyword) {
                parser->EatToken();
            }

        }

        if (parser->currentToken.kind == SyntaxKind::IdentifierToken) {

            assert(parser->currentToken.kind == SyntaxKind::IdentifierToken);

            result = ScanNamedTypePart(parser, lastTokenOfType);
            if (result == ScanTypeFlags::NotType) {
                return ScanTypeFlags::NotType;
            }

            assert(result == ScanTypeFlags::GenericTypeOrExpression || result == ScanTypeFlags::GenericTypeOrMethod || result == ScanTypeFlags::NonGenericTypeOrExpression);

            // Scan a name
            for (bool firstLoop = true; parser->currentToken.kind == SyntaxKind::DotToken || parser->currentToken.kind == SyntaxKind::DotDotToken; firstLoop = false) {

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
        else if (parser->currentToken.kind == SyntaxKind::OpenParenToken) {
            *lastTokenOfType = parser->EatToken();

            result = ScanTupleType(parser, lastTokenOfType);
            if (result == ScanTypeFlags::NotType || mode == ParseTypeMode::DefinitePattern && parser->currentToken.kind != SyntaxKind::OpenBracketToken) {
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

            if (parser->currentToken.kind == SyntaxKind::QuestionToken) {

                if (lastTokenOfType->kind == SyntaxKind::QuestionToken && lastTokenOfType->kind != SyntaxKind::AsteriskToken) {
                    // don't allow `Type??`
                    // don't allow `Type*?`
                    *lastTokenOfType = parser->EatToken();
                    result = ScanTypeFlags::NullableType;
                    continue;
                }

            }

            switch (parser->currentToken.kind) {
                case SyntaxKind::OpenBracketToken:
                    // Check for array types.
                    parser->EatToken();
                    while (parser->currentToken.kind == SyntaxKind::CommaToken) {
                        parser->EatToken();
                    }

                    if (parser->currentToken.kind != SyntaxKind::CloseBracketToken) {
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

    // For better error recovery 'static =>' is also considered a possible lambda expression.
    // pass by copy to avoid mutation since we advance token stream a bit
    bool IsPossibleLambdaExpression(Parser parser, Precedence precedence) {
        if (precedence > Precedence::Lambda) {
            return false;
        }

        SyntaxToken token1 = parser.PeekToken(1);

        // x =>
        if (token1.kind == SyntaxKind::EqualsGreaterThanToken) {
            return true;
        }

        bool seenStatic = false;
        if (parser.currentToken.kind == SyntaxKind::StaticKeyword) {
            parser.EatToken();
            seenStatic = true;
        }

        if (seenStatic) {
            if (parser.currentToken.kind == SyntaxKind::EqualsGreaterThanToken) {
                // `static =>`
                // This is an error case, but we have enough code in front of us to be certain
                // the user was trying to write a static lambda.
                return true;
            }

            if (parser.currentToken.kind == SyntaxKind::OpenParenToken) {
                // static (...
                return true;
            }
        }

        if (parser.currentToken.kind == SyntaxKind::IdentifierToken && parser.PeekToken(1).kind == SyntaxKind::EqualsGreaterThanToken) {
            // 1. `a => ...`
            // 2. `static a => ...`
            return true;
        }

        // Have checked all the static forms.  And have checked for the basic `a => a` form.
        // At this point we have must be on an explicit return type for this to still be a lambda.
        //        if (Parser.currentToken.contextualKind == SyntaxKind::AsyncKeyword &&            IsAnonymousFunctionAsyncModifier()) {
        //            EatToken();
        //        }

        struct Parser cpy = parser;
        SyntaxToken k = {0};
        ScanTypeFlags st = ScanType(&cpy, &k, false);
        if (st == ScanTypeFlags::NotType || parser.currentToken.kind != SyntaxKind::OpenParenToken) {
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
        if (parser.currentToken.kind == SyntaxKind::IdentifierToken && parser.PeekToken(1).kind == SyntaxKind::EqualsGreaterThanToken) {
            //  a => ...
            return true;
        }

        // Non-simple async lambda must be of the form 'async (...'
        if (parser.currentToken.kind != SyntaxKind::OpenParenToken) {
            return false;
        }

        // Check whether looks like implicitly or explicitly typed lambda
        return ScanParenthesizedLambda(&parser, precedence);
    }

    bool IsPossibleExpression(Parser* parser, bool allowBinaryExpressions, bool allowAssignmentExpressions) {
        SyntaxKind tk = parser->currentToken.kind;
        switch (tk) {
            case SyntaxKind::TypeOfKeyword:
            case SyntaxKind::DefaultKeyword:
            case SyntaxKind::SizeOfKeyword:
            case SyntaxKind::BaseKeyword:
            case SyntaxKind::FalseKeyword:
            case SyntaxKind::ThisKeyword:
            case SyntaxKind::TrueKeyword:
            case SyntaxKind::NullKeyword:
            case SyntaxKind::OpenParenToken:
            case SyntaxKind::NumericLiteralToken:
            case SyntaxKind::StringLiteralToken:
                // todo -- strings
//            case SyntaxKind::SingleLineRawStringLiteralToken:
//            case SyntaxKind::MultiLineRawStringLiteralToken:
//            case SyntaxKind::InterpolatedStringToken:
//            case SyntaxKind::InterpolatedStringStartToken:
//            case SyntaxKind::InterpolatedVerbatimStringStartToken:
//            case SyntaxKind::InterpolatedSingleLineRawStringStartToken:
//            case SyntaxKind::InterpolatedMultiLineRawStringStartToken:
            case SyntaxKind::CharacterLiteralToken:
            case SyntaxKind::NewKeyword:
            case SyntaxKind::DelegateKeyword:
            case SyntaxKind::ThrowKeyword:
//            case SyntaxKind::StackAllocKeyword: probably allow something like this
            case SyntaxKind::DotDotToken:
            case SyntaxKind::RefKeyword:
            case SyntaxKind::OpenBracketToken: // collection expression
                return true;
            case SyntaxKind::StaticKeyword:
                return IsPossibleAnonymousMethodExpression(parser) || IsPossibleLambdaExpression(*parser, Precedence::Expression);
            case SyntaxKind::IdentifierToken:
                return IsTrueIdentifier(parser);
            default:
                return SyntaxFacts::IsPredefinedType(tk)
                       || SyntaxFacts::IsAnyUnaryExpression(tk)
                       || (allowBinaryExpressions && SyntaxFacts::IsBinaryExpression(tk))
                       || (allowAssignmentExpressions && SyntaxFacts::IsAssignmentExpressionOperatorToken(tk));
        }
    }

    bool IsPossibleExpression(Parser* parser) {
        return IsPossibleExpression(parser, true, true);
    }

    bool IsPossibleStatement(Parser* parser, bool acceptAccessibilityMods) {
        SyntaxKind tk = parser->currentToken.kind;
        switch (tk) {
            case SyntaxKind::BreakKeyword:
            case SyntaxKind::ContinueKeyword:
            case SyntaxKind::TryKeyword:
            case SyntaxKind::ConstKeyword:
            case SyntaxKind::DoKeyword:
            case SyntaxKind::ForKeyword:
            case SyntaxKind::ForEachKeyword:
            case SyntaxKind::IfKeyword:
            case SyntaxKind::ElseKeyword:
            case SyntaxKind::ReturnKeyword:
            case SyntaxKind::SwitchKeyword:
            case SyntaxKind::ThrowKeyword:
            case SyntaxKind::UsingKeyword:
            case SyntaxKind::WhileKeyword:
            case SyntaxKind::OpenBraceToken:
            case SyntaxKind::SemicolonToken:
            case SyntaxKind::StaticKeyword:
            case SyntaxKind::ReadOnlyKeyword: // not sure about this one
            case SyntaxKind::RefKeyword:
            case SyntaxKind::ExternKeyword: // not sure this could be right here
            case SyntaxKind::OpenBracketToken:
            case SyntaxKind::CaseKeyword: // for parsing an errant case without a switch.
                return true;

            case SyntaxKind::IdentifierToken:
                return IsTrueIdentifier(parser);

                // Accessibility modifiers are not legal in a statement,
                // but a common mistake for local functions. Parse to give a
                // better error message.
            case SyntaxKind::PublicKeyword:
            case SyntaxKind::InternalKeyword:
            case SyntaxKind::ProtectedKeyword:
            case SyntaxKind::PrivateKeyword:
                return acceptAccessibilityMods;
            default:
                return SyntaxFacts::IsPredefinedType(tk) || IsPossibleExpression(parser);
        }
    }

    bool IsPossibleStatementStartOrStop(Parser* parser) {
        return parser->currentToken.kind == SyntaxKind::SemicolonToken || IsPossibleStatement(parser, true);
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
        assert(parser->currentToken.contextualKind == SyntaxKind::PartialKeyword);
        switch (parser->PeekToken(1).kind) {
            case SyntaxKind::StructKeyword:
            case SyntaxKind::ClassKeyword:
            case SyntaxKind::InterfaceKeyword:
                return true;
            default:
                return false;
        }
    }

    bool IsPartialInNamespaceMemberDeclaration(Parser* parser) {
        if (parser->currentToken.contextualKind == SyntaxKind::PartialKeyword) {
            if (IsPartialType(parser)) {
                return true;
            }
            else if (parser->PeekToken(1).kind == SyntaxKind::NamespaceKeyword) {
                return true;
            }
        }

        return false;
    }

    bool IsTypeModifierOrTypeKeyword(SyntaxKind kind) {
        switch (kind) {
            case SyntaxKind::EnumKeyword:
            case SyntaxKind::DelegateKeyword:
            case SyntaxKind::ClassKeyword:
            case SyntaxKind::InterfaceKeyword:
            case SyntaxKind::StructKeyword:
            case SyntaxKind::AbstractKeyword:
            case SyntaxKind::InternalKeyword:
            case SyntaxKind::PrivateKeyword:
            case SyntaxKind::ProtectedKeyword:
            case SyntaxKind::PublicKeyword:
            case SyntaxKind::SealedKeyword:
            case SyntaxKind::StaticKeyword:
                // case SyntaxKind::NewKeyword: // might remove?
                return true;
            default:
                return false;
        }
    }

    bool IsTypeDeclarationStart(Parser* parser) {
        switch (parser->currentToken.kind) {
            case SyntaxKind::ClassKeyword:
            case SyntaxKind::DelegateKeyword:
            case SyntaxKind::EnumKeyword:
            case SyntaxKind::InterfaceKeyword:
            case SyntaxKind::StructKeyword:
                return true;

            default:
                return false;
        }
    }

    bool IsPossibleStartOfTypeDeclaration(SyntaxKind kind) {
        return IsTypeModifierOrTypeKeyword(kind) || kind == SyntaxKind::OpenBracketToken;
    }

    bool IsPossibleNamespaceMemberDeclaration(Parser* parser) {
        switch (parser->currentToken.kind) {
            case SyntaxKind::ExternKeyword:
            case SyntaxKind::UsingKeyword:
            case SyntaxKind::NamespaceKeyword:
                return true;
            case SyntaxKind::IdentifierToken:
                return IsPartialInNamespaceMemberDeclaration(parser);
            default:
                return IsPossibleStartOfTypeDeclaration(parser->currentToken.kind);
        }
    }

    bool IsTerminator(Parser* parser) {

        if (parser->currentToken.kind == SyntaxKind::EndOfFileToken) {
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

    struct MemberDeclarationSyntax {};

    SyntaxToken ParseIdentifierToken(Parser* parser, ErrorCode errorCode = ErrorCode::ERR_IdentifierExpected) {
        if (parser->currentToken.kind == SyntaxKind::IdentifierToken) {
            return parser->EatToken();
        }
        else {
            SyntaxToken token = parser->currentToken;
            assert(token.id >= 0);
            SyntaxTokenCold cold = parser->GetColdToken(token);
            Diagnostic diagnostic(errorCode, cold.text, cold.text + cold.textSize);
            parser->AddError(token, diagnostic);
            return parser->CreateMissingToken(SyntaxKind::IdentifierToken);
        }
    }

// I think we just want our tree, which contains token starts. (ends we can compute as needed)
// tree isn't tied to what's in the file since we can have missing nodes
// skipped nodes are flagged as such and I guess either added as trivia directly or just directly skipped over

// we need lists that' well build, list pool makes sense, can probably just use an arena but can't be the temp one right?
// visiting the tree from an ast / cst sense is different than visiting from a compiler standpoint
// we want a way to essentially re-construct the file in order right? that's psi
// what the compiler wants is different, we don't care about / actually don't want a cst
// but I imagine the lsp desparately needs it.


// ast nodes always start w/ valid tokens (or it wouldn't make any sense to parse them as such a construct)
// they may not end on a valid token though (could be missing / skipped)
// i guess we can treat their token range as the valid start and either parent's end or next sibling's start token
// except parent could end on an invalid one right?
// i guess then it's parent's next sibling start or it's parent's next sibling start etc until eof
// i suppose we then look into trivia to figure out start end bounds
// trivia is associated w/ tokens right now
// when we skip a token we likely add it the trivia list
// maybe its easist to do with a flag so we don't move the actual token
// but searching for trivia entails checking previous tokens for a 'became-trivia' flag
// we only ever skip tokens, not psi nodes

// how can we check if cursor is over a missing token? like .missing. ? flag the token maybe

// missing nodes are added to our tree but how are they really represented?
// (missing) (token type) I guess. and the non missing node gets marked as trivia, so the missing node's token id is set to the real token but marked as trivia


    void ParseNamespaceBody(Parser* parser, SyntaxToken* openBraceOrSemicolon) {
        bool isGlobal = openBraceOrSemicolon == nullptr;

        TerminatorState saveTerm = parser->_termState;
        parser->_termState |= TerminatorState::IsNamespaceMemberStartOrStop;
        NamespaceParts seen = NamespaceParts::None;

        NodeListBuilder<MemberDeclarationSyntax> pendingIncompleteMembers(parser->tempAllocator);

        bool reportUnexpectedToken = true;

        while (true) {
            switch (parser->currentToken.kind) {
                case SyntaxKind::NamespaceKeyword: {

                    break;
                }
                default: {
                    break;
                }
            }

        }

    }

    struct ResetPoint {

        bool resetOnDispose;
        size_t allocatorOffset;
        size_t tempAllocatorOffset;
        Parser* originalParser;
        Parser copyParser;
        Diagnostics diagnosticsCopy;
        SyntaxTokenFlags* flags;

        explicit ResetPoint(Parser* parser, bool resetOnDispose = true)
            : originalParser(parser)
            , copyParser(*parser)
            , allocatorOffset(parser->allocator->offset)
            , tempAllocatorOffset(parser->tempAllocator->offset)
            , diagnosticsCopy(*parser->diagnostics)
            , resetOnDispose(resetOnDispose) {
            flags = parser->tempAllocator->AllocateUncleared<SyntaxTokenFlags>(parser->hotTokens.size - parser->ptr);
            int32 flagIdx = 0;
            for (int32 i = parser->ptr; i < parser->hotTokens.size; i++) {
                flags[flagIdx++] = parser->hotTokens.array[i].flags;
            }
        }

        ~ResetPoint() {
            if (resetOnDispose) {
                Reset();
            }
        }

        void Reset() {

            int32 flagIdx = 0;
            for (int32 i = copyParser.ptr; i < copyParser.hotTokens.size; i++) {
                originalParser->hotTokens.array[i].flags = flags[flagIdx++];
            }

            *originalParser = copyParser;
            *originalParser->diagnostics = diagnosticsCopy;
            originalParser->tempAllocator->offset = tempAllocatorOffset;
            originalParser->allocator->offset = allocatorOffset;
        }

    };

    IdentifierNameSyntax* ParseIdentifierName(Parser* parser, ErrorCode code = ErrorCode::ERR_IdentifierExpected) {
        return parser->CreateNode<IdentifierNameSyntax>(parser->EatToken(SyntaxKind::IdentifierToken));
    }

    ScanTypeArgumentListKind ScanTypeArgumentList(Parser* parser, NameOptions options) {
        if (parser->currentToken.kind != SyntaxKind::LessThanToken) {
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

        assert(greaterThanToken.kind == SyntaxKind::GreaterThanToken);
        switch (parser->currentToken.kind) {
            case SyntaxKind::OpenParenToken:
            case SyntaxKind::CloseParenToken:
            case SyntaxKind::CloseBracketToken:
            case SyntaxKind::CloseBraceToken:
            case SyntaxKind::ColonToken:
            case SyntaxKind::SemicolonToken:
            case SyntaxKind::CommaToken:
            case SyntaxKind::DotToken:
            case SyntaxKind::QuestionToken:
            case SyntaxKind::EqualsEqualsToken:
            case SyntaxKind::ExclamationEqualsToken:
            case SyntaxKind::BarToken:
            case SyntaxKind::CaretToken:
                // These tokens are from 7.5.4.2 Grammar Ambiguities
                return ScanTypeArgumentListKind::DefiniteTypeArgumentList;

            case SyntaxKind::AmpersandAmpersandToken: // e.g. `e is A<B> && e`
            case SyntaxKind::BarBarToken:             // e.g. `e is A<B> || e`
            case SyntaxKind::AmpersandToken:          // e.g. `e is A<B> & e`
            case SyntaxKind::OpenBracketToken:        // e.g. `e is A<B>[]`
            case SyntaxKind::LessThanToken:           // e.g. `e is A<B> < C`
            case SyntaxKind::LessThanEqualsToken:     // e.g. `e is A<B> <= C`
            case SyntaxKind::GreaterThanEqualsToken:  // e.g. `e is A<B> >= C`
            case SyntaxKind::IsKeyword:               // e.g. `e is A<B> is bool`
            case SyntaxKind::AsKeyword:               // e.g. `e is A<B> as bool`
                // These tokens were added to 7.5.4.2 Grammar Ambiguities in C# 7.0
                return ScanTypeArgumentListKind::DefiniteTypeArgumentList;

            case SyntaxKind::OpenBraceToken: // e.g. `e is A<B> {}`
                // This token was added to 7.5.4.2 Grammar Ambiguities in C# 8.0
                return ScanTypeArgumentListKind::DefiniteTypeArgumentList;

            case SyntaxKind::GreaterThanToken: {
                // This token is added to 7.5.4.2 Grammar Ambiguities in C#7 for the special case in which
                // the possible generic is following an `is` keyword, e.g. `e is A<B> > C`.
                // We test one further token ahead because a right-shift operator `>>` looks like a pair of greater-than
                // tokens at this stage, but we don't intend to be handling the right-shift operator.
                // The upshot is that we retain compatibility with the two previous behaviors:
                // `(x is A<B>>C)` is parsed as `(x is A<B>) > C`
                // `A<B>>C` elsewhere is parsed as `A < (B >> C)`
                if (((options & NameOptions::AfterIs) != 0) && parser->PeekToken(1).kind != SyntaxKind::GreaterThanToken) {
                    return ScanTypeArgumentListKind::DefiniteTypeArgumentList;
                }

                return ScanTypeArgumentListKind::PossibleTypeArgumentList;
            }

            case SyntaxKind::IdentifierToken:
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
                    (options & NameOptions::AfterTupleComma) != 0 && parser->PeekToken(1).kind == SyntaxKind::CommaToken || parser->PeekToken(1).kind == SyntaxKind::CloseParenToken ||
                    (options & NameOptions::FirstElementOfPossibleTupleLiteral) != 0 && parser->PeekToken(1).kind == SyntaxKind::CommaToken) {
                    // we allow 'G<T,U> x' as a pattern-matching operation and a declaration expression in a tuple.
                    return ScanTypeArgumentListKind::DefiniteTypeArgumentList;
                }

                return ScanTypeArgumentListKind::PossibleTypeArgumentList;

            case SyntaxKind::EndOfFileToken:          // e.g. `e is A<B>`
                // This is useful for parsing expressions in isolation
                return ScanTypeArgumentListKind::DefiniteTypeArgumentList;

            case SyntaxKind::EqualsGreaterThanToken:  // e.g. `e switch { A<B> => 1 }`
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
            case SyntaxKind::OrKeyword:
                // Example: x is IEnumerable<string and IDisposable
            case SyntaxKind::AndKeyword:
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
            case SyntaxKind::OpenParenToken:
                // Example: IEnumerable<string Method<T>() --- (< in <T>)
            case SyntaxKind::LessThanToken:
                // Example: Method(IEnumerable<string parameter)
            case SyntaxKind::CloseParenToken:
                // Example: IEnumerable<string field;
            case SyntaxKind::SemicolonToken:
                // Example: IEnumerable<string Property { get; set; }
            case SyntaxKind::OpenBraceToken:
                // Example:
                // {
                //     IEnumerable<string field
                // }
            case SyntaxKind::CloseBraceToken:
                // Examples:
                // - IEnumerable<string field = null;
                // - Method(IEnumerable<string parameter = null)
            case SyntaxKind::EqualsToken:
                // Example: IEnumerable<string Property => null;
            case SyntaxKind::EqualsGreaterThanToken:
                // Example: IEnumerable<string this[string key] { get; set; }
            case SyntaxKind::ThisKeyword:
                // Example: static IEnumerable<string operator +(A left, A right);
            case SyntaxKind::OperatorKeyword:
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

        if (result->IsMissing() && current.kind != SyntaxKind::CommaToken && current.kind != SyntaxKind::GreaterThanToken && next.kind == SyntaxKind::CommaToken || next.kind == SyntaxKind::GreaterThanToken) {
            // skip the current token so we can recover
            parser->SkipToken();
        }

        return result;
    }

    bool typeArgumentListNotExpectedFn(Parser* parser) {
        return parser->currentToken.kind != SyntaxKind::CommaToken && !IsPossibleType(parser);
    }

    bool typeArgumentListAbortFn(Parser* parser, SyntaxKind kind) {
        return parser->currentToken.kind == SyntaxKind::GreaterThanToken;
    }

    PostSkipAction SkipBadTypeArgumentListTokens(Parser* parser, SyntaxKind kind) {
        return SkipBadSeparatedListTokensWithExpectedKind(parser, &typeArgumentListNotExpectedFn, &typeArgumentListAbortFn, kind);
    }

    void ParseTypeArgumentList(Parser* parser, SyntaxToken* open, SeparatedNodeListBuilder<TypeSyntax>* builder, SyntaxToken* close) {
        assert(parser->currentToken.kind == SyntaxKind::LessThanToken);
        *open = parser->EatToken();

        builder->Add(ParseTypeArgument(parser));
        // remaining types & commas
        while (true) {
            if (parser->currentToken.kind == SyntaxKind::GreaterThanToken) {
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
            if (parser->currentToken.kind == SyntaxKind::IdentifierToken && TokenBreaksTypeArgumentList(parser->PeekToken(1))) {
                break;
            }

            // This is for the case where we are in a this[] accessor, and the last one of the parameters in the parameter list
            // is missing a > on its type
            // Example: X this[IEnumerable<string parameter] =>
            //                 current token:     ^^^^^^^^^
            if (parser->currentToken.kind == SyntaxKind::IdentifierToken && parser->PeekToken(1).kind == SyntaxKind::CloseBracketToken) {
                break;
            }

            if (parser->currentToken.kind == SyntaxKind::CommaToken || IsPossibleType(parser)) {
                builder->AddSeparator(parser->EatToken(SyntaxKind::CommaToken));
                builder->Add(ParseTypeArgument(parser));
            }
            else if (SkipBadTypeArgumentListTokens(parser, SyntaxKind::CommaToken) == PostSkipAction::Abort) {
                break;
            }

        }

        *close = parser->EatToken(SyntaxKind::GreaterThanToken);

    }

    ExpressionSyntax* ParseExpressionCore(Parser* parser) {
        return nullptr;
    }

    SimpleNameSyntax* ParseSimpleName(Parser* parser, NameOptions options) {
        IdentifierNameSyntax* id = ParseIdentifierName(parser);

        if (id->identifier.IsMissing()) {
            return id;
        }

        SimpleNameSyntax* name = id;
        ScanTypeArgumentListKind kind;
        if (parser->currentToken.kind == SyntaxKind::LessThanToken) {
            {
                ResetPoint resetPoint(parser);
                kind = ScanTypeArgumentList(parser, options);
            }

            if (kind == ScanTypeArgumentListKind::DefiniteTypeArgumentList || (kind == ScanTypeArgumentListKind::PossibleTypeArgumentList && (options & NameOptions::InTypeList) != 0)) {
                assert(parser->currentToken.kind == SyntaxKind::LessThanToken);
                SeparatedNodeListBuilder<TypeSyntax> builder(parser->tempAllocator);
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
        assert(separator->kind == SyntaxKind::DotDotToken);

        IdentifierNameSyntax* missingName = CreateMissingIdentifierName(parser);
        SyntaxToken leftDot = MakeMissingToken(SyntaxKind::DotToken, parser->ptr);
        parser->AddError(*separator, Diagnostic(ErrorCode::ERR_IdentifierExpected, parser->GetTokenText(*separator)));
        *separator = MakeMissingToken(SyntaxKind::DotToken, parser->ptr);
        return parser->CreateNode<QualifiedNameSyntax>(left, leftDot, missingName);
    }

    NameSyntax* ParseQualifiedNameRight(Parser* parser, NameOptions options, NameSyntax* left, SyntaxToken separator) {
        // assert(separator.Kind is SyntaxKind.DotToken or SyntaxKind.DotDotToken or SyntaxKind.ColonColonToken);
        SimpleNameSyntax* right = ParseSimpleName(parser, options);

        switch (separator.kind) {
            case SyntaxKind::DotToken:
                return parser->CreateNode<QualifiedNameSyntax>(left, separator, right);

            case SyntaxKind::DotDotToken:
                // Error recovery.  If we have `X..Y` break that into `X.<missing-id>.Y`
                return parser->CreateNode<QualifiedNameSyntax>(RecoverFromDotDot(parser, left, &separator), separator, right);

                // removing :: support for now, doesn't seem super helpful and is maybe just confusing

//            case SyntaxKind::ColonColonToken:
//
//                if (left->kind != SyntaxKind::IdentifierName) {
//                    separator = this.AddError(separator, ErrorCode::ERR_UnexpectedAliasedName);
//                }
//
//                // If the left hand side is not an identifier name then the user has done
//                // something like Goo.Bar::Blah. We've already made an error node for the
//                // ::, so just pretend that they typed Goo.Bar.Blah and continue on.
//
//                if (left->kind != SyntaxKind::IdentifierName) {
//                    separator = this.ConvertToMissingWithTrailingTrivia(separator, SyntaxKind::DotToken);
//                    return _syntaxFactory.QualifiedName(left, separator, right);
//                }
//                else {
//
////                    if (identifierLeft.Identifier.contextualKind == SyntaxKind::GlobalKeyword) {
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
        while (parser->currentToken.kind == SyntaxKind::DotToken || parser->currentToken.kind == SyntaxKind::DotDotToken) {

            if (parser->PeekToken(1).kind == SyntaxKind::ThisKeyword) {
                break;
            }

            SyntaxToken separator = parser->EatToken();
            name = ParseQualifiedNameRight(parser, options, name, separator);
        }

        return name;

    }

    SyntaxToken MakeMissingToken(SyntaxKind kind, int32 id) {
        assert(SyntaxFacts::IsToken(kind));
        SyntaxToken retn;
        retn.kind = kind;
        retn.id = id;
        retn.flags |= SyntaxTokenFlags::Missing;
        return retn;
    }

    IdentifierNameSyntax* CreateMissingIdentifierName(Parser* parser) {
        return parser->CreateNode<IdentifierNameSyntax>(MakeMissingToken(SyntaxKind::IdentifierToken, parser->ptr));
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
        SyntaxToken open = parser->EatToken(SyntaxKind::OpenParenToken);
        TempAllocator::ScopedMarker m(parser->tempAllocator);
        SeparatedNodeListBuilder<TupleElementSyntax> list(parser->tempAllocator);

        if (parser->currentToken.kind != SyntaxKind::CloseParenToken) {
            list.Add(ParseTupleElement(parser));

            while (parser->currentToken.kind == SyntaxKind::CommaToken) {
                list.AddSeparator(parser->EatToken(SyntaxKind::CommaToken));
                list.Add(ParseTupleElement(parser));
            }
        }

        // todo -- not sure about these two checks, validate
        if (list.itemCount < 2) {

            if (list.itemCount == 0) {
                list.Add(parser->CreateNode<TupleElementSyntax>(CreateMissingIdentifierName(parser), SyntaxToken()));
            }

            IdentifierNameSyntax* missing = CreateMissingIdentifierName(parser);

            list.AddSeparator(MakeMissingToken(SyntaxKind::CommaToken, parser->ptr));
            list.Add(parser->CreateNode<TupleElementSyntax>(missing, SyntaxToken()));

            FixedCharSpan span = parser->GetTokenText(parser->currentToken);
            parser->diagnostics->AddError(Diagnostic(ErrorCode::ERR_TupleTooFewElements, span));
        }

        return parser->CreateNode<TupleTypeSyntax>(open, list.ToList(parser->allocator), parser->EatToken(SyntaxKind::CloseParenToken));

    }

    TypeSyntax* ParseUnderlyingType(Parser* parser, ParseTypeMode mode, NameOptions options = NameOptions::None) {
        if (SyntaxFacts::IsPredefinedType(parser->currentToken.kind)) {
            SyntaxToken token = parser->EatToken();
            if (token.kind == SyntaxKind::VoidKeyword) {
                ErrorCode code = mode == ParseTypeMode::Parameter ? ErrorCode::ERR_NoVoidParameter : ErrorCode::ERR_NoVoidHere;
                parser->AddError(token, parser->MakeDiagnostic(code, token));
            }

            return parser->CreateNode<PredefinedTypeSyntax>(token);
        }

        if (IsTrueIdentifier(parser)) {
            return ParseQualifiedName(parser, options);
        }

        if (parser->currentToken.kind == SyntaxKind::OpenParenToken) {
            return ParseTupleType(parser);
        }

        // we don't advance in the error case, is that correct?
        IdentifierNameSyntax* retn = CreateMissingIdentifierName(parser);
        ErrorCode errorCode = mode == ParseTypeMode::NewExpression ? ErrorCode::ERR_BadNewExpr : ErrorCode::ERR_TypeExpected;
        FixedCharSpan span = parser->GetTokenText(parser->currentToken);
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
                return !CanStartExpression(parser) || parser->currentToken.kind == SyntaxKind::OpenBracketToken;
            }
            case ParseTypeMode::NewExpression: {
                // A nullable qualifier is permitted as part of the type in a `new` expression.
                // e.g. `new int?()` is allowed.  It creates a null value of type `Nullable<int>`.
                // Similarly `new int? {}` is allowed.
                SyntaxKind kind = parser->currentToken.kind;

                return
                    kind == SyntaxKind::OpenParenToken ||   // ctor parameters
                    kind == SyntaxKind::OpenBracketToken ||   // array type
                    kind == SyntaxKind::OpenBraceToken;   // object initializer
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
        if (parser->PeekToken(1).kind == SyntaxKind::OpenBracketToken) {
            return true;
        }
        if (mode == ParseTypeMode::DefinitePattern) {
            return true; // Permit nullable type parsing and report while binding for a better error message
        }
        if (mode == ParseTypeMode::NewExpression && typeSyntax->GetKind() == SyntaxKind::TupleType && parser->PeekToken(1).kind != SyntaxKind::OpenParenToken && parser->PeekToken(1).kind != SyntaxKind::OpenBraceToken) {
            return false; // Permit `new (int, int)?(t)` (creation) and `new (int, int) ? x : y` (conditional)
        }

        return true;
    }

    PostSkipAction SkipBadTokensWithExpectedKind(Parser* parser, notExpectedFn isNotExpectedFunction, abortFn abortFunction, SyntaxKind expected, SyntaxKind closeKind) {
        int32 startToken = parser->ptr;
        bool first = true;
        PostSkipAction action = PostSkipAction::Continue;
        // we can re-allocate trivia list and append
        // or we can mark these tokens as skipped
        // or we can
        while (isNotExpectedFunction(parser)) {
            if (abortFunction(parser, closeKind) || IsTerminator(parser)) {
                action = PostSkipAction::Abort;
                break;
            }

            // if this token already contains diagnostics we don't want to add another one
            if (first && !parser->currentToken.ContainsDiagnostics()) {
                parser->EatTokenWithPrejudice(expected);
            }
            else {
                parser->EatToken();
            }

            first = false;

        }

        for (int32 i = startToken; i < parser->ptr; i++) {
            parser->hotTokens[i].flags |= SyntaxTokenFlags::Skipped;
        }

        return action;
    }

    PostSkipAction SkipBadSeparatedListTokensWithExpectedKind(Parser* parser, notExpectedFn isNotExpectedFunction, abortFn abortFunction, SyntaxKind expected, SyntaxKind closeKind) {
        return SkipBadTokensWithExpectedKind(parser, isNotExpectedFunction, abortFunction, expected, closeKind);
    }

    bool arrayRankNotExpectedFn(Parser* parser) {
        return parser->currentToken.kind != SyntaxKind::CommaToken && !IsPossibleExpression(parser);
    }

    bool arrayRankAbortFn(Parser* parser, SyntaxKind kind) {
        return parser->currentToken.kind == SyntaxKind::CloseBracketToken;
    }

    PostSkipAction SkipBadArrayRankSpecifierTokens(Parser* parser, SyntaxKind expected) {
        return SkipBadSeparatedListTokensWithExpectedKind(
            parser,
            &arrayRankNotExpectedFn,
            &arrayRankAbortFn,
            expected
        );
    }

    SyntaxToken MakeOmittedToken(SyntaxKind kind, int32 position) {
        SyntaxToken omittedToken;
        omittedToken.kind = kind;
        omittedToken.contextualKind = SyntaxKind::None;
        omittedToken.id = position;
        omittedToken.flags |= SyntaxTokenFlags::Omitted;
        return omittedToken;
    }

    ArrayRankSpecifierSyntax* ParseArrayRankSpecifier(Parser* parser, bool* pSawNonOmittedSize) {
        *pSawNonOmittedSize = false;
        bool sawOmittedSize = false;
        SyntaxToken open = parser->EatToken(SyntaxKind::OpenBracketToken);
        TempAllocator::ScopedMarker scopedMarker(parser->tempAllocator);
        SeparatedNodeListBuilder<ExpressionSyntax> list(parser->tempAllocator);

        int32 lastTokenPosition = -1;
        while (IsMakingProgress(parser, &lastTokenPosition) && parser->currentToken.kind != SyntaxKind::CloseBracketToken) {

            if (parser->currentToken.kind == SyntaxKind::CommaToken) {
                sawOmittedSize = true;
                list.Add(parser->CreateNode<OmittedArraySizeExpressionSyntax>(MakeOmittedToken(SyntaxKind::OmittedArraySizeExpressionToken, parser->ptr)));
                list.AddSeparator(parser->EatToken());
            }
            else if (IsPossibleExpression(parser)) {
                ExpressionSyntax* size = ParseExpressionCore(parser);
                *pSawNonOmittedSize = true;
                list.Add(size);
                if (parser->currentToken.kind != SyntaxKind::CloseBracketToken) {
                    list.AddSeparator(parser->EatToken(SyntaxKind::CommaToken));
                }
            }
            else if (SkipBadArrayRankSpecifierTokens(parser, SyntaxKind::CommaToken) == PostSkipAction::Abort) {
                break;
            }

        }

        // Don't end on a comma.
        // If the omitted size would be the only element, then skip it unless sizes were expected.
        if (list.separatorCount == list.itemCount + 1) {
            sawOmittedSize = true;
            list.Add(parser->CreateNode<OmittedArraySizeExpressionSyntax>(MakeOmittedToken(SyntaxKind::OmittedArraySizeExpressionToken, parser->ptr)));
        }

        // Never mix omitted and non-omitted array sizes.  If there were non-omitted array sizes,
        // then convert all of the omitted array sizes to missing identifiers.
        if (sawOmittedSize && *pSawNonOmittedSize) {
            for (int32 i = 0; i < list.itemCount; i++) {
                if (list.GetItem(i)->GetKind() == SyntaxKind::OmittedArraySizeExpression) {
                    SyntaxToken separator = list.GetSeparator(i);
                    FixedCharSpan text = parser->GetTokenText(separator); // we need a text range for the error. try to use the next separator. todo -- probably fails on the end
                    parser->AddError(separator, Diagnostic(ErrorCode::ERR_ValueExpected, text.ptr, text.ptr + text.size));
                    list.items[i] = parser->CreateNode<IdentifierNameSyntax>(parser->CreateMissingToken(SyntaxKind::IdentifierToken));
                }
            }
        }

        return parser->CreateNode<ArrayRankSpecifierSyntax>(open, list.ToList(parser->allocator), parser->EatToken(SyntaxKind::CloseBracketToken));

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
                case SyntaxKind::QuestionToken: {
                    if (CanBeNullableType(parser, type, mode)) {
                        SyntaxToken question;
                        if (TryEatNullableQualifierIfApplicable(parser, mode, &question)) {
                            type = parser->CreateNode<NullableType>(type, question);
                            continue;
                        }
                    }
                    goto done; // token not consumed

                }
                case SyntaxKind::OpenBracketToken: {
                    // Now check for arrays.
                    TempAllocator::ScopedMarker marker(parser->tempAllocator);
                    NodeListBuilder<ArrayRankSpecifierSyntax> ranks(parser->tempAllocator);
                    do {
                        bool unused = false;
                        ranks.Add(ParseArrayRankSpecifier(parser, &unused));
                    } while (parser->currentToken.kind == SyntaxKind::OpenBracketToken);

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

        if (parser->currentToken.kind == SyntaxKind::RefKeyword) {

            return parser->CreateNode<RefTypeSyntax>(
                parser->EatToken(),
                parser->currentToken.kind == SyntaxKind::ReadOnlyKeyword ? parser->EatToken() : SyntaxToken(),
                ParseTypeCore(parser, ParseTypeMode::AfterRef)
            );

        }

        return ParseTypeCore(parser, mode);
    }
}