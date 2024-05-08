#include "../PrimitiveTypes.h"
#include "./SyntaxKind.h"
#include "./Scanning.h"
#include "../Collections/CheckedArray.h"
#include "./TokenStream.h"
#include "./SyntaxFacts.h"

namespace Alchemy::Compilation {


    enum class Precedence : uint16 {
        Expression = 0, // Loosest possible precedence, used to accept all expressions
        Assignment = Expression,
        Lambda = Assignment, // "The => operator has the same precedence as assignment (=) and is right-associative."
        Conditional,
        Coalescing,
        ConditionalOr,
        ConditionalAnd,
        LogicalOr,
        LogicalXor,
        LogicalAnd,
        Equality,
        Relational,
        Shift,
        Additive,
        Multiplicative,
        Switch,
        Range,
        Unary,
        Cast,
        PointerIndirection, // maybe remove
        AddressOf,          // maybe remove
        Primary,
    };

    DEFINE_ENUM_FLAGS(TerminatorState, uint32, {
        EndOfFile = 0,
        IsNamespaceMemberStartOrStop = 1 << 0,
        IsAttributeDeclarationTerminator = 1 << 1,
        IsPossibleAggregateClauseStartOrStop = 1 << 2,
        IsPossibleMemberStartOrStop = 1 << 3,
        IsEndOfReturnType = 1 << 4,
        IsEndOfParameterList = 1 << 5,
        IsEndOfFieldDeclaration = 1 << 6,
        IsPossibleEndOfVariableDeclaration = 1 << 7,
        IsEndOfTypeArgumentList = 1 << 8,
        IsPossibleStatementStartOrStop = 1 << 9,
        IsEndOfFixedStatement = 1 << 10,
        IsEndOfTryBlock = 1 << 11,
        IsEndOfCatchClause = 1 << 12,
        IsEndOfFilterClause = 1 << 13,
        IsEndOfCatchBlock = 1 << 14,
        IsEndOfDoWhileExpression = 1 << 15,
        IsEndOfForStatementArgument = 1 << 16,
        IsEndOfDeclarationClause = 1 << 17,
        IsEndOfArgumentList = 1 << 18,
        IsSwitchSectionStart = 1 << 19,
        IsEndOfTypeParameterList = 1 << 20,
        IsEndOfMethodSignature = 1 << 21,
        IsEndOfNameInExplicitInterface = 1 << 22,
        IsEndOfFunctionPointerParameterList = 1 << 23,
        IsEndOfFunctionPointerParameterListErrored = 1 << 24,
        IsEndOfFunctionPointerCallingConvention = 1 << 25,
        IsEndOfRecordOrClassOrStructOrInterfaceSignature = 1 << 26,
        IsExpressionOrPatternInCaseLabelOfSwitchStatement = 1 << 27,
        IsPatternInSwitchExpressionArm = 1 << 28,
        __LAST__
    })

    bool IsPossibleNamespaceMemberDeclaration(TokenStream* tokenStream);

    bool IsPartialType(TokenStream* tokenStream);

    ScanTypeFlags ScanType(TokenStream* tokenStream, ParseTypeMode mode, SyntaxToken* lastTokenOfType);

    ScanTypeFlags ScanType(TokenStream* tokenStream, SyntaxToken* lastTokenOfType, bool forPattern) {
        return ScanType(tokenStream, forPattern ? ParseTypeMode::DefinitePattern : ParseTypeMode::Normal, lastTokenOfType);
    }

    bool IsEndOfNamespace(TokenStream* tokenStream) {
        return tokenStream->currentToken.kind == SyntaxKind::CloseBraceToken;
    }

