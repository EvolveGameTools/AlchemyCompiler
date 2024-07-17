#pragma once

#include <stdarg.h>

#include "../../Collections/CheckedArray.h"
#include "../../Collections/PodList.h"
#include "../../Util/FixedCharSpan.h"
#include "../../Allocation/LinearAllocator.h"

#include "../Token.h"
#include "../AstNode.h"
#include "../ParseResult.h"

#include "./Nodes.generated.h"
#include "./AlchemyTokenizer.h"

namespace Alchemy::Parsing {

    struct Marker {

        int32 id {}; // will also serve as our typed node lookup index

        explicit Marker(int32 index) : id(index) {}

    };

    struct NonTrivialToken : Token {

        int32 tokenIndex;

        NonTrivialToken(Token token, int32 tokenIndex)
            : Token(token), tokenIndex(tokenIndex) {}

    };

    struct PsiBuilder2 {

    private:

        struct StreamRange {
            int32 start;
            int32 end;
        };

        struct BlockHeader {
            uint32 padding {}; // we don't want to touch the bits here or we can break NodeType enum
            int32 next {};
        };

        int32 currentNonTrivialTokenIndex {};
        BlockHeader allocator;

        char* src {};
        PodList<Token> tokens;
        PodList<NonTrivialToken> nonTrivialTokens;
        PodList<int32> production;
        PodList<AbstractPsiNode> nodeAllocator;
        PodList<ParseError> errors;
        PodList<int32> matchingStack;
        PodList<StreamRange> streamStack;
        LinearAllocator errorAllocator;
        int32 streamStart;
        int32 streamEnd;

    public:

        PsiBuilder2()
            : tokens(1024)
            , errorAllocator(0, KILOBYTES(4))
            , production(512)
            , nodeAllocator(512)
            , nonTrivialTokens(1024)
            , errors(32)
            , currentNonTrivialTokenIndex(0) {}

        void Reset() {
            src = nullptr;
            currentNonTrivialTokenIndex = 0;
            allocator.next = 0;
            nodeAllocator.size = 0;
            tokens.size = 0;
            production.size = 0;
            errors.size = 0;
            nonTrivialTokens.size = 0;
            matchingStack.size = 0;
            errorAllocator.Clear();
        }

