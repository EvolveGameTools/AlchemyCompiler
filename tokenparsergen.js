const fs = require('fs');

const keyword_match = `
bool Matches2(const char* keyword, char* test) {
    return *(uint16 * )
    keyword == *(uint16 * )
    test;
}

bool Matches3(const char* keyword, char* test) {
    return *(uint16 * )
    keyword == *(uint16 * )
    test && keyword[2] == test[2];
}

bool Matches4(const char* keyword, char* test) {
    return *(uint32*) keyword == *(uint32*) test;
}

bool Matches5(const char* keyword, char* test) {
    return Matches4(keyword, test) && keyword[4] == test[4];
}

bool Matches6(const char* keyword, char* test) {
    return Matches4(keyword, test) && Matches2(keyword + 4, test + 4);
}

bool Matches7(const char* keyword, char* test) {
    return Matches4(keyword, test) && Matches3(keyword + 4, test + 4);
}

bool Matches8(const char* keyword, char* test) {
    return *(uint64*) keyword == *(uint64*) test;
}

bool Matches9(const char* keyword, char* test) {
    return Matches8(keyword, test) && keyword[8] == test[8];
}

bool Matches10(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches2(keyword + 8, test + 8);
}

bool Matches11(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches3(keyword + 8, test + 8);
}

bool Matches12(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches4(keyword + 8, test + 8);
}

bool Matches13(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches4(keyword + 8, test + 8) && keyword[12] == test[12];
}

bool Matches14(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches4(keyword + 8, test + 8) && Matches2(keyword + 12, test + 12);
}

bool Matches15(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches4(keyword + 8, test + 8) && Matches3(keyword + 12, test + 12);
}

bool Matches16(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches8(keyword + 8, test + 8);
}

bool MatchesGT16(const char* keyword, char* test, int32 length) {

    if (!Matches16(keyword, test)) {
        return false;
    }

    keyword += 16;
    test += 16;
    length -= 16;

    for (int32 i = 0; i < length; i++) {
        if (keyword[i] != test[i]) {
            return false;
        }
    }

    return true;

}
`;

