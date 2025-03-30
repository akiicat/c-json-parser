#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "json.h"

#include <execinfo.h>
#include <string.h>

#define UNUSED(x) ((void)(x))

void json_print_trace(void) {
    void *array[10];
    char **strings;
    int size, i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);
    if (strings != NULL) {

        printf("Obtained %d stack frames.\n", size);
        for (i = 0; i < size; i++)
            printf("%s\n", strings[i]);
    }

    free(strings);
}

char *json_strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = (char *)malloc(size);
    if (p != NULL) {
        memcpy(p, s, size);
    }
    return p;
}

char *json_strndup(const char *s, size_t n) {
    char *p;
    size_t n1;

    for (n1 = 0; n1 < n && s[n1] != '\0'; n1++)
        continue;
    p = (char *)malloc(n + 1);
    if (p != NULL) {
        memcpy(p, s, n1);
        p[n1] = '\0';
    }
    return p;
}

// --------------------------------------------------
// SECTION: JSON TOKEN
// --------------------------------------------------
static const char *type_str[] = {
    "JT_MISSING", "JT_STRING", "JT_NUMBER", "JT_BOOL",  "JT_NULL",
    "JT_INT",     "JT_UINT",  "JT_FLOAT",  "JT_OBJECT", "JT_ARRAY", "JT_TOKEN_SIZE",
};

const char *json_type2str(enum json_token_type_t type) {
    if (type >= JT_MISSING && type <= JT_TOKEN_SIZE) {
        return type_str[type];
    }
    return NULL;
}

// --------------------------------------------------
// !SECTION: END JSON TOKEN
// --------------------------------------------------

// --------------------------------------------------
// SECTION: JSON COMMON FUNCTION
// --------------------------------------------------

union json_t json_dup(union json_t j) {
    union json_t res = {.type = j.type};

    switch (j.type) {
    case JT_STRING:
    case JT_NUMBER:
    case JT_BOOL:
    case JT_NULL:
    case JT_INT:
    case JT_UINT:
    case JT_FLOAT: {
        res.tok = j.tok;
        if (j.text)
            res.text = json_strdup(j.text);
        break;
    }
    case JT_ARRAY: {
        res.type = JT_ARRAY;
        for (size_t i = 0; i < jsonext_arr_length(&j); i++) {
            union json_t *it = jsonext_arr_get(&j, i);
            json_append_value(&res, *it);
        }
        break;
    }
    case JT_OBJECT: {
        res.type = JT_OBJECT;

        /* first/next approach for loop */
        size_t i;
        struct json_pair_t *it = jsonext_obj_iter_first(&j);
        for (i = 0; i < jsonext_obj_length(&j); i++) {
            json_set_obj_value(&res, it->key, it->value);
            it = jsonext_obj_iter_next(&j, it);
        }

        if (i != jsonext_obj_length(&j)) {
            printf("%s:%d Bug Dup Sanity Check Fail type=%s key=%s\n", __func__, __LINE__, json_type2str(j.type),
                   it->key);
        }

        break;
    }
    default:
        fprintf(stderr, "%s: unsupport token <%d|%s>", __func__, j.type, json_type2str(j.type));
        json_print_trace();
        break;
    }

    return res;
}

size_t json_length(union json_t j) {
    if (j.type == JT_OBJECT)
        return jsonext_obj_length(&j);
    if (j.type == JT_ARRAY)
        return jsonext_arr_length(&j);
    return 0;
}

size_t json_capacity(union json_t j) {
    if (j.type == JT_OBJECT)
        return jsonext_obj_capacity(&j);
    if (j.type == JT_ARRAY)
        return jsonext_arr_capacity(&j);
    return 0;
}

// Assume these functions are defined elsewhere:
// jsonext_arr_length(), jsonext_arr_get(), jsonext_obj_length(),
// jsonext_obj_iter_first(), jsonext_obj_iter_next(), json_type2str(),
// json_print_trace(). Also assume the json_t union has a field 'type' and a member
// 'tok' with subfields appropriate to each JSON type (e.g., text, boolean, u64, f).

// A simple string builder structure.
struct sb {
    char *data;
    size_t length;
    size_t capacity;
};

// Initialize the string builder.
static void sb_init(struct sb *sb) {
    sb->capacity = 128;
    sb->data = (char *)malloc(sb->capacity);
    if (!sb->data) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    sb->length = 0;
    sb->data[0] = '\0';
}

// Ensure there is enough capacity to append additional bytes.
static void sb_ensure_capacity(struct sb *sb, size_t additional) {
    if (sb->length + additional + 1 > sb->capacity) {
        sb->capacity *= 2;
        char *new_data = (char *)realloc(sb->data, sb->capacity);
        if (!new_data) {
            free(sb->data);
            fprintf(stderr, "Memory allocation error\n");
            exit(1);
        }
        sb->data = new_data;
    }
}

// Append a C-string to the string builder.
static void sb_append(struct sb *sb, const char *str) {
    size_t len = strlen(str);
    sb_ensure_capacity(sb, len);
    memcpy(sb->data + sb->length, str, len);
    sb->length += len;
    sb->data[sb->length] = '\0';
}

// Append a single character to the string builder.
static void sb_append_char(struct sb *sb, char c) {
    sb_ensure_capacity(sb, 1);
    sb->data[sb->length++] = c;
    sb->data[sb->length] = '\0';
}

// Append formatted text to the string builder.
static void sb_appendf(struct sb *sb, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    // Determine the size needed.
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    if (needed < 0) {
        va_end(args);
        return;
    }
    sb_ensure_capacity(sb, (size_t)needed);
    vsnprintf(sb->data + sb->length, needed + 1, fmt, args);
    sb->length += needed;
    va_end(args);
}

// Append indentation (spaces) to the string builder.
static void sb_append_indent(struct sb *sb, int indent) {
    for (int i = 0; i < indent; i++) {
        sb_append_char(sb, ' ');
    }
}

