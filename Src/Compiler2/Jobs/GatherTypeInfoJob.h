#pragma once

#include "../../JobSystem/JobSystem.h"
#include "../../JobSystem/Job.h"
#include "../SourceFileInfo.h"
#include "../../Parsing3/SyntaxBase.h"
#include "../../Parsing3/SyntaxNodes.h"

namespace Alchemy::Compilation {

    struct GatherTypeInfoJob : Jobs::IJob {

        CheckedArray<SourceFileInfo*> files;

        explicit GatherTypeInfoJob(CheckedArray<SourceFileInfo*> files);

        void Execute(int32 idx) override;

    private:
        SourceFileInfo * fileInfo {};

        void CreateTypeInfo(CheckedArray<TypeInfo*> typeInfos, int32 * typeInfoIndex, MemberDeclarationSyntax* pSyntax);

        void CreateClassDeclaration(CheckedArray<TypeInfo*> typeInfos, int32 * typeInfoIndex, ClassDeclarationSyntax* pSyntax);
        void CreateStructDeclaration(CheckedArray<TypeInfo*> typeInfos, int32 * typeInfoIndex, StructDeclarationSyntax* pSyntax);

        void HandleModifiers(TypeInfo* pTypeInfo, TokenList* modifiers);

        void HandleMembers(TypeInfo* pTypeInfo, SyntaxList<MemberDeclarationSyntax>* members);

        void HandleAttributes(TypeInfo* pTypeInfo, SyntaxList<AttributeListSyntax>* attributes);

        void HandleBaseTypes(TypeInfo* pInfo, BaseListSyntax* pSyntax);

        void MakeGenericArgumentTypes(TypeInfo * pInfo, CheckedArray<TypeInfo*> typeInfos, int32 * typeInfoIndex, TypeParameterListSyntax* typeParameterList);
    };

}