        bool Initialize(ParseResult* parseResult, LinearAllocator * outputAllocator) {

            Reset();

            src = parseResult->src;

            Tokenizer tokenizer {};

            // if we have bad tokens or too many tokens we copy just the token buffer over and consider that a failed parse
            bool success = tokenizer.Tokenize(parseResult->src, parseResult->srcLength, &tokens) && tokens.size < 65536;

            nonTrivialTokens.EnsureCapacity(tokens.size);

            // find the first non-trivial token
            int32 firstNonTrivialIndex = -1;
            for (int32 i = 0; i < tokens.size; i++) {
                Token& token = tokens[i];
                if (token.tokenType != TokenType::Whitespace && token.tokenType != TokenType::Comment) {
                    firstNonTrivialIndex = i;
                    break;
                }
            }

            parseResult->hasBadCharacters = !success;
            parseResult->hasTooManyTokens = tokens.size >= 65536;
            parseResult->hasUnmatchedDelimiters = false;
            parseResult->hasNonTrivialContent = firstNonTrivialIndex != -1;

            if (firstNonTrivialIndex > -1) {

                for (int32 i = firstNonTrivialIndex; i < tokens.size; i++) {

                    // we want to know if a token has a following token that separates it because we sometimes
                    // need to find tokens like << which is an operator but < < is either garbage or a generic type path
                    // since we remove the whitespace tokens this lets us remember that
                    Token& token = tokens[i];
                    if (token.tokenType == TokenType::Whitespace || token.tokenType == TokenType::Comment) {
                        nonTrivialTokens[nonTrivialTokens.size - 1].flags |= TokenFlags::FollowedByWhitespaceOrComment;
                    }
                    else {
                        nonTrivialTokens.Add(NonTrivialToken(token, i));
                    }

                }

                bool hasUnmatched = false;

                for (int32 i = 0; i < nonTrivialTokens.size; i++) {
                    NonTrivialToken& nonTrivialToken = nonTrivialTokens[i];
                    TokenType tokenType = nonTrivialToken.tokenType;

                    if (tokenType == TokenType::OpenParen || tokenType == TokenType::CurlyBraceOpen || tokenType == TokenType::SquareBraceOpen) {
                        matchingStack.Push(i);
                    }
                    else if (tokenType == TokenType::CloseParen || tokenType == TokenType::CurlyBraceClose || tokenType == TokenType::SquareBraceClose) {
                        if (matchingStack.size == 0) {
                            nonTrivialToken.flags |= TokenFlags::InvalidMatch;
                            hasUnmatched = true;
                        }
                        else {

                            TokenType match;

                            switch (tokenType) {

                                case TokenType::CloseParen:
                                    match = TokenType::OpenParen;
                                    break;
                                case TokenType::CurlyBraceClose:
                                    match = TokenType::CurlyBraceOpen;
                                    break;
                                case TokenType::SquareBraceClose:
                                    match = TokenType::SquareBraceOpen;
                                    break;
                                default:break;
                            }

                            int32 matchIndex = -1;
                            for (int32 x = matchingStack.size - 1; x >= 0; x--) {
                                if (nonTrivialTokens[matchingStack[x]].tokenType == match) {
                                    matchIndex = x;
                                    break;
                                }
                            }

                            if (matchIndex != -1) {

                                for (int32 x = matchingStack.size - 1; x > matchIndex; x--) {
                                    nonTrivialTokens[matchingStack[x]].flags |= TokenFlags::InvalidMatch;
                                    hasUnmatched = true;
                                }

                                matchingStack.size = matchIndex;

                            }
                            else {
                                // no matches at all in the stack, mark invalid & drop it
                                nonTrivialToken.flags |= TokenFlags::InvalidMatch;
                                hasUnmatched = true;
                            }

                        }
                    }
                }

                for (int32 i = 0; i < matchingStack.size; i++) {
                    nonTrivialTokens[matchingStack[i]].flags |= TokenFlags::InvalidMatch;
                }

                // parseResult->hasUnmatchedDelimiters = hasUnmatched || matchingStack.size != 0;
            }

            for (int32 i = 0; i < nonTrivialTokens.size; i++) {

                NonTrivialToken& pNonTrivialToken = nonTrivialTokens[i];
                tokens[pNonTrivialToken.tokenIndex].flags = pNonTrivialToken.flags;

                if((tokens[pNonTrivialToken.tokenIndex].flags & TokenFlags::InvalidMatch) != 0) {
                    parseResult->hasUnmatchedDelimiters = true;
                }

            }

            if (parseResult->hasUnmatchedDelimiters) {
                for (int32 i = 0; i < tokens.size; i++) {
                    if ((tokens[i].flags & TokenFlags::InvalidMatch) != 0) {
                        ParseError error;

                        switch (tokens[i].tokenType) {
                            case TokenType::OpenParen: {
                                error.message = (char*) "Unmatched paren open (";
                                break;
                            }
                            case TokenType::CloseParen: {
                                error.message = (char*) "Unmatched paren close )";
                                break;
                            }
                            case TokenType::CurlyBraceOpen: {
                                error.message = (char*) "Unmatched curly bracket open {";
                                break;
                            }
                            case TokenType::CurlyBraceClose: {
                                error.message = (char*) "Unmatched curly bracket close }";
                                break;
                            }
                            case TokenType::SquareBraceOpen: {
                                error.message = (char*) "Unmatched square brace open [";
                                break;
                            }
                            case TokenType::SquareBraceClose: {
                                error.message = (char*) "Unmatched square brace close ]";
                                break;
                            }
                            case TokenType::StringStart: {
                                error.message = (char*) "Unclosed string literal";
                                break;
                            }
                            default: {
                                error.message = (char*) "Unmatched delimiter";
                                break;
                            }
                        }

                        error.psiIndex = -1;
                        error.tokenStart = i;
                        error.tokenEnd = i + 1;
                        error.sourceStart = tokens[i].position;
                        error.sourceEnd = tokens[i].position + 1;
                        error.messageLength = (int32) strlen(error.message);

                        errors.Add(error);
                    }
                }
            }

            if (parseResult->hasBadCharacters) {
                for (int32 i = 0; i < tokens.size; i++) {
                    Token& token = tokens[i];
                    if (token.tokenType != TokenType::BadCharacter) {
                        continue;
                    }
                    int32 endIndex = i + 1;
                    for (int32 j = endIndex; j < tokens.size; j++) {
                        Token next = tokens[j];
                        if (next.tokenType != TokenType::BadCharacter) {
                            break;
                        }
                        endIndex++;
                    }
                    ParseError error;
                    error.message = (char*) "Bad character";
                    error.messageLength = (int32) strlen(error.message);
                    error.tokenStart = i;
                    error.tokenEnd = endIndex;
                    error.sourceStart = tokens[i].position;
                    error.sourceEnd = tokens[endIndex].position;
                    error.psiIndex = -1;
                    errors.Add(error);
                }
            }

            if (parseResult->hasTooManyTokens) {
                ParseError error;
                error.message = (char*) "The file contains too many tokens, for performance reasons we only support 65536 tokens";
                error.messageLength = (int32) strlen(error.message);
                error.tokenStart = 0;
                error.tokenEnd = tokens.size;
                error.sourceStart = tokens[0].position;
                error.sourceEnd = tokens[tokens.size - 1].position;
                error.psiIndex = -1;
                errors.Add(error);
            }

            memset(nodeAllocator.Reserve(), 0, sizeof(AbstractPsiNode));

            FileNode* rootNode = (FileNode*)nodeAllocator.Reserve();
            new (rootNode) FileNode();
            rootNode->tokenStart = 0;
            rootNode->tokenEnd = tokens.size;
            rootNode->nodeIndex = nodeAllocator.size - 1;

            _Add(nodeAllocator.size - 1);

            bool validParse = !parseResult->hasUnmatchedDelimiters && !parseResult->hasBadCharacters && !parseResult->hasTooManyTokens;

            parseResult->tokens = outputAllocator->Copy(tokens.ToCheckedArray());

            streamStart = 0;
            streamEnd = nonTrivialTokens.size;
            streamStack.size = 0;

            if (!validParse) {
                _Add(-1); // eof

                parseResult->production = CheckedArray<int32>(nullptr, 0);
                parseResult->psiNodes = outputAllocator->Copy(nodeAllocator.ToCheckedArray());
                parseResult->errors = outputAllocator->Copy(errors.ToCheckedArray());

            }

            return validParse;

        }

