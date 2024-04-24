#pragma once

#include "./AstNode.h"
#include "../Collections/CheckedArray.h"
#include "../Collections/PodList.h"
#include "./CstNode.h"
#include "./AstNode.h"
#include "./ParseResult.h"

namespace Alchemy::Parsing {

    using namespace Alchemy;

    struct SyntaxTreeBuilder {

    private:

        struct NodePointer {
            union {
                AstNode* astNode {};
                CstNode* cstNode;
            };

            explicit NodePointer(AstNode* astNode) : astNode(astNode) {}

            explicit NodePointer(CstNode* cstNode) : cstNode(cstNode) {}

        };

        PodList<NodePointer> nodeStack;
        AllocatedArray<AstNode> astNodes;
        PodList<CstNode> cstNodes;
        ParseResult* parseResult;

    public:

        void Dispose() {
            TypedUnsafe::FreeArray(astNodes);
        }

        explicit SyntaxTreeBuilder(ParseResult* parseResult)
            : parseResult(parseResult) {}

        // This creates an ast without any of the trivial tokens, just nodes that were marked
        // we really only need this for building a ConcreteSyntaxTree for editor operations
        // compilation shouldn't need to touch this, it will use the NodeTree directly
        AstNode* BuildAst() {

            if (parseResult->production.size == 0) {
                return nullptr;
            }

            if (astNodes.size != 0) {
                return astNodes.GetPointer(1);
            }

            TypedUnsafe::ResizeArray(&astNodes, parseResult->psiNodes.size);
            TypedUnsafe::MemClear(astNodes.ToCheckedArray());

            nodeStack.size = 0;

            // first real node of the production starts at 1
            AstNode* rootAstNode = &astNodes[1];
            AstNode* curNode = rootAstNode;
            curNode->psiIndex = 1;
            nodeStack.Push(NodePointer(curNode));
            for (int32 i = 1; i < parseResult->production.size; i++) {
                int32 id = parseResult->production[i];

                if (id < 0) {

                    assert(id < 0 && astNodes.GetPointer(-id) == curNode && "markers are unbalanced"); // maybe an error code instead

                    curNode = nodeStack.Pop().astNode;
                    continue;

                }

                AstNode* item = astNodes.GetPointer(id);

                item->psiIndex = id;
                item->parentIndex = curNode->psiIndex;
                item->depth = nodeStack.size;
                AddAstChild(curNode, item);
                nodeStack.Push(NodePointer(curNode));
                curNode = item;

            }

            assert(nodeStack.size == 0 && "stack size should be 0");
            assert(curNode == rootAstNode);

            return rootAstNode;

        }

        int32 ErrorsToString(char* output) {
            int32 outputSize = 0;
            const char* baseStr = "%.*s (%d:%d)\n";
            for (int32 i = 0; i < parseResult->errors.size; i++) {
                ParseError& error = parseResult->errors.Get(i);
                LineColumn lc = parseResult->GetLocationFromByteOffset(error.sourceStart);
                if (output == nullptr) {
                    outputSize += snprintf(nullptr, 0, baseStr, error.messageLength, error.message, lc.line, lc.column);
                }
                else {
                    outputSize += sprintf(output + outputSize, baseStr, error.messageLength, error.message, lc.line, lc.column);
                }

            }
            return outputSize;
        }

        char* ErrorsToStringMalloc() {
            int32 size = ErrorsToString(nullptr);
            char * buffer = (char*) MallocByteArray((size + 1) * sizeof(char), alignof(char));
            buffer[size] = '\0';
            ErrorsToString(buffer);
            return buffer;
        }

        char* AbstractSyntaxTreeToStringMalloc() {
            int32 size = PrintAbstractSyntaxTree(nullptr);
            char* buffer = (char*) MallocByteArray((size + 1) * sizeof(char), alignof(char));
            buffer[size] = '\0';
            PrintAbstractSyntaxTree(buffer);
            return buffer;
        }

        char* ConcreteSyntaxTreeToStringMalloc(bool skipWhitespace = false, bool skipComments = false) {
            int32 size = CstToString(nullptr, skipWhitespace, skipComments);
            char* buffer = (char*) MallocByteArray((size + 1) * sizeof(char), alignof(char));
            buffer[size] = '\0';
            CstToString(buffer, skipWhitespace, skipComments);
            return buffer;
        }