function extractEnumValues(content) {
    const match = content.match(/enum class Keyword\s*:\s*uint8\s*{([\s\S]*?)}/);
    if (!match) {
        throw new Error("enum class Keyword : uint8 not found");
    }

    let enumContent = match[1];

    // Remove single-line comments
    let noSingleLineComments = enumContent.replace(/\/\/.*$/gm, '');

    // Remove multi-line comments
    let cleanedEnumContent = noSingleLineComments.replace(/\/\*[\s\S]*?\*\//gm, '');

    return cleanedEnumContent.replace("NotAKeyword", "")
        .split(',')
        .map(e => e.trim())
        .filter(e => e && e !== ';');
}

function groupByLength(arr) {
    const groups = {};

    arr.forEach(str => {
        const length = str.length;
        if (!groups[length]) {
            groups[length] = {length: length, values: []};
        }
        groups[length].values.push(str);
    });

    // Convert the groups object into an array
    return Object.values(groups);
}

function generateKeywordMatch(cppEnumContent, start, end) {
    const enumValues = getEnumNames(cppEnumContent, "SyntaxKind");

    const groupedValues = {};

    let longestKeywordLength = 0;
    let shortestKeywordLength = 100;

    var startIdx = enumValues.indexOf(start);
    var endIdx = enumValues.indexOf(end);

    if(startIdx === -1 || endIdx === -1) {
        throw new Error("didnt find enum boundaries");
    }

    for(let i = startIdx; i < endIdx; i++) {
        var value = enumValues[i];

        if(value.startsWith("__")) {
            continue
        }

        var lowered = value.toLowerCase();
        var char1Code = lowered.charCodeAt(0);
        var char2Code = lowered.charCodeAt(1);
        var val = ((char2Code) << 8) | (char1Code);

        if (!groupedValues[val]) {
            groupedValues[val] = [];
        }

        groupedValues[val].push(value);

        const l = value.length - "Keyword".length;
        if (l > longestKeywordLength) {
            longestKeywordLength = l;
        }

        if (l < shortestKeywordLength) {
            shortestKeywordLength = l;
        }

    }
    let functionBody = keyword_match;

    functionBody += `constexpr int32 kLongestKeywordLength = ${longestKeywordLength};\n\n`;

    functionBody += `constexpr int32 kShortestKeywordLength = ${shortestKeywordLength};\n\n`;
    functionBody += 'bool TryMatchKeyword_Generated(char * buffer, int32 length, SyntaxKind * keywordType) {\n';
    functionBody += '\n    if(length < kShortestKeywordLength || length > kLongestKeywordLength) {\n' +
        '        return false;\n' +
        '    }\n\n'

    functionBody += '    // we generated a trie like structure in an offline step. this encodes the first 2 chars into a uint\n'
    functionBody += '    // and then matches on the rest of the string after switching over the resulting string length\n'
    functionBody += '    char * truncatedBuffer = buffer + 2;\n';
    functionBody += '    switch(*(uint16*)buffer) {\n';

    for (const [char, values] of Object.entries(groupedValues)) {

        var grouped = groupByLength(values);

        functionBody += `        case ${char}:\n`;
        if(grouped.length > 1) {
            functionBody += `            switch(length) {\n`
        }

        grouped.forEach((g) => {
            if(grouped.length > 1) {
                functionBody += `                case ${g.length - "Keyword".length}: {\n`;
            }

            g.values.forEach((value) => {
                var keyword = value;
                if(value.endsWith("Keyword")) {
                    value = value.substring(0, value.length - "Keyword".length);
                }
                value = value.toLowerCase();
                var strLen = keyword.length - "Keyword".length;
                var abridged = strLen - 2;
                var abridgedStr = `"${value.slice(2)}"`;

                var matchMethod = "";
                if (abridged === 0) {
                    matchMethod = "";
                } else if (abridged === 1) {
                    matchMethod = `truncatedBuffer[0] == '${abridgedStr.charAt(1)}'`;
                } else if (abridged >= 2 && abridged <= 16) {
                    matchMethod = `Matches${abridged}(${abridgedStr}, truncatedBuffer)`;
                } else {
                    matchMethod += `MatchesGT16(${abridgedStr}, truncatedBuffer, ${abridged})`;
                }

                if (abridged === 0) {
                    functionBody += `                    *keywordType = SyntaxKind::${keyword};\n`;
                    functionBody += `                    return true;`;
                } else {
                    functionBody += `                    if(${matchMethod}) { // ${keyword}\n`;
                    functionBody += `                        *keywordType = SyntaxKind::${keyword};\n`;
                    functionBody += `                        return true;\n`
                    functionBody += `                    }\n`
                }

            });
            if (g.length !== 2) {
               //if(grouped.length > 1) {
                    functionBody += `                    return false;\n`
                //}
                //else {
                //    functionBody += '\n';
               // }
            }
            if(grouped.length > 1) {
                functionBody += `                }\n`
            }
        });

        if(grouped.length > 1) {
            functionBody += "                default: return false;\n"
            functionBody += `            }\n`
        }

    }

    functionBody += '    }\n    return false;\n}\n';

    return functionBody;

}

function generateMatcherFunction(cppEnumContent) {
    const enumValues = extractEnumValues(cppEnumContent);

    const groupedValues = {};

    let longestKeywordLength = 0;
    let shortestKeywordLength = 100;

    for (const value of enumValues) {

        var lowered = value.toLowerCase();

        var char1Code = lowered.charCodeAt(0);
        var char2Code = lowered.charCodeAt(1);
        var val = ((char2Code) << 8) | (char1Code);

        if (!groupedValues[val]) {
            groupedValues[val] = [];
        }

        groupedValues[val].push(value);

        if (value.length > longestKeywordLength) {
            longestKeywordLength = value.length;
        }
        if (value.length < shortestKeywordLength) {
            shortestKeywordLength = value.length;
        }

    }

    let functionBody = keyword_match;

    functionBody += `constexpr int32 kLongestKeywordLength = ${longestKeywordLength};\n\n`;

    functionBody += `constexpr int32 kShortestKeywordLength = ${shortestKeywordLength};\n\n`;
    functionBody += 'bool TryMatchKeyword_Generated(char * buffer, int32 length, Keyword * keywordType) {\n';
    functionBody += '\n    if(length < kShortestKeywordLength || length > kLongestKeywordLength) {\n' +
        '        return false;\n' +
        '    }\n\n'

    functionBody += '    // we generated a trie like structure in an offline step. this encodes the first 2 chars into a uint\n'
    functionBody += '    // and then matches on the rest of the string after switching over the resulting string length\n'
    functionBody += '    char * truncatedBuffer = buffer + 2;\n';
    functionBody += '    switch(*(uint16*)buffer) {\n';

    for (const [char, values] of Object.entries(groupedValues)) {

        var grouped = groupByLength(values);

        functionBody += `        case ${char}:\n`;
        functionBody += `            switch(length) {\n`


        grouped.forEach((g) => {

            functionBody += `                case ${g.length}: {\n`;

            g.values.forEach((value) => {
                var keyword = value;
                value = value.toLowerCase();
                var strLen = value.length;
                var abridged = strLen - 2;
                var abridgedStr = `"${value.slice(2)}"`;

                var matchMethod = "";
                if (abridged === 0) {
                    matchMethod = "";
                } else if (abridged === 1) {
                    matchMethod = `truncatedBuffer[0] == '${abridgedStr.charAt(1)}'`;
                } else if (abridged >= 2 && abridged <= 16) {
                    matchMethod = `Matches${abridged}(${abridgedStr}, truncatedBuffer)`;
                } else {
                    matchMethod += `MatchesGT16(${abridgedStr}, truncatedBuffer, ${abridged})`;
                }

                if (abridged === 0) {
                    functionBody += `                    *keywordType = Keyword::${keyword};\n`;
                    functionBody += `                    return true;`;
                } else {
                    functionBody += `                    if(${matchMethod}) { // ${keyword}\n`;
                    functionBody += `                        *keywordType = Keyword::${keyword};\n`;
                    functionBody += `                        return true;\n`
                    functionBody += `                    }\n`
                }

            });
            if (g.length !== 2) {
                functionBody += `                    return false;\n`
            }
            functionBody += `                }\n`
        });

        functionBody += "                default: return false;\n"
        functionBody += `            }\n`

    }

    functionBody += '    }\n    return false;\n}\n';

    return functionBody;
}

function generateKeywordToString(inputFile, outputFile) {
    fs.readFile(inputFile, 'utf8', (err, data) => {
        if (err) {
            console.error("Error reading the file:", err);
            return;
        }

        const keywords = extractEnumValues(data);
        var output = '#pragma once\n#include "../Keyword.h"\n\n';

        output += "namespace Alchemy::Parsing {\n";
        output += "    const char * KeywordToString(Keyword keyword) {\n";
        output += "        switch(keyword) {\n";
        for (var i = 0; i < keywords.length; i++) {
            output += "            case Keyword::" + keywords[i] + ": return \"" + keywords[i].toLowerCase() + "\";\n";
        }
        output += "            default: return \"Invalid\";\n";
        output += "        }\n";
        output += "    }\n";
        output += "}";

        fs.writeFile(outputFile, output, (err) => {
            if (err) {
                console.error("Error writing the file:", err);
            } else {
                console.log(`Generated function has been written to ${outputFile}`);
            }
        });
    });
}

function generateSyntaxKindKeywordMatchingCode(inputFile, outputFile, start, end) {
    fs.readFile(inputFile, 'utf8', (err, data) => {
        if (err) {
            console.error("Error reading the file:", err);
            return;
        }

        const generatedFunction = generateKeywordMatch(data, start, end);

        var output = '#include "../PrimitiveTypes.h"\n#include "./SyntaxKind.h"\n\n' + 'namespace Alchemy::Compilation {\n' + generatedFunction + '\n}';

        fs.writeFile(outputFile, output, (err) => {
            if (err) {
                console.error("Error writing the file:", err);
            } else {
                console.log(`Generated function has been written to ${outputFile}`);
            }
        });
    });
}

function generateKeywordMatchingCode(inputFile, outputFile) {
    fs.readFile(inputFile, 'utf8', (err, data) => {
        if (err) {
            console.error("Error reading the file:", err);
            return;
        }

        const generatedFunction = generateMatcherFunction(data);

        var output = '#pragma once\n#include "../../PrimitiveTypes.h"\n#include "../Keyword.h"\n\n' + 'namespace Alchemy::Parsing {\n' + generatedFunction + '\n}';

        fs.writeFile(outputFile, output, (err) => {
            if (err) {
                console.error("Error writing the file:", err);
            } else {
                console.log(`Generated function has been written to ${outputFile}`);
            }
        });
    });
}

function getEnumNames(content, enumName, valuesToIgnore = []) {
    const enumRegex = new RegExp(`enum\\s+class\\s+${enumName}\\s*(?::\\s*([^\\s{]+))?\\s*{([\\s\\S]*?)}`);

    const lines = content.split("\n").map(s => s.trim());
    const enumString = "enum class " + enumName;
    for (var i = 0; i < lines.length; i++) {
        if (lines[i].startsWith(enumString)) {
            break;
        }
    }

    if (i === lines.length) {
        throw new Error(`${enumName} enum not found`);
    }

    for (; i < lines.length; i++) {
        if (lines[i].indexOf("{")) {
            break;
        }
    }

    var startLine = i + 1;
    var endLine = startLine;
    for (; i < lines.length; i++) {
        if (lines[i].startsWith("}")) {
            endLine = i;
            break;
        }
    }

    if (startLine === endLine) {
        console.log("bad");
    }

    var names = [];
    for (var j = startLine; j < endLine; j++) {
        var line = lines[j];

        if (line.startsWith("//")) {
            continue
        }

        var comment = line.indexOf("//");
        if (comment > 0) {
            line = line.substring(0, comment);
        }
        var eq = line.indexOf("=");
        if (eq > 0) {
            line = line.substring(0, eq);
        }

        var comma = line.indexOf(",");
        if (comma > 0) {
            line = line.substring(0, comma);
        }

        if (line.trim().length === 0) {
            continue
        }

        if (valuesToIgnore.indexOf(line) === -1) {
            names.push(line);
        }


    }

    // Remove values specified in valuesToIgnore array
    valuesToIgnore.forEach(value => {
        names = names.filter(e => e !== `${enumName}::${value}`);
    });

    return names;
}

function generateIsDeclarationNode(src) {
    fs.readFile(src, 'utf8', (err, fileContent) => {
        if (err) {
            console.error("Error reading the file:", err);
            return;
        }

        const enumNames = getEnumNames(fileContent, "NodeType", []);

        enumNames.forEach((name) => {

            if(name.endsWith("Declaration")) {
                // console.log(name);
            }

        });

    });
}

function enumToStringFn(src, dst, enumName, valuesToIgnore = []) {
    fs.readFile(src, 'utf8', (err, fileContent) => {
        if (err) {
            console.error("Error reading the file:", err);
            return;
        }

        const enumNames = getEnumNames(fileContent, enumName, valuesToIgnore);

        fs.readFile(dst, 'utf8', (err, dstContent) => {

            var output = "    const char* " + enumName + "ToString(" + enumName + " e) {\n";
            output += "        switch(e) {\n";
            for (let i = 0; i < enumNames.length; i++) {
                output += "            case " + enumName + "::" + enumNames[i] + ": return \"" + enumNames[i] + "\";\n";
            }
            output += "            default: return \"\";\n";
            output += "        }\n";
            output += "    }\n";

            const startMarker = `// --- Generate ${enumName}ToString Start`;
            const endMarker = `// --- Generate ${enumName}ToString End`;

            var startIdx = dstContent.indexOf(startMarker);
            var endIdx = dstContent.indexOf(endMarker);

            if (startIdx < 0 || endIdx < 0) {
                console.log(`nowhere to output ${enumName}ToString function, missing marker comments`);
                return;
            }

            // Extract the content before and after the markers
            const beforeContent = dstContent.substring(0, startIdx);
            const afterContent = dstContent.substring(endIdx + endMarker.length);

            // Concatenate the new content with the content before and after the markers
            output = beforeContent + startMarker + '\n' + output + '\n' + endMarker + afterContent;

            fs.writeFile(dst, output, (err) => {
                if (err) {
                    console.error("Error writing the file:", err);
                } else {
                    console.log(`Generated function has been written to ${src}`);
                }
            });

        });
    });
}

function generateNodeType(src, dst) {
    fs.readFile(src, 'utf8', (err, fileContent) => {
        if (err) {
            console.error("Error reading the file:", err);
            return;
        }

        // Regular expression to match struct declarations
        const structRegex = /struct\s+(\w+)/;
        const lines = fileContent.split('\n');

        let structNames = [];
        for (const line of lines) {
            const match = line.match(structRegex);
            if (match !== null) {
                // ignore forward declarations
                // if (line.indexOf("NodeBase") === -1 && line.indexOf(';') === -1) {

                    // if(line.indexOf(": StatementNode") === -1) {
                    //     console.error(match + " didn't extend NodeBase");
                    // }

                // }
                const structName = match[1];
                if (structName !== "NodeBase" && !structName.endsWith("Node")) {
                    console.log(structName, "should end with 'Node'");
                }
                // don't push forward declarations
                if(line.indexOf(';') === -1) {
                    structNames.push(structName);
                }
            }
        }

        var output = `#pragma once
#include <algorithm> // for std::max

namespace {

    template<typename... Ts>
    constexpr size_t MaxStructSize() {
        return (std::max)({sizeof(Ts)...});
    }

    constexpr size_t kMaxNodeSize = MaxStructSize<
$REPLACE    
    >() - sizeof(Alchemy::Parsing::NodeBase);
    
$REPLACE_SIZE_ASSERTS

}
    
namespace Alchemy::Parsing {

    static_assert(sizeof(AbstractPsiNode) == kMaxNodeSize + sizeof(NodeBase));
    static_assert(sizeof(AbstractPsiNode) == 32);

}
`;
        var structOutput = "";
        for (var i = 0; i < structNames.length; i++) {
            structOutput += '        Alchemy::Parsing::' + structNames[i];
            if (i !== structNames.length - 1) {
                structOutput += ',\n'
            }
        }
        output = output.replace('$REPLACE', structOutput);

        var assertOutput = "";
        for( i = 0; i< structNames.length; i++) {
            assertOutput += '        static_assert(sizeof(Alchemy::Parsing::' + structNames[i] + ") <= 32);\n";
        }
        output = output.replace('$REPLACE_SIZE_ASSERTS', assertOutput);

        fs.writeFile(dst, output, (err) => {
            if (err) {
                console.error("Error writing the file:", err);
            } else {
                console.log(`Generated function has been written to ${dst}`);
            }
        });
    });
}

generateKeywordMatchingCode('Src/Parsing/Keyword.h', 'Src/Parsing/private/MatchKeyword.generated.inc');
generateSyntaxKindKeywordMatchingCode('Src/Parsing2/SyntaxKind.h', 'Src/Parsing2/MatchKeyword.generated.cpp', '__FirstKeyword__', '__LastContextualKeyword__');

generateKeywordToString('Src/Parsing/Keyword.h', 'Src/Parsing/impl/Keyword.cpp');

generateNodeType('Src/Parsing/Nodes.h', 'Src/Parsing/private/Nodes.generated.h')

generateIsDeclarationNode("Src/Parsing/NodeType.h");

enumToStringFn("Src/Parsing/NodeType.h", "Src/Parsing/impl/NodeType.cpp", "NodeType", ["None"]);
enumToStringFn("Src/Parsing/TokenType.h", "Src/Parsing/impl/TokenType.cpp", "TokenType", ["Invalid"]);
