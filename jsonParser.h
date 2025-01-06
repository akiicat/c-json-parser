#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include "token.h"

struct ParserContext;

struct BaseToken {
    enum TokenType type;
};

struct objToken {
    enum TokenType type;
    int pairLength;
    int pairCapacity;
    struct pairToken *pairList;
};

struct arrToken {
    enum TokenType type;
    int valueLength;
    int valueCapacity;
    struct valueToken *valueList;
};

struct valueToken {
    enum TokenType type;
    union {
        struct BaseToken next;
        struct Token anyToken;
        struct Token stringToken;
        struct Token numberToken;
        struct Token trueToken;
        struct Token falseToken;
        struct Token nullToken;
        struct objToken obj;
        struct arrToken arr;
    };
};

struct pairToken {
    enum TokenType type;
    struct Token key;
    struct valueToken value;
};

struct jsonToken {
    enum TokenType type;
    struct valueToken value;
};

struct ParserContext {
    int tokenIndex;
    unsigned int tokenLength;
    struct Token *tokenList;
    struct jsonToken json;
};

void jsonParser(struct ParserContext *ctx);
void printTree(struct ParserContext *ctx);
void printTreeNode(struct ParserContext *ctx, struct BaseToken *token);

#endif