        int32 CstToString(char* output, bool skipWhitespace, bool skipComments) {

            if (cstNodes.size == 0) {
                CreateConcreteSyntaxTree();
            }

            int32 currentIndexOutput = 0;

            const char* baseStr = "%*s%s(%d, %d)\n";

            nodeStack.Add(NodePointer(cstNodes.GetPointer(astNodes[1].cstIndex)));

            while (nodeStack.size > 0) {

                CstNode* cst = nodeStack.Pop().cstNode;

                int32 indent = cst->depth * 2;

                if (cst->IsPsiNode()) {
                    NodeBase psi = parseResult->psiNodes.GetPointer(cst->GetPsiIndex())->nodeBase;

                    if (output == nullptr) {
                        currentIndexOutput += snprintf(nullptr, 0, baseStr, indent, "", NodeTypeToString(psi.nodeType), psi.tokenStart, psi.tokenEnd);
                    }
                    else {
                        currentIndexOutput += sprintf(output + currentIndexOutput, baseStr, indent, "", NodeTypeToString(psi.nodeType), psi.tokenStart, psi.tokenEnd);
                    }

                    int32 childIndex = cst->firstChildIndex.index;
                    int32 childCount = 0;

                    while (childIndex != 0) {
                        childCount++;
                        childIndex = cstNodes.GetPointer(childIndex)->nextSiblingIndex.index;
                    }

                    NodePointer* pReserved = nodeStack.Reserve(childCount);
                    CheckedArray<CstNode*> reserved(&pReserved->cstNode, childCount); // just for bounds checking

                    childIndex = cst->firstChildIndex.index;
                    int32 i = 1;
                    while (childIndex != 0) {
                        reserved[childCount - i] = cstNodes.GetPointer(childIndex);
                        childIndex = cstNodes.GetPointer(childIndex)->nextSiblingIndex.index;
                        i++;
                    }

                }
                else {
                    // leaf, just print it
                    Token token = parseResult->tokens[cst->GetTokenIndex()];
                    const char* baseLeafString = "%*s%s(%.*s)\n";

                    if (skipWhitespace && token.tokenType == TokenType::Whitespace) {
                        continue;
                    }

                    if (skipComments && token.tokenType == TokenType::Comment) {
                        continue;
                    }

                    char* src = parseResult->src + token.position;
                    int32 length = token.tokenType == TokenType::Whitespace || token.tokenType == TokenType::Comment || cst->GetTokenIndex() == parseResult->tokens.size - 1
                                   ? 0
                                   : parseResult->tokens[cst->GetTokenIndex() + 1].position - token.position;

                    if (output == nullptr) {
                        currentIndexOutput += snprintf(nullptr, 0, baseLeafString, indent, "", TokenTypeToString(token.tokenType), length, src);
                    }
                    else {
                        currentIndexOutput += sprintf(output + currentIndexOutput, baseLeafString, indent, "", TokenTypeToString(token.tokenType), length, src);
                    }
                }

            }

            return currentIndexOutput;
        }

        AstNode* GetRootAst() {
            return BuildAst();
        }

        int32 PrintAbstractSyntaxTree(char* output) {

            nodeStack.Push(NodePointer(GetRootAst()));
            int32 currentIndexOutput = 0;

            while (nodeStack.size != 0) {

                AstNode* currentNode = nodeStack.Pop().astNode;

                NodeBase psi = parseResult->psiNodes.GetPointer(currentNode->psiIndex)->nodeBase;

                int32 indent = currentNode->depth * 2;

                const char* baseStr = "%*s%s(%d, %d)\n";

                if (output == nullptr) {
                    currentIndexOutput += snprintf(nullptr, 0, baseStr, indent, "", NodeTypeToString(psi.nodeType), psi.tokenStart, psi.tokenEnd);
                }
                else {
                    currentIndexOutput += sprintf(output + currentIndexOutput, baseStr, indent, "", NodeTypeToString(psi.nodeType), psi.tokenStart, psi.tokenEnd);
                }

                int32 childIndex = currentNode->firstChild;
                int32 childCount = 0;

                while (childIndex != 0) {
                    childCount++;
                    childIndex = astNodes.GetPointer(childIndex)->nextIndex;
                }

                NodePointer* pReserved = nodeStack.Reserve(childCount);
                CheckedArray<AstNode*> reserved(&pReserved->astNode, childCount); // just for bounds checking

                childIndex = currentNode->firstChild;
                int32 i = 1;
                while (childIndex != 0) {
                    reserved[childCount - i] = astNodes.GetPointer(childIndex);
                    childIndex = astNodes.GetPointer(childIndex)->nextIndex;
                    i++;
                }

            }

            return currentIndexOutput;

        }

