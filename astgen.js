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

        if(lastWasAbstract) {
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
        i++;
        for (; i < lines.length; i++) {

            var candidate = lines[i];

            if (candidate.startsWith("//")) continue;
            if (candidate.length === 0) continue;

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

    }

    return output;

}

const filename = './Src/Parsing2/SyntaxNodes.h';

const structs = findCppStructs(filename);

const caseIndent = "            ";
const statementIndent = caseIndent + "    ";

function stripSuffix(str, suffix) {
    if (str.endsWith(suffix)) {
        return str.slice(0, -suffix.length);
    }
    return str;
}

for (var i = 0; i < structs.length; i++) {
    const struct = structs[i];

    struct.printBlock = caseIndent + "case SyntaxKind::" + stripSuffix(struct.structName, "Syntax") + ": {\n";
    struct.printBlock += statementIndent;
    struct.printBlock += struct.structName;
    struct.printBlock += `* p = (${struct.structName}*)syntaxBase;\n`
    struct.printBlock += statementIndent;
    struct.printBlock += `PrintLine("${struct.structName}");\n`
    struct.printBlock += statementIndent;
    struct.printBlock += "indent++;\n";
    for (var f = 0; f < struct.fields.length; f++) {
        const field = struct.fields[f];
        struct.printBlock += statementIndent;
        if(field.fieldType === "SyntaxToken") {
            struct.printBlock += `PrintToken(p->${field.fieldName});\n`;
        }
        else if(field.fieldType.startsWith("SyntaxList")) {
            struct.printBlock += `PrintSyntaxList((SyntaxListUntyped*)p->${field.fieldName});\n`;
        }
        else if(field.fieldType.startsWith("SeparatedSyntaxList")) {
            struct.printBlock += `PrintSeparatedSyntaxList((SeparatedSyntaxListUntyped*)p->${field.fieldName});\n`;
        }
        else {
            struct.printBlock += `PrintNode(p->${field.fieldName});\n`;
        }

    }
    struct.printBlock += statementIndent;
    struct.printBlock += "indent--;\n";
    struct.printBlock += statementIndent;
    struct.printBlock += "break;\n";
    struct.printBlock += caseIndent;
    struct.printBlock += '}\n';
    console.log(struct.printBlock);
}

// console.log(structs);