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
