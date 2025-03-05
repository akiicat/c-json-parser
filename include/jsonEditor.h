#ifndef __JSON_EDITOR_H__
#define __JSON_EDITOR_H__

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#include "token.h"

typedef struct json_tok_t json_tok_t;
typedef struct json_arr_t json_arr_t;
typedef struct json_obj_t json_obj_t;
typedef union json_value_t json_val_t;

#define JSON_ARRAY ((struct json_arr_t) { .type = ARRAY })
#define JSON_OBJECT ((struct json_obj_t) { .type = OBJECT })
#define JSON_NULL ((struct json_tok_t) { .type = T_NULL })
#define JSON_EMPTY ((struct json_tok_t) { .type = T_EMPTY })

#define JSON_STR(s) ((struct json_tok_t) { .type = T_STRING, .text = s })
#define JSON_NUM(s) ((struct json_tok_t) { .type = T_NUMBER, .text = s })
#define JSON_BOOL(s) ((struct json_tok_t) { .type = T_BOOL, .boolean = s })

#define JSON_INT(s) ((struct json_tok_t) { .type = T_INT, .i64 = s })
#define JSON_UINT(s) ((struct json_tok_t) { .type = T_UINT, .u64 = s })

#define JSON_INT8(s) ((struct json_tok_t) { .type = T_INT8, .i8 = s })
#define JSON_INT16(s) ((struct json_tok_t) { .type = T_INT16, .i16 = s })
#define JSON_INT32(s) ((struct json_tok_t) { .type = T_INT32, .i32 = s })
#define JSON_INT64(s) ((struct json_tok_t) { .type = T_INT64, .i64 = s })
#define JSON_UINT8(s) ((struct json_tok_t) { .type = T_UINT8, .u8 = s })
#define JSON_UINT16(s) ((struct json_tok_t) { .type = T_UINT16, .u16 = s })
#define JSON_UINT32(s) ((struct json_tok_t) { .type = T_UINT32, .u32 = s })
#define JSON_UINT64(s) ((struct json_tok_t) { .type = T_UINT64, .u64 = s })

#define JSON_FLOAT(s) ((struct json_tok_t) { .type = T_FLOAT, .f32 = s })
#define JSON_DOUBLE(s) ((struct json_tok_t) { .type = T_DOUBLE, .f64 = s })

#define json_dup(X) _Generic((X),           \
    struct json_tok_t:     __json_dup_nonterm_tok((union json_value_t)X).tok,           \
    struct json_arr_t:     __json_dup_nonterm_tok((union json_value_t)X).arr,                \
    struct json_obj_t:     __json_dup_nonterm_tok((union json_value_t)X).obj,                \
    default:                 __json_dup_nonterm_tok(X)                     \
)
#define json_obj_set(obj, key, value)                   \
    _Generic((value),                              \
        const char *:        __json_obj_set_str,        \
        char *:              __json_obj_set_str,        \
        bool:                __json_obj_set_bool,       \
        int8_t:              __json_obj_set_i8,         \
        int16_t:             __json_obj_set_i16,        \
        int32_t:             __json_obj_set_i32,        \
        int64_t:             __json_obj_set_i64,        \
        uint8_t:             __json_obj_set_u8,         \
        uint16_t:            __json_obj_set_u16,        \
        uint32_t:            __json_obj_set_u32,        \
        uint64_t:            __json_obj_set_u64,        \
        float:               __json_obj_set_f32,        \
        double:              __json_obj_set_f64,        \
        struct json_obj_t:     __json_obj_set_obj,        \
        struct json_obj_t *:   __json_obj_set_obj_p,      \
        struct json_arr_t:     __json_obj_set_arr,        \
        struct json_arr_t *:   __json_obj_set_arr_p,      \
        struct json_tok_t:        __json_obj_set_token,      \
        default:             __json_obj_set_value       \
    )((obj), (key), (value)) \

// obj pairs pointer may update, it must to define as pointer without const
// the __json_obj_t value and __json_obj_t *value there is no different, it is use pointer to distinguish copy and move
int __json_obj_set_obj(struct json_obj_t *obj, const char *key, struct json_obj_t value);
int __json_obj_set_obj_p(struct json_obj_t *obj, const char *key, struct json_obj_t *value);
int __json_obj_set_arr(struct json_obj_t *obj, const char *key, struct json_arr_t value);
int __json_obj_set_arr_p(struct json_obj_t *obj, const char *key, struct json_arr_t *value);
int __json_obj_set_str(struct json_obj_t *obj, const char *key, const char *value);
int __json_obj_set_bool(struct json_obj_t *obj, const char *key, bool value);
int __json_obj_set_null(struct json_obj_t *obj, const char *key, void *value);
int __json_obj_set_i8(struct json_obj_t *obj, const char *key, int8_t value);
int __json_obj_set_i16(struct json_obj_t *obj, const char *key, int16_t value);
int __json_obj_set_i32(struct json_obj_t *obj, const char *key, int32_t value);
int __json_obj_set_i64(struct json_obj_t *obj, const char *key, int64_t value);
int __json_obj_set_u8(struct json_obj_t *obj, const char *key, uint8_t value);
int __json_obj_set_u16(struct json_obj_t *obj, const char *key, uint16_t value);
int __json_obj_set_u32(struct json_obj_t *obj, const char *key, uint32_t value);
int __json_obj_set_u64(struct json_obj_t *obj, const char *key, uint64_t value);
int __json_obj_set_token(struct json_obj_t *obj, const char *key, struct json_tok_t value);
int __json_obj_set_value(struct json_obj_t *obj, const char *key, union json_value_t value);
int __json_obj_set_f32(struct json_obj_t *obj, const char *key, float value);
int __json_obj_set_f64(struct json_obj_t *obj, const char *key, double value);

#define json_obj_get(a,b) __json_obj_get(a,b)
union json_value_t *__json_obj_get(struct json_obj_t obj, const char *key);

#define json_obj_delete(a,b) __json_obj_delete(a,b)
void __json_obj_delete(struct json_obj_t *obj, const char *key);

// #define json_obj_remove(a,b) __json_obj_remove(a,b)
// void __json_obj_remove(struct json_obj_t *obj, union json_value_t value);

#define json_obj_create(a) __json_obj_create(a)
struct json_obj_t __json_obj_create(size_t capacity);

void printJson(union json_value_t value);

#define json_arr_get(a,b) __json_arr_get(a,b)
union json_value_t *__json_arr_get(struct json_arr_t arr, size_t i);

// void json_dump(char *s, size_t n);

#define json_clean_up(a) ({__json_clean_up((union json_value_t*)a);})
void __json_clean_up(union json_value_t *value);

const char *type2str(enum json_type_t type);

void insertPair(struct json_obj_t *obj, struct json_pair_t pair);
void insertValue(struct json_arr_t *arr, union json_value_t value);

struct json_tok_t __json_dup_term_tok(const struct json_tok_t t);
union json_value_t __json_dup_nonterm_tok(union json_value_t t);


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
