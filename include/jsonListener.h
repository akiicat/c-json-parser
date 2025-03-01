#ifndef __JSON_LISTENER_H__
#define __JSON_LISTENER_H__

#include "token.h"
#include "jsonParser.h"
#include <stddef.h>

struct WalkerContext;

typedef void (*anyListenerCallback) (struct WalkerContext *ctx, struct BaseToken *token);

typedef void (*jsonListenerCallback) (struct WalkerContext *ctx, struct jsonToken *token);
typedef void (*objListenerCallback) (struct WalkerContext *ctx, struct objToken *token);
typedef void (*arrListenerCallback) (struct WalkerContext *ctx, struct arrToken *token);
typedef void (*valueListenerCallback) (struct WalkerContext *ctx, union valueToken *token);
typedef void (*pairListenerCallback) (struct WalkerContext *ctx, struct pairToken *token);

struct Listener {
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

struct WalkerContext {
    struct Listener listener;
    struct BaseToken *entry;
    struct BaseToken **path;
    unsigned int pathLength;
    unsigned int pathCapacity;
};

void jsonWalker(struct WalkerContext *ctx);
void freeWalker(struct WalkerContext *ctx);

#endif