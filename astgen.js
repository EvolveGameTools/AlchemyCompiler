const fs = require('fs');

function findCppStructs(filename) {
    const fileContent = fs.readFileSync(filename, 'utf8');

    var lines = fileContent.split('\n');

    for (var i = 0; i < lines.length; i++) {
        lines[i] = lines[i].trim();
        lines[i] = lines[i].replace(/\/\/.*$/, "");
        lines[i] = lines[i].trim();
    }

    var output = [];

    var lastWasAbstract = false;
    for (i = 0; i < lines.length; i++) {

        const line = lines[i];
        if (line.length === 0 || line.startsWith("//")) continue;

        if (line.startsWith("abstract")) {
            // if the only thing on the line was 'abstract' the next line is considered abstract instead
            lastWasAbstract = line === "abstract";
            continue;
        }

        if (lastWasAbstract) {
            lastWasAbstract = false;
            continue;
        }

        const structRegex = /struct\s+(\w+)\s*(?::\s*(\w+))?\s*[{;]/;

        const match = line.match(structRegex);

        if (!(match && match[2])) {
            continue;
        }

        var obj = {};
        output.push(obj);
        obj.structName = match[1];
        obj.baseType = match[2];
        obj.fields = [];
        obj.validSyntaxKinds = [];

        var hasSyntaxKindList = false;

        i++;
        for (; i < lines.length; i++) {

            var candidate = lines[i];

            if (candidate.startsWith("//")) continue;
            if (candidate.length === 0) continue;

            if(candidate.startsWith("VALID_SYNTAX_KINDS")) {
                hasSyntaxKindList = true;
                i++;
                for(; i < lines.length; i++){
                    if(lines[i].startsWith("SyntaxKind::")) {
                        const l = lines[i];
                        var str = l.split("::")[1];
                        var commaIdx = str.indexOf(',');
                        if(commaIdx !== -1) {
                            str = str.substring(0, commaIdx);
                        }
                        str = str.trim();
                        console.log(obj.structName + " -> " + str);
                        obj.validSyntaxKinds.push(str);
                    }
                    if(lines[i].startsWith("};")){
                        break
                    }
                }

                i--;
                continue;

            }

            if ((candidate.startsWith(obj.structName) || candidate.startsWith("explicit")) && candidate.indexOf("(") >= 0) {
                break;
            }

            if (candidate.startsWith("};")) {
                break;
            }

            const match = candidate.match(/(\w+);$/);

            if (match != null) {
                const fieldName = match[0].substring(0, match[0].length - 1);
                const fieldType = candidate.substring(0, candidate.indexOf(match[0])).replace(/\s+/g, "");

                obj.fields.push({
                    fieldName: fieldName,
                    fieldType: fieldType,
                });

            }

        }

        if(!hasSyntaxKindList) {
            obj.validSyntaxKinds.push(stripSuffix(obj.structName, "Syntax"));
        }

    }

    return output;

}

const filename = './Src/Parsing3/SyntaxNodes.h';

const structs = findCppStructs(filename);

const caseIndent = "            ";
const statementIndent = caseIndent + "    ";

function stripSuffix(str, suffix) {
    if (str.endsWith(suffix)) {
        return str.slice(0, -suffix.length);
    }
    return str;
}

function capitalizeFirstLetter(str) {
    return str.charAt(0).toUpperCase() + str.slice(1);
}

function createBuilders(structs) {

    for (let i = 0; i < structs.length; i++) {
        const struct = structs[i];

        var str = "    struct " + struct.structName + "Builder";
        if (struct.baseType) {
            str += ": " + struct.baseType + "Builder {\n\n";
        } else {
            str += " : Builder {\n\n";
        }

        for (var f = 0; f < struct.fields.length; f++) {
            const field = struct.fields[f];
            str += "        ";

            if (field.fieldType.startsWith("SeparatedSyntaxList")) {
                field.builderType = "SeparatedSyntaxListBuilder*";
            } else if (field.fieldType.startsWith("SyntaxList")) {
                field.builderType = "SyntaxListBuilder*";
            } else if (field.fieldType === "SyntaxToken") {
                field.builderType = "SyntaxToken";
            } else {
                field.builderType = stripSuffix(field.fieldType, "*") + "Builder *";
            }
            str += field.builderType;
            str += " ";
            str += field.fieldName
            str += " {};\n";
        }

        for (f = 0; f < struct.fields.length; f++) {
            const field = struct.fields[f];
            str += "\n        inline ";
            str += struct.structName + "Builder* ";
            str += capitalizeFirstLetter(field.fieldName);
            str += "(";
            str += field.builderType;
            str += " builder";
            str += ") {\n";
            str += "            ";
            str += field.fieldName;
            str += " = builder;\n";
            str += "            ";
            str += "return this;\n";
            str += "        ";
            str += "}\n";
        }

        str += "\n        inline SyntaxBase* Build() override {\n";
        str += `            ${struct.structName}* retn = allocator->Allocate<${struct.structName}>(1);\n`;
        str += `            retn->_kind = SyntaxKind::${stripSuffix(struct.structName, "Syntax")};\n`;
        for (f = 0; f < struct.fields.length; f++) {
            const field = struct.fields[f];

            str += "            ";
            str += `retn->${field.fieldName} = `;
            if (field.fieldType === "SyntaxToken") {
                str += field.fieldName + ";\n";
            } else {
                str += `${field.fieldName} != nullptr ? (${field.fieldType})${field.fieldName}->Build() : nullptr;\n`;
            }
        }
        str += "            ";
        str += "return retn;\n        }\n";

        str += "\n    };\n";

        struct.builder = str;

        str = "\n        inline " + struct.structName + "Builder* ";
        str += struct.structName + "() { \n";
        str += `            ${struct.structName}Builder * retn = allocator->Allocate<${struct.structName}Builder>(1);\n`;
        str += `            new (retn) ${struct.structName}Builder();\n`;
        str += "            retn->allocator = allocator;\n";
        str += "            return retn;\n";
        str += "        }";

        struct.builderGen = str;

    }

}

function createCompareBlocks(structs) {
    for (var i = 0; i < structs.length; i++) {
        const struct = structs[i];

        var comp = "";
        comp = caseIndent + "case SyntaxKind::" + stripSuffix(struct.structName, "Syntax") + ": {\n";
        comp += statementIndent;
        comp += struct.structName;
        comp += `* pA = (${struct.structName}*)a;\n`
        comp += statementIndent;
        comp += struct.structName;
        comp += `* pB = (${struct.structName}*)b;\n`
        for (var f = 0; f < struct.fields.length; f++) {
            const field = struct.fields[f];
            comp += statementIndent;

            if (field.fieldType === "SyntaxToken") {
                comp += `if(!TokensEqual(pA->${field.fieldName}, pB->${field.fieldName}, options)) return false;\n`;
            } else if (field.fieldType.startsWith("TokenList")) {
                comp += `if(!TokenListsEqual(pA->${field.fieldName}, pB->${field.fieldName}, options)) return false;\n`;
            } else if (field.fieldType.startsWith("SyntaxList")) {
                comp += `if(!SyntaxListEqual((SyntaxListUntyped*)pA->${field.fieldName}, (SyntaxListUntyped*)pB->${field.fieldName}, options)) return false;\n`;
            } else if (field.fieldType.startsWith("SeparatedSyntaxList")) {
                comp += `if(!SeparatedSyntaxListEqual((SeparatedSyntaxListUntyped*)pA->${field.fieldName}, (SeparatedSyntaxListUntyped*)pB->${field.fieldName}, options)) return false;\n`;
            } else {
                comp += `if(!NodesEqual(pA->${field.fieldName}, pB->${field.fieldName}, options)) return false;\n`;
            }

        }
        comp += statementIndent;
        comp += "return true;\n";
        comp += caseIndent;
        comp += '}\n';
        struct.compBlock = comp;
    }
}

function createPrintBlocks(structs) {
    for (var i = 0; i < structs.length; i++) {
        const struct = structs[i];

        struct.printBlock = "";

        for(var j = 0; j < struct.validSyntaxKinds.length; j++) {

            struct.printBlock += caseIndent + "case SyntaxKind::" + struct.validSyntaxKinds[j] + ": {\n";
            struct.printBlock += statementIndent;
            struct.printBlock += struct.structName;
            struct.printBlock += `* p = (${struct.structName}*)syntaxBase;\n`
            struct.printBlock += statementIndent;
            struct.printBlock += `PrintNodeHeader("${struct.structName}", syntaxBase);\n`
            struct.printBlock += statementIndent;
            struct.printBlock += "indent++;\n";
            for (var f = 0; f < struct.fields.length; f++) {
                const field = struct.fields[f];
                struct.printBlock += statementIndent;
                struct.printBlock += `PrintFieldName("${field.fieldName}");\n`;
                struct.printBlock += statementIndent;

                if (field.fieldType === "SyntaxToken") {
                    struct.printBlock += `PrintToken(p->${field.fieldName});\n`;
                } else if (field.fieldType.startsWith("TokenList")) {
                    struct.printBlock += `PrintTokenList(p->${field.fieldName});\n`
                } else if (field.fieldType.startsWith("SyntaxList")) {
                    struct.printBlock += `PrintSyntaxList((SyntaxListUntyped*)p->${field.fieldName});\n`;
                } else if (field.fieldType.startsWith("SeparatedSyntaxList")) {
                    struct.printBlock += `PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->${field.fieldName});\n`;
                } else {
                    struct.printBlock += `PrintNode(p->${field.fieldName});\n`;
                }

            }
            struct.printBlock += statementIndent;
            struct.printBlock += "indent--;\n";
            struct.printBlock += statementIndent;
            struct.printBlock += "break;\n";
            struct.printBlock += caseIndent;
            struct.printBlock += '}\n';

        }
    }
}

function findSkippedTokens(structs) {
    for (let i = 0; i < structs.length; i++) {
        const struct = structs[i];

        if (struct.fields.length === 0) {
            continue
        }

        var block = "";

        for(var j = 0; j < struct.validSyntaxKinds.length; j++) {
            block += caseIndent + "case SyntaxKind::" + struct.validSyntaxKinds[j] + ": {\n";
            block += statementIndent;
            block += struct.structName;
            block += `* p = (${struct.structName}*)syntaxBase;\n`

            for (var f = 0; f < struct.fields.length; f++) {
                const field = struct.fields[f];
                const x = `p->${field.fieldName}`;
                block += statementIndent;
                if (field.fieldType === "SyntaxToken") {
                    block += `TouchToken(${x});\n`;
                } else if (field.fieldType.startsWith("TokenList")) {
                    block += `TouchTokenList(${x});\n`;
                } else if (field.fieldType.startsWith("SyntaxList")) {
                    block += `TouchSyntaxList((SyntaxListUntyped*)${x});\n`;
                } else if (field.fieldType.startsWith("SeparatedSyntaxList")) {
                    block += `TouchSeparatedSyntaxList((SeparatedSyntaxListUntyped*)${x});\n`;
                } else {
                    block += `TouchNode(${x});\n`;
                }
            }
            block += statementIndent + "break;\n";
            block += caseIndent;
            block += '}\n';
        }

        struct.touchBlock = block;
    }
}

function createGetLastTokens(structs) {
    for (let i = 0; i < structs.length; i++) {
        const struct = structs[i];

        if (struct.fields.length === 0) {
            continue
        }

        var block = "";

        for(var j = 0; j < struct.validSyntaxKinds.length; j++) {
            block += caseIndent + "case SyntaxKind::" + struct.validSyntaxKinds[j] + ": {\n";
            block += statementIndent;
            block += struct.structName;
            block += `* p = (${struct.structName}*)syntaxBase;\n`

            for (var f = struct.fields.length - 1; f >= 0; f--) {
                const field = struct.fields[f];
                const x = `p->${field.fieldName}`;
                block += statementIndent;
                if (field.fieldType === "SyntaxToken") {
                    block += `if(${x}.IsValid()) return ${x};\n`;
                } else if (field.fieldType.startsWith("TokenList")) {
                    block += `if(${x} != nullptr && ${x}->size != 0) return ${x}->array[${x}->size - 1];\n`;
                } else if (field.fieldType.startsWith("SyntaxList")) {
                    block += `if(${x} != nullptr && ${x}->size != 0) return GetLastToken(${x}->array[${x}->size - 1]);\n`;
                } else if (field.fieldType.startsWith("SeparatedSyntaxList")) {
                    // we want the last of separator or item, we don't know which is last
                    block += `if(${x} != nullptr && ${x}->itemCount != 0) {\n`
                    block += statementIndent + "    ";
                    block += `SyntaxToken a = GetLastToken(${x}->items[${x}->itemCount - 1]);\n`
                    block += statementIndent + "    ";
                    block += `SyntaxToken b = ${x}->separatorCount == 0 ? SyntaxToken() : ${x}->separators[${x}->separatorCount - 1];\n`
                    block += statementIndent + "    ";
                    block += `return a.GetId() > b.GetId() ? a : b;\n`;
                    block += statementIndent;
                    block += "}\n";
                } else {
                    block += `if(${x} != nullptr) return GetLastToken((SyntaxBase*)${x});\n`;
                }
            }

            block += statementIndent;
            block += "return SyntaxToken();\n";
            block += caseIndent;
            block += '}\n';
        }

        struct.lastTokenBlock = block;
    }
}

function createGetFirstTokens(structs) {
    for (let i = 0; i < structs.length; i++) {
        const struct = structs[i];

        if (struct.fields.length === 0) {
            console.log(struct.structName + " has no fields");
            continue
        }

        var block = "";
        for(var j = 0; j < struct.validSyntaxKinds.length; j++) {

            block += caseIndent + "case SyntaxKind::" + struct.validSyntaxKinds[j] + ": {\n";
            block += statementIndent;
            block += struct.structName;
            block += `* p = (${struct.structName}*)syntaxBase;\n`

            for (var f = 0; f < struct.fields.length; f++) {
                const field = struct.fields[f];
                const x = `p->${field.fieldName}`;
                block += statementIndent;
                if (field.fieldType === "SyntaxToken") {
                    block += `if(${x}.IsValid()) return ${x};\n`;
                } else if (field.fieldType.startsWith("TokenList")) {
                    block += `if(${x} != nullptr && ${x}->size != 0) return ${x}->array[0];\n`;
                } else if (field.fieldType.startsWith("SyntaxList")) {
                    block += `if(${x} != nullptr && ${x}->size != 0) return GetFirstToken(${x}->array[0]);\n`;
                } else if (field.fieldType.startsWith("SeparatedSyntaxList")) {
                    block += `if(${x} != nullptr && ${x}->itemCount != 0) return GetFirstToken(${x}->items[0]);\n`;
                } else {
                    block += `if(${x} != nullptr) return GetFirstToken((SyntaxBase*)${x});\n`;
                }
            }

            block += statementIndent;
            block += "return SyntaxToken();\n";
            block += caseIndent;
            block += '}\n';
            struct.firstTokenBlock = block;
        }

    }
}

const nodePrinterTemplate = `#include "../Src/Parsing3/NodePrinter.h"

namespace Alchemy::Compilation {

    void NodePrinter::PrintNode(SyntaxBase* syntaxBase) {

        if (syntaxBase == nullptr) {
            PrintLine("nullptr");
            return;
        }

        switch (syntaxBase->GetKind()) {
__REPLACE__
            default: {
                break;
            }
            
        }
    }
}
`
const firstTokenTemplate = `#include "../Src/Parsing3/SyntaxBase.h"
#include "../Src/Parsing3/SyntaxNodes.h"

namespace Alchemy::Compilation {
    
    SyntaxToken GetFirstToken(SyntaxBase * syntaxBase) {
    
        if(syntaxBase == nullptr) {
            return SyntaxToken();
        }
        
        switch(syntaxBase->GetKind()) {
__REPLACE_GET_FIRST__
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
__REPLACE_GET_LAST__
            default: {
                return SyntaxToken();
            }
            
        }        
        
    }
    
}
`;

const builderTemplate = `#include "./Builders.h"

namespace Alchemy::Compilation {

__REPLACE__
    struct Builder : BuilderBase {
    
        explicit Builder(LinearAllocator * allocator) : BuilderBase((allocator)) {}
__REPLACE_BUILDER_API__        
        
    };

}
`;

const touchTemplate = `#include "../Src/Parsing3/FindSkippedTokens.h"

namespace Alchemy::Compilation {
    
    void FindSkippedTokens::TouchNode(SyntaxBase * syntaxBase) {

        if(syntaxBase == nullptr) {
            return;
        }
        
        switch(syntaxBase->GetKind()) {
__REPLACE__
            default: {
                UNREACHABLE("TouchNode");
                return;
            }
            
        }        
    }
    
}
`;

const compareTemplate = `#include "./NodeEquality.h"

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
__REPLACE__
            default: {
                UNREACHABLE("NodesEqual");
                return true;
            }
            
        }        
        
    }
    
}`;

function makeFirstTokenSource() {
    createGetFirstTokens(structs);
    createGetLastTokens(structs);
    return firstTokenTemplate
            .replace("__REPLACE_GET_FIRST__", structs.map(s => s.firstTokenBlock).join('\n'))
            .replace("__REPLACE_GET_LAST__", structs.map(s => s.lastTokenBlock).join('\n'));
}

function makeNodePrinter() {
    createPrintBlocks(structs);
    return nodePrinterTemplate.replace("__REPLACE__", structs.map(s => s.printBlock).join('\n'));
}

function makeBuilders() {
    createBuilders(structs);
    return builderTemplate
        .replace("__REPLACE__", structs.map(s => s.builder).join('\n'))
        .replace("__REPLACE_BUILDER_API__", structs.map(s => s.builderGen).join('\n')
        );
}

function makeEqualityComparisons() {
    createCompareBlocks(structs);
    return compareTemplate.replace("__REPLACE__", structs.map(s => s.compBlock).join('\n'));
}
function makeTouches() {
    findSkippedTokens(structs);
    return touchTemplate.replace("__REPLACE__", structs.map(s => s.touchBlock).join('\n'));
}

module.exports = {
    makeTouches,
    makeBuilders,
    makeEqualityComparisons,
    makeFirstTokenSource,
    makeNodePrinter
}