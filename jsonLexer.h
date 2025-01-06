#ifndef __JSON_LEXER_H__
#define __JSON_LEXER_H__

#include <stdio.h>

#include "token.h"

struct LexerContext {
    unsigned int tokenLength;
    unsigned int tokenCapacity;
    struct Token *tokenList;
    int currentChar;
    unsigned int offset;
    unsigned int column;
    unsigned int row;
    FILE *stream;
};

void jsonLexer(struct LexerContext *ctx);
void printToken(struct LexerContext *ctx);

#endif
