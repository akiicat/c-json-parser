#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "jsonEditor.h"
#include "token.h"

struct json_arr_t __json_arr_create(size_t capacity) {
    if (capacity == 0) {
        capacity = 1;
    }

    struct json_values_t *values = (struct json_values_t *)malloc(sizeof(struct json_values_t) + (sizeof(union json_value_t) * capacity)); 

    return (struct json_arr_t) {
        .type = OBJECT,
        .values = values,
    };
}