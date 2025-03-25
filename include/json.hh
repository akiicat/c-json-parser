
#ifndef __JSON_HH__
#define __JSON_HH__

#include <cstddef>
#include <cstdint>

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
constexpr union json_t JSON_TRUE = {
    .tok = {.type = JT_BOOL, .text = nullptr, .boolean = true}};
constexpr union json_t JSON_FALSE = {
    .tok = {.type = JT_BOOL, .text = nullptr, .boolean = false}};
constexpr union json_t JSON_STRING(const char *x) {
  return {.tok = {.type = JT_STRING, .text = const_cast<char *>(x)}};
}
constexpr union json_t JSON_NUMBER(const char *x) {
  return {.tok = {.type = JT_NUMBER, .text = const_cast<char *>(x)}};
}
constexpr union json_t JSON_BOOL(bool x) {
  return {.tok = {.type = JT_BOOL, .text = nullptr, .boolean = (x)}};
}
constexpr union json_t JSON_INT(int64_t x) {
  return {.tok = {.type = JT_INT, .text = nullptr, .i64 = (int64_t)(x)}};
}
constexpr union json_t JSON_UINT(uint64_t x) {
  return {.tok = {.type = JT_UINT, .text = nullptr, .u64 = (uint64_t)(x)}};
}
constexpr union json_t JSON_FLOAT(double x) {
  return {.tok = {.type = JT_FLOAT, .text = nullptr, .f64 = (double)(x)}};
}

static inline bool
json_set(union json_t *j, const char *key, const char *value) {
  return json_set_str(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, char *value) {
  return json_set_str(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, bool value) {
  return json_set_bool(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, void *value) {
  return json_set_null(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, int8_t value) {
  return json_set_i8(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, int16_t value) {
  return json_set_i16(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, int32_t value) {
  return json_set_i32(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, int64_t value) {
  return json_set_i64(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, uint8_t value) {
  return json_set_u8(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, uint16_t value) {
  return json_set_u16(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, uint32_t value) {
  return json_set_u32(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, uint64_t value) {
  return json_set_u64(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, float value) {
  return json_set_f32(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key, double value) {
  return json_set_f64(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key,
                            union json_t value) {
  return json_set_value(j, key, value);
}
static inline bool json_set(union json_t *j, const char *key,
                            union json_t *value) {
  return json_set_value_p(j, key, value);
}

static inline bool json_append(union json_t *j, const char *value) {
  return json_append_str(j, value);
}
static inline bool json_append(union json_t *j, char *value) {
  return json_append_str(j, value);
}
static inline bool json_append(union json_t *j, bool value) {
  return json_append_bool(j, value);
}
static inline bool json_append(union json_t *j, void *value) {
  return json_append_null(j, value);
}
static inline bool json_append(union json_t *j, int8_t value) {
  return json_append_i8(j, value);
}
static inline bool json_append(union json_t *j, int16_t value) {
  return json_append_i16(j, value);
}
static inline bool json_append(union json_t *j, int32_t value) {
  return json_append_i32(j, value);
}
static inline bool json_append(union json_t *j, int64_t value) {
  return json_append_i64(j, value);
}
static inline bool json_append(union json_t *j, uint8_t value) {
  return json_append_u8(j, value);
}
static inline bool json_append(union json_t *j, uint16_t value) {
  return json_append_u16(j, value);
}
static inline bool json_append(union json_t *j, uint32_t value) {
  return json_append_u32(j, value);
}
static inline bool json_append(union json_t *j, uint64_t value) {
  return json_append_u64(j, value);
}
static inline bool json_append(union json_t *j, float value) {
  return json_append_f32(j, value);
}
static inline bool json_append(union json_t *j, double value) {
  return json_append_f64(j, value);
}
static inline bool json_append(union json_t *j, union json_t value) {
  return json_append_value(j, value);
}
static inline bool json_append(union json_t *j, union json_t *value) {
  return json_append_value_p(j, value);
}

static inline union json_t json_get(union json_t j, const char *key) {
  return __json_get_from_obj(j, key);
}
static inline union json_t json_get(union json_t j, int i) {
  return __json_get_from_arr(j, i);
}

static inline union json_t *json_getp(union json_t j, const char *key) {
  return __json_getp_from_obj(j, key);
}
static inline union json_t *json_getp(union json_t j, int i) {
  return __json_getp_from_arr(j, i);
}

static inline union json_t json_remove(union json_t *j, const char *key) {
  return __json_remove_from_obj(j, key);
}
static inline union json_t json_remove(union json_t *j, int i) {
  return __json_remove_from_arr(j, i);
}

static inline void json_delete(union json_t *j, const char *key) {
  return __json_delete_from_obj(j, key);
}
static inline void json_delete(union json_t *j, int i) {
  return __json_delete_from_arr(j, i);
}

static inline void json_merge(union json_t *j, union json_t from) {
  return __json_merge(j, from);
}
static inline void json_merge(union json_t *j, union json_t *from) {
  return __json_merge_p(j, from);
}

static inline void json_concat(union json_t *j, union json_t from) {
  return __json_concat(j, from);
}

static inline void json_concat_p(union json_t *j, union json_t *from) {
  return __json_concat_p(j, from);
}

#endif /* __JSON_HH__ */