static void sb_append_crlf(struct sb *sb, int indent) {
    if (indent >= 0)
        sb_append_char(sb, '\n');
}

// Recursive function to dump JSON into the string builder.
// offset: current indentation level,
// indent: number of spaces to add per level.
static void json_dumps_internal(union json_t j, int offset, int indent, struct sb *sb) {
    switch (j.type) {
    case JT_STRING:
        sb_appendf(sb, "\"%s\"", j.text);
        break;
    case JT_NUMBER:
        sb_appendf(sb, "%s", j.text);
        break;
    case JT_BOOL:
        sb_append(sb, j.boolean ? "true" : "false");
        break;
    case JT_NULL:
        sb_append(sb, "null");
        break;
    case JT_INT:
        sb_appendf(sb, "%ld", j.u64);
        break;
    case JT_UINT:
        sb_appendf(sb, "%lu", j.u64);
        break;
    case JT_FLOAT:
        sb_appendf(sb, "%lf", j.f);
        break;
    case JT_ARRAY: {
        size_t length = jsonext_arr_length(&j);
        sb_append(sb, "[");
        if (length > 0) {
            sb_append_crlf(sb, indent);
            for (size_t i = 0; i < length; i++) {
                sb_append_indent(sb, offset + indent);
                union json_t *elem = jsonext_arr_get(&j, i);
                json_dumps_internal(*elem, offset + indent, indent, sb);
                if (i + 1 < length) {
                    sb_append(sb, ", ");
                    sb_append_crlf(sb, indent);
                }
            }
            sb_append_crlf(sb, indent);
            sb_append_indent(sb, offset);
        }
        sb_append(sb, "]");
        break;
    }
    case JT_OBJECT: {
        size_t length = jsonext_obj_length(&j);
        sb_append(sb, "{");
        if (length > 0) {
            sb_append_crlf(sb, indent);
            struct json_pair_t *it = jsonext_obj_iter_first(&j);
            while (it != NULL) {
                sb_append_indent(sb, offset + indent);
                sb_appendf(sb, "\"%s\": ", it->key);
                json_dumps_internal(it->value, offset + indent, indent, sb);
                it = jsonext_obj_iter_next(&j, it);
                if (it != NULL) {
                    sb_append(sb, ", ");
                    sb_append_crlf(sb, indent);
                }
            }
            sb_append_crlf(sb, indent);
            sb_append_indent(sb, offset);
        }
        sb_append(sb, "}");
        break;
    }
    case JT_MISSING:
        sb_append(sb, "MISSING\n");
        break;
    default:
        fprintf(stderr, "Error: Print Token Not found <%d|%s>\n", j.type, json_type2str(j.type));
        json_print_trace();
        assert(0);
    }
}

// Public function that returns a malloc'd string containing the JSON dump.
// The 'indent' parameter specifies the number of spaces to use per nested
// level.
char *__json_dumps(union json_t j, struct json_config config) {
    struct sb sb;
    sb_init(&sb);
    json_dumps_internal(j, config.offset, config.indent, &sb);
    // Allocate a new buffer with the exact size.
    char *result = (char *)malloc(sb.length + 1);
    if (result) {
        memcpy(result, sb.data, sb.length + 1);
    }
    free(sb.data);
    return result;
}

// Helper function to print indent spaces.
static void print_indent(FILE *fp, int indent) {
    for (int i = 0; i < indent; i++) {
        fputc(' ', fp);
    }
}

// Helper function to print a newline if the indent value is non-negative.
static void print_crlf(FILE *fp, int diff) {
    if (diff >= 0)
        fputc('\n', fp);
}

// Internal recursive function that prints JSON with the given indent level to
// the specified FILE.
static void json_print_internal(union json_t j, int offset, int indent, FILE *fp) {
    switch (j.type) {
    case JT_STRING:
        fprintf(fp, "\"%s\"", j.text);
        break;
    case JT_NUMBER:
        fprintf(fp, "%s", j.text);
        break;
    case JT_BOOL:
        fprintf(fp, j.boolean ? "true" : "false");
        break;
    case JT_NULL:
        fprintf(fp, "null");
        break;
    case JT_INT:
        fprintf(fp, "%ld", j.u64);
        break;
    case JT_UINT:
        fprintf(fp, "%lu", j.u64);
        break;
    case JT_FLOAT:
        fprintf(fp, "%lf", j.f);
        break;
    case JT_ARRAY: {
        size_t length = jsonext_arr_length(&j);
        fprintf(fp, "[");
        if (length > 0) {
            print_crlf(fp, indent);
            for (size_t i = 0; i < length; i++) {
                print_indent(fp, offset + indent);
                json_print_internal(*jsonext_arr_get(&j, i), offset + indent, indent, fp);
                if (i + 1 < length) {
                    fprintf(fp, ", ");
                    print_crlf(fp, indent);
                }
            }
            print_crlf(fp, indent);
            print_indent(fp, offset);
        }
        fprintf(fp, "]");
        break;
    }
    case JT_OBJECT: {
        size_t length = jsonext_obj_length(&j);
        fprintf(fp, "{");
        if (length > 0) {
            print_crlf(fp, indent);
            struct json_pair_t *it = jsonext_obj_iter_first(&j);
            while (it != NULL) {
                print_indent(fp, offset + indent);
                fprintf(fp, "\"%s\": ", it->key);
                json_print_internal(it->value, offset + indent, indent, fp);
                it = jsonext_obj_iter_next(&j, it);
                if (it != NULL) {
                    fprintf(fp, ", ");
                    print_crlf(fp, indent);
                }
            }
            print_crlf(fp, indent);
            print_indent(fp, offset);
        }
        fprintf(fp, "}");
        break;
    }
    default:
        fprintf(stderr, "Error: Print Token Not found <%d|%s>\n", j.type, json_type2str(j.type));
        json_print_trace();
        assert(0);
    }
}

