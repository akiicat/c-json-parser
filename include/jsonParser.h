#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include "jsonLexer.h"
#include "token.h"

struct ParserContext {
    unsigned int tokenIndex;
    struct TokenContainer *container;
    union valueToken root;
};

struct ParserContext *initJsonParser(struct LexerContext *lexer_ctx);
void freeJsonParser(struct ParserContext *);

union valueToken *jsonParser(struct ParserContext *ctx);
void printTree(struct ParserContext *ctx);
void freeParser(struct ParserContext *ctx);

#endif