        // this includes trivial tokens like comments, commas, semicolons, braces, etc
        void CreateConcreteSyntaxTree() {

            BuildAst();

            cstNodes.size = 0;
            // burn index 0 to represent invalid
            memset(cstNodes.Reserve(), 0, sizeof(CstNode));

            {
                // we don't want to touch this pointer in case we re-allocate, so place in its own scope
                CstNode* cstNode = cstNodes.Reserve();
                cstNode->psiIndexOrTokenIndex = GetRootAst()->psiIndex;
                cstNode->parentIndex = CstNodeIndex(0);
                cstNode->firstChildIndex = CstNodeIndex(0);
                cstNode->lastChildIndex = CstNodeIndex(0);
                cstNode->nextSiblingIndex = CstNodeIndex(0);
                cstNode->prevSiblingIndex = CstNodeIndex(0);
                cstNode->depth = 0;

                GetRootAst()->cstIndex = cstNodes.size - 1;

            }

            nodeStack.Add(NodePointer(GetRootAst()));

            while (nodeStack.size != 0) {

                AstNode* currentNode = nodeStack.Pop().astNode;

                CstNodeIndex currentCst = CstNodeIndex((uint16) currentNode->cstIndex);
                NodeBase nodeBase = parseResult->psiNodes[currentNode->psiIndex].nodeBase;

                int32 lastTokenWrite = nodeBase.tokenStart;

                int32 childIndex = currentNode->firstChild;

                while (childIndex != 0) {
                    // find tokens between last write point and child start, child will encompass the rest

                    AstNode* child = astNodes.GetPointer(childIndex);

                    AbstractPsiNode* psi = parseResult->psiNodes.GetPointer(child->psiIndex);

                    int32 target = psi->nodeBase.tokenStart;

                    // add leaves between last write point and current child start
                    for (int32 i = lastTokenWrite; i < target; i++) {

                        { // don't touch this pointer in case of reallocation
                            CstNode* cstNode = cstNodes.Reserve();
                            cstNode->psiIndexOrTokenIndex = -i;
                            cstNode->parentIndex = currentCst;
                            cstNode->firstChildIndex = CstNodeIndex(0);
                            cstNode->lastChildIndex = CstNodeIndex(0);
                            cstNode->nextSiblingIndex = CstNodeIndex(0);
                            cstNode->prevSiblingIndex = CstNodeIndex(0);
                            cstNode->depth = currentNode->depth + 1;
                        }

                        AddCstChild(currentCst, CstNodeIndex(cstNodes.size - 1));

                    }

                    // add the child psi but none of it's contents

                    // last write target is now end of the child

                    lastTokenWrite = psi->nodeBase.tokenEnd;

                    { // don't touch this pointer in case of reallocation
                        CstNode* cst = cstNodes.Reserve();
                        cst->psiIndexOrTokenIndex = child->psiIndex;
                        cst->parentIndex = currentCst;
                        cst->firstChildIndex = CstNodeIndex(0);
                        cst->lastChildIndex = CstNodeIndex(0);
                        cst->nextSiblingIndex = CstNodeIndex(0);
                        cst->prevSiblingIndex = CstNodeIndex(0);
                        cst->depth = currentNode->depth + 1;
                        child->cstIndex = AddCstChild(currentCst, CstNodeIndex(cstNodes.size - 1)).index;

                    }

                    childIndex = child->nextIndex;

                    nodeStack.Push(NodePointer(child));

                }

                // add any tokens between end of last child and end of this node

                for (int32 i = lastTokenWrite; i < nodeBase.tokenEnd; i++) {

                    { // don't touch this pointer in case of reallocation
                        CstNode* cstNode = cstNodes.Reserve();
                        cstNode->psiIndexOrTokenIndex = -i;
                        cstNode->parentIndex = currentCst;
                        cstNode->firstChildIndex = CstNodeIndex(0);
                        cstNode->lastChildIndex = CstNodeIndex(0);
                        cstNode->nextSiblingIndex = CstNodeIndex(0);
                        cstNode->prevSiblingIndex = CstNodeIndex(0);
                        cstNode->depth = currentNode->depth + 1;
                    }

                    AddCstChild(currentCst, CstNodeIndex(cstNodes.size - 1));

                }
            }

        }

    private:

        int32 ComputeAstNodeIndex(AstNode* current) {
            return (int32) (current - astNodes.array);
        }

        void AddAstChild(AstNode* parent, AstNode* child) {
            uint16 childIndex = (uint16) ComputeAstNodeIndex(child);
            if (parent->firstChild == 0) {
                parent->firstChild = childIndex;
            }
            else {
                astNodes.GetPointer(parent->lastChild)->nextIndex = childIndex;
            }
            parent->lastChild = childIndex;
        }

        CstNodeIndex AddCstChild(CstNodeIndex parent, CstNodeIndex childIndex) {
            CstNode* parentNode = GetCstNodeByIndex(parent);
            if (parentNode->firstChildIndex.index == 0) {
                parentNode->firstChildIndex = childIndex;
            }
            else {
                GetCstNodeByIndex(parentNode->lastChildIndex)->nextSiblingIndex = childIndex;
                GetCstNodeByIndex(childIndex)->prevSiblingIndex = parentNode->lastChildIndex;
            }
            parentNode->lastChildIndex = childIndex;
            return childIndex;
        }

        CstNode* GetCstNodeByIndex(CstNodeIndex index) {
            return cstNodes.GetPointer(index.index);
        }
    };

}