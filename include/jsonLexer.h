#ifndef __JSON_LEXER_H__
#define __JSON_LEXER_H__

#include <stdio.h>

enum json_lexer_type {
    LT_MISSING,
    LT_STRING,
    LT_NUMBER,
    LT_COMMA,
    LT_COLON,
    LT_LPAIR,
    LT_RPAIR,
    LT_LARRAY,
    LT_RARRAY,
    LT_TRUE,
    LT_FALSE,
    LT_NULL,
    LEXER_TOKEN_SIZE
};

struct json_lexer_token_t {
    enum json_lexer_type type;
    char *text;
    unsigned int index;
    unsigned int start;
    unsigned int end;
    unsigned int column;
    unsigned int row;
};

struct json_lexer_container_t {
    unsigned int length;
    unsigned int capacity;
    struct json_lexer_token_t *list;
};

struct json_lexer_context_t {
    struct json_lexer_container_t tokens;
    int currentChar;
    unsigned int offset;
    unsigned int column;
    unsigned int row;
    FILE *stream;
};

struct json_lexer_context_t *initJsonLexer(FILE *stream);
void freeJsonLexer(struct json_lexer_context_t *);

void jsonLexer(struct json_lexer_context_t *ctx);
void printLexerToken(struct json_lexer_context_t *ctx);

#endif
