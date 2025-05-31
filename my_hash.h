#ifndef MY_HASH_H_
#define MY_HASH_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define MY_HASH_IMPL

#ifndef HASHMAP_CAP
#define HASHMAP_CAP 10
#endif

typedef size_t(HashFN)(const void *, size_t cap);

typedef struct HashMapHeader {
    HashFN *hashFN;
    size_t cap;
} HashMapHeader;

typedef struct Entry {
    void *next;
    char *key;
} Entry;

#define hm_init(T) (T *)init_hashmap(sizeof(T), default_string_hash)
#define hm_deinit(hm) free_hashmap((void **)&hm, sizeof(typeof(*hm)))
#define hm_header(hm) (((HashMapHeader *)hm) - 1)
#define hm_print_pro(hm) print_hashmap_pro(hm, sizeof(typeof(*hm)))
#define hm_contains(hm, key)                                                   \
    ((hm) != NULL ? contains_hashmap((hm), sizeof(typeof(*(hm))), (key)) : 0)
#define hm_get(hm, key)                                                        \
    ((hm) != NULL ? ({                                                         \
        Entry *entry = get_hashmap(hm, sizeof(typeof(*(hm))), (key));          \
        entry ? *(typeof((hm)->value) *)(entry + 1) : (typeof((hm)->value))0;  \
    })                                                                         \
                  : (typeof((hm)->value))0)
#define hm_delete(hm, key)                                                     \
    do {                                                                       \
        if (hm == NULL)                                                        \
            break;                                                             \
        delete_hashmap(hm, sizeof(typeof(*(hm))), key);                        \
    } while (0)
#define hm_set(hm, key, value)                                                 \
    do {                                                                       \
        if (hm == NULL)                                                        \
            break;                                                             \
        typeof(*hm) entry = {                                                  \
            NULL,                                                              \
            strdup(key),                                                       \
            value,                                                             \
        };                                                                     \
        set_hashmap(hm, sizeof(typeof(*hm)), (Entry *)&entry);                 \
    } while (0)

int contains_hashmap(void *self, size_t entry_size, const char *key)
#ifdef MY_HASH_IMPL
{
#define GET(i) (*((Entry *)(self + i * entry_size)))
    HashMapHeader *header = hm_header(self);
    size_t hash = header->hashFN(key, header->cap);
    size_t actual_hash = hash % header->cap;

    if (GET(actual_hash).key == NULL) {
        return 0;
    }

    if (strcmp(GET(actual_hash).key, key) == 0) {
        return 1;
    }

    Entry *current = GET(actual_hash).next;
    while (current != NULL) {
        Entry *next = current->next;

        if (strcmp(current->key, key) == 0) {
            return 1;
        }

        current = next;
    }

    return 0;
#undef GET
}
#else
    ;
#endif

void *get_hashmap(void *self, size_t entry_size, const char *key)
#ifdef MY_HASH_IMPL
{
#define GET(i) (*((Entry *)(self + i * entry_size)))
    HashMapHeader *header = hm_header(self);
    size_t hash = header->hashFN(key, header->cap);
    size_t actual_hash = hash % header->cap;

    if (GET(actual_hash).key == NULL) {
        return NULL;
    }

    if (strcmp(GET(actual_hash).key, key) == 0) {
        return &GET(actual_hash);
    }

    Entry *current = GET(actual_hash).next;
    while (current != NULL) {
        Entry *next = current->next;

        if (strcmp(current->key, key) == 0) {
            return current;
        }

        current = next;
    }

    return NULL;
#undef GET
}
#else
    ;
#endif

