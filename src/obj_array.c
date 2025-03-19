#include <stdlib.h>
#include <string.h>

#include "../json.h"

__attribute__((weak)) void json_insert_pair(union json_t *j, struct json_pair_t pair) {
    struct json_obj_t *obj = (struct json_obj_t *)j;
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


// __attribute__((weak)) void json_insert_pair(union json_t *j, struct json_pair_t pair) {
//     struct json_obj_t *obj = (struct json_obj_t *)j;
//     struct json_pairs_t *oldList = NULL;
//     struct json_pairs_t *newList = NULL;
//     size_t oldSize = 0;
//     size_t newSize = 0;

//     // set default token list
//     if (!obj->pairs) {
//         obj->pairs = (struct json_pairs_t *)malloc(sizeof(struct json_pairs_t) + sizeof(struct json_pair_t));
//         obj->pairs->length = 0;
//         obj->pairs->capacity = 1;
//     }

//     oldList = obj->pairs;
//     oldSize = sizeof(struct json_pairs_t) + sizeof(struct json_pair_t) * obj->pairs->capacity;

//     // double token list if full, time complexity O(3n)
//     if (oldList->length >= oldList->capacity) {
//         newSize = sizeof(struct json_pairs_t) + sizeof(struct json_pair_t) * oldList->capacity * 2;
//         newList = (struct json_pairs_t *)malloc(newSize);

//         memcpy(newList, oldList, oldSize);
//         free(oldList);
//         oldList = NULL;

//         obj->pairs = newList;
//         obj->pairs->capacity *= 2;
//     }

//     obj->pairs->list[obj->pairs->length++] = pair;
// }

// void insertValue(struct json_arr_t *arr, union json_t value) {
//     struct json_values_t *oldList = NULL;
//     struct json_values_t *newList = NULL;
//     size_t oldSize = 0;
//     size_t newSize = 0;

//     // set default token list
//     if (!arr->values) {
//         arr->values = (struct json_values_t *)malloc(sizeof(struct json_values_t) + sizeof(union json_t));
//         arr->values->length = 0;
//         arr->values->capacity = 1;
//     }

//     oldList = arr->values;
//     oldSize = sizeof(struct json_values_t) + sizeof(union json_t) * arr->values->capacity;

//     // double token list if full, time complexity O(3n)
//     if (oldList->length >= oldList->capacity) {
//         newSize = sizeof(struct json_values_t) + sizeof(union json_t) * oldList->capacity * 2;
//         newList = (struct json_values_t *)malloc(newSize);

//         memcpy(newList, oldList, oldSize);
//         free(oldList);
//         oldList = NULL;

//         arr->values = newList;
//         arr->values->capacity *= 2;
//     }

//     arr->values->list[arr->values->length++] = value;
// }