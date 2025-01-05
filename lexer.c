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

enum TokenType {
    T_MISSING, T_STRING, T_NUMBER, T_COMMA, T_COLON, T_LPAIR, T_RPAIR, T_LARRAY, T_RARRAY, T_TRUE, T_FALSE, T_NULL, VALUE, PAIR, OBJ, ARR, JSON
};

struct Token {
    enum TokenType type;
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
    unsigned int oldTokenSize = 0;
    unsigned int newTokenSize = 0;
    int error = 0;

    // set default token list
    if (!ctx->tokenList) {
        ctx->tokenList = (struct Token *)malloc(sizeof(struct Token) * DEFAULT_TOKEN_CAPACITY); 
        ctx->tokenCapacity = DEFAULT_TOKEN_CAPACITY;
    }

    // double token list if full, time complexity O(3n)
    if (ctx->tokenLength >= ctx->tokenCapacity) {
        oldTokenSize = sizeof(struct Token) * ctx->tokenCapacity;
        newTokenSize = oldTokenSize * 2;

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

struct ParserContext;
struct objToken objRule(struct ParserContext *ctx);
struct arrToken arrRule(struct ParserContext *ctx);

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

void printTreeNode(struct ParserContext *ctx, struct BaseToken *token) {
    static int indent = 0;

    switch (token->type) {
        case T_STRING:
        case T_NUMBER:
        case T_TRUE:
        case T_FALSE:
        case T_NULL:
        {
            struct Token *t = (struct Token *)token;
            printf("%s", t->text);
            break;
        }
        case VALUE:
        {
            struct valueToken *value = (struct valueToken *)token;
            printTreeNode(ctx, &value->next);
            break;
        }
        case PAIR:
        {
            struct pairToken *pair = (struct pairToken *)token;
            printTreeNode(ctx, (struct BaseToken *)&pair->key);
            printf(": ");
            printTreeNode(ctx, (struct BaseToken *)&pair->value);
            break;
        }
        case ARR:
        {
            struct arrToken *arr = (struct arrToken *)token;
            printf("[\n");
            indent += 4;
            for (int i = 0; i < indent; i++)
                printf(" ");
            for (int i = 0; i < arr->valueLength; i++) {
                printTreeNode(ctx, (struct BaseToken *)&arr->valueList[i]);
            }
            printf("\n");
            indent -= 4;
            for (int i = 0; i < indent; i++)
                printf(" ");
            printf("]");
            break;
        }
        case OBJ:
        {
            struct objToken *obj = (struct objToken *)token;
            printf("{\n");
            indent += 4;
            for (int i = 0; i < indent; i++)
                printf(" ");
            for (int i = 0; i < obj->pairLength; i++) {
                printTreeNode(ctx, (struct BaseToken *)&obj->pairList[i]);
                if (i + 1 < obj->pairLength) {
                    printf(",\n");
                    for (int i = 0; i < indent; i++)
                        printf(" ");
                }
            }
            printf("\n");
            indent -= 4;
            for (int i = 0; i < indent; i++)
                printf(" ");
            printf("}");
            break;
        }
        case JSON:
        {
            struct jsonToken *json = (struct jsonToken *)token;
            printTreeNode(ctx, (struct BaseToken *)&json->value);
            break;
        }
        case T_MISSING:
        {
            printf("MISSING\n");
            break;
        }
        default:
        {
            fprintf(stderr, "Error: Print Token Not found <%d>\n", token->type);
            print_trace();
            assert(0);
        }
    }
}

void printTree(struct ParserContext *ctx) {
    printTreeNode(ctx, (struct BaseToken *)&ctx->json);
}

void nextToken(struct ParserContext *ctx) {
    ctx->tokenIndex++;
}

void matchToken(struct ParserContext *ctx, enum TokenType t) {
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

enum TokenType LAToken(struct ParserContext *ctx, int n) {
    if (ctx->tokenIndex + n < 0) {
        return T_MISSING;
    }

    int LAIndex = ctx->tokenIndex + n;

    return ctx->tokenList[LAIndex].type;
}

bool checknLAToken(struct ParserContext *ctx, int n, enum TokenType t) {
    return LAToken(ctx, n) == t;
}

bool checkLAToken(struct ParserContext *ctx, enum TokenType t) {
    return LAToken(ctx, 0) == t;
}


void insertPairToken(struct ParserContext *ctx, struct objToken *obj, struct pairToken pair) {
    struct pairToken *oldPairList = NULL;
    unsigned int oldPairSize = 0;
    unsigned int newPairSize = 0;

    // set default token list
    if (!obj->pairList) {
        obj->pairList = (struct pairToken *)malloc(sizeof(struct pairToken)); 
        obj->pairCapacity = 1;
    }

    // double token list if full, time complexity O(3n)
    if (obj->pairLength >= obj->pairCapacity) {
        oldPairSize = sizeof(struct pairToken) * obj->pairCapacity;
        newPairSize = oldPairSize * 2;

        oldPairList = obj->pairList;
        obj->pairList = (struct pairToken *)malloc(newPairSize);
        memcpy(obj->pairList, oldPairList, oldPairSize);

        free(oldPairList);
        oldPairList = NULL;

        obj->pairCapacity *= 2;
    }

    obj->pairList[obj->pairLength++] = pair;
}


void insertValueToken(struct ParserContext *ctx, struct arrToken *arr, struct valueToken value) {
    struct valueToken *oldValueList = NULL;
    unsigned int oldValueSize = 0;
    unsigned int newValueSize = 0;

    // set default token list
    if (!arr->valueList) {
        arr->valueList = (struct valueToken *)malloc(sizeof(struct valueToken)); 
        arr->valueCapacity = 1;
    }

    // double token list if full, time complexity O(3n)
    if (arr->valueLength >= arr->valueCapacity) {
        oldValueSize = sizeof(struct valueToken) * arr->valueCapacity;
        newValueSize = oldValueSize * 2;

        oldValueList = arr->valueList;
        arr->valueList = (struct valueToken *)malloc(newValueSize);
        memcpy(arr->valueList, oldValueList, oldValueSize);

        free(oldValueList);
        oldValueList = NULL;

        arr->valueCapacity *= 2;
    }

    arr->valueList[arr->valueLength++] = value;
}

struct valueToken valueRule(struct ParserContext *ctx) {
    struct valueToken value = {
        .type = VALUE,
    };

    // value : obj | arr | STRING | NUMBER | 'true' | 'false' | 'null' ;
    if (checkLAToken(ctx, T_LPAIR)) {
        value.obj = objRule(ctx);
    } else if (checkLAToken(ctx, T_LARRAY)) {
        value.arr = arrRule(ctx);
    } else if (checkLAToken(ctx, T_STRING)) {
        value.stringToken = ctx->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_STRING);
    } else if (checkLAToken(ctx, T_NUMBER)) {
        value.numberToken = ctx->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_NUMBER);
    } else if (checkLAToken(ctx, T_TRUE)) {
        value.trueToken = ctx->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_TRUE);
    } else if (checkLAToken(ctx, T_FALSE)) {
        value.falseToken = ctx->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_FALSE);
    } else if (checkLAToken(ctx, T_NULL)) {
        value.nullToken = ctx->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_NULL);
    } else {
        print_trace();
        assert(0);
        // Syntax Error: Unexpected Token
    }

    return value;
}

