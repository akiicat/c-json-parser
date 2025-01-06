#ifndef __TOKEN_H__
#define __TOKEN_H__

enum TokenType {
    T_MISSING, T_STRING, T_NUMBER, T_COMMA, T_COLON, T_LPAIR, T_RPAIR, T_LARRAY, T_RARRAY, T_TRUE, T_FALSE, T_NULL, VALUE, PAIR, OBJ, ARR, JSON
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

#endif