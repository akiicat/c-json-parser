#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "debug.h"
#include "jsonLexer.h"
#include "jsonParser.h"
#include "token.h"
#include "jsonEditor.h"

void __json_clean_up(union json_value_t *base) {
    switch (base->type) {
    case T_STRING:
    case T_NUMBER:
    case T_BOOL:
    case T_NULL:
    case T_INT8:
    case T_INT16:
    case T_INT32:
    case T_INT64:
    case T_UINT8:
    case T_UINT16:
    case T_UINT32:
    case T_UINT64:
    case T_FLOAT:
    case T_DOUBLE:
    {
        free(base->tok.text);
        base->tok.text = NULL;
        break;
    }
    case ARRAY:
    {
        struct json_arr_t *t1 = (struct json_arr_t *)base;
        if (t1->values) {
            for (int i = 0; i < t1->values->length; i++) {
                __json_clean_up(&t1->values->list[i]);
            }
            free(t1->values);
            t1->values = NULL;
        }
        break;
    }
    case OBJECT:
    {
        struct json_obj_t *t1 = (struct json_obj_t *)base;
        if (t1->pairs) {
            for (int i = 0; i < t1->pairs->length; i++) {
                free(t1->pairs->list[i].key);
                __json_clean_up(&t1->pairs->list[i].value);
            }
            free(t1->pairs);
            t1->pairs = NULL;
        }
        break;
    }
    default:
        fprintf(stderr, "%s: unsupport token <%d|%s>", __FUNCTION__, base->type, type2str(base->type));
        break;
    }
}

void printJson(union json_value_t token) {

    static int indent = 0;

    switch (token.type) {
        case T_STRING:
        {
            printf("\"%s\"", token.tok.text);
            break;
        }
        case T_NUMBER:
        {
            printf("%s", token.tok.text);
            break;
        }
        case T_BOOL:
        {
            printf(token.tok.boolean ? "true" : "false");
            break;
        }
        case T_NULL:
        {
            printf("null");
            break;
        }
        case T_INT8:
        case T_INT16:
        case T_INT32:
        case T_INT64:
        {
            printf("%ld", token.tok.u64);
            break;
        }
        case T_UINT8:
        case T_UINT16:
        case T_UINT32:
        case T_UINT64:
        {
            printf("%lu", token.tok.u64);
            break;
        }
        case T_FLOAT:
        {
            printf("%f", token.tok.f32);
            break;
        }
        case T_DOUBLE:
        {
            printf("%lf", token.tok.f64);
            break;
        }
        case ARRAY:
        {
            printf("[\n");
            indent += 4;
            for (int i = 0; i < indent; i++)
                printf(" ");
            if (token.arr.values) {
                for (int i = 0; i < token.arr.values->length; i++) {
                    printJson(token.arr.values->list[i]);
                }
            }
            printf("\n");
            indent -= 4;
            for (int i = 0; i < indent; i++)
                printf(" ");
            printf("]");
            break;
        }
        case OBJECT:
        {
            printf("{\n");
            indent += 4;
            for (int i = 0; i < indent; i++)
                printf(" ");
            if (token.obj.pairs) {
                for (int i = 0; i < token.obj.pairs->length; i++) {
                    printf("\"%s\": ", token.obj.pairs->list[i].key);
                    printJson(token.obj.pairs->list[i].value);
                    if (i + 1 < token.obj.pairs->length) {
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
        case T_MISSING:
        {
            printf("MISSING\n");
            break;
        }
        default:
        {
            fprintf(stderr, "Error: Print Token Not found <%d|%s>\n", token.type, type2str(token.type));
            print_trace();
            assert(0);
        }
    }
}


const char *typeString[] = {
    "T_MISSING",
    "T_STRING",
    "T_NUMBER",
    "T_BOOL",
    "T_NULL",
    "T_INT8",
    "T_INT16",
    "T_INT32",
    "T_INT64",
    "T_UINT8",
    "T_UINT16",
    "T_UINT32",
    "T_UINT64",
    "T_FLOAT",
    "T_DOUBLE",
    "VALUE",
    "OBJECT",
    "ARRAY",
    "TOKEN_SIZE",
};

const char *type2str(enum json_type_t type) {
    if (type >= T_MISSING && type < TOKEN_SIZE) {
        return typeString[type];
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

void insertValue(struct json_arr_t *arr, union json_value_t value) {
    struct json_values_t *oldList = NULL;
    struct json_values_t *newList = NULL;
    size_t oldSize = 0;
    size_t newSize = 0;

    // set default token list
    if (!arr->values) {
        arr->values = (struct json_values_t *)malloc(sizeof(struct json_values_t) + sizeof(union json_value_t));
        arr->values->length = 0;
        arr->values->capacity = 1;
    }

    oldList = arr->values;
    oldSize = sizeof(struct json_values_t) + sizeof(union json_value_t) * arr->values->capacity;

    // double token list if full, time complexity O(3n)
    if (oldList->length >= oldList->capacity) {
        newSize = sizeof(struct json_values_t) + sizeof(union json_value_t) * oldList->capacity * 2;
        newList = (struct json_values_t *)malloc(newSize);

        memcpy(newList, oldList, oldSize);
        free(oldList);
        oldList = NULL;

        arr->values = newList;
        arr->values->capacity *= 2;
    }

    arr->values->list[arr->values->length++] = value;
}

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

union json_value_t __json_dup_nonterm_tok(union json_value_t t) {
    union json_value_t res = { .type = T_MISSING };

    switch (t.type) {
    case T_STRING:
    case T_NUMBER:
    case T_BOOL:
    case T_NULL:
    case T_INT8:
    case T_INT16:
    case T_INT32:
    case T_INT64:
    case T_UINT8:
    case T_UINT16:
    case T_UINT32:
    case T_UINT64:
    case T_FLOAT:
    case T_DOUBLE:
    {
        res.tok = __json_dup_term_tok(t.tok);
        break;
    }
    case ARRAY:
    {
        res.arr = (struct json_arr_t) {
            .type = ARRAY,
        };

        if (t.arr.values) {
            for (size_t i = 0; i < t.arr.values->length; i++) {
                insertValue(&res.arr, __json_dup_nonterm_tok(t.arr.values->list[i]));
            }
        }
        break;
    }
    case OBJECT:
    {
        res.obj = (struct json_obj_t) {
            .type = OBJECT,
        };

        if (t.obj.pairs) {
            for (size_t i = 0; i < t.obj.pairs->length; i++) {
                struct json_pair_t pair = {
                    .key = strdup(t.obj.pairs->list[i].key),
                    .value = __json_dup_nonterm_tok(t.obj.pairs->list[i].value),
                };
                insertPair(&res.obj, pair);
            }
        }
        break;
    }
    default:
        fprintf(stderr, "%s: unsupport token <%d|%s>", __FUNCTION__, t.type, type2str(t.type));
        print_trace();
        break;
    }

    return res;
}