    bool IsNamespaceMemberStartOrStop(TokenStream* tokenStream) {
        return IsEndOfNamespace(tokenStream) || IsPossibleNamespaceMemberDeclaration(tokenStream);
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

    bool IsPossibleMemberStart(TokenStream* tokenStream) {
        return CanStartMember(tokenStream->currentToken.kind);
    }

    bool IsPossibleMemberStartOrStop(TokenStream* tokenStream) {
        return IsPossibleMemberStart(tokenStream) || tokenStream->currentToken.kind == SyntaxKind::CloseBraceToken;
    }

    bool IsEndOfReturnType(TokenStream* tokenStream) {
        switch (tokenStream->currentToken.kind) {
            case SyntaxKind::OpenParenToken:
            case SyntaxKind::OpenBraceToken:
            case SyntaxKind::SemicolonToken:
                return true;
            default:
                return false;
        }
    }

    bool IsEndOfParameterList(TokenStream* tokenStream) {
        SyntaxKind kind = tokenStream->currentToken.kind;
        return kind == SyntaxKind::CloseParenToken || kind == SyntaxKind::CloseBracketToken || kind == SyntaxKind::SemicolonToken;
    }

    bool IsEndOfFieldDeclaration(TokenStream* tokenStream) {
        return tokenStream->currentToken.kind == SyntaxKind::SemicolonToken;
    }

    bool IsPossibleEndOfVariableDeclaration(TokenStream* tokenStream) {
        switch (tokenStream->currentToken.kind) {
            case SyntaxKind::CommaToken:
            case SyntaxKind::SemicolonToken:
                return true;
            default:
                return false;
        }
    }

    bool IsEndOfTypeArgumentList(TokenStream* tokenStream) {
        return tokenStream->currentToken.kind == SyntaxKind::GreaterThanToken;
    }

    bool IsPartialMember(TokenStream* tokenStream) {
        // partials must be void return type
        return tokenStream->currentToken.contextualKind == SyntaxKind::PartialKeyword && tokenStream->PeekToken(1).kind == SyntaxKind::VoidKeyword;
    }

    bool IsCurrentTokenPartialKeywordOfPartialMethodOrType(TokenStream* tokenStream) {
        if (tokenStream->currentToken.contextualKind == SyntaxKind::PartialKeyword) {
            if (IsPartialType(tokenStream) || IsPartialMember(tokenStream)) {
                return true;
            }
        }

        return false;
    }

    bool IsCurrentTokenWhereOfConstraintClause(TokenStream* tokenStream) {
        return
                tokenStream->currentToken.contextualKind == SyntaxKind::WhereKeyword &&
                tokenStream->PeekToken(1).kind == SyntaxKind::IdentifierToken &&
                tokenStream->PeekToken(2).kind == SyntaxKind::ColonToken;
    }

    bool IsTrueIdentifier(TokenStream* tokenStream) {
        if (tokenStream->currentToken.kind == SyntaxKind::IdentifierToken
            && !IsCurrentTokenPartialKeywordOfPartialMethodOrType(tokenStream)
            && !IsCurrentTokenWhereOfConstraintClause(tokenStream)) {
            return true;
        }

        return false;
    }

    bool IsPossibleAnonymousMethodExpression(TokenStream* tokenStream) {
        // Skip past any static/async keywords.
        int32 tokenIndex = 0;
        while (tokenStream->PeekToken(tokenIndex).kind == SyntaxKind::StaticKeyword) {
            tokenIndex++;
        }

        return tokenStream->PeekToken(tokenIndex).kind == SyntaxKind::DelegateKeyword;
    }

    ScanTypeFlags ScanPossibleTypeArgumentList(TokenStream* tokenStream, SyntaxToken* greaterThanToken, bool* isDefinitelyTypeArgumentList) {
        *isDefinitelyTypeArgumentList = false;
        assert(tokenStream->currentToken.kind == SyntaxKind::LessThanToken);

        ScanTypeFlags result = ScanTypeFlags::GenericTypeOrExpression;
        ScanTypeFlags lastScannedType;

        do {
            tokenStream->EatToken();

            // Type arguments cannot contain attributes, so if this is an open square, we early out and assume it is not a type argument
            if (tokenStream->currentToken.kind == SyntaxKind::OpenBracketToken) {
                return ScanTypeFlags::NotType;
            }

            if (tokenStream->currentToken.kind == SyntaxKind::GreaterThanToken) {
                *greaterThanToken = tokenStream->EatToken();
                return result;
            }

            SyntaxToken unused = SyntaxToken();
            lastScannedType = ScanType(tokenStream, &unused, false);

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
                    *isDefinitelyTypeArgumentList = *isDefinitelyTypeArgumentList || tokenStream->currentToken.kind == SyntaxKind::CommaToken || tokenStream->currentToken.kind == SyntaxKind::GreaterThanToken;
                    result = ScanTypeFlags::GenericTypeOrMethod;
                    break;

                case ScanTypeFlags::NullableType:
                    // See above.  If we have `X<Y?,` or `X<Y?>` then this is definitely a type argument list.
                    *isDefinitelyTypeArgumentList = *isDefinitelyTypeArgumentList || tokenStream->currentToken.kind == SyntaxKind::CommaToken || tokenStream->currentToken.kind == SyntaxKind::GreaterThanToken;
                    if (*isDefinitelyTypeArgumentList) {
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
                    if (!*isDefinitelyTypeArgumentList) {
                        *isDefinitelyTypeArgumentList = tokenStream->currentToken.kind == SyntaxKind::CommaToken;
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
                case ScanTypeFlags::AliasQualifiedName:
                case ScanTypeFlags::PointerOrMultiplication:
                case ScanTypeFlags::TupleType:
                    break;
            }
        } while (tokenStream->currentToken.kind == SyntaxKind::CommaToken);

        if (tokenStream->currentToken.kind != SyntaxKind::GreaterThanToken) {
            // Error recovery after missing > token:

            // In the case of an identifier, we assume that there could be a missing > token
            // For example, we have reached C in X<A, B C
            if (tokenStream->currentToken.kind == SyntaxKind::IdentifierToken) {
                *greaterThanToken = tokenStream->EatToken(SyntaxKind::GreaterThanToken);
                return result;
            }

            // As for tuples, we do not expect direct invocation right after the parenthesis
            // EXAMPLE: X<(string, string)(), where we imply a missing > token between )(
            // as the user probably wants to invoke X by X<(string, string)>()
            if (lastScannedType == ScanTypeFlags::TupleType && tokenStream->currentToken.kind == SyntaxKind::OpenParenToken) {
                *greaterThanToken = tokenStream->EatToken(SyntaxKind::GreaterThanToken);
                return result;
            }

            return ScanTypeFlags::NotType;
        }

        *greaterThanToken = tokenStream->EatToken();

        // If we have `X<Y>)` then this would definitely be a type argument list.
        *isDefinitelyTypeArgumentList = *isDefinitelyTypeArgumentList || tokenStream->currentToken.kind == SyntaxKind::CloseParenToken;
        if (*isDefinitelyTypeArgumentList) {
            result = ScanTypeFlags::GenericTypeOrMethod;
        }

        return result;
    }

    ScanTypeFlags ScanNamedTypePart(TokenStream* tokenStream, SyntaxToken* lastTokenOfType) {

        if (tokenStream->currentToken.kind != SyntaxKind::IdentifierToken || !IsTrueIdentifier(tokenStream)) {
            return ScanTypeFlags::NotType;
        }

        *lastTokenOfType = tokenStream->EatToken();
        if (tokenStream->currentToken.kind == SyntaxKind::LessThanToken) {
            bool unused = false;
            return ScanPossibleTypeArgumentList(tokenStream, lastTokenOfType, &unused);
        }
        else {
            return ScanTypeFlags::NonGenericTypeOrExpression;
        }

        return ScanTypeFlags::NullableType;
    }


    bool IsDotOrColonColon(TokenStream* tokenStream) {
        return tokenStream->currentToken.kind == SyntaxKind::DotToken || tokenStream->currentToken.kind == SyntaxKind::ColonColonToken;
    }



    ScanTypeFlags ScanTupleType(TokenStream* tokenStream, SyntaxToken* lastTokenOfType) {
        ScanTypeFlags tupleElementType = ScanType(tokenStream, lastTokenOfType, false);
        if (tupleElementType != ScanTypeFlags::NotType) {
            if (IsTrueIdentifier(tokenStream)) {
                *lastTokenOfType = tokenStream->EatToken();
            }

            if (tokenStream->currentToken.kind == SyntaxKind::CommaToken) {
                do {
                    *lastTokenOfType = tokenStream->EatToken();
                    tupleElementType = ScanType(tokenStream, lastTokenOfType, false);

                    if (tupleElementType == ScanTypeFlags::NotType) {
                        *lastTokenOfType = tokenStream->EatToken();
                        return ScanTypeFlags::NotType;
                    }

                    if (IsTrueIdentifier(tokenStream)) {
                        *lastTokenOfType = tokenStream->EatToken();
                    }
                } while (tokenStream->currentToken.kind == SyntaxKind::CommaToken);

                if (tokenStream->currentToken.kind == SyntaxKind::CloseParenToken) {
                    *lastTokenOfType = tokenStream->EatToken();
                    return ScanTypeFlags::TupleType;
                }
            }
        }

        // Can't be a type!
        return ScanTypeFlags::NotType;
    }

    bool IsFunctionPointerStart(TokenStream* tokenStream) {
        return false;
    }

    ScanTypeFlags ScanFunctionPointerType(TokenStream* pStream, SyntaxToken* pToken) {
        return ScanTypeFlags::NullableType;
    }

    bool IsMakingProgress(TokenStream* tokenStream, int* lastTokenPosition, bool assertIfFalse = true) {
        int32 pos = tokenStream->ptr;

        if (pos > *lastTokenPosition) {
            *lastTokenPosition = pos;
            return true;
        }

        assert(!assertIfFalse);
        return false;
    }

    ScanTypeFlags ScanType(TokenStream* tokenStream, ParseTypeMode mode, SyntaxToken* lastTokenOfType) {
        assert(mode != ParseTypeMode::NewExpression);
        ScanTypeFlags result;

        if (tokenStream->currentToken.kind == SyntaxKind::RefKeyword) {
            // todo -- extend this for 'temp', 'scoped' etc
            // in a ref local or ref return, we treat "ref" and "ref readonly" as part of the type
            tokenStream->EatToken();

            if (tokenStream->currentToken.kind == SyntaxKind::ReadOnlyKeyword) {
                tokenStream->EatToken();
            }

        }

        // Handle :: as well for error case of an alias used without a preceding identifier.
        if (tokenStream->currentToken.kind == SyntaxKind::IdentifierToken || tokenStream->currentToken.kind == SyntaxKind::ColonColonToken) {
            bool isAlias;
            if (tokenStream->currentToken.kind == SyntaxKind::ColonColonToken) {
                result = ScanTypeFlags::NonGenericTypeOrExpression;

                // Definitely seems like an alias if we're starting with a ::
                isAlias = true;
            }
            else {
                assert(tokenStream->currentToken.kind == SyntaxKind::IdentifierToken);

                // We're an alias if we start with an: id::
                isAlias = tokenStream->PeekToken(1).kind == SyntaxKind::ColonColonToken;

                result = ScanNamedTypePart(tokenStream, lastTokenOfType);
                if (result == ScanTypeFlags::NotType) {
                    return ScanTypeFlags::NotType;
                }

                assert(result == ScanTypeFlags::GenericTypeOrExpression || result == ScanTypeFlags::GenericTypeOrMethod || result == ScanTypeFlags::NonGenericTypeOrExpression);
            }

            // Scan a name
            for (bool firstLoop = true; IsDotOrColonColon(tokenStream); firstLoop = false) {
                // If we consume any more dots or colons, don't consider us an alias anymore.  For dots, we now have
                // x::y.z (which is now back to a normal expr/type, not an alias), and for colons that means we have
                // x::y::z or x.y::z both of which are effectively gibberish.

                // todo -- we actually allow this because :: is our namespace separator

                if (!firstLoop) {
                    isAlias = false;
                }

                tokenStream->EatToken();
                result = ScanNamedTypePart(tokenStream, lastTokenOfType);
                if (result == ScanTypeFlags::NotType) {
                    return ScanTypeFlags::NotType;
                }

                assert(result == ScanTypeFlags::GenericTypeOrExpression || result == ScanTypeFlags::GenericTypeOrMethod || result == ScanTypeFlags::NonGenericTypeOrExpression);
            }

            if (isAlias) {
                result = ScanTypeFlags::AliasQualifiedName;
            }

        }
        else if (SyntaxFacts::IsPredefinedType(tokenStream->currentToken.kind)) {
            // Simple type...
            *lastTokenOfType = tokenStream->EatToken();
            result = ScanTypeFlags::MustBeType;
        }
        else if (tokenStream->currentToken.kind == SyntaxKind::OpenParenToken) {
            *lastTokenOfType = tokenStream->EatToken();

            result = ScanTupleType(tokenStream, lastTokenOfType);
            if (result == ScanTypeFlags::NotType || mode == ParseTypeMode::DefinitePattern && tokenStream->currentToken.kind != SyntaxKind::OpenBracketToken) {
                // A tuple type can appear in a pattern only if it is the element type of an array type.
                return ScanTypeFlags::NotType;
            }
        }
        else if (IsFunctionPointerStart(tokenStream)) {
            result = ScanFunctionPointerType(tokenStream, lastTokenOfType);
        }
        else {
            // Can't be a type!
            return ScanTypeFlags::NotType;
        }

        int lastTokenPosition = -1;

        while (IsMakingProgress(tokenStream, &lastTokenPosition)) {

            if (tokenStream->currentToken.kind == SyntaxKind::QuestionToken) {

                if (lastTokenOfType->kind == SyntaxKind::QuestionToken && lastTokenOfType->kind != SyntaxKind::AsteriskToken) {
                    // don't allow `Type??`
                    // don't allow `Type*?`
                    *lastTokenOfType = tokenStream->EatToken();
                    result = ScanTypeFlags::NullableType;
                    continue;
                }

            }

            switch (tokenStream->currentToken.kind) {
                case SyntaxKind::OpenBracketToken:
                    // Check for array types.
                    tokenStream->EatToken();
                    while (tokenStream->currentToken.kind == SyntaxKind::CommaToken) {
                        tokenStream->EatToken();
                    }

                    if (tokenStream->currentToken.kind != SyntaxKind::CloseBracketToken) {
                        return ScanTypeFlags::NotType;
                    }

                    *lastTokenOfType = tokenStream->EatToken();
                    result = ScanTypeFlags::MustBeType;
                    break;
                default:
                    goto done;
            }
        }

        done:
        return result;
    }

    bool ScanParenthesizedLambda(TokenStream* tokenStream, Precedence precedence) {
        return false;
    }

// For better error recovery 'static =>' is also considered a possible lambda expression.
    // pass by copy to avoid mutation since we advance token stream a bit
    bool IsPossibleLambdaExpression(TokenStream tokenStream, Precedence precedence) {
        if (precedence > Precedence::Lambda) {
            return false;
        }

        SyntaxToken token1 = tokenStream.PeekToken(1);

        // x =>
        if (token1.kind == SyntaxKind::EqualsGreaterThanToken) {
            return true;
        }

        bool seenStatic = false;
        if (tokenStream.currentToken.kind == SyntaxKind::StaticKeyword) {
            tokenStream.EatToken();
            seenStatic = true;
        }

        if (seenStatic) {
            if (tokenStream.currentToken.kind == SyntaxKind::EqualsGreaterThanToken) {
                // `static =>`
                // This is an error case, but we have enough code in front of us to be certain
                // the user was trying to write a static lambda.
                return true;
            }

            if (tokenStream.currentToken.kind == SyntaxKind::OpenParenToken) {
                // static (...
                return true;
            }
        }

        if (tokenStream.currentToken.kind == SyntaxKind::IdentifierToken && tokenStream.PeekToken(1).kind == SyntaxKind::EqualsGreaterThanToken) {
            // 1. `a => ...`
            // 2. `static a => ...`
            return true;
        }

        // Have checked all the static forms.  And have checked for the basic `a => a` form.
        // At this point we have must be on an explicit return type for this to still be a lambda.
        //        if (tokenStream.currentToken.contextualKind == SyntaxKind::AsyncKeyword &&            IsAnonymousFunctionAsyncModifier()) {
        //            EatToken();
        //        }

        TokenStream cpy = tokenStream;
        SyntaxToken k = {0};
        ScanTypeFlags st = ScanType(&cpy, &k, false);
        if (st == ScanTypeFlags::NotType || tokenStream.currentToken.kind != SyntaxKind::OpenParenToken) {
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
        if (tokenStream.currentToken.kind == SyntaxKind::IdentifierToken && tokenStream.PeekToken(1).kind == SyntaxKind::EqualsGreaterThanToken) {
            //  a => ...
            return true;
        }

        // Non-simple async lambda must be of the form 'async (...'
        if (tokenStream.currentToken.kind != SyntaxKind::OpenParenToken) {
            return false;
        }

        // Check whether looks like implicitly or explicitly typed lambda
        return ScanParenthesizedLambda(&tokenStream, precedence);
    }

    bool IsPossibleExpression(TokenStream* tokenStream, bool allowBinaryExpressions, bool allowAssignmentExpressions) {
        SyntaxKind tk = tokenStream->currentToken.kind;
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
            case SyntaxKind::ColonColonToken: // I guess namespace specifier
            case SyntaxKind::ThrowKeyword:
//            case SyntaxKind::StackAllocKeyword: probably allow something like this
            case SyntaxKind::DotDotToken:
            case SyntaxKind::RefKeyword:
            case SyntaxKind::OpenBracketToken: // attributes on a lambda, or a collection expression.
                return true;
            case SyntaxKind::StaticKeyword:
                return IsPossibleAnonymousMethodExpression(tokenStream) || IsPossibleLambdaExpression(*tokenStream, Precedence::Expression);
            case SyntaxKind::IdentifierToken:
                // Specifically allow the from contextual keyword, because it can always be the start of an
                // expression (whether it is used as an identifier or a keyword).
                return IsTrueIdentifier(tokenStream) || tokenStream->currentToken.contextualKind == SyntaxKind::FromKeyword;
            default:
                return SyntaxFacts::IsPredefinedType(tk)
                       || SyntaxFacts::IsAnyUnaryExpression(tk)
                       || (allowBinaryExpressions && SyntaxFacts::IsBinaryExpression(tk))
                       || (allowAssignmentExpressions && SyntaxFacts::IsAssignmentExpressionOperatorToken(tk));
        }
    }

    bool IsPossibleExpression(TokenStream* tokenStream) {
        return IsPossibleExpression(tokenStream, true, true);
    }

    bool IsPossibleStatement(TokenStream* tokenStream, bool acceptAccessibilityMods) {
        SyntaxKind tk = tokenStream->currentToken.kind;
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
                return IsTrueIdentifier(tokenStream);

                // Accessibility modifiers are not legal in a statement,
                // but a common mistake for local functions. Parse to give a
                // better error message.
            case SyntaxKind::PublicKeyword:
            case SyntaxKind::InternalKeyword:
            case SyntaxKind::ProtectedKeyword:
            case SyntaxKind::PrivateKeyword:
                return acceptAccessibilityMods;
            default:
                return SyntaxFacts::IsPredefinedType(tk) || IsPossibleExpression(tokenStream);
        }
    }

    bool IsPossibleStatementStartOrStop(TokenStream* tokenStream) {
        return tokenStream->currentToken.kind == SyntaxKind::SemicolonToken || IsPossibleStatement(tokenStream, true);
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

    bool IsPartialType(TokenStream* tokenStream) {
        assert(tokenStream->currentToken.contextualKind == SyntaxKind::PartialKeyword);
        switch (tokenStream->PeekToken(1).kind) {
            case SyntaxKind::StructKeyword:
            case SyntaxKind::ClassKeyword:
            case SyntaxKind::InterfaceKeyword:
                return true;
            default:
                return false;
        }
    }

    bool IsPartialInNamespaceMemberDeclaration(TokenStream* tokenStream) {
        if (tokenStream->currentToken.contextualKind == SyntaxKind::PartialKeyword) {
            if (IsPartialType(tokenStream)) {
                return true;
            }
            else if (tokenStream->PeekToken(1).kind == SyntaxKind::NamespaceKeyword) {
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

    bool IsTypeDeclarationStart(TokenStream* tokenStream) {
        switch (tokenStream->currentToken.kind) {
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

    bool IsPossibleNamespaceMemberDeclaration(TokenStream* tokenStream) {
        switch (tokenStream->currentToken.kind) {
            case SyntaxKind::ExternKeyword:
            case SyntaxKind::UsingKeyword:
            case SyntaxKind::NamespaceKeyword:
                return true;
            case SyntaxKind::IdentifierToken:
                return IsPartialInNamespaceMemberDeclaration(tokenStream);
            default:
                return IsPossibleStartOfTypeDeclaration(tokenStream->currentToken.kind);
        }
    }

    bool IsTerminator(TerminatorState terminatorState, TokenStream* tokenStream) {

        if (tokenStream->currentToken.kind == SyntaxKind::EndOfFileToken) {
            return true;
        }

        int32 num = (int32) terminatorState;

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
                    if (IsNamespaceMemberStartOrStop(tokenStream)) {
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
                    if (IsPossibleMemberStartOrStop(tokenStream)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfReturnType: {
                    if (IsEndOfReturnType(tokenStream)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfParameterList: {
                    if (IsEndOfParameterList(tokenStream)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfFieldDeclaration: {
                    if (IsEndOfFieldDeclaration(tokenStream)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsPossibleEndOfVariableDeclaration: {
                    if (IsPossibleEndOfVariableDeclaration(tokenStream)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsEndOfTypeArgumentList: {
                    if (IsEndOfTypeArgumentList(tokenStream)) {
                        return true;
                    }
                    continue;
                }
                case TerminatorState::IsPossibleStatementStartOrStop: {
                    if (IsPossibleStatementStartOrStop(tokenStream)) {
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


}

