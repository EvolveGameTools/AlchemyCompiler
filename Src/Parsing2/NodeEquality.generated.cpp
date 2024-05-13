#include "./NodeEquality.h"

namespace Alchemy::Compilation {
    
    bool NodesEqual(SyntaxBase * a, SyntaxBase * b, NodeEqualityOptions options) {
    
        if(a == nullptr && b == nullptr) {
            return true;
        }
        
        if(a == nullptr || b == nullptr) {
            return false;
        }

        if(a->GetKind() != b->GetKind()) {
            return false;
        }
        
        switch(a->GetKind()) {
            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* pA = (ArrayRankSpecifierSyntax*)a;
                ArrayRankSpecifierSyntax* pB = (ArrayRankSpecifierSyntax*)b;
                if(!TokensEqual(pA->open, pB->open, options)) return false;
                if(!SeparatedSyntaxListEqual((SeparatedSyntaxListUntyped*)pA->ranks, (SeparatedSyntaxListUntyped*)pB->ranks, options)) return false;
                if(!TokensEqual(pA->close, pB->close, options)) return false;
                return true;
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* pA = (TypeArgumentListSyntax*)a;
                TypeArgumentListSyntax* pB = (TypeArgumentListSyntax*)b;
                if(!TokensEqual(pA->lessThanToken, pB->lessThanToken, options)) return false;
                if(!SeparatedSyntaxListEqual((SeparatedSyntaxListUntyped*)pA->arguments, (SeparatedSyntaxListUntyped*)pB->arguments, options)) return false;
                if(!TokensEqual(pA->greaterThanToken, pB->greaterThanToken, options)) return false;
                return true;
            }

            case SyntaxKind::GenericName: {
                GenericNameSyntax* pA = (GenericNameSyntax*)a;
                GenericNameSyntax* pB = (GenericNameSyntax*)b;
                if(!TokensEqual(pA->identifier, pB->identifier, options)) return false;
                if(!NodesEqual(pA->typeArgumentList, pB->typeArgumentList, options)) return false;
                return true;
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* pA = (QualifiedNameSyntax*)a;
                QualifiedNameSyntax* pB = (QualifiedNameSyntax*)b;
                if(!NodesEqual(pA->left, pB->left, options)) return false;
                if(!TokensEqual(pA->dotToken, pB->dotToken, options)) return false;
                if(!NodesEqual(pA->right, pB->right, options)) return false;
                return true;
            }

            case SyntaxKind::IdentifierName: {
                IdentifierNameSyntax* pA = (IdentifierNameSyntax*)a;
                IdentifierNameSyntax* pB = (IdentifierNameSyntax*)b;
                if(!TokensEqual(pA->identifier, pB->identifier, options)) return false;
                return true;
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* pA = (PredefinedTypeSyntax*)a;
                PredefinedTypeSyntax* pB = (PredefinedTypeSyntax*)b;
                if(!TokensEqual(pA->typeToken, pB->typeToken, options)) return false;
                return true;
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* pA = (TupleElementSyntax*)a;
                TupleElementSyntax* pB = (TupleElementSyntax*)b;
                if(!NodesEqual(pA->type, pB->type, options)) return false;
                if(!TokensEqual(pA->identifier, pB->identifier, options)) return false;
                return true;
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* pA = (TupleTypeSyntax*)a;
                TupleTypeSyntax* pB = (TupleTypeSyntax*)b;
                if(!TokensEqual(pA->openParenToken, pB->openParenToken, options)) return false;
                if(!SeparatedSyntaxListEqual((SeparatedSyntaxListUntyped*)pA->elements, (SeparatedSyntaxListUntyped*)pB->elements, options)) return false;
                if(!TokensEqual(pA->closeParenToken, pB->closeParenToken, options)) return false;
                return true;
            }

            case SyntaxKind::ArrayType: {
                ArrayTypeSyntax* pA = (ArrayTypeSyntax*)a;
                ArrayTypeSyntax* pB = (ArrayTypeSyntax*)b;
                if(!NodesEqual(pA->elementType, pB->elementType, options)) return false;
                if(!SyntaxListEqual((SyntaxListUntyped*)pA->ranks, (SyntaxListUntyped*)pB->ranks, options)) return false;
                return true;
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* pA = (RefTypeSyntax*)a;
                RefTypeSyntax* pB = (RefTypeSyntax*)b;
                if(!TokensEqual(pA->refKeyword, pB->refKeyword, options)) return false;
                if(!TokensEqual(pA->readonlyKeyword, pB->readonlyKeyword, options)) return false;
                if(!NodesEqual(pA->type, pB->type, options)) return false;
                return true;
            }

            case SyntaxKind::OmittedArraySizeExpression: {
                OmittedArraySizeExpressionSyntax* pA = (OmittedArraySizeExpressionSyntax*)a;
                OmittedArraySizeExpressionSyntax* pB = (OmittedArraySizeExpressionSyntax*)b;
                if(!TokensEqual(pA->token, pB->token, options)) return false;
                return true;
            }

            case SyntaxKind::NullableType: {
                NullableType* pA = (NullableType*)a;
                NullableType* pB = (NullableType*)b;
                if(!NodesEqual(pA->elementType, pB->elementType, options)) return false;
                if(!TokensEqual(pA->questionMark, pB->questionMark, options)) return false;
                return true;
            }

            case SyntaxKind::Argument: {
                ArgumentSyntax* pA = (ArgumentSyntax*)a;
                ArgumentSyntax* pB = (ArgumentSyntax*)b;
                if(!TokensEqual(pA->nameColon, pB->nameColon, options)) return false;
                if(!TokensEqual(pA->refKindKeyword, pB->refKindKeyword, options)) return false;
                if(!NodesEqual(pA->expression, pB->expression, options)) return false;
                return true;
            }

            case SyntaxKind::BracketedArgumentList: {
                BracketedArgumentListSyntax* pA = (BracketedArgumentListSyntax*)a;
                BracketedArgumentListSyntax* pB = (BracketedArgumentListSyntax*)b;
                if(!TokensEqual(pA->openBracket, pB->openBracket, options)) return false;
                if(!SeparatedSyntaxListEqual((SeparatedSyntaxListUntyped*)pA->arguments, (SeparatedSyntaxListUntyped*)pB->arguments, options)) return false;
                if(!TokensEqual(pA->closeBracket, pB->closeBracket, options)) return false;
                return true;
            }

            case SyntaxKind::EqualsValueClause: {
                EqualsValueClauseSyntax* pA = (EqualsValueClauseSyntax*)a;
                EqualsValueClauseSyntax* pB = (EqualsValueClauseSyntax*)b;
                if(!TokensEqual(pA->equalsToken, pB->equalsToken, options)) return false;
                if(!NodesEqual(pA->value, pB->value, options)) return false;
                return true;
            }

            case SyntaxKind::RefExpression: {
                RefExpressionSyntax* pA = (RefExpressionSyntax*)a;
                RefExpressionSyntax* pB = (RefExpressionSyntax*)b;
                if(!TokensEqual(pA->refKeyword, pB->refKeyword, options)) return false;
                if(!NodesEqual(pA->expression, pB->expression, options)) return false;
                return true;
            }

            case SyntaxKind::VariableDeclarator: {
                VariableDeclaratorSyntax* pA = (VariableDeclaratorSyntax*)a;
                VariableDeclaratorSyntax* pB = (VariableDeclaratorSyntax*)b;
                if(!TokensEqual(pA->identifier, pB->identifier, options)) return false;
                if(!NodesEqual(pA->argumentList, pB->argumentList, options)) return false;
                if(!NodesEqual(pA->initializer, pB->initializer, options)) return false;
                return true;
            }

            case SyntaxKind::TypeParameter: {
                TypeParameterSyntax* pA = (TypeParameterSyntax*)a;
                TypeParameterSyntax* pB = (TypeParameterSyntax*)b;
                if(!TokensEqual(pA->identifier, pB->identifier, options)) return false;
                return true;
            }

            case SyntaxKind::TypeParameterList: {
                TypeParameterListSyntax* pA = (TypeParameterListSyntax*)a;
                TypeParameterListSyntax* pB = (TypeParameterListSyntax*)b;
                if(!TokensEqual(pA->lessThanToken, pB->lessThanToken, options)) return false;
                if(!SeparatedSyntaxListEqual((SeparatedSyntaxListUntyped*)pA->parameters, (SeparatedSyntaxListUntyped*)pB->parameters, options)) return false;
                if(!TokensEqual(pA->greaterThanToken, pB->greaterThanToken, options)) return false;
                return true;
            }

            case SyntaxKind::Parameter: {
                ParameterSyntax* pA = (ParameterSyntax*)a;
                ParameterSyntax* pB = (ParameterSyntax*)b;
                if(!TokenListsEqual(pA->modifiers, pB->modifiers, options)) return false;
                if(!NodesEqual(pA->type, pB->type, options)) return false;
                if(!TokensEqual(pA->identifier, pB->identifier, options)) return false;
                if(!NodesEqual(pA->defaultValue, pB->defaultValue, options)) return false;
                return true;
            }

            case SyntaxKind::ParameterList: {
                ParameterListSyntax* pA = (ParameterListSyntax*)a;
                ParameterListSyntax* pB = (ParameterListSyntax*)b;
                if(!TokensEqual(pA->openParen, pB->openParen, options)) return false;
                if(!SeparatedSyntaxListEqual((SeparatedSyntaxListUntyped*)pA->parameters, (SeparatedSyntaxListUntyped*)pB->parameters, options)) return false;
                if(!TokensEqual(pA->closeParen, pB->closeParen, options)) return false;
                return true;
            }

            case SyntaxKind::ConstraintClauses: {
                ConstraintClausesSyntax* pA = (ConstraintClausesSyntax*)a;
                ConstraintClausesSyntax* pB = (ConstraintClausesSyntax*)b;
                if(!TokensEqual(pA->dummy, pB->dummy, options)) return false;
                return true;
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* pA = (LocalFunctionStatementSyntax*)a;
                LocalFunctionStatementSyntax* pB = (LocalFunctionStatementSyntax*)b;
                if(!TokenListsEqual(pA->modifiers, pB->modifiers, options)) return false;
                if(!NodesEqual(pA->returnType, pB->returnType, options)) return false;
                if(!TokensEqual(pA->identifier, pB->identifier, options)) return false;
                if(!NodesEqual(pA->typeParameters, pB->typeParameters, options)) return false;
                if(!NodesEqual(pA->parameters, pB->parameters, options)) return false;
                if(!NodesEqual(pA->constraints, pB->constraints, options)) return false;
                if(!NodesEqual(pA->body, pB->body, options)) return false;
                return true;
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* pA = (VariableDeclarationSyntax*)a;
                VariableDeclarationSyntax* pB = (VariableDeclarationSyntax*)b;
                if(!NodesEqual(pA->type, pB->type, options)) return false;
                if(!SeparatedSyntaxListEqual((SeparatedSyntaxListUntyped*)pA->variables, (SeparatedSyntaxListUntyped*)pB->variables, options)) return false;
                return true;
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* pA = (FieldDeclarationSyntax*)a;
                FieldDeclarationSyntax* pB = (FieldDeclarationSyntax*)b;
                if(!TokenListsEqual(pA->modifiers, pB->modifiers, options)) return false;
                if(!NodesEqual(pA->declaration, pB->declaration, options)) return false;
                if(!TokensEqual(pA->semicolonToken, pB->semicolonToken, options)) return false;
                return true;
            }

            default: {
                UNREACHABLE("NodesEqual");
                return true;
            }
            
        }        
        
    }
    
}