        struct SubStream {

            PsiBuilder2* b {};
            int32 tokenStart {};
            int32 tokenEnd {};

            SubStream() = default;

            SubStream(PsiBuilder2* b, int32 tokenStart, int32 tokenEnd)
                : b(b)
                , tokenStart(tokenStart)
                , tokenEnd(tokenEnd) {}

            bool HasMoreTokens() const {
                return b->currentNonTrivialTokenIndex < tokenEnd; // we don't report the ending delimiter as part of the stream
            }

            void Done() const {
                b->SetTokenIndex(tokenEnd + 1); // skip over the last token
            }

            bool Empty() const {
                return tokenStart == tokenEnd;
            }

        };

        bool TryGetSubSubStream(SubStream* subStream, TokenType open, TokenType close) {
            int32 start = currentNonTrivialTokenIndex;

            if (GetTokenType() != open) {
                return false;
            }

            int32 level = 1;
            AdvanceLexer();

            while (!EndOfInput()) {
                TokenType tokenType = GetTokenType();
                if (tokenType == open) {
                    level++;
                }
                else if (tokenType == close) {
                    level--;
                    if (level == 0) {
                        *subStream = SubStream(this, start + 1, currentNonTrivialTokenIndex);
                        SetTokenIndex(start + 1); // stream will start at { and end at }
                        return true;
                    }
                }

                AdvanceLexer();
            }

            SetTokenIndex(start);
            return false;
        }

