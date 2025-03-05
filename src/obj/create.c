#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "jsonEditor.h"
#include "token.h"

struct json_obj_t __json_obj_create(size_t capacity) {
    if (capacity == 0) {
        capacity = 1;
    }

    struct json_pairs_t *pairs = (struct json_pairs_t *)malloc(sizeof(struct json_pairs_t) + (sizeof(struct json_pair_t) * capacity)); 

    return (struct json_obj_t) {
        .type = OBJECT,
        .pairs = pairs,
    };
}