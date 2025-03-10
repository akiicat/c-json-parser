#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../json.h"

/* This value is between 0 and 1. If it defines 0.3, it means at least 70% of space will be waste. */
#define HASHMAP_MIN_SIZE 8
#define HASHMAP_FILL_FACTOR 0.5

// We need to keep keys and values
struct hashmap_element {
    const char *key;
    struct json_pair_t *value;
    bool in_use;
};

// A hashmap has some maximum size and current size,
// as well as the data to hold.
struct hashmap_map {
    size_t table_size;
    size_t size;
    struct hashmap_element *data;
};

void hashmap_free(struct hashmap_map *m) {
    if (m) free(m->data);
    free(m);
}

/* Return the length of the hashmap */
size_t hashmap_length(struct hashmap_map *m) { return m ? m->size : 0; }
size_t hashmap_capacity(struct hashmap_map *m) { return m ? m->table_size : 0; }

struct hashmap_map *hashmap_new(size_t capacity) {
    struct hashmap_map *m = (struct hashmap_map *)malloc(sizeof(struct hashmap_map));
    if (!m) return NULL;

    m->data = (struct hashmap_element *)calloc(capacity, sizeof(struct hashmap_element));
    if (!m->data) {
        hashmap_free(m);
        return NULL;
    }

    m->table_size = capacity;
    m->size = 0;

    return m;
}

