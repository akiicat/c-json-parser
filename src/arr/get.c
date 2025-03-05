#include <stddef.h>
#include <stdint.h>
#include <string.h>

// #include "jsonEditor.h"
#include "token.h"

union json_value_t *__json_arr_get(struct json_arr_t arr, size_t i) {
    if (arr.values && i < arr.values->length) {
        return &arr.values->list[i];
    }
    return NULL;
}

size_t __json_arr_length(struct json_arr_t arr) {
    return (arr.values) ? arr.values->length : 0;
}

size_t __json_arr_capacity(struct json_arr_t arr) {
    return (arr.values) ? arr.values->capacity : 0;
}

// union json_value_t __json_arr_get_copy(struct json_arr_t arr, const char *key) {
//     if (key && arr.values) {
//         for (int i = 0; i < arr.values->length; i++) {
//             if (strcmp(arr.values->list[i].key, key) == 0) {
//                 return dupToken(arr.values->list[i].value);
//             }
//         }
//     }
//     return (union json_value_t) {
//         .type = T_MISSING,
//     };
// }
