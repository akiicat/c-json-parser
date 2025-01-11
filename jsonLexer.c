#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "jsonLexer.h"
#include "debug.h"

int nextChar(struct LexerContext *ctx) {
    if (!ctx->stream) {
        fprintf(stderr, "No Input stream\n");
        print_trace();
        assert(0);
    }

    ctx->currentChar = fgetc(ctx->stream);
    ctx->offset = ftell(ctx->stream);
    ctx->row++;
    return ctx->currentChar;
}

bool lookahead(struct LexerContext *ctx, int c) {
    return ctx->currentChar == c;
}

void match(struct LexerContext *ctx, int c) {
    char curC = ctx->currentChar;

    // always get the next one before checking
    nextChar(ctx);

    if (curC != c) {
        fprintf(stderr, "Syntax Error: Unexpected Char: %c\n", curC);
        print_trace();
        assert(0);
    }
}

void matchStr(struct LexerContext *ctx, const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        match(ctx, str[i]);
    }
}

bool matchIfExist(struct LexerContext *ctx, int c) {
    bool isMatch = (ctx->currentChar == c);

    if (isMatch) {
        nextChar(ctx);
    }

    return isMatch;
}

void insertToken(struct LexerContext *ctx, struct Token t) {
    struct Token *oldTokenList = NULL;
    unsigned int oldTokenSize = 0;
    unsigned int newTokenSize = 0;
    unsigned int textSize = 0;
    int error = 0;

    // set default token list
    if (!ctx->tokenList) {
        ctx->tokenList = (struct Token *)malloc(sizeof(struct Token) * 1); 
        ctx->tokenCapacity = 1;
    }

    // double token list if full, time complexity O(3n)
    if (ctx->tokenLength >= ctx->tokenCapacity) {
        oldTokenSize = sizeof(struct Token) * ctx->tokenCapacity;
        newTokenSize = oldTokenSize * 2;

        oldTokenList = ctx->tokenList;
        ctx->tokenList = (struct Token *)malloc(newTokenSize);
        memcpy(ctx->tokenList, oldTokenList, oldTokenSize);

        free(oldTokenList);
        oldTokenList = NULL;

        ctx->tokenCapacity *= 2;
    }

    error = fseek(ctx->stream, t.start-1, SEEK_SET);
    if (error) {
        fprintf(stderr, "Error fseek: %s\n", strerror(error));
        print_trace();
        assert(0);
    }

    textSize = t.end - t.start + 1;
    t.text = (char *)malloc(textSize + 1);
    fread(t.text, textSize, 1, ctx->stream);
    t.text[textSize] = '\0';
    
    error = fseek(ctx->stream, ctx->offset, SEEK_SET);
    if (error) {
        fprintf(stderr, "Error fseek: %s\n", strerror(error));
        print_trace();
        assert(0);
    }

    ctx->tokenList[ctx->tokenLength++] = t;
}

void printToken(struct LexerContext *ctx) {
    struct Token t;
    for (int i = 0; i < ctx->tokenLength; i++) {
        t = ctx->tokenList[i];
        if (t.text) {
            printf("@%u#%u,%u<%u|%s>%u:%u %s\n", t.index, t.start, t.end, t.type, type2str(t.type), t.column, t.row, t.text);
        } else {
            printf("@%u#%u,%u<%u|%s>%u:%u\n", t.index, t.start, t.end, t.type, type2str(t.type), t.column, t.row);
        }
    }
}

