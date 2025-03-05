#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "jsonEditor.h"
#include "token.h"

int __json_obj_set(struct json_obj_t *obj, const char *key, union json_value_t value, bool copy_value) {
    union json_value_t *exist_value = __json_obj_get(*obj, key);

    if (exist_value) {
        __json_clean_up(exist_value);
        *exist_value = copy_value ? __json_dup_nonterm_tok(value) : value;
        return 1;
    }

    insertPair(obj, (struct json_pair_t) {
        .key = (char*)strdup(key),
        .value = copy_value ? __json_dup_nonterm_tok(value) : value,
    });

    return 0;
}

int __json_obj_set_obj(struct json_obj_t *obj, const char *key, struct json_obj_t value) {
    return __json_obj_set(obj, key, (union json_value_t)value, true);
}

int __json_obj_set_obj_p(struct json_obj_t *obj, const char *key, struct json_obj_t *value) {
    return __json_obj_set(obj, key, (union json_value_t)*value, false);
}

int __json_obj_set_arr(struct json_obj_t *obj, const char *key, struct json_arr_t value) {
    return __json_obj_set(obj, key, (union json_value_t)value, true);
}

int __json_obj_set_arr_p(struct json_obj_t *obj, const char *key, struct json_arr_t *value) {
    return __json_obj_set(obj, key, (union json_value_t)*value, false);
}

int __json_obj_set_token(struct json_obj_t *obj, const char *key, struct json_tok_t value) {
    if (value.type == T_EMPTY) {
        __json_obj_delete(obj, key);
        return 0;
    }

    bool copy_value = value.type == T_STRING;
    return __json_obj_set(obj, key, (union json_value_t)value, copy_value);
}

int __json_obj_set_value(struct json_obj_t *obj, const char *key, union json_value_t value) {
    return __json_obj_set(obj, key, value, true);
}

int __json_obj_set_str(struct json_obj_t *obj, const char *key, const char *value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_STR((char*)value), true);
}

int __json_obj_set_bool(struct json_obj_t *obj, const char *key, bool value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_BOOL(value), false);
}

int __json_obj_set_i8(struct json_obj_t *obj, const char *key, int8_t value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_INT8(value), false);
}

int __json_obj_set_i16(struct json_obj_t *obj, const char *key, int16_t value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_INT16(value), false);
}

int __json_obj_set_i32(struct json_obj_t *obj, const char *key, int32_t value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_INT32(value), false);
}

int __json_obj_set_i64(struct json_obj_t *obj, const char *key, int64_t value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_INT64(value), false);
}

int __json_obj_set_u8(struct json_obj_t *obj, const char *key, uint8_t value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_UINT8(value), false);
}

int __json_obj_set_u16(struct json_obj_t *obj, const char *key, uint16_t value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_UINT16(value), false);
}

int __json_obj_set_u32(struct json_obj_t *obj, const char *key, uint32_t value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_UINT32(value), false);
}

int __json_obj_set_u64(struct json_obj_t *obj, const char *key, uint64_t value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_UINT64(value), false);
}

int __json_obj_set_f32(struct json_obj_t *obj, const char *key, float value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_FLOAT(value), false);
}

int __json_obj_set_f64(struct json_obj_t *obj, const char *key, double value) {
    return __json_obj_set(obj, key, (union json_value_t)JSON_DOUBLE(value), false);
}
