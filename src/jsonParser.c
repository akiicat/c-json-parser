#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "jsonLexer.h"
#include "jsonParser.h"
#include "jsonEditor.h"
#include "debug.h"
#include "token.h"

struct objToken objRule(struct ParserContext *ctx);
struct arrToken arrRule(struct ParserContext *ctx);

struct ParserContext *initJsonParser(struct LexerContext *lexer_ctx) {
    struct ParserContext *parser_ctx = malloc(sizeof(struct ParserContext));

    *parser_ctx = (struct ParserContext) {
        .tokenIndex = 0,
        .lexer = lexer_ctx,
    };

    return parser_ctx;
}

void freeJsonParser(struct ParserContext *parser_ctx) {
    free(parser_ctx);
}

// void printTreeNode(struct ParserContext *ctx, union valueToken *token) {
//     static int indent = 0;

//     switch (token->type) {
//         case T_STRING:
//         {
//             struct Token *t = (struct Token *)token;
//             printf("\"%s\"", t->text);
//             break;
//         }
//         case T_NUMBER:
//         case T_TRUE:
//         case T_FALSE:
//         case T_NULL:
//         {
//             struct Token *t = (struct Token *)token;
//             printf("%s", t->text);
//             break;
//         }
//         case ARRAY:
//         {
//             struct arrToken *arr = (struct arrToken *)token;
//             printf("[\n");
//             indent += 4;
//             for (int i = 0; i < indent; i++)
//                 printf(" ");
//             if (arr->values) {
//                 for (int i = 0; i < arr->values->length; i++) {
//                     printTreeNode(ctx, &arr->values->list[i]);
//                 }
//             }
//             printf("\n");
//             indent -= 4;
//             for (int i = 0; i < indent; i++)
//                 printf(" ");
//             printf("]");
//             break;
//         }
//         case OBJECT:
//         {
//             struct objToken *obj = (struct objToken *)token;
//             printf("{\n");
//             indent += 4;
//             for (int i = 0; i < indent; i++)
//                 printf(" ");
//             if (obj->pairs) {
//                 for (int i = 0; i < obj->pairs->length; i++) {
//                     struct pairToken *pair = (struct pairToken *)token;
//                     printf("\"%s\": ", obj->pairs->list[i].key);
//                     printTreeNode(ctx, &pair->value);
//                     if (i + 1 < obj->pairs->length) {
//                         printf(",\n");
//                         for (int i = 0; i < indent; i++)
//                             printf(" ");
//                     }
//                 }
//             }
//             printf("\n");
//             indent -= 4;
//             for (int i = 0; i < indent; i++)
//                 printf(" ");
//             printf("}");
//             break;
//         }
//         case T_MISSING:
//         {
//             printf("MISSING\n");
//             break;
//         }
//         default:
//         {
//             fprintf(stderr, "Error: Print Token Not found <%d>\n", token->type);
//             print_trace();
//             assert(0);
//         }
//     }
// }

// void printTree(struct ParserContext *ctx) {
//     printTreeNode(ctx, &ctx->root);
// }

void nextToken(struct ParserContext *ctx) {
    ctx->tokenIndex++;
}

void matchToken(struct ParserContext *ctx, enum LexerTokenType t) {
    int index = ctx->tokenIndex;

    // always get next token before checking
    nextToken(ctx);

    if (index < 0 || index >= ctx->lexer->tokens.length) {
        fprintf(stderr, "Error Token Index: %d expect <%d>\n", index, t);
        print_trace();
        assert(0);
    }

    if (ctx->lexer->tokens.list[index].type != t) {
        fprintf(stderr, "Syntax Error: Unexpected Token: <%d> expect <%d>\n", ctx->lexer->tokens.list[index].type, t);
        print_trace();
        assert(0);
    }
}

enum LexerTokenType LAToken(struct ParserContext *ctx, int n) {
    unsigned int LAIndex = ctx->tokenIndex + n;

    if (LAIndex >= ctx->lexer->tokens.length) {
        return LT_MISSING;
    }

    return ctx->lexer->tokens.list[LAIndex].type;
}

bool checknLAToken(struct ParserContext *ctx, int n, enum LexerTokenType t) {
    return LAToken(ctx, n) == t;
}

bool checkLAToken(struct ParserContext *ctx, enum LexerTokenType t) {
    return ctx->lexer->tokens.list[ctx->tokenIndex].type == t;
}

union valueToken valueRule(struct ParserContext *ctx) {
    union valueToken value = {
        .type = VALUE,
    };

