#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stddef.h>
enum TokenType {
    T_MISSING, T_STRING, T_NUMBER, T_COMMA, T_COLON, T_LPAIR, T_RPAIR, T_LARRAY, T_RARRAY, T_TRUE, T_FALSE, T_NULL, VALUE, PAIR, OBJ, ARR, JSON, TOKEN_SIZE
};

struct Token {
    enum TokenType type;
    struct TokenContainer *container;
    unsigned int index;
    unsigned int start;
    unsigned int end;
    unsigned int column;
    unsigned int row;
    char *text;
};

struct BaseToken {
    enum TokenType type;
    struct TokenContainer *container;
};

struct objToken {
    enum TokenType type;
    struct TokenContainer *container;
    struct pairs *pairs;
};

struct arrToken {
    enum TokenType type;
    struct TokenContainer *container;
    struct values *values;
};

struct valueToken {
    enum TokenType type;
    struct TokenContainer *container;
    union {
        struct BaseToken next;
        struct Token anyToken;
        struct Token stringToken;
        struct Token numberToken;
        struct Token trueToken;
        struct Token falseToken;
        struct Token nullToken;
        struct objToken obj;
        struct arrToken arr;
    };
};

struct values {
    size_t length;
    size_t capacity;
    struct valueToken list[];
};

struct pairToken {
    enum TokenType type;
    struct TokenContainer *container;
    struct Token key;
    struct valueToken value;
};

struct pairs {
    size_t length;
    size_t capacity;
    struct pairToken list[];
};

struct jsonToken {
    enum TokenType type;
    struct TokenContainer *container;
    struct valueToken value;
};

struct TokenContainer {
    unsigned int tokenLength;
    unsigned int tokenCapacity;
    struct Token *tokenList;
    struct jsonToken root;
};

const char *type2str(enum TokenType type);
struct TokenContainer *initTokenContainer();
void freeTokenContainer(struct TokenContainer *container);

void insertToken(struct TokenContainer *container, struct Token t);
void insertPair(struct objToken *obj, struct pairToken pair);
void insertValue(struct arrToken *arr, struct valueToken value);

struct Token copyToken(struct Token t);

#endif