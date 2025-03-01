#ifndef __JSON_EDITOR_H__
#define __JSON_EDITOR_H__

#include <stdbool.h>
#include <stddef.h>

#include "token.h"

#define JSON_STR(s) ((struct anyValue) { .type = T_STRING, .str = s })
#define JSON_NUM(s) ({ char *buffer = (char *)alloca(32); itoa(s,buffer,10); (struct anyValue) { .type = T_NUMBER, .str = buffer }; })
#define JSON_ARR ((struct anyValue) { .type = ARR })

#define dupToken(X) _Generic((X),           \
    struct Token:        dupNonTerminalToken((union valueToken)X).anyToken,           \
    struct arrToken:     dupNonTerminalToken((union valueToken)X).arr,                \
    struct objToken:     dupNonTerminalToken((union valueToken)X).obj,                \
    default:             dupNonTerminalToken(X)                                       \
)

struct objToken *createObj();
struct objToken *dupObj(struct objToken *obj);
void freeObj(struct objToken *obj);

int objInsert(struct objToken *obj, const char *key, union valueToken value);
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
