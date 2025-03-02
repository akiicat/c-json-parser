#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stddef.h>
#include <stdint.h>

enum TokenType {
    T_MISSING, T_STRING, T_NUMBER,
    
    T_INT8,
    T_INT16,
    T_INT32,
    T_INT64,
    T_INT = T_INT64,

    T_UINT8,
    T_UINT16,
    T_UINT32,
    T_UINT64,
    T_UINT = T_UINT64,

    T_FLOAT,
    T_DOUBLE,

    T_COMMA, T_COLON, T_LPAIR, T_RPAIR, T_LARRAY, T_RARRAY, T_TRUE, T_FALSE, T_NULL, VALUE, PAIR, OBJ, ARR, JSON, TOKEN_SIZE
};

struct Token {
    enum TokenType type;
    char *text;
    union {
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float f32;
        double f64;
        __float128 f128; // long double
    };
    unsigned int index;
    unsigned int start;
    unsigned int end;
    unsigned int column;
    unsigned int row;
};

struct BaseToken {
    enum TokenType type;
};

struct objToken {
    enum TokenType type;
    struct pairs *pairs;
};

struct arrToken {
    enum TokenType type;
    struct values *values;
};

// struct valueToken {
union valueToken {
    enum TokenType type;
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
// };

struct values {
    size_t length;
    size_t capacity;
    union valueToken list[];
};

struct pairToken {
    enum TokenType type;
    struct Token key;
    union valueToken value;
};

struct pairs {
    size_t length;
    size_t capacity;
    struct pairToken list[];
};

struct jsonToken {
    enum TokenType type;
    union valueToken value;
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
void insertValue(struct arrToken *arr, union valueToken value);

struct Token dupTerminalToken(const struct Token t);
union valueToken dupNonTerminalToken(union valueToken t);

#endif