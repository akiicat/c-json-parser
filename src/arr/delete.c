#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "jsonEditor.h"
#include "token.h"

void __json_arr_delete(struct json_arr_t *arr, size_t i) {
    if (arr && arr->values && i < arr->values->length) {
        __json_clean_up(&arr->values->list[i]);
        
        for (int j = i + 1; j < arr->values->length; i++, j++) {
            arr->values->list[i] = arr->values->list[j];
        }
        arr->values->length--;
    }
}

