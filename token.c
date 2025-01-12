#include <stddef.h>
#include <stdlib.h>

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
