/*
 * Hashtable.c
 * Alexandru-Cosmin Mihai
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Hashtable.h"




void init_htS(struct HashtableS *ht, int hmax, unsigned int (*hash_function)(void*), int (*compare_function)(void*, void*)) {
    /* TODO */
    ht->buckets = malloc(hmax * sizeof(struct LinkedList));
    for(int i = 0; i < hmax; i++) {
        init_list(&ht->buckets[i]);
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
void putS(struct HashtableS *ht, void *key, size_t key_size_bytes, void *value) {
    /* TODO */
    int index = ht->hash_function(key) % ht->hmax;
    for(struct Node *it = ht->buckets[index].head; it != NULL; it = it->next) {
        if(!ht->compare_function(((struct info*)it->data)->key, key)) {
            ((struct info*)it->data)->value = value;
            return;
        }
    }
    struct info* new_entry = malloc(sizeof(struct info));
    new_entry->key = malloc(key_size_bytes);
    memcpy(new_entry->key, key, key_size_bytes);
    new_entry->value = value;
    add_nth_node(&ht->buckets[index], ht->buckets[index].size, new_entry);
    ht->size++;


}

void* getS(struct HashtableS *ht, void *key) {
    /* TODO */
    int index = ht->hash_function(key) % ht->hmax;
    for(struct Node *it = ht->buckets[index].head; it != NULL; it = it->next) {
        if(!ht->compare_function(((struct info*)it->data)->key, key)) {
            return ((struct info*)it->data)->value;
        }
    }
    return NULL;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable folosind functia put
 * 0, altfel.
 */
int has_keyS(struct HashtableS *ht, void *key) {
    /* TODO */
    int index = ht->hash_function(key) % ht->hmax;
    for(struct Node *it = ht->buckets[index].head; it != NULL; it = it->next) {
        if(!ht->compare_function(((struct info*)it->data)->key, key)) {
            return 1;
        }
    }
    return 0;
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o intrare din hashtable (adica memoria
 * pentru copia lui key --vezi observatia de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void remove_ht_entryS(struct HashtableS *ht, void *key) {
    /* TODO */
    int index = ht->hash_function(key) % ht->hmax;
    struct Node *rm;
    int position = 0;
    for(struct Node *it = ht->buckets[index].head; it != NULL; it = it->next, position++) {
        if(!ht->compare_function(((struct info*)it->data)->key, key)) {
            rm = remove_nth_node(&ht->buckets[index], position);
            free(((struct info*)rm->data)->key);
            free(((struct info*)rm->data)->value);
            free(rm->data);
            free(rm);
            ht->size--;
            break;
        } 
    }

}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable, dupa care elibereaza si memoria folosita
 * pentru a stoca structura hashtable.
 */
void free_htS(struct HashtableS *ht) {
    /* TODO */
    struct Node *it;
    for(int i = 0; i < ht->hmax; i++) {
        while(get_size(&ht->buckets[i])) {
            it = remove_nth_node(&ht->buckets[i],0);
            free(((struct info*)it->data)->key);
            free(((struct info*)it->data)->value);
            free(it->data);
            free(it);
        }
    }
    free(ht->buckets);
    free(ht);
}

int get_ht_simple_sizeS(struct Hashtable *ht) {
    if (ht == NULL) {
        return -1;
    }

    return ht->size;
}

int get_ht_simple_hmaxS(struct Hashtable *ht) {
    if (ht == NULL) {
        return -1;
    }

    return ht->hmax;
}

