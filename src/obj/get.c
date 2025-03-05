#include <stddef.h>
#include <stdint.h>
#include <string.h>

// #include "jsonEditor.h"
#include "token.h"

union json_value_t *__json_obj_get(struct json_obj_t obj, const char *key) {
    if (key && obj.pairs) {
        for (int i = 0; i < obj.pairs->length; i++) {
            if (strcmp(obj.pairs->list[i].key, key) == 0) {
                return &obj.pairs->list[i].value;
            }
        }
    }
    return NULL;
}

size_t __json_obj_length(struct json_obj_t obj) {
    return (obj.pairs) ? obj.pairs->length : 0;
}

size_t __json_obj_capacity(struct json_obj_t obj) {
    return (obj.pairs) ? obj.pairs->capacity : 0;
}

// union json_value_t __json_obj_get_copy(struct json_obj_t obj, const char *key) {
//     if (key && obj.pairs) {
//         for (int i = 0; i < obj.pairs->length; i++) {
//             if (strcmp(obj.pairs->list[i].key, key) == 0) {
//                 return dupToken(obj.pairs->list[i].value);
//             }
//         }
//     }
//     return (union json_value_t) {
//         .type = T_MISSING,
//     };
// }