    // value : obj | arr | STRING | NUMBER | 'true' | 'false' | 'null' ;
    if (checkLAToken(ctx, LT_LPAIR)) {
        value.obj = objRule(ctx);
    }
    else if (checkLAToken(ctx, LT_LARRAY)) {
        value.arr = arrRule(ctx);
    }
    else if (checkLAToken(ctx, LT_STRING)) {
        value.stringToken = ctx->lexer->tokens.list[ctx->tokenIndex];
        matchToken(ctx, LT_STRING);
    }
    else if (checkLAToken(ctx, LT_NUMBER)) {
        value.numberToken = ctx->lexer->tokens.list[ctx->tokenIndex];
        matchToken(ctx, LT_NUMBER);
    }
    else if (checkLAToken(ctx, LT_TRUE)) {
        value.trueToken = ctx->lexer->tokens.list[ctx->tokenIndex];
        matchToken(ctx, LT_TRUE);
    }
    else if (checkLAToken(ctx, LT_FALSE)) {
        value.falseToken = ctx->lexer->tokens.list[ctx->tokenIndex];
        matchToken(ctx, LT_FALSE);
    }
    else if (checkLAToken(ctx, LT_NULL)) {
        value.nullToken = ctx->lexer->tokens.list[ctx->tokenIndex];
        matchToken(ctx, LT_NULL);
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
        .key = ctx->lexer->tokens.list[ctx->tokenIndex].text,
    };

    // member : STRING ':' value ;
    matchToken(ctx, LT_STRING);
    matchToken(ctx, LT_COLON);

    pair.value = valueRule(ctx);

    return pair;
}

struct objToken objRule(struct ParserContext *ctx) {
    struct pairToken pair = {};
    struct objToken obj = {
        .type = OBJECT,
    };

    struct objToken *aobj = createObj();

    // obj : T_LPAIR pair (',' pair)* T_RPAIR | T_LPAIR T_RPAIR;
    matchToken(ctx, LT_LPAIR);

    while (!checkLAToken(ctx, LT_RPAIR)) {
        pair = pairRule(ctx);

        objInsert(aobj, pair.key, pair.value);

        insertPair(&obj, pair);

        if (checkLAToken(ctx, LT_COMMA)) {
            matchToken(ctx, LT_COMMA);
        }
    }

    matchToken(ctx, LT_RPAIR);

    return obj;
}

struct arrToken arrRule(struct ParserContext *ctx) {
    union valueToken value = {};
    struct arrToken arr = {
        .type = ARRAY,
    };

    // arr : T_LARRAY value* T_RARRAY ;
    matchToken(ctx, LT_LARRAY);

    while (!checkLAToken(ctx, LT_RARRAY)) {
        value = valueRule(ctx);

        insertValue(&arr, value);

        if (checkLAToken(ctx, LT_COMMA)) {
            matchToken(ctx, LT_COMMA);
        }
    }

    matchToken(ctx, LT_RARRAY);

    return arr;
}

union valueToken *jsonParser(struct ParserContext *ctx) {
    // json : value EOF;
    ctx->root = valueRule(ctx);
    return &ctx->root;
}

// void freeParserNode(struct ParserContext *ctx, struct BaseToken *token) {
//     switch (token->type) {
//         case T_STRING:
//         case T_NUMBER:
//         case T_COMMA:
//         case T_COLON:
//         case T_LPAIR:
//         case T_RPAIR:
//         case T_LARRAY:
//         case T_RARRAY:
//         case T_TRUE:
//         case T_FALSE:
//         case T_NULL:
//             break;
//         case VALUE:
//         {
//             union valueToken *value = (union valueToken *)token;
//             freeParserNode(ctx, &value->next);
//             break;
//         }
//         case PAIR:
//         {
//             struct pairToken *pair = (struct pairToken *)token;
//             freeParserNode(ctx, &pair->value.next);
//             break;
//         }
//         case ARR:
//         {
//             struct arrToken *arr = (struct arrToken *)token;
//             if (arr->values) {
//                 for (int i = 0; i < arr->values->length; i++) {
//                     freeParserNode(ctx, (struct BaseToken *)&arr->values->list[i]);
//                 }
//                 free(arr->values);
//                 arr->values = NULL;
//             }
//             break;
//         }
//         case OBJ:
//         {
//             struct objToken *obj = (struct objToken *)token;
//             if (obj->pairs) {
//                 for (int i = 0; i < obj->pairs->length; i++) {
//                     freeParserNode(ctx, (struct BaseToken *)&obj->pairs->list[i]);
//                 }
//                 free(obj->pairs);
//                 obj->pairs = NULL;
//             }
//             break;
//         }
//         case JSON:
//         {
//             struct jsonToken *json = (struct jsonToken *)token;
//             freeParserNode(ctx, &json->value.next);
//             break;
//         }
//         case T_MISSING:
//         {
//             printf("MISSING\n");
//             break;
//         }
//         default:
//         {
//             fprintf(stderr, "Error: Print Token Not found <%d>\n", token->type);
//             print_trace();
//             assert(0);
//         }
//     }
// }

// void freeParser(struct ParserContext *ctx) {
//     freeParserNode(ctx, (struct BaseToken *)&ctx->container->root);
// }