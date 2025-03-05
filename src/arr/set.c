#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "jsonEditor.h"
#include "token.h"

int __arr_set(struct json_arr_t *arr, size_t i, union json_value_t value, bool copy_value) {
    union json_value_t *exist_value = __json_arr_get(*arr, i);

    if (exist_value) {
        __json_clean_up(exist_value);
        *exist_value = copy_value ? __json_dup_nonterm_tok(value) : value;
        return 1;
    }

    return 0;
}

int __arr_set_obj(struct json_arr_t *arr, size_t i, struct json_obj_t value) {
    return __arr_set(arr, i, (union json_value_t)value, true);
}

int __arr_set_obj_p(struct json_arr_t *arr, size_t i, struct json_obj_t *value) {
    return __arr_set(arr, i, (union json_value_t)*value, false);
}

int __arr_set_arr(struct json_arr_t *arr, size_t i, struct json_arr_t value) {
    return __arr_set(arr, i, (union json_value_t)value, true);
}

int __arr_set_arr_p(struct json_arr_t *arr, size_t i, struct json_arr_t *value) {
    return __arr_set(arr, i, (union json_value_t)*value, false);
}

int __arr_set_token(struct json_arr_t *arr, size_t i, struct json_tok_t value) {
    if (value.type == T_EMPTY) {
        // __arr_delete(arr, i);
        return 0;
    }

    bool copy_value = value.type == T_STRING;
    return __arr_set(arr, i, (union json_value_t)value, copy_value);
}

int __arr_set_value(struct json_arr_t *arr, size_t i, union json_value_t value) {
    return __arr_set(arr, i, value, true);
}

int __arr_set_str(struct json_arr_t *arr, size_t i, const char *value) {
    return __arr_set(arr, i, (union json_value_t)JSON_STR((char*)value), true);
}

int __arr_set_bool(struct json_arr_t *arr, size_t i, bool value) {
    return __arr_set(arr, i, (union json_value_t)JSON_BOOL(value), false);
}

int __arr_set_i8(struct json_arr_t *arr, size_t i, int8_t value) {
    return __arr_set(arr, i, (union json_value_t)JSON_INT8(value), false);
}

int __arr_set_i16(struct json_arr_t *arr, size_t i, int16_t value) {
    return __arr_set(arr, i, (union json_value_t)JSON_INT16(value), false);
}

int __arr_set_i32(struct json_arr_t *arr, size_t i, int32_t value) {
    return __arr_set(arr, i, (union json_value_t)JSON_INT32(value), false);
}

int __arr_set_i64(struct json_arr_t *arr, size_t i, int64_t value) {
    return __arr_set(arr, i, (union json_value_t)JSON_INT64(value), false);
}

int __arr_set_u8(struct json_arr_t *arr, size_t i, uint8_t value) {
    return __arr_set(arr, i, (union json_value_t)JSON_UINT8(value), false);
}

int __arr_set_u16(struct json_arr_t *arr, size_t i, uint16_t value) {
    return __arr_set(arr, i, (union json_value_t)JSON_UINT16(value), false);
}

int __arr_set_u32(struct json_arr_t *arr, size_t i, uint32_t value) {
    return __arr_set(arr, i, (union json_value_t)JSON_UINT32(value), false);
}

int __arr_set_u64(struct json_arr_t *arr, size_t i, uint64_t value) {
    return __arr_set(arr, i, (union json_value_t)JSON_UINT64(value), false);
}

int __arr_set_f32(struct json_arr_t *arr, size_t i, float value) {
    return __arr_set(arr, i, (union json_value_t)JSON_FLOAT(value), false);
}

int __arr_set_f64(struct json_arr_t *arr, size_t i, double value) {
    return __arr_set(arr, i, (union json_value_t)JSON_DOUBLE(value), false);
}