struct pairToken pairRule(struct ParserContext *ctx) {

    printf("@%d key %d:%d %s\n", ctx->tokenIndex, ctx->tokenList[ctx->tokenIndex].column, ctx->tokenList[ctx->tokenIndex].row, ctx->tokenList[ctx->tokenIndex].text);

    struct pairToken pair = {
        .type = PAIR,
        .key = ctx->tokenList[ctx->tokenIndex],
    };

    // member : STRING ':' value ;
    matchToken(ctx, T_STRING);
    matchToken(ctx, T_COLON);

    pair.value = valueRule(ctx);

    return pair;
}

struct objToken objRule(struct ParserContext *ctx) {
    struct pairToken pair = {};
    struct objToken obj = {
        .type = OBJ,
    };

    // obj : T_LPAIR pair (',' pair)* T_RPAIR | T_LPAIR T_RPAIR;
    matchToken(ctx, T_LPAIR);

    while (!checkLAToken(ctx, T_RPAIR)) {
        pair = pairRule(ctx);

        insertPairToken(ctx, &obj, pair);

        if (checkLAToken(ctx, T_COMMA)) {
            matchToken(ctx, T_COMMA);
        }
    }

    matchToken(ctx, T_RPAIR);

    return obj;
}

struct arrToken arrRule(struct ParserContext *ctx) {
    struct valueToken value = {};
    struct arrToken arr = {
        .type = ARR,
    };

    // arr : T_LARRAY value* T_RARRAY ;
    matchToken(ctx, T_LARRAY);

    while (!checkLAToken(ctx, T_RARRAY)) {
        value = valueRule(ctx);

        insertValueToken(ctx, &arr, value);

        if (checkLAToken(ctx, T_COMMA)) {
            matchToken(ctx, T_COMMA);
        }
    }

    matchToken(ctx, T_RARRAY);

    return arr;
}

struct jsonToken jsonRule(struct ParserContext *ctx) {
    struct jsonToken json = {
        .type = JSON,
    };

    // json : value EOF;
    json.value = valueRule(ctx);
    
    // should add EOF token and match it here

    return json;
}

void jsonParser(struct ParserContext *ctx) {
    ctx->json = jsonRule(ctx);
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

    printTree(&parser_ctx);

    return 0;
}