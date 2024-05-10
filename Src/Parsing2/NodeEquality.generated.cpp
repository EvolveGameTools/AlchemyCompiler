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

            default: {
                UNREACHABLE("NodesEqual");
                return true;
            }
            
        }        
        
    }
    
}