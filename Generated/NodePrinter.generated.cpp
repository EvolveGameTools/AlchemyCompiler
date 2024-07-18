#include "../Src/Parsing3/NodePrinter.h"

namespace Alchemy::Compilation {

    void NodePrinter::PrintNode(SyntaxBase* syntaxBase) {

        if (syntaxBase == nullptr) {
            PrintLine("nullptr");
            return;
        }

        switch (syntaxBase->GetKind()) {
            case SyntaxKind::EmptyStatement: {
                EmptyStatementSyntax* p = (EmptyStatementSyntax*)syntaxBase;
                PrintNodeHeader("EmptyStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::BreakStatement: {
                BreakStatementSyntax* p = (BreakStatementSyntax*)syntaxBase;
                PrintNodeHeader("BreakStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("breakKeyword");
                PrintToken(p->breakKeyword);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::ContinueStatement: {
                ContinueStatementSyntax* p = (ContinueStatementSyntax*)syntaxBase;
                PrintNodeHeader("ContinueStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("continueKeyword");
                PrintToken(p->continueKeyword);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::ForStatement: {
                ForStatementSyntax* p = (ForStatementSyntax*)syntaxBase;
                PrintNodeHeader("ForStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("forKeyword");
                PrintToken(p->forKeyword);
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("declaration");
                PrintNode(p->declaration);
                PrintFieldName("initializers");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->initializers);
                PrintFieldName("firstSemiColon");
                PrintToken(p->firstSemiColon);
                PrintFieldName("condition");
                PrintNode(p->condition);
                PrintFieldName("secondSemiColon");
                PrintToken(p->secondSemiColon);
                PrintFieldName("incrementors");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->incrementors);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                PrintFieldName("statement");
                PrintNode(p->statement);
                indent--;
                break;
            }

            case SyntaxKind::ThrowStatement: {
                ThrowStatementSyntax* p = (ThrowStatementSyntax*)syntaxBase;
                PrintNodeHeader("ThrowStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("throwKeyword");
                PrintToken(p->throwKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::CatchDeclaration: {
                CatchDeclarationSyntax* p = (CatchDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("CatchDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                indent--;
                break;
            }

            case SyntaxKind::CatchFilterClause: {
                CatchFilterClauseSyntax* p = (CatchFilterClauseSyntax*)syntaxBase;
                PrintNodeHeader("CatchFilterClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("whenKeyword");
                PrintToken(p->whenKeyword);
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("filterExpression");
                PrintNode(p->filterExpression);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::CatchClause: {
                CatchClauseSyntax* p = (CatchClauseSyntax*)syntaxBase;
                PrintNodeHeader("CatchClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("catchKeyword");
                PrintToken(p->catchKeyword);
                PrintFieldName("declaration");
                PrintNode(p->declaration);
                PrintFieldName("filter");
                PrintNode(p->filter);
                PrintFieldName("block");
                PrintNode(p->block);
                indent--;
                break;
            }

            case SyntaxKind::FinallyClause: {
                FinallyClauseSyntax* p = (FinallyClauseSyntax*)syntaxBase;
                PrintNodeHeader("FinallyClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("finallyKeyword");
                PrintToken(p->finallyKeyword);
                PrintFieldName("block");
                PrintNode(p->block);
                indent--;
                break;
            }

            case SyntaxKind::TryStatement: {
                TryStatementSyntax* p = (TryStatementSyntax*)syntaxBase;
                PrintNodeHeader("TryStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("tryKeyword");
                PrintToken(p->tryKeyword);
                PrintFieldName("tryBlock");
                PrintNode(p->tryBlock);
                PrintFieldName("catchClauses");
                PrintSyntaxList((SyntaxListUntyped*)p->catchClauses);
                PrintFieldName("finallyClaus");
                PrintNode(p->finallyClaus);
                indent--;
                break;
            }

            case SyntaxKind::DefaultSwitchLabel: {
                DefaultSwitchLabelSyntax* p = (DefaultSwitchLabelSyntax*)syntaxBase;
                PrintNodeHeader("DefaultSwitchLabelSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("colon");
                PrintToken(p->colon);
                indent--;
                break;
            }

            case SyntaxKind::CaseSwitchLabel: {
                CaseSwitchLabelSyntax* p = (CaseSwitchLabelSyntax*)syntaxBase;
                PrintNodeHeader("CaseSwitchLabelSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("value");
                PrintNode(p->value);
                PrintFieldName("colon");
                PrintToken(p->colon);
                indent--;
                break;
            }

            case SyntaxKind::CasePatternSwitchLabel: {
                CasePatternSwitchLabelSyntax* p = (CasePatternSwitchLabelSyntax*)syntaxBase;
                PrintNodeHeader("CasePatternSwitchLabelSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                PrintFieldName("whenClause");
                PrintNode(p->whenClause);
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                indent--;
                break;
            }

            case SyntaxKind::SwitchSection: {
                SwitchSectionSyntax* p = (SwitchSectionSyntax*)syntaxBase;
                PrintNodeHeader("SwitchSectionSyntax", syntaxBase);
                indent++;
                PrintFieldName("labels");
                PrintSyntaxList((SyntaxListUntyped*)p->labels);
                PrintFieldName("statements");
                PrintSyntaxList((SyntaxListUntyped*)p->statements);
                indent--;
                break;
            }

            case SyntaxKind::SwitchStatement: {
                SwitchStatementSyntax* p = (SwitchStatementSyntax*)syntaxBase;
                PrintNodeHeader("SwitchStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("switchKeyword");
                PrintToken(p->switchKeyword);
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("sections");
                PrintSyntaxList((SyntaxListUntyped*)p->sections);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }

            case SyntaxKind::UsingStatement: {
                UsingStatementSyntax* p = (UsingStatementSyntax*)syntaxBase;
                PrintNodeHeader("UsingStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("usingKeyword");
                PrintToken(p->usingKeyword);
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("declaration");
                PrintNode(p->declaration);
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                PrintFieldName("statement");
                PrintNode(p->statement);
                indent--;
                break;
            }

            case SyntaxKind::WhileStatement: {
                WhileStatementSyntax* p = (WhileStatementSyntax*)syntaxBase;
                PrintNodeHeader("WhileStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("whileKeyword");
                PrintToken(p->whileKeyword);
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("condition");
                PrintNode(p->condition);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                PrintFieldName("statement");
                PrintNode(p->statement);
                indent--;
                break;
            }

            case SyntaxKind::DoStatement: {
                DoStatementSyntax* p = (DoStatementSyntax*)syntaxBase;
                PrintNodeHeader("DoStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("doKeyword");
                PrintToken(p->doKeyword);
                PrintFieldName("statement");
                PrintNode(p->statement);
                PrintFieldName("whileKeyword");
                PrintToken(p->whileKeyword);
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("condition");
                PrintNode(p->condition);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*)syntaxBase;
                PrintNodeHeader("ArrayRankSpecifierSyntax", syntaxBase);
                indent++;
                PrintFieldName("open");
                PrintToken(p->open);
                PrintFieldName("ranks");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->ranks);
                PrintFieldName("close");
                PrintToken(p->close);
                indent--;
                break;
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* p = (TypeArgumentListSyntax*)syntaxBase;
                PrintNodeHeader("TypeArgumentListSyntax", syntaxBase);
                indent++;
                PrintFieldName("lessThanToken");
                PrintToken(p->lessThanToken);
                PrintFieldName("arguments");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                PrintFieldName("greaterThanToken");
                PrintToken(p->greaterThanToken);
                indent--;
                break;
            }

            case SyntaxKind::GenericName: {
                GenericNameSyntax* p = (GenericNameSyntax*)syntaxBase;
                PrintNodeHeader("GenericNameSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeArgumentList");
                PrintNode(p->typeArgumentList);
                indent--;
                break;
            }

            case SyntaxKind::ElementBindingExpression: {
                ElementBindingExpressionSyntax* p = (ElementBindingExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ElementBindingExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                indent--;
                break;
            }

            case SyntaxKind::MemberBindingExpression: {
                MemberBindingExpressionSyntax* p = (MemberBindingExpressionSyntax*)syntaxBase;
                PrintNodeHeader("MemberBindingExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("name");
                PrintNode(p->name);
                indent--;
                break;
            }

            case SyntaxKind::ConditionalAccessExpression: {
                ConditionalAccessExpressionSyntax* p = (ConditionalAccessExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ConditionalAccessExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("whenNotNull");
                PrintNode(p->whenNotNull);
                indent--;
                break;
            }

            case SyntaxKind::SimpleMemberAccessExpression: {
                MemberAccessExpressionSyntax* p = (MemberAccessExpressionSyntax*)syntaxBase;
                PrintNodeHeader("MemberAccessExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("name");
                PrintNode(p->name);
                indent--;
                break;
            }
            case SyntaxKind::PointerMemberAccessExpression: {
                MemberAccessExpressionSyntax* p = (MemberAccessExpressionSyntax*)syntaxBase;
                PrintNodeHeader("MemberAccessExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("name");
                PrintNode(p->name);
                indent--;
                break;
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* p = (QualifiedNameSyntax*)syntaxBase;
                PrintNodeHeader("QualifiedNameSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("dotToken");
                PrintToken(p->dotToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }

            case SyntaxKind::IdentifierName: {
                IdentifierNameSyntax* p = (IdentifierNameSyntax*)syntaxBase;
                PrintNodeHeader("IdentifierNameSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::NameColon: {
                NameColonSyntax* p = (NameColonSyntax*)syntaxBase;
                PrintNodeHeader("NameColonSyntax", syntaxBase);
                indent++;
                PrintFieldName("name");
                PrintNode(p->name);
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                indent--;
                break;
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                PrintNodeHeader("TupleElementSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                PrintNodeHeader("PredefinedTypeSyntax", syntaxBase);
                indent++;
                PrintFieldName("typeToken");
                PrintToken(p->typeToken);
                indent--;
                break;
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                PrintNodeHeader("TupleTypeSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("elements");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->elements);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                PrintNodeHeader("RefTypeSyntax", syntaxBase);
                indent++;
                PrintFieldName("refKeyword");
                PrintToken(p->refKeyword);
                PrintFieldName("readonlyKeyword");
                PrintToken(p->readonlyKeyword);
                PrintFieldName("type");
                PrintNode(p->type);
                indent--;
                break;
            }

            case SyntaxKind::NullableType: {
                NullableTypeSyntax* p = (NullableTypeSyntax*)syntaxBase;
                PrintNodeHeader("NullableTypeSyntax", syntaxBase);
                indent++;
                PrintFieldName("elementType");
                PrintNode(p->elementType);
                PrintFieldName("questionMark");
                PrintToken(p->questionMark);
                indent--;
                break;
            }

            case SyntaxKind::LabeledStatement: {
                LabeledStatementSyntax* p = (LabeledStatementSyntax*)syntaxBase;
                PrintNodeHeader("LabeledStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("colon");
                PrintToken(p->colon);
                PrintFieldName("statement");
                PrintNode(p->statement);
                indent--;
                break;
            }

            case SyntaxKind::Argument: {
                ArgumentSyntax* p = (ArgumentSyntax*)syntaxBase;
                PrintNodeHeader("ArgumentSyntax", syntaxBase);
                indent++;
                PrintFieldName("nameColon");
                PrintNode(p->nameColon);
                PrintFieldName("refKindKeyword");
                PrintToken(p->refKindKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::NameEquals: {
                NameEqualsSyntax* p = (NameEqualsSyntax*)syntaxBase;
                PrintNodeHeader("NameEqualsSyntax", syntaxBase);
                indent++;
                PrintFieldName("name");
                PrintNode(p->name);
                PrintFieldName("equalsToken");
                PrintToken(p->equalsToken);
                indent--;
                break;
            }

            case SyntaxKind::ImplicitArrayCreationExpression: {
                ImplicitArrayCreationExpressionSyntax* p = (ImplicitArrayCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ImplicitArrayCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("newKeyword");
                PrintToken(p->newKeyword);
                PrintFieldName("openBracket");
                PrintToken(p->openBracket);
                PrintFieldName("commas");
                PrintTokenList(p->commas);
                PrintFieldName("closeBracket");
                PrintToken(p->closeBracket);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::ObjectInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }
            case SyntaxKind::CollectionInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }
            case SyntaxKind::ArrayInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }
            case SyntaxKind::ComplexElementInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }
            case SyntaxKind::WithInitializerExpression: {
                InitializerExpressionSyntax* p = (InitializerExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InitializerExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("list");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->list);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }

            case SyntaxKind::StackAllocArrayCreationExpression: {
                StackAllocArrayCreationExpressionSyntax* p = (StackAllocArrayCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("StackAllocArrayCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("stackallocKeyword");
                PrintToken(p->stackallocKeyword);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::ImplicitStackAllocArrayCreationExpression: {
                ImplicitStackAllocArrayCreationExpressionSyntax* p = (ImplicitStackAllocArrayCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ImplicitStackAllocArrayCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("stackallocKeyword");
                PrintToken(p->stackallocKeyword);
                PrintFieldName("openBracket");
                PrintToken(p->openBracket);
                PrintFieldName("closeBracket");
                PrintToken(p->closeBracket);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::ArgumentList: {
                ArgumentListSyntax* p = (ArgumentListSyntax*)syntaxBase;
                PrintNodeHeader("ArgumentListSyntax", syntaxBase);
                indent++;
                PrintFieldName("openToken");
                PrintToken(p->openToken);
                PrintFieldName("arguments");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                PrintFieldName("closeToken");
                PrintToken(p->closeToken);
                indent--;
                break;
            }

            case SyntaxKind::ObjectCreationExpression: {
                ObjectCreationExpressionSyntax* p = (ObjectCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ObjectCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("newKeyword");
                PrintToken(p->newKeyword);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("arguments");
                PrintNode(p->arguments);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::ImplicitObjectCreationExpression: {
                ImplicitObjectCreationExpressionSyntax* p = (ImplicitObjectCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ImplicitObjectCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("newKeyword");
                PrintToken(p->newKeyword);
                PrintFieldName("arguments");
                PrintNode(p->arguments);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::AnonymousObjectMemberDeclarator: {
                AnonymousObjectMemberDeclaratorSyntax* p = (AnonymousObjectMemberDeclaratorSyntax*)syntaxBase;
                PrintNodeHeader("AnonymousObjectMemberDeclaratorSyntax", syntaxBase);
                indent++;
                PrintFieldName("nameEquals");
                PrintNode(p->nameEquals);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::AnonymousObjectCreationExpression: {
                AnonymousObjectCreationExpressionSyntax* p = (AnonymousObjectCreationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AnonymousObjectCreationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("newToken");
                PrintToken(p->newToken);
                PrintFieldName("openBrace");
                PrintToken(p->openBrace);
                PrintFieldName("initializers");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->initializers);
                PrintFieldName("closeBrace");
                PrintToken(p->closeBrace);
                indent--;
                break;
            }

            case SyntaxKind::TupleExpression: {
                TupleExpressionSyntax* p = (TupleExpressionSyntax*)syntaxBase;
                PrintNodeHeader("TupleExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openToken");
                PrintToken(p->openToken);
                PrintFieldName("arguments");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                PrintFieldName("closeToken");
                PrintToken(p->closeToken);
                indent--;
                break;
            }

            case SyntaxKind::ParenthesizedExpression: {
                ParenthesizedExpressionSyntax* p = (ParenthesizedExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ParenthesizedExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openToken");
                PrintToken(p->openToken);
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("closeToken");
                PrintToken(p->closeToken);
                indent--;
                break;
            }

            case SyntaxKind::BracketedArgumentList: {
                BracketedArgumentListSyntax* p = (BracketedArgumentListSyntax*)syntaxBase;
                PrintNodeHeader("BracketedArgumentListSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBracket");
                PrintToken(p->openBracket);
                PrintFieldName("arguments");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arguments);
                PrintFieldName("closeBracket");
                PrintToken(p->closeBracket);
                indent--;
                break;
            }

            case SyntaxKind::EqualsValueClause: {
                EqualsValueClauseSyntax* p = (EqualsValueClauseSyntax*)syntaxBase;
                PrintNodeHeader("EqualsValueClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("equalsToken");
                PrintToken(p->equalsToken);
                PrintFieldName("value");
                PrintNode(p->value);
                indent--;
                break;
            }

            case SyntaxKind::RefExpression: {
                RefExpressionSyntax* p = (RefExpressionSyntax*)syntaxBase;
                PrintNodeHeader("RefExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("refKeyword");
                PrintToken(p->refKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::VariableDeclarator: {
                VariableDeclaratorSyntax* p = (VariableDeclaratorSyntax*)syntaxBase;
                PrintNodeHeader("VariableDeclaratorSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                indent--;
                break;
            }

            case SyntaxKind::TypeParameter: {
                TypeParameterSyntax* p = (TypeParameterSyntax*)syntaxBase;
                PrintNodeHeader("TypeParameterSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::TypeParameterList: {
                TypeParameterListSyntax* p = (TypeParameterListSyntax*)syntaxBase;
                PrintNodeHeader("TypeParameterListSyntax", syntaxBase);
                indent++;
                PrintFieldName("lessThanToken");
                PrintToken(p->lessThanToken);
                PrintFieldName("parameters");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->parameters);
                PrintFieldName("greaterThanToken");
                PrintToken(p->greaterThanToken);
                indent--;
                break;
            }

            case SyntaxKind::ArrowExpressionClause: {
                ArrowExpressionClauseSyntax* p = (ArrowExpressionClauseSyntax*)syntaxBase;
                PrintNodeHeader("ArrowExpressionClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("arrowToken");
                PrintToken(p->arrowToken);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::Block: {
                BlockSyntax* p = (BlockSyntax*)syntaxBase;
                PrintNodeHeader("BlockSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("statements");
                PrintSyntaxList((SyntaxListUntyped*)p->statements);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }

            case SyntaxKind::DefaultLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::FalseLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::NullLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::NumericLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::EmptyStringLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }
            case SyntaxKind::TrueLiteralExpression: {
                LiteralExpressionSyntax* p = (LiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("LiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("literal");
                PrintToken(p->literal);
                indent--;
                break;
            }

            case SyntaxKind::CastExpression: {
                CastExpressionSyntax* p = (CastExpressionSyntax*)syntaxBase;
                PrintNodeHeader("CastExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::BaseExpression: {
                BaseExpressionSyntax* p = (BaseExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BaseExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                indent--;
                break;
            }

            case SyntaxKind::ThisExpression: {
                ThisExpressionSyntax* p = (ThisExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ThisExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                indent--;
                break;
            }

            case SyntaxKind::DefaultExpression: {
                DefaultExpressionSyntax* p = (DefaultExpressionSyntax*)syntaxBase;
                PrintNodeHeader("DefaultExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::TypeOfExpression: {
                TypeOfExpressionSyntax* p = (TypeOfExpressionSyntax*)syntaxBase;
                PrintNodeHeader("TypeOfExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::DiscardDesignation: {
                DiscardDesignationSyntax* p = (DiscardDesignationSyntax*)syntaxBase;
                PrintNodeHeader("DiscardDesignationSyntax", syntaxBase);
                indent++;
                PrintFieldName("underscoreToken");
                PrintToken(p->underscoreToken);
                indent--;
                break;
            }

            case SyntaxKind::SingleVariableDesignation: {
                SingleVariableDesignationSyntax* p = (SingleVariableDesignationSyntax*)syntaxBase;
                PrintNodeHeader("SingleVariableDesignationSyntax", syntaxBase);
                indent++;
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                indent--;
                break;
            }

            case SyntaxKind::ParenthesizedVariableDesignation: {
                ParenthesizedVariableDesignationSyntax* p = (ParenthesizedVariableDesignationSyntax*)syntaxBase;
                PrintNodeHeader("ParenthesizedVariableDesignationSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("designators");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->designators);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                indent--;
                break;
            }

            case SyntaxKind::ExpressionElement: {
                ExpressionElementSyntax* p = (ExpressionElementSyntax*)syntaxBase;
                PrintNodeHeader("ExpressionElementSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::SpreadElement: {
                SpreadElementSyntax* p = (SpreadElementSyntax*)syntaxBase;
                PrintNodeHeader("SpreadElementSyntax", syntaxBase);
                indent++;
                PrintFieldName("dotDotToken");
                PrintToken(p->dotDotToken);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::CollectionExpression: {
                CollectionExpressionSyntax* p = (CollectionExpressionSyntax*)syntaxBase;
                PrintNodeHeader("CollectionExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("open");
                PrintToken(p->open);
                PrintFieldName("elements");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->elements);
                PrintFieldName("close");
                PrintToken(p->close);
                indent--;
                break;
            }

            case SyntaxKind::DeclarationExpression: {
                DeclarationExpressionSyntax* p = (DeclarationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("DeclarationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("designation");
                PrintNode(p->designation);
                indent--;
                break;
            }

            case SyntaxKind::ThrowExpression: {
                ThrowExpressionSyntax* p = (ThrowExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ThrowExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("throwKeyword");
                PrintToken(p->throwKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::PostIncrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PostfixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                indent--;
                break;
            }
            case SyntaxKind::PostDecrementExpression: {
                PostfixUnaryExpressionSyntax* p = (PostfixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PostfixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                indent--;
                break;
            }

            case SyntaxKind::ElementAccessExpression: {
                ElementAccessExpressionSyntax* p = (ElementAccessExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ElementAccessExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                indent--;
                break;
            }

            case SyntaxKind::InvocationExpression: {
                InvocationExpressionSyntax* p = (InvocationExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InvocationExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                indent--;
                break;
            }

            case SyntaxKind::ConditionalExpression: {
                ConditionalExpressionSyntax* p = (ConditionalExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ConditionalExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("condition");
                PrintNode(p->condition);
                PrintFieldName("questionToken");
                PrintToken(p->questionToken);
                PrintFieldName("whenTrue");
                PrintNode(p->whenTrue);
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("whenFalse");
                PrintNode(p->whenFalse);
                indent--;
                break;
            }

            case SyntaxKind::RangeExpression: {
                RangeExpressionSyntax* p = (RangeExpressionSyntax*)syntaxBase;
                PrintNodeHeader("RangeExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("leftOperand");
                PrintNode(p->leftOperand);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("rightOperand");
                PrintNode(p->rightOperand);
                indent--;
                break;
            }

            case SyntaxKind::UnaryPlusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::UnaryMinusExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::BitwiseNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::LogicalNotExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::PreIncrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::PreDecrementExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }
            case SyntaxKind::IndexExpression: {
                PrefixUnaryExpressionSyntax* p = (PrefixUnaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("PrefixUnaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("operand");
                PrintNode(p->operand);
                indent--;
                break;
            }

            case SyntaxKind::Parameter: {
                ParameterSyntax* p = (ParameterSyntax*)syntaxBase;
                PrintNodeHeader("ParameterSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("defaultValue");
                PrintNode(p->defaultValue);
                indent--;
                break;
            }

            case SyntaxKind::SimpleLambdaExpression: {
                SimpleLambdaExpressionSyntax* p = (SimpleLambdaExpressionSyntax*)syntaxBase;
                PrintNodeHeader("SimpleLambdaExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("parameter");
                PrintNode(p->parameter);
                PrintFieldName("arrowToken");
                PrintToken(p->arrowToken);
                PrintFieldName("blockBody");
                PrintNode(p->blockBody);
                PrintFieldName("expressionBody");
                PrintNode(p->expressionBody);
                indent--;
                break;
            }

            case SyntaxKind::ParenthesizedLambdaExpression: {
                ParenthesizedLambdaExpressionSyntax* p = (ParenthesizedLambdaExpressionSyntax*)syntaxBase;
                PrintNodeHeader("ParenthesizedLambdaExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("returnType");
                PrintNode(p->returnType);
                PrintFieldName("parameters");
                PrintNode(p->parameters);
                PrintFieldName("arrowToken");
                PrintToken(p->arrowToken);
                PrintFieldName("blockBody");
                PrintNode(p->blockBody);
                PrintFieldName("expressionBody");
                PrintNode(p->expressionBody);
                indent--;
                break;
            }

            case SyntaxKind::BaseConstructorInitializer: {
                BaseConstructorInitializerSyntax* p = (BaseConstructorInitializerSyntax*)syntaxBase;
                PrintNodeHeader("BaseConstructorInitializerSyntax", syntaxBase);
                indent++;
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("baseKeyword");
                PrintToken(p->baseKeyword);
                PrintFieldName("argumentListSyntax");
                PrintNode(p->argumentListSyntax);
                indent--;
                break;
            }

            case SyntaxKind::ThisConstructorInitializer: {
                ThisConstructorInitializerSyntax* p = (ThisConstructorInitializerSyntax*)syntaxBase;
                PrintNodeHeader("ThisConstructorInitializerSyntax", syntaxBase);
                indent++;
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("thisKeyword");
                PrintToken(p->thisKeyword);
                PrintFieldName("argumentListSyntax");
                PrintNode(p->argumentListSyntax);
                indent--;
                break;
            }

            case SyntaxKind::NamedConstructorInitializer: {
                NamedConstructorInitializerSyntax* p = (NamedConstructorInitializerSyntax*)syntaxBase;
                PrintNodeHeader("NamedConstructorInitializerSyntax", syntaxBase);
                indent++;
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("name");
                PrintToken(p->name);
                PrintFieldName("argumentListSyntax");
                PrintNode(p->argumentListSyntax);
                indent--;
                break;
            }

            case SyntaxKind::ParameterList: {
                ParameterListSyntax* p = (ParameterListSyntax*)syntaxBase;
                PrintNodeHeader("ParameterListSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("parameters");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->parameters);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                indent--;
                break;
            }

            case SyntaxKind::BracketedParameterList: {
                BracketedParameterListSyntax* p = (BracketedParameterListSyntax*)syntaxBase;
                PrintNodeHeader("BracketedParameterListSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBracket");
                PrintToken(p->openBracket);
                PrintFieldName("parameters");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->parameters);
                PrintFieldName("closeBracket");
                PrintToken(p->closeBracket);
                indent--;
                break;
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                PrintNodeHeader("LocalFunctionStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("returnType");
                PrintNode(p->returnType);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeParameters");
                PrintNode(p->typeParameters);
                PrintFieldName("parameters");
                PrintNode(p->parameters);
                PrintFieldName("constraints");
                PrintSyntaxList((SyntaxListUntyped*)p->constraints);
                PrintFieldName("blockBody");
                PrintNode(p->blockBody);
                PrintFieldName("arrowBody");
                PrintNode(p->arrowBody);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* p = (VariableDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("VariableDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("variables");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->variables);
                indent--;
                break;
            }

            case SyntaxKind::LocalDeclarationStatement: {
                LocalDeclarationStatementSyntax* p = (LocalDeclarationStatementSyntax*)syntaxBase;
                PrintNodeHeader("LocalDeclarationStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("usingKeyword");
                PrintToken(p->usingKeyword);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("declaration");
                PrintNode(p->declaration);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* p = (FieldDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("FieldDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("declaration");
                PrintNode(p->declaration);
                PrintFieldName("semicolonToken");
                PrintToken(p->semicolonToken);
                indent--;
                break;
            }

            case SyntaxKind::ExpressionColon: {
                ExpressionColonSyntax* p = (ExpressionColonSyntax*)syntaxBase;
                PrintNodeHeader("ExpressionColonSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                indent--;
                break;
            }

            case SyntaxKind::Subpattern: {
                SubpatternSyntax* p = (SubpatternSyntax*)syntaxBase;
                PrintNodeHeader("SubpatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("expressionColon");
                PrintNode(p->expressionColon);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                indent--;
                break;
            }

            case SyntaxKind::PropertyPatternClause: {
                PropertyPatternClauseSyntax* p = (PropertyPatternClauseSyntax*)syntaxBase;
                PrintNodeHeader("PropertyPatternClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("subpatterns");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->subpatterns);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }

            case SyntaxKind::DeclarationPattern: {
                DeclarationPatternSyntax* p = (DeclarationPatternSyntax*)syntaxBase;
                PrintNodeHeader("DeclarationPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("designation");
                PrintNode(p->designation);
                indent--;
                break;
            }

            case SyntaxKind::PositionalPatternClause: {
                PositionalPatternClauseSyntax* p = (PositionalPatternClauseSyntax*)syntaxBase;
                PrintNodeHeader("PositionalPatternClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("subpatterns");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->subpatterns);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::RecursivePattern: {
                RecursivePatternSyntax* p = (RecursivePatternSyntax*)syntaxBase;
                PrintNodeHeader("RecursivePatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("positionalPatternClause");
                PrintNode(p->positionalPatternClause);
                PrintFieldName("propertyPatternClause");
                PrintNode(p->propertyPatternClause);
                PrintFieldName("designation");
                PrintNode(p->designation);
                indent--;
                break;
            }

            case SyntaxKind::ParenthesizedPattern: {
                ParenthesizedPatternSyntax* p = (ParenthesizedPatternSyntax*)syntaxBase;
                PrintNodeHeader("ParenthesizedPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("openParenToken");
                PrintToken(p->openParenToken);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                PrintFieldName("closeParenToken");
                PrintToken(p->closeParenToken);
                indent--;
                break;
            }

            case SyntaxKind::VarPattern: {
                VarPatternSyntax* p = (VarPatternSyntax*)syntaxBase;
                PrintNodeHeader("VarPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("varKeyword");
                PrintToken(p->varKeyword);
                PrintFieldName("designation");
                PrintNode(p->designation);
                indent--;
                break;
            }

            case SyntaxKind::TypePattern: {
                TypePatternSyntax* p = (TypePatternSyntax*)syntaxBase;
                PrintNodeHeader("TypePatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                indent--;
                break;
            }

            case SyntaxKind::ConstantPattern: {
                ConstantPatternSyntax* p = (ConstantPatternSyntax*)syntaxBase;
                PrintNodeHeader("ConstantPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::RelationalPattern: {
                RelationalPatternSyntax* p = (RelationalPatternSyntax*)syntaxBase;
                PrintNodeHeader("RelationalPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::SlicePattern: {
                SlicePatternSyntax* p = (SlicePatternSyntax*)syntaxBase;
                PrintNodeHeader("SlicePatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("dotDotToken");
                PrintToken(p->dotDotToken);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                indent--;
                break;
            }

            case SyntaxKind::DiscardPattern: {
                DiscardPatternSyntax* p = (DiscardPatternSyntax*)syntaxBase;
                PrintNodeHeader("DiscardPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("underscore");
                PrintToken(p->underscore);
                indent--;
                break;
            }

            case SyntaxKind::NotPattern: {
                UnaryPatternSyntax* p = (UnaryPatternSyntax*)syntaxBase;
                PrintNodeHeader("UnaryPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                indent--;
                break;
            }

            case SyntaxKind::OrPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                PrintNodeHeader("BinaryPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::AndPattern: {
                BinaryPatternSyntax* p = (BinaryPatternSyntax*)syntaxBase;
                PrintNodeHeader("BinaryPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }

            case SyntaxKind::IsPatternExpression: {
                IsPatternExpressionSyntax* p = (IsPatternExpressionSyntax*)syntaxBase;
                PrintNodeHeader("IsPatternExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("leftOperand");
                PrintNode(p->leftOperand);
                PrintFieldName("opToken");
                PrintToken(p->opToken);
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                indent--;
                break;
            }

            case SyntaxKind::AddExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::SubtractExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::MultiplyExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::DivideExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::ModuloExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LeftShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::RightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::UnsignedRightShiftExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LogicalOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LogicalAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::BitwiseOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::BitwiseAndExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::ExclusiveOrExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::EqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::NotEqualsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LessThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LessThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::GreaterThanExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::GreaterThanOrEqualExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::IsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::AsExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::CoalesceExpression: {
                BinaryExpressionSyntax* p = (BinaryExpressionSyntax*)syntaxBase;
                PrintNodeHeader("BinaryExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }

            case SyntaxKind::ImplicitElementAccess: {
                ImplicitElementAccessSyntax* p = (ImplicitElementAccessSyntax*)syntaxBase;
                PrintNodeHeader("ImplicitElementAccessSyntax", syntaxBase);
                indent++;
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                indent--;
                break;
            }

            case SyntaxKind::WhenClause: {
                WhenClauseSyntax* p = (WhenClauseSyntax*)syntaxBase;
                PrintNodeHeader("WhenClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("whenKeyword");
                PrintToken(p->whenKeyword);
                PrintFieldName("condition");
                PrintNode(p->condition);
                indent--;
                break;
            }

            case SyntaxKind::SwitchExpressionArm: {
                SwitchExpressionArmSyntax* p = (SwitchExpressionArmSyntax*)syntaxBase;
                PrintNodeHeader("SwitchExpressionArmSyntax", syntaxBase);
                indent++;
                PrintFieldName("pattern");
                PrintNode(p->pattern);
                PrintFieldName("whenClause");
                PrintNode(p->whenClause);
                PrintFieldName("equalsGreaterThanToken");
                PrintToken(p->equalsGreaterThanToken);
                PrintFieldName("expression");
                PrintNode(p->expression);
                indent--;
                break;
            }

            case SyntaxKind::SwitchExpression: {
                SwitchExpressionSyntax* p = (SwitchExpressionSyntax*)syntaxBase;
                PrintNodeHeader("SwitchExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("governingExpression");
                PrintNode(p->governingExpression);
                PrintFieldName("switchKeyword");
                PrintToken(p->switchKeyword);
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("arms");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->arms);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }

            case SyntaxKind::ListPattern: {
                ListPatternSyntax* p = (ListPatternSyntax*)syntaxBase;
                PrintNodeHeader("ListPatternSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBracketToken");
                PrintToken(p->openBracketToken);
                PrintFieldName("patterns");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->patterns);
                PrintFieldName("closeBracketToken");
                PrintToken(p->closeBracketToken);
                PrintFieldName("designation");
                PrintNode(p->designation);
                indent--;
                break;
            }

            case SyntaxKind::SimpleAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::AddAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::SubtractAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::MultiplyAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::DivideAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::ModuloAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::AndAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::ExclusiveOrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::OrAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::LeftShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::RightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::UnsignedRightShiftAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }
            case SyntaxKind::CoalesceAssignmentExpression: {
                AssignmentExpressionSyntax* p = (AssignmentExpressionSyntax*)syntaxBase;
                PrintNodeHeader("AssignmentExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("left");
                PrintNode(p->left);
                PrintFieldName("operatorToken");
                PrintToken(p->operatorToken);
                PrintFieldName("right");
                PrintNode(p->right);
                indent--;
                break;
            }

            case SyntaxKind::ForEachStatement: {
                ForEachStatementSyntax* p = (ForEachStatementSyntax*)syntaxBase;
                PrintNodeHeader("ForEachStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("foreachKeyword");
                PrintToken(p->foreachKeyword);
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("inKeyword");
                PrintToken(p->inKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                PrintFieldName("statement");
                PrintNode(p->statement);
                indent--;
                break;
            }

            case SyntaxKind::ForEachVariableStatement: {
                ForEachVariableStatementSyntax* p = (ForEachVariableStatementSyntax*)syntaxBase;
                PrintNodeHeader("ForEachVariableStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("foreachKeyword");
                PrintToken(p->foreachKeyword);
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("variable");
                PrintNode(p->variable);
                PrintFieldName("inKeyword");
                PrintToken(p->inKeyword);
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                PrintFieldName("statement");
                PrintNode(p->statement);
                indent--;
                break;
            }

            case SyntaxKind::GotoCaseStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                PrintNodeHeader("GotoStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("gotoToken");
                PrintToken(p->gotoToken);
                PrintFieldName("caseOrDefault");
                PrintToken(p->caseOrDefault);
                PrintFieldName("arg");
                PrintNode(p->arg);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }
            case SyntaxKind::GotoDefaultStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                PrintNodeHeader("GotoStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("gotoToken");
                PrintToken(p->gotoToken);
                PrintFieldName("caseOrDefault");
                PrintToken(p->caseOrDefault);
                PrintFieldName("arg");
                PrintNode(p->arg);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }
            case SyntaxKind::GotoStatement: {
                GotoStatementSyntax* p = (GotoStatementSyntax*)syntaxBase;
                PrintNodeHeader("GotoStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("gotoToken");
                PrintToken(p->gotoToken);
                PrintFieldName("caseOrDefault");
                PrintToken(p->caseOrDefault);
                PrintFieldName("arg");
                PrintNode(p->arg);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::ElseClause: {
                ElseClauseSyntax* p = (ElseClauseSyntax*)syntaxBase;
                PrintNodeHeader("ElseClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("elseKeyword");
                PrintToken(p->elseKeyword);
                PrintFieldName("statement");
                PrintNode(p->statement);
                indent--;
                break;
            }

            case SyntaxKind::IfStatement: {
                IfStatementSyntax* p = (IfStatementSyntax*)syntaxBase;
                PrintNodeHeader("IfStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("ifKeyword");
                PrintToken(p->ifKeyword);
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("condition");
                PrintNode(p->condition);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                PrintFieldName("statement");
                PrintNode(p->statement);
                PrintFieldName("elseClause");
                PrintNode(p->elseClause);
                indent--;
                break;
            }

            case SyntaxKind::ExpressionStatement: {
                ExpressionStatementSyntax* p = (ExpressionStatementSyntax*)syntaxBase;
                PrintNodeHeader("ExpressionStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::ReturnStatement: {
                ReturnStatementSyntax* p = (ReturnStatementSyntax*)syntaxBase;
                PrintNodeHeader("ReturnStatementSyntax", syntaxBase);
                indent++;
                PrintFieldName("returnKeyword");
                PrintToken(p->returnKeyword);
                PrintFieldName("expressionSyntax");
                PrintNode(p->expressionSyntax);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::BaseList: {
                BaseListSyntax* p = (BaseListSyntax*)syntaxBase;
                PrintNodeHeader("BaseListSyntax", syntaxBase);
                indent++;
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("types");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->types);
                indent--;
                break;
            }

            case SyntaxKind::Attribute: {
                AttributeSyntax* p = (AttributeSyntax*)syntaxBase;
                PrintNodeHeader("AttributeSyntax", syntaxBase);
                indent++;
                PrintFieldName("name");
                PrintNode(p->name);
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                indent--;
                break;
            }

            case SyntaxKind::AttributeList: {
                AttributeListSyntax* p = (AttributeListSyntax*)syntaxBase;
                PrintNodeHeader("AttributeListSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBracket");
                PrintToken(p->openBracket);
                PrintFieldName("attributes");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->attributes);
                PrintFieldName("closeBracket");
                PrintToken(p->closeBracket);
                indent--;
                break;
            }

            case SyntaxKind::TypeConstraint: {
                TypeConstraintSyntax* p = (TypeConstraintSyntax*)syntaxBase;
                PrintNodeHeader("TypeConstraintSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                indent--;
                break;
            }

            case SyntaxKind::ConstructorConstraint: {
                ConstructorConstraintSyntax* p = (ConstructorConstraintSyntax*)syntaxBase;
                PrintNodeHeader("ConstructorConstraintSyntax", syntaxBase);
                indent++;
                PrintFieldName("newKeyword");
                PrintToken(p->newKeyword);
                PrintFieldName("openParen");
                PrintToken(p->openParen);
                PrintFieldName("closeParen");
                PrintToken(p->closeParen);
                indent--;
                break;
            }

            case SyntaxKind::ClassConstraint: {
                ClassOrStructConstraintSyntax* p = (ClassOrStructConstraintSyntax*)syntaxBase;
                PrintNodeHeader("ClassOrStructConstraintSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("questionToken");
                PrintToken(p->questionToken);
                indent--;
                break;
            }
            case SyntaxKind::StructConstraint: {
                ClassOrStructConstraintSyntax* p = (ClassOrStructConstraintSyntax*)syntaxBase;
                PrintNodeHeader("ClassOrStructConstraintSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("questionToken");
                PrintToken(p->questionToken);
                indent--;
                break;
            }

            case SyntaxKind::TypeParameterConstraintClause: {
                TypeParameterConstraintClauseSyntax* p = (TypeParameterConstraintClauseSyntax*)syntaxBase;
                PrintNodeHeader("TypeParameterConstraintClauseSyntax", syntaxBase);
                indent++;
                PrintFieldName("whereKeyword");
                PrintToken(p->whereKeyword);
                PrintFieldName("name");
                PrintNode(p->name);
                PrintFieldName("colonToken");
                PrintToken(p->colonToken);
                PrintFieldName("constraints");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->constraints);
                indent--;
                break;
            }

            case SyntaxKind::StructDeclaration: {
                StructDeclarationSyntax* p = (StructDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("StructDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeParameterList");
                PrintNode(p->typeParameterList);
                PrintFieldName("parameterList");
                PrintNode(p->parameterList);
                PrintFieldName("baseList");
                PrintNode(p->baseList);
                PrintFieldName("constraintClauses");
                PrintSyntaxList((SyntaxListUntyped*)p->constraintClauses);
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("members");
                PrintSyntaxList((SyntaxListUntyped*)p->members);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                PrintFieldName("semicolonToken");
                PrintToken(p->semicolonToken);
                indent--;
                break;
            }

            case SyntaxKind::EnumMemberDeclaration: {
                EnumMemberDeclarationSyntax* p = (EnumMemberDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("EnumMemberDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("equalsValue");
                PrintNode(p->equalsValue);
                indent--;
                break;
            }

            case SyntaxKind::EnumDeclaration: {
                EnumDeclarationSyntax* p = (EnumDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("EnumDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("baseList");
                PrintNode(p->baseList);
                PrintFieldName("openBrace");
                PrintToken(p->openBrace);
                PrintFieldName("members");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->members);
                PrintFieldName("closeBrace");
                PrintToken(p->closeBrace);
                PrintFieldName("semicolonToken");
                PrintToken(p->semicolonToken);
                indent--;
                break;
            }

            case SyntaxKind::DelegateDeclaration: {
                DelegateDeclarationSyntax* p = (DelegateDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("DelegateDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("returnType");
                PrintNode(p->returnType);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeParameterList");
                PrintNode(p->typeParameterList);
                PrintFieldName("parameterList");
                PrintNode(p->parameterList);
                PrintFieldName("constraintClauses");
                PrintSyntaxList((SyntaxListUntyped*)p->constraintClauses);
                PrintFieldName("semicolonToken");
                PrintToken(p->semicolonToken);
                indent--;
                break;
            }

            case SyntaxKind::ClassDeclaration: {
                ClassDeclarationSyntax* p = (ClassDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("ClassDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeParameterList");
                PrintNode(p->typeParameterList);
                PrintFieldName("parameterList");
                PrintNode(p->parameterList);
                PrintFieldName("baseList");
                PrintNode(p->baseList);
                PrintFieldName("constraintClauses");
                PrintSyntaxList((SyntaxListUntyped*)p->constraintClauses);
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("members");
                PrintSyntaxList((SyntaxListUntyped*)p->members);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                PrintFieldName("semicolonToken");
                PrintToken(p->semicolonToken);
                indent--;
                break;
            }

            case SyntaxKind::NamespaceDeclaration: {
                NamespaceDeclarationSyntax* p = (NamespaceDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("NamespaceDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("names");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->names);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::InterfaceDeclaration: {
                InterfaceDeclarationSyntax* p = (InterfaceDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("InterfaceDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeParameterList");
                PrintNode(p->typeParameterList);
                PrintFieldName("parameterList");
                PrintNode(p->parameterList);
                PrintFieldName("baseList");
                PrintNode(p->baseList);
                PrintFieldName("constraintClauses");
                PrintSyntaxList((SyntaxListUntyped*)p->constraintClauses);
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("members");
                PrintSyntaxList((SyntaxListUntyped*)p->members);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                PrintFieldName("semicolonToken");
                PrintToken(p->semicolonToken);
                indent--;
                break;
            }

            case SyntaxKind::ConstructorDeclaration: {
                ConstructorDeclarationSyntax* p = (ConstructorDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("ConstructorDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("parameterList");
                PrintNode(p->parameterList);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                PrintFieldName("bodyBlock");
                PrintNode(p->bodyBlock);
                PrintFieldName("bodyExpression");
                PrintNode(p->bodyExpression);
                PrintFieldName("semiColon");
                PrintToken(p->semiColon);
                indent--;
                break;
            }

            case SyntaxKind::BaseType: {
                BaseTypeSyntax* p = (BaseTypeSyntax*)syntaxBase;
                PrintNodeHeader("BaseTypeSyntax", syntaxBase);
                indent++;
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("argumentList");
                PrintNode(p->argumentList);
                indent--;
                break;
            }

            case SyntaxKind::StringLiteralExpression: {
                StringLiteralExpression* p = (StringLiteralExpression*)syntaxBase;
                PrintNodeHeader("StringLiteralExpression", syntaxBase);
                indent++;
                PrintFieldName("start");
                PrintToken(p->start);
                PrintFieldName("parts");
                PrintSyntaxList((SyntaxListUntyped*)p->parts);
                PrintFieldName("end");
                PrintToken(p->end);
                indent--;
                break;
            }

            case SyntaxKind::RawStringLiteralExpression: {
                RawStringLiteralExpression* p = (RawStringLiteralExpression*)syntaxBase;
                PrintNodeHeader("RawStringLiteralExpression", syntaxBase);
                indent++;
                PrintFieldName("start");
                PrintToken(p->start);
                PrintFieldName("parts");
                PrintSyntaxList((SyntaxListUntyped*)p->parts);
                PrintFieldName("end");
                PrintToken(p->end);
                indent--;
                break;
            }

            case SyntaxKind::InterpolatedIdentifierPart: {
                InterpolatedIdentifierPartSyntax* p = (InterpolatedIdentifierPartSyntax*)syntaxBase;
                PrintNodeHeader("InterpolatedIdentifierPartSyntax", syntaxBase);
                indent++;
                PrintFieldName("interpolatedIdentifier");
                PrintToken(p->interpolatedIdentifier);
                indent--;
                break;
            }

            case SyntaxKind::InterpolatedStringExpression: {
                InterpolatedStringExpressionSyntax* p = (InterpolatedStringExpressionSyntax*)syntaxBase;
                PrintNodeHeader("InterpolatedStringExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("start");
                PrintToken(p->start);
                PrintFieldName("expression");
                PrintNode(p->expression);
                PrintFieldName("end");
                PrintToken(p->end);
                indent--;
                break;
            }

            case SyntaxKind::StringLiteralPart: {
                StringLiteralPartSyntax* p = (StringLiteralPartSyntax*)syntaxBase;
                PrintNodeHeader("StringLiteralPartSyntax", syntaxBase);
                indent++;
                PrintFieldName("part");
                PrintToken(p->part);
                indent--;
                break;
            }

            case SyntaxKind::CharacterLiteralExpression: {
                CharacterLiteralExpressionSyntax* p = (CharacterLiteralExpressionSyntax*)syntaxBase;
                PrintNodeHeader("CharacterLiteralExpressionSyntax", syntaxBase);
                indent++;
                PrintFieldName("start");
                PrintToken(p->start);
                PrintFieldName("contents");
                PrintToken(p->contents);
                PrintFieldName("end");
                PrintToken(p->end);
                indent--;
                break;
            }

            case SyntaxKind::IncompleteMember: {
                IncompleteMemberSyntax* p = (IncompleteMemberSyntax*)syntaxBase;
                PrintNodeHeader("IncompleteMemberSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("type");
                PrintNode(p->type);
                indent--;
                break;
            }

            case SyntaxKind::GetAccessorDeclaration: {
                AccessorDeclarationSyntax* p = (AccessorDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("AccessorDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("bodyBlock");
                PrintNode(p->bodyBlock);
                PrintFieldName("expressionBody");
                PrintNode(p->expressionBody);
                PrintFieldName("semiColon");
                PrintToken(p->semiColon);
                indent--;
                break;
            }
            case SyntaxKind::SetAccessorDeclaration: {
                AccessorDeclarationSyntax* p = (AccessorDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("AccessorDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("bodyBlock");
                PrintNode(p->bodyBlock);
                PrintFieldName("expressionBody");
                PrintNode(p->expressionBody);
                PrintFieldName("semiColon");
                PrintToken(p->semiColon);
                indent--;
                break;
            }
            case SyntaxKind::InitAccessorDeclaration: {
                AccessorDeclarationSyntax* p = (AccessorDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("AccessorDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("keyword");
                PrintToken(p->keyword);
                PrintFieldName("bodyBlock");
                PrintNode(p->bodyBlock);
                PrintFieldName("expressionBody");
                PrintNode(p->expressionBody);
                PrintFieldName("semiColon");
                PrintToken(p->semiColon);
                indent--;
                break;
            }

            case SyntaxKind::AccessorList: {
                AccessorListSyntax* p = (AccessorListSyntax*)syntaxBase;
                PrintNodeHeader("AccessorListSyntax", syntaxBase);
                indent++;
                PrintFieldName("openBraceToken");
                PrintToken(p->openBraceToken);
                PrintFieldName("accessors");
                PrintSyntaxList((SyntaxListUntyped*)p->accessors);
                PrintFieldName("closeBraceToken");
                PrintToken(p->closeBraceToken);
                indent--;
                break;
            }

            case SyntaxKind::IndexerDeclaration: {
                IndexerDeclarationSyntax* p = (IndexerDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("IndexerDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("thisKeyword");
                PrintToken(p->thisKeyword);
                PrintFieldName("parameters");
                PrintNode(p->parameters);
                PrintFieldName("accessorList");
                PrintNode(p->accessorList);
                PrintFieldName("expressionBody");
                PrintNode(p->expressionBody);
                PrintFieldName("semiColon");
                PrintToken(p->semiColon);
                indent--;
                break;
            }

            case SyntaxKind::PropertyDeclaration: {
                PropertyDeclarationSyntax* p = (PropertyDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("PropertyDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("type");
                PrintNode(p->type);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("accessorList");
                PrintNode(p->accessorList);
                PrintFieldName("expressionBody");
                PrintNode(p->expressionBody);
                PrintFieldName("initializer");
                PrintNode(p->initializer);
                PrintFieldName("semiColon");
                PrintToken(p->semiColon);
                indent--;
                break;
            }

            case SyntaxKind::MethodDeclaration: {
                MethodDeclarationSyntax* p = (MethodDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("MethodDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("attributes");
                PrintSyntaxList((SyntaxListUntyped*)p->attributes);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("returnType");
                PrintNode(p->returnType);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("typeParameterList");
                PrintNode(p->typeParameterList);
                PrintFieldName("parameterList");
                PrintNode(p->parameterList);
                PrintFieldName("constraintClauses");
                PrintSyntaxList((SyntaxListUntyped*)p->constraintClauses);
                PrintFieldName("body");
                PrintNode(p->body);
                PrintFieldName("expressionBody");
                PrintNode(p->expressionBody);
                PrintFieldName("semicolonToken");
                PrintToken(p->semicolonToken);
                indent--;
                break;
            }

            case SyntaxKind::UsingNamespaceDeclaration: {
                UsingNamespaceDeclarationSyntax* p = (UsingNamespaceDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("UsingNamespaceDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("usingKeyword");
                PrintToken(p->usingKeyword);
                PrintFieldName("namePath");
                PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->namePath);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::UsingDeclaration: {
                UsingDeclarationSyntax* p = (UsingDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("UsingDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("usingKeyword");
                PrintToken(p->usingKeyword);
                PrintFieldName("staticKeyword");
                PrintToken(p->staticKeyword);
                PrintFieldName("alias");
                PrintNode(p->alias);
                PrintFieldName("namespaceOrType");
                PrintNode(p->namespaceOrType);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::ExternDeclaration: {
                ExternDeclarationSyntax* p = (ExternDeclarationSyntax*)syntaxBase;
                PrintNodeHeader("ExternDeclarationSyntax", syntaxBase);
                indent++;
                PrintFieldName("externKeyword");
                PrintToken(p->externKeyword);
                PrintFieldName("modifiers");
                PrintTokenList(p->modifiers);
                PrintFieldName("returnType");
                PrintNode(p->returnType);
                PrintFieldName("identifier");
                PrintToken(p->identifier);
                PrintFieldName("parameterList");
                PrintNode(p->parameterList);
                PrintFieldName("semicolon");
                PrintToken(p->semicolon);
                indent--;
                break;
            }

            case SyntaxKind::CompilationUnit: {
                CompilationUnitSyntax* p = (CompilationUnitSyntax*)syntaxBase;
                PrintNodeHeader("CompilationUnitSyntax", syntaxBase);
                indent++;
                PrintFieldName("members");
                PrintSyntaxList((SyntaxListUntyped*)p->members);
                PrintFieldName("eof");
                PrintToken(p->eof);
                indent--;
                break;
            }

            default: {
                break;
            }
            
        }
    }
}