        bool TryGetSubSubStream_CurlyBracket(SubStream* subStream) {
            return TryGetSubSubStream(subStream, TokenType::CurlyBraceOpen, TokenType::CurlyBraceClose);
        }

        bool TryGetSubSubStream_Paren(SubStream* subStream) {
            return TryGetSubSubStream(subStream, TokenType::OpenParen, TokenType::CloseParen);
        }

        bool TryGetSubSubStream_SquareBrace(SubStream* subStream) {
            return TryGetSubSubStream(subStream, TokenType::SquareBraceOpen, TokenType::SquareBraceClose);
        }

        bool Finalize(ParseResult* parseResult, LinearAllocator * outputAllocator) {

            assert(currentNonTrivialTokenIndex >= nonTrivialTokens.size && "some tokens were missing from the tree");

            _Add(-1); // eof

#if false
            struct ProductionItem {
                int32 markerId {};
                NodeType nodeType {NodeType::None};
            };

            PodList<ProductionItem> debugItems;
            bool unclosed = false;
            for (int32 i = 0; i < production.size; i++) {
                ProductionItem item;
                item.markerId = production[i];
                if (item.markerId < 0) {
                    item.nodeType = nodeAllocator.GetPointer(-item.markerId)->nodeBase.nodeType;
                }
                else {
                    item.nodeType = nodeAllocator.GetPointer(item.markerId)->nodeBase.nodeType;
                    bool found = false;
                    for (int32 j = i + 1; j < production.size; j++) {
                        if (production[j] == -item.markerId) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        unclosed = true;
                    }

                }

                debugItems.Add(item);
            }
            if (unclosed) {
                for (int32 i = 0; i < debugItems.size; i++) {
                    const char* str = debugItems[i].nodeType == NodeType::None ? "__UNCLOSED__" : NodeTypeToString(debugItems[i].nodeType);
                    printf("%d %s\n", debugItems[i].markerId, str);
                }
                assert(false && "has unclosed markers");
            }
#endif

            // ParseResult owns all the error allocations

            for (int32 i = 0; i < production.size; i++) {
                int32 index = production[i];
                if (index > 0 && nodeAllocator[index].nodeBase.nodeType == NodeType::Error) {
                    ErrorNode* e = (ErrorNode*) nodeAllocator.GetPointer(index);
                    ParseError* error = errors.Reserve();
                    if (e->tokenEnd >= tokens.size) {
                        e->tokenEnd = tokens.size - 1;
                    }
                    error->message = outputAllocator->AllocateUncleared<char>(e->messageLength);
                    error->messageLength = e->messageLength;
                    error->tokenStart = e->tokenStart;
                    error->tokenEnd = e->tokenEnd;

                    error->psiIndex = e->nodeIndex;

                    memcpy(error->message, e->message, e->messageLength);

                    // attach to previous token if we are an inline error
                    if (e->tokenStart == e->tokenEnd) {

                        // skip whitespace and comments
                        int32 offset = 1;

                        while (tokens[error->tokenStart - offset].tokenType == TokenType::Whitespace || tokens[error->tokenStart - offset].tokenType == TokenType::Comment) {
                            offset++;
                        }

                        error->tokenStart -= offset;
                        error->tokenEnd -= offset;
                        error->sourceStart = tokens[error->tokenStart].position;
                        error->sourceEnd = tokens[error->tokenEnd + 1].position;
                    }
                    else {
                        error->sourceStart = tokens[e->tokenStart].position;

                        error->sourceEnd = tokens[e->tokenEnd].position;
                    }

                }
            }

            parseResult->production = outputAllocator->Copy(production.ToCheckedArray());
            parseResult->psiNodes = outputAllocator->Copy(nodeAllocator.ToCheckedArray());
            parseResult->errors = outputAllocator->Copy(errors.ToCheckedArray());

            Reset();

            return parseResult->errors.size == 0;

        }

