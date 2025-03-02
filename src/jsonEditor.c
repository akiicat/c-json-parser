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

void freeValue(union valueToken *base) {
    switch (base->type) {
    case T_STRING:
    case T_NUMBER:
    case T_TRUE:
    case T_FALSE:
    case T_NULL:
    {
        free(base->anyToken.text);
        base->anyToken.text = NULL;
        break;
    }
    case ARR:
    {
        struct arrToken *t1 = (struct arrToken *)base;
        if (t1->values) {
            for (int i = 0; i < t1->values->length; i++) {
                freeValue(&t1->values->list[i]);
            }
            free(t1->values);
            t1->values = NULL;
        }
        break;
    }
    case OBJ:
    {
        struct objToken *t1 = (struct objToken *)base;
        if (t1->pairs) {
            for (int i = 0; i < t1->pairs->length; i++) {
                freeValue((union valueToken *)&t1->pairs->list[i].key);
                freeValue(&t1->pairs->list[i].value);
            }
            free(t1->pairs);
            t1->pairs = NULL;
        }
        break;
    }
    default:
        fprintf(stderr, "dupToken: unsupport token <%d|%s>", base->type, type2str(base->type));
        break;
    }
}


struct objToken *createObj() {
    struct objToken *obj = (struct objToken *)malloc(sizeof(struct objToken));

    *obj = (struct objToken) {
        .type = OBJ,
    };

    return obj;
}

void freeObj(struct objToken *obj) {
    freeValue((union valueToken *)obj);
    free(obj);
}

struct objToken *dupObj(struct objToken *obj) {
    struct objToken *res = createObj();
    *res = dupNonTerminalToken((union valueToken)*obj).obj;
    return res;
}

struct arrToken *createArr() {
    struct arrToken *arr = (struct arrToken *)malloc(sizeof(struct arrToken));

    *arr = (struct arrToken) {
        .type = ARR,
    };

    return arr;
}

void freeArr(struct arrToken *arr) {
    freeValue((union valueToken *)arr);
    free(arr);
}


void objDelete(struct objToken *obj, const char *key) {
    for (int i = 0; i < obj->pairs->length; i++) {
        if (strlen(obj->pairs->list[i].key.text) == strlen(key) && strcmp(obj->pairs->list[i].key.text, key) == 0) {
            freeValue((union valueToken *)&obj->pairs->list[i].key);
            freeValue(&obj->pairs->list[i].value);

            for (int j = i + 1; j < obj->pairs->length; i++, j++) {
                obj->pairs->list[i] = obj->pairs->list[j];
            }
            obj->pairs->length--;
        }
    }
}

union valueToken *objFind(struct objToken *obj, const char *key) {
    if (key && obj && obj->pairs) {
        for (int i = 0; i < obj->pairs->length; i++) {
            if (strlen(obj->pairs->list[i].key.text) == strlen(key) && strcmp(obj->pairs->list[i].key.text, key) == 0) {
                return &obj->pairs->list[i].value;
            }
        }
    }
    return NULL;
}

void printJson(union valueToken token) {

    static int indent = 0;

    switch (token.type) {
        case T_STRING:
        {
            printf("\"%s\"", token.anyToken.text);
            break;
        }
        case T_NUMBER:
        {
            printf("%s", token.anyToken.text);
            break;
        }
        case T_TRUE:
        {
            printf("true");
            break;
        }
        case T_FALSE:
        {
            printf("false");
            break;
        }
        case T_NULL:
        {
            printf("null");
            break;
        }
        case ARR:
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
        case OBJ:
        {
            printf("{\n");
            indent += 4;
            for (int i = 0; i < indent; i++)
                printf(" ");
            if (token.obj.pairs) {
                for (int i = 0; i < token.obj.pairs->length; i++) {
                    printJson((union valueToken)token.obj.pairs->list[i].key);
                    printf(": ");
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