// Public function that starts printing with zero indentation to the specified
// file.
void json_print(union json_t j) { json_print_internal(j, 0, -1, stdout); }

void __json_pprint(union json_t j, struct json_config config) {
    json_print_internal(j, config.offset, config.indent, stdout);
}

void __json_dump(union json_t j, FILE *f, struct json_config config) {
    json_print_internal(j, config.offset, config.indent, f);
    fflush(f);
}

void json_clean(union json_t *j) {
    switch (j->type) {
    case JT_STRING:
    case JT_NUMBER:
    case JT_BOOL:
    case JT_NULL:
    case JT_FLOAT: {
        free(j->text);
        j->text = NULL;
        break;
    }
    case JT_INT:
    case JT_UINT: {
        break;
    }
    case JT_ARRAY: {
        for (size_t i = jsonext_arr_length(j) - 1; i < jsonext_arr_length(j); i--) {
            __json_delete_from_arr(j, i);
        }
        jsonext_arr_clean(j);
        break;
    }
    case JT_OBJECT: {
        size_t size = jsonext_obj_length(j);
        const char **keys = (const char **)calloc(sizeof(const char *), size);
        size_t key_len = 0;

        for (struct json_pair_t *it = jsonext_obj_iter_first(j); it != NULL; it = jsonext_obj_iter_next(j, it)) {
            keys[key_len++] = it->key;
        }

        for (size_t i = 0; i < key_len; i++) {
            __json_delete_from_obj(j, keys[i]);
        }
        jsonext_obj_clean(j);
        free(keys);
        break;
    }
    default:
        fprintf(stderr, "%s: unsupport token <%d|%s>", __func__, j->type, json_type2str(j->type));
        break;
    }
}

bool __json_update(union json_t *j, union json_t value, bool copy_value) {
    if (!j)
        return false;

    json_clean(j);

    if (value.type == JT_MISSING) {
        return true;
    }

    *j = copy_value ? json_dup(value) : value;

    return true;
}
bool json_update_str(union json_t *j, const char *value) {
    return __json_update(j, JSON_STRING((char *)value), true);
}
bool json_update_bool(union json_t *j, bool value) {
    return __json_update(j, JSON_BOOL(value), false);
}
bool json_update_null(union json_t *j, void *value) {
    UNUSED(value);
    return __json_update(j, JSON_NULL, false);
}
bool json_update_i8(union json_t *j, int8_t value) {
    return __json_update(j, JSON_INT(value), false);
}
bool json_update_i16(union json_t *j, int16_t value) {
    return __json_update(j, JSON_INT(value), false);
}
bool json_update_i32(union json_t *j, int32_t value) {
    return __json_update(j, JSON_INT(value), false);
}
bool json_update_i64(union json_t *j, int64_t value) {
    return __json_update(j, JSON_INT(value), false);
}
bool json_update_u8(union json_t *j, uint8_t value) {
    return __json_update(j, JSON_UINT(value), false);
}
bool json_update_u16(union json_t *j, uint16_t value) {
    return __json_update(j, JSON_UINT(value), false);
}
bool json_update_u32(union json_t *j, uint32_t value) {
    return __json_update(j, JSON_UINT(value), false);
}
bool json_update_u64(union json_t *j, uint64_t value) {
    return __json_update(j, JSON_UINT(value), false);
}
bool json_update_f32(union json_t *j, float value) {
    return __json_update(j, JSON_FLOAT(value), false);
}
bool json_update_f64(union json_t *j, double value) {
    return __json_update(j, JSON_FLOAT(value), false);
}
bool json_update_value(union json_t *j, union json_t value) {
    return __json_update(j, value, true);
}
bool json_update_value_p(union json_t *j, union json_t *value) {
    bool res = __json_update(j, *value, false);
    *value = JSON_TYPE(value->type);
    return res;
}

// --------------------------------------------------
// !SECTION: END JSON COMMON FUNCTION
// --------------------------------------------------

// --------------------------------------------------
// SECTION: JSON OBJECT FUNCTION
// --------------------------------------------------
union json_t json_create_obj(size_t capacity) {
    union json_t j = {.type = JT_OBJECT};
    jsonext_obj_new(&j, capacity);
    return j;
}

union json_t *__json_getp_from_obj(union json_t j, const char *key) {
    if (j.type != JT_OBJECT)
        return NULL;
    struct json_pair_t *p = jsonext_obj_get(&j, key);
    return p ? &p->value : NULL;
}

union json_t __json_get_from_obj(union json_t j, const char *key) {
    union json_t empty = {.type = JT_MISSING};
    union json_t *res = __json_getp_from_obj(j, key);
    return res ? *res : empty;
}

struct json_pair_t *json_obj_iter_first(union json_t j) {
    if (j.type != JT_OBJECT)
        return NULL;
    return jsonext_obj_iter_first(&j);
}

struct json_pair_t *json_obj_iter_next(union json_t j, struct json_pair_t *it) {
    if (j.type != JT_OBJECT)
        return NULL;
    return jsonext_obj_iter_next(&j, it);
}

void __json_merge(union json_t *j, union json_t from) {
    if (!j || j->type != JT_OBJECT || from.type != JT_OBJECT)
        return;
    struct json_pair_t *it = jsonext_obj_iter_first(&from);
    for (size_t i = 0; i < jsonext_obj_length(&from); i++) {
        json_set_obj_value(j, it->key, it->value);
        it = jsonext_obj_iter_next(&from, it);
    }
}

void __json_merge_p(union json_t *j, union json_t *from) {
    if (!j || j->type != JT_OBJECT || from->type != JT_OBJECT)
        return;
    __json_merge(j, *from);
    json_clean(from);
}