/* djb2 */
unsigned long hash_str(const char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/*
 * Return the integer of the location in data
 * to store the point to the item, or Table Size.
 */
size_t hashmap_hash(struct hashmap_map *m, const char *key) {
    /* Find the best index */
    size_t curr = hash_str(key) % m->table_size;

    /* Linear probling */
    for (size_t i = 0; i < m->table_size; i++) {
        if (!m->data[curr].in_use)
            return curr;

        if (strcmp(m->data[curr].key, key) == 0)
            return curr;

        curr = (curr + 1) % m->table_size;
    }

    /* Cannot Found Space */
    return m->table_size;
}

bool hashmap_put(struct hashmap_map *m, const char *key, struct json_pair_t *value);

/*
 * Doubles the size of the hashmap, and rehashes all the elements
 */
bool hashmap_rehash(struct hashmap_map *m, size_t new_size) {
    size_t old_size;
    struct hashmap_element *curr;

    if (m->size > new_size) return false;

    /* Setup the new elements */
    struct hashmap_element *temp = (struct hashmap_element *)calloc(new_size, sizeof(struct hashmap_element));
    if (!temp) return false;

    /* Update the array */
    curr = m->data;
    m->data = temp;

    /* Update the size */
    old_size = m->table_size;
    m->table_size = new_size;
    m->size = 0;

    /* Rehash the elements */
    for (size_t i = 0; i < old_size; i++) {
        if (curr[i].in_use) {
            hashmap_put(m, curr[i].key, curr[i].value);
        }
    }

    free(curr);

    return true;
}

/*
 * Add a pointer to the hashmap with some key
 */
bool hashmap_put(struct hashmap_map *m, const char *key, struct json_pair_t *value) {
    size_t index;

    /* If full, resize immediately */
    if (m->size >= m->table_size * HASHMAP_FILL_FACTOR) {
        hashmap_rehash(m, 2 * m->table_size);
    }

    /* Find a place to put our value */
    index = hashmap_hash(m, key);

    /* Location not found */
    if (index == m->table_size) {
        return false;
    }

    /* Set the data */
    m->data[index].value = value;
    m->data[index].key = key;
    m->data[index].in_use = 1;
    m->size++;

    return true;
}

/*
 * Get your pointer out of the hashmap with a key
 */
struct json_pair_t *hashmap_get(struct hashmap_map *m, const char *key) {

    /* Find data location */
    size_t curr = hashmap_hash(m, key);

    /* Linear probing, if necessary */
    for (size_t i = 0; i < m->table_size; i++) {
        if (m->data[curr].in_use && strcmp(m->data[curr].key, key) == 0) {
            return m->data[curr].value;
        }
        curr = (curr + 1) % m->table_size;
    }

    /* Not found */
    return NULL;
}

/*
 * Get first element in hashmap_map.
 */
struct json_pair_t *hashmap_get_first(struct hashmap_map *m) {
    /* Linear probing, if necessary */
    for (size_t i = 0; i < m->table_size; i++) {
        if (m->data[i].in_use) {
            return m->data[i].value;
        }
    }

    /* Not found */
    return NULL;
}

/*
 * Get following element.
 */
struct json_pair_t *hashmap_get_next(struct hashmap_map *m, struct json_pair_t *prev) {
    /* Linear probing, if necessary */
    for (size_t i = hashmap_hash(m, prev->key) + 1; i < m->table_size; i++) {
        if (m->data[i].in_use) {
            return m->data[i].value;
        }
    }

    /* Not found */
    return NULL;
}


/*
 * Iterate the function parameter over each element in the hashmap.  The
 * additional argument is passed to the function as its third
 * argument and the hashmap element is the second.
 */
void hashmap_iterate(struct hashmap_map *m, json_obj_iter_cb f, void *args) {
    size_t i;

    /* On empty hashmap, return immediately */
    if (hashmap_length(m) <= 0)
        return;

    /* Linear probing */
    for (i = 0; i < m->table_size; i++) {
        if (m->data[i].in_use != 0) {
            f(m->data[i].value, args);
        }
    }
}

/*
 * Remove an element with that key from the map, we only need pair.
 */
struct json_pair_t *hashmap_delete(struct hashmap_map *m, const char *key) {
    size_t i;
    size_t curr;
    struct json_pair_t *delete_pair = NULL;

    /* Find key */
    curr = hashmap_hash(m, key);

    /* Linear probing, if necessary */
    for (i = 0; i < m->table_size; i++) {
        if (m->data[curr].in_use && strcmp(m->data[curr].key, key) == 0) {
            /* Set found data */
            delete_pair = m->data[curr].value;

            /* Blank out the fields */
            m->data[curr].in_use = 0;
            m->data[curr].value = NULL;
            m->data[curr].key = 0;

            /* Reduce the size */
            m->size--;
            break;
        }
        curr = (curr + 1) % m->table_size;
    }

    if (m->table_size / 2 >= HASHMAP_MIN_SIZE && m->size <= m->table_size * 0.5 * HASHMAP_FILL_FACTOR) {
        hashmap_rehash(m, m->table_size / 2);
    }

    return delete_pair;
}

void jsonext_obj_new(union json_t *j, size_t capacity) {
    if (!j->obj.pairs) {
        j->obj.pairs = hashmap_new(capacity);
    } else {
        hashmap_rehash(j->obj.pairs, capacity);
    }
}

// don't need dup key and value, but it can give value a unique address by malloc
// we gurrentee j is not NULL and j->type is JT_OBJECT.
void jsonext_obj_insert(union json_t *j, struct json_pair_t *pair) {
    char *key = pair->key;
    if (!j->obj.pairs) {
        j->obj.pairs = hashmap_new(HASHMAP_MIN_SIZE);
    }
    bool put_res = hashmap_put(j->obj.pairs, key, pair);
    printf("%s:%d result=%d\n", __FUNCTION__, __LINE__, put_res);
}

struct json_pair_t *jsonext_obj_get(union json_t *j, const char *key) {
	if (j->obj.pairs) {
		return hashmap_get(j->obj.pairs, key);
	}
	return NULL;
}

struct json_pair_t *jsonext_obj_delete(union json_t *j, const char *key) {
    if (j->obj.pairs) {
        return hashmap_delete(j->obj.pairs, key);
    }
    return NULL;
}

size_t jsonext_obj_length(union json_t *j) {
	return hashmap_length(j->obj.pairs);
}

size_t jsonext_obj_capacity(union json_t *j) {
	return hashmap_capacity(j->obj.pairs);
}

void jsonext_obj_clean(union json_t *j) {
    hashmap_free(j->obj.pairs);
    j->obj.pairs = NULL;
}

void jsonext_obj_iter(union json_t *j, json_obj_iter_cb f, void *fargs) {
	hashmap_iterate(j->obj.pairs, f, fargs);
}

struct json_pair_t *jsonext_obj_iter_first(union json_t *j) {
    if (j->obj.pairs) {
        return hashmap_get_first(j->obj.pairs);
    }
    return NULL;
}

struct json_pair_t *jsonext_obj_iter_next(union json_t *j, struct json_pair_t *pair) {
    if (j->obj.pairs) {
        return hashmap_get_next(j->obj.pairs, pair);
    }
    return NULL;
}