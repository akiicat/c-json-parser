#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "token.h"

const char *typeString[TOKEN_SIZE] = {
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
    "VALUE",
    "PAIR",
    "OBJ",
    "ARR",
    "JSON",
};

const char *type2str(enum TokenType type) {
    if (type >= T_MISSING && type < TOKEN_SIZE) {
        return typeString[type];
    }

    return NULL;
}

struct TokenContainer *initTokenContainer() {
    struct TokenContainer *container = (struct TokenContainer *)malloc(sizeof(struct TokenContainer));

    *container = (struct TokenContainer) {
        .tokenLength = 0,
        .tokenCapacity = 0,
        .tokenList = NULL,
    };

    return container;
}

void freeTokenContainer(struct TokenContainer *container) {
    char *text = NULL;

    if (!container->tokenList) {
        return;
    }

    while (container->tokenLength > 0) {
        container->tokenLength--;

        text = container->tokenList[container->tokenLength].text;
        container->tokenList[container->tokenLength].text = NULL;

        if (text) {
            free(text);
            text = NULL;
        }
    }

    free(container->tokenList);
}

void insertToken(struct TokenContainer *container, struct Token t) {
    struct Token *oldList = NULL;
    struct Token *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    if (!container) {
        fprintf(stderr, "Error Token Container is not initialized\n");
        print_trace();
        assert(0);
    }

    // set default token list
    if (!container->tokenList) {
        container->tokenCapacity = 1;
        container->tokenList = (struct Token *)malloc(sizeof(struct Token) * container->tokenCapacity); 
    }

    oldSize = sizeof(struct Token) * container->tokenCapacity;
    oldList = container->tokenList;

    // double token list if full, time complexity O(3n)
    if (container->tokenLength >= container->tokenCapacity) {
        newSize = sizeof(struct Token) * container->tokenCapacity * 2;
        newList = (struct Token *)malloc(newSize);

        memcpy(newList, oldList, oldSize);
        free(oldList);
        oldList = NULL;

        container->tokenList = newList;
        container->tokenCapacity *= 2;
    }

    t.container = container;
    t.index = container->tokenLength;

    container->tokenList[container->tokenLength] = t;
    container->tokenLength++;
}

void insertPair(struct objToken *obj, struct pairToken pair) {
    struct pairs *oldList = NULL;
    struct pairs *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    // set default token list
    if (!obj->pairs) {
        obj->pairs = (struct pairs *)malloc(sizeof(struct pairs) + sizeof(struct pairToken)); 
        obj->pairs->length = 0;
        obj->pairs->capacity = 1;
    }

    oldList = obj->pairs;
    oldSize = sizeof(struct pairs) + sizeof(struct pairToken) * obj->pairs->capacity;

    // double token list if full, time complexity O(3n)
    if (oldList->length >= oldList->capacity) {
        newSize = sizeof(struct pairs) + sizeof(struct pairToken) * oldList->capacity * 2;
        newList = (struct pairs *)malloc(newSize);

        memcpy(newList, oldList, oldSize);
        free(oldList);
        oldList = NULL;

        obj->pairs = newList;
        obj->pairs->capacity *= 2;
    }

    pair.container = obj->container;
    obj->pairs->list[obj->pairs->length++] = pair;
}

void insertValue(struct arrToken *arr, struct valueToken value) {
    struct values *oldList = NULL;
    struct values *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    // set default token list
    if (!arr->values) {
        arr->values = (struct values *)malloc(sizeof(struct values) + sizeof(struct valueToken));
        arr->values->length = 0;
        arr->values->capacity = 1;
    }

    oldList = arr->values;
    oldSize = sizeof(struct values) + sizeof(struct valueToken) * arr->values->capacity;

    // double token list if full, time complexity O(3n)
    if (oldList->length >= oldList->capacity) {
        newSize = sizeof(struct values) + sizeof(struct valueToken) * oldList->capacity * 2;
        newList = (struct values *)malloc(newSize);

        memcpy(newList, oldList, oldSize);
        free(oldList);
        oldList = NULL;

        arr->values = newList;
        arr->values->capacity *= 2;
    }

    value.container = arr->container;
    arr->values->list[arr->values->length++] = value;
}

struct Token copyToken(struct Token t) {
    size_t textSize = 0;
    char *text = NULL;

    textSize = strlen(t.text);
    text = (char *)malloc(textSize + 1);

    strncpy(text, t.text, textSize);
    text[textSize] = '\0';

    t.text = text;

    return t;
}
