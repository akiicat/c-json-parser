#ifndef __JSON_LISTENER_H__
#define __JSON_LISTENER_H__

#include "token.h"
#include "jsonParser.h"
#include <stddef.h>

struct jsonWalkerContext;

typedef void (*anyListenerCallback) (struct jsonWalkerContext *ctx, struct BaseToken *token);

typedef void (*jsonListenerCallback) (struct jsonWalkerContext *ctx, struct jsonToken *token);
typedef void (*objListenerCallback) (struct jsonWalkerContext *ctx, struct objToken *token);
typedef void (*arrListenerCallback) (struct jsonWalkerContext *ctx, struct arrToken *token);
typedef void (*valueListenerCallback) (struct jsonWalkerContext *ctx, struct valueToken *token);
typedef void (*pairListenerCallback) (struct jsonWalkerContext *ctx, struct pairToken *token);

struct jsonListener {
    jsonListenerCallback enterJson;
    jsonListenerCallback exitJson;

    objListenerCallback enterObj;
    objListenerCallback exitObj;
    
    arrListenerCallback enterArr;
    arrListenerCallback exitArr;

    valueListenerCallback enterValue;
    valueListenerCallback exitValue;

    pairListenerCallback enterPair;
    pairListenerCallback exitPair;
};

struct jsonWalkerContext {
    struct jsonListener listener;
    struct BaseToken *entry;
    struct BaseToken **path;
    unsigned int pathLength;
    unsigned int pathCapacity;
};

void jsonWalker(struct jsonWalkerContext *ctx);

#endif