void getTokenLPAIR(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    match(ctx, '{');

    insertToken(ctx, (struct Token) {
        .type = T_LPAIR,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenRPAIR(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    match(ctx, '}');

    insertToken(ctx, (struct Token) {
        .type = T_RPAIR,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenLARRAY(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    match(ctx, '[');

    insertToken(ctx, (struct Token) {
        .type = T_LARRAY,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenRARRAY(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    match(ctx, ']');

    insertToken(ctx, (struct Token) {
        .type = T_RARRAY,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenCOMMA(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    match(ctx, ',');

    insertToken(ctx, (struct Token) {
        .type = T_COMMA,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenCOLON(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    match(ctx, ':');

    insertToken(ctx, (struct Token) {
        .type = T_COLON,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenSTRING(struct LexerContext *ctx) {
    unsigned int start = 0;
    unsigned int end = 0;
    unsigned int startRow = ctx->row;

    match(ctx, '"');

    start = ctx->offset;

    // '"' (~["\r\n] | '\\' . ) * '"'
    while (!(lookahead(ctx, '"') || lookahead(ctx, '\r') || lookahead(ctx, '\r') || lookahead(ctx, EOF))) {
        matchIfExist(ctx, '\\');
        nextChar(ctx);
    }

    end = ctx->offset;

    match(ctx, '"');

    insertToken(ctx, (struct Token) {
        .type = T_STRING,
        .index = ctx->tokenLength,
        .start = start,
        .end = end - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenNUMBER(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    // we should lookahead more, but here we get number simply
    // '-'? ('0' | [1-9][0-9]*)+ ('.' [0-9]+)? ([eE] [+-]? [0-9]+)?
    while (matchIfExist(ctx, '0') || matchIfExist(ctx, '1') || matchIfExist(ctx, '2') || matchIfExist(ctx, '3') || matchIfExist(ctx, '4') || 
            matchIfExist(ctx, '5') || matchIfExist(ctx, '6') || matchIfExist(ctx, '7') || matchIfExist(ctx, '8') || matchIfExist(ctx, '9') ||
            matchIfExist(ctx, '-') || matchIfExist(ctx, '+') || matchIfExist(ctx, '.') || matchIfExist(ctx, 'e') || matchIfExist(ctx, 'E'));

    insertToken(ctx, (struct Token) {
        .type = T_NUMBER,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenTRUE(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    matchStr(ctx, "true");

    insertToken(ctx, (struct Token) {
        .type = T_TRUE,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenFALSE(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    matchStr(ctx, "false");

    insertToken(ctx, (struct Token) {
        .type = T_FALSE,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void getTokenNULL(struct LexerContext *ctx) {
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    matchStr(ctx, "null");

    insertToken(ctx, (struct Token) {
        .type = T_NULL,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
        .column = ctx->column,
        .row = startRow,
    });
}

void jsonLexer(struct LexerContext *ctx) {

    nextChar(ctx);

    while (!lookahead(ctx, EOF)) {

        if (lookahead(ctx, '{')) {
            getTokenLPAIR(ctx);
        }
        else if (lookahead(ctx, '}')) {
            getTokenRPAIR(ctx);
        }
        else if (lookahead(ctx, '[')) {
            getTokenLARRAY(ctx);
        }
        else if (lookahead(ctx, ']')) {
            getTokenRARRAY(ctx);
        }
        else if (lookahead(ctx, ',')) {
            getTokenCOMMA(ctx);
        }
        else if (lookahead(ctx, ':')) {
            getTokenCOLON(ctx);
        }
        else if (lookahead(ctx, '"')) {
            getTokenSTRING(ctx);
        }
        else if (lookahead(ctx, '-') || lookahead(ctx, '0') || lookahead(ctx, '1') || lookahead(ctx, '2') || lookahead(ctx, '3') || lookahead(ctx, '4') ||
                    lookahead(ctx, '5') || lookahead(ctx, '6') || lookahead(ctx, '7') || lookahead(ctx, '8') || lookahead(ctx, '9')) {
            getTokenNUMBER(ctx);
        }
        else if (lookahead(ctx, 't')) {
            getTokenTRUE(ctx);
        }
        else if (lookahead(ctx, 'f')) {
            getTokenFALSE(ctx);
        }
        else if (lookahead(ctx, 'n')) {
            getTokenNULL(ctx);
        }
        else if (matchIfExist(ctx, '\n')) { // only match "\n", should consider these cases "\r\n" "\r"
            ctx->column++;
            ctx->row = 1;
        }
        else if (matchIfExist(ctx, ' ') || matchIfExist(ctx, '\t')) {
            continue;
        }
        else {
            match(ctx, EOF); // error
        }
    }
}
