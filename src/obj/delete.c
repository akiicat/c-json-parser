#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "jsonEditor.h"
#include "token.h"

void __json_obj_delete(struct json_obj_t *obj, const char *key) {
    if (obj && obj->pairs) {
        for (int i = 0; i < obj->pairs->length; i++) {
            if (strcmp(obj->pairs->list[i].key, key) == 0) {
                free(obj->pairs->list[i].key);
                __json_clean_up(&obj->pairs->list[i].value);

                for (int j = i + 1; j < obj->pairs->length; i++, j++) {
                    obj->pairs->list[i] = obj->pairs->list[j];
                }
                obj->pairs->length--;
            }
        }
    }
}

void __json_obj_remove(struct json_obj_t *obj, union json_value_t value) {
    if (obj && obj->pairs) {
        for (int i = 0; i < obj->pairs->length; i++) {
            if (obj->pairs->list[i].value.type == value.type) {
                switch (value.type) {
                case T_STRING:
                    if (strcmp(obj->pairs->list[i].value.tok.text, value.tok.text) == 0) {
                        __json_obj_delete(obj, value.tok.text);
                        return;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    return;
}
