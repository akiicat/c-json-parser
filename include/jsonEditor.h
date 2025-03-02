#ifndef __JSON_EDITOR_H__
#define __JSON_EDITOR_H__

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#include "token.h"

#define JSON_STR(s) ((union valueToken) { .anyToken = { .type = T_STRING, .text = s } })
#define JSON_NUM(s) ((union valueToken) { .anyToken = { .type = T_NUMBER, .text = s } })
#define JSON_INT8(s) ((union valueToken) { .anyToken = { .type = T_INT8, .i8 = s } })
#define JSON_INT16(s) ((union valueToken) { .anyToken = { .type = T_INT16, .i16 = s } })
#define JSON_INT32(s) ((union valueToken) { .anyToken = { .type = T_INT32, .i32 = s } })
#define JSON_INT64(s) ((union valueToken) { .anyToken = { .type = T_INT64, .i64 = s } })
#define JSON_UINT8(s) ((union valueToken) { .anyToken = { .type = T_UINT8, .u8 = s } })
#define JSON_UINT16(s) ((union valueToken) { .anyToken = { .type = T_UINT16, .u16 = s } })
#define JSON_UINT32(s) ((union valueToken) { .anyToken = { .type = T_UINT32, .u32 = s } })
#define JSON_UINT64(s) ((union valueToken) { .anyToken = { .type = T_UINT64, .u64 = s } })

#define dupToken(X) _Generic((X),           \
    struct Token:        dupNonTerminalToken((union valueToken)X).anyToken,           \
    struct arrToken:     dupNonTerminalToken((union valueToken)X).arr,                \
    struct objToken:     dupNonTerminalToken((union valueToken)X).obj,                \
    default:             dupNonTerminalToken((union valueToken)X)                     \
)

struct objToken *createObj();
struct objToken *dupObj(struct objToken *obj);
void freeObj(struct objToken *obj);

#define objInsert(obj, key, value)                            \
    _Generic((key),                                           \
        const char *: _Generic((value),                       \
            const char *:     _objInsertStr,        \
            char *:           _objInsertStr,        \
            bool:             _objInsertBool,       \
            default:          _objInsert            \
        ),                                                    \
        char *: _Generic((value),                       \
            const char *:     _objInsertStr,        \
            char *:           _objInsertStr,        \
            bool:             _objInsertBool,       \
            default:          _objInsert            \
        ),                                                    \
        union valueToken:     _objInsert                      \
    )((obj), (key), (value))

int _objInsertStr(struct objToken *obj, const char *key, const char *value);
int _objInsertBool(struct objToken *obj, const char *key, bool value);

int _objInsert(struct objToken *obj, union valueToken key, union valueToken value);
// bool objUpdate(struct objToken *obj, const char *key, struct valueToken value);
void objDelete(struct objToken *obj, const char *key);
union valueToken *objFind(struct objToken *obj, const char *key);


struct arrToken *createArr();
struct arrToken *dupArr(struct arrToken *arr);
void freeArr(struct arrToken *arr);

bool arrInsert(struct arrToken *arr, union valueToken value);
// bool arrUpdate(struct arrToken *arr, const char *key, struct valueToken value);
bool arrDelete(struct arrToken *arr, size_t index);
union valueToken arrFind(struct arrToken *arr, size_t index);

void printJson(union valueToken value);

// bool insertPair(jc.root.obj, "str1", JSON_STR("value"));
// bool insertPair(jc.root.obj, "num1", JSON_NUM(0));
// bool insertPair(jc.root.obj, "arr1", JSON_ARR);
// bool updatePair(jc.root.obj, "str1", JSON_STR("xyz"));
// bool deletePair(jc.root.obj, "xxx");
// struct anyValue getPairValue(jc.root.obj, "xxx");
// objKeys(jc.root.obj)
// objValues(jc.root.obj)
// iterObj(jc.root.obj, void (key, value) {})
// copy

#endif
