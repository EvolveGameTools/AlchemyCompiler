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
                if(p->open.IsValid()) return p->open;
                if(p->ranks->itemCount != 0) return GetFirstToken(p->ranks->items[0]);
                if(p->close.IsValid()) return p->close;
                return SyntaxToken();
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* p = (TypeArgumentListSyntax*)syntaxBase;
                if(p->lessThanToken.IsValid()) return p->lessThanToken;
                if(p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
                if(p->greaterThanToken.IsValid()) return p->greaterThanToken;
                return SyntaxToken();
            }

            case SyntaxKind::GenericName: {
                GenericNameSyntax* p = (GenericNameSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeArgumentList != nullptr) return GetFirstToken((SyntaxBase*)p->typeArgumentList);
                return SyntaxToken();
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* p = (QualifiedNameSyntax*)syntaxBase;
                if(p->left != nullptr) return GetFirstToken((SyntaxBase*)p->left);
                if(p->dotToken.IsValid()) return p->dotToken;
                if(p->right != nullptr) return GetFirstToken((SyntaxBase*)p->right);
                return SyntaxToken();
            }

            case SyntaxKind::IdentifierName: {
                IdentifierNameSyntax* p = (IdentifierNameSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                if(p->typeToken.IsValid()) return p->typeToken;
                return SyntaxToken();
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                if(p->openParenToken.IsValid()) return p->openParenToken;
                if(p->elements->itemCount != 0) return GetFirstToken(p->elements->items[0]);
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::ArrayType: {
                ArrayTypeSyntax* p = (ArrayTypeSyntax*)syntaxBase;
                if(p->elementType != nullptr) return GetFirstToken((SyntaxBase*)p->elementType);
                if(p->ranks->size != 0) return GetFirstToken(p->ranks->array[0]);
                return SyntaxToken();
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                if(p->refKeyword.IsValid()) return p->refKeyword;
                if(p->readonlyKeyword.IsValid()) return p->readonlyKeyword;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::OmittedArraySizeExpression: {
                OmittedArraySizeExpressionSyntax* p = (OmittedArraySizeExpressionSyntax*)syntaxBase;
                if(p->token.IsValid()) return p->token;
                return SyntaxToken();
            }

            case SyntaxKind::NullableType: {
                NullableType* p = (NullableType*)syntaxBase;
                if(p->elementType != nullptr) return GetFirstToken((SyntaxBase*)p->elementType);
                if(p->questionMark.IsValid()) return p->questionMark;
                return SyntaxToken();
            }

            case SyntaxKind::Argument: {
                ArgumentSyntax* p = (ArgumentSyntax*)syntaxBase;
                if(p->nameColon.IsValid()) return p->nameColon;
                if(p->refKindKeyword.IsValid()) return p->refKindKeyword;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::BracketedArgumentList: {
                BracketedArgumentListSyntax* p = (BracketedArgumentListSyntax*)syntaxBase;
                if(p->openBracket.IsValid()) return p->openBracket;
                if(p->arguments->itemCount != 0) return GetFirstToken(p->arguments->items[0]);
                if(p->closeBracket.IsValid()) return p->closeBracket;
                return SyntaxToken();
            }

            case SyntaxKind::EqualsValueClause: {
                EqualsValueClauseSyntax* p = (EqualsValueClauseSyntax*)syntaxBase;
                if(p->equalsToken.IsValid()) return p->equalsToken;
                if(p->value != nullptr) return GetFirstToken((SyntaxBase*)p->value);
                return SyntaxToken();
            }

            case SyntaxKind::RefExpression: {
                RefExpressionSyntax* p = (RefExpressionSyntax*)syntaxBase;
                if(p->refKeyword.IsValid()) return p->refKeyword;
                if(p->expression != nullptr) return GetFirstToken((SyntaxBase*)p->expression);
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclarator: {
                VariableDeclaratorSyntax* p = (VariableDeclaratorSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->argumentList != nullptr) return GetFirstToken((SyntaxBase*)p->argumentList);
                if(p->initializer != nullptr) return GetFirstToken((SyntaxBase*)p->initializer);
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameter: {
                TypeParameterSyntax* p = (TypeParameterSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameterList: {
                TypeParameterListSyntax* p = (TypeParameterListSyntax*)syntaxBase;
                if(p->lessThanToken.IsValid()) return p->lessThanToken;
                if(p->parameters->itemCount != 0) return GetFirstToken(p->parameters->items[0]);
                if(p->greaterThanToken.IsValid()) return p->greaterThanToken;
                return SyntaxToken();
            }

            case SyntaxKind::Parameter: {
                ParameterSyntax* p = (ParameterSyntax*)syntaxBase;
                if(p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->defaultValue != nullptr) return GetFirstToken((SyntaxBase*)p->defaultValue);
                return SyntaxToken();
            }

            case SyntaxKind::ParameterList: {
                ParameterListSyntax* p = (ParameterListSyntax*)syntaxBase;
                if(p->openParen.IsValid()) return p->openParen;
                if(p->parameters->itemCount != 0) return GetFirstToken(p->parameters->items[0]);
                if(p->closeParen.IsValid()) return p->closeParen;
                return SyntaxToken();
            }

            case SyntaxKind::ConstraintClauses: {
                ConstraintClausesSyntax* p = (ConstraintClausesSyntax*)syntaxBase;
                if(p->dummy.IsValid()) return p->dummy;
                return SyntaxToken();
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                if(p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->returnType != nullptr) return GetFirstToken((SyntaxBase*)p->returnType);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->typeParameters != nullptr) return GetFirstToken((SyntaxBase*)p->typeParameters);
                if(p->parameters != nullptr) return GetFirstToken((SyntaxBase*)p->parameters);
                if(p->constraints != nullptr) return GetFirstToken((SyntaxBase*)p->constraints);
                if(p->body != nullptr) return GetFirstToken((SyntaxBase*)p->body);
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* p = (VariableDeclarationSyntax*)syntaxBase;
                if(p->type != nullptr) return GetFirstToken((SyntaxBase*)p->type);
                if(p->variables->itemCount != 0) return GetFirstToken(p->variables->items[0]);
                return SyntaxToken();
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* p = (FieldDeclarationSyntax*)syntaxBase;
                if(p->modifiers->size != 0) return p->modifiers->array[0];
                if(p->declaration != nullptr) return GetFirstToken((SyntaxBase*)p->declaration);
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                return SyntaxToken();
            }

            default: {
                return SyntaxToken();
            }
            
        }        
        
    }
    
    SyntaxToken GetLastToken(SyntaxBase * syntaxBase) {
    
        if(syntaxBase == nullptr) {
            return SyntaxToken();
        }
        
        switch(syntaxBase->GetKind()) {
            case SyntaxKind::ArrayRankSpecifier: {
                ArrayRankSpecifierSyntax* p = (ArrayRankSpecifierSyntax*)syntaxBase;
                if(p->close.IsValid()) return p->close;
                if(p->ranks->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->ranks->items[p->ranks->itemCount - 1]);
                    SyntaxToken b = p->ranks->separatorCount == 0 ? SyntaxToken() : p->ranks->separators[p->ranks->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->open.IsValid()) return p->open;
                return SyntaxToken();
            }

            case SyntaxKind::TypeArgumentList: {
                TypeArgumentListSyntax* p = (TypeArgumentListSyntax*)syntaxBase;
                if(p->greaterThanToken.IsValid()) return p->greaterThanToken;
                if(p->arguments->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->arguments->items[p->arguments->itemCount - 1]);
                    SyntaxToken b = p->arguments->separatorCount == 0 ? SyntaxToken() : p->arguments->separators[p->arguments->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->lessThanToken.IsValid()) return p->lessThanToken;
                return SyntaxToken();
            }

            case SyntaxKind::GenericName: {
                GenericNameSyntax* p = (GenericNameSyntax*)syntaxBase;
                if(p->typeArgumentList != nullptr) return GetLastToken((SyntaxBase*)p->typeArgumentList);
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::QualifiedName: {
                QualifiedNameSyntax* p = (QualifiedNameSyntax*)syntaxBase;
                if(p->right != nullptr) return GetLastToken((SyntaxBase*)p->right);
                if(p->dotToken.IsValid()) return p->dotToken;
                if(p->left != nullptr) return GetLastToken((SyntaxBase*)p->left);
                return SyntaxToken();
            }

            case SyntaxKind::IdentifierName: {
                IdentifierNameSyntax* p = (IdentifierNameSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::PredefinedType: {
                PredefinedTypeSyntax* p = (PredefinedTypeSyntax*)syntaxBase;
                if(p->typeToken.IsValid()) return p->typeToken;
                return SyntaxToken();
            }

            case SyntaxKind::TupleElement: {
                TupleElementSyntax* p = (TupleElementSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::TupleType: {
                TupleTypeSyntax* p = (TupleTypeSyntax*)syntaxBase;
                if(p->closeParenToken.IsValid()) return p->closeParenToken;
                if(p->elements->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->elements->items[p->elements->itemCount - 1]);
                    SyntaxToken b = p->elements->separatorCount == 0 ? SyntaxToken() : p->elements->separators[p->elements->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openParenToken.IsValid()) return p->openParenToken;
                return SyntaxToken();
            }

            case SyntaxKind::ArrayType: {
                ArrayTypeSyntax* p = (ArrayTypeSyntax*)syntaxBase;
                if(p->ranks->size != 0) return GetLastToken(p->ranks->array[p->ranks->size - 1]);
                if(p->elementType != nullptr) return GetLastToken((SyntaxBase*)p->elementType);
                return SyntaxToken();
            }

            case SyntaxKind::RefType: {
                RefTypeSyntax* p = (RefTypeSyntax*)syntaxBase;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->readonlyKeyword.IsValid()) return p->readonlyKeyword;
                if(p->refKeyword.IsValid()) return p->refKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::OmittedArraySizeExpression: {
                OmittedArraySizeExpressionSyntax* p = (OmittedArraySizeExpressionSyntax*)syntaxBase;
                if(p->token.IsValid()) return p->token;
                return SyntaxToken();
            }

            case SyntaxKind::NullableType: {
                NullableType* p = (NullableType*)syntaxBase;
                if(p->questionMark.IsValid()) return p->questionMark;
                if(p->elementType != nullptr) return GetLastToken((SyntaxBase*)p->elementType);
                return SyntaxToken();
            }

            case SyntaxKind::Argument: {
                ArgumentSyntax* p = (ArgumentSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->refKindKeyword.IsValid()) return p->refKindKeyword;
                if(p->nameColon.IsValid()) return p->nameColon;
                return SyntaxToken();
            }

            case SyntaxKind::BracketedArgumentList: {
                BracketedArgumentListSyntax* p = (BracketedArgumentListSyntax*)syntaxBase;
                if(p->closeBracket.IsValid()) return p->closeBracket;
                if(p->arguments->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->arguments->items[p->arguments->itemCount - 1]);
                    SyntaxToken b = p->arguments->separatorCount == 0 ? SyntaxToken() : p->arguments->separators[p->arguments->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openBracket.IsValid()) return p->openBracket;
                return SyntaxToken();
            }

            case SyntaxKind::EqualsValueClause: {
                EqualsValueClauseSyntax* p = (EqualsValueClauseSyntax*)syntaxBase;
                if(p->value != nullptr) return GetLastToken((SyntaxBase*)p->value);
                if(p->equalsToken.IsValid()) return p->equalsToken;
                return SyntaxToken();
            }

            case SyntaxKind::RefExpression: {
                RefExpressionSyntax* p = (RefExpressionSyntax*)syntaxBase;
                if(p->expression != nullptr) return GetLastToken((SyntaxBase*)p->expression);
                if(p->refKeyword.IsValid()) return p->refKeyword;
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclarator: {
                VariableDeclaratorSyntax* p = (VariableDeclaratorSyntax*)syntaxBase;
                if(p->initializer != nullptr) return GetLastToken((SyntaxBase*)p->initializer);
                if(p->argumentList != nullptr) return GetLastToken((SyntaxBase*)p->argumentList);
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameter: {
                TypeParameterSyntax* p = (TypeParameterSyntax*)syntaxBase;
                if(p->identifier.IsValid()) return p->identifier;
                return SyntaxToken();
            }

            case SyntaxKind::TypeParameterList: {
                TypeParameterListSyntax* p = (TypeParameterListSyntax*)syntaxBase;
                if(p->greaterThanToken.IsValid()) return p->greaterThanToken;
                if(p->parameters->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->parameters->items[p->parameters->itemCount - 1]);
                    SyntaxToken b = p->parameters->separatorCount == 0 ? SyntaxToken() : p->parameters->separators[p->parameters->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->lessThanToken.IsValid()) return p->lessThanToken;
                return SyntaxToken();
            }

            case SyntaxKind::Parameter: {
                ParameterSyntax* p = (ParameterSyntax*)syntaxBase;
                if(p->defaultValue != nullptr) return GetLastToken((SyntaxBase*)p->defaultValue);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                if(p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            case SyntaxKind::ParameterList: {
                ParameterListSyntax* p = (ParameterListSyntax*)syntaxBase;
                if(p->closeParen.IsValid()) return p->closeParen;
                if(p->parameters->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->parameters->items[p->parameters->itemCount - 1]);
                    SyntaxToken b = p->parameters->separatorCount == 0 ? SyntaxToken() : p->parameters->separators[p->parameters->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->openParen.IsValid()) return p->openParen;
                return SyntaxToken();
            }

            case SyntaxKind::ConstraintClauses: {
                ConstraintClausesSyntax* p = (ConstraintClausesSyntax*)syntaxBase;
                if(p->dummy.IsValid()) return p->dummy;
                return SyntaxToken();
            }

            case SyntaxKind::LocalFunctionStatement: {
                LocalFunctionStatementSyntax* p = (LocalFunctionStatementSyntax*)syntaxBase;
                if(p->body != nullptr) return GetLastToken((SyntaxBase*)p->body);
                if(p->constraints != nullptr) return GetLastToken((SyntaxBase*)p->constraints);
                if(p->parameters != nullptr) return GetLastToken((SyntaxBase*)p->parameters);
                if(p->typeParameters != nullptr) return GetLastToken((SyntaxBase*)p->typeParameters);
                if(p->identifier.IsValid()) return p->identifier;
                if(p->returnType != nullptr) return GetLastToken((SyntaxBase*)p->returnType);
                if(p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            case SyntaxKind::VariableDeclaration: {
                VariableDeclarationSyntax* p = (VariableDeclarationSyntax*)syntaxBase;
                if(p->variables->itemCount != 0) {
                    SyntaxToken a = GetLastToken(p->variables->items[p->variables->itemCount - 1]);
                    SyntaxToken b = p->variables->separatorCount == 0 ? SyntaxToken() : p->variables->separators[p->variables->separatorCount - 1];
                    return a.GetId() > b.GetId() ? a : b;
                }
                if(p->type != nullptr) return GetLastToken((SyntaxBase*)p->type);
                return SyntaxToken();
            }

            case SyntaxKind::FieldDeclaration: {
                FieldDeclarationSyntax* p = (FieldDeclarationSyntax*)syntaxBase;
                if(p->semicolonToken.IsValid()) return p->semicolonToken;
                if(p->declaration != nullptr) return GetLastToken((SyntaxBase*)p->declaration);
                if(p->modifiers->size != 0) return p->modifiers->array[p->modifiers->size - 1];
                return SyntaxToken();
            }

            default: {
                return SyntaxToken();
            }
            
        }        
        
    }
    
}
