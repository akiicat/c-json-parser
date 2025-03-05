#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "jsonLexer.h"
#include "debug.h"

const char *LexerTypeString[] = {
    "T_MISSING",
    "T_STRING",
    "T_NUMBER",
    "T_COMMA",
    "T_COLON",
    "T_LPAIR",
    "T_RPAIR",
    "T_LARRAY",
    "T_RARRAY",
    "T_TRUE",
    "T_FALSE",
    "T_NULL",
};

const char *lextype2str(enum json_lexer_type type) {
    if (type >= LT_MISSING && type < LEXER_TOKEN_SIZE) {
        return LexerTypeString[type];
    }

    return NULL;
}

struct json_lexer_context_t *initJsonLexer(FILE *stream) {
    struct json_lexer_context_t *lexer_ctx = malloc(sizeof(struct json_lexer_context_t));

    *lexer_ctx = (struct json_lexer_context_t) {
        .tokens = {
            .length = 0,
            .capacity = 0,
            .list = NULL,
        },
        .currentChar = '\0',
        .offset = 0,
        .column = 0,
        .row = 1,
        .stream = stream,
    };

    return lexer_ctx;
}

void freeJsonLexer(struct json_lexer_context_t *lexer_ctx) {
    if (lexer_ctx && lexer_ctx->tokens.list) {

        // Free each token's text in reverse order
        for (unsigned int i = lexer_ctx->tokens.length; i > 0; i--) {
            free(lexer_ctx->tokens.list[i - 1].text);
            lexer_ctx->tokens.list[i - 1].text = NULL;  // Defensive: nullify pointer after free
        }

        free(lexer_ctx->tokens.list);
    }

    free(lexer_ctx);
};

void insertToken(struct json_lexer_context_t *ctx, struct json_lexer_token_t *t) {
    struct json_lexer_container_t *tokens = &ctx->tokens;
    struct json_lexer_token_t *oldList = NULL;
    struct json_lexer_token_t *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    if (!tokens) {
        fprintf(stderr, "Error Token Container is not initialized\n");
        print_trace();
        assert(0);
    }

    // set default token list
    if (!tokens->list) {
        tokens->capacity = 1;
        tokens->list = (struct json_lexer_token_t *)malloc(sizeof(struct json_lexer_token_t) * tokens->capacity); 
    }

    // double token list if full, time complexity O(3n)
    if (tokens->length >= tokens->capacity) {
        oldSize = sizeof(struct json_lexer_token_t) * tokens->capacity;
        oldList = tokens->list;
        newSize = sizeof(struct json_lexer_token_t) * tokens->capacity * 2;
        newList = (struct json_lexer_token_t *)malloc(newSize);

        memcpy(newList, oldList, oldSize);
        free(oldList);
        oldList = NULL;

        tokens->list = newList;
        tokens->capacity *= 2;
    }

    t->index = tokens->length;

    tokens->list[tokens->length] = *t;
    tokens->length++;
}

int nextChar(struct json_lexer_context_t *ctx) {
    if (!ctx->stream) {
        fprintf(stderr, "No Input stream\n");
        print_trace();
        assert(0);
    }

    ctx->currentChar = fgetc(ctx->stream);
    ctx->offset = ftell(ctx->stream);
    ctx->column++;
    return ctx->currentChar;
}

bool lookahead(struct json_lexer_context_t *ctx, int c) {
    return ctx->currentChar == c;
}

void match(struct json_lexer_context_t *ctx, int c) {
    char curC = ctx->currentChar;

    // always get the next one before checking
    nextChar(ctx);

    if (curC != c) {
        fprintf(stderr, "Syntax Error %d:%d: Unexpected Char: %c\n", ctx->row, ctx->column, curC);
        print_trace();
        assert(0);
    }
}

void matchStr(struct json_lexer_context_t *ctx, const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        match(ctx, str[i]);
    }
}

bool matchIfExist(struct json_lexer_context_t *ctx, int c) {
    bool isMatch = (ctx->currentChar == c);

    if (isMatch) {
        nextChar(ctx);
    }

    return isMatch;
}

char *substring(struct json_lexer_context_t *ctx, size_t start, size_t end) {
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

void print__json_lexer_t(struct json_lexer_context_t *ctx) {
    struct json_lexer_token_t t;
    for (int i = 0; i < ctx->tokens.length; i++) {
        t = ctx->tokens.list[i];
        if (t.text) {
            printf("@%u#%u,%u<%u|%s>%u:%u %s\n", t.index, t.start, t.end, t.type, lextype2str(t.type), t.row, t.column, t.text);
        } else {
            printf("@%u#%u,%u<%u|%s>%u:%u\n", t.index, t.start, t.end, t.type, lextype2str(t.type), t.row, t.column);
        }
    }
}

void getTokenLPAIR(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, '{');

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_LPAIR,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenRPAIR(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, '}');

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_RPAIR,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenLARRAY(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, '[');

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_LARRAY,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenRARRAY(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, ']');

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_RARRAY,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenCOMMA(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, ',');

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_COMMA,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenCOLON(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, ':');

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_COLON,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenSTRING(struct json_lexer_context_t *ctx) {
    size_t start = 0;
    size_t end = 0;
    size_t startCol = ctx->column;

    match(ctx, '"');

    start = ctx->offset;

    // '"' (~["\r\n] | '\\' . ) * '"'
    while (!(lookahead(ctx, '"') || lookahead(ctx, '\r') || lookahead(ctx, '\r') || lookahead(ctx, EOF))) {
        matchIfExist(ctx, '\\');
        nextChar(ctx);
    }

    end = ctx->offset - 1;

    match(ctx, '"');

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_STRING,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenNUMBER(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    // we should lookahead more, but here we get number simply
    // '-'? ('0' | [1-9][0-9]*)+ ('.' [0-9]+)? ([eE] [+-]? [0-9]+)?
    while (matchIfExist(ctx, '0') || matchIfExist(ctx, '1') || matchIfExist(ctx, '2') || matchIfExist(ctx, '3') || matchIfExist(ctx, '4') || 
            matchIfExist(ctx, '5') || matchIfExist(ctx, '6') || matchIfExist(ctx, '7') || matchIfExist(ctx, '8') || matchIfExist(ctx, '9') ||
            matchIfExist(ctx, '-') || matchIfExist(ctx, '+') || matchIfExist(ctx, '.') || matchIfExist(ctx, 'e') || matchIfExist(ctx, 'E'));

    end = ctx->offset - 1;

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_NUMBER,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenTRUE(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    matchStr(ctx, "true");

    end = ctx->offset - 1;

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_TRUE,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenFALSE(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    matchStr(ctx, "false");

    end = ctx->offset - 1;

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_FALSE,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void getTokenNULL(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    matchStr(ctx, "null");

    end = ctx->offset - 1;

    insertToken(ctx, & (struct json_lexer_token_t) {
        .type = LT_NULL,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
        .text = substring(ctx, start, end),
    });
}

void jsonLexer(struct json_lexer_context_t *ctx) {

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
            ctx->row++;
            ctx->column = 0;
        }
        else if (matchIfExist(ctx, ' ') || matchIfExist(ctx, '\t')) {
            continue;
        }
        else {
            match(ctx, EOF); // error
        }
    }
}
