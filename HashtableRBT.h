/*
 * Hashtable.h
 * Alexandru-Cosmin Mihai
 */
#ifndef __HASHTABLERBT_H
#define __HASHTABLERBT_H

#include "rb_tree.h"
#define HMAX 50000

struct info {
    void *key;
    void *value;
};

struct Hashtable {
    struct rb_tree_t **buckets;
    int size;
    int hmax;
    unsigned int (*hash_function)(void*);
    int (*compare_function)(void*, void*);
};

void init_ht(struct Hashtable *ht, int hmax, unsigned int (*hash_function)(void*), int (*compare_function)(void*, void*));

void put(struct Hashtable *ht, void *key, size_t key_size_bytes, void *value);

void put_value(struct Hashtable *ht, void *key, size_t key_size_bytes, int *value); 

void* get(struct Hashtable *ht, void *key);

int has_key(struct Hashtable *ht, void *key);

void remove_ht_entry(struct Hashtable *ht, void *key);

int get_ht_size(struct Hashtable *ht);

int get_ht_hmax(struct Hashtable *ht);

void free_ht(struct Hashtable *ht);

struct Hashtable* resize(struct Hashtable **ht);

/*
 * Functii de comparare a cheilor:
 */
int compare_function_ints(void *a, void *b);

int compare_function_int64(void *a, void *b);

int compare_function_strings(void *a, void *b);

/*
 * Functii de hashing:
 */
unsigned int hash_function_int(void *a);

unsigned int hash_function_string(void *a);

void remove_entry(struct Hashtable *ht, void *key);

#endif
