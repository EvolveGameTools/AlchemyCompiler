#pragma once

#include "./PsiBuilder2.h"
#include "KeywordString.h"

namespace Alchemy {

    struct ParserImpl {

        typedef bool (* TryParseExpressionFn)(ParserImpl* p, NodeIndex<ExpressionNode>* lhs, bool* recovery);

        struct RhsTypeInfo {
            TokenType tokenType;
            BinaryExpressionType expressionType;
            const char* failureMessage;
        };

        PsiBuilder2 b;

        bool ExpectSemicolon() {
            if (b.GetTokenType() != TokenType::SemiColon) {
                b.InlineError("Expected ';'");
                return false;
            }
            else {
                b.AdvanceLexer();
                return true;
            }
        }

        bool TryParseNamespacePath(NodeIndex<NamespacePathNode>* nodeIndex, bool* needsRecovery) {
            // namespace_path ::= identifier ('::' identifier)*
            if (b.GetTokenType() != TokenType::KeywordOrIdentifier) {
                return false;
            }

            Marker m = b.Mark();

            NodeIndex<IdentifierNode> first;
            NodeIndex<IdentifierNode> ptr;
            NodeIndex<IdentifierNode> next;

            int32 startToken = b.GetTokenIndex();

            TryParseIdentifier(&first, true);

            while (b.GetTokenType() == TokenType::DoubleColon) {

                if ((b.GetToken().flags & TokenFlags::FollowedByWhitespaceOrComment) != 0) {
                    break;
                }

                b.AdvanceLexer();

                if ((b.GetToken().flags & TokenFlags::FollowedByWhitespaceOrComment) != 0) {
                    break;
                }

                if (TryParseIdentifier(&next, true)) {
                    b.GetPsiNodeUnsafe(ptr)->next = next;
                    ptr = next;
                }
                else {
                    b.CompletionOpportunity("Expected an identifier after ::");
                    *needsRecovery = true;
                    break;
                }

            }

            FixedCharSpan span = b.GetTokenSource(startToken, b.GetTokenIndex());

            *nodeIndex = b.Done(m, NamespacePathNode(span));
            return true;

        }

        bool TryParseVisibility(DefinitionVisibility* visibility) {
            // visibility ::= 'public' | 'private' | 'protected' | 'internal'

            switch (b.GetKeyword()) {

                case Keyword::Export: {
                    *visibility = DefinitionVisibility::Export;
                    b.AdvanceLexer();
                    return true;
                }
                case Keyword::Public: {
                    *visibility = DefinitionVisibility::Public;
                    b.AdvanceLexer();
                    return true;
                }
                case Keyword::Private: {
                    *visibility = DefinitionVisibility::Private;
                    b.AdvanceLexer();
                    return true;
                }
                case Keyword::Internal: {
                    *visibility = DefinitionVisibility::Internal;
                    b.AdvanceLexer();
                    return true;
                }
                case Keyword::Protected: {
                    *visibility = DefinitionVisibility::Protected;
                    b.AdvanceLexer();
                    return true;
                }
                default: {
                    *visibility = DefinitionVisibility::Unspecified;
                    return false;
                }

            }

        }

        void AddDeclarationModifier(DeclarationModifier* modifier, DeclarationModifier add, DeclarationModifier exclusiveMask = DeclarationModifier::None) {

            if ((*modifier & add) != 0) {
                b.InlineError("Duplicate modifier");
            }

            exclusiveMask &= ~add;
            if ((add & exclusiveMask) != 0) {
                // todo -- could be a better error
                b.InlineError("Modifier is exclusive with previously declaredModifier");
            }

            *modifier |= add;
            b.AdvanceLexer();

        }

        bool TryParseModifierList(NodeIndex<ModifierListNode>* nodeIndex) {

            DeclarationModifier modifiers = DeclarationModifier::None;

            Marker m = b.Mark();

            constexpr DeclarationModifier kMethodType = DeclarationModifier::Virtual | DeclarationModifier::Abstract | DeclarationModifier::Override;
            constexpr DeclarationModifier kVisibilityType = DeclarationModifier::Public | DeclarationModifier::Protected | DeclarationModifier::Private | DeclarationModifier::Internal | DeclarationModifier::Export;

            // todo -- maybe move dedupe elsewhere, we want to report better errors during sema anyway
            while (b.GetTokenType() == TokenType::KeywordOrIdentifier) {
                switch (b.GetKeyword()) {
                    case Keyword::Static: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Static);
                        break;
                    }
                    case Keyword::Ref: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Ref);
                        break;
                    }
                    case Keyword::Readonly: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Readonly);
                        break;
                    }
                    case Keyword::Sealed: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Sealed, DeclarationModifier::Virtual | DeclarationModifier::Abstract);
                        break;
                    }
                    case Keyword::Abstract: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Abstract, kMethodType | DeclarationModifier::Sealed);
                        break;
                    }
                    case Keyword::Virtual: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Virtual, kMethodType | DeclarationModifier::Sealed);
                        break;
                    }
                    case Keyword::Override: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Override, kMethodType);
                        break;
                    }
                    case Keyword::Public: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Public, kVisibilityType);
                        break;
                    }
                    case Keyword::Export: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Export, kVisibilityType);
                        break;
                    }
                    case Keyword::Private: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Private, kVisibilityType);
                        break;
                    }
                    case Keyword::Protected: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Protected, kVisibilityType);
                        break;
                    }
                    case Keyword::Internal: {
                        AddDeclarationModifier(&modifiers, DeclarationModifier::Internal, kVisibilityType);
                        break;
                    }
                    default: {
                        goto end;
                    }
                }
            }
            end:

            if (modifiers == DeclarationModifier::None) {
                b.Drop(m);
                return false;
            }
            *nodeIndex = b.Done(m, ModifierListNode(modifiers));
            return true;

        }

