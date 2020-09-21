/*
 * Hashtable.c
 * Alexandru-Cosmin Mihai
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HashtableRBT.h"

/*
 * Functii de comparare a cheilor:
 */
int compare_function_ints(void *a, void *b) {
    int int_a = *((int *)a);
    int int_b = *((int *)b);

    if (int_a == int_b) {
        return 0;
    } else if (int_a < int_b) {
        return -1;
    } else {
        return 1;
    }
}

int compare_function_int64(void *a, void *b) {
    int64_t int64_a = *((int64_t *)a);
    int64_t int64_b = *((int64_t *)b);

    if (int64_a == int64_b) {
        return 0;
    } else if (int64_a < int64_b) {
        return -1;
    } else {
        return 1;
    }
}


int compare_function_strings(void *a, void *b) {
    char *str_a = (char *)a;
    char *str_b = (char *)b;

    return strcmp(str_a, str_b);
}

/*
 * Functii de hashing:
 */
unsigned int hash_function_int(void *a) {
    /*
     * Credits: https://stackoverflow.com/a/12996028/7883884
     */
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_int64(void *a) {
  
    return *(int64_t *)a % HMAX;
}



unsigned int hash_function_string(void *a) {
    /*
     * Credits: http://www.cse.yorku.ca/~oz/hash.html
     */
    unsigned char *puchar_a = (unsigned char*) a;
    unsigned long hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

    return hash;
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
void init_ht(struct Hashtable *ht, int hmax, unsigned int (*hash_function)(void*), int (*compare_function)(void*, void*)) {
    /* TODO */
    ht->buckets = malloc(hmax * sizeof(rb_tree_t*));
    int i;
    for(i = 0; i < hmax; i++) {
        ht->buckets[i] = rb_tree_create(compare_function);
    }
    ht->compare_function = compare_function;
    ht->hash_function = hash_function;
    ht->hmax = hmax;
    ht->size = 0;
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune tipul ei), in momentul in care
 * se creeaza o noua intrare in hashtable (in cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie
 * a valorii la care pointeaza key si adresa acestei copii trebuie salvata in structura info asociata intrarii din ht.
 * Pentru a sti cati octeti trebuie alocati si copiati, folositi parametrul key_size_bytes.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel put(ht, key_actual, value_actual),
 * valoarea la care pointeaza key_actual poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia unei intrari din hashtable.
 * Nu ne dorim acest lucru, fiindca exista riscul sa ajungem in situatia in care nu mai stim la ce cheie este
 * inregistrata o anumita valoare.
 */
void put(struct Hashtable *ht, void *key, size_t key_size_bytes, void *value) {
    /* TODO */
    int index;
    index = ht->hash_function(key) % ht->hmax;
    struct info* intel;
    intel =  rb_tree_find(ht->buckets[index], key);
    if(intel != NULL) {
        intel->value = value;
        return;
    }
    struct info* new_entry;
    new_entry = malloc(sizeof(struct info));
    new_entry->key = malloc(key_size_bytes);
    memcpy(new_entry->key, key, key_size_bytes);
    new_entry->value = value;
    rb_tree_insert(ht->buckets[index], new_entry);
    ht->size++;

}

void put_value(struct Hashtable *ht, void *key, size_t key_size_bytes, int* value) {
    int index = ht->hash_function(key) % ht->hmax;
    struct info* intel = rb_tree_find(ht->buckets[index], key);
    if(intel != NULL) {
        return;
    }
    struct info* new_entry = malloc(sizeof(struct info));
    new_entry->key = malloc(key_size_bytes);
    memcpy(new_entry->key, key, key_size_bytes);
    new_entry->value = malloc(sizeof(int));
    memcpy(new_entry->value, value, sizeof(int));
    rb_tree_insert(ht->buckets[index], new_entry);
    ht->size++;

}

void remove_entry(struct Hashtable *ht, void *key) {
    /* TODO */
    int index = ht->hash_function(key) % ht->hmax;
    int rc;
	rb_node_t *node = ht->buckets[index]->root;

	while (node != NULL) {
		rc = ht->buckets[index]->cmp(key, ((struct info*)node->data)->key);

		if (rc == 0)
			rb_delete(ht->buckets[index], node);
            return;

		if (rc < 0)
			node = node->left;
		else
			node = node->right;
	}

	return;

}


void* get(struct Hashtable *ht, void *key) {
    /* TODO */
    int index = ht->hash_function(key) % ht->hmax;
    struct info* intel = rb_tree_find(ht->buckets[index], key);
    if(intel != NULL) {
        return intel->value;
    }
    return NULL;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable folosind functia put
 * 0, altfel.
 */
int has_key(struct Hashtable *ht, void *key) {
    /* TODO */
    int index = ht->hash_function(key) % ht->hmax;
    if(rb_tree_find(ht->buckets[index], key) != NULL) {
        
        return 1;
    }
    return 0;
}



void free_ht(struct Hashtable *ht) {
    /* TODO */
    int i;
    for(i = 0; i < ht->hmax; i++) {
       rb_tree_free(ht->buckets[i]);
       free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
}

int get_ht_size(struct Hashtable *ht) {
    if (ht == NULL) {
        return -1;
    }

    return ht->size;
}

int get_ht_hmax(struct Hashtable *ht) {
    if (ht == NULL) {
        return -1;
    }

    return ht->hmax;
}

