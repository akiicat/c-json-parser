#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <execinfo.h>

void print_trace(void) {
  void *array[10];
  char **strings;
  int size, i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);
  if (strings != NULL)
  {

    printf ("Obtained %d stack frames.\n", size);
    for (i = 0; i < size; i++)
      printf ("%s\n", strings[i]);
  }

  free (strings);
}

enum LexerToken {
    MISSING, T_STRING, T_NUMBER, T_COMMA, T_COLON, T_LPAIR, T_RPAIR, T_LARRAY, T_RARRAY, T_TRUE, T_FALSE, T_NULL
};

struct Token {
    enum LexerToken type;
    unsigned int index;
    unsigned int start;
    unsigned int end;
    unsigned int column;
    unsigned int row;
    char *text;
};

#define DEFAULT_TOKEN_CAPACITY 128

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

struct ParserContext {
    int tokenIndex;
    unsigned int tokenLength;
    struct Token *tokenList;
};

int nextChar(struct LexerContext *ctx) {
    if (!ctx->stream) {
        fprintf(stderr, "No Input stream\n");
        print_trace();
        assert(0);
    }

    ctx->currentChar = fgetc(ctx->stream);
    ctx->offset++;
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
    char *text = NULL;
    int error = 0;

    // set default token list
    if (!ctx->tokenList) {
        ctx->tokenList = (struct Token *)malloc(sizeof(struct Token) * DEFAULT_TOKEN_CAPACITY); 
        ctx->tokenCapacity = DEFAULT_TOKEN_CAPACITY;
    }

    // double token list if full, time complexity O(3n)
    if (ctx->tokenLength >= ctx->tokenCapacity - 1) {
        oldTokenList = ctx->tokenList;
        ctx->tokenList = (struct Token *)malloc(sizeof(struct Token) * ctx->tokenCapacity * 2);
        memcpy(ctx->tokenList, oldTokenList, ctx->tokenCapacity);
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

    t.text = malloc(t.end - t.start + 2);
    fread (t.text, t.end - t.start + 1, 1, ctx->stream);
    
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
            printf("@%u#%u:%u<%u>%u:%u %s\n", t.index, t.start, t.end, t.type, t.column, t.row, t.text);
        } else {
            printf("@%u#%u:%u<%u>%u:%u\n", t.index, t.start, t.end, t.type, t.column, t.row);
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
    unsigned int start = ctx->offset;
    unsigned int startRow = ctx->row;

    match(ctx, '"');

    // '"' (~["\r\n] | '\\' . ) * '"'
    while (!(lookahead(ctx, '"') || lookahead(ctx, '\r') || lookahead(ctx, '\r') || lookahead(ctx, EOF))) {
        matchIfExist(ctx, '\\');
        nextChar(ctx);
    }

    match(ctx, '"');

    insertToken(ctx, (struct Token) {
        .type = T_STRING,
        .index = ctx->tokenLength,
        .start = start,
        .end = ctx->offset - 1,
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
        } else if (lookahead(ctx, '}')) {
            getTokenRPAIR(ctx);
        } else if (lookahead(ctx, '[')) {
            getTokenLARRAY(ctx);
        } else if (lookahead(ctx, ']')) {
            getTokenRARRAY(ctx);
        } else if (lookahead(ctx, ',')) {
            getTokenCOMMA(ctx);
        } else if (lookahead(ctx, ':')) {
            getTokenCOLON(ctx);
        } else if (lookahead(ctx, '"')) {
            getTokenSTRING(ctx);
        } else if (lookahead(ctx, '-') || lookahead(ctx, '0') || lookahead(ctx, '1') || lookahead(ctx, '2') || lookahead(ctx, '3') || lookahead(ctx, '4') ||
                    lookahead(ctx, '5') || lookahead(ctx, '6') || lookahead(ctx, '7') || lookahead(ctx, '8') || lookahead(ctx, '9')) {
            getTokenNUMBER(ctx);
        } else if (lookahead(ctx, 't')) {
            getTokenTRUE(ctx);
        } else if (lookahead(ctx, 'f')) {
            getTokenFALSE(ctx);
        } else if (lookahead(ctx, 'n')) {
            getTokenNULL(ctx);
        } else if (matchIfExist(ctx, '\n')) { // only match "\n", should consider these cases "\r\n" "\r"
            ctx->column++;
            ctx->row = 1;
        } else if (matchIfExist(ctx, ' ') || matchIfExist(ctx, '\t')) {
            continue;
        } else {
            match(ctx, EOF); // error
        }
    }
}

void nextToken(struct ParserContext *ctx) {
    ctx->tokenIndex++;
}

void matchToken(struct ParserContext *ctx, enum LexerToken t) {
    int index = ctx->tokenIndex;

    // always get next token before checking
    nextToken(ctx);

    if (index < 0 || index >= ctx->tokenLength) {
        fprintf(stderr, "Error Token Index: %d\n", index);
        print_trace();
        assert(0);
    }

    if (ctx->tokenList[index].type != t) {
        fprintf(stderr, "Syntax Error: Unexpected Token: <%d> expect <%d>\n", ctx->tokenList[index].type, t);
        print_trace();
        assert(0);
    }
}

enum LexerToken LAToken(struct ParserContext *ctx, int n) {
    if (ctx->tokenIndex + n < 0) {
        return MISSING;
    }

    int LAIndex = ctx->tokenIndex + n;

    return ctx->tokenList[LAIndex].type;
}

bool checknLAToken(struct ParserContext *ctx, int n, enum LexerToken t) {
    return LAToken(ctx, n) == t;
}

bool checkLAToken(struct ParserContext *ctx, enum LexerToken t) {
    return LAToken(ctx, 0) == t;
}

void objRule(struct ParserContext *ctx);
void arrRule(struct ParserContext *ctx);

void valueRule(struct ParserContext *ctx) {
    // value : obj | arr | STRING | NUMBER | 'true' | 'false' | 'null' ;
    if (checkLAToken(ctx, T_LPAIR)) {
        objRule(ctx);
    } else if (checkLAToken(ctx, T_LARRAY)) {
        arrRule(ctx);
    } else if (checkLAToken(ctx, T_STRING)) {
        matchToken(ctx, T_STRING);
    } else if (checkLAToken(ctx, T_NUMBER)) {
        matchToken(ctx, T_NUMBER);
    } else if (checkLAToken(ctx, T_TRUE)) {
        matchToken(ctx, T_TRUE);
    } else if (checkLAToken(ctx, T_FALSE)) {
        matchToken(ctx, T_FALSE);
    } else if (checkLAToken(ctx, T_NULL)) {
        matchToken(ctx, T_NULL);
    } else {
        print_trace();
        assert(0);
        // Syntax Error: Unexpected Token
    }
}

void pairRule(struct ParserContext *ctx) {
    // member : STRING ':' value ;

    printf("key %d:%d\n", ctx->tokenList[ctx->tokenIndex].column, ctx->tokenList[ctx->tokenIndex].row);

    matchToken(ctx, T_STRING);
    matchToken(ctx, T_COLON);
    valueRule(ctx);
}

void objRule(struct ParserContext *ctx) {
    // obj : T_LPAIR pair (',' pair)* T_RPAIR | T_LPAIR T_RPAIR;
    matchToken(ctx, T_LPAIR);

    while (!checkLAToken(ctx, T_RPAIR)) {
        pairRule(ctx);
        if (checkLAToken(ctx, T_COMMA)) {
            matchToken(ctx, T_COMMA);
        }
    }

    matchToken(ctx, T_RPAIR);
}

void arrRule(struct ParserContext *ctx) {
    // arr : T_LARRAY value* T_RARRAY ;
    matchToken(ctx, T_LARRAY);

    while (!checkLAToken(ctx, T_RARRAY)) {
        valueRule(ctx);

        if (checkLAToken(ctx, T_COMMA)) {
            matchToken(ctx, T_COMMA);
        }
    }

    matchToken(ctx, T_RARRAY);
}

void jsonRule(struct ParserContext *ctx) {
    // json : value EOF;
    valueRule(ctx);
    
    // should add EOF token and match it here
}

void jsonParser(struct ParserContext *ctx) {
    jsonRule(ctx);
}

int main(int argc, char *argv[]) {

    struct LexerContext lexer_ctx = {
        .tokenLength = 0,
        .tokenCapacity = 0,
        .tokenList = NULL,
        .currentChar = '\0',
        .offset = 0,
        .column = 1,
        .row = 0,
        .stream = stdin,
    };

    if (argc >= 2) {
        lexer_ctx.stream = fopen(argv[1], "r");
    }

    jsonLexer(&lexer_ctx);

    struct ParserContext parser_ctx = {
        .tokenIndex = 0,
        .tokenLength = lexer_ctx.tokenLength,
        .tokenList = lexer_ctx.tokenList,
    };

    printToken(&lexer_ctx);

    jsonParser(&parser_ctx);

    return 0;
}