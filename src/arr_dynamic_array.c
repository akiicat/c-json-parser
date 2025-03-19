#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"

#define ARRAY_MIN_SIZE 8

struct my_array {
    size_t length;
    size_t capacity;
    union json_t **data;
};

size_t my_array_length(struct my_array *m) {
    return m ? m->length : 0;
}

size_t my_array_capacity(struct my_array *m) {
    return m ? m->capacity : 0;
}

void my_array_free(struct my_array *m) {
    if (m) free(m->data);
    free(m);
}

struct my_array *my_array_new(size_t capacity) {
    struct my_array *m = (struct my_array *)malloc(sizeof(struct my_array));
    if (!m) return NULL;

    m->data = (union json_t **)calloc(capacity, sizeof(union json_t *));
    if (!m->data) {
        my_array_free(m);
        return NULL;
    }

    m->length = 0;
    m->capacity = capacity;

    return m;
}

bool my_array_expand(struct my_array *m, size_t new_size) {
    union json_t **curr;

    if (!m) return false;
    if (m->length > new_size) return false;

    /* Setup the new elements */
    union json_t **temp = (union json_t **)calloc(new_size, sizeof(union json_t *));
    if (!temp) return false;

    curr = m->data;
    m->data = temp;

    m->capacity = new_size;

    memcpy(m->data, curr, m->length * sizeof(union json_t *));

    free(curr);

    return temp;
}

void my_array_push_back(struct my_array *m, union json_t *j) {

    if (m->length == m->capacity) {
        my_array_expand(m, 2 * m->capacity);
    }

    m->data[m->length++] = j;
}

union json_t *my_array_get(struct my_array *m, size_t i) {
    return (i <= m->length) ? m->data[i] : NULL;
}

union json_t *my_array_delete(struct my_array *m, size_t i) {
    union json_t *del;

    if (!m) return NULL;
    if (i >= m->length) return NULL;

    /* Return deleted element's pointer */
    del = m->data[i];
    m->length--;

    /* Shift left */
    for (; i < m->length; i++) {
        m->data[i] = m->data[i+1];
    }

    if (m->capacity / 2 >= ARRAY_MIN_SIZE && m->length <= m->capacity / 4) {
        printf("%s:%d length=%ld capacity=%ld\n", __FUNCTION__, __LINE__, m->length, m->capacity);
        my_array_expand(m, m->capacity / 2);
    }

    return del;
}

void jsonext_arr_new(union json_t *j, size_t capacity) {
    j->arr.values = my_array_new(capacity);
}

void jsonext_arr_append(union json_t *j, union json_t *value) {
    if (!j->arr.values) {
        j->arr.values = my_array_new(ARRAY_MIN_SIZE);
    }
    my_array_push_back(j->arr.values, value);
}

union json_t *jsonext_arr_get(union json_t *j, size_t index) {
    return my_array_get(j->arr.values, index);
}

union json_t *jsonext_arr_delete(union json_t *j, size_t index) {
    return my_array_delete(j->arr.values, index);
}

void jsonext_arr_clean(union json_t *j) {
    my_array_free(j->arr.values);
    j->arr.values = NULL;
}

size_t jsonext_arr_length(union json_t *j) {
    return my_array_length(j->arr.values);
}

size_t jsonext_arr_capacity(union json_t *j) {
    return my_array_capacity(j->arr.values);
}

