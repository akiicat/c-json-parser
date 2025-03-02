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
    "T_INT8",
    "T_INT16",
    "T_INT32",
    "T_INT64",
    "T_UINT8",
    "T_UINT16",
    "T_UINT32",
    "T_UINT64",
    "T_FLOAT",
    "T_DOUBLE",
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
    if (!container) {
        return;
    }

    // Store internal pointers and count before freeing the container itself
    struct Token *tokenList = container->tokenList;
    unsigned int tokenLength = container->tokenLength;

    // Free the container structure first since tokenList was allocated separately
    free(container);

    // If tokenList is NULL, there's nothing more to free
    if (tokenList == NULL) {
        return;
    }

    // Free each token's text in reverse order
    for (unsigned int i = tokenLength; i > 0; i--) {
        free(tokenList[i - 1].text);
        tokenList[i - 1].text = NULL;  // Defensive: nullify pointer after free
    }

    // Free the token list array itself
    free(tokenList);
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

    obj->pairs->list[obj->pairs->length++] = pair;
}

void insertValue(struct arrToken *arr, union valueToken value) {
    struct values *oldList = NULL;
    struct values *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    // set default token list
    if (!arr->values) {
        arr->values = (struct values *)malloc(sizeof(struct values) + sizeof(union valueToken));
        arr->values->length = 0;
        arr->values->capacity = 1;
    }

    oldList = arr->values;
    oldSize = sizeof(struct values) + sizeof(union valueToken) * arr->values->capacity;

    // double token list if full, time complexity O(3n)
    if (oldList->length >= oldList->capacity) {
        newSize = sizeof(struct values) + sizeof(union valueToken) * oldList->capacity * 2;
        newList = (struct values *)malloc(newSize);

        memcpy(newList, oldList, oldSize);
        free(oldList);
        oldList = NULL;

        arr->values = newList;
        arr->values->capacity *= 2;
    }

    arr->values->list[arr->values->length++] = value;
}

struct Token dupTerminalToken(struct Token t) {
    size_t textSize = 0;
    char *text = NULL;

    if (t.text) {
        textSize = strlen(t.text);
        text = (char *)malloc(textSize + 1);

        strncpy(text, t.text, textSize);
        text[textSize] = '\0';

        t.text = text;
    }

    return t;
}

union valueToken dupNonTerminalToken(union valueToken t) {
    union valueToken res;

    switch (t.type) {
    case T_STRING:
    case T_NUMBER:
    case T_TRUE:
    case T_FALSE:
    case T_NULL:
    {
        res.anyToken = dupTerminalToken(t.anyToken);
        break;
    }
    case ARR:
    {
        res.arr = (struct arrToken) {
            .type = ARR,
        };

        if (t.arr.values) {
            for (int i = 0; i < t.arr.values->length; i++) {
                insertValue(&res.arr, dupNonTerminalToken(t.arr.values->list[i]));
            }
        }
        break;
    }
    case OBJ:
    {
        res.obj = (struct objToken) {
            .type = OBJ,
        };

        if (t.obj.pairs) {
            for (int i = 0; i < t.obj.pairs->length; i++) {
                struct pairToken pair = {
                    .type = PAIR,
                    .key = dupTerminalToken(t.obj.pairs->list[i].key),
                    .value = dupNonTerminalToken(t.obj.pairs->list[i].value),
                };
                insertPair(&res.obj, pair);
            }
        }
        break;
    }
    default:
        fprintf(stderr, "dupToken: unsupport token <%d|%s>", t.type, type2str(t.type));
        break;
    }

    return res;
}