        FixedCharSpan GetTokenSource(int32 startToken, int32 endToken) {
            if (EndOfInput()) {
                return FixedCharSpan(nullptr, 0);
            }

            NonTrivialToken& nonTrivialTokenStart = nonTrivialTokens[startToken];
            NonTrivialToken& nonTrivialTokenEnd  = nonTrivialTokens[endToken];

            int32 start = tokens[nonTrivialTokenStart.tokenIndex].position;
            int32 end = tokens[nonTrivialTokenEnd.tokenIndex].position;

            return FixedCharSpan(src + start, end - start);

        }

        FixedCharSpan GetTokenSource() {

            if (EndOfInput()) {
                return FixedCharSpan(nullptr, 0);
            }

            NonTrivialToken& nonTrivialToken = nonTrivialTokens[currentNonTrivialTokenIndex];

            int32 start = tokens[nonTrivialToken.tokenIndex].position;
            int32 end = tokens[nonTrivialToken.tokenIndex + 1].position;

            return FixedCharSpan(src + start, end - start);

        }

        Token TokenFromNonTrivial(int32 nonTrivialToken) {
            return tokens[nonTrivialTokens[nonTrivialToken].tokenIndex];
        }

        Marker MarkFromToken(int32 nonTrivialTokenIndex) {
            // probably want to be sure there aren't completed markers between current token and token
            Marker m = Allocate(nonTrivialTokens[nonTrivialTokenIndex].tokenIndex);
            _Add(m.id);
            return m;
        }

        Marker Mark() {
            Marker m = Allocate(nonTrivialTokens[currentNonTrivialTokenIndex].tokenIndex);
            _Add(m.id);
            return m;
        }

        template<typename T>
        T* GetPsiNodeUnsafe(NodeIndex<T> index) {
            return (T*) nodeAllocator.GetPointer(index.index);
        }

        int32 GetTokenIndex() const {
            return currentNonTrivialTokenIndex;
        }

        void SetTokenIndex(int32 tokenIndex) {
            currentNonTrivialTokenIndex = tokenIndex;
            if (currentNonTrivialTokenIndex < 0) currentNonTrivialTokenIndex = 0;
            if (currentNonTrivialTokenIndex > nonTrivialTokens.size) currentNonTrivialTokenIndex = nonTrivialTokens.size;
        }

        void InlineError(const char* message) {
            Error(Mark(), message);
        }

        void CompletionOpportunity(const char* message) {
            // todo -- another node type for completions
            Error(Mark(), message);
        }

        void ErrorArgs(Marker m, const char* base, ...) {

            va_list args;
            va_start(args, base);

            int32 size = vsnprintf(nullptr, 0, base, args) + 1;
            char* pStr = (char*) errorAllocator.AllocateUncleared<char>(size + 1);
            vsnprintf(pStr, size, base, args);
            pStr[size] = '\0';

            va_end(args);

            ErrorNode errorNode(pStr, size);
            ErrorDone(m, errorNode);
        }

        void Drop(Marker marker) {
            AbstractPsiNode* n = nodeAllocator.GetPointer(marker.id);
            if (n->nodeBase.tokenEnd != 0) {
                production.RemoveAt(LastIndexOf(-marker.id));
            }
            production.RemoveAt(LastIndexOf(marker.id));
            Free(marker.id);
        }

