#ifndef __TOKEN_H__
#define __TOKEN_H__

enum TokenType {
    T_MISSING, T_STRING, T_NUMBER, T_COMMA, T_COLON, T_LPAIR, T_RPAIR, T_LARRAY, T_RARRAY, T_TRUE, T_FALSE, T_NULL, VALUE, PAIR, OBJ, ARR, JSON, TOKEN_SIZE
};

struct Token {
    enum TokenType type;
    unsigned int index;
    unsigned int start;
    unsigned int end;
    unsigned int column;
    unsigned int row;
    char *text;
};

struct BaseToken {
    enum TokenType type;
};

struct objToken {
    enum TokenType type;
    int pairLength;
    int pairCapacity;
    struct pairToken *pairList;
};

struct arrToken {
    enum TokenType type;
    int valueLength;
    int valueCapacity;
    struct valueToken *valueList;
};

struct valueToken {
    enum TokenType type;
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

struct pairToken {
    enum TokenType type;
    struct Token key;
    struct valueToken value;
};

struct jsonToken {
    enum TokenType type;
    struct valueToken value;
};

const char *type2str(enum TokenType type);

#endif