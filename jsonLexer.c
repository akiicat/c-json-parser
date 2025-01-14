#include <stddef.h>
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

char *substring(struct LexerContext *ctx, size_t start, size_t end) {
    size_t textSize = 0;
    char *text = NULL;
    int error = 0;

    error = fseek(ctx->stream, start-1, SEEK_SET);
    if (error) {
        fprintf(stderr, "Error fseek: %s\n", strerror(error));
        print_trace();
        assert(0);
    }

    textSize = end - start + 1;
    text = (char *)malloc(textSize + 1);
    fread(text, textSize, 1, ctx->stream);
    text[textSize] = '\0';
    
    error = fseek(ctx->stream, ctx->offset, SEEK_SET);
    if (error) {
        fprintf(stderr, "Error fseek: %s\n", strerror(error));
        print_trace();
        assert(0);
    }

    return text;
}

void printToken(struct LexerContext *ctx) {
    struct Token t;
    for (int i = 0; i < ctx->container->tokenLength; i++) {
        t = ctx->container->tokenList[i];
        if (t.text) {
            printf("@%u#%u,%u<%u|%s>%u:%u %s\n", t.index, t.start, t.end, t.type, type2str(t.type), t.column, t.row, t.text);
        } else {
            printf("@%u#%u,%u<%u|%s>%u:%u\n", t.index, t.start, t.end, t.type, type2str(t.type), t.column, t.row);
        }
    }
}

void getTokenLPAIR(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startRow = ctx->row;

    match(ctx, '{');

    insertToken(ctx->container, (struct Token) {
        .type = T_LPAIR,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenRPAIR(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startRow = ctx->row;

    match(ctx, '}');

    insertToken(ctx->container, (struct Token) {
        .type = T_RPAIR,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenLARRAY(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startRow = ctx->row;

    match(ctx, '[');

    insertToken(ctx->container, (struct Token) {
        .type = T_LARRAY,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenRARRAY(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startRow = ctx->row;

    match(ctx, ']');

    insertToken(ctx->container, (struct Token) {
        .type = T_RARRAY,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenCOMMA(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startRow = ctx->row;

    match(ctx, ',');

    insertToken(ctx->container, (struct Token) {
        .type = T_COMMA,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenCOLON(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startRow = ctx->row;

    match(ctx, ':');

    insertToken(ctx->container, (struct Token) {
        .type = T_COLON,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenSTRING(struct LexerContext *ctx) {
    size_t start = 0;
    size_t end = 0;
    size_t startRow = ctx->row;

    match(ctx, '"');

    start = ctx->offset;

    // '"' (~["\r\n] | '\\' . ) * '"'
    while (!(lookahead(ctx, '"') || lookahead(ctx, '\r') || lookahead(ctx, '\r') || lookahead(ctx, EOF))) {
        matchIfExist(ctx, '\\');
        nextChar(ctx);
    }

    end = ctx->offset - 1;

    match(ctx, '"');

    insertToken(ctx->container, (struct Token) {
        .type = T_STRING,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenNUMBER(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startRow = ctx->row;

    // we should lookahead more, but here we get number simply
    // '-'? ('0' | [1-9][0-9]*)+ ('.' [0-9]+)? ([eE] [+-]? [0-9]+)?
    while (matchIfExist(ctx, '0') || matchIfExist(ctx, '1') || matchIfExist(ctx, '2') || matchIfExist(ctx, '3') || matchIfExist(ctx, '4') || 
            matchIfExist(ctx, '5') || matchIfExist(ctx, '6') || matchIfExist(ctx, '7') || matchIfExist(ctx, '8') || matchIfExist(ctx, '9') ||
            matchIfExist(ctx, '-') || matchIfExist(ctx, '+') || matchIfExist(ctx, '.') || matchIfExist(ctx, 'e') || matchIfExist(ctx, 'E'));

    end = ctx->offset - 1;

    insertToken(ctx->container, (struct Token) {
        .type = T_NUMBER,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenTRUE(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startRow = ctx->row;

    matchStr(ctx, "true");

    end = ctx->offset - 1;

    insertToken(ctx->container, (struct Token) {
        .type = T_TRUE,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenFALSE(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startRow = ctx->row;

    matchStr(ctx, "false");

    end = ctx->offset - 1;

    insertToken(ctx->container, (struct Token) {
        .type = T_FALSE,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
    });
}

void getTokenNULL(struct LexerContext *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startRow = ctx->row;

    matchStr(ctx, "null");

    end = ctx->offset - 1;

    insertToken(ctx->container, (struct Token) {
        .type = T_NULL,
        .start = start,
        .end = end,
        .column = ctx->column,
        .row = startRow,
        .text = substring(ctx, start, end),
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