        void Error(Marker m, const char* error) {
            ErrorNode errorNode((char*) error, (int32) strlen(error));
            ErrorDone(m, errorNode);
        }

        NodeIndex<DeclarationNode> ErrorDeclaration(Marker m, const char* error) {
            Marker e = Mark();
            ErrorNode errorNode((char*) error, (int32) strlen(error));
            ErrorDone(e, errorNode);
            return DoneDeclaration(m, ErrorDeclarationNode());
        }

        Marker Precede(Marker marker) {
            assert(marker.id > 0);
            Marker retn = Allocate(nodeAllocator[marker.id].nodeBase.tokenStart);
            _InsertAt(retn.id, LastIndexOf(marker.id));
            return retn;
        }

        Marker Precede(UntypedNodeIndex nodeIndex) {
            assert(nodeIndex.IsValid());
            Marker retn = Allocate(nodeAllocator[nodeIndex.index].nodeBase.tokenStart);
            _InsertAt(retn.id, LastIndexOf(nodeIndex.index));
            return retn;
        }

        template<typename T>
        Marker Precede(NodeIndex<T> index) {
            assert(index.IsValid());
            Marker retn = Allocate(nodeAllocator[index.index].nodeBase.tokenStart);
            _InsertAt(retn.id, LastIndexOf(index.index));  // production.Insert(retn.id, IndexOf(index.index));
            return retn;
        }

        template<typename T>
        [[nodiscard]] NodeIndex<T> Done(Marker m, T&& node) {
            AbstractPsiNode* n = nodeAllocator.GetPointer(m.id);
            node.nodeIndex = m.id;
            node.tokenStart = n->nodeBase.tokenStart;
            node.tokenEnd = nonTrivialTokens[currentNonTrivialTokenIndex].tokenIndex;
            memcpy(n, &node, sizeof(T));
            _Add(-m.id);
            return NodeIndex<T>(m.id);
        }

        template<typename T>
        [[nodiscard]] UntypedNodeIndex DoneUntyped(Marker m, T&& node) {
            AbstractPsiNode* n = nodeAllocator.GetPointer(m.id);
            node.nodeIndex = m.id;
            node.tokenStart = n->nodeBase.tokenStart;
            node.tokenEnd = nonTrivialTokens[currentNonTrivialTokenIndex].tokenIndex;
            memcpy(n, &node, sizeof(T));
            _Add(-m.id);
            return UntypedNodeIndex(m.id);
        }

        template<typename T>
        [[nodiscard]] NodeIndex<InitializerNode> DoneInitializer(Marker m, T&& node) {

            static_assert(std::is_base_of<InitializerNode, T>::value);

            AbstractPsiNode* n = nodeAllocator.GetPointer(m.id);
            node.nodeIndex = m.id;
            node.tokenStart = n->nodeBase.tokenStart;
            node.tokenEnd = nonTrivialTokens[currentNonTrivialTokenIndex].tokenIndex;
            memcpy(n, &node, sizeof(T));
            _Add(-m.id);
            return NodeIndex<InitializerNode>(m.id);
        }

        template<typename T>
        [[nodiscard]] NodeIndex<ExpressionNode> DoneExpression(Marker m, T&& node) {

            static_assert(std::is_base_of<ExpressionNode, T>::value);

            AbstractPsiNode* n = nodeAllocator.GetPointer(m.id);
            node.nodeIndex = m.id;
            node.tokenStart = n->nodeBase.tokenStart;
            node.tokenEnd = nonTrivialTokens[currentNonTrivialTokenIndex].tokenIndex;
            memcpy(n, &node, sizeof(T));
            _Add(-m.id);
            return NodeIndex<ExpressionNode>(m.id);
        }

