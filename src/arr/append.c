#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "jsonEditor.h"
#include "token.h"

int __json_arr_append(struct json_arr_t *arr, union json_value_t value, bool copy_value) {
    insertValue(arr, copy_value ? __json_dup_nonterm_tok(value) : value);
    return 0;
}

int __json_arr_append_obj(struct json_arr_t *arr, size_t i, struct json_obj_t value) {
    return __json_arr_append(arr, (union json_value_t)value, true);
}

int __json_arr_append_obj_p(struct json_arr_t *arr, size_t i, struct json_obj_t *value) {
    return __json_arr_append(arr, (union json_value_t)*value, false);
}

int __json_arr_append_arr(struct json_arr_t *arr, size_t i, struct json_arr_t value) {
    return __json_arr_append(arr, (union json_value_t)value, true);
}

int __json_arr_append_arr_p(struct json_arr_t *arr, size_t i, struct json_arr_t *value) {
    return __json_arr_append(arr, (union json_value_t)*value, false);
}

int __json_arr_append_token(struct json_arr_t *arr, size_t i, struct json_tok_t value) {
    if (value.type == T_EMPTY) {
        return 0;
    }

    bool copy_value = value.type == T_STRING;
    return __json_arr_append(arr, (union json_value_t)value, copy_value);
}

int __json_arr_append_value(struct json_arr_t *arr, size_t i, union json_value_t value) {
    return __json_arr_append(arr, value, true);
}

int __json_arr_append_str(struct json_arr_t *arr, size_t i, const char *value) {
    unsigned long l = strlen(value);
    char *dup = (char *)malloc(l+1);
    memcpy(dup, value, l);
    dup[l] = '\0';
    return __json_arr_append(arr, (union json_value_t)JSON_STR(dup), false);
}

int __json_arr_append_bool(struct json_arr_t *arr, size_t i, bool value) {
    return __json_arr_append(arr, (union json_value_t)JSON_BOOL(value), false);
}

int __json_arr_append_i8(struct json_arr_t *arr, size_t i, int8_t value) {
    return __json_arr_append(arr, (union json_value_t)JSON_INT8(value), false);
}

int __json_arr_append_i16(struct json_arr_t *arr, size_t i, int16_t value) {
    return __json_arr_append(arr, (union json_value_t)JSON_INT16(value), false);
}

int __json_arr_append_i32(struct json_arr_t *arr, size_t i, int32_t value) {
    return __json_arr_append(arr, (union json_value_t)JSON_INT32(value), false);
}

int __json_arr_append_i64(struct json_arr_t *arr, size_t i, int64_t value) {
    return __json_arr_append(arr, (union json_value_t)JSON_INT64(value), false);
}

int __json_arr_append_u8(struct json_arr_t *arr, size_t i, uint8_t value) {
    return __json_arr_append(arr, (union json_value_t)JSON_UINT8(value), false);
}

int __json_arr_append_u16(struct json_arr_t *arr, size_t i, uint16_t value) {
    return __json_arr_append(arr, (union json_value_t)JSON_UINT16(value), false);
}

int __json_arr_append_u32(struct json_arr_t *arr, size_t i, uint32_t value) {
    return __json_arr_append(arr, (union json_value_t)JSON_UINT32(value), false);
}

int __json_arr_append_u64(struct json_arr_t *arr, size_t i, uint64_t value) {
    return __json_arr_append(arr, (union json_value_t)JSON_UINT64(value), false);
}

int __json_arr_append_f32(struct json_arr_t *arr, size_t i, float value) {
    return __json_arr_append(arr, (union json_value_t)JSON_FLOAT(value), false);
}

int __json_arr_append_f64(struct json_arr_t *arr, size_t i, double value) {
    return __json_arr_append(arr, (union json_value_t)JSON_DOUBLE(value), false);
}