void delete_hashmap(void *self, size_t entry_size, const char *key)
#ifdef MY_HASH_IMPL
{
#define GET(i) (*((Entry *)(self + i * entry_size)))
    HashMapHeader *header = hm_header(self);
    size_t hash = header->hashFN(key, header->cap);
    size_t actual_hash = hash % header->cap;

    if (GET(actual_hash).key == NULL)
        return;

    if (strcmp(GET(actual_hash).key, key) == 0) {
        Entry *entry = &GET(actual_hash);
        Entry *next = entry->next;
        free(entry->key);
        memcpy(entry, next, entry_size);
        free(next);
        return;
    }

    Entry *current = GET(actual_hash).next;
    Entry *prev = &GET(actual_hash);
    while (current != NULL) {
        Entry *next = current->next;
        if (strcmp(current->key, key) == 0) {
            free(current->key);
            free(current);
            prev->next = next;
            return;
        }
        prev = current;
        current = next;
    }

#undef GET
}
#else
    ;
#endif

void set_hashmap(void *self, size_t entry_size, Entry *entry)
#ifdef MY_HASH_IMPL
{
#define GET(i) (*((Entry *)(self + i * entry_size)))
    HashMapHeader *header = hm_header(self);
    size_t hash = header->hashFN(entry->key, header->cap);
    size_t actual_hash = hash % header->cap;

    if (GET(actual_hash).key == NULL) {
        memcpy(&GET(actual_hash), entry, entry_size);
        return;
    }
    if (strcmp(GET(actual_hash).key, entry->key) == 0) {
        free(GET(actual_hash).key);
        memcpy(&GET(actual_hash), entry, entry_size);
        return;
    }

    Entry *current = &GET(actual_hash);
    while (current->next != NULL) {
        Entry *next = current->next;
        if (strcmp(current->key, entry->key) == 0) {
            free(current->key);
            memcpy(current, entry, entry_size);
            return;
        }
        current = next;
    }

    current->next = malloc(entry_size);
    memcpy(current->next, entry, entry_size);
#undef GET
}
#else
    ;
#endif

void print_hashmap_pro(void *self, size_t entry_size)
#ifdef MY_HASH_IMPL
{
#define GET(i) (*((Entry *)(self + i * entry_size)))
    HashMapHeader *header = hm_header(self);

    for (size_t i = 0; i < header->cap; i++) {
        Entry entry = GET(i);
        if (entry.key == NULL) {
            printf("[%04zu] (NULL)", i);
        } else {
            printf("[%04zu] \"%s\"", i, entry.key);
        }

        Entry *current = entry.next;
        while (current != NULL) {
            Entry *next = current->next;
            printf(" -> \"%s\"", current->key);
            current = next;
        }

        puts("");
    }
#undef GET
}
#else
    ;
#endif

size_t default_string_hash(const void *key, size_t cap)
#ifdef MY_HASH_IMPL
{
    const char *key_s = (const char *)key;
    size_t result = 0;
    for (; *key_s != '\0'; key_s++) {
        result += *key_s;
    }
    return result % cap;
}
#else
    ;
#endif

void free_hashmap_entry(Entry *self)
#ifdef MY_HASH_IMPL
{
    Entry *current = self;
    while (current != NULL) {
        void *next = current->next;
        free(current->key);
        free(current);
        current = next;
    }
}
#else
    ;
#endif

void free_hashmap(void **self, size_t entry_size)
#ifdef MY_HASH_IMPL
{
    HashMapHeader *header = hm_header(*self);

    for (size_t i = 0; i < header->cap; i++) {
        Entry entry = *(Entry *)((*self) + i * entry_size);
        free(entry.key);
        if (entry.key != NULL)
            free_hashmap_entry(entry.next);
    }

    free(header);
    *self = NULL;
}
#else
    ;
#endif

void *init_hashmap(size_t item_size, HashFN *hash_fn)
#ifdef MY_HASH_IMPL
{
    size_t allocation_size = sizeof(HashMapHeader) + (item_size * HASHMAP_CAP);
    HashMapHeader *res = malloc(allocation_size);
    if (res == NULL) {
        return NULL;
    }
    memset(res, 0, allocation_size);
    res->cap = HASHMAP_CAP;
    res->hashFN = hash_fn;
    return res + 1;
}
#else
    ;
#endif

#endif // MY_HASH_H_