        template<typename T>
        [[nodiscard]] NodeIndex<StatementNode> DoneStatement(Marker m, T&& node) {

            static_assert(std::is_base_of<StatementNode, T>::value);

            AbstractPsiNode* n = nodeAllocator.GetPointer(m.id);
            node.nodeIndex = m.id;
            node.tokenStart = n->nodeBase.tokenStart;
            node.tokenEnd = nonTrivialTokens[currentNonTrivialTokenIndex].tokenIndex;
            memcpy(n, &node, sizeof(T));
            _Add(-m.id);//production.Add(-m.id);
            return NodeIndex<StatementNode>(m.id);
        }

        template<typename T>
        [[nodiscard]] NodeIndex<DeclarationNode> DoneDeclaration(Marker m, T&& node) {

            static_assert(std::is_base_of<DeclarationNode, T>::value);

            AbstractPsiNode* n = nodeAllocator.GetPointer(m.id);
            node.nodeIndex = m.id;
            node.tokenStart = n->nodeBase.tokenStart;
            node.tokenEnd = nonTrivialTokens[currentNonTrivialTokenIndex].tokenIndex;
            memcpy(n, &node, sizeof(T));
            _Add(-m.id);//production.Add(-m.id);
            return NodeIndex<DeclarationNode>(m.id);
        }

        bool EndOfInput() const {
            return currentNonTrivialTokenIndex >= streamEnd; // nonTrivialTokens.size;
        }

        Token GetToken() {
            return nonTrivialTokens[currentNonTrivialTokenIndex]; // NOLINT(*-slicing)
        }

        Token GetTokenAt(int32 idx) {
            return nonTrivialTokens[idx]; // NOLINT(*-slicing)
        }

        Token GetRawTokenAt(int32 idx) {
            return tokens[idx];
        }

        Token Peek(uint32 steps) {

            int32 cur = currentNonTrivialTokenIndex + steps;
            if (cur < nonTrivialTokens.size && cur >= 0) {
                return nonTrivialTokens[cur];
            }
            else {
                return nonTrivialTokens[nonTrivialTokens.size - 1];
            }
        }

        void AdvanceToStreamEnd() {
            currentNonTrivialTokenIndex = streamEnd;
        }

        void PushStream(SubStream* stream) {
            currentNonTrivialTokenIndex = stream->tokenStart;
            streamStack.Push({streamStart, streamEnd});
            streamStart = stream->tokenStart;
            streamEnd = stream->tokenEnd;
        }

        void PopStream() {
            if(currentNonTrivialTokenIndex != streamEnd) {
                 Marker m = Mark();
                 currentNonTrivialTokenIndex = streamEnd;
                 Error(m, "invalid content");
            }
            currentNonTrivialTokenIndex = streamEnd + 1;
            StreamRange r = streamStack.Pop();
            streamStart = r.start;
            streamEnd = r.end;
        }

        void ClearStreams() {
            streamStack.size = 0;
            streamStart = 0;
            streamEnd = nonTrivialTokens.size;
        }

        void AdvanceLexer() {
            if (currentNonTrivialTokenIndex < streamEnd) {
                currentNonTrivialTokenIndex++;
            }
//            if (EndOfInput()) {
//                return;
//            }
//            currentNonTrivialTokenIndex++;
        }

        bool IsReservedKeyword() {
            return GetKeyword() != Keyword::NotAKeyword;
        }

        Keyword GetKeyword() {
            if (EndOfInput()) {
                return Keyword::NotAKeyword;
            }
            return nonTrivialTokens[currentNonTrivialTokenIndex].keyword;
        }

        TokenType GetTokenType() {

            if (EndOfInput()) {
                return TokenType::EndOfInput;
            }

            return nonTrivialTokens[currentNonTrivialTokenIndex].tokenType;

        }

        Marker GetMarker(int32 markerId) {
            assert(markerId > 0);
            return Marker(markerId);
        }

        bool HasErrorAfter(Marker marker) {

            assert(marker.id > 0);

            for (int32 i = LastIndexOf(marker.id) + 1; i < production.size; ++i) {

                int32 nodeIndex = production[i];

                if (nodeIndex <= 0) {
                    continue;
                }

                NodeBase* n = (NodeBase*) nodeAllocator.GetPointer(nodeIndex);

                if (n->nodeType == NodeType::Error) {
                    return true;
                }

            }

            return false;

        }

