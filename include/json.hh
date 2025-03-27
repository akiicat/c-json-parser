
#ifndef __JSON_HH__
#define __JSON_HH__

#include <cstddef>
#include <cstdint>
#include <type_traits>

extern "C" {
#include "json.h"
}

#define json_dumps(j, ...) __json_dumps((j), {__VA_ARGS__})
#define json_dump(j, f, ...) __json_dump((j), (f), {__VA_ARGS__})
#define json_pprint(j, ...) __json_pprint((j), {__VA_ARGS__})

constexpr union json_t JSON_MISSING = {.type = JT_MISSING};
constexpr union json_t JSON_DELETE = {.type = JT_MISSING};
constexpr union json_t JSON_ARRAY = {.type = JT_ARRAY};
constexpr union json_t JSON_OBJECT = {.type = JT_OBJECT};
constexpr union json_t JSON_NULL = {.type = JT_NULL};
constexpr union json_t JSON_TRUE = {.tok = {.type = JT_BOOL, .text = nullptr, .boolean = true}};
constexpr union json_t JSON_FALSE = {.tok = {.type = JT_BOOL, .text = nullptr, .boolean = false}};
constexpr union json_t JSON_TYPE(enum json_token_type_t x) {
    return {.tok = {.type = x, .text = nullptr}};
} constexpr union json_t JSON_STRING(const char *x) {
    return {.tok = {.type = JT_STRING, .text = const_cast<char *>(x)}};
} constexpr union json_t JSON_NUMBER(const char *x) {
    return {.tok = {.type = JT_NUMBER, .text = const_cast<char *>(x)}};
} constexpr union json_t JSON_BOOL(bool x) {
    return {.tok = {.type = JT_BOOL, .text = nullptr, .boolean = (x)}};
} constexpr union json_t JSON_INT(int64_t x) {
    return {.tok = {.type = JT_INT, .text = nullptr, .i64 = (int64_t)(x)}};
} constexpr union json_t JSON_UINT(uint64_t x) {
    return {.tok = {.type = JT_UINT, .text = nullptr, .u64 = (uint64_t)(x)}};
} constexpr union json_t JSON_FLOAT(double x) {
    return {.tok = {.type = JT_FLOAT, .text = nullptr, .f = (double)(x)}};
}

static inline bool
json_set(union json_t *j, const char *key, const char *value) {
    return json_set_obj_str(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, char *value) { return json_set_obj_str(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, bool value) { return json_set_obj_bool(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, void *value) { return json_set_obj_null(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, int8_t value) { return json_set_obj_i8(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, int16_t value) { return json_set_obj_i16(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, int32_t value) { return json_set_obj_i32(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, int64_t value) { return json_set_obj_i64(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, uint8_t value) { return json_set_obj_u8(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, uint16_t value) {
    return json_set_obj_u16(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, uint32_t value) {
    return json_set_obj_u32(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, uint64_t value) {
    return json_set_obj_u64(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, float value) { return json_set_obj_f32(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, double value) { return json_set_obj_f64(j, key, value); }
static inline bool json_set(union json_t *j, const char *key, union json_t value) {
    return json_set_obj_value(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, union json_t *value) {
    return json_set_obj_value_p(j, key, value);
}

// Overload for const char* values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, const char *value) {
    return json_set_arr_str(j, i, value);
}

// Overload for bool values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, bool value) {
    return json_set_arr_bool(j, i, value);
}

// Overload for void* values (typically used for setting null)
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, void *value) {
    return json_set_arr_null(j, i, value);
}

// Overload for int8_t values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, int8_t value) {
    return json_set_arr_i8(j, i, value);
}

// Overload for int16_t values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, int16_t value) {
    return json_set_arr_i16(j, i, value);
}

// Overload for int32_t values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, int32_t value) {
    return json_set_arr_i32(j, i, value);
}

// Overload for int64_t values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, int64_t value) {
    return json_set_arr_i64(j, i, value);
}

// Overload for uint8_t values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, uint8_t value) {
    return json_set_arr_u8(j, i, value);
}

// Overload for uint16_t values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, uint16_t value) {
    return json_set_arr_u16(j, i, value);
}

// Overload for uint32_t values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, uint32_t value) {
    return json_set_arr_u32(j, i, value);
}

// Overload for uint64_t values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, uint64_t value) {
    return json_set_arr_u64(j, i, value);
}

// Overload for float values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, float value) {
    return json_set_arr_f32(j, i, value);
}

// Overload for double values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, double value) {
    return json_set_arr_f64(j, i, value);
}

// Overload for union json_t values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, union json_t value) {
    return json_set_arr_value(j, i, value);
}

// Overload for union json_t* values
template <typename Index, typename = std::enable_if_t<std::is_integral_v<Index>>>
static inline bool json_set(union json_t *j, Index i, union json_t *value) {
    return json_set_arr_value_p(j, i, value);
}

static inline bool json_append(union json_t *j, const char *value) { return json_append_str(j, value); }
static inline bool json_append(union json_t *j, char *value) { return json_append_str(j, value); }
static inline bool json_append(union json_t *j, bool value) { return json_append_bool(j, value); }
static inline bool json_append(union json_t *j, void *value) { return json_append_null(j, value); }
static inline bool json_append(union json_t *j, int8_t value) { return json_append_i8(j, value); }
static inline bool json_append(union json_t *j, int16_t value) { return json_append_i16(j, value); }
static inline bool json_append(union json_t *j, int32_t value) { return json_append_i32(j, value); }
static inline bool json_append(union json_t *j, int64_t value) { return json_append_i64(j, value); }
static inline bool json_append(union json_t *j, uint8_t value) { return json_append_u8(j, value); }
static inline bool json_append(union json_t *j, uint16_t value) { return json_append_u16(j, value); }
static inline bool json_append(union json_t *j, uint32_t value) { return json_append_u32(j, value); }
static inline bool json_append(union json_t *j, uint64_t value) { return json_append_u64(j, value); }
static inline bool json_append(union json_t *j, float value) { return json_append_f32(j, value); }
static inline bool json_append(union json_t *j, double value) { return json_append_f64(j, value); }
static inline bool json_append(union json_t *j, union json_t value) { return json_append_value(j, value); }
static inline bool json_append(union json_t *j, union json_t *value) { return json_append_value_p(j, value); }

static inline union json_t json_get(union json_t j, const char *key) { return __json_get_from_obj(j, key); }
static inline union json_t json_get(union json_t j, int i) { return __json_get_from_arr(j, i); }

static inline union json_t *json_getp(union json_t j, const char *key) { return __json_getp_from_obj(j, key); }
static inline union json_t *json_getp(union json_t j, int i) { return __json_getp_from_arr(j, i); }

static inline union json_t json_remove(union json_t *j, const char *key) { return __json_remove_from_obj(j, key); }
static inline union json_t json_remove(union json_t *j, int i) { return __json_remove_from_arr(j, i); }

static inline void json_delete(union json_t *j, const char *key) { return __json_delete_from_obj(j, key); }
static inline void json_delete(union json_t *j, int i) { return __json_delete_from_arr(j, i); }

static inline void json_merge(union json_t *j, union json_t from) { return __json_merge(j, from); }
static inline void json_merge(union json_t *j, union json_t *from) { return __json_merge_p(j, from); }

static inline void json_concat(union json_t *j, union json_t from) { return __json_concat(j, from); }

static inline void json_concat_p(union json_t *j, union json_t *from) { return __json_concat_p(j, from); }

#endif /* __JSON_HH__ */
