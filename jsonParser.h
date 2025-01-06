#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include "token.h"

struct ParserContext;

struct ParserContext {
    int tokenIndex;
    unsigned int tokenLength;
    struct Token *tokenList;
    struct jsonToken json;
};

void jsonParser(struct ParserContext *ctx);
void printTree(struct ParserContext *ctx);

#endif