        void _Add(int32 value) {
            production.Add(value);
        }

        void _InsertAt(int32 value, int32 atIndex) {
            production.Insert(value, atIndex);
        }

        void RollbackTo(Marker m) {

            // need to find the non-trivial index for the marker
            // we've stored token start as an absolute index so we
            // start there so i guess we just start
            // there and go backwards until we find the right index

            int32 trivialIndex = nodeAllocator.GetPointer(m.id)->nodeBase.tokenStart;
            int32 searchStart = trivialIndex;

            if (trivialIndex > nonTrivialTokens.size - 1) {
                searchStart = nonTrivialTokens.size - 1;
            }

            bool found = false;
            for (int32 i = searchStart; i >= 0; i--) {
                if (nonTrivialTokens[i].tokenIndex == trivialIndex) {
                    currentNonTrivialTokenIndex = i;
                    found = true;
                    break;
                }
            }

            assert(found);

            int32 idx = LastIndexOf(m.id);
            for (int32 i = production.size - 1; i >= idx; i--) {
                int32 markerId = production[i];
                if (markerId > 0) {
                    Free(markerId);
                }
            }

            production.size = idx;

        }

    private:

        int32 LastIndexOf(int32 target) const {

            const int32 size = production.size;
            for (int32 i = size - 1; i >= 0; i--) {
                if (production.array[i] == target) return i;
            }

            return -1;
        }

        int32 IndexOf(int32 target) const {

            const int32 size = production.size;
            for (int32 i = 0; i < size; i++) {
                if (production.array[i] == target) {
                    return i;
                }
            }
            return -1;

        }

        Marker Allocate(int32 tokenIndex) {

            AbstractPsiNode* allocatedNode;

            if (allocator.next != 0) {
                AbstractPsiNode* ptr = nodeAllocator.GetPointer(allocator.next);
                BlockHeader* newHeader = (BlockHeader*) ptr;
                allocator.next = newHeader->next;
                allocatedNode = ptr;
            }
            else {
                allocatedNode = nodeAllocator.Reserve();
                memset(allocatedNode, 0, sizeof(AbstractPsiNode));
            }

            int32 markerId = (int32)(allocatedNode - nodeAllocator.array);

            allocatedNode->nodeBase.tokenStart = tokenIndex;
            allocatedNode->nodeBase.nodeIndex = markerId;

            return Marker(markerId);

        }

        void Free(int32 markerId) {

            assert(markerId > 0 && markerId < nodeAllocator.size);

            AbstractPsiNode* allocatedNode = nodeAllocator.GetPointer(markerId);

            // assuming is an error node, we check the type to make sure it's actually an error before freeing
            ErrorNode* errorNode = (ErrorNode*) allocatedNode;

            memset(allocatedNode, 0, sizeof(AbstractPsiNode));

            BlockHeader* blockHeader = (BlockHeader*) allocatedNode;
            blockHeader->next = allocator.next;
            allocator.next = markerId;

        }

        void ErrorDone(Marker m, ErrorNode& errorNode) {
            AbstractPsiNode* n = nodeAllocator.GetPointer(m.id);
            errorNode.nodeIndex = m.id;
            errorNode.tokenStart = n->nodeBase.tokenStart;
            if (currentNonTrivialTokenIndex >= nonTrivialTokens.size) {
                errorNode.tokenEnd = nonTrivialTokens[currentNonTrivialTokenIndex - 1].tokenIndex;
            }
            else {
                errorNode.tokenEnd = nonTrivialTokens[currentNonTrivialTokenIndex].tokenIndex;
            }
            memcpy(n, &errorNode, sizeof(ErrorNode));
            _Add(-m.id);
        }

    };

}