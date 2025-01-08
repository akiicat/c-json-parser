#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "debug.h"
#include "jsonListener.h"
#include "token.h"

void pushPathToken(struct jsonWalkerContext *ctx, struct BaseToken *token) {
    struct BaseToken **oldPath = NULL;
    unsigned int oldPathSize = 0;
    unsigned int newPathSize = 0;

    // set default token list
    if (!ctx->path) {
        ctx->path = (struct BaseToken **)malloc(sizeof(struct BaseToken *) * 1); 
        ctx->pathCapacity = 1;
    }

    // double token list if full, time complexity O(3n)
    if (ctx->pathLength >= ctx->pathCapacity) {
        oldPathSize = sizeof(struct BaseToken *) * ctx->pathCapacity;
        newPathSize = oldPathSize * 2;

        oldPath = ctx->path;
        ctx->path = (struct BaseToken **)malloc(newPathSize);
        memcpy(ctx->path, oldPath, oldPathSize);

        free(oldPath);
        oldPath = NULL;

        ctx->pathCapacity *= 2;
    }

    ctx->path[ctx->pathLength++] = token;
}

struct BaseToken *popPathToken(struct jsonWalkerContext *ctx) {
    struct BaseToken **oldPath = NULL;
    unsigned int oldPathSize = 0;
    unsigned int newPathSize = 0;
    struct BaseToken *token = NULL;
    unsigned int thrthreshold = 0;

    if (ctx->pathLength == 0) {
        return NULL;
    }

    token = ctx->path[--ctx->pathLength];

    if (__builtin_umul_overflow(ctx->pathLength, 4, &thrthreshold)) {
        thrthreshold = UINT_MAX;
    }

    // privent freqently allocate. e.g. pathLength(pathCapacity) : 1(2) -> 2(4) -> 1(2) -> 2(4) -> ..
    #define MIN_SHRINK_CAPACITY_SIZE 64
    if (thrthreshold <= ctx->pathCapacity && ctx->pathCapacity > MIN_SHRINK_CAPACITY_SIZE) {
        oldPathSize = sizeof(struct BaseToken *) * ctx->pathCapacity;
        newPathSize = oldPathSize / 2;

        oldPath = ctx->path;
        ctx->path = (struct BaseToken **)malloc(newPathSize);
        memcpy(ctx->path, oldPath, newPathSize);

        free(oldPath);
        oldPath = NULL;

        ctx->pathCapacity /= 2;
    }

    return token;
}

void jsonWalkerNode(struct jsonWalkerContext *ctx, struct BaseToken *token) {
    struct jsonListener *listener = &ctx->listener;

    pushPathToken(ctx, token);

    switch (token->type) {
        case T_STRING:
        case T_NUMBER:
        case T_TRUE:
        case T_FALSE:
        case T_NULL: {
            break;
        }
        case VALUE:
        {
            struct valueToken *value = (struct valueToken *)token;
            if (listener->enterValue)
                listener->enterValue(ctx, value);
            jsonWalkerNode(ctx, &value->next);
            if (listener->exitValue)
                listener->exitValue(ctx, value);
            break;
        }
        case PAIR:
        {
            struct pairToken *pair = (struct pairToken *)token;
            if (listener->enterPair)
                listener->enterPair(ctx, pair);
            jsonWalkerNode(ctx, (struct BaseToken *)&pair->key);
            jsonWalkerNode(ctx, (struct BaseToken *)&pair->value);
            if (listener->exitPair)
                listener->exitPair(ctx, pair);
            break;
        }
        case ARR:
        {
            struct arrToken *arr = (struct arrToken *)token;
            if (listener->enterArr)
                listener->enterArr(ctx, arr);
            for (int i = 0; i < arr->valueLength; i++) {
                jsonWalkerNode(ctx, (struct BaseToken *)&arr->valueList[i]);
            }
            if (listener->exitArr)
                listener->exitArr(ctx, arr);
            break;
        }
        case OBJ:
        {
            struct objToken *obj = (struct objToken *)token;
            if (listener->enterObj)
                listener->enterObj(ctx, obj);
            for (int i = 0; i < obj->pairLength; i++) {
                jsonWalkerNode(ctx, (struct BaseToken *)&obj->pairList[i]);
            }
            if (listener->exitObj)
                listener->exitObj(ctx, obj);
            break;
        }
        case JSON:
        {
            struct jsonToken *json = (struct jsonToken *)token;
            if (listener->enterJson)
                listener->enterJson(ctx, json);
            jsonWalkerNode(ctx, (struct BaseToken *)&json->value);
            if (listener->exitJson)
                listener->exitJson(ctx, json);
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

    popPathToken(ctx);
}

void jsonWalker(struct jsonWalkerContext *ctx) {
    jsonWalkerNode(ctx, ctx->entry);
}