//        bool TryParseTypeDeclarationModifier(TypeDeclarationModifier* modifiers) {
//            *modifiers = TypeDeclarationModifier::None;
//
//            constexpr TypeDeclarationModifier kVisibilityType = TypeDeclarationModifier::Public | TypeDeclarationModifier::Protected | TypeDeclarationModifier::Private | TypeDeclarationModifier::Internal;
//
//            while (b.GetTokenType() == TokenType::KeywordOrIdentifier) {
//                switch (b.GetKeyword()) {
//                    case Keyword::Static: {
//                        AddTypeModifier(modifiers, TypeDeclarationModifier::Static);
//                        break;
//                    }
//                    case Keyword::Readonly: {
//                        AddTypeModifier(modifiers, TypeDeclarationModifier::Readonly);
//                        break;
//                    }
//                    case Keyword::Sealed: {
//                        AddTypeModifier(modifiers, TypeDeclarationModifier::Sealed, TypeDeclarationModifier::Abstract);
//                        break;
//                    }
//                    case Keyword::Abstract: {
//                        AddTypeModifier(modifiers, TypeDeclarationModifier::Abstract, TypeDeclarationModifier::Sealed);
//                        break;
//                    }
//                    case Keyword::Public: {
//                        AddTypeModifier(modifiers, TypeDeclarationModifier::Public, kVisibilityType);
//                        break;
//                    }
//                    case Keyword::Private: {
//                        AddTypeModifier(modifiers, TypeDeclarationModifier::Private, kVisibilityType);
//                        break;
//                    }
//                    case Keyword::Protected: {
//                        AddTypeModifier(modifiers, TypeDeclarationModifier::Protected, kVisibilityType);
//                        break;
//                    }
//                    case Keyword::Internal: {
//                        AddTypeModifier(modifiers, TypeDeclarationModifier::Internal, kVisibilityType);
//                        break;
//                    }
//                    default: {
//                        goto end;
//                    }
//                }
//            }
//            end:
//            return *modifiers != TypeDeclarationModifier::None;
//        }

        bool AdvanceToCurlyOpen() {

            int32 start = b.GetTokenIndex();

            while (!b.EndOfInput()) {
                if (b.GetTokenType() != TokenType::CurlyBraceOpen) {
                    b.AdvanceLexer();
                    continue;
                }
                return true;
            }
            b.SetTokenIndex(start);
            return false;
        }

        void AdvanceToCommaOrCurlyOpen() {
            while (!b.EndOfInput()) {
                if (b.GetTokenType() == TokenType::Comma) {
                    break;
                }
                if (b.GetTokenType() != TokenType::CurlyBraceOpen) {
                    b.AdvanceLexer();
                    continue;
                }
                break;
            }
        }

        bool TryParseBaseList(NodeIndex<TypeListNode>* nodeIndex, bool* needsRecovery) {
            // base_list ::= ':' type_path (',' type_path)*
            if (b.GetTokenType() != TokenType::Colon) {
                return false;
            }

            Marker m = b.Mark();

            b.AdvanceLexer();

            NodeIndex<TypePathNode> first;
            if (!TryParseTypePath(&first, needsRecovery) || *needsRecovery) {

                if (!*needsRecovery) {
                    b.InlineError("Expected type path after :");
                }

            }
            else {
                NodeIndex<TypePathNode> next;
                NodeIndex<TypePathNode> ptr = first;
                while (b.GetTokenType() == TokenType::Comma) {
                    b.AdvanceLexer();

                    if (TryParseTypePath(&next, needsRecovery)) {
                        b.GetPsiNodeUnsafe(ptr)->next = next;
                        ptr = next;
                    }
                    else {
                        b.InlineError("Expected a type after , in interface list");
                        *needsRecovery = true;
                    }

                    if (*needsRecovery) {
                        AdvanceToCommaOrCurlyOpen();
                        if (b.GetTokenType() == TokenType::Comma) {
                            continue;
                        }
                        break;
                    }

                }
            }

            *nodeIndex = b.Done(m, TypeListNode(first));
            return true;


        }


        static bool IsAssignmentOperator(TokenType tokenType) {
            switch (tokenType) {
                case TokenType::Assign:
                case TokenType::AddAssign:
                case TokenType::SubtractAssign:
                case TokenType::MultiplyAssign:
                case TokenType::DivideAssign:
                case TokenType::ModAssign:
                case TokenType::AndAssign:
                case TokenType::OrAssign:
                case TokenType::XorAssign:
                case TokenType::LeftShiftAssign:
                case TokenType::RightShiftAssign:
                case TokenType::CoalesceAssign:
                    return true;
                default:
                    return false;
            }
        }

        AssignmentOperator TryParseAssignmentOperator() {
            TokenType tokenType = b.GetTokenType();
            switch (tokenType) {
                case TokenType::Assign:
                    b.AdvanceLexer();
                    return AssignmentOperator::Assign;
                case TokenType::AddAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::PlusEquals;
                case TokenType::SubtractAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::MinusEquals;
                case TokenType::MultiplyAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::TimesEquals;
                case TokenType::DivideAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::DivideEquals;
                case TokenType::ModAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::ModEquals;
                case TokenType::AndAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::AndEquals;
                case TokenType::OrAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::OrEquals;
                case TokenType::XorAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::XorEquals;
                case TokenType::LeftShiftAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::LeftShiftEquals;
                case TokenType::RightShiftAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::RightShiftEquals;
                case TokenType::CoalesceAssign:
                    b.AdvanceLexer();
                    return AssignmentOperator::CoalesceEquals;
                default:
                    return AssignmentOperator::Invalid;
            }
        }


        bool TryParseAssignmentExpression(NodeIndex<ExpressionNode> lhs, NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // assignment_expression ::= unary_expression assignment_operator expression
            //                         | unary_expression '??=' throwable_expression


            AssignmentOperator op = TryParseAssignmentOperator();

            if (op == AssignmentOperator::Invalid) {
                return false;
            }

            Marker lhsMarker = b.GetMarker(lhs.index);
            Marker m = b.Precede(lhsMarker);

            NodeIndex<ExpressionNode> rhs;

            if (op == AssignmentOperator::CoalesceEquals) {

                if (!TryParseThrowableExpression(&rhs, needsRecovery)) {
                    *needsRecovery = true;
                    b.InlineError("Expected an expression after coalesce assignment operator");
                }

                *nodeIndex = b.DoneExpression(m, AssignmentExpressionNode(lhs, op, rhs));
                return true;
            }

            if (!TryParseExpression(&rhs, needsRecovery)) {
                *needsRecovery = true;
                b.InlineError("Expected an expression after assignment operator");
            }

            *nodeIndex = b.DoneExpression(m, AssignmentExpressionNode(lhs, op, rhs));

            return true;

        }

        bool TryParseLambdaParameter(NodeIndex<LambdaParameterNode>* nodeIndex) {

            // peek identifier
            // if next is identifier we have a type + identifier
            // if next is ? or [ or :: or < we have a type
            Marker m = b.Mark();

            StorageClass storage = StorageClass::Default;
            ArgumentPassByModifier passByModifier = ArgumentPassByModifier::None;
            NodeIndex<TypePathNode> typePath;
            NodeIndex<IdentifierNode> identifier;

            int32 lexStart = b.GetTokenIndex();

            Keyword keyword = b.GetKeyword();
            if (keyword == Keyword::Temp || keyword == Keyword::Scoped) {
                storage = keyword == Keyword::Temp ? StorageClass::Temp : StorageClass::Scoped;
                b.AdvanceLexer();
                keyword = b.GetKeyword();
            }

            if (keyword == Keyword::Ref || keyword == Keyword::Out) {
                passByModifier = keyword == Keyword::Ref ? ArgumentPassByModifier::Ref : ArgumentPassByModifier::Out;
                b.AdvanceLexer();
            }

            TokenType tokenType = b.GetTokenType();

            if (tokenType != TokenType::KeywordOrIdentifier) {
                if (lexStart == b.GetTokenIndex()) {
                    b.RollbackTo(m);
                    return false;
                }
                else {
                    TryRecoverToCommaOrEnd("Expected an identifier");
                    *nodeIndex = b.Done(m, LambdaParameterNode(storage, passByModifier, typePath, identifier));
                    return true;
                }
            }

            Token nextToken = b.Peek(1);
            TokenType nextType = nextToken.tokenType;

            if (nextType == TokenType::QuestionMark || nextType == TokenType::SquareBraceOpen || nextType == TokenType::DoubleColon || nextType == TokenType::AngleBracketOpen) {

                bool recovery = false;

                // should never fail, we checked identifier & next token, may still need recovery
                TryParseTypePath(&typePath, &recovery);

                if (recovery) {
                    TryRecoverToCommaOrEnd("Expected a lambda parameter");
                }

                if (!TryParseIdentifier(&identifier, true)) {
                    b.InlineError("Expected a lambda parameter name");
                }

                *nodeIndex = b.Done(m, LambdaParameterNode(storage, passByModifier, typePath, identifier));
                return true;

            }
            else if (nextType == TokenType::KeywordOrIdentifier) {
                *nodeIndex = b.Done(m, LambdaParameterNode(storage, passByModifier, typePath, identifier));
                return true;
            }
            else {
                // single identifier
                TryParseIdentifier(&identifier, true);
                *nodeIndex = b.Done(m, LambdaParameterNode(storage, passByModifier, typePath, identifier));
                return true;
            }

        }

        bool TryParseLambdaExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // lambda_expression ::= '(' (lambda_parameter (',' lambda_parameter)*)? ')' '=>' (block | throwable_expression)
            if (b.GetTokenType() == TokenType::OpenParen) {

                Marker m = b.Mark();

                PsiBuilder2::SubStream stream;
                b.TryGetSubSubStream_Paren(&stream);

                b.PushStream(&stream);

                // (x) => {}
                // (float x) => {}
                NodeIndex<LambdaParameterNode> first;
                NodeIndex<LambdaParameterNode> next;
                NodeIndex<LambdaParameterNode> ptr;

                if (!stream.Empty()) {

                    while (!b.EndOfInput()) {

                        // if we fail to get the first parameter we skip until we can get it
                        if (TryParseLambdaParameter(&first)) {
                            break;
                        }

                    }

                    while (!b.EndOfInput()) {

                        if (b.GetTokenType() == TokenType::Comma) {

                            b.AdvanceLexer();
                            if (b.EndOfInput()) {
                                b.InlineError("Trailing comma in lambda parameter list is not allowed");
                                break;
                            }

                            if (TryParseLambdaParameter(&next)) {
                                b.GetPsiNodeUnsafe(ptr)->next = next;
                                ptr = next;
                            }

                        }

                        else {
                            TryRecoverToCommaOrEnd("Expected a lambda parameter definition, you might be missing a comma");
                        }

                    }

                }

                b.PopStream();

                if (b.GetTokenType() != TokenType::FatArrow) {
                    b.RollbackTo(m);
                    return false;
                }

                NodeIndex<BlockNode> block;
                NodeIndex<ExpressionNode> expression;

                if (!TryParseBlock(&block) && !TryParseThrowableExpression(&expression, needsRecovery)) {
                    *needsRecovery = true;
                }

                *nodeIndex = b.DoneExpression(m, LambdaExpressionNode(first, block, expression));
                return true;

            }
            else if (b.GetTokenType() == TokenType::KeywordOrIdentifier) {

                // identifier => throwable

                if (b.GetKeyword() == Keyword::NotAKeyword && b.Peek(1).tokenType == TokenType::FatArrow) {
                    Marker m = b.Mark();
                    Marker m2 = b.Mark();

                    NodeIndex<IdentifierNode> identifier;
                    NodeIndex<ExpressionNode> expression;

                    TryParseIdentifier(&identifier, true);

                    NodeIndex<LambdaParameterNode> parameterIndex = b.Done(m2, LambdaParameterNode(StorageClass::Default, ArgumentPassByModifier::None, NodeIndex<TypePathNode>(0), identifier));

                    b.AdvanceLexer(); // =>

                    if (!TryParseThrowableExpression(&expression, needsRecovery)) {
                        b.InlineError("Expected an expression after =>");
                        *needsRecovery = true;
                    }

                    *nodeIndex = b.DoneExpression(m, LambdaExpressionNode(parameterIndex, NodeIndex<BlockNode>(0), expression));
                    return true;

                }
                else {
                    return false;
                }

            }

            return false;

        }

        bool TryParseTernaryExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // ternary_expression ::= null_coalescing_expression ('?' throwable_expression ':' throwable_expression)?

            if (!TryParseNullCoalescingExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            // no question mark means we pass along the previous result w/o creating a new node
            if (b.GetTokenType() != TokenType::QuestionMark) {
                return true;
            }

            Marker m = b.GetMarker(nodeIndex->index);

            m = b.Precede(m);

            b.AdvanceLexer();

            NodeIndex<ExpressionNode> trueNodeIndex;
            if (TryParseThrowableExpression(&trueNodeIndex, needsRecovery)) {

                if (*needsRecovery) {
                    *nodeIndex = b.DoneExpression(m, TernaryExpressionNode(*nodeIndex, NodeIndex<ExpressionNode>(0), NodeIndex<ExpressionNode>(0)));
                    return true;
                }

                if (b.GetTokenType() != TokenType::Colon) {
                    b.Error(b.Mark(), "Expected a ':' as part of a ternary expression");
                    *needsRecovery = true;
                    *nodeIndex = b.DoneExpression(m, TernaryExpressionNode(*nodeIndex, NodeIndex<ExpressionNode>(0), NodeIndex<ExpressionNode>(0)));
                    return true;
                }

                b.AdvanceLexer();

                NodeIndex<ExpressionNode> falseNodeIndex;
                if (TryParseThrowableExpression(&falseNodeIndex, needsRecovery)) {
                    *nodeIndex = b.DoneExpression(m, TernaryExpressionNode(*nodeIndex, trueNodeIndex, *needsRecovery ? NodeIndex<ExpressionNode>(0) : falseNodeIndex));
                    return true;
                }
                else {
                    b.Error(m, "Expected an expression after ':' as part of a ternary expression");
                    *needsRecovery = true;
                    return true;
                }

            }
            else {
                *needsRecovery = true;
                b.Error(m, "Expected an expression after '?' as part of a ternary expression");
                return true;
            }

        }

        bool TryParseNonAssignmentExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // non_assignment_expression ::= lambda_expression | conditional_expression
            return TryParseLambdaExpression(nodeIndex, needsRecovery) || TryParseTernaryExpression(nodeIndex, needsRecovery);
        }

        bool TryParseExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // expression ::= assignment | 'ref'? non_assignment_expression

            if (b.GetKeyword() == Keyword::Ref) {
                Marker m = b.Mark();
                b.AdvanceLexer();

                NodeIndex<ExpressionNode> expression;

                if (TryParseNonAssignmentExpression(&expression, needsRecovery)) {
                    return true;
                }
                else {
                    *needsRecovery = true;
                    b.InlineError("Expected an expression after 'ref'");
                }

                *nodeIndex = b.DoneExpression(m, RefExpressionNode(expression));
                return true;

            }
            else {

                // if we peek identifier + operator, do assignment to short circuit parsing
//                if (b.GetTokenType() == TokenType::KeywordOrIdentifier && b.GetKeyword() == Keyword::NotAKeyword && IsAssignmentOperator(b.Peek(1).tokenType)) {
//                    return TryParseAssignmentExpression(nodeIndex, needsRecovery);
//                }

                NodeIndex<ExpressionNode> lhs;
                if (TryParseNonAssignmentExpression(&lhs, needsRecovery)) {

                    if (TryParseAssignmentExpression(lhs, nodeIndex, needsRecovery)) {
                        return true;
                    }
                    else {
                        *nodeIndex = lhs;
                        return true;
                    }

                }

                return false;

            }

        }

        bool TryParsePropertySetter(DefinitionVisibility visibility, NodeIndex<PropertySetterNode>* nodeIndex, bool* needsRecovery) {
            // property_setter ::= 'set' (';' | ('|' identifier '|')? ('=>' expression ';') | block)
            if (b.GetKeyword() != Keyword::Set) {
                return false;
            }

            Marker m = b.Mark();

            b.AdvanceLexer();

            NodeIndex<ExpressionNode> expression;
            NodeIndex<BlockNode> block;
            NodeIndex<IdentifierNode> valueIdentifier;

            if (b.GetTokenType() == TokenType::BinaryOr) {
                b.AdvanceLexer();

                if (!TryParseIdentifier(&valueIdentifier, true)) {
                    b.InlineError("Expected a property setter value identifier");
                }

                if (b.GetTokenType() == TokenType::BinaryOr) {
                    b.AdvanceLexer();
                }
                else {
                    b.InlineError("Expected a closing | after property value identifier");
                    // try to reset to => or '{'
                    if (b.GetTokenType() != TokenType::FatArrow && b.GetTokenType() != TokenType::CurlyBraceOpen) {
                        *needsRecovery = true;
                        *nodeIndex = b.Done(m, PropertySetterNode(visibility, valueIdentifier, expression));
                        return true;
                    }
                }
            }

            if (b.GetTokenType() == TokenType::FatArrow) {
                b.AdvanceLexer();

                if (!TryParseExpression(&expression, needsRecovery)) {
                    b.InlineError("Expected a property setter expression");
                    *needsRecovery = true;
                }

                if (b.GetTokenType() != TokenType::SemiColon) {
                    *needsRecovery = true;
                    b.InlineError("Expected a semicolon");
                }
                else {
                    b.AdvanceLexer();
                }

                *nodeIndex = b.Done(m, PropertySetterNode(visibility, valueIdentifier, expression));
                return true;
            }
            else if (TryParseBlock(&block)) {
                *nodeIndex = b.Done(m, PropertySetterNode(visibility, valueIdentifier, block));
                return true;
            }
            else {
                *needsRecovery = true;
                *nodeIndex = b.Done(m, PropertySetterNode(visibility, valueIdentifier, expression));
                return true;
            }

        }

        bool TryParsePropertyGetter(DefinitionVisibility visibility, NodeIndex<PropertyGetterNode>* nodeIndex, bool* needsRecovery) {
            // property_getter ::= 'get' ';' | ('=>' expression ';') | block
            if (b.GetKeyword() != Keyword::Get) {
                return false;
            }

            Marker m = b.Mark();

            b.AdvanceLexer();

            NodeIndex<ExpressionNode> expression;
            NodeIndex<BlockNode> block;

            if (b.GetTokenType() == TokenType::FatArrow) {

                b.AdvanceLexer();

                if (!TryParseExpression(&expression, needsRecovery)) {
                    b.InlineError("Expected a property getter expression");
                    *needsRecovery = true;
                }

                if (b.GetTokenType() != TokenType::SemiColon) {
                    *needsRecovery = true;
                    b.InlineError("Expected a semicolon");
                }
                else {
                    b.AdvanceLexer();
                }

                *nodeIndex = b.Done(m, PropertyGetterNode(visibility, expression));
                return true;

            }
            else if (TryParseBlock(&block)) {
                *nodeIndex = b.Done(m, PropertyGetterNode(visibility, block));
                return true;
            }
            else {
                *needsRecovery = true;
                *nodeIndex = b.Done(m, PropertyGetterNode(visibility, expression));
                return true;
            }
        }

        bool TryParsePropertyDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            return TryParsePropertyDeclarationBody<PropertyDeclarationNode, NodeIndex<IdentifierNode>>(m, modifiers, typePath, identifier, nodeIndex, needsRecovery);
        }

        template<typename T, typename U>
        bool TryParsePropertyDeclarationBody(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, U input, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // property_declaration ::= type_path (identifier | 'this' indexer_parameter_list) ( '=>' expression ';') | ('{' property_getter | property_setter '}')

            NodeIndex<PropertyGetterNode> getterNode;
            NodeIndex<PropertySetterNode> setterNode;
            int32 start = b.GetTokenIndex();

            if (b.GetTokenType() == TokenType::FatArrow) {
                if (TryParsePropertyGetter(DefinitionVisibility::Unspecified, &getterNode, needsRecovery)) {
                    *nodeIndex = b.DoneDeclaration(m, T(modifiers, typePath, input, getterNode, setterNode));
                    return true;
                }
                else {
                    b.InlineError("Expected an inlined getter");
                    *needsRecovery = true;
                    *nodeIndex = b.DoneDeclaration(m, T(modifiers, typePath, input, getterNode, setterNode));
                    return true;
                }
            }

            // float x {get; set;}

            PsiBuilder2::SubStream stream;
            if (b.TryGetSubSubStream_CurlyBracket(&stream)) {

                b.PushStream(&stream);

                if (stream.Empty()) {
                    b.InlineError("Expected at least one getter or setter");
                }
                else {

                    bool foundGetter = false;
                    bool foundSetter = false;

                    while (!b.EndOfInput()) {

                        DefinitionVisibility visibility;
                        TryParseVisibility(&visibility);

                        if (b.GetKeyword() == Keyword::Get) {

                            if (foundGetter) {
                                b.InlineError("duplicate getter");
                            }

                            foundGetter = true;

                            if (b.Peek(1).tokenType == TokenType::SemiColon) {
                                Marker g = b.Mark();
                                b.AdvanceLexer();
                                b.AdvanceLexer();
                                getterNode = b.Done(g, PropertyGetterNode(visibility));
                                continue;
                            }

                            TryParsePropertyGetter(visibility, &getterNode, needsRecovery);

                            if (*needsRecovery) {
                                // find 'set' or visibilty or end of block

                                while (!b.EndOfInput()) {
                                    switch (b.GetKeyword()) {
                                        case Keyword::Public:
                                        case Keyword::Export:
                                        case Keyword::Private:
                                        case Keyword::Protected:
                                        case Keyword::Internal:
                                        case Keyword::Get:
                                        case Keyword::Set: {
                                            *needsRecovery = false;
                                            break;
                                        }
                                        default: {
                                            b.AdvanceLexer();
                                        }
                                    }
                                }

                                if (*needsRecovery) {
                                    *nodeIndex = b.DoneDeclaration(m, T(modifiers, typePath, input, getterNode, setterNode));
                                    return true;
                                }

                            }

                        }

                        else if (b.GetKeyword() == Keyword::Set) {

                            if (foundSetter) {
                                b.InlineError("duplicate setter");
                            }

                            foundSetter = true;

                            if (b.Peek(1).tokenType == TokenType::SemiColon) {
                                Marker g = b.Mark();
                                b.AdvanceLexer();
                                b.AdvanceLexer();
                                setterNode = b.Done(g, PropertySetterNode(visibility));
                                continue;
                            }

                            TryParsePropertySetter(visibility, &setterNode, needsRecovery);

                            if (*needsRecovery) {
                                // find 'set' or visibilty or end of block

                                while (!b.EndOfInput()) {
                                    switch (b.GetKeyword()) {
                                        case Keyword::Public:
                                        case Keyword::Export:
                                        case Keyword::Private:
                                        case Keyword::Protected:
                                        case Keyword::Internal:
                                        case Keyword::Get:
                                        case Keyword::Set: {
                                            *needsRecovery = false;
                                            break;
                                        }
                                        default: {
                                            b.AdvanceLexer();
                                        }
                                    }
                                }

                                if (*needsRecovery) {
                                    *nodeIndex = b.DoneDeclaration(m, T(modifiers, typePath, input, getterNode, setterNode));
                                    return true;
                                }

                            }

                        }

                        else {

                            b.InlineError("Expected a getter or setter declaration");

                            while (!b.EndOfInput()) {
                                switch (b.GetKeyword()) {
                                    case Keyword::Public:
                                    case Keyword::Export:
                                    case Keyword::Private:
                                    case Keyword::Protected:
                                    case Keyword::Internal:
                                    case Keyword::Get:
                                    case Keyword::Set: {
                                        *needsRecovery = false;
                                        break;
                                    }
                                    default: {
                                        b.AdvanceLexer();
                                    }
                                }
                            }

                            if (*needsRecovery) {
                                *nodeIndex = b.DoneDeclaration(m, T(modifiers, typePath, input, getterNode, setterNode));
                                return true;
                            }

                        }

                    }

                }

                b.PopStream();
                *nodeIndex = b.DoneDeclaration(m, T(modifiers, typePath, input, getterNode, setterNode));
                return true;
            }
            else {
                assert(start == b.GetTokenIndex());
                return false;
            }
        }

        bool TryParseFieldDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<IdentifierNode> identifier, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // field_declaration ::= type_path identifier ('=' expression)? ';'

            NodeIndex<ExpressionNode> initExpression;

            if (b.GetTokenType() == TokenType::Assign) {
                b.AdvanceLexer();

                if (!TryParseExpression(&initExpression, needsRecovery)) {
                    b.InlineError("Expected an expression after field assignment");
                }

                // kick recovery up one level
                if (!*needsRecovery) {
                    ExpectSemicolon();
                }

                *nodeIndex = b.DoneDeclaration(m, FieldDeclarationNode(modifiers, typePath, identifier, initExpression));
                return true;
            }
            else if (b.GetTokenType() == TokenType::SemiColon) {
                b.AdvanceLexer();
                *nodeIndex = b.DoneDeclaration(m, FieldDeclarationNode(modifiers, typePath, identifier, initExpression));
                return true;
            }

            return false;

        }

        bool TryRecoverToColonOrCommaOrEnd(const char* error) {
            Marker e = b.Mark();

            while (!b.EndOfInput()) {

                TokenType tokenType = b.GetTokenType();

                if (tokenType == TokenType::Colon || tokenType == TokenType::Comma) {
                    b.Error(e, error);
                    return true;
                }

                b.AdvanceLexer();

            }

            b.Error(e, error);
            return false;
        }

        bool TryRecoverToCommaOrEnd(const char* error) {

            Marker e = b.Mark();

            while (!b.EndOfInput()) {

                TokenType tokenType = b.GetTokenType();

                if (tokenType == TokenType::Comma) {
                    b.Error(e, error);
                    return true;
                }

                b.AdvanceLexer();

            }

            b.Error(e, error);
            return false;

        }

        bool TryParseFormalParameterWithRecovery(NodeIndex<FormalParameterNode>* nodeIndex, bool allowDefaultValue) {
            // formal_parameter ::= ('temp' | 'scoped')? ('ref' | 'out')? type_path non_keyword_identifier ('=' expression)?

            // we always recover to , or end of stream

            StorageClass storage = StorageClass::Default;
            ArgumentPassByModifier passByModifier = ArgumentPassByModifier::None;
            NodeIndex<IdentifierNode> identifier;
            NodeIndex<ExpressionNode> defaultValue;

            int32 lexPosition = b.GetTokenIndex();

            {
                Keyword keyword = b.GetKeyword();
                if (keyword == Keyword::Temp || keyword == Keyword::Scoped) {
                    storage = keyword == Keyword::Temp ? StorageClass::Temp : StorageClass::Scoped;
                    b.AdvanceLexer();
                    keyword = b.GetKeyword();
                }

                if (keyword == Keyword::Ref || keyword == Keyword::Out) {
                    passByModifier = keyword == Keyword::Ref ? ArgumentPassByModifier::Ref : ArgumentPassByModifier::Out;
                    b.AdvanceLexer();
                }
            }

            NodeIndex<TypePathNode> typePath;
            bool localRecovery = false;
            if (!TryParseTypePath(&typePath, &localRecovery)) {

                if (lexPosition != b.GetTokenIndex()) {
                    Marker m2 = b.MarkFromToken(lexPosition);
                    TryRecoverToCommaOrEnd("Expected a parameter type");
                    *nodeIndex = b.Done(m2, FormalParameterNode(storage, passByModifier, typePath, identifier, defaultValue));
                    return true;
                }
                else {
                    // no lexer input
                    return false;
                }

            }

            Marker m = b.MarkFromToken(lexPosition);

            if (!TryParseIdentifier(&identifier, true)) {
                TryRecoverToCommaOrEnd("Expected parameter to have a name");
                *nodeIndex = b.Done(m, FormalParameterNode(storage, passByModifier, typePath, identifier, defaultValue));
                return true;
            }

            if (b.GetTokenType() == TokenType::Assign) {

                if (!allowDefaultValue) {
                    b.InlineError("Default values are not allowed in lambda parameters");
                }

                b.AdvanceLexer();

                bool expressionRecovery = false;
                if (!TryParseExpression(&defaultValue, &expressionRecovery)) {
                    TryRecoverToCommaOrEnd("Expected a default value expression after `=` when defining a parameter");
                }

            }

            *nodeIndex = b.Done(m, FormalParameterNode(storage, passByModifier, typePath, identifier, defaultValue));
            return true;

        }

        static bool TryParseFormalParameterWithRecoveryFn(ParserImpl* parser, NodeIndex<FormalParameterNode>* first) {
            return parser->TryParseFormalParameterWithRecovery(first, false);
        }

        static bool TryParseFormalParameterWithRecoveryFnWithDefaultValues(ParserImpl* parser, NodeIndex<FormalParameterNode>* first) {
            return parser->TryParseFormalParameterWithRecovery(first, true);
        }

        bool TryParseFormalParameterList(NodeIndex<FormalParameterListNode>* nodeIndex, bool allowDefaultValues) {
            // formal_parameter_list ::= '(' (formal_parameter (',' formal_parameter)*)? ')'

            if (b.GetTokenType() != TokenType::OpenParen) {
                return false;
            }

            Marker m = b.Mark();

            PsiBuilder2::SubStream subStream;
            b.TryGetSubSubStream_Paren(&subStream);

            b.PushStream(&subStream);

            NodeIndex<FormalParameterNode> first;
            ParseCommaSeparatedList(&first, false, allowDefaultValues ? &TryParseFormalParameterWithRecoveryFnWithDefaultValues : &TryParseFormalParameterWithRecoveryFn);
//
//            NodeIndex<FormalParameterNode> next;
//            NodeIndex<FormalParameterNode> ptr;
//
//            if (!subStream.Empty()) {
//                while (!b.EndOfInput()) {
//
//                    // if we fail to get the first parameter we skip until we can get it
//                    if (TryParseFormalParameterWithRecovery(&first, allowDefaultValues)) {
//                        break;
//                    }
//
//                }
//
//                while (!b.EndOfInput()) {
//
//                    if (b.GetTokenType() == TokenType::Comma) {
//
//                        b.AdvanceLexer();
//                        if (b.EndOfInput()) {
//                            b.InlineError("Trailing comma in formal parameter list is not allowed");
//                            break;
//                        }
//
//                        if (TryParseFormalParameterWithRecovery(&next, allowDefaultValues)) {
//                            b.GetPsiNodeUnsafe(ptr)->next = next;
//                            ptr = next;
//                        }
//
//                    }
//
//                    else {
//                        TryRecoverToCommaOrEnd("Expected a parameter definition, you might be missing a comma");
//                    }
//
//                }
//
//            }

            b.PopStream();
            *nodeIndex = b.Done(m, FormalParameterListNode(first));
            return true;
        }

        bool TryParseExpressionList(NodeIndex<ExpressionListNode>* nodeIndex) {
            // expression_list ::= '(' expression (',' expression)* ')'

            if (b.GetTokenType() != TokenType::OpenParen) {
                return false;
            }

            Marker m = b.Mark();
            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_Paren(&stream);
            b.PushStream(&stream);
            NodeIndex<ExpressionNode> first;

            ParseCommaSeparatedList(&first, false, &TryParseExpressionWithRecoveryFn);

            b.PopStream();
            *nodeIndex = b.Done(m, ExpressionListNode(first));
            return true;
        }

        static bool IsEmbeddedStatementKeyword(Keyword keyword) {
            switch (keyword) {
                case Keyword::While:
                case Keyword::Do:
                case Keyword::For:
                case Keyword::Foreach:
                case Keyword::Switch:
                case Keyword::Try:
                case Keyword::Break:
                case Keyword::Continue:
                case Keyword::Return:
                case Keyword::Throw:
                case Keyword::With:
                case Keyword::Using:
                case Keyword::If:
                    return true;
                default:
                    return false;
            }
        }

        static bool TryParseIdentifierFn(ParserImpl* parser, NodeIndex<IdentifierNode>* identifier) {
            return parser->TryParseIdentifier(identifier, true);
        }

        bool TryParseContextList(NodeIndex<ContextListNode>* nodeIndex, bool* needsRecovery) {
            // context_list ::= '|' identifier, (',' identifier)* '|'
            if (b.GetTokenType() != TokenType::BinaryOr) {
                return false;
            }

            Marker m = b.Mark();
            bool validStream = true;
            NodeIndex<IdentifierNode> first;

            int32 start = b.GetTokenIndex();
            PsiBuilder2::SubStream stream;
            if (b.TryGetSubSubStream(&stream, TokenType::BinaryOr, TokenType::BinaryOr)) {

                // make sure we only have , and non keyword identifiers in the stream
                // otherwise we may have matched way too far
                for (int32 i = stream.tokenStart; i < stream.tokenEnd; i++) {
                    Token t = b.GetTokenAt(i);
                    if (t.tokenType != TokenType::KeywordOrIdentifier && t.tokenType != TokenType::Comma) {
                        validStream = false;
                        b.SetTokenIndex(start);
                        break;
                    }
                }

            }
            else {
                validStream = false;
            }

            if (validStream) {

                b.PushStream(&stream);

                if (stream.Empty()) {
                    b.InlineError("Expected context list to contain at least one identifier");
                }
                else {
                    ParseCommaSeparatedList(&first, false, &TryParseIdentifierFn);
                }

                b.PopStream();

            }
            else {

                // todo -- parse identifiers & , until we hit something that isn't an id or comma

                b.AdvanceLexer(); // step over | since we reset

                if (TryParseIdentifier(&first, true)) {

                    NodeIndex<IdentifierNode> next;
                    NodeIndex<IdentifierNode> ptr = first;

                    while (b.GetTokenType() == TokenType::Comma) {
                        b.AdvanceLexer();
                        if (TryParseIdentifier(&next, true)) {
                            b.GetPsiNodeUnsafe(ptr)->next = next;
                            ptr = next;
                        }
                        else {
                            break;
                        }
                    }

                }

                // we are being careful not to match a | operator and consuming way too many characters
                b.InlineError("Expected a context list enclosed in pipes `|` containing only identifiers and commas");
                *needsRecovery = true;
            }

            *nodeIndex = b.Done(m, ContextListNode(first));
            return true;

        }

        bool TryParseIfStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // if_statement ::= 'if' '(' expression_list ')' context_list? if_body ('else' if_body)?
            // if_body ::= block | embedded_statement

            if (b.GetKeyword() != Keyword::If) {
                return false;
            }

            Marker m = b.Mark();

            b.AdvanceLexer();

            NodeIndex<ExpressionListNode> conditions;
            NodeIndex<ContextListNode> context;
            NodeIndex<StatementNode> body;
            NodeIndex<StatementNode> elseCase;

            if (!TryParseExpressionList(&conditions)) {
                b.InlineError("Expected an open paren after 'if'");
                // pipe, embedded_statement_keyword, open brace
                TokenType tokenType = b.GetTokenType();
                *needsRecovery = tokenType != TokenType::BinaryOr && tokenType != TokenType::CurlyBraceOpen && !IsEmbeddedStatementKeyword(b.GetKeyword());
            }

            if (*needsRecovery) {
                *nodeIndex = b.DoneStatement(m, IfStatementNode(conditions, context, body, elseCase));
                return true;
            }

            TryParseContextList(&context, needsRecovery);

            if (*needsRecovery) {
                *needsRecovery = b.GetTokenType() != TokenType::CurlyBraceOpen && !IsEmbeddedStatementKeyword(b.GetKeyword());

                if (*needsRecovery) {
                    *nodeIndex = b.DoneStatement(m, IfStatementNode(conditions, context, body, elseCase));
                    return true;
                }

            }

            if (!TryParseEmbeddedStatement(&body, needsRecovery)) {
                *needsRecovery = true;
                b.InlineError("Expected a block or statement following if statement header");
                *nodeIndex = b.DoneStatement(m, IfStatementNode(conditions, context, body, elseCase));
                return true;
            }

            if (b.GetKeyword() == Keyword::Else) {
                *needsRecovery = false;
                b.AdvanceLexer();

                if (!TryParseEmbeddedStatement(&elseCase, needsRecovery)) {
                    b.InlineError("Expected a block or embedded statement following 'else'");
                    *needsRecovery = true;
                }
            }

            *nodeIndex = b.DoneStatement(m, IfStatementNode(conditions, context, body, elseCase));
            return true;
        }

        bool TryRecoverToTokens(TokenType* tokens, int32 cnt) {
            int32 location = b.GetTokenIndex();

            while (!b.EndOfInput()) {

                TokenType tokenType = b.GetTokenType();

                for (int32 i = 0; i < cnt; i++) {
                    if (tokens[i] == tokenType) {
                        return true;
                    }
                }

                b.AdvanceLexer();
            }

            // if we never found it, we can't recover
            return false;
        }

        bool SwitchLabelRecovery() {
            Marker e = b.Mark();

            while (!b.EndOfInput()) {

                TokenType t = b.GetTokenType();
                if (t == TokenType::Colon || t == TokenType::CurlyBraceOpen) {
                    b.Error(e, "unexpected token");
                    return true;
                }

                if (t == TokenType::KeywordOrIdentifier) {
                    switch (b.GetKeyword()) {
                        case Keyword::Case:
                        case Keyword::Default:
                        case Keyword::When:
                            b.Error(e, "unexpected token");
                            return true;
                        default:
                            break;
                    }
                }

                b.AdvanceLexer();
            }

            b.Error(e, "unexpected token");
            return false;
        }

        bool TryParseSwitchStatementSectionLabel(NodeIndex<SwitchSectionLabelNode>* nodeIndex) {
            // switch_section_label ::= ('case' expression case_guard? ':') | ('default' ':')
            // case_guard ::= 'when' expression

            Keyword keyword = b.GetKeyword();
            if (keyword != Keyword::Case && keyword != Keyword::Default) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();
            NodeIndex<ExpressionNode> expression;
            NodeIndex<ExpressionNode> caseGuard;

            // recover to :, {, case, default, when
            // stop if we hit a statement or type keyword

            if (keyword == Keyword::Default) {

                if (b.GetTokenType() == TokenType::Colon) {
                    b.AdvanceLexer();
                }
                else {
                    SwitchLabelRecovery();
                }

                *nodeIndex = b.Done(m, SwitchSectionLabelNode(true));
                return true;

            }

            // otherwise it's a 'case'

            bool recovery = false;
            if (!TryParseNonAssignmentExpression(&expression, &recovery)) {
                b.InlineError("Expected an expression after 'case'");
                recovery = true;
            }

            if (recovery) {
                SwitchLabelRecovery();
                recovery = false;
            }

            if (b.GetKeyword() == Keyword::When) {

                b.AdvanceLexer();

                if (!TryParseExpression(&caseGuard, &recovery)) {
                    b.InlineError("Expected an expression following 'when' in switch statement label");
                    recovery = true;
                }

                if (recovery) {
                    SwitchLabelRecovery();
                    recovery = false;
                }

            }

            if (b.GetTokenType() != TokenType::Colon) {
                b.InlineError("Expected a ':' after switch label");
                SwitchLabelRecovery();
            }
            else {
                b.AdvanceLexer();
            }

            *nodeIndex = b.Done(m, SwitchSectionLabelNode(expression, caseGuard));
            return true;

        }

        bool TryParseSwitchStatementSections(NodeIndex<SwitchSectionNode>* nodeIndex) {
            // switch_statement_section ::= (switch_label+ statement_list)*
            Keyword keyword = b.GetKeyword();
            if (keyword != Keyword::Case && keyword != Keyword::Default) {
                return false;
            }

            NodeIndex<SwitchSectionLabelNode> firstLabel;
            NodeIndex<StatementNode> firstStatement;

            Marker m = b.Mark();

            // can't fail, but not optional (we already checked the keywords)
            TryParseSwitchStatementSectionLabel(&firstLabel);

            NodeIndex<SwitchSectionLabelNode> ptr = firstLabel;
            NodeIndex<SwitchSectionLabelNode> next;

            while (TryParseSwitchStatementSectionLabel(&next)) {
                b.GetPsiNodeUnsafe(ptr)->next = next;
                ptr = next;
            }

            bool recovery = false;
            if (!TryParseEmbeddedStatement(&firstStatement, &recovery)) {
                b.InlineError("Expected a body block or statement for switch case");
                recovery = true;
            }

            if (recovery) {
                SwitchLabelRecovery();
            }

            *nodeIndex = b.Done(m, SwitchSectionNode(firstLabel, firstStatement));
            return true;

        }

        bool TryParseSwitchStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // switch_statement ::= 'switch' '(' expression ')' context_list? '{' switch_sections* '}'
            if (b.GetKeyword() != Keyword::Switch) {
                return false;
            }
            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<SwitchSectionNode> firstSection;
            NodeIndex<ContextListNode> contextList;
            NodeIndex<ExpressionNode> condition;

            PsiBuilder2::SubStream stream;
            if (!b.TryGetSubSubStream_Paren(&stream)) {
                b.InlineError("Expected an open paren after 'switch'");
                *needsRecovery = true;
                *nodeIndex = b.DoneStatement(m, SwitchStatementNode(condition, contextList, firstSection));
                return true;
            }

            b.PushStream(&stream);

            if (!TryParseExpression(&condition, needsRecovery)) {
                b.InlineError("Expected a condition inside parens after 'switch' statement");
            }

            b.PopStream();

            TryParseContextList(&contextList, needsRecovery);

            if (*needsRecovery) {
                *needsRecovery = b.GetTokenType() != TokenType::CurlyBraceOpen;
                if (*needsRecovery) {
                    *nodeIndex = b.DoneStatement(m, SwitchStatementNode(condition, contextList, firstSection));
                    return true;
                }
            }

            if (!b.TryGetSubSubStream_CurlyBracket(&stream)) {
                *needsRecovery = true;
                b.InlineError("Expected an opening curly brace before `switch` statement sections");
                *nodeIndex = b.DoneStatement(m, SwitchStatementNode(condition, contextList, firstSection));
                return true;
            }

            b.PushStream(&stream);

            TryParseSwitchStatementSections(&firstSection);

            b.PopStream();

            *nodeIndex = b.DoneStatement(m, SwitchStatementNode(condition, contextList, firstSection));
            return true;

        }

        // no support for context because don't execute conditions until the end of the block
        bool TryParseDoWhileStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // do_while_statement ::= 'do' embedded_statement 'while' (' expression ')'

            if (b.GetKeyword() != Keyword::Do) {
                return false;
            }
            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<StatementNode> body;
            NodeIndex<ExpressionNode> condition;

            if (!TryParseEmbeddedStatement(&body, needsRecovery)) {
                b.InlineError("Expected a body after 'do'");
                *needsRecovery = b.GetKeyword() != Keyword::While;
            }

            if (*needsRecovery) {
                *nodeIndex = b.DoneStatement(m, DoWhileStatementNode(body, condition));
                return true;
            }

            if (b.GetKeyword() != Keyword::While) {
                b.InlineError("Expected a `while` keyword to match 'do'");
                *needsRecovery = true;
                *nodeIndex = b.DoneStatement(m, DoWhileStatementNode(body, condition));
                return true;
            }
            b.AdvanceLexer();

            PsiBuilder2::SubStream stream;

            if (!b.TryGetSubSubStream_Paren(&stream)) {
                b.InlineError("Expected an open paren after do statement body");
                *needsRecovery = true;
                *nodeIndex = b.DoneStatement(m, DoWhileStatementNode(body, condition));
                return true;
            }

            b.PushStream(&stream);

            if (!TryParseExpression(&condition, needsRecovery)) {
                b.InlineError("Expected a condition expression in the do statement tail");
            }

            b.PopStream();

            *nodeIndex = b.DoneStatement(m, DoWhileStatementNode(body, condition));
            return true;

        }

        bool TryParseWhileStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // while_statement ::= 'while' '(' expression_list ')' context_list? embedded_statement
            if (b.GetKeyword() != Keyword::While) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<ExpressionListNode> conditions;
            NodeIndex<ContextListNode> context;
            NodeIndex<StatementNode> body;


            if (!TryParseExpressionList(&conditions)) {
                b.InlineError("Expected a condition after open paren in 'while' statement");
                *needsRecovery = true;
                *nodeIndex = b.DoneStatement(m, WhileStatementNode(conditions, context, body));
                return true;
            }

            bool recovery = false;
            TryParseContextList(&context, &recovery);


            if (b.GetTokenType() == TokenType::CurlyBraceOpen || IsEmbeddedStatementKeyword(b.GetKeyword())) {
                recovery = false;
            }

            if (recovery) {
                *needsRecovery = true;
                *nodeIndex = b.DoneStatement(m, WhileStatementNode(conditions, context, body));
                return true;
            }

            if (!TryParseEmbeddedStatement(&body, needsRecovery)) {
                b.InlineError("Expected a block or embedded statement following while statement header");
                *needsRecovery = true;
            }

            *nodeIndex = b.DoneStatement(m, WhileStatementNode(conditions, context, body));
            return true;
        }

        bool TryParseForEachLoopStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // foreach_loop_statement ::= 'foreach' '(' expression ')' context_list? embedded_statement
            if (b.GetKeyword() != Keyword::Foreach) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<ContextListNode> context;
            NodeIndex<ExpressionNode> expression;
            NodeIndex<StatementNode> body;

            PsiBuilder2::SubStream stream;

            if (!b.TryGetSubSubStream_Paren(&stream)) {
                b.InlineError("Expected an open paren after 'foreach'");
                *needsRecovery = true;
                *nodeIndex = b.DoneStatement(m, ForEachLoopStatementNode(expression, context, body));
                return true;
            }

            b.PushStream(&stream);

            if (!TryParseExpression(&expression, needsRecovery)) {
                b.InlineError("Expected an expression inside parens after 'foreach'");
            }

            b.PopStream();

            TryParseContextList(&context, needsRecovery);

            if (*needsRecovery) {
                *needsRecovery = b.GetTokenType() != TokenType::CurlyBraceOpen && !IsEmbeddedStatementKeyword(b.GetKeyword());
                if (*needsRecovery) {
                    *nodeIndex = b.DoneStatement(m, ForEachLoopStatementNode(expression, context, body));
                    return true;
                }
            }

            if (!TryParseEmbeddedStatement(&body, needsRecovery)) {
                b.InlineError("Expected  loop body after foreach");
                *needsRecovery = true;
            }

            *nodeIndex = b.DoneStatement(m, ForEachLoopStatementNode(expression, context, body));
            return true;

        }

        bool TryParseStorageClass(StorageClass* storageClass) {
            // storage_class ::= 'temp' | 'scoped'

            VariableTypeModifier modifier;
            if (b.GetKeyword() == Keyword::Temp) {
                b.AdvanceLexer();
                *storageClass = StorageClass::Temp;
                return true;
            }
            else if (b.GetKeyword() == Keyword::Scoped) {
                b.AdvanceLexer();
                *storageClass = StorageClass::Scoped;
                return true;
            }
            else {
                *storageClass = StorageClass::Default;
                return false;
            }
        }

        bool TryParseLocalVariableType(NodeIndex<LocalVariableTypeNode>* nodeIndex, bool* needsRecovery) {
            // local_variable_type ::= storage_class? 'ref'? 'var' | type_path

            Marker m = b.Mark();

            StorageClass storageClass;

            TryParseStorageClass(&storageClass);

            bool isRef = false;
            if (b.GetKeyword() == Keyword::Ref) {
                b.AdvanceLexer();
                isRef = true;
            }

            NodeIndex<TypePathNode> typePath;

            if (b.GetKeyword() == Keyword::Var) {
                b.AdvanceLexer();
                *nodeIndex = b.Done(m, LocalVariableTypeNode(storageClass, isRef, true, typePath));
                return true;
            }

            if (TryParseTypePath(&typePath, needsRecovery)) {
                Marker typePathMark = b.GetMarker(typePath.index);
                *nodeIndex = b.Done(m, LocalVariableTypeNode(storageClass, isRef, false, typePath));
                return true;
            }

            b.RollbackTo(m);
            return false;

        }

        // todo -- maybe handle array initializer, but don't let it conflict with dynamic {} or dynamic []
        bool TryParseLocalVariableDeclarator(NodeIndex<LocalVariableInitializerNode>* nodeIndex, bool* needsRecovery) {
            // local_variable_declarator ::= '=' 'ref'? expression

            if (b.GetTokenType() != TokenType::Assign) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            bool isByRef = false;
            if (b.GetKeyword() == Keyword::Ref) {
                b.AdvanceLexer();
                isByRef = true;
            }

            NodeIndex<ExpressionNode> expression;
            if (!TryParseExpression(&expression, needsRecovery)) {
                b.InlineError("Expected an expression after variable initializer");
                *needsRecovery = true;
            }

            *nodeIndex = b.Done(m, LocalVariableInitializerNode(isByRef, expression));
            return true;

        }

        bool TryParseLocalVariableDeclaration(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // local_variable_declaration ::= local_variable_type identifier local_variable_declarator?

            Marker m = b.Mark();

            NodeIndex<LocalVariableTypeNode> variableType;
            NodeIndex<IdentifierNode> identifier;
            NodeIndex<LocalVariableInitializerNode> initializer;

            if (!TryParseLocalVariableType(&variableType, needsRecovery)) {
                b.RollbackTo(m);
                return false;
            }

            if (!TryParseIdentifier(&identifier, true)) {
                b.InlineError("Expected a variable name");
            }

            TryParseLocalVariableDeclarator(&initializer, needsRecovery);

            *nodeIndex = b.DoneStatement(m, LocalVariableDeclarationNode(variableType, identifier, initializer));
            return true;

        }

        bool TryParseConstantDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // constant_declaration :: = 'const' type_path identifier '=' expression
            if (b.GetKeyword() != Keyword::Const) {
                return false;
            }

            b.AdvanceLexer();

            NodeIndex<TypePathNode> typePath;
            NodeIndex<IdentifierNode> identifier;
            NodeIndex<ExpressionNode> expression;

            if (!TryParseTypePath(&typePath, needsRecovery)) {
                b.InlineError("Expected a type name");
            }

            if (!TryParseIdentifier(&identifier, true)) {
                b.InlineError("Expected an identifier");
            }

            if (b.GetTokenType() != TokenType::Assign) {
                b.InlineError("Expected an equal sign");
            }
            else {
                b.AdvanceLexer();
            }

            if (!TryParseExpression(&expression, needsRecovery)) {
                b.InlineError("Expected an expression");
            }

            *nodeIndex = b.DoneDeclaration(m, ConstantDeclarationNode(modifiers, typePath, identifier, expression));
            return true;
        }

        bool TryParseLocalConstantDeclaration(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // constant_declaration :: = 'const' type_path identifier '=' expression
            if (b.GetKeyword() != Keyword::Const) {
                return false;
            }
            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<TypePathNode> typePath;
            NodeIndex<IdentifierNode> identifier;
            NodeIndex<ExpressionNode> expression;

            if (!TryParseTypePath(&typePath, needsRecovery)) {
                b.InlineError("Expected a type name");
            }

            if (!TryParseIdentifier(&identifier, true)) {
                b.InlineError("Expected an identifier");
            }

            if (b.GetTokenType() != TokenType::Assign) {
                b.InlineError("Expected an equal sign");
            }
            else {
                b.AdvanceLexer();
            }

            if (!TryParseExpression(&expression, needsRecovery)) {
                b.InlineError("Expected an expression");
            }

            *nodeIndex = b.DoneStatement(m, LocalConstantDeclarationNode(typePath, identifier, expression));
            return true;
        }

        bool TryParseDeclarationStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // declaration_statement ::= local_variable_declaration ';' | local_constant_declaration ';' | local_remember_declaration ';'

            if (TryParseLocalConstantDeclaration(nodeIndex, needsRecovery)) {

                if (b.GetTokenType() == TokenType::SemiColon) {
                    *needsRecovery = false;
                }
                ExpectSemicolon();
                return true;
            }

            if (TryParseLocalVariableDeclaration(nodeIndex, needsRecovery)) {

                if (b.GetTokenType() == TokenType::SemiColon) {
                    *needsRecovery = false;
                }

                ExpectSemicolon();
                return true;
            }

            return false;
        }

        bool TryParseForInitializer(NodeIndex<ForInitializerNode>* nodeIndex, bool* needsRecovery) {
            // for_initializer ::= local_variable_declaration | expression

            Marker m = b.Mark();
            NodeIndex<StatementNode> variableDeclaration;
            if (TryParseLocalVariableDeclaration(&variableDeclaration, needsRecovery)) {
                *nodeIndex = b.Done(m, ForInitializerNode(variableDeclaration));
                return true;
            }

            NodeIndex<ExpressionNode> expression;
            if (TryParseExpression(&expression, needsRecovery)) {
                *nodeIndex = b.Done(m, ForInitializerNode(expression));
                return true;
            }

            b.RollbackTo(m);
            return false;

        }

        static bool TryParseForInitializerFn(ParserImpl* parser, NodeIndex<ForInitializerNode>* first) {
            bool recover = false; // ignore errors since we're limited to the paren stream anywy
            return parser->TryParseForInitializer(first, &recover);
        }

        bool TryParseForInitializerList(NodeIndex<ForInitializerListNode>* nodeIndex) {
            // for_initializer_list ::= for_initializer (',' for_initializer)

            Marker m = b.Mark();

            NodeIndex<ForInitializerNode> first;

            ParseCommaSeparatedList(&first, false, &TryParseForInitializerFn);

            *nodeIndex = b.Done(m, ForInitializerListNode(first));

            return true;
        }

        bool TryParseForLoopStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // for_loop_statement ::= 'for' '(' for_initializer_list? ';' expression? ';' for_iterator? ')' embedded_statement
            // for_iterator ::= (expression (',' expression)*) ?

            if (b.GetKeyword() != Keyword::For) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<ExpressionListNode> initializerExpressions;
            NodeIndex<LocalVariableDeclarationNode> variableNode;
            NodeIndex<ForInitializerListNode> initializer;
            NodeIndex<ExpressionNode> condition;
            NodeIndex<ForLoopIteratorNode> iterator;
            NodeIndex<StatementNode> body;

            PsiBuilder2::SubStream stream;

            if (!b.TryGetSubSubStream_Paren(&stream)) {
                b.InlineError("Expected an open paren after 'for'");
                *needsRecovery = true;
                *nodeIndex = b.DoneStatement(m, ForLoopStatementNode(initializer, condition, iterator, body));
                return true;
            }

            b.PushStream(&stream);

            TryParseForInitializerList(&initializer);

            ExpectSemicolon();

            {
                bool recovery = false;
                TryParseExpression(&condition, &recovery); // optional

            }

            ExpectSemicolon();

            if (!b.EndOfInput()) {
                bool recovery = false;
                Marker x = b.Mark();
                NodeIndex<ExpressionNode> first;
                ParseCommaSeparatedList(&first, false, &TryParseExpressionWithRecoveryFn);
                iterator = b.Done(x, ForLoopIteratorNode(first));
            }

            b.PopStream();

            if (!TryParseEmbeddedStatement(&body, needsRecovery)) {
                b.InlineError("Expected a body after for loop header");
            }

            *nodeIndex = b.DoneStatement(m, ForLoopStatementNode(initializer, condition, iterator, body));
            return true;
        }

        bool TryParseCatchClause(NodeIndex<CatchClauseNode>* nodeIndex, bool* needsRecovery) {
            // catch_clause ::= 'catch' context_list? block

            if (b.GetKeyword() != Keyword::Catch) {
                return false;
            }

            Marker m = b.Mark();

            b.AdvanceLexer();

            NodeIndex<ContextListNode> contextList;
            NodeIndex<BlockNode> blockNode;

            TryParseContextList(&contextList, needsRecovery);

            if (*needsRecovery) {
                *needsRecovery = b.GetTokenType() != TokenType::CurlyBraceOpen;
                if (*needsRecovery) {
                    *nodeIndex = b.Done(m, CatchClauseNode(contextList, blockNode));
                    return true;
                }
            }

            if (!TryParseBlock(&blockNode)) {
                b.InlineError("Expected a block following 'catch' ");
                *needsRecovery = true;
            }

            *nodeIndex = b.Done(m, CatchClauseNode(contextList, blockNode));
            return true;
        }

        bool TryParseTryStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // try_statement:: = 'try' (expression catch_clause? ';' | block catch_clause? ('finally' block)? )

            if (b.GetKeyword() != Keyword::Try) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<ExpressionNode> expression;
            NodeIndex<BlockNode> block;
            NodeIndex<CatchClauseNode> catchClause;
            NodeIndex<BlockNode> finallyBlock;

            if (TryParseBlock(&block)) {

                TryParseCatchClause(&catchClause, needsRecovery);

                if (b.GetKeyword() == Keyword::Finally) {
                    *needsRecovery = false;
                    b.AdvanceLexer();

                    TryParseBlock(&finallyBlock);

                }

                *nodeIndex = b.DoneStatement(m, TryBlockNode(block, catchClause, finallyBlock));
                return true;
            }
            else if (TryParseExpression(&expression, needsRecovery)) {

                TryParseCatchClause(&catchClause, needsRecovery);

                if (*needsRecovery) {
                    *needsRecovery = b.GetTokenType() != TokenType::SemiColon;
                }

                ExpectSemicolon();

                *nodeIndex = b.DoneStatement(m, TryExpressionNode(expression, catchClause));
                return true;

            }
            else {
                b.Error(m, "Expected a block or expression following 'try'");
                return true;
            }

        }

        bool TryParseSimpleEmbeddedStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // simple_embedded_statement ::= ';'
            // | expression
            // | if_statement
            // | switch_statement
            // | while_statement
            // | do_while_statement
            // | for_loop_statement
            // | foreach_loop_statement
            // | break_statement
            // | continue_statement
            // | return_statement
            // | throw_statement
            // | try_block
            // | using_statement

            TokenType tokenType = b.GetTokenType();
            if (tokenType == TokenType::SemiColon) {
                Marker m = b.Mark();
                b.AdvanceLexer();
                *nodeIndex = b.DoneStatement(m, EmptyStatementNode());
                return true;
            }

            if (tokenType == TokenType::KeywordOrIdentifier) {
                switch (b.GetKeyword()) {
                    case Keyword::If: {
                        return TryParseIfStatement(nodeIndex, needsRecovery);
                    }
                    case Keyword::Switch: {
                        return TryParseSwitchStatement(nodeIndex, needsRecovery);
                    }
                    case Keyword::While: {
                        return TryParseWhileStatement(nodeIndex, needsRecovery);
                    }
                    case Keyword::Do: {
                        return TryParseDoWhileStatement(nodeIndex, needsRecovery);
                    }
                    case Keyword::For: {
                        return TryParseForLoopStatement(nodeIndex, needsRecovery);
                    }
                    case Keyword::Foreach: {
                        return TryParseForEachLoopStatement(nodeIndex, needsRecovery);
                    }
                    case Keyword::Try: {
                        return TryParseTryStatement(nodeIndex, needsRecovery);
                    }
                    case Keyword::Break: {
                        // break_statement ::= 'break' ';'
                        Marker m = b.Mark();
                        b.AdvanceLexer();
                        *needsRecovery = !ExpectSemicolon();
                        *nodeIndex = b.DoneStatement(m, BreakStatementNode());
                        return true;
                    }
                    case Keyword::Continue: {
                        // continue_statement ::= 'continue' ';'
                        Marker m = b.Mark();
                        b.AdvanceLexer();
                        *needsRecovery = !ExpectSemicolon();
                        *nodeIndex = b.DoneStatement(m, ContinueStatementNode());
                        return true;
                    }
                    case Keyword::Return: {
                        // return_statement ::= 'return' expression? ';'
                        Marker m = b.Mark();
                        b.AdvanceLexer();
                        NodeIndex<ExpressionNode> expression;
                        TryParseExpression(&expression, needsRecovery);
                        if (b.GetTokenType() == TokenType::SemiColon) {
                            *needsRecovery = false;
                            b.AdvanceLexer();
                        }
                        else if (!*needsRecovery) {
                            ExpectSemicolon();
                        }
                        *nodeIndex = b.DoneStatement(m, ReturnStatementNode(expression));
                        return true;
                    }
                    case Keyword::Throw: {
                        // throw_statement ::= 'throw' expression ';'

                        Marker m = b.Mark();
                        b.AdvanceLexer();
                        NodeIndex<ExpressionNode> expression;

                        if (!TryParseExpression(&expression, needsRecovery)) {
                            b.InlineError("Expected an expression following 'throw'");
                            *needsRecovery = true;
                        }

                        if (b.GetTokenType() == TokenType::SemiColon) {
                            *needsRecovery = false;
                            b.AdvanceLexer();
                        }
                        else if (!*needsRecovery) {
                            ExpectSemicolon();
                        }

                        *nodeIndex = b.DoneStatement(m, ThrowStatementNode(expression));
                        return true;

                    }

                    case Keyword::With: {
                        return TryParseWithStatement(nodeIndex, needsRecovery);
                    }
                    case Keyword::Using: {
                        return TryParseUsingStatement(nodeIndex, needsRecovery);
                    }
                    default: {
                        break;
                    }
                }
            }

            return TryParseExpressionStatement(nodeIndex, needsRecovery);

        }

        bool TryParseWithStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // with_statement ::= 'with' expression_list context_list? block

            if (b.GetKeyword() != Keyword::With) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<BlockNode> block;
            NodeIndex<ExpressionListNode> expressionList;
            NodeIndex<ContextListNode> contextList;

            if (TryParseExpressionList(&expressionList)) {

                bool recover = false;
                TryParseContextList(&contextList, &recover);

                if (recover && b.GetTokenType() != TokenType::CurlyBraceOpen) {
                    *needsRecovery = true;
                    *nodeIndex = b.DoneStatement(m, UsingStatementBlockNode(expressionList, contextList, block));
                    return true;
                }

                if (!TryParseBlock(&block)) {
                    b.InlineError("Expected a block after 'with' statement");
                    *needsRecovery = true;
                }

            }
            else {
                b.InlineError("Expected an expression in parens after 'with'");
                *needsRecovery = true;
            }

            *nodeIndex = b.DoneStatement(m, WithStatementNode(expressionList, contextList, block));
            return true;
        }

        bool TryParseUsingStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // using_statement ::= 'using' (block_using | scope_using)
            // block_using ::= '(' expression ')' context_list? block
            // scoped_using ::= expression ';'

            if (b.GetKeyword() != Keyword::Using) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<ExpressionNode> expression;

            NodeIndex<BlockNode> block;
            NodeIndex<ExpressionListNode> expressionList;
            NodeIndex<ContextListNode> contextList;

            if (TryParseExpressionList(&expressionList)) {

                bool recover = false;
                TryParseContextList(&contextList, &recover);

                if (recover && b.GetTokenType() != TokenType::CurlyBraceOpen) {
                    *needsRecovery = true;
                    *nodeIndex = b.DoneStatement(m, UsingStatementBlockNode(expressionList, contextList, block));
                    return true;
                }

                if (!TryParseBlock(&block)) {
                    b.InlineError("Expected a block after 'using' statement");
                    *needsRecovery = true;
                }

                *nodeIndex = b.DoneStatement(m, UsingStatementBlockNode(expressionList, contextList, block));
                return true;
            }


            if (!TryParseExpression(&expression, needsRecovery)) {
                b.InlineError("Expected an expression");
            }

            ExpectSemicolon();
            *nodeIndex = b.DoneStatement(m, UsingStatementScopeNode(expression));
            return true;
        }

        bool TryParseExpressionStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // wrapper for expression to chain them

            NodeIndex<ExpressionNode> expression;

            Marker m = b.Mark();

            if (!TryParseExpression(&expression, needsRecovery)) {
                b.RollbackTo(m);
                return false;
            }

            // if we want to have trailing lambdas this is the place

            *needsRecovery = *needsRecovery || !ExpectSemicolon();

            *nodeIndex = b.DoneStatement(m, ExpressionStatementNode(expression));
            return true;

        }

        bool TryParseEmbeddedStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // embedded_statement ::= block | simple_embedded_statement

            NodeIndex<BlockNode> block;
            if (TryParseBlock(&block)) {
                *nodeIndex = NodeIndex<StatementNode>(block.index);
                return true;
            }

            if (TryParseSimpleEmbeddedStatement(nodeIndex, needsRecovery)) {
                return true;
            }

            return false;
        }

        bool TryParseStatement(NodeIndex<StatementNode>* nodeIndex, bool* needsRecovery) {
            // statement ::= declaration_statement | embedded_statement

            if (TryParseEmbeddedStatement(nodeIndex, needsRecovery)) {
                return true;
            }

            if (TryParseDeclarationStatement(nodeIndex, needsRecovery)) {
                return true;
            }

            return false;

        }

        bool TryParseBlock(NodeIndex<BlockNode>* nodeIndex) {
            // block ::= '{' statement_list? '}'

            if (b.GetTokenType() != TokenType::CurlyBraceOpen) {
                return false;
            }

            Marker m = b.Mark();

            NodeIndex<StatementNode> firstStatement;
            NodeIndex<StatementNode> ptr;

            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_CurlyBracket(&stream);

            b.PushStream(&stream);

            while (!b.EndOfInput()) {

                bool recovery = false;
                NodeIndex<StatementNode> statement;
                if (TryParseStatement(&statement, &recovery)) {

                    if (!firstStatement.IsValid()) {
                        firstStatement = statement;
                    }
                    else {
                        b.GetPsiNodeUnsafe(ptr)->next = statement;
                    }
                    ptr = statement;

                }
                else {
                    recovery = true;
                }
                if (recovery) {

                    // anything that could signify statement start that is easy to identify
                    // try to see if we can keep  processing statements: 'return', }, 'if', 'else', 'for', 'foreach', 'do', 'while', 'var', builtin_type, ';' 'remember', 'const',  'break;, 'continue', 'try', 'catch', 'finally', 'throw', 'switch', 'case',
                    // then there are symbols that tell us we are definitely not in a statement list anymore: 'public', 'private', 'protected', 'internal', 'extern', 'class', 'struct', 'enum', 'delegate', 'sealed', etc

                    Marker e = b.Mark();

                    while (!b.EndOfInput()) {

                        TokenType tokenType = b.GetTokenType();

                        if (tokenType == TokenType::CurlyBraceOpen || tokenType == TokenType::SemiColon) {
                            break;
                        }

                        switch (b.GetKeyword()) {
                            case Keyword::Return:
                            case Keyword::Break:
                            case Keyword::Continue:
                            case Keyword::Try:
                            case Keyword::Foreach:
                            case Keyword::For:
                            case Keyword::Do:
                            case Keyword::While:
                            case Keyword::Var:
                            case Keyword::Const:
                            case Keyword::Finally:
                            case Keyword::Throw:
                            case Keyword::Switch:
                            case Keyword::If:
                            case Keyword::Else:
                            case Keyword::Using:
                            case Keyword::With:
                            case Keyword::This:
                            case Keyword::Base: {
                                goto end_recovery;
                                break;
                            }
                            default: {
                                b.AdvanceLexer();
                                break;
                            }

                        }
                    }

                    end_recovery:
                    b.Error(e, "unexpected token");
                }
            }

            b.PopStream();

            *nodeIndex = b.Done(m, BlockNode(firstStatement));
            return true;
        }

        bool TryParseCastExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // cast_expression ::= '(' type_path ')' unary_expression

            if (b.GetTokenType() != TokenType::OpenParen) {
                return false;
            }

            Marker m = b.Mark();

            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_Paren(&stream);

            b.PushStream(&stream);

            NodeIndex<TypePathNode> typePath;
            if (!TryParseTypePath(&typePath, needsRecovery)) {
                b.RollbackTo(m);
                return false;
            }

            b.PopStream();

            NodeIndex<ExpressionNode> expression;
            if (TryParseUnaryExpression(&expression, needsRecovery)) {
                *nodeIndex = b.DoneExpression(m, TypeCastExpressionNode(typePath, expression));
            }
            else {
                *needsRecovery = true;
                b.InlineError("Expected an expression after type cast");
                *nodeIndex = b.DoneExpression(m, TypeCastExpressionNode(typePath, NodeIndex<ExpressionNode>(0)));
            }

            return true;

        }

        static int32 BufferNumber(char* str, int32 length, char* buffer, int32 bufferLimit) {
            int32 write = 0;
            int32 max = length < bufferLimit - 1 ? length : bufferLimit - 1;
            for (int32 i = 0; i < max; i++) {
                if (str[i] != '_') {
                    buffer[write++] = str[i];
                }
            }
            buffer[write] = '\0';
            return write;
        }

        bool TryParseLiteralExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // literal_expression ::= numeric_literal | default_literal | null_literal | char_literal | bool_literal | string_literal_or_interpolation | style_literal
            TokenType tokenType = b.GetTokenType();

            switch (tokenType) {
                default: {
                    return false;
                }

                case TokenType::HexLiteral: {
                    Marker m = b.Mark();

                    // we need to strip out the _ before conversion
                    FixedCharSpan span = b.GetTokenSource();
                    char buffer[128] {};
                    BufferNumber(span.ptr + 2, span.size - 2, buffer, 128); // skip 0x

                    char* start = buffer;
                    char* end;
                    int64 value = strtoll(start, &end, 16); // Base 16 for hex

                    b.AdvanceLexer();
                    LiteralValue v;

                    if ((errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) || (errno != 0 && value == 0)) {
                        v.int64Value = 0;
                        b.InlineError("value overflows 64 bits of integer storage or is otherwise invalid");
                    }
                    else {
                        v.int64Value = value;
                    }
                    *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Int64, v));

                    return true;
                }

                case TokenType::BinaryNumberLiteral: {

                    Marker m = b.Mark();

                    // we need to strip out the _ before conversion
                    FixedCharSpan span = b.GetTokenSource();
                    char buffer[128] {};
                    BufferNumber(span.ptr + 2, span.size - 2, buffer, 128); // skip 0b

                    char* start = buffer;
                    char* end;
                    int64 value = strtoll(start, &end, 2); // Base 2

                    b.AdvanceLexer();
                    LiteralValue v;

                    if ((errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) || (errno != 0 && value == 0)) {
                        v.int64Value = 0;
                        b.InlineError("value overflows 64 bits of integer storage or is otherwise invalid");
                    }
                    else {
                        v.int64Value = value;
                    }
                    *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Int64, v));
                    return true;
                }

                case TokenType::UInt32Literal:
                case TokenType::UInt64Literal: {
                    Marker m = b.Mark();

                    FixedCharSpan span = b.GetTokenSource();
                    char buffer[128];
                    BufferNumber(span.ptr, span.size, buffer, 128);
                    char* start = buffer;

                    char* end;
                    errno = 0;
                    uint64 value = strtoull(start, &end, 10);

                    b.AdvanceLexer();

                    if ((errno == ERANGE && value == ULLONG_MAX) || (errno != 0 && value == 0)) {
                        LiteralValue v;
                        v.uint64Value = 0;
                        b.InlineError("value overflows 64 bits of integer storage or is otherwise invalid");
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::UInt64, v));
                        return true;
                    }

                    if (tokenType == TokenType::UInt32Literal) {
                        if (value > UINT32_MAX) {
                            b.InlineError("value overflows 32 bits of integer storage");
                        }
                        LiteralValue v;
                        v.uint32Value = (uint32) value;
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::UInt32, v));
                        return true;
                    }
                    else {
                        LiteralValue v;
                        v.uint64Value = value;
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::UInt64, v));
                        return true;
                    }
                }
                case TokenType::Int64Literal:
                case TokenType::Int32Literal: {
                    Marker m = b.Mark();

                    FixedCharSpan span = b.GetTokenSource();
                    char buffer[128];
                    BufferNumber(span.ptr, span.size, buffer, 128);
                    char* start = buffer;

                    char* end;
                    errno = 0;
                    int64 value = strtoll(start, &end, 10);

                    b.AdvanceLexer();
                    LiteralValue v;

                    if ((errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) || (errno != 0 && value == 0)) {
                        v.int64Value = 0;
                        b.InlineError("value overflows 64 bits of integer storage or is otherwise invalid");
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Int64, v));
                    }
                    else if (tokenType == TokenType::Int32Literal) {
                        if (value > INT32_MAX || value < INT32_MIN) {
                            b.InlineError("value overflows 32 bits of integer storage");
                        }
                        v.int32Value = (int32) value;
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Int32, v));
                    }
                    else {
                        v.int64Value = value;
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Int64, v));
                    }

                    return true;

                }

                case TokenType::FloatLiteral: {
                    Marker m = b.Mark();

                    FixedCharSpan span = b.GetTokenSource();
                    char buffer[128];
                    BufferNumber(span.ptr, span.size, buffer, 128);
                    char* start = buffer;
                    char* end;
                    float value = strtof(start, &end);

                    b.AdvanceLexer();

                    if (start != end && !isnan(value) && !isinf(value)) {
                        LiteralValue v;
                        v.floatValue = value;
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Float, v));
                        return true;
                    }

                    if (start == end) {
                        b.ErrorArgs(m, "float literal `%.*s` didn't parse to a value", span.size, span.ptr);
                        return true;
                    }
                    else if (isnan(value)) {
                        b.ErrorArgs(m, "float literal `%.*s` is NaN", span.size, span.ptr);
                        return true;
                    }
                    else if (isinf(value)) {
                        b.ErrorArgs(m, "float literal `%.*s` is infinite", span.size, span.ptr);
                        return true;
                    }
                    else {
                        b.ErrorArgs(m, "float literal `%.*s` is invalid", span.size, span.ptr);
                        return true;
                    }
                }

                case TokenType::DoubleLiteral: {
                    Marker m = b.Mark();

                    FixedCharSpan span = b.GetTokenSource();
                    char buffer[128];
                    BufferNumber(span.ptr, span.size, buffer, 128);
                    char* start = buffer;
                    char* end;
                    double value = strtod(start, &end);

                    b.AdvanceLexer();

                    if (start != end && !isnan(value) && !isinf(value)) {
                        LiteralValue v;
                        v.doubleValue = value;
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Double, v));
                        return true;
                    }

                    if (start == end) {
                        b.ErrorArgs(m, "double literal `%.*s` didn't parse to a value", span.size, span.ptr);
                        return true;
                    }
                    else if (isnan(value)) {
                        b.ErrorArgs(m, "double literal `%.*s` is NaN", span.size, span.ptr);
                        return true;
                    }
                    else if (isinf(value)) {
                        b.ErrorArgs(m, "double literal `%.*s` is infinite", span.size, span.ptr);
                        return true;
                    }
                    else {
                        b.ErrorArgs(m, "double literal `%.*s` is invalid", span.size, span.ptr);
                        return true;
                    }
                }

                case TokenType::KeywordOrIdentifier: {
                    Keyword keyword = b.GetKeyword();
                    if (keyword == Keyword::True) {
                        Marker m = b.Mark();
                        b.AdvanceLexer();
                        LiteralValue v;
                        v.boolValue = true;
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Bool, v));
                        return true;
                    }
                    else if (keyword == Keyword::False) {
                        Marker m = b.Mark();
                        b.AdvanceLexer();
                        LiteralValue v;
                        v.boolValue = false;
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Bool, v));
                        return true;
                    }
                    else if (keyword == Keyword::Default) {
                        Marker m = b.Mark();

                        PsiBuilder2::SubStream stream;

                        if (b.TryGetSubSubStream_Paren(&stream)) {

                            b.PushStream(&stream);

                            NodeIndex<TypePathNode> typePath;
                            if (!TryParseTypePath(&typePath, needsRecovery)) {
                                b.InlineError("Expected a type name after 'default' expression");
                            }

                            b.PopStream();

                            *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Default, typePath));
                            return true;

                        }
                        else {
                            *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Default));
                            return true;
                        }
                    }
                    else if (keyword == Keyword::Null) {
                        Marker m = b.Mark();
                        b.AdvanceLexer();
                        *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::Null));
                        return true;
                    }
                    else {
                        return false;
                    }
                }


                case TokenType::RegularStringPart: {
                    return true;
                }

                case TokenType::OpenCharacter: {
                    return true;
                }

            }

        }


        bool TryParseTypeOfExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // typeof_expression ::= 'typeof' '(' type_path ')'
            if (b.GetKeyword() != Keyword::Typeof) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();
            NodeIndex<TypePathNode> typePath;

            PsiBuilder2::SubStream stream;
            if (!b.TryGetSubSubStream_Paren(&stream)) {
                *needsRecovery = true;
                b.InlineError("Expected a type expression inside parens after 'typeof'");
                *nodeIndex = b.DoneExpression(m, TypeOfExpressionNode(typePath));
                return true;
            }

            if (!TryParseTypePath(&typePath, needsRecovery)) {
                b.InlineError("Expected a type path after `typeof(`");
            }

            b.PopStream();
            *nodeIndex = b.DoneExpression(m, TypeOfExpressionNode(typePath));
            return true;

        }

        bool TryParseParenExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // paren_expression :: '(' expression ')'

            if (b.GetTokenType() != TokenType::OpenParen) {
                return false;
            }

            Marker m = b.Mark();

            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_Paren(&stream);

            b.PushStream(&stream);

            NodeIndex<ExpressionNode> expression;
            if (!TryParseExpression(&expression, needsRecovery)) {
                b.InlineError("Expected an expression between ( )");
            }

            b.PopStream();
            *nodeIndex = b.DoneExpression(m, ParenExpressionNode(expression));
            return true;

        }

        bool TryParseDynamicExpression(NodeIndex<ExpressionNode>* nodeIndex) {
            // dynamic_expression ::= dynamic_array_expression | dynamic_object_expression | expression
            bool recovery = false;

            if (TryParseDynamicArrayExpression(nodeIndex)) {
                return true;
            }
            else if (TryParseDynamicObjectExpression(nodeIndex)) {
                return true;
            }
            else if (TryParseExpression(nodeIndex, &recovery)) {
                if (recovery) {
                    TryRecoverToCommaOrEnd("Expected an expression");
                }
                return true;
            }
            else {
                return false;
            }

        }

        static bool TryParseDynamicExpressionFn(ParserImpl* parser, NodeIndex<ExpressionNode>* n) {
            return parser->TryParseDynamicExpression(n);
        }

        static bool TryParseDynamicKeyValueFn(ParserImpl* parser, NodeIndex<DynamicKeyValueNode>* n) {
            return parser->TryParseDynamicKeyValue(n);
        }

        bool TryParseDynamicKeyValue(NodeIndex<DynamicKeyValueNode>* nodeIndex) {
            // dynamic_key_value ::= dynamic_key ':' dynamic_value

            Marker m = b.Mark();

            NodeIndex<DynamicKeyNode> key;
            NodeIndex<ExpressionNode> value;

            if (!TryParseDynamicKey(&key)) {
                b.RollbackTo(m);
                return false;
            }

            if (b.GetTokenType() == TokenType::Colon) {
                b.AdvanceLexer();
            }
            else {
                TryRecoverToColonOrCommaOrEnd("Expected a : after dynamic literal key");
            }

            if (!TryParseDynamicValue(&value)) {
                TryRecoverToCommaOrEnd("Expected a value or expression after :");
            }

            *nodeIndex = b.Done(m, DynamicKeyValueNode(key, value));
            return true;

        }

        bool TryParseDynamicValue(NodeIndex<ExpressionNode>* nodeIndex) {
            // dynamic_value ::= dynamic_object_literal | dynamic_array_literal | expression

            bool recovery = false;
            if (TryParseDynamicObjectExpression(nodeIndex)) {
                return true;
            }
            else if (TryParseDynamicArrayExpression(nodeIndex)) {
                return true;
            }
            else if (TryParseExpression(nodeIndex, &recovery)) {
                if (recovery) {
                    TryRecoverToCommaOrEnd("Expected an expression");
                }
                return true;
            }
            else {
                return false;
            }

        }

        bool TryParseStringPart(NodeIndex<StringPartNode>* nodeIndex) {

            if (b.GetTokenType() == TokenType::RegularStringPart) {

                Marker m = b.Mark();
                FixedCharSpan source = b.GetTokenSource();
                b.AdvanceLexer();

                *nodeIndex = b.Done(m, StringPartNode(StringPartType::RegularStringLiteral, source));
                return true;

            }
            else if (b.GetTokenType() == TokenType::ShortStringInterpolation) {

                Marker m = b.Mark();
                FixedCharSpan identifier = b.GetTokenSource();

                // skip over the $
                identifier.ptr++;
                identifier.size--;

                b.AdvanceLexer();
                *nodeIndex = b.Done(m, StringPartNode(StringPartType::ShortInterpolation, identifier));
                return true;

            }
            else if (b.GetTokenType() == TokenType::LongStringInterpolationStart) {

                Marker m = b.Mark();

                PsiBuilder2::SubStream stream;

                b.TryGetSubSubStream(&stream, TokenType::LongStringInterpolationStart, TokenType::LongStringInterpolationEnd);

                NodeIndex<ExpressionNode> expression;

                bool recovery = false;
                b.PushStream(&stream);

                if (!TryParseExpression(&expression, &recovery)) {
                    b.InlineError("Expected an expression in string interpolation");
                }

                b.PopStream();

                *nodeIndex = b.Done(m, StringPartNode(StringPartType::LongInterpolation, expression));
                return true;

            }

            return false;

        }

        bool TryParseStringLiteral(NodeIndex<ExpressionNode>* nodeIndex) {
            // string_literal ::= string_part (short_string_template | string_part | template_string)*
            // ${expression} $identifier

            if (b.GetTokenType() != TokenType::StringStart) {
                return false;
            }

            Marker m = b.Mark();

            PsiBuilder2::SubStream stringStream;
            b.TryGetSubSubStream(&stringStream, TokenType::StringStart, TokenType::StringEnd);

            b.PushStream(&stringStream);

            NodeIndex<StringPartNode> first;
            NodeIndex<StringPartNode> part;
            NodeIndex<StringPartNode> ptr;

            while (TryParseStringPart(&part)) {
                if (!first.IsValid()) {
                    first = part;
                }
                else {
                    b.GetPsiNodeUnsafe(ptr)->next = part;
                }

                ptr = part;
            }

            b.PopStream();

            // maybe make this a string literal node instead
            LiteralValue literalValue;
            literalValue.stringNodeIndex = UntypedNodeIndex(first.index);
            *nodeIndex = b.DoneExpression(m, LiteralExpressionNode(LiteralType::String, literalValue));

            return true;

        }

        bool TryParseDynamicKey(NodeIndex<DynamicKeyNode>* nodeIndex) {
            // dynamic_key ::= identifier | simple_string_literal | '[' expression ']'

            NodeIndex<ExpressionNode> expression;
            if (b.GetTokenType() == TokenType::KeywordOrIdentifier) {
                Marker m = b.Mark();
                FixedCharSpan name = b.GetTokenSource();
                b.AdvanceLexer();

                *nodeIndex = b.Done(m, DynamicKeyNode(name));
                return true;
            }
            else if (TryParseStringLiteral(&expression)) {
                Marker m = b.Precede(expression);
                b.AdvanceLexer();
                *nodeIndex = b.Done(m, DynamicKeyNode(expression));
                return true;
            }
            else if (b.GetTokenType() == TokenType::SquareBraceOpen) {
                Marker m = b.Mark();

                PsiBuilder2::SubStream stream;
                b.TryGetSubSubStream_SquareBrace(&stream);
                b.PushStream(&stream);

                bool recover = false;
                if (!TryParseExpression(&expression, &recover)) {
                    b.InlineError("Expected an expression after [ in dynamic key");
                }

                b.PopStream();

                *nodeIndex = b.Done(m, DynamicKeyNode(expression));
                return true;
            }
            else {
                return false;
            }
        }

        bool TryParseDynamicArrayExpression(NodeIndex<ExpressionNode>* nodeIndex) {
            // dynamic_array_expression ::= '[' (dynamic_expression (',' dynamic_expression)*)? ']'

            if (b.GetTokenType() != TokenType::SquareBraceOpen) {
                return false;
            }

            Marker m = b.Mark();

            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_SquareBrace(&stream);

            b.PushStream(&stream);

            NodeIndex<ExpressionNode> first;

            ParseCommaSeparatedList(&first, true, &TryParseDynamicExpressionFn);

            b.PopStream();

            *nodeIndex = b.DoneExpression(m, DynamicArrayLiteralNode(first));

            return true;

        }

        bool TryParseDynamicObjectExpression(NodeIndex<ExpressionNode>* nodeIndex) {
            // dynamic_object_expression ::= '{' (dynamic_key_value (',' dynamic_key_value)*)? ','? '}'
            if (b.GetTokenType() != TokenType::CurlyBraceOpen) {
                return false;
            }

            Marker m = b.Mark();

            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_CurlyBracket(&stream);

            b.PushStream(&stream);

            NodeIndex<DynamicKeyValueNode> first;

            ParseCommaSeparatedList(&first, true, &TryParseDynamicKeyValueFn);

            b.PopStream();

            *nodeIndex = b.DoneExpression(m, DynamicObjectLiteralNode(first));

            return true;
        }

        bool TryParseNewExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {

            if (b.GetKeyword() != Keyword::New) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            AllocatorType allocatorType = AllocatorType::Default;

            switch (b.GetKeyword()) {

                case Keyword::Tempalloc: {
                    b.AdvanceLexer();
                    allocatorType = AllocatorType::TempAllocator;
                    break;
                }
                case Keyword::Scopealloc: {
                    b.AdvanceLexer();
                    allocatorType = AllocatorType::ScopeAllocator;
                    break;
                }
                case Keyword::Stackalloc: {
                    b.AdvanceLexer();
                    allocatorType = AllocatorType::StackAllocator;
                    break;
                }

                default: {
                    break;
                }

            }

            if (b.GetKeyword() == Keyword::Dynamic) {

                b.AdvanceLexer();

                NodeIndex<ExpressionNode> dynamicExpression;

                if (TryParseDynamicArrayExpression(&dynamicExpression)) {
                    *nodeIndex = b.DoneExpression(m, NewDynamicArrayLiteralNode(allocatorType, NodeIndex<DynamicArrayLiteralNode>(dynamicExpression.index)));
                    return true;
                }

                if (TryParseDynamicObjectExpression(&dynamicExpression)) {
                    *nodeIndex = b.DoneExpression(m, NewDynamicArrayLiteralNode(allocatorType, NodeIndex<DynamicArrayLiteralNode>(dynamicExpression.index)));
                    return true;
                }

                *needsRecovery = true;
                b.InlineError("Expected a dynamic array or dynamic object literal");
                *nodeIndex = b.DoneExpression(m, NewDynamicObjectLiteralNode(allocatorType, NodeIndex<DynamicObjectLiteralNode>(0)));
                return true;

            }

            // new alloc type_path ('.' identifier)? argument_list ('{' initializer_list? '}')?

            NodeIndex<TypePathNode> typePath;
            NodeIndex<IdentifierNode> constructorName;
            NodeIndex<ArgumentListNode> argumentList;
            NodeIndex<InitializerListNode> initializerList;

            if (!TryParseTypePath(&typePath, needsRecovery)) {
                b.InlineError("Expected a type name after 'new'");
                *nodeIndex = b.DoneExpression(m, NewExpressionNode(typePath, constructorName, argumentList, initializerList));
                return true;
            }

            if (b.GetTokenType() == TokenType::Dot) {
                b.AdvanceLexer();

                if (!TryParseIdentifier(&constructorName, true)) {
                    *needsRecovery = true;
                    b.InlineError("Expected a constructor name after new expression '.'");
                }

            }

            if (!TryParseArgumentList(&argumentList)) {
                *needsRecovery = true;
                b.InlineError("Expected a constructor argument list");
                *nodeIndex = b.DoneExpression(m, NewExpressionNode(typePath, constructorName, argumentList, initializerList));
                return true;
            }

            TryParseInitializerList(&initializerList);

            *nodeIndex = b.DoneExpression(m, NewExpressionNode(typePath, constructorName, argumentList, initializerList));
            return true;

        }

        bool TryParseMemberInitializer(NodeIndex<InitializerNode>* nodeIndex, bool* needsRecovery) {
            // member_initializer ::= identifier '=' expression

            if (b.GetTokenType() != TokenType::KeywordOrIdentifier) {
                return false;
            }

            NodeIndex<ExpressionNode> expression;
            FixedCharSpan memberName;

            Marker m = b.Mark();
            b.AdvanceLexer();

            if (b.GetTokenType() != TokenType::Assign) {
                b.InlineError("Expected an '=' sign in member initializer");
                *needsRecovery = true;
                *nodeIndex = b.DoneInitializer(m, MemberInitializerNode(memberName, expression));
                return true;
            }
            else {
                b.AdvanceLexer();
            }

            if (!TryParseExpression(&expression, needsRecovery)) {
                b.InlineError("Expected an expression after '=' in member initializer");
                *needsRecovery = true;
            }

            *nodeIndex = b.DoneInitializer(m, MemberInitializerNode(memberName, expression));
            return true;

        }

        static bool TryParseExpressionWithRecoveryFn(ParserImpl* parser, NodeIndex<ExpressionNode>* first) {
            bool recovery = false;
            if (!parser->TryParseExpression(first, &recovery)) {
                return false;
            }
            if (recovery) {
                parser->TryRecoverToCommaOrEnd("unexpected token");
            }
            return true;
        }

        bool TryParseCollectionInitializer(NodeIndex<InitializerNode>* nodeIndex) {
            // collection_initializer ::= '{' (non_assignment_expression  (',' non_assignment_expression)* )? '}'

            if (b.GetTokenType() != TokenType::CurlyBraceOpen) {
                return false;
            }

            Marker m = b.Mark();
            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_CurlyBracket(&stream);

            b.PushStream(&stream);

            NodeIndex<ExpressionNode> first;

            ParseCommaSeparatedList(&first, true, &TryParseExpressionWithRecoveryFn);

            b.PopStream();

            *nodeIndex = b.DoneInitializer(m, CollectionInitializerNode(first));
            return true;

        }

        bool TryParseIndexedInitializer(NodeIndex<InitializerNode>* nodeIndex, bool* needsRecovery) {
            // indexed_initializer ::= '[' argument_list ']' '=' expression

            if (b.GetTokenType() != TokenType::SquareBraceOpen) {
                return false;
            }

            Marker m = b.Mark();
            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_SquareBrace(&stream);

            b.PushStream(&stream);

            NodeIndex<ExpressionNode> value;
            NodeIndex<ArgumentListNode> argumentList;

            if (stream.Empty()) {
                b.InlineError("Expected an argument");
            }

            if (!TryParseArgumentList(&argumentList)) {
                b.InlineError("Expected a list of arguments in indexed initializer after [");
                *needsRecovery = true;
            }

            b.PopStream();

            if (b.GetTokenType() == TokenType::Assign) {
                b.AdvanceLexer();
            }
            else {
                b.InlineError("expected an assignment after indexed initializer");
            }

            if (!TryParseExpression(&value, needsRecovery)) {
                *needsRecovery = true;
                b.InlineError("Expected an expression after indexed initializer '='");
            }

            *nodeIndex = b.DoneInitializer(m, IndexedInitializerNode(argumentList, value));
            return true;

        }

        bool TryParseListInitializer(NodeIndex<InitializerNode>* nodeIndex, bool* needsRecovery) {
            // List_initializer ::= expression

            Marker m = b.Mark();
            NodeIndex<ExpressionNode> expression;
            if (TryParseExpression(&expression, needsRecovery)) {
                *nodeIndex = b.DoneInitializer(m, ListInitializerNode(expression));
                return true;
            }
            b.RollbackTo(m);
            return false;
        }

        bool TryParseInitializer(NodeIndex<InitializerNode>* nodeIndex) {
            // initializer ::= member_initializer | collection_initializer | indexed_initializer | list_initializer

            bool recovery = false;
            bool success = TryParseMemberInitializer(nodeIndex, &recovery) ||
                           TryParseCollectionInitializer(nodeIndex) ||
                           TryParseIndexedInitializer(nodeIndex, &recovery) ||
                           TryParseListInitializer(nodeIndex, &recovery);

            if (recovery) {
                TryRecoverToCommaOrEnd("unexpected token");
            }

            return success;

        }

        static bool TryParseInitializerFn(ParserImpl* parser, NodeIndex<InitializerNode>* first) {
            return parser->TryParseInitializer(first);
        }

        bool TryParseInitializerList(NodeIndex<InitializerListNode>* nodeIndex) {
            // initializer_list ::= '{' (initializer (',' initializer)*)? '}'

            if (b.GetTokenType() != TokenType::CurlyBraceOpen) {
                return false;
            }
            Marker m = b.Mark();
            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_CurlyBracket(&stream);
            b.PushStream(&stream);
            NodeIndex<InitializerNode> first;
            ParseCommaSeparatedList(&first, true, &TryParseInitializerFn);
            b.PopStream();
            *nodeIndex = b.Done(m, InitializerListNode(first));
            return true;
        }

        bool TryParsePrimaryExpressionStart(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // primary_expression_start ::= literal
            // | paren_expression
            // | predefined_type
            // | type_path
            // | primary_method_call
            // | identifier_with_type_arguments
            // | THIS
            // | BASE
            // | typeof_expression
            // | nameof_expression
            // | new_expression

            if (TryParseLiteralExpression(nodeIndex, needsRecovery)) {
                return true;
            }

            if (TryParseParenExpression(nodeIndex, needsRecovery)) {
                return true;
            }

            if (TryParseTypeOfExpression(nodeIndex, needsRecovery)) {
                return true;
            }

            Keyword keyword = b.GetKeyword();
            if (b.GetTokenType() == TokenType::KeywordOrIdentifier && keyword != Keyword::NotAKeyword) {
                switch (keyword) {
                    case Keyword::This: {
                        Marker m = b.Mark();
                        b.AdvanceLexer();
                        *nodeIndex = b.DoneExpression(m, ThisReferenceExpressionNode());
                        return true;
                    }
                    case Keyword::Base: {
                        Marker m = b.Mark();
                        b.AdvanceLexer();
                        *nodeIndex = b.DoneExpression(m, BaseReferenceExpressionNode());
                        return true;
                    }
                    case Keyword::NameOf: {
                        assert(false && "nameof not implemented");
                        break;
                    }

                    case Keyword::New: {
                        return TryParseNewExpression(nodeIndex, needsRecovery);
                    }

                    default: {
                        BuiltInTypeName builtInTypeName = BuiltInTypeNameFromKeyword(keyword);
                        if (builtInTypeName != BuiltInTypeName::Void && builtInTypeName != BuiltInTypeName::Invalid) {
                            Marker m = b.Mark();
                            b.AdvanceLexer();
                            *nodeIndex = b.DoneExpression(m, BuiltInTypeAccessExpressionNode(builtInTypeName));
                            return true;
                        }
                        break;
                    }

                }
            }

            UntypedNodeIndex methodOrId;
            if(TryParseSimpleMethodInvocationOrIdentifier(&methodOrId, needsRecovery)) {
                *nodeIndex = NodeIndex<ExpressionNode>(methodOrId.index);
                return true;
            }

//            NodeIndex<IdentifierNode> id;
//            if (TryParseIdentifierWithTypeArguments(&id, needsRecovery, false)) {
//                *nodeIndex = NodeIndex<ExpressionNode>(id.index);
//                return true;
//            }

            return false;

        }

        bool TryParseBracketExpression(NodeIndex<BracketExpressionNode>* nodeIndex) {
            // bracket_expression ::= '?' '[' expression (',' expression)* ']'

            if (b.GetTokenType() != TokenType::QuestionMark && b.GetTokenType() != TokenType::SquareBraceOpen) {
                return false;
            }

            Marker m = b.Mark();

            bool isNullable = false;
            NodeIndex<ExpressionNode> expression;

            if (b.GetTokenType() == TokenType::QuestionMark) {
                isNullable = true;
                b.AdvanceLexer();
            }

            // I'm not sure that this is required is it?
            PsiBuilder2::SubStream stream;

            if (!b.TryGetSubSubStream_SquareBrace(&stream)) {
                *nodeIndex = b.Done(m, BracketExpressionNode(isNullable, expression));
                return true;
            }

            b.PushStream(&stream);

            bool recovery = false; // recovery handled by the stream

            if (stream.Empty()) {
                b.InlineError("Expected an expression inside []");
            }
            else if (!TryParseExpression(&expression, &recovery)) {
                b.InlineError("Expected an expression as part of an index expression");
            }

            b.PopStream();

            *nodeIndex = b.Done(m, BracketExpressionNode(isNullable, expression));
            return true;


        }

        bool TryParseBracketExpressionList(NodeIndex<BracketExpressionNode>* nodeIndex) {
            // bracket_expression_list ::= bracket_expression+

            if (!TryParseBracketExpression(nodeIndex)) {
                return false;
            }

            NodeIndex<BracketExpressionNode> ptr = *nodeIndex;
            NodeIndex<BracketExpressionNode> next;

            while (TryParseBracketExpression(&next)) {
                b.GetPsiNodeUnsafe(ptr)->next = next;
                ptr = next;
            }

            return true;

        }

        bool TryParsePrimaryExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // primary_expression ::= primary_expression_start bracket_expression_list primary_expression_tail_list

            NodeIndex<ExpressionNode> peStart;
            if (!TryParsePrimaryExpressionStart(&peStart, needsRecovery)) {
                return false;
            }

            Marker m = b.Precede(b.GetMarker(peStart.index));

            if (*needsRecovery) {
                *nodeIndex = b.DoneExpression(m, PrimaryExpressionNode(peStart, NodeIndex<BracketExpressionNode>(0), NodeIndex<PrimaryExpressionTailNode>(0)));
                return true;
            }

            NodeIndex<BracketExpressionNode> startBracketExpression;
            TryParseBracketExpressionList(&startBracketExpression);

            NodeIndex<PrimaryExpressionTailNode> tail;
            TryParsePrimaryExpressionTailList(&tail, needsRecovery);

            *nodeIndex = b.DoneExpression(m, PrimaryExpressionNode(peStart, startBracketExpression, tail));
            return true;

        }

        bool TryParseScopeMemberAccess(UntypedNodeIndex* nodeIndex, bool* needsRecovery) {
            // scope_member_access ::= '::' identifier type_argument_list?

            TokenType tokenType = b.GetTokenType();

            if (tokenType != TokenType::DoubleColon) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<IdentifierNode> identifier;
            if (!TryParseIdentifierWithTypeArguments(&identifier, needsRecovery, false)) {
                *needsRecovery = true;
                b.InlineError("Expected an identifier after ::");
            }

            *nodeIndex = b.DoneUntyped(m, ScopeMemberAccessNode(identifier));
            return true;

        }

        bool TryParseConditionalMemberAccess(UntypedNodeIndex* nodeIndex, bool* needsRecovery) {
            // conditional_member_access ::= '?.' identifier type_argument_list?

            if (b.GetTokenType() != TokenType::ConditionalAccess) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<IdentifierNode> identifier;
            if (!TryParseIdentifierWithTypeArguments(&identifier, needsRecovery, false)) {
                *needsRecovery = true;
                b.InlineError("Expected an identifier after ?.");
            }

            *nodeIndex = b.DoneUntyped(m, ConditionalMemberAccessNode(identifier));
            return true;
        }

        bool TryParseMemberAccess(UntypedNodeIndex* nodeIndex, bool* needsRecovery) {
            // member_access ::= '.' identifier type_argument_list?

            if (b.GetTokenType() != TokenType::Dot) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<IdentifierNode> identifier;
            if (!TryParseIdentifierWithTypeArguments(&identifier, needsRecovery, false)) {
                *needsRecovery = true;
                b.InlineError("Expected an identifier after .");
            }

            *nodeIndex = b.DoneUntyped(m, MemberAccessNode(identifier));
            return true;

        }

        // Method(temp new Thing(), scoped new float[] {1, 3, 4, 5 }, ref value, out float v, out temp float[] result)
        bool TryParseArgument(NodeIndex<ArgumentNode>* nodeIndex) {
            // argument ::= ref expression
            //           | out 'temp'? ('var' | type_path)? expression
            //           | expression

            // not sure how to tell the difference between an identifier and a type path at this point
            // a type path will have an identifier following it, an expression won't

            Keyword keyword = b.GetKeyword();

            if (keyword == Keyword::Ref) {
                Marker m = b.Mark();
                b.AdvanceLexer();

                NodeIndex<ExpressionNode> expression;

                bool recovery = false;
                if (!TryParseExpression(&expression, &recovery)) {
                    b.InlineError("Expected an expression");
                    recovery = true;
                }

                *nodeIndex = b.Done(m, ArgumentNode(ArgumentPassByModifier::Ref, false, NodeIndex<TypePathNode>(0), expression));

                if (recovery) {
                    TryRecoverToCommaOrEnd("unexpected token");
                }

                return true;

            }
            else if (keyword == Keyword::Out) {
                Marker m = b.Mark();
                b.AdvanceLexer();

                NodeIndex<ExpressionNode> expression;
                NodeIndex<TypePathNode> typePath;

                if (b.GetKeyword() == Keyword::Var) {
                    b.AdvanceLexer();

                    // out var identifier

                    FixedCharSpan variableName = b.GetTokenSource();
                    if (b.GetTokenType() == TokenType::KeywordOrIdentifier) {
                        if (b.GetKeyword() != Keyword::NotAKeyword) {
                            b.InlineError("Cannot use keyword as an identifier");
                        }
                        b.AdvanceLexer();
                    }
                    else {
                        b.InlineError("Expected an out var identifier");
                    }

                    *nodeIndex = b.Done(m, ArgumentNode(ArgumentPassByModifier::Out, true, NodeIndex<TypePathNode>(0), variableName));
                    return true;

                }
                else {

                    int32 location = b.GetTokenIndex();


                    bool recovery = false;

                    // out type identifier
                    if (TryParseTypePath(&typePath, &recovery)) {

                        if (recovery) {
                            TryRecoverToCommaOrEnd("unexpected token");
                        }

                        FixedCharSpan variableName = b.GetTokenSource();
                        if (b.GetTokenType() == TokenType::KeywordOrIdentifier) {
                            if (b.GetKeyword() != Keyword::NotAKeyword) {
                                b.InlineError("Cannot use keyword as an identifier");
                            }
                            b.AdvanceLexer();
                            *nodeIndex = b.Done(m, ArgumentNode(ArgumentPassByModifier::Out, false, typePath, variableName));
                            return true;
                        }
                        else {
                            b.RollbackTo(b.MarkFromToken(location));
                        }

                    }

                    // out expression
                    if (!TryParseNonAssignmentExpression(&expression, &recovery)) {
                        b.InlineError("Expected a variable declaration or expression after 'out'");
                    }

                    *nodeIndex = b.Done(m, ArgumentNode(ArgumentPassByModifier::Out, false, NodeIndex<TypePathNode>(0), expression));

                    if (recovery) {
                        TryRecoverToCommaOrEnd("unexpected token");
                    }

                    return true;
                }

            }
            else {

                Marker m = b.Mark();
                NodeIndex<ExpressionNode> expression;

                bool recovery = false;
                if (!TryParseExpression(&expression, &recovery)) {
                    b.RollbackTo(m);
                    return false;
                }

                *nodeIndex = b.Done(m, ArgumentNode(ArgumentPassByModifier::None, false, NodeIndex<TypePathNode>(0), expression));

                if (recovery) {
                    TryRecoverToCommaOrEnd("unexpected token");
                }

                return true;
            }


        }

        template<class T>
        using ParseFunctionPtr = bool (*)(ParserImpl*, T*);

        template<class T>
        void ParseCommaSeparatedList(T* first, bool allowTrailingComma, ParseFunctionPtr<T> fnPtr) {

            T ptr;

            while (!b.EndOfInput()) {
                if (fnPtr(this, first)) {
                    ptr = *first;
                    break;
                }
            }

            while (!b.EndOfInput()) {

                if (b.GetTokenType() == TokenType::Comma) {
                    b.AdvanceLexer();

                    if (!allowTrailingComma && b.EndOfInput()) {
                        b.InlineError("Trailing comma is not allowed here");
                        break;
                    }

                    T next;
                    if (fnPtr(this, &next)) {
                        b.GetPsiNodeUnsafe(ptr)->next = next;
                        ptr = next;
                        continue;
                    }

                    TryRecoverToCommaOrEnd("unexpected token");

                }
                else {
                    TryRecoverToCommaOrEnd("expected a comma");
                }

            }
        }

        bool TryParseArgumentList(NodeIndex<ArgumentListNode>* nodeIndex) {
            // argument_list ::= '(' (argument (',' argument)*)? ')'

            // ParseArgument handles local recovery, so if needsRecovery is set, we're going to need to bail out
            // the result of ParseArgument might be empty, which is ok since we'll never look at it if invalid

            if (b.GetTokenType() != TokenType::OpenParen) {
                return false;
            }

            Marker m = b.Mark();

            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_Paren(&stream);

            b.PushStream(&stream);

            NodeIndex<ArgumentNode> first;
            NodeIndex<ArgumentNode> ptr;

            while (!b.EndOfInput()) {
                if (TryParseArgument(&first)) {
                    ptr = first;
                    break;
                }
            }

            while (!b.EndOfInput()) {

                if (b.GetTokenType() == TokenType::Comma) {
                    b.AdvanceLexer();

                    if (b.EndOfInput()) {
                        b.InlineError("Trailing comma is not allowed in argument list");
                        break;
                    }

                    NodeIndex<ArgumentNode> next;
                    if (TryParseArgument(&next)) {
                        b.GetPsiNodeUnsafe(ptr)->next = next;
                        ptr = next;
                        continue;
                    }

                    TryRecoverToCommaOrEnd("expected an argument following ,");

                }
                else {
                    TryRecoverToCommaOrEnd("expected a comma following argument");
                }

            }

            b.PopStream();

            *nodeIndex = b.Done(m, ArgumentListNode(first));
            return true;

        }

        bool TryParseSimpleMethodInvocationOrIdentifier(UntypedNodeIndex* nodeIndex, bool * needsRecovery) {
            // primary_method_invocation ::= identifier_with_type_arguments argument_list

            if (b.GetTokenType() != TokenType::KeywordOrIdentifier) {
                return false;
            }

            NodeIndex<IdentifierNode> identifierNode;
            if(!TryParseIdentifierWithTypeArguments(&identifierNode, needsRecovery, false)) {
                return false;
            }

            if(b.GetTokenType() != TokenType::OpenParen) {
                *nodeIndex = identifierNode;
                return true;
            }

            Marker m = b.Precede(identifierNode);

            // can't fail
            NodeIndex<ArgumentListNode> argumentList;
            TryParseArgumentList(&argumentList);

            *nodeIndex = b.DoneUntyped(m, PrimaryMethodInvocationNode(identifierNode, argumentList));
            return true;

        }

        bool TryParseMethodInvocation(UntypedNodeIndex* nodeIndex) {
            // method_invocation ::= argument_list

            if (b.GetTokenType() != TokenType::OpenParen) {
                return false;
            }

            Marker m = b.Mark();

            NodeIndex<ArgumentListNode> argumentList;
            TryParseArgumentList(&argumentList);

            *nodeIndex = b.DoneUntyped(m, MethodInvocationNode(argumentList));
            return true;

        }

        bool TryParsePrimaryExpressionTail(NodeIndex<PrimaryExpressionTailNode>* nodeIndex, bool* needsRecovery) {
            // primary_expression_tail ::= (member_access | method_invocation | '++' | '--' | '!') bracket_expression_list?

            if (b.GetTokenType() == TokenType::Increment || b.GetTokenType() == TokenType::Decrement) {

                Marker m = b.Mark();
                Marker m2 = b.Mark();

                b.AdvanceLexer();

                UntypedNodeIndex incr = b.DoneUntyped(m2, IncrementDecrementNode(b.GetTokenType() == TokenType::Increment));

                NodeIndex<BracketExpressionNode> bracketExpressionList;
                TryParseBracketExpressionList(&bracketExpressionList);

                *nodeIndex = b.Done(m, PrimaryExpressionTailNode(incr, bracketExpressionList));
                return true;

            }

            if(b.GetTokenType() == TokenType::Not) {
                Marker m = b.Mark();
                Marker m2 = b.Mark();

                b.AdvanceLexer();

                UntypedNodeIndex nullableDeref = b.Done(m2, NullableDereferenceNode());

                NodeIndex<BracketExpressionNode> bracketExpressionList;
                TryParseBracketExpressionList(&bracketExpressionList);

                *nodeIndex = b.Done(m, PrimaryExpressionTailNode(nullableDeref, bracketExpressionList));
                return true;

            }

            UntypedNodeIndex node;
            if (TryParseMemberAccess(&node, needsRecovery)) {
                Marker m = b.Precede(b.GetMarker(node.index));
                NodeIndex<BracketExpressionNode> bracketExpressionList;
                TryParseBracketExpressionList(&bracketExpressionList);
                *nodeIndex = b.Done(m, PrimaryExpressionTailNode(node, bracketExpressionList));
                return true;
            }
            if (TryParseConditionalMemberAccess(&node, needsRecovery)) {
                Marker m = b.Precede(b.GetMarker(node.index));
                NodeIndex<BracketExpressionNode> bracketExpressionList;
                TryParseBracketExpressionList(&bracketExpressionList);
                *nodeIndex = b.Done(m, PrimaryExpressionTailNode(node, bracketExpressionList));
                return true;
            }
            else if (TryParseScopeMemberAccess(&node, needsRecovery)) {
                Marker m = b.Precede(b.GetMarker(node.index));
                NodeIndex<BracketExpressionNode> bracketExpressionList;
                TryParseBracketExpressionList(&bracketExpressionList);
                *nodeIndex = b.Done(m, PrimaryExpressionTailNode(node, bracketExpressionList));
                return true;
            }
            else if (TryParseMethodInvocation(&node)) {
                Marker m = b.Precede(b.GetMarker(node.index));
                NodeIndex<BracketExpressionNode> bracketExpressionList;
                TryParseBracketExpressionList(&bracketExpressionList);
                *nodeIndex = b.Done(m, PrimaryExpressionTailNode(node, bracketExpressionList));
                return true;
            }

            return false;

        }

        bool TryParsePrimaryExpressionTailList(NodeIndex<PrimaryExpressionTailNode>* nodeIndex, bool* needsRecovery) {
            // primary_expression_tail_list ::= primary_expression_tail+

            if (!TryParsePrimaryExpressionTail(nodeIndex, needsRecovery)) {
                return false;
            }

            if (*needsRecovery) {
                return true;
            }

            NodeIndex<PrimaryExpressionTailNode> ptr = *nodeIndex;
            NodeIndex<PrimaryExpressionTailNode> next;
            while (TryParsePrimaryExpressionTail(&next, needsRecovery)) {
                b.GetPsiNodeUnsafe(ptr)->next = next;
                ptr = next;
                if (*needsRecovery) {
                    break;
                }
            }
            return true;
        }

        bool TryParseUnaryExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // unary_expression ::= cast_expression
            //                    | primary_expression
            //                    | '+' unary_expression
            //                    | '-' unary_expression
            //                    | '!' unary_expression
            //                    | '~' unary_expression
            //                    | '++' unary_expression
            //                    | '--' unary_expression

            if (TryParseCastExpression(nodeIndex, needsRecovery)) {
                return true;
            }

            if (TryParsePrimaryExpression(nodeIndex, needsRecovery)) {
                return true;
            }

            TokenType tokenType = b.GetTokenType();

            UnaryExpressionType type;
            switch (tokenType) {
                case TokenType::Plus: {
                    type = UnaryExpressionType::Plus;
                    break;
                }
                case TokenType::Minus: {
                    type = UnaryExpressionType::Minus;
                    break;
                }
                case TokenType::Not: {
                    type = UnaryExpressionType::Not;
                    break;
                }
                case TokenType::BinaryNot: {
                    type = UnaryExpressionType::BitwiseNot;
                    break;
                }
                case TokenType::Increment: {
                    type = UnaryExpressionType::PreIncrement;
                    break;
                }
                case TokenType::Decrement: {
                    type = UnaryExpressionType::PreDecrement;
                    break;
                }
                default: {
                    return false;
                }
            }

            Marker m = b.Mark();
            b.AdvanceLexer();
            NodeIndex<ExpressionNode> expression;
            if (TryParseUnaryExpression(&expression, needsRecovery)) {
                *nodeIndex = b.DoneExpression(m, UnaryExpressionNode(type, expression));
            }
            else {
                *needsRecovery = true;
                b.InlineError("Expected an expression after unary expression start");
                *nodeIndex = b.DoneExpression(m, UnaryExpressionNode(type, expression));
            }

            return true;

        }

        static bool TryParseSwitchExpressionFn(ParserImpl* p, NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            return p->TryParseSwitchExpression(lhs, needsRecovery);
        }

        static bool TryParseMultiplicativeExpressionFn(ParserImpl* p, NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            return p->TryParseMultiplicativeExpression(lhs, needsRecovery);
        }

        static bool TryParseShiftExpressionFn(ParserImpl* p, NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            return p->TryParseShiftExpression(lhs, needsRecovery);
        }

        static bool TryParseRelationalExpressionFn(ParserImpl* p, NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            return p->TryParseRelationalExpression(lhs, needsRecovery);
        }

        static bool TryParseEqualityExpressionFn(ParserImpl* p, NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            return p->TryParseEqualityExpression(lhs, needsRecovery);
        }

        static bool TryParseBitwiseXorExpressionFn(ParserImpl* p, NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            return p->TryParseBitwiseXorExpression(lhs, needsRecovery);
        }

        static bool TryParseBitwiseAndExpressionFn(ParserImpl* p, NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            return p->TryParseBitwiseAndExpression(lhs, needsRecovery);
        }

        static bool TryParseBitwiseOrExpressionFn(ParserImpl* p, NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            return p->TryParseBitwiseOrExpression(lhs, needsRecovery);
        }

        void ParseExpressionRHSMultiple(NodeIndex<ExpressionNode>* lhs, bool* needsRecovery, RhsTypeInfo* info, int32 count, TryParseExpressionFn method) {

            loop:
            TokenType tokenType = b.GetTokenType();
            int32 index = -1;

            for (int i = 0; i < count; i++) {
                if (tokenType == info[i].tokenType) {
                    index = i;
                    break;
                }
            }

            if (index == -1) {
                return;
            }

            BinaryExpressionType expressionType = info[index].expressionType;

            b.AdvanceLexer();
            Marker lhsMarker = b.GetMarker(lhs->index);

            NodeIndex<ExpressionNode> rhs;
            if (!method(this, &rhs, needsRecovery)) {
                *needsRecovery = true;
                b.Error(b.Mark(), info[index].failureMessage);
                Marker m = b.GetMarker(lhs->index);
                *lhs = b.DoneExpression(m, BinaryExpressionNode(expressionType, NodeIndex<ExpressionNode>(lhs->index), NodeIndex<ExpressionNode>(0)));
                return;
            }
            else if (*needsRecovery) {
                Marker m = b.GetMarker(lhs->index);
                *lhs = b.DoneExpression(m, BinaryExpressionNode(expressionType, NodeIndex<ExpressionNode>(lhs->index), rhs));
                return;
            }

            Marker m = b.Precede(lhsMarker);
            *lhs = b.DoneExpression(m, BinaryExpressionNode(expressionType, *lhs, rhs));
            goto loop;
        }

        void ParseConditionalAndExpressionRHS(NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            // conditional_and_expression_rhs ::= ('&&' bitwise_or_expression)*
            ParseExpressionRHS<TokenType::ConditionalAnd, BinaryExpressionType::ConditionalAnd>(lhs, needsRecovery, "Expected an expression following && operator", &TryParseBitwiseOrExpressionFn);
        }

        bool TryParseSwitchExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // switch_expression ::= unary_expression ('switch' '{' (switch_expression_arms ','?)? '}')?

            NodeIndex<ExpressionNode> switchValue;
            if (!TryParseUnaryExpression(&switchValue, needsRecovery)) {
                return false;
            }

            if (*needsRecovery || b.GetKeyword() != Keyword::Switch) {
                *nodeIndex = switchValue;
                return true;
            }

            Marker m = b.Precede(switchValue);

            NodeIndex<SwitchExpressionArmNode> firstArm;

            b.AdvanceLexer();

            PsiBuilder2::SubStream stream;
            if (!b.TryGetSubSubStream_CurlyBracket(&stream)) {
                b.InlineError("Expected an opening curly brace after `switch` expression");
                *nodeIndex = b.DoneExpression(m, SwitchExpressionNode(switchValue, firstArm));
                *needsRecovery = true;
                return true;
            }

            b.PushStream(&stream);

            if (!TryParseSwitchExpressionArms(&firstArm)) {
                b.InlineError("Expected a set of switch expression arms");
            }

            b.PopStream();

            *nodeIndex = b.DoneExpression(m, SwitchExpressionNode(switchValue, firstArm));

            return true;

        }

        bool TryParseSwitchExpressionArm(NodeIndex<SwitchExpressionArmNode>* nodeIndex, bool* needsRecovery) {
            // switch_expression_arm ::= expression case_guard? right_arrow throwable_expression

            NodeIndex<ExpressionNode> condition;
            NodeIndex<ExpressionNode> caseGuard;
            NodeIndex<ExpressionNode> action;

            Marker m = b.Mark();

            if (!TryParseNonAssignmentExpression(&condition, needsRecovery)) {
                b.RollbackTo(m);
                return false;
            }
            else if (*needsRecovery) {
                goto end;
            }

            if (b.GetKeyword() == Keyword::When) {

                b.AdvanceLexer();

                if (!TryParseExpression(&caseGuard, needsRecovery)) {
                    b.InlineError("Expected an expression following 'when' in switch expression arm");
                    *needsRecovery = b.GetTokenType() != TokenType::FatArrow;
                }

                if (*needsRecovery) {
                    goto end;
                }

            }

            if (b.GetTokenType() != TokenType::FatArrow) {
                b.InlineError("Expected a `=>` following expression in switch arm expression");
                goto end;
            }

            b.AdvanceLexer();

            if (!TryParseThrowableExpression(&action, needsRecovery)) {
                b.InlineError("Expected an expression following `=>` switch arm");
            }

            end:
            *nodeIndex = b.Done(m, SwitchExpressionArmNode(condition, caseGuard, action));
            return true;

        }

        bool TryParseSwitchExpressionArms(NodeIndex<SwitchExpressionArmNode>* nodeIndex) {
            // switch_expression_arms ::= switch_expression_arm (',' switch_expression_arm)*

            bool recovery = false;
            if (!TryParseSwitchExpressionArm(nodeIndex, &recovery)) {
                return false;
            }

            if (recovery) {
                TryRecoverToCommaOrEnd("unexpected token");
            }

            NodeIndex<SwitchExpressionArmNode> ptr = *nodeIndex;

            while (!b.EndOfInput()) {

                if (b.GetTokenType() == TokenType::Comma) {

                    b.AdvanceLexer();

                    // trailing comma is fine here
                    if (b.EndOfInput()) {
                        break;
                    }

                    NodeIndex<SwitchExpressionArmNode> next;
                    bool armRecovery = false;
                    if (TryParseSwitchExpressionArm(&next, &armRecovery)) {
                        b.GetPsiNodeUnsafe(ptr)->next = next;
                        ptr = next;
                    }
                    else {
                        armRecovery = true;
                    }

                    if (armRecovery) {
                        TryRecoverToCommaOrEnd("Expected a switch arm");
                    }

                }
                else {
                    TryRecoverToCommaOrEnd("Expected a comma after switch expression arm");
                }

            }

            return true;

        }

        bool TryParseMultiplicativeExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // multiplicative_expression ::= switch_expression (('*' | '/' | '%') switch_expression)*

            if (!TryParseSwitchExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            RhsTypeInfo info[3];
            info[0] = {TokenType::Multiply, BinaryExpressionType::Multiply, "Expected an expression following * operator"};
            info[1] = {TokenType::Divide, BinaryExpressionType::Divide, "Expected an expression following / operator"};
            info[2] = {TokenType::Modulus, BinaryExpressionType::Modulus, "Expected an expression following % operator"};

            ParseExpressionRHSMultiple(nodeIndex, needsRecovery, info, 3, &TryParseSwitchExpressionFn);

            return true;

        }

        bool TryParseAdditiveExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // additive_expression ::= multiplicative_expression (('+' | '-') multiplicative_expression)*

            if (!TryParseMultiplicativeExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            RhsTypeInfo info[2];
            info[0] = {TokenType::Plus, BinaryExpressionType::Add, "Expected an expression following + operator"};
            info[1] = {TokenType::Minus, BinaryExpressionType::Subtract, "Expected an expression following - operator"};

            ParseExpressionRHSMultiple(nodeIndex, needsRecovery, info, 2, &TryParseMultiplicativeExpressionFn);

            return true;
        }

        bool TryParseShiftExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // shift_expression ::= additive_expression (('<<' | '>>') additive_expression)*

            if (!TryParseAdditiveExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            NodeIndex<ExpressionNode>* lhs = nodeIndex;
            loop:
            TokenType tokenType = b.GetTokenType();
            BinaryExpressionType expressionType;
            const char* failureMessage;

            Marker lhsMarker = b.GetMarker(lhs->index);

            if (tokenType == TokenType::AngleBracketOpen && (b.GetToken().flags & TokenFlags::FollowedByWhitespaceOrComment) == 0 && b.Peek(1).tokenType == TokenType::AngleBracketOpen) {
                expressionType = BinaryExpressionType::ShiftRight;
                failureMessage = "";
                b.AdvanceLexer();
                b.AdvanceLexer();

            }
            else if (tokenType == TokenType::AngleBracketClose && (b.GetToken().flags & TokenFlags::FollowedByWhitespaceOrComment) == 0 && b.Peek(1).tokenType == TokenType::AngleBracketClose) {
                expressionType = BinaryExpressionType::ShiftLeft;
                failureMessage = "";
                b.AdvanceLexer();
                b.AdvanceLexer();
            }
            else {
                return true;
            }

            NodeIndex<ExpressionNode> rhs;
            if (!TryParseAdditiveExpression(&rhs, needsRecovery)) {
                *needsRecovery = true;
                b.Error(b.Mark(), failureMessage);
                Marker m = b.GetMarker(lhs->index);
                *lhs = b.DoneExpression(m, BinaryExpressionNode(expressionType, NodeIndex<ExpressionNode>(lhs->index), NodeIndex<ExpressionNode>(0)));
                return true;
            }
            else if (*needsRecovery) {
                Marker m = b.GetMarker(lhs->index);
                *lhs = b.DoneExpression(m, BinaryExpressionNode(expressionType, NodeIndex<ExpressionNode>(lhs->index), rhs));
                return true;
            }

            Marker m = b.Precede(lhsMarker);
            *lhs = b.DoneExpression(m, BinaryExpressionNode(expressionType, *lhs, rhs));
            goto loop;

        }

        bool TryParseIsExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // is_expression ::= 'is' type_path identifier?
            if (b.GetKeyword() != Keyword::Is) {
                return false;
            }

            Marker m = b.Mark();
            b.AdvanceLexer();

            NodeIndex<TypePathNode> typePath;
            if (!TryParseTypePath(&typePath, needsRecovery)) {
                // we don't set recovery here, maybe we should?
                b.InlineError("Expected a type path as part of an `is` expression");
            }

            FixedCharSpan name;

            if (*needsRecovery) {
                *nodeIndex = b.DoneExpression(m, IsExpressionNode(typePath, name));
                return true;
            }

            if (b.GetTokenType() == TokenType::KeywordOrIdentifier) {
                name = b.GetTokenSource();
                if (b.GetKeyword() != Keyword::NotAKeyword) {
                    b.InlineError("keyword cannot be used as an identifier in `is` expression");
                }
                b.AdvanceLexer();
            }

            *nodeIndex = b.DoneExpression(m, IsExpressionNode(typePath, name));

            return true;
        }

        bool TryParseAsExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // as_expression ::= 'as' '!'? type_path
            if (b.GetKeyword() != Keyword::As) {
                return false;
            }
            Marker m = b.Mark();
            b.AdvanceLexer();

            bool panicOnFailure = false;
            if (b.GetTokenType() == TokenType::Not) {
                b.AdvanceLexer();
                panicOnFailure = true;
            }

            NodeIndex<TypePathNode> type;
            if (!TryParseTypePath(&type, needsRecovery)) {
                // we don't set recovery here, maybe we should?
                b.InlineError("`as` expression requires a type to cast to");
            }

            *nodeIndex = b.DoneExpression(m, AsExpressionNode(panicOnFailure, type));
            return true;
        }

        bool TryParseRelationalExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // relational_expression ::= shift_expression (('<' | '>' | '<=' | '>=') shift_expression | IS isType | AS '!'? type_)*

            if (!TryParseShiftExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            int32 location;

            RhsTypeInfo info[4];
            info[0] = {TokenType::AngleBracketOpen, BinaryExpressionType::LessThan, "Expected an expression following < operator"};
            info[1] = {TokenType::AngleBracketClose, BinaryExpressionType::GreaterThan, "Expected an expression following > operator"};
            info[2] = {TokenType::LessThanEqualTo, BinaryExpressionType::LessThanOrEqualTo, "Expected an expression following <= operator"};
            info[3] = {TokenType::GreaterThanEqualTo, BinaryExpressionType::GreaterThanOrEqualTo, "Expected an expression following >= operator"};

            do {
                location = b.GetTokenIndex();

                ParseExpressionRHSMultiple(nodeIndex, needsRecovery, info, 4, &TryParseShiftExpressionFn);

                Keyword keyword = b.GetKeyword();

                if (keyword == Keyword::Is) {
                    if (!TryParseIsExpression(nodeIndex, needsRecovery)) {
                        *needsRecovery = true;
                        break;
                    }
                }
                else if (keyword == Keyword::As) {
                    if (!TryParseAsExpression(nodeIndex, needsRecovery)) {
                        *needsRecovery = true;
                        break;
                    }
                }

            } while (location != b.GetTokenIndex());

            return true;
        }

        bool TryParseEqualityExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // relational_expression (('==' | '!=') relational_expression)*

            if (!TryParseRelationalExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            RhsTypeInfo info[2];
            info[0] = {TokenType::ConditionalEquals, BinaryExpressionType::Equal, "Expected an expression following == operator"};
            info[1] = {TokenType::ConditionalNotEquals, BinaryExpressionType::NotEqual, "Expected an expression following == operator"};
            ParseExpressionRHSMultiple(nodeIndex, needsRecovery, info, 2, &TryParseRelationalExpressionFn);

            return true;

        }

        bool TryParseBitwiseAndExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // bitwise_and_expression ::= equality_expression ('&' equality_expression)*

            if (!TryParseEqualityExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            ParseExpressionRHS<TokenType::BinaryAnd, BinaryExpressionType::BitwiseAnd>(nodeIndex, needsRecovery, "Expected an expression following & operator", &TryParseEqualityExpressionFn);
            return true;

        }

        bool TryParseBitwiseXorExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // bitwise_xor_expression ::= bitwise_and_expression ('^' bitwise_and_expression)*
            if (!TryParseBitwiseAndExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            ParseExpressionRHS<TokenType::BinaryXor, BinaryExpressionType::BitwiseXor>(nodeIndex, needsRecovery, "Expected an expression following ^ operator", &TryParseBitwiseAndExpressionFn);

            return true;

        }

        bool TryParseBitwiseOrExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // bitwise_or_expression ::= bitwise_xor_expression ('|' bitwise_xor_expression)*

            if (!TryParseBitwiseXorExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            ParseExpressionRHS<TokenType::BinaryOr, BinaryExpressionType::BitwiseOr>(nodeIndex, needsRecovery, "Expected an expression following | operator", &TryParseBitwiseXorExpressionFn);

            return true;
        }

        template<TokenType kOperatorType, BinaryExpressionType kExpressionType>
        void ParseExpressionRHS(NodeIndex<ExpressionNode>* lhs, bool* needsRecovery, const char* failureMessage, TryParseExpressionFn method) {
            while (b.GetTokenType() == kOperatorType) {
                b.AdvanceLexer();
                Marker lhsMarker = b.GetMarker(lhs->index);

                // maybe break if we need recovery

                NodeIndex<ExpressionNode> rhs;
                if (!method(this, &rhs, needsRecovery)) {
                    *needsRecovery = true;
                    b.Error(b.Mark(), failureMessage);
                    Marker m = b.GetMarker(lhs->index);
                    *lhs = b.DoneExpression(m, BinaryExpressionNode(kExpressionType, NodeIndex<ExpressionNode>(lhs->index), NodeIndex<ExpressionNode>(0)));
                    return;
                }
                else if (*needsRecovery) {
                    Marker m = b.GetMarker(lhs->index);
                    *lhs = b.DoneExpression(m, BinaryExpressionNode(kExpressionType, NodeIndex<ExpressionNode>(lhs->index), rhs));
                    return;
                }

                Marker m = b.Precede(lhsMarker);
                *lhs = b.DoneExpression(m, BinaryExpressionNode(kExpressionType, *lhs, rhs));
            }
        }

        bool TryParseConditionalAndExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // conditional_and_expression ::= bitwise_or_expression conditional_and_expression_rhs

            if (!TryParseBitwiseOrExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            ParseConditionalAndExpressionRHS(nodeIndex, needsRecovery);

            return true;
        }

        void ParseConditionalOrExpressionRHS(NodeIndex<ExpressionNode>* lhs, bool* needsRecovery) {
            // conditional_or_expression_rhs ::= ('||'  conditional_and_expression)*

            Marker lhsMarker = b.GetMarker(lhs->index);

            while (b.GetTokenType() == TokenType::ConditionalOr) {

                b.AdvanceLexer();

                NodeIndex<ExpressionNode> rhs;
                if (!TryParseConditionalAndExpression(&rhs, needsRecovery)) {
                    *needsRecovery = true;
                    b.Error(b.Mark(), "Expected an expression following || operator");
                    Marker m = b.GetMarker(lhs->index);
                    *lhs = b.DoneExpression(m, BinaryExpressionNode(BinaryExpressionType::ConditionalOr, NodeIndex<ExpressionNode>(lhs->index), NodeIndex<ExpressionNode>(0)));
                    return;
                }
                else if (*needsRecovery) {
                    Marker m = b.GetMarker(lhs->index);
                    *lhs = b.DoneExpression(m, BinaryExpressionNode(BinaryExpressionType::ConditionalOr, NodeIndex<ExpressionNode>(lhs->index), rhs));
                    return;
                }

                Marker m = b.Precede(lhsMarker);
                // should have 'lhs', 'op', 'rhs' now

                *lhs = b.DoneExpression(m, BinaryExpressionNode(BinaryExpressionType::ConditionalOr, *lhs, rhs));
                lhsMarker = b.GetMarker(lhs->index);
            }

        }

        bool TryParseConditionalOrExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // conditional_or_expression ::= conditional_and_expression conditional_or_expression_rhs

            if (!TryParseConditionalAndExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            ParseConditionalOrExpressionRHS(nodeIndex, needsRecovery);

            return true;
        }

        bool TryParseNullCoalescingExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // null_coalescing_expression ::= conditional_or_expression ('??' (null_coalescing_expression | throw_expression))?

            if (!TryParseConditionalOrExpression(nodeIndex, needsRecovery)) {
                return false;
            }

            if (*needsRecovery) {
                return true;
            }

            if (b.GetTokenType() == TokenType::Coalesce) {
                Marker m = b.Precede(b.GetMarker(nodeIndex->index));

                b.AdvanceLexer();

                NodeIndex<ExpressionNode> rhs;
                if (TryParseNullCoalescingExpression(&rhs, needsRecovery)) {
                    *nodeIndex = b.DoneExpression(m, NullCoalescingExpressionNode(*nodeIndex, rhs));
                    return true;
                }
                else if (TryParseThrowableExpression(&rhs, needsRecovery)) {
                    *nodeIndex = b.DoneExpression(m, NullCoalescingExpressionNode(*nodeIndex, rhs));
                    return true;
                }
                else {
                    *needsRecovery = true;
                    b.InlineError("Expected a valid right hand side expression after ?? operator");
                    *nodeIndex = b.DoneExpression(m, NullCoalescingExpressionNode(*nodeIndex, NodeIndex<ExpressionNode>(0)));
                    return true;
                }

            }

            return true;
        }

        bool TryParseThrowableExpression(NodeIndex<ExpressionNode>* nodeIndex, bool* needsRecovery) {
            // throwable_expression :: 'throw' expression | expression
            NodeIndex<ExpressionNode> expression;

            if (b.GetKeyword() == Keyword::Throw) {
                Marker m = b.Mark();
                b.AdvanceLexer();

                // pinned if we see a throw statement

                if (!TryParseExpression(&expression, needsRecovery)) {
                    // requires recovery
                    b.Error(b.Mark(), "Expected a valid expression after 'throw'");
                    *needsRecovery = true;
                }

                *nodeIndex = b.DoneExpression(m, ThrowExpressionNode(expression));
                return true;
            }

            return TryParseExpression(&expression, needsRecovery);

        }

        bool TryParseMethodMember(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePathNode, NodeIndex<IdentifierNode> identifier, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // method_declaration ::= identifier type_parameter_list? '(' formal_parameter_list ')' type_parameter_constraints? (block | '=>' throwable_expression ';')

            NodeIndex<TypeParameterListNode> typeParameters;
            NodeIndex<FormalParameterListNode> parameters;
            // NodeIndex<ParameterTypeConstraintNode> typeConstraints;
            NodeIndex<BlockNode> body;
            NodeIndex<ExpressionNode> arrowBody;

            if (b.GetTokenType() != TokenType::AngleBracketOpen && b.GetTokenType() != TokenType::OpenParen) {
                return false;
            }

            TryParseTypeParameterList(&typeParameters);

            if (!TryParseFormalParameterList(&parameters, true)) {

                // look for 'where' or { or '=>'
                Marker e = b.Mark();
                while (!b.EndOfInput()) {
                    if (b.GetTokenType() == TokenType::CurlyBraceOpen || b.GetTokenType() == TokenType::FatArrow) {
                        break;
                    }
                    b.AdvanceLexer();
                }

                b.Error(e, "Expected a parameter list");
                *needsRecovery = true;

            }

            if (b.GetTokenType() == TokenType::FatArrow) {
                b.AdvanceLexer();

                if (!TryParseThrowableExpression(&arrowBody, needsRecovery)) {
                    b.InlineError("Expected an expression");
                    *needsRecovery = b.GetTokenType() != TokenType::SemiColon;
                }

                ExpectSemicolon();

            }
            else if (!TryParseBlock(&body)) {
                b.InlineError("Expected a block or => expression");
            }

            *nodeIndex = b.DoneDeclaration(m, MethodDeclarationNode(modifiers, typePathNode, identifier, typeParameters, parameters, body, arrowBody));
            return true;
        }

        bool TryParseDelegateDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // delegate_declaration ::= 'delegate' type_path identifier type_argument_list formal_parameter_list type_parameter_constraints? ';'
            if (b.GetKeyword() != Keyword::Delegate) {
                return false;
            }
            b.AdvanceLexer();

            NodeIndex<TypePathNode> typePath;
            NodeIndex<IdentifierNode> identifier;
            NodeIndex<TypeParameterListNode> typeParameters;
            NodeIndex<FormalParameterListNode> parameters;

            bool isVoid = false;
            if (b.GetKeyword() == Keyword::Void) {
                b.AdvanceLexer();
                isVoid = true;
            }
            else if (!TryParseTypePath(&typePath, needsRecovery)) {
                b.InlineError("Expected a return type");
                *needsRecovery = b.GetTokenType() != TokenType::KeywordOrIdentifier;
            }

            if (*needsRecovery) {
                *nodeIndex = b.DoneDeclaration(m, DelegateDeclarationNode(modifiers, isVoid, typePath, identifier, typeParameters, parameters));
                return true;
            }

            if (!TryParseIdentifier(&identifier, true)) {
                b.InlineError("Expected an identifier");
                *needsRecovery = b.GetTokenType() != TokenType::AngleBracketOpen && b.GetTokenType() != TokenType::OpenParen;
            }

            if (*needsRecovery) {
                *nodeIndex = b.DoneDeclaration(m, DelegateDeclarationNode(modifiers, isVoid, typePath, identifier, typeParameters, parameters));
                return true;
            }

            TryParseTypeParameterList(&typeParameters);

            *needsRecovery = *needsRecovery ? b.GetTokenType() != TokenType::OpenParen : false;

            if (*needsRecovery) {
                *nodeIndex = b.DoneDeclaration(m, DelegateDeclarationNode(modifiers, isVoid, typePath, identifier, typeParameters, parameters));
                return true;
            }

            if (!TryParseFormalParameterList(&parameters, false)) {
                b.InlineError("expected a parameter list");
                *needsRecovery = b.GetTokenType() != TokenType::SemiColon;
            }

            ExpectSemicolon();

            *nodeIndex = b.DoneDeclaration(m, DelegateDeclarationNode(modifiers, isVoid, typePath, identifier, typeParameters, parameters));
            return true;

        }

        bool TryParseConstructorDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // constructor_declaration ::= 'constructor' identifier? formal_parameter_list block

            if (b.GetKeyword() != Keyword::Constructor) {
                return false;
            }

            b.AdvanceLexer();

            NodeIndex<IdentifierNode> identifier;
            NodeIndex<FormalParameterListNode> parameters;
            NodeIndex<BlockNode> body;

            TryParseIdentifier(&identifier, true);

            if (!TryParseFormalParameterList(&parameters, true)) {
                *needsRecovery = b.GetTokenType() != TokenType::CurlyBraceOpen;
                b.InlineError("Expected a parameter list");
                *nodeIndex = b.DoneDeclaration(m, ConstructorDeclarationNode(modifiers, identifier, parameters, body));
                return true;
            }

            if (!TryParseBlock(&body)) {
                b.InlineError("Expected a constructor body");
            }

            *nodeIndex = b.DoneDeclaration(m, ConstructorDeclarationNode(modifiers, identifier, parameters, body));
            return true;

        }

        bool TryParseIndexerParameterList(NodeIndex<IndexerParameterListNode>* nodeIndex) {
            if (b.GetTokenType() != TokenType::SquareBraceOpen) {
                return false;
            }

            Marker m = b.Mark();

            NodeIndex<FormalParameterNode> first;
            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_SquareBrace(&stream);

            b.PushStream(&stream);

            ParseCommaSeparatedList(&first, false, &TryParseFormalParameterWithRecoveryFn);

            b.PopStream();
            *nodeIndex = b.Done(m, IndexerParameterListNode(first));
            return true;
        }

        bool TryParseIndexerDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<TypePathNode> typePath, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // indexer_declaration ::= 'this' indexer_parameter_list property_body
            if (b.GetKeyword() != Keyword::This) {
                return false;
            }

            b.AdvanceLexer();

            PsiBuilder2::SubStream stream;
            NodeIndex<IndexerParameterListNode> parameterList;
            NodeIndex<PropertySetterNode> setter;
            NodeIndex<PropertyGetterNode> getter;

            if (!TryParseIndexerParameterList(&parameterList)) {
                *needsRecovery = true;
                *nodeIndex = b.ErrorDeclaration(m, "Expected an indexer declaration");
                return true;
            }

            return TryParsePropertyDeclarationBody<IndexerDeclarationNode, NodeIndex<IndexerParameterListNode>>(m, modifiers, typePath, parameterList, nodeIndex, needsRecovery);

        }

        bool TryParseMemberDeclaration(NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // member_declaration ::= constructor_declaration
            //                      | method_declaration
            //                      | property_declaration
            //                      | indexer_declaration
            //                      | operator_declaration
            //                      | field_declaration

            int32 start = b.GetTokenIndex();

            Marker m = b.Mark();

            NodeIndex<ModifierListNode> modifiers;
            TryParseModifierList(&modifiers);

            switch (b.GetKeyword()) {
                case Keyword::Const: {
                    TryParseConstantDeclaration(m, modifiers, nodeIndex, needsRecovery);
                    return true;
                }
                case Keyword::Constructor: {
                    TryParseConstructorDeclaration(m, modifiers, nodeIndex, needsRecovery);
                    return true;
                }
                case Keyword::Delegate: {
                    TryParseDelegateDeclaration(m, modifiers, nodeIndex, needsRecovery);
                    return true;
                }
                case Keyword::Class: {
                    TryParseClassDeclaration(m, modifiers, nodeIndex, needsRecovery);
                    return true;
                }
                case Keyword::Struct: {
                    TryParseStructDeclaration(m, modifiers, nodeIndex, needsRecovery);
                    return true;
                }
                case Keyword::Interface: {
                    TryParseInterfaceDeclaration(m, modifiers, nodeIndex, needsRecovery);
                    return true;
                }
                case Keyword::Enum: {
                    TryParseEnumDeclaration(m, modifiers, nodeIndex, needsRecovery);
                    return true;
                }
                default: {
                    NodeIndex<TypePathNode> typePath;

                    if (!TryParseTypePath(&typePath, needsRecovery)) {

                        if (modifiers.IsValid()) {
                            // return an invalid declaration type
                            // skip until top level reset or block
                            *needsRecovery = true;
                            *nodeIndex = b.ErrorDeclaration(m, "unexpected token");
                            return true;
                        }
                        else {
                            *needsRecovery = false;
                            b.RollbackTo(m);
                            return false;
                        }
                    }

                    // public float x() method
                    // public float x<T>() method
                    // public float x { property
                    // public float x (; || '=') field

                    if (b.GetKeyword() == Keyword::This) {
                        if (!TryParseIndexerDeclaration(m, modifiers, typePath, nodeIndex, needsRecovery)) {
                            *nodeIndex = b.ErrorDeclaration(m, "Expected an indexer declaration");
                        }
                        return true;
                    }

                    NodeIndex<IdentifierNode> identifier;
                    if (!TryParseIdentifier(&identifier, true)) {
                        *needsRecovery = true;
                        *nodeIndex = b.ErrorDeclaration(m, "Expected an identifier as part of a member declaration");
                        return true;
                    }

                    if (TryParseMethodMember(m, modifiers, typePath, identifier, nodeIndex, needsRecovery)) {
                        return true;
                    }

                    if (TryParsePropertyDeclaration(m, modifiers, typePath, identifier, nodeIndex, needsRecovery)) {
                        return true;
                    }

                    if (TryParseFieldDeclaration(m, modifiers, typePath, identifier, nodeIndex, needsRecovery)) {
                        return true;
                    }

                    // if we've consumed some tokens we'll need to do something with them

                    *nodeIndex = b.ErrorDeclaration(m, "Expected a member declaration");
                    return true;
                }

            }

            if (modifiers.IsValid()) {
                *nodeIndex = b.ErrorDeclaration(m, "Expected a member declaration");
                return true;
            }

            b.RollbackTo(m);
            return false;

        }

        void LinkDeclarations(NodeIndex<DeclarationNode>* start, NodeIndex<DeclarationNode> next, NodeIndex<DeclarationNode>* ptr) {
            if (start->IsValid()) {
                b.GetPsiNodeUnsafe(*ptr)->next = next;
            }
            else {
                *start = next;
            }
            *ptr = next;
        }

        bool TryParseClassBody(NodeIndex<ClassBodyNode>* nodeIndex) {
            PsiBuilder2::SubStream subStream;

            if (b.GetTokenType() != TokenType::CurlyBraceOpen) {
                return false;
            }

            Marker m = b.Mark();

            b.TryGetSubSubStream_CurlyBracket(&subStream);

            b.PushStream(&subStream);

            NodeIndex<DeclarationNode> declarationStart;
            NodeIndex<DeclarationNode> ptr;

            // this is a recovery scope
            while (!b.EndOfInput()) {

                NodeIndex<DeclarationNode> declaration;

                if (b.GetTokenType() == TokenType::SemiColon) {
                    b.AdvanceLexer();
                    continue;
                }

                // recovery case
                if (b.GetTokenType() == TokenType::CurlyBraceOpen) {
                    Marker r = b.Mark();
                    NodeIndex<BlockNode> throwaway;
                    TryParseBlock(&throwaway);
                    declaration = b.DoneDeclaration(r, InvalidDeclarationNode(throwaway));
                    LinkDeclarations(&declarationStart, declaration, &ptr);
                    continue;
                }

                bool recovery = false;
                if (TryParseMemberDeclaration(&declaration, &recovery)) {
                    LinkDeclarations(&declarationStart, declaration, &ptr);
                }
                else {
                    recovery = true;
                }

                if (recovery) {
                    // class, struct, enum, interface, public, private, protected, internal, virtual, sealed, override, abstract
                    // { }
                    // ;
                    Marker e = b.Mark();

                    while (!b.EndOfInput()) {

                        if (b.GetTokenType() == TokenType::CurlyBraceOpen || b.GetTokenType() == TokenType::SemiColon) {
                            break;
                        }

                        switch (b.GetKeyword()) {
                            case Keyword::Public:
                            case Keyword::Export:
                            case Keyword::Private:
                            case Keyword::Internal:
                            case Keyword::Protected:
                            case Keyword::Class:
                            case Keyword::Struct:
                            case Keyword::Enum:
                            case Keyword::Interface:
                            case Keyword::Virtual:
                            case Keyword::Void:
                            case Keyword::Sealed:
                            case Keyword::Abstract:
                            case Keyword::Override:
                                goto end_recovery;
                            default: {
                                b.AdvanceLexer();
                                break;
                            }
                        }

                    }
                    end_recovery:
                    b.Error(e, "Expected a member declaration");

                }

            }

            b.PopStream();

            *nodeIndex = b.Done(m, ClassBodyNode(declarationStart));
            return true;

        }

        bool TryParseEnumMemberDeclaration(NodeIndex<EnumMemberDeclarationNode>* nodeIndex) {
            // enum_member_declaration ::= identifier ('=' expression)?
            NodeIndex<IdentifierNode> identifier;
            NodeIndex<ExpressionNode> expression;
            Marker m = b.Mark();

            if (!TryParseIdentifier(&identifier, true)) {
                b.RollbackTo(m);
                return false;
            }

            if (b.GetTokenType() == TokenType::Assign) {
                b.AdvanceLexer();

                bool recovery = false;

                if (!TryParseExpression(&expression, &recovery)) {
                    b.InlineError("Expected an expression");
                }

            }
            *nodeIndex = b.Done(m, EnumMemberDeclarationNode(identifier, expression));
            return true;

        }

        static bool TryParseEnumMemberFn(ParserImpl* parser, NodeIndex<EnumMemberDeclarationNode>* first) {
            return parser->TryParseEnumMemberDeclaration(first);
        }

        bool TryParseEnumMemberList(NodeIndex<EnumMemberListNode>* nodeIndex, bool* needsRecovery) {
            // enum_member_list ::= '{' (enum_member_declaration (',' enum_member_declaration)*)? '}'

            if (b.GetTokenType() != TokenType::CurlyBraceOpen) {
                return false;
            }
            Marker m = b.Mark();

            NodeIndex<EnumMemberDeclarationNode> first;
            PsiBuilder2::SubStream stream;
            b.TryGetSubSubStream_CurlyBracket(&stream);
            b.PushStream(&stream);
            ParseCommaSeparatedList(&first, true, &TryParseEnumMemberFn);
            b.PopStream();
            return true;

        }

        bool TryParseEnumDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // enum_declaration ::= 'enum' identifier (':' builtin_type_name)? enum_member_declaration_list

            if (b.GetKeyword() != Keyword::Enum) {
                return false;
            }

            b.AdvanceLexer();

            NodeIndex<IdentifierNode> identifier;
            NodeIndex<TypePathNode> baseType;
            NodeIndex<EnumMemberListNode> members;

            if (!TryParseIdentifier(&identifier, true)) {
                b.InlineError("expected an enum name");
                *needsRecovery = b.GetTokenType() != TokenType::Colon && b.GetTokenType() != TokenType::CurlyBraceOpen;

                if (*needsRecovery) {
                    *nodeIndex = b.DoneDeclaration(m, EnumDeclarationNode(modifiers, identifier, baseType, members));
                    return true;
                }
            }

            if (b.GetTokenType() == TokenType::Colon) {

                b.AdvanceLexer();
                if (!TryParseTypePath(&baseType, needsRecovery)) {
                    b.InlineError("expected a backing type");
                    *needsRecovery = b.GetTokenType() != TokenType::CurlyBraceOpen;
                }

                if (*needsRecovery) {
                    *nodeIndex = b.DoneDeclaration(m, EnumDeclarationNode(modifiers, identifier, baseType, members));
                    return true;
                }

            }

            if (!TryParseEnumMemberList(&members, needsRecovery)) {
                b.InlineError("expected an enum body");
                *needsRecovery = true;
            }

            *nodeIndex = b.DoneDeclaration(m, EnumDeclarationNode(modifiers, identifier, baseType, members));
            return true;

        }

        bool TryParseInterfaceDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // assert(false && "interface not implemented");
            return false;
        }

        bool TryParseStructDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // struct_declaration ::= 'struct' identifier type_parameter_list? base_list? type_parameter_constraints? class_body
            if (b.GetKeyword() != Keyword::Struct) {
                return false;
            }

            b.AdvanceLexer();
            NodeIndex<IdentifierNode> identifier;
            NodeIndex<TypeListNode> baseType;
            NodeIndex<TypeParameterListNode> typeParameters;
            NodeIndex<ClassBodyNode> body;

            if (!TryParseIdentifier(&identifier, true)) {
                b.InlineError("Expected identifier");
                // skip to body I guess, maybe can handle : or < alignment
                if (b.GetTokenType() != TokenType::Colon && b.GetTokenType() != TokenType::AngleBracketOpen) {
                    AdvanceToCurlyOpen();
                }

            }

            TryParseTypeParameterList(&typeParameters);

            if (TryParseBaseList(&baseType, needsRecovery)) {

                if (*needsRecovery) {
                    AdvanceToCurlyOpen(); // I guess this can fail, but then that's catastrophic
                    *needsRecovery = false;
                }

            }

            // todo type constraints

            if (!TryParseClassBody(&body)) {
                b.InlineError("Expected a type body");
                *needsRecovery = true;
            }

            *nodeIndex = b.DoneDeclaration(m, StructDeclarationNode(modifiers, identifier, typeParameters, baseType, body));
            return true;
        }

        bool TryParseClassDeclaration(Marker m, NodeIndex<ModifierListNode> modifiers, NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // class_declaration ::= 'class' identifier type_parameter_list? base_list? type_parameter_constraints? class_body

            if (b.GetKeyword() != Keyword::Class) {
                return false;
            }

            b.AdvanceLexer();
            NodeIndex<IdentifierNode> identifier;
            NodeIndex<TypeListNode> baseType;
            NodeIndex<TypeParameterListNode> typeParameters;
            NodeIndex<ClassBodyNode> body;

            if (!TryParseIdentifier(&identifier, true)) {
                b.InlineError("Expected identifier");
                // skip to body I guess, maybe can handle : or < alignment
                if (b.GetTokenType() != TokenType::Colon && b.GetTokenType() != TokenType::AngleBracketOpen) {
                    AdvanceToCurlyOpen();
                }

            }

            TryParseTypeParameterList(&typeParameters);

            if (TryParseBaseList(&baseType, needsRecovery)) {

                if (*needsRecovery) {
                    AdvanceToCurlyOpen(); // I guess this can fail, but then that's catastrophic
                    *needsRecovery = false;
                }

            }

            // todo type constraints

            if (!TryParseClassBody(&body)) {
                b.InlineError("Expected a type body");
                *needsRecovery = true;
            }

            *nodeIndex = b.DoneDeclaration(m, ClassDeclarationNode(modifiers, identifier, typeParameters, baseType, body));
            return true;


        }

        bool TryParseTypeDeclaration(NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // type_declaration ::= attributes? modifier_list? (class_declaration | struct_declaration | enum_declaration)

            Marker m = b.Mark();

            NodeIndex<ModifierListNode> modifiers;

            TryParseModifierList(&modifiers);

            if (TryParseClassDeclaration(m, modifiers, nodeIndex, needsRecovery)) {
                return true;
            }

            if (TryParseStructDeclaration(m, modifiers, nodeIndex, needsRecovery)) {
                return true;
            }

            if (TryParseEnumDeclaration(m, modifiers, nodeIndex, needsRecovery)) {
                return true;
            }

            if (TryParseInterfaceDeclaration(m, modifiers, nodeIndex, needsRecovery)) {
                return true;
            }

            if (TryParseDelegateDeclaration(m, modifiers, nodeIndex, needsRecovery)) {
                return true;
            }

            if (modifiers.IsValid()) {
                *nodeIndex = b.ErrorDeclaration(m, "Expected a declaration");
                return true;
            }

            b.RollbackTo(m);
            return false;

        }

        bool TryParseTopLevelUsingDeclaration(NodeIndex<DeclarationNode>* nodeIndex, bool* needsRecovery) {
            // using_directive ::= 'using' identifier '=' type_path';'
            //                   | 'using' namespace_path ';'
            //                   | 'using' 'static' type_path ';'


            if (b.GetKeyword() != Keyword::Using) {
                return false;
            }

            Marker m = b.Mark();

            b.AdvanceLexer();

            if (b.GetKeyword() == Keyword::Static) {

                b.AdvanceLexer();
                NodeIndex<TypePathNode> typePath;
                if (!TryParseTypePath(&typePath, needsRecovery)) {
                    b.InlineError("Expected a type path following 'using static'");
                    *needsRecovery = true;
                }

                *needsRecovery = *needsRecovery || !ExpectSemicolon();
                *nodeIndex = b.DoneDeclaration(m, UsingStaticNode(typePath));
                return true;
            }
            else if (b.GetTokenType() == TokenType::KeywordOrIdentifier && b.Peek(1).tokenType == TokenType::Assign) {
                FixedCharSpan name = b.GetTokenSource();
                NodeIndex<TypePathNode> typePath;
                if (!TryParseTypePath(&typePath, needsRecovery)) {
                    b.InlineError("Expected a type path following using alias declaration");
                    *needsRecovery = true;
                }

                *needsRecovery = *needsRecovery || !ExpectSemicolon();
                *nodeIndex = b.DoneDeclaration(m, UsingAliasNode(name, typePath));
                return true;
            }
            else {

                // Namespace;
                // Namespace::Namespace;

                NodeIndex<NamespacePathNode> namespacePath;
                if (!TryParseNamespacePath(&namespacePath, needsRecovery)) {
                    b.InlineError("Expected a namespace path without type arguments following using declaration");
                    *needsRecovery = true;
                }

                *needsRecovery = *needsRecovery || !ExpectSemicolon();
                *nodeIndex = b.DoneDeclaration(m, UsingNamespaceNode(namespacePath));
                return true;
            }

        }

        static bool IsTopLevelRecoveryToken(Token token) {
            switch (token.tokenType) {
                case TokenType::SemiColon:
                    return true;
                case TokenType::KeywordOrIdentifier: {
                    switch (token.keyword) {
                        case Keyword::Static:
                        case Keyword::Public:
                        case Keyword::Export:
                        case Keyword::Protected:
                        case Keyword::Virtual:
                        case Keyword::Abstract:
                        case Keyword::Sealed:
                        case Keyword::Internal:
                        case Keyword::Interface:
                        case Keyword::Enum:
                        case Keyword::Using:
                        case Keyword::Class:
                        case Keyword::Struct:
                        case Keyword::Delegate:
                        case Keyword::Const:
                        case Keyword::Extern:
                        case Keyword::Namespace:
                            return true;
                        default:
                            return false;
                    }
                }
                default: {
                    return false;
                }
            }
        }

        bool TryParseFile(char* src, int32 srcLength, ParseResult* parseResult, LinearAllocator * outputAllocator) {

            parseResult->src = src;
            parseResult->srcLength = srcLength;

            if (!b.Initialize(parseResult, outputAllocator)) {
                return false;
            }

            // if the file just comments and whitespace we don't need to parse anything
            if (!parseResult->hasNonTrivialContent) {
                return true;
            }

            int32 stuckCount = 0;
            int32 firstStuckIndex = 0;

            NodeIndex<DeclarationNode> ptr;

            while (!b.EndOfInput()) {

                int32 tokenIndex = b.GetTokenIndex();

                if (b.GetTokenType() == TokenType::EndOfInput) {
                    b.AdvanceLexer();
                    break;
                }

                NodeIndex<DeclarationNode> topLevelDeclaration;

                bool needsRecovery = false;
//                if (TryParseMethodDeclaration(&methodDeclaration, &needsRecovery)) {
//                    topLevelDeclarationIndices.Add(methodDeclaration.index);
//
//                    if (stuckCount > 0) {
//                        Marker stuckMarker = b.MarkFromToken(firstStuckIndex);
//                        b.Error(stuckMarker, "parser stuck");
//                        stuckCount = 0;
//                        firstStuckIndex = 0;
//                    }
//
//                    continue;
//                }

                if (b.GetTokenType() == TokenType::SemiColon) {
                    b.AdvanceLexer();
                    continue;
                }

                if (TryParseTopLevelUsingDeclaration(&topLevelDeclaration, &needsRecovery)) {
                    goto postamble;
                }

                if (TryParseTypeDeclaration(&topLevelDeclaration, &needsRecovery)) {
                    goto postamble;
                }

                if (tokenIndex == b.GetTokenIndex()) {
                    if (stuckCount == 0) {
                        firstStuckIndex = tokenIndex;
                    }
                    stuckCount++;
                    b.AdvanceLexer();
                }

                continue;
                postamble:

                FileNode* fileNode = b.GetPsiNodeUnsafe(NodeIndex<FileNode>(1));

                if (!fileNode->firstDeclaration.IsValid()) {
                    fileNode->firstDeclaration = topLevelDeclaration;
                    ptr = topLevelDeclaration;
                }
                else {
                    b.GetPsiNodeUnsafe(ptr)->next = topLevelDeclaration;
                    ptr = topLevelDeclaration;
                }

                if (needsRecovery) {

                    b.ClearStreams();

                    while (!b.EndOfInput()) {
                        if (IsTopLevelRecoveryToken(b.GetToken())) {
                            break;
                        }
                        b.AdvanceLexer();
                    }
                }

                if (stuckCount > 0) {
                    Marker stuckMarker = b.MarkFromToken(firstStuckIndex);
                    b.Error(stuckMarker, "parser stuck");
                    stuckCount = 0;
                    firstStuckIndex = 0;
                }

            }

            if (stuckCount > 0) {
                Marker stuckMarker = b.MarkFromToken(firstStuckIndex);
                b.Error(stuckMarker, "parser stuck");
            }

            return b.Finalize(parseResult, outputAllocator);

        }

        bool TryParseIdentifierOrBuiltInType(NodeIndex<IdentifierNode>* nodeIndex) {
            if (b.GetTokenType() != TokenType::KeywordOrIdentifier) {
                return false;
            }
            Marker m = b.Mark();
            FixedCharSpan name = b.GetTokenSource();

            Keyword keyword = b.GetKeyword();

            b.AdvanceLexer();

            BuiltInTypeName builtIn = BuiltInTypeNameFromKeyword(keyword);

            if (builtIn == BuiltInTypeName::Invalid && keyword != Keyword::NotAKeyword) {
                b.ErrorArgs(b.Mark(), "Reserved keyword `%s` cannot be used as an identifier", KeywordToString(keyword));
            }

            NodeIndex<TypeArgumentListNode> typeArguments;
            *nodeIndex = b.Done(m, IdentifierNode(name, typeArguments));

            return true;
        }

        bool TryParseIdentifier(NodeIndex<IdentifierNode>* nodeIndex, bool warnAboutKeywords) {
            if (b.GetTokenType() != TokenType::KeywordOrIdentifier) {
                return false;
            }
            Marker m = b.Mark();
            FixedCharSpan name = b.GetTokenSource();

            Keyword keyword = b.GetKeyword();

            b.AdvanceLexer();

            if (warnAboutKeywords && keyword != Keyword::NotAKeyword) {
                b.ErrorArgs(b.Mark(), "Reserved keyword `%s` cannot be used as an identifier", KeywordToString(keyword));
            }

            NodeIndex<TypeArgumentListNode> typeArguments;
            *nodeIndex = b.Done(m, IdentifierNode(name, typeArguments));

            return true;
        }

        bool TryParseIdentifierWithTypeArguments(NodeIndex<IdentifierNode>* nodeIndex, bool* needsRecovery, bool typeArgsRequired) {
            // identifier_with_type_arguments ::= identifier type_argument_list?
            if (b.GetTokenType() != TokenType::KeywordOrIdentifier) {
                return false;
            }
            Marker m = b.Mark();
            FixedCharSpan name = b.GetTokenSource();
            b.AdvanceLexer();

            NodeIndex<TypeArgumentListNode> typeArguments;
            TryParseTypeArgumentList(&typeArguments, needsRecovery, typeArgsRequired);

            *nodeIndex = b.Done(m, IdentifierNode(name, typeArguments));

            return true;

        }

        bool TryParseTypePath(NodeIndex<TypePathNode>* nodeIndex, bool* needsRecovery) {
            // type_path ::= identifier ('::' identifier)* type_argument_list? array_rank? ('?')?

            if (b.GetTokenType() != TokenType::KeywordOrIdentifier) {
                return false;
            }

            Marker m = b.Mark();

            NodeIndex<IdentifierNode> first;

            BuiltInTypeName builtInTypeName = BuiltInTypeNameFromKeyword(b.GetKeyword());

            if (!TryParseIdentifierOrBuiltInType(&first)) {
                b.RollbackTo(m);
                return false;
            }

            NodeIndex<IdentifierNode> ptr = first;
            NodeIndex<ArrayRankNode> arrayRank;
            bool isNullableType = false;

            TokenType tokenType = b.GetTokenType();

            while (tokenType == TokenType::DoubleColon) {

                b.AdvanceLexer();

                NodeIndex<IdentifierNode> next;
                if (!TryParseIdentifier(&next, true)) {
                    b.CompletionOpportunity("expected an identifier after ::");
                    *needsRecovery = true;
                    break;
                }

                b.GetPsiNodeUnsafe(ptr)->next = next;
                ptr = next;

                tokenType = b.GetTokenType();

                if (*needsRecovery) {
                    break;
                }
            }

            NodeIndex<TypeArgumentListNode> typeArgumentList;
            TryParseTypeArgumentList(&typeArgumentList, needsRecovery, true);

            if (b.GetTokenType() == TokenType::QuestionMark) {
                isNullableType = true;
                b.AdvanceLexer();
            }

            TryParseArrayRank(&arrayRank);

            *nodeIndex = b.Done(m, TypePathNode(first, builtInTypeName, typeArgumentList, arrayRank, isNullableType));
            return true;

        }

        bool TryParseArrayRank(NodeIndex<ArrayRankNode>* nodeIndex) {
            // array_rank ::= '[' ']' '?'

            // we don't support multi dimensional arrays

            if (b.GetTokenType() != TokenType::SquareBraceOpen) {
                return false;
            }

            Marker m = b.Mark();

            PsiBuilder2::SubStream subStream;
            b.TryGetSubSubStream_SquareBrace(&subStream); // can't fail, we pre-processed the file already

            if (subStream.HasMoreTokens()) {
                Marker e = b.Mark();
                while (subStream.HasMoreTokens()) {
                    b.AdvanceLexer();
                }
                b.Error(e, "Array rank should not have content, multi-dimensional arrays are not supported");
            }

            subStream.Done();

            bool isNullable = false;
            if (b.GetTokenType() == TokenType::QuestionMark) {
                b.AdvanceLexer();
                isNullable = true;
            }
            *nodeIndex = b.Done(m, ArrayRankNode(isNullable));
            return true;

        }

        static bool IsTypePathToken(Token token) {
            TokenType tokenType = token.tokenType;
            return (tokenType == TokenType::AngleBracketOpen ||
                    tokenType == TokenType::AngleBracketClose ||
                    tokenType == TokenType::Comma ||
                    tokenType == TokenType::DoubleColon ||
                    (tokenType == TokenType::KeywordOrIdentifier && BuiltInTypeNameFromKeyword(token.keyword) != BuiltInTypeName::Invalid));
        }

        bool TryParseNamespaceOrTypeName(NodeIndex<NamespaceOrTypeNameNode>* nodeIndex, bool* needsRecovery) {
            // namespace_or_type_name ::= identifier type_argument_list? ('::' identifier type_argument_list?)*

            if (b.GetTokenType() != TokenType::KeywordOrIdentifier) {
                return false;
            }

            Marker m = b.Mark();

            Keyword firstKeyword = b.GetKeyword();
            BuiltInTypeName builtInTypeName = BuiltInTypeNameFromKeyword(firstKeyword);
            FixedCharSpan name = b.GetTokenSource();

            // maybe also warn about using a built in type name? or let sema handle that?
            if (firstKeyword != Keyword::NotAKeyword && builtInTypeName == BuiltInTypeName::Invalid) {
                const char* c = KeywordToString(firstKeyword);
                b.ErrorArgs(b.Mark(), "keyword %s cannot be used as a type name", c);
            }

            NodeIndex<NamespaceOrTypeNameNode> next;
            NodeIndex<TypeArgumentListNode> typeArguments;

            b.AdvanceLexer();

            if (TryParseTypeArgumentList(&typeArguments, needsRecovery, true)) {

                if (*needsRecovery) {
                    *nodeIndex = b.Done(m, NamespaceOrTypeNameNode(name, builtInTypeName, typeArguments, next));
                    return true;
                }

            }

            if (b.GetTokenType() == TokenType::DoubleColon) {

                b.AdvanceLexer();
                if (!TryParseNamespaceOrTypeName(&next, needsRecovery)) {
                    b.InlineError("Expected a type name following ::");
                    *needsRecovery = true;
                }

            }

            *nodeIndex = b.Done(m, NamespaceOrTypeNameNode(name, builtInTypeName, typeArguments, next));
            return true;

        }

        bool TryParseTypeParameterList(NodeIndex<TypeParameterListNode>* nodeIndex) {
            // type_parameter_list ::= '<' identifier (',' identifier)* '>'
            if (b.GetTokenType() != TokenType::AngleBracketOpen) {
                return false;
            }

            Marker m = b.Mark();
            PsiBuilder2::SubStream stream;
            NodeIndex<IdentifierNode> first;
            NodeIndex<IdentifierNode> next;
            NodeIndex<IdentifierNode> ptr;

            if (!b.TryGetSubSubStream(&stream, TokenType::AngleBracketOpen, TokenType::AngleBracketClose)) {
                b.AdvanceLexer();

                // consume until we hit a non comma or identifier token
                // todo -- this could be improved maybe to parse until we fail

                TryParseIdentifier(&first, true);

                Marker e = b.Mark();

                while (!b.EndOfInput()) {

                    TokenType tokenType = b.GetTokenType();

                    if (tokenType != TokenType::Comma && !(tokenType == TokenType::KeywordOrIdentifier && b.GetKeyword() == Keyword::NotAKeyword)) {
                        break;
                    }

                    b.AdvanceLexer();

                }

                b.Error(e, "Opening < isn't matched by >");
                *nodeIndex = b.Done(m, TypeParameterListNode(first));
                return true;
            }

            b.PushStream(&stream);

            if (!TryParseIdentifier(&first, true)) {
                b.PopStream();
                *nodeIndex = b.Done(m, TypeParameterListNode(first));
                return true;
            }

            ptr = first;

            while (!b.EndOfInput()) {

                if (b.GetTokenType() != TokenType::Comma) {
                    Marker e = b.Mark();
                    b.AdvanceToStreamEnd();
                    b.Error(e, "Expected a comma or > after type parameter");
                    break;
                }

                b.AdvanceLexer();

                if (TryParseIdentifier(&next, true)) {
                    b.GetPsiNodeUnsafe(ptr)->next = next;
                    ptr = next;
                }
                else {
                    Marker e = b.Mark();
                    b.AdvanceToStreamEnd();
                    b.Error(e, "Expected an identifier following comma");
                    break;
                }

            }

            b.PopStream();

            *nodeIndex = b.Done(m, TypeParameterListNode(first));
            return true;

        }

        bool TryParseTypeArgumentList(NodeIndex<TypeArgumentListNode>* nodeIndex, bool* needsRecovery, bool mustBeValid) {
            // type_argument_list ::= '<' type_path (',' type_path)* '>'

            if (b.GetTokenType() != TokenType::AngleBracketOpen) {
                return false;
            }

            Marker m = b.Mark();
            PsiBuilder2::SubStream subStream;
            NodeIndex<TypePathNode> first;

            if (!b.TryGetSubSubStream(&subStream, TokenType::AngleBracketOpen, TokenType::AngleBracketClose)) {
                // unmatched < bracket
                // type<<xyz>

                if (!mustBeValid) {
                    b.RollbackTo(m);
                    return false;
                }

                b.AdvanceLexer();
                Marker e = b.Mark();

                // consume until we hit a non type path character probably and return true
                // since we do actually want to keep parsing
                while (!b.EndOfInput()) {
                    b.AdvanceLexer();
                    if (!IsTypePathToken(b.GetToken())) {
                        b.Error(e, "Opening < isn't matched by >");
                        *nodeIndex = b.Done(m, TypeArgumentListNode(first));
                        return true;
                    }
                }
                b.Error(e, "Opening < isn't matched by >");
                // this is probably bad, means we hit end of file (or end of stream)
                *nodeIndex = b.Done(m, TypeArgumentListNode(first));
                return true;

            }

            b.PushStream(&subStream);

            if (!TryParseTypePath(&first, needsRecovery) || *needsRecovery) {

                if (!*needsRecovery) {
                    b.InlineError("Expected a type path inside < >");
                }

                b.PopStream();
                *nodeIndex = b.Done(m, TypeArgumentListNode(first));
                return true;
            }

            NodeIndex<TypePathNode> ptr = first;

            while (!b.EndOfInput()) {

                if (b.GetTokenType() == TokenType::Comma) {
                    b.AdvanceLexer();
                }
                else {

                    Marker e = b.Mark();
                    b.AdvanceToStreamEnd();
                    b.Error(e, "Expected a comma or > after type argument path");
                    break;
                }

                NodeIndex<TypePathNode> next;
                if (TryParseTypePath(&next, needsRecovery)) {
                    b.GetPsiNodeUnsafe(ptr)->next = next;
                    ptr = next;
                }
                else if (*needsRecovery) {
                    break;
                }
                else {
                    Marker e = b.Mark();
                    b.AdvanceToStreamEnd();
                    b.Error(e, "Expected a type path to follow comma");
                    break;
                }
            }

            b.PopStream();

            *nodeIndex = b.Done(m, TypeArgumentListNode(first));
            return true;

        }
    };

}