union json_t __json_remove_from_obj(union json_t *j, const char *key) {
    struct json_pair_t *p = jsonext_obj_delete(j, key);
    union json_t res = {.type = JT_MISSING};
    if (!j || j->type != JT_OBJECT)
        return res;
    if (p) {
        res = p->value;
        free(p->key);
        free(p);
    }
    return res;
}

void __json_delete_from_obj(union json_t *j, const char *key) {
    if (!j || j->type != JT_OBJECT)
        return;
    union json_t rm = __json_remove_from_obj(j, key);
    json_clean(&rm);
}

bool __json_set_obj(union json_t *j, const char *key, size_t key_len, union json_t value, bool copy_value) {
    if (!j || j->type != JT_OBJECT)
        return false;

    if (value.type == JT_MISSING) {
        __json_delete_from_obj(j, key);
        return true;
    }

    union json_t *exist_value = __json_getp_from_obj(*j, key);

    if (exist_value) {
        printf("%s:%d Obj Key Exist key=%s\n", __func__, __LINE__, key);
        json_clean(exist_value);
        *exist_value = copy_value ? json_dup(value) : value;
        return true;
    }

    struct json_pair_t *new_pair = (struct json_pair_t *)malloc(sizeof(struct json_pair_t));
    new_pair->key = json_strndup(key, key_len);
    new_pair->value = copy_value ? json_dup(value) : value;

    printf("%s:%d Obj insert key=%s\n", __func__, __LINE__, key);
    jsonext_obj_insert(j, new_pair);

    return true;
}
bool json_set_obj_str(union json_t *j, const char *key, const char *value) {
    return __json_set_obj(j, key, strlen(key), JSON_STRING((char *)value), true);
}
bool json_set_obj_bool(union json_t *j, const char *key, bool value) {
    return __json_set_obj(j, key, strlen(key), JSON_BOOL(value), false);
}
bool json_set_obj_null(union json_t *j, const char *key, void *value) {
    UNUSED(value);
    return __json_set_obj(j, key, strlen(key), JSON_NULL, false);
}
bool json_set_obj_i8(union json_t *j, const char *key, int8_t value) {
    return __json_set_obj(j, key, strlen(key), JSON_INT(value), false);
}
bool json_set_obj_i16(union json_t *j, const char *key, int16_t value) {
    return __json_set_obj(j, key, strlen(key), JSON_INT(value), false);
}
bool json_set_obj_i32(union json_t *j, const char *key, int32_t value) {
    return __json_set_obj(j, key, strlen(key), JSON_INT(value), false);
}
bool json_set_obj_i64(union json_t *j, const char *key, int64_t value) {
    return __json_set_obj(j, key, strlen(key), JSON_INT(value), false);
}
bool json_set_obj_u8(union json_t *j, const char *key, uint8_t value) {
    return __json_set_obj(j, key, strlen(key), JSON_UINT(value), false);
}
bool json_set_obj_u16(union json_t *j, const char *key, uint16_t value) {
    return __json_set_obj(j, key, strlen(key), JSON_UINT(value), false);
}
bool json_set_obj_u32(union json_t *j, const char *key, uint32_t value) {
    return __json_set_obj(j, key, strlen(key), JSON_UINT(value), false);
}
bool json_set_obj_u64(union json_t *j, const char *key, uint64_t value) {
    return __json_set_obj(j, key, strlen(key), JSON_UINT(value), false);
}
bool json_set_obj_f32(union json_t *j, const char *key, float value) {
    return __json_set_obj(j, key, strlen(key), JSON_FLOAT(value), false);
}
bool json_set_obj_f64(union json_t *j, const char *key, double value) {
    return __json_set_obj(j, key, strlen(key), JSON_FLOAT(value), false);
}
bool json_set_obj_value(union json_t *j, const char *key, union json_t value) {
    return __json_set_obj(j, key, strlen(key), value, true);
}
bool json_set_obj_value_p(union json_t *j, const char *key, union json_t *value) {
    bool res = __json_set_obj(j, key, strlen(key), *value, false);
    *value = JSON_TYPE(value->type);
    return res;
}
bool json_set_obj_value_np(union json_t *j, const char *key, size_t key_len, union json_t *value) {
    bool res = __json_set_obj(j, key, key_len, *value, false);
    *value = JSON_TYPE(value->type);
    return res;
}

// --------------------------------------------------
// !SECTION: END JSON OBJECT FUNCTION
// --------------------------------------------------

// --------------------------------------------------
// SECTION: JSON ARRAY FUNCTION
// --------------------------------------------------

union json_t json_create_arr(size_t capacity) {
    union json_t j = {.type = JT_ARRAY};
    jsonext_arr_new(&j, capacity);
    return j;
}

void __json_concat(union json_t *j, union json_t from) {
    if (!j || j->type != JT_ARRAY || from.type != JT_ARRAY)
        return;
    for (size_t i = jsonext_arr_length(j) - 1; i < jsonext_arr_length(j); i--) {
        json_append_value(j, __json_get_from_arr(from, i));
    }
}

void __json_concat_p(union json_t *j, union json_t *from) {
    if (!j || j->type != JT_ARRAY || from->type != JT_ARRAY)
        return;
    __json_concat(j, *from);
    json_clean(from);
}

union json_t *__json_getp_from_arr(union json_t j, long int i) {
    if (j.type != JT_ARRAY)
        return NULL;
    size_t index = (i < 0) ? jsonext_arr_length(&j) + i : (size_t)i;
    return jsonext_arr_get(&j, index);
}

union json_t __json_get_from_arr(union json_t j, long int i) {
    union json_t empty = {.type = JT_MISSING};
    union json_t *res = __json_getp_from_arr(j, i);
    return res ? *res : empty;
}

union json_t __json_remove_from_arr(union json_t *j, long int i) {
    size_t index = (i < 0) ? jsonext_arr_length(j) + i : (size_t)i;
    union json_t *it = jsonext_arr_delete(j, index);
    union json_t res = {.type = JT_MISSING};
    if (!j || j->type != JT_ARRAY)
        return res;
    if (it) {
        res = *it;
        free(it);
    }
    return res;
}

