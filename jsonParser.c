#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "jsonParser.h"
#include "debug.h"

struct objToken objRule(struct ParserContext *ctx);
struct arrToken arrRule(struct ParserContext *ctx);

void printTreeNode(struct ParserContext *ctx, struct BaseToken *token) {
    static int indent = 0;

    switch (token->type) {
        case T_STRING:
        {
            struct Token *t = (struct Token *)token;
            printf("\"%s\"", t->text);
            break;
        }
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
            if (arr->values) {
                for (int i = 0; i < arr->values->length; i++) {
                    printTreeNode(ctx, (struct BaseToken *)&arr->values->list[i]);
                }
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
            if (obj->pairs) {
                for (int i = 0; i < obj->pairs->length; i++) {
                    printTreeNode(ctx, (struct BaseToken *)&obj->pairs->list[i]);
                    if (i + 1 < obj->pairs->length) {
                        printf(",\n");
                        for (int i = 0; i < indent; i++)
                            printf(" ");
                    }
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
    printTreeNode(ctx, (struct BaseToken *)&ctx->container->root);
}

void nextToken(struct ParserContext *ctx) {
    ctx->tokenIndex++;
}

void matchToken(struct ParserContext *ctx, enum TokenType t) {
    int index = ctx->tokenIndex;

    // always get next token before checking
    nextToken(ctx);

    if (index < 0 || index >= ctx->container->tokenLength) {
        fprintf(stderr, "Error Token Index: %d expect <%d>\n", index, t);
        print_trace();
        assert(0);
    }

    if (ctx->container->tokenList[index].type != t) {
        fprintf(stderr, "Syntax Error: Unexpected Token: <%d> expect <%d>\n", ctx->container->tokenList[index].type, t);
        print_trace();
        assert(0);
    }
}

enum TokenType LAToken(struct ParserContext *ctx, int n) {
    unsigned int LAIndex = ctx->tokenIndex + n;

    if (LAIndex >= ctx->container->tokenLength) {
        return T_MISSING;
    }

    return ctx->container->tokenList[LAIndex].type;
}

bool checknLAToken(struct ParserContext *ctx, int n, enum TokenType t) {
    return LAToken(ctx, n) == t;
}

bool checkLAToken(struct ParserContext *ctx, enum TokenType t) {
    return LAToken(ctx, 0) == t;
}

struct valueToken valueRule(struct ParserContext *ctx) {
    struct valueToken value = {
        .type = VALUE,
        .container = ctx->container,
    };

    // value : obj | arr | STRING | NUMBER | 'true' | 'false' | 'null' ;
    if (checkLAToken(ctx, T_LPAIR)) {
        value.obj = objRule(ctx);
    }
    else if (checkLAToken(ctx, T_LARRAY)) {
        value.arr = arrRule(ctx);
    }
    else if (checkLAToken(ctx, T_STRING)) {
        value.stringToken = ctx->container->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_STRING);
    }
    else if (checkLAToken(ctx, T_NUMBER)) {
        value.numberToken = ctx->container->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_NUMBER);
    }
    else if (checkLAToken(ctx, T_TRUE)) {
        value.trueToken = ctx->container->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_TRUE);
    }
    else if (checkLAToken(ctx, T_FALSE)) {
        value.falseToken = ctx->container->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_FALSE);
    }
    else if (checkLAToken(ctx, T_NULL)) {
        value.nullToken = ctx->container->tokenList[ctx->tokenIndex];
        matchToken(ctx, T_NULL);
    }
    else {
        print_trace();
        assert(0);
        // Syntax Error: Unexpected Token
    }

    return value;
}

struct pairToken pairRule(struct ParserContext *ctx) {
    struct pairToken pair = {
        .type = PAIR,
        .container = ctx->container,
        .key = ctx->container->tokenList[ctx->tokenIndex],
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
        .container = ctx->container,
    };

    // obj : T_LPAIR pair (',' pair)* T_RPAIR | T_LPAIR T_RPAIR;
    matchToken(ctx, T_LPAIR);

    while (!checkLAToken(ctx, T_RPAIR)) {
        pair = pairRule(ctx);

        insertPair(&obj, pair);

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
        .container = ctx->container,
    };

    // arr : T_LARRAY value* T_RARRAY ;
    matchToken(ctx, T_LARRAY);

    while (!checkLAToken(ctx, T_RARRAY)) {
        value = valueRule(ctx);

        insertValue(&arr, value);

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
        .container = ctx->container,
    };

    // json : value EOF;
    json.value = valueRule(ctx);
    
    // should add EOF token and match it here

    return json;
}

void jsonParser(struct ParserContext *ctx) {
    ctx->container->root = jsonRule(ctx);
}

void freeParserNode(struct ParserContext *ctx, struct BaseToken *token) {
    switch (token->type) {
        case T_STRING:
        case T_NUMBER:
        case T_COMMA:
        case T_COLON:
        case T_LPAIR:
        case T_RPAIR:
        case T_LARRAY:
        case T_RARRAY:
        case T_TRUE:
        case T_FALSE:
        case T_NULL:
            break;
        case VALUE:
        {
            struct valueToken *value = (struct valueToken *)token;
            freeParserNode(ctx, &value->next);
            break;
        }
        case PAIR:
        {
            struct pairToken *pair = (struct pairToken *)token;
            freeParserNode(ctx, &pair->value.next);
            break;
        }
        case ARR:
        {
            struct arrToken *arr = (struct arrToken *)token;
            if (arr->values) {
                for (int i = 0; i < arr->values->length; i++) {
                    freeParserNode(ctx, (struct BaseToken *)&arr->values->list[i]);
                }
                free(arr->values);
                arr->values = NULL;
            }
            break;
        }
        case OBJ:
        {
            struct objToken *obj = (struct objToken *)token;
            if (obj->pairs) {
                for (int i = 0; i < obj->pairs->length; i++) {
                    freeParserNode(ctx, (struct BaseToken *)&obj->pairs->list[i]);
                }
                free(obj->pairs);
                obj->pairs = NULL;
            }
            break;
        }
        case JSON:
        {
            struct jsonToken *json = (struct jsonToken *)token;
            freeParserNode(ctx, &json->value.next);
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

void freeParser(struct ParserContext *ctx) {
    freeParserNode(ctx, (struct BaseToken *)&ctx->container->root);
}