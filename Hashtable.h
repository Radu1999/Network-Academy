/*
 * Hashtable.h
 * Alexandru-Cosmin Mihai
 */
#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include "HashtableRBT.h"
#include "LinkedList.h"

struct HashtableS {
    struct LinkedList *buckets;
    int size;
    int hmax;
    unsigned int (*hash_function)(void*);
    int (*compare_function)(void*, void*);
};

void init_htS(struct HashtableS *ht, int hmax, unsigned int (*hash_function)(void*), int (*compare_function)(void*, void*));

void putS(struct HashtableS *ht, void *key, size_t key_size_bytes, void *value);

void* getS(struct HashtableS *ht, void *key);

int has_keyS(struct HashtableS *ht, void *key);

void remove_ht_entryS(struct HashtableS *ht, void *key);

int get_ht_sizeS(struct HashtableS *ht);

int get_ht_hmaxS(struct HashtableS *ht);

void free_htS(struct HashtableS *ht);

struct HashtableS* resizeS(struct HashtableS **ht);

unsigned int hash_function_int64(void *a);
#endif