void __json_delete_from_arr(union json_t *j, long int i) {
    if (!j || j->type != JT_ARRAY)
        return;
    size_t index = (i < 0) ? jsonext_arr_length(j) + i : (size_t)i;
    union json_t rm = __json_remove_from_arr(j, index);
    json_clean(&rm);
}

bool __json_set_arr(union json_t *j, long int i, union json_t value, bool copy_value) {
    if (!j || j->type != JT_ARRAY)
        return false;

    size_t length = jsonext_arr_length(j);

    size_t index = (i < 0) ? length + i : (size_t)i;
    if (index >= length) {
        printf("%s:%d Array Out of Range: Index=%ld Length=%ld\n", __func__, __LINE__, i, length);
        return false;   
    }

    if (value.type == JT_MISSING) {
        __json_delete_from_arr(j, index);
        return true;
    }

    union json_t *exist_value = __json_getp_from_arr(*j, index);
    if (exist_value) {
        json_clean(exist_value);
        *exist_value = copy_value ? json_dup(value) : value;
        return true;
    }

    printf("[Bug] %s:%d Array Element Not Exist: Index=%ld Length=%ld\n", __func__, __LINE__, i, length);
    return false;
}

bool json_set_arr_str(union json_t *j, long int i, const char *value) {
    return __json_set_arr(j, i, JSON_STRING((char *)value), true);
}
bool json_set_arr_bool(union json_t *j, long int i, bool value) {
    return __json_set_arr(j, i, JSON_BOOL(value), false);
}
bool json_set_arr_null(union json_t *j, long int i, void *value) {
    UNUSED(value);
    return __json_set_arr(j, i, JSON_NULL, false);
}
bool json_set_arr_i8(union json_t *j, long int i, int8_t value) {
    return __json_set_arr(j, i, JSON_INT(value), false);
}
bool json_set_arr_i16(union json_t *j, long int i, int16_t value) {
    return __json_set_arr(j, i, JSON_INT(value), false);
}
bool json_set_arr_i32(union json_t *j, long int i, int32_t value) {
    return __json_set_arr(j, i, JSON_INT(value), false);
}
bool json_set_arr_i64(union json_t *j, long int i, int64_t value) {
    return __json_set_arr(j, i, JSON_INT(value), false);
}
bool json_set_arr_u8(union json_t *j, long int i, uint8_t value) {
    return __json_set_arr(j, i, JSON_UINT(value), false);
}
bool json_set_arr_u16(union json_t *j, long int i, uint16_t value) {
    return __json_set_arr(j, i, JSON_UINT(value), false);
}
bool json_set_arr_u32(union json_t *j, long int i, uint32_t value) {
    return __json_set_arr(j, i, JSON_UINT(value), false);
}
bool json_set_arr_u64(union json_t *j, long int i, uint64_t value) {
    return __json_set_arr(j, i, JSON_UINT(value), false);
}
bool json_set_arr_f32(union json_t *j, long int i, float value) {
    return __json_set_arr(j, i, JSON_FLOAT(value), false);
}
bool json_set_arr_f64(union json_t *j, long int i, double value) {
    return __json_set_arr(j, i, JSON_FLOAT(value), false);
}
bool json_set_arr_value(union json_t *j, long int i, union json_t value) {
    return __json_set_arr(j, i, value, true);
}
bool json_set_arr_value_p(union json_t *j, long int i, union json_t *value) {
    bool res = __json_set_arr(j, i, *value, false);
    *value = JSON_TYPE(value->type);
    return res;
}

bool __json_append(union json_t *j, union json_t value, bool copy_value) {
    if (!j || j->type != JT_ARRAY || value.type == JT_MISSING)
        return false;

    union json_t *new_value = (union json_t *)malloc(sizeof(union json_t));
    *new_value = copy_value ? json_dup(value) : value;

    jsonext_arr_append(j, new_value);

    return true;
}
bool json_append_str(union json_t *j, const char *value) { return __json_append(j, JSON_STRING((char *)value), true); }
bool json_append_bool(union json_t *j, bool value) { return __json_append(j, JSON_BOOL(value), false); }
bool json_append_null(union json_t *j, void *value) {
    UNUSED(value);
    return __json_append(j, JSON_NULL, false);
}
bool json_append_i8(union json_t *j, int8_t value) { return __json_append(j, JSON_INT(value), false); }
bool json_append_i16(union json_t *j, int16_t value) { return __json_append(j, JSON_INT(value), false); }
bool json_append_i32(union json_t *j, int32_t value) { return __json_append(j, JSON_INT(value), false); }
bool json_append_i64(union json_t *j, int64_t value) { return __json_append(j, JSON_INT(value), false); }
bool json_append_u8(union json_t *j, uint8_t value) { return __json_append(j, JSON_UINT(value), false); }
bool json_append_u16(union json_t *j, uint16_t value) { return __json_append(j, JSON_UINT(value), false); }
bool json_append_u32(union json_t *j, uint32_t value) { return __json_append(j, JSON_UINT(value), false); }
bool json_append_u64(union json_t *j, uint64_t value) { return __json_append(j, JSON_UINT(value), false); }
bool json_append_f32(union json_t *j, float value) { return __json_append(j, JSON_FLOAT(value), false); }
bool json_append_f64(union json_t *j, double value) { return __json_append(j, JSON_FLOAT(value), false); }
bool json_append_value(union json_t *j, union json_t value) { return __json_append(j, value, true); }
bool json_append_value_p(union json_t *j, union json_t *value) {
    bool res = __json_append(j, *value, false);
    *value = JSON_TYPE(value->type);
    return res;
}

// --------------------------------------------------
// !SECTION: END JSON ARRAY FUNCTION
// --------------------------------------------------

// --------------------------------------------------
// SECTION: JSON Lexer
// --------------------------------------------------

static const char *lexer_type_str[] = {
    "JLT_MISSING", "JLT_STRING", "JLT_NUMBER",           "JLT_COMMA",  "JLT_COLON",
    "JLT_LPAIR",   "JLT_RPAIR",  "JLT_LARRAY",           "JLT_RARRAY", "JLT_TRUE",
    "JLT_FALSE",   "JLT_NULL",   "JLT_LEXER_TOKEN_SIZE",
};

const char *json_lexer_type2str(enum json_lexer_token_type_t type) {
    if (type >= JLT_MISSING && type <= JLT_LEXER_TOKEN_SIZE) {
        return lexer_type_str[type];
    }

    return NULL;
}

struct json_lexer_context_t *json_create_lexer(const char *str) {
    struct json_lexer_context_t *ctx_p = (struct json_lexer_context_t *)malloc(sizeof(struct json_lexer_context_t));


    struct json_lexer_context_t ctx = {
        .tokens =
            {
                .length = 0,
                .capacity = 0,
                .list = NULL,
            },
        .offset = 0,
        .column = 1,
        .row = 1,
        .from_string = str,
        .from_string_len = strlen(str),
    };

    *ctx_p = ctx;

    return ctx_p;
}

void json_print_lexer(struct json_lexer_context_t *ctx) {
    struct json_lexer_token_t t;
    for (size_t i = 0; i < ctx->tokens.length; i++) {
        t = ctx->tokens.list[i];
        printf("@%lu#%lu,%lu<%u|%s>%lu:%lu", t.index, t.start, t.end, t.type, json_lexer_type2str(t.type), t.row,
               t.column);

        if (t.text) {
            int textSize = t.end - t.start;
            printf(" %.*s", textSize, t.text);
        }

        printf("\n");
    }
}

void json_delete_lexer(struct json_lexer_context_t *ctx) {
    if (ctx && ctx->tokens.list)
        free(ctx->tokens.list);
    free(ctx);
}

/*
 * The comment block below explains the role of the offset:
 *
 * For example:
 *   Offset is 0 means the lookahead char is str[0] and no character has been matched yet.
 *   Offset is n means the lookahead char is str[n] and the last matched char is str[n-1]
 *                     |
 *         'a' 'b' 'c' | 'd' 'e' ...
 *  -------------------|---------------------> Input Stream
 *                   | |  ^
 *        Last Matched |  Lookahead
 *                     Offset
*/
static int next_char(struct json_lexer_context_t *ctx) {
    if (!ctx->from_string) {
        fprintf(stderr, "No streaming input\n");
        json_print_trace();
        assert(0);
    }

    if (ctx->offset >= ctx->from_string_len) {
        return EOF;
    }

    ctx->offset++;
    ctx->column++;

    return ctx->from_string[ctx->offset];
}

static int lookahead_char(struct json_lexer_context_t *ctx) {
    if (ctx->offset >= ctx->from_string_len) {
        return EOF;
    }

    return ctx->from_string[ctx->offset];
}

static bool lookahead(struct json_lexer_context_t *ctx, int c) { return lookahead_char(ctx) == c; }

static void match(struct json_lexer_context_t *ctx, int c) {
    int cur = lookahead_char(ctx);

    // always get the next one before checking
    next_char(ctx);

    if (cur != c) {
        fprintf(stderr, "Syntax Error %lu:%lu: Unexpected Char: %c\n", ctx->row, ctx->column, cur);
        json_print_trace();
        assert(0);
    }
}

static void match_str(struct json_lexer_context_t *ctx, const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        match(ctx, str[i]);
    }
}

static bool match_if_exist(struct json_lexer_context_t *ctx, int c) {
    if (lookahead(ctx, c)) {
        match(ctx, c);
        return true;
    }
    return false;
}

static bool match_between_if_exist(struct json_lexer_context_t *ctx, int start, int end) {
    int cur = lookahead_char(ctx);
    if (cur >= start && cur <= end) {
        match(ctx, cur);
        return true;
    }
    return false;
}

static const char *substring(struct json_lexer_context_t *ctx, size_t start, size_t end) {
    return ctx->from_string + start;
}

static void insert_token(struct json_lexer_context_t *ctx, struct json_lexer_token_t *t) {
    struct json_lexer_container_t *tokens = &ctx->tokens;
    struct json_lexer_token_t *oldList = NULL;
    struct json_lexer_token_t *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    if (!tokens) {
        fprintf(stderr, "Error Token Container is not initialized\n");
        json_print_trace();
        assert(0);
    }

    // If the token list is not yet allocated, initialize it with a capacity of 1.
    if (!tokens->list) {
        tokens->capacity = 1;
        tokens->list = (struct json_lexer_token_t *)malloc(sizeof(struct json_lexer_token_t) * tokens->capacity);
    }

    // If the token list is full, double its capacity.
    // This operation involves:
    // 1. Calculating the size of the old list.
    // 2. Allocating a new list with double the capacity.
    // 3. Copying the old tokens into the new list.
    // 4. Freeing the old list.
    if (tokens->length >= tokens->capacity) {
        oldSize = sizeof(struct json_lexer_token_t) * tokens->capacity;
        oldList = tokens->list;
        newSize = sizeof(struct json_lexer_token_t) * tokens->capacity * 2;
        newList = (struct json_lexer_token_t *)malloc(newSize);

        memcpy(newList, oldList, oldSize);
        free(oldList);
        oldList = NULL;

        tokens->list = newList;
        tokens->capacity *= 2;
    }

    t->index = tokens->length;

    tokens->list[tokens->length] = *t;
    tokens->length++;
}

