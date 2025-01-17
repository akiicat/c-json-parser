#include <stddef.h>

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
