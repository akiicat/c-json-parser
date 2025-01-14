#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include "token.h"

struct ParserContext {
    unsigned int tokenIndex;
    struct TokenContainer *container;
};

void jsonParser(struct ParserContext *ctx);
void printTree(struct ParserContext *ctx);
void freeParser(struct ParserContext *ctx);

#endif