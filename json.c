#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "json.h"

#include <execinfo.h>
#include <string.h>

void print_trace(void) {
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

// --------------------------------------------------
// SECTION: JSON TOKEN
// --------------------------------------------------

const char *json_type2str(enum json_token_type_t type) {
    static const char *type_str[] = {
        "JT_MISSING", "JT_EMPTY", "JT_STRING", "JT_NUMBER", "JT_BOOL",   "JT_NULL",  "JT_INT",
        "JT_UINT",    "JT_FLOAT", "JT_DOUBLE", "JT_VALUE",  "JT_OBJECT", "JT_ARRAY", "JT_TOKEN_SIZE",
    };

    if (type >= JT_MISSING && type < JT_TOKEN_SIZE) {
        return type_str[type];
    }

    return NULL;
}

void insertPair(struct json_obj_t *obj, struct json_pair_t pair) {
    struct json_pairs_t *oldList = NULL;
    struct json_pairs_t *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    // set default token list
    if (!obj->pairs) {
        obj->pairs = (struct json_pairs_t *)malloc(sizeof(struct json_pairs_t) + sizeof(struct json_pair_t));
        obj->pairs->length = 0;
        obj->pairs->capacity = 1;
    }

    oldList = obj->pairs;
    oldSize = sizeof(struct json_pairs_t) + sizeof(struct json_pair_t) * obj->pairs->capacity;

    // double token list if full, time complexity O(3n)
    if (oldList->length >= oldList->capacity) {
        newSize = sizeof(struct json_pairs_t) + sizeof(struct json_pair_t) * oldList->capacity * 2;
        newList = (struct json_pairs_t *)malloc(newSize);

        memcpy(newList, oldList, oldSize);
        free(oldList);
        oldList = NULL;

        obj->pairs = newList;
        obj->pairs->capacity *= 2;
    }

    obj->pairs->list[obj->pairs->length++] = pair;
}

void insertValue(struct json_arr_t *arr, union json_t value) {
    struct json_values_t *oldList = NULL;
    struct json_values_t *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    // set default token list
    if (!arr->values) {
        arr->values = (struct json_values_t *)malloc(sizeof(struct json_values_t) + sizeof(union json_t));
        arr->values->length = 0;
        arr->values->capacity = 1;
    }

    oldList = arr->values;
    oldSize = sizeof(struct json_values_t) + sizeof(union json_t) * arr->values->capacity;

    // double token list if full, time complexity O(3n)
    if (oldList->length >= oldList->capacity) {
        newSize = sizeof(struct json_values_t) + sizeof(union json_t) * oldList->capacity * 2;
        newList = (struct json_values_t *)malloc(newSize);

        memcpy(newList, oldList, oldSize);
        free(oldList);
        oldList = NULL;

        arr->values = newList;
        arr->values->capacity *= 2;
    }

    arr->values->list[arr->values->length++] = value;
}

// --------------------------------------------------
// !SECTION: END JSON TOKEN
// --------------------------------------------------

// --------------------------------------------------
// SECTION: JSON COMMON FUNCTION
// --------------------------------------------------

struct json_tok_t __json_dup_term_tok(struct json_tok_t t) {
    size_t textSize = 0;
    char *text = NULL;

    if (t.text) {
        textSize = strlen(t.text);
        text = (char *)malloc(textSize + 1);

        strncpy(text, t.text, textSize);
        text[textSize] = '\0';

        t.text = text;
    }

    return t;
}

union json_t __json_dup(union json_t t) {
    union json_t res = {.type = JT_MISSING};

    switch (t.type) {
    case JT_STRING:
    case JT_NUMBER:
    case JT_BOOL:
    case JT_NULL:
    case JT_INT:
    case JT_UINT:
    case JT_FLOAT:
    case JT_DOUBLE: {
        res.tok = __json_dup_term_tok(t.tok);
        break;
    }
    case JT_ARRAY: {
        res.arr = (struct json_arr_t){
            .type = JT_ARRAY,
        };

        if (t.arr.values) {
            for (size_t i = 0; i < t.arr.values->length; i++) {
                insertValue(&res.arr, __json_dup(t.arr.values->list[i]));
            }
        }
        break;
    }
    case JT_OBJECT: {
        res.obj = (struct json_obj_t){
            .type = JT_OBJECT,
        };

        if (t.obj.pairs) {
            for (size_t i = 0; i < t.obj.pairs->length; i++) {
                struct json_pair_t pair = {
                    .key = strdup(t.obj.pairs->list[i].key),
                    .value = __json_dup(t.obj.pairs->list[i].value),
                };
                insertPair(&res.obj, pair);
            }
        }
        break;
    }
    default:
        fprintf(stderr, "%s: unsupport token <%d|%s>", __FUNCTION__, t.type, json_type2str(t.type));
        print_trace();
        break;
    }

    return res;
}

size_t json_length(union json_t j) {
    if (j.type == JT_OBJECT && j.obj.pairs) {
        return j.obj.pairs->length;
    } else if (j.type == JT_ARRAY && j.arr.values) {
        return j.arr.values->length;
    }
    return 0;
}

size_t json_capacity(union json_t j) {
    if (j.type == JT_OBJECT && j.obj.pairs) {
        return j.obj.pairs->capacity;
    } else if (j.type == JT_ARRAY && j.arr.values) {
        return j.arr.values->capacity;
    }
    return 0;
}

// TODO json_assign
// TODO void json_dump(char *s, size_t n);

void json_print(union json_t j) {

    static int indent = 0;

    switch (j.type) {
    case JT_STRING: {
        printf("\"%s\"", j.tok.text);
        break;
    }
    case JT_NUMBER: {
        printf("%s", j.tok.text);
        break;
    }
    case JT_BOOL: {
        printf(j.tok.boolean ? "true" : "false");
        break;
    }
    case JT_NULL: {
        printf("null");
        break;
    }
    case JT_INT: {
        printf("%ld", j.tok.u64);
        break;
    }
    case JT_UINT: {
        printf("%lu", j.tok.u64);
        break;
    }
    case JT_FLOAT: {
        printf("%f", j.tok.f32);
        break;
    }
    case JT_DOUBLE: {
        printf("%lf", j.tok.f64);
        break;
    }
    case JT_ARRAY: {
        printf("[\n");
        indent += 4;
        for (int i = 0; i < indent; i++)
            printf(" ");
        if (j.arr.values) {
            for (int i = 0; i < j.arr.values->length; i++) {
                json_print(j.arr.values->list[i]);
            }
        }
        printf("\n");
        indent -= 4;
        for (int i = 0; i < indent; i++)
            printf(" ");
        printf("]");
        break;
    }
    case JT_OBJECT: {
        printf("{\n");
        indent += 4;
        for (int i = 0; i < indent; i++)
            printf(" ");
        if (j.obj.pairs) {
            for (int i = 0; i < j.obj.pairs->length; i++) {
                printf("\"%s\": ", j.obj.pairs->list[i].key);
                json_print(j.obj.pairs->list[i].value);
                if (i + 1 < j.obj.pairs->length) {
                    printf(",\n");
                    for (int i = 0; i < indent; i++)
                        printf(" ");
                }
            }
        }
        printf("\n");
        indent -= 4;
        for (int i = 0; i < indent; i++)
            printf(" ");
        printf("}");
        break;
    }
    case JT_MISSING: {
        printf("MISSING\n");
        break;
    }
    default: {
        fprintf(stderr, "Error: Print Token Not found <%d|%s>\n", j.type, json_type2str(j.type));
        print_trace();
        assert(0);
    }
    }
}

void json_clean(union json_t *j) {
    switch (j->type) {
    case JT_STRING:
    case JT_NUMBER:
    case JT_BOOL:
    case JT_NULL:
    case JT_FLOAT:
    case JT_DOUBLE: {
        free(j->tok.text);
        j->tok.text = NULL;
        break;
    }
    case JT_INT:
    case JT_UINT:
    {
        break;
    }
    case JT_ARRAY: {
        struct json_arr_t *t1 = (struct json_arr_t *)j;
        if (t1->values) {
            for (int i = 0; i < t1->values->length; i++) {
                json_clean(&t1->values->list[i]);
            }
            free(t1->values);
            t1->values = NULL;
        }
        break;
    }
    case JT_OBJECT: {
        struct json_obj_t *t1 = (struct json_obj_t *)j;
        if (t1->pairs) {
            for (int i = 0; i < t1->pairs->length; i++) {
                free(t1->pairs->list[i].key);
                json_clean(&t1->pairs->list[i].value);
            }
            free(t1->pairs);
            t1->pairs = NULL;
        }
        break;
    }
    default:
        fprintf(stderr, "%s: unsupport token <%d|%s>", __FUNCTION__, j->type, json_type2str(j->type));
        break;
    }
}

// --------------------------------------------------
// !SECTION: END JSON COMMON FUNCTION
// --------------------------------------------------

// --------------------------------------------------
// SECTION: JSON OBJECT FUNCTION
// --------------------------------------------------
union json_t json_create_obj(size_t capacity) {
    struct json_pairs_t *pairs = (struct json_pairs_t *)malloc(sizeof(struct json_pairs_t) + (sizeof(struct json_pair_t) * capacity)); 

    return (union json_t) (struct json_obj_t) {
        .type = JT_OBJECT,
        .pairs = pairs,
    };
}

union json_t *__json_get_from_obj(union json_t j, const char *key) {
    if (key && j.obj.pairs) {
        for (int i = 0; i < j.obj.pairs->length; i++) {
            if (strcmp(j.obj.pairs->list[i].key, key) == 0) {
                return &j.obj.pairs->list[i].value;
            }
        }
    }
    return NULL;
}

void __json_delete_from_obj(union json_t *j, const char *key) {
    struct json_obj_t *obj = (struct json_obj_t *) j;
    if (obj && obj->pairs) {
        for (int i = 0; i < obj->pairs->length; i++) {
            if (strcmp(obj->pairs->list[i].key, key) == 0) {
                free(obj->pairs->list[i].key);
                json_clean(&obj->pairs->list[i].value);

                for (int j = i + 1; j < obj->pairs->length; i++, j++) {
                    obj->pairs->list[i] = obj->pairs->list[j];
                }
                obj->pairs->length--;
            }
        }
    }
}

int __json_set(union json_t *j, const char *key, union json_t value, bool copy_value) {
    if (value.type == JT_EMPTY) {
        __json_delete_from_obj(j, key);
        return 0;
    }

    union json_t *exist_value = __json_get_from_obj(*j, key);

    if (exist_value) {
        json_clean(exist_value);
        *exist_value = copy_value ? __json_dup(value) : value;
        return 1;
    }

    insertPair(&j->obj, (struct json_pair_t){
                              .key = (char *)strdup(key),
                              .value = copy_value ? __json_dup(value) : value,
                          });

    return 0;
}

int __json_set_obj(union json_t *j, const char *key, struct json_obj_t value) {
    return __json_set(j, key, (union json_t)value, true);
}

int __json_set_obj_p(union json_t *j, const char *key, struct json_obj_t *value) {
    return __json_set(j, key, (union json_t) * value, false);
}

int __json_set_arr(union json_t *j, const char *key, struct json_arr_t value) {
    return __json_set(j, key, (union json_t)value, true);
}

int __json_set_arr_p(union json_t *j, const char *key, struct json_arr_t *value) {
    return __json_set(j, key, (union json_t) * value, false);
}

int __json_set_token(union json_t *j, const char *key, struct json_tok_t value) {
    bool copy_value = value.type == JT_STRING || value.type == JT_NUMBER;
    return __json_set(j, key, (union json_t)value, copy_value);
}

int __json_set_token_p(union json_t *j, const char *key, struct json_tok_t *value) {
    return __json_set(j, key, (union json_t) * value, false);
}

int __json_set_value(union json_t *j, const char *key, union json_t value) {
    return __json_set(j, key, value, true);
}

int __json_set_value_p(union json_t *j, const char *key, union json_t *value) {
    return __json_set(j, key, *value, false);
}

int __json_set_str(union json_t *j, const char *key, const char *value) {
    return __json_set(j, key, (union json_t)JSON_STR((char *)value), true);
}

int __json_set_bool(union json_t *j, const char *key, bool value) {
    return __json_set(j, key, (union json_t)JSON_BOOL(value), false);
}

int __json_set_i8(union json_t *j, const char *key, int8_t value) {
    return __json_set(j, key, (union json_t)JSON_INT(value), false);
}

int __json_set_i16(union json_t *j, const char *key, int16_t value) {
    return __json_set(j, key, (union json_t)JSON_INT(value), false);
}

int __json_set_i32(union json_t *j, const char *key, int32_t value) {
    return __json_set(j, key, (union json_t)JSON_INT(value), false);
}

int __json_set_i64(union json_t *j, const char *key, int64_t value) {
    return __json_set(j, key, (union json_t)JSON_INT(value), false);
}

int __json_set_u8(union json_t *j, const char *key, uint8_t value) {
    return __json_set(j, key, (union json_t)JSON_UINT(value), false);
}

int __json_set_u16(union json_t *j, const char *key, uint16_t value) {
    return __json_set(j, key, (union json_t)JSON_UINT(value), false);
}

int __json_set_u32(union json_t *j, const char *key, uint32_t value) {
    return __json_set(j, key, (union json_t)JSON_UINT(value), false);
}

int __json_set_u64(union json_t *j, const char *key, uint64_t value) {
    return __json_set(j, key, (union json_t)JSON_UINT(value), false);
}

int __json_set_f32(union json_t *j, const char *key, float value) {
    return __json_set(j, key, (union json_t)JSON_FLOAT(value), false);
}

int __json_set_f64(union json_t *j, const char *key, double value) {
    return __json_set(j, key, (union json_t)JSON_DOUBLE(value), false);
}


// --------------------------------------------------
// !SECTION: END JSON OBJECT FUNCTION
// --------------------------------------------------

// --------------------------------------------------
// SECTION: JSON ARRAY FUNCTION
// --------------------------------------------------

union json_t *__json_get_from_arr(union json_t j, size_t i) {
    if (j.arr.values && i < j.arr.values->length) {
        return &j.arr.values->list[i];
    }
    return NULL;
}

// --------------------------------------------------
// !SECTION: END JSON ARRAY FUNCTION
// --------------------------------------------------

// --------------------------------------------------
// SECTION: JSON Lexer
// --------------------------------------------------
const char *json_lexer_type2str(enum json_lexer_token_type_t type) {
    static const char *type_string[] = {
        "JLT_MISSING", "JLT_STRING", "JLT_NUMBER",           "JLT_COMMA",  "JLT_COLON",
        "JLT_LPAIR",   "JLT_RPAIR",  "JLT_LARRAY",           "JLT_RARRAY", "JLT_TRUE",
        "JLT_FALSE",   "JLT_NULL",   "JLT_LEXER_TOKEN_SIZE",
    };

    if (type >= JLT_MISSING && type <= JLT_LEXER_TOKEN_SIZE) {
        return type_string[type];
    }

    return NULL;
}

struct json_lexer_context_t *json_create_lexer(FILE *stream) {
    struct json_lexer_context_t *ctx = malloc(sizeof(struct json_lexer_context_t));

    *ctx = (struct json_lexer_context_t){
        .tokens =
            {
                .length = 0,
                .capacity = 0,
                .list = NULL,
            },
        .currentChar = '\0',
        .offset = 0,
        .column = 0,
        .row = 1,
        .stream = stream,
    };

    return ctx;
}

void json_print_lexer(struct json_lexer_context_t *ctx) {
    struct json_lexer_token_t t;
    for (int i = 0; i < ctx->tokens.length; i++) {
        t = ctx->tokens.list[i];
        if (t.text) {
            printf("@%u#%u,%u<%u|%s>%u:%u %s\n", t.index, t.start, t.end, t.type, json_lexer_type2str(t.type), t.row,
                   t.column, t.text);
        } else {
            printf("@%u#%u,%u<%u|%s>%u:%u\n", t.index, t.start, t.end, t.type, json_lexer_type2str(t.type), t.row,
                   t.column);
        }
    }
}

void json_clean_lexer(struct json_lexer_context_t *ctx) {
    if (ctx && ctx->tokens.list) {

        // Free each token's text in reverse order
        for (unsigned int i = ctx->tokens.length; i > 0; i--) {
            free(ctx->tokens.list[i - 1].text);
            ctx->tokens.list[i - 1].text = NULL; // Defensive: nullify pointer after free
        }

        free(ctx->tokens.list);
    }

    free(ctx);
};

static void insert_token(struct json_lexer_context_t *ctx, struct json_lexer_token_t *t) {
    struct json_lexer_container_t *tokens = &ctx->tokens;
    struct json_lexer_token_t *oldList = NULL;
    struct json_lexer_token_t *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    if (!tokens) {
        fprintf(stderr, "Error Token Container is not initialized\n");
        print_trace();
        assert(0);
    }

    // set default token list
    if (!tokens->list) {
        tokens->capacity = 1;
        tokens->list = (struct json_lexer_token_t *)malloc(sizeof(struct json_lexer_token_t) * tokens->capacity);
    }

    // double token list if full, time complexity O(3n)
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

static int next_char(struct json_lexer_context_t *ctx) {
    if (!ctx->stream) {
        fprintf(stderr, "No Input stream\n");
        print_trace();
        assert(0);
    }

    ctx->currentChar = fgetc(ctx->stream);
    ctx->offset = ftell(ctx->stream);
    ctx->column++;
    return ctx->currentChar;
}

static bool lookahead(struct json_lexer_context_t *ctx, int c) { return ctx->currentChar == c; }

static void match(struct json_lexer_context_t *ctx, int c) {
    char curC = ctx->currentChar;

    // always get the next one before checking
    next_char(ctx);

    if (curC != c) {
        fprintf(stderr, "Syntax Error %d:%d: Unexpected Char: %c\n", ctx->row, ctx->column, curC);
        print_trace();
        assert(0);
    }
}

static void match_str(struct json_lexer_context_t *ctx, const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        match(ctx, str[i]);
    }
}

static bool match_if_exist(struct json_lexer_context_t *ctx, int c) {
    bool isMatch = (ctx->currentChar == c);

    if (isMatch) {
        next_char(ctx);
    }

    return isMatch;
}

static char *substring(struct json_lexer_context_t *ctx, size_t start, size_t end) {
    size_t textSize = 0;
    char *text = NULL;
    int error = 0;

    error = fseek(ctx->stream, start - 1, SEEK_SET);
    if (error) {
        fprintf(stderr, "Error fseek: %s\n", strerror(error));
        print_trace();
        assert(0);
    }

    textSize = end - start + 1;
    text = (char *)malloc(textSize + 1);
    fread(text, textSize, 1, ctx->stream);
    text[textSize] = '\0';

    error = fseek(ctx->stream, ctx->offset, SEEK_SET);
    if (error) {
        fprintf(stderr, "Error fseek: %s\n", strerror(error));
        print_trace();
        assert(0);
    }

    return text;
}

static void get_tok_LPAIR(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, '{');

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_LPAIR,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_RPAIR(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, '}');

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_RPAIR,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_LARRAY(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, '[');

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_LARRAY,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_RARRAY(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, ']');

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_RARRAY,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_COMMA(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, ',');

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_COMMA,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_COLON(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = ctx->offset;
    size_t startCol = ctx->column;

    match(ctx, ':');

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_COLON,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_STRING(struct json_lexer_context_t *ctx) {
    size_t start = 0;
    size_t end = 0;
    size_t startCol = ctx->column;

    match(ctx, '"');

    start = ctx->offset;

    // '"' (~["\r\n] | '\\' . ) * '"'
    while (!(lookahead(ctx, '"') || lookahead(ctx, '\r') || lookahead(ctx, '\r') || lookahead(ctx, EOF))) {
        match_if_exist(ctx, '\\');
        next_char(ctx);
    }

    end = ctx->offset - 1;

    match(ctx, '"');

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_STRING,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_NUMBER(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    // we should lookahead more, but here we get number simply
    // '-'? ('0' | [1-9][0-9]*)+ ('.' [0-9]+)? ([eE] [+-]? [0-9]+)?
    while (match_if_exist(ctx, '0') || match_if_exist(ctx, '1') || match_if_exist(ctx, '2') ||
           match_if_exist(ctx, '3') || match_if_exist(ctx, '4') || match_if_exist(ctx, '5') ||
           match_if_exist(ctx, '6') || match_if_exist(ctx, '7') || match_if_exist(ctx, '8') ||
           match_if_exist(ctx, '9') || match_if_exist(ctx, '-') || match_if_exist(ctx, '+') ||
           match_if_exist(ctx, '.') || match_if_exist(ctx, 'e') || match_if_exist(ctx, 'E'))
        ;

    end = ctx->offset - 1;

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_NUMBER,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_TRUE(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    match_str(ctx, "true");

    end = ctx->offset - 1;

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_TRUE,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_FALSE(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    match_str(ctx, "false");

    end = ctx->offset - 1;

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_FALSE,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

static void get_tok_NULL(struct json_lexer_context_t *ctx) {
    size_t start = ctx->offset;
    size_t end = 0;
    size_t startCol = ctx->column;

    match_str(ctx, "null");

    end = ctx->offset - 1;

    insert_token(ctx, &(struct json_lexer_token_t){
                          .type = JLT_NULL,
                          .start = start,
                          .end = end,
                          .column = startCol,
                          .row = ctx->row,
                          .text = substring(ctx, start, end),
                      });
}

void json_execute_lexer(struct json_lexer_context_t *ctx) {

    next_char(ctx);

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
        } else if (lookahead(ctx, '-') || lookahead(ctx, '0') || lookahead(ctx, '1') || lookahead(ctx, '2') ||
                   lookahead(ctx, '3') || lookahead(ctx, '4') || lookahead(ctx, '5') || lookahead(ctx, '6') ||
                   lookahead(ctx, '7') || lookahead(ctx, '8') || lookahead(ctx, '9')) {
            get_tok_NUMBER(ctx);
        } else if (lookahead(ctx, 't')) {
            get_tok_TRUE(ctx);
        } else if (lookahead(ctx, 'f')) {
            get_tok_FALSE(ctx);
        } else if (lookahead(ctx, 'n')) {
            get_tok_NULL(ctx);
        } else if (match_if_exist(ctx, '\n')) { // only match "\n", should consider these cases "\r\n" "\r"
            ctx->row++;
            ctx->column = 0;
        } else if (match_if_exist(ctx, ' ') || match_if_exist(ctx, '\t')) {
            continue;
        } else {
            match(ctx, EOF); // error
        }
    }
}

// --------------------------------------------------
// !SECTION: END JSON Lexer
// --------------------------------------------------