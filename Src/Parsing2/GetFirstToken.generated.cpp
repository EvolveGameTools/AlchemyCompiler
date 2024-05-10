#include "./SyntaxBase.h"
#include "./SyntaxNodes.h"

namespace Alchemy::Compilation {
    
    SyntaxToken GetFirstToken(SyntaxBase * syntaxBase) {
    
        if(syntaxBase == nullptr) {
            return SyntaxToken();
        }
        
        switch(syntaxBase->GetKind()) {
            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*)syntaxBase;
                return p->open;
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* p = (TypeArgumentListSyntax*)syntaxBase;
                return p->lessThanToken;
            }

            case SyntaxKind::GenericName: {
                GenericNameSyntax* p = (GenericNameSyntax*)syntaxBase;
                return p->identifier;
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* p = (QualifiedNameSyntax*)syntaxBase;
                return GetFirstToken((SyntaxBase*)p->left);
            }

            case SyntaxKind::IdentifierName: {
                IdentifierNameSyntax* p = (IdentifierNameSyntax*)syntaxBase;
                return p->identifier;
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                return p->typeToken;
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                return GetFirstToken((SyntaxBase*)p->type);
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                return p->openParenToken;
            }

            case SyntaxKind::ArrayType: {
                ArrayTypeSyntax* p = (ArrayTypeSyntax*)syntaxBase;
                return GetFirstToken((SyntaxBase*)p->elementType);
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                return p->refKeyword;
            }

            case SyntaxKind::OmittedArraySizeExpression: {
                OmittedArraySizeExpressionSyntax* p = (OmittedArraySizeExpressionSyntax*)syntaxBase;
                return p->token;
            }

            case SyntaxKind::NullableType: {
                NullableType* p = (NullableType*)syntaxBase;
                return GetFirstToken((SyntaxBase*)p->elementType);
            }

            default: {
                return SyntaxToken();
            }
            
        }        
        
    }
    
}