static void get_tok_LPAIR(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset + 1;
    size_t startCol = ctx->column;

    match(ctx, '{');

    struct json_lexer_token_t token = {
        .type = JLT_LPAIR,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_RPAIR(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset + 1;
    size_t startCol = ctx->column;

    match(ctx, '}');

    struct json_lexer_token_t token = {
        .type = JLT_RPAIR,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_LARRAY(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset + 1;
    size_t startCol = ctx->column;

    match(ctx, '[');

    struct json_lexer_token_t token = {
        .type = JLT_LARRAY,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_RARRAY(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset + 1;
    size_t startCol = ctx->column;

    match(ctx, ']');

    struct json_lexer_token_t token = {
        .type = JLT_RARRAY,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_COMMA(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset + 1;
    size_t startCol = ctx->column;

    match(ctx, ',');

    struct json_lexer_token_t token = {
        .type = JLT_COMMA,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_COLON(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset + 1;
    size_t startCol = ctx->column;

    match(ctx, ':');

    struct json_lexer_token_t token = {
        .type = JLT_COLON,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_STRING(struct json_lexer_context_t *ctx) {
    size_t start = 0;
    size_t end = 0;
    size_t startCol = ctx->column;

    match(ctx, '"');

    start = ctx->offset;

    // '"' (~["\r\n] | '\\' . ) * '"'
    while (!(lookahead(ctx, '"') || lookahead(ctx, '\r') || lookahead(ctx, '\n') || lookahead(ctx, EOF))) {
        match_if_exist(ctx, '\\');
        next_char(ctx);
    }

    end = ctx->offset;

    match(ctx, '"');

    struct json_lexer_token_t token = {
        .type = JLT_STRING,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_NUMBER(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    // NOTE: This implementation uses a simple scan.
    // The valid JSON number format is:
    //   '-'? ('0' | [1-9][0-9]*) ('.' [0-9]+)? ([eE] [+-]? [0-9]+)?
    // Here, we check if the next character matches any of the valid components:
    // digits, minus sign, plus sign, decimal point, or exponent indicator.
    // We loop as long as any valid numeric character is found.
    while (match_between_if_exist(ctx, '0', '9') || match_if_exist(ctx, '-') || match_if_exist(ctx, '+') ||
           match_if_exist(ctx, '.') || match_if_exist(ctx, 'e') || match_if_exist(ctx, 'E'))
        continue;

    end = ctx->offset;

    struct json_lexer_token_t token = {
        .type = JLT_NUMBER,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_TRUE(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    match_str(ctx, "true");

    end = ctx->offset;

    struct json_lexer_token_t token = {
        .type = JLT_TRUE,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_FALSE(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    match_str(ctx, "false");

    end = ctx->offset;

    struct json_lexer_token_t token = {
        .type = JLT_FALSE,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

static void get_tok_NULL(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    match_str(ctx, "null");

    end = ctx->offset;

    struct json_lexer_token_t token = {
        .type = JLT_NULL,
        .text = substring(ctx, start, end),
        .index = 0,
        .start = start,
        .end = end,
        .column = startCol,
        .row = ctx->row,
    };

    insert_token(ctx, &token);
}

void json_execute_lexer(struct json_lexer_context_t *ctx) {

    while (!lookahead(ctx, EOF)) {

        if (lookahead(ctx, '{')) {
            get_tok_LPAIR(ctx);
        } else if (lookahead(ctx, '}')) {
            get_tok_RPAIR(ctx);
        } else if (lookahead(ctx, '[')) {
            get_tok_LARRAY(ctx);
        } else if (lookahead(ctx, ']')) {
            get_tok_RARRAY(ctx);
        } else if (lookahead(ctx, ',')) {
            get_tok_COMMA(ctx);
        } else if (lookahead(ctx, ':')) {
            get_tok_COLON(ctx);
        } else if (lookahead(ctx, '"')) {
            get_tok_STRING(ctx);
        } else if (lookahead(ctx, '-') || lookahead(ctx, '+') || lookahead(ctx, '.') ||
                   (lookahead_char(ctx) >= '0' && lookahead_char(ctx) <= '9')) {
            get_tok_NUMBER(ctx);
        } else if (lookahead(ctx, 't')) {
            get_tok_TRUE(ctx);
        } else if (lookahead(ctx, 'f')) {
            get_tok_FALSE(ctx);
        } else if (lookahead(ctx, 'n')) {
            get_tok_NULL(ctx);
        } else if (match_if_exist(ctx, '\n')) { // only match "\n", should consider these cases "\r\n" "\r"
            ctx->row++;
            ctx->column = 1;
        } else if (match_if_exist(ctx, ' ') || match_if_exist(ctx, '\t') || match_if_exist(ctx, '\r')) {
            continue;
        } else {
            match(ctx, EOF); // error
        }
    }
}

// --------------------------------------------------
// !SECTION: END JSON Lexer
// --------------------------------------------------

// --------------------------------------------------
// SECTION: JSON Parser
// --------------------------------------------------

struct json_parser_context_t *json_create_parser(struct json_lexer_context_t *lexer) {
    struct json_parser_context_t *parser_p =
        (struct json_parser_context_t *)malloc(sizeof(struct json_parser_context_t));

    struct json_parser_context_t parser = {
        .token_index = 0,
        .lexer = lexer,
    };

    *parser_p = parser;

    return parser_p;
}

void json_delete_parser(struct json_parser_context_t *ctx) { free(ctx); }

static struct json_lexer_token_t *current_token(struct json_parser_context_t *ctx) {
    size_t index = ctx->token_index - 1;
    if (index >= ctx->lexer->tokens.length)
        return NULL;
    return &ctx->lexer->tokens.list[index];
}

static void match_token(struct json_parser_context_t *ctx, enum json_lexer_token_type_t t) {
    size_t index = ctx->token_index;

    // always get next token before checking
    ctx->token_index++;

    if (index >= ctx->lexer->tokens.length) {
        fprintf(stderr, "Invalid Token Index: %lu expect <%d|%s>\n", index, t, json_lexer_type2str(t));
        json_print_trace();
        assert(0);
    }

    if (ctx->lexer->tokens.list[index].type != t) {
        fprintf(stderr, "Syntax Error: Unexpected Token at Token Index %lu: <%d|%s> expect <%d|%s>\n", index,
                ctx->lexer->tokens.list[index].type, json_lexer_type2str(ctx->lexer->tokens.list[index].type), t,
                json_lexer_type2str(t));
        json_print_trace();
        assert(0);
    }
}

static bool lookahead_n_token(struct json_parser_context_t *ctx, int n, enum json_lexer_token_type_t t) {
    size_t index = ctx->token_index + n;

    if (index >= ctx->lexer->tokens.length) {
        return false;
    }

    return ctx->lexer->tokens.list[index].type == t;
}

static bool lookahead_token(struct json_parser_context_t *ctx, enum json_lexer_token_type_t t) {
    return lookahead_n_token(ctx, 0, t);
}

// Forward declarations for the recursive descent parser rules.
static union json_t object_rule(struct json_parser_context_t *ctx);
static union json_t array_rule(struct json_parser_context_t *ctx);
static union json_t value_rule(struct json_parser_context_t *ctx);

static union json_t value_rule(struct json_parser_context_t *ctx) {
    struct json_lexer_token_t *token_p;

    // value : obj | arr | STRING | NUMBER | 'true' | 'false' | 'null' ;
    union json_t j;

    if (lookahead_token(ctx, JLT_LPAIR)) {
        j = object_rule(ctx);
    } else if (lookahead_token(ctx, JLT_LARRAY)) {
        j = array_rule(ctx);
    } else if (lookahead_token(ctx, JLT_STRING)) {
        match_token(ctx, JLT_STRING);
        token_p = current_token(ctx);
        j = JSON_STRING(json_strndup(token_p->text, token_p->end - token_p->start));
    } else if (lookahead_token(ctx, JLT_NUMBER)) {
        match_token(ctx, JLT_NUMBER);
        token_p = current_token(ctx);
        j = JSON_NUMBER(json_strndup(token_p->text, token_p->end - token_p->start));
    } else if (lookahead_token(ctx, JLT_TRUE)) {
        match_token(ctx, JLT_TRUE);
        j = JSON_TRUE;
    } else if (lookahead_token(ctx, JLT_FALSE)) {
        match_token(ctx, JLT_FALSE);
        j = JSON_FALSE;
    } else if (lookahead_token(ctx, JLT_NULL)) {
        match_token(ctx, JLT_NULL);
        j = JSON_NULL;
    } else {
        token_p = current_token(ctx);
        fprintf(stderr, "Syntax Error: Unexpected Token at Token Index %lu: <%d|%s>\n", ctx->token_index, token_p->type,
                json_lexer_type2str(token_p->type));
        json_print_trace();
        assert(0);
        // Syntax Error: Unexpected Token
    }

    return j;
}

static union json_t object_rule(struct json_parser_context_t *ctx) {
    union json_t jobj = JSON_OBJECT;
    struct json_lexer_token_t *key_tok;
    size_t key_len;
    const char *key;
    union json_t value;

    // pair : STRING ':' value ;
    // object : LPAIR pair (',' pair)* RPAIR | LPAIR RPAIR;
    match_token(ctx, JLT_LPAIR);

    while (!lookahead_token(ctx, JLT_RPAIR)) {
        /* Key */
        match_token(ctx, JLT_STRING);
        key_tok = current_token(ctx);
        key_len = key_tok->end - key_tok->start;
        key = key_tok->text;

        match_token(ctx, JLT_COLON);

        /* Value */
        value = value_rule(ctx);
        json_set_obj_value_np(&jobj, key, key_len, &value);

        if (lookahead_token(ctx, JLT_COMMA)) {
            match_token(ctx, JLT_COMMA);
        }
    }

    match_token(ctx, JLT_RPAIR);

    return jobj;
}

static union json_t array_rule(struct json_parser_context_t *ctx) {
    union json_t jarr = JSON_ARRAY;
    union json_t value;

    // array : LARRAY value (',' value)* RARRAY | LARRAY RARRAY ;
    match_token(ctx, JLT_LARRAY);

    while (!lookahead_token(ctx, JLT_RARRAY)) {
        value = value_rule(ctx);
        json_append_value_p(&jarr, &value);

        if (lookahead_token(ctx, JLT_COMMA)) {
            match_token(ctx, JLT_COMMA);
        }
    }

    match_token(ctx, JLT_RARRAY);

    return jarr;
}

void json_parse(struct json_parser_context_t *ctx) {
    // json : value EOF;
    ctx->root = value_rule(ctx);
}

union json_t json_string(const char *input_text) {
    struct json_lexer_context_t *lexer = json_create_lexer(input_text);
    struct json_parser_context_t *parser = json_create_parser(lexer);

    json_execute_lexer(lexer);
    json_print_lexer(lexer);
    json_parse(parser);

    union json_t j = parser->root;

    json_delete_lexer(lexer);
    json_delete_parser(parser);

    return j;
}

union json_t json_load(FILE *f) {
    if (!f) {
        fprintf(stderr, "Error: Could not open file\n");
        return JSON_MISSING;
    }

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *file_content = (char *)malloc(file_size + 1);
    if (!file_content) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(f);
        return JSON_MISSING;
    }

    fread(file_content, 1, file_size, f);
    file_content[file_size] = '\0';

    union json_t j = json_string(file_content);
    free(file_content);
    return j;
}

union json_t json_file(const char *file_path) {
    FILE *f = fopen(file_path, "r");
    if (!f) {
        fprintf(stderr, "Error: Could not open file %s\n", file_path);
        return JSON_MISSING;
    }

    union json_t j = json_load(f);
    fclose(f);
    return j;
}

// --------------------------------------------------
// !SECTION: END JSON Parser
// --------------------------------------------------
