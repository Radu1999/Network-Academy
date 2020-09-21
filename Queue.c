#include <stdlib.h>
#include <stdio.h>
#include "Queue.h"

void init_q(struct Queue *q) {
    q->list = malloc(sizeof(struct LinkedList));
    if (q->list == NULL) {
        printf("Not enough memory to initialize the queue!");
        return;
    }
    init_list(q->list);
}

int get_size_q(struct Queue *q) {
    /* TODO */
    return q->list->size;
}

int is_empty_q(struct Queue *q) {
    /* TODO */
    if(q->list->size) {
        return 0;
    }
    return 1;
}

void* front(struct Queue *q) {
    /* TODO */
    if(q->list->head == NULL) {
        return NULL;
    }
    return q->list->head->data;
}

void* dequeue(struct Queue *q) {
    /* TODO */
    struct Node *it =  remove_nth_node(q->list, 0);
    free(it);
    
}

void enqueue(struct Queue *q, void *new_data) {
    /* TODO */
    if(q == NULL) {
        return;
    }
    add_nth_node(q->list, q->list->size, new_data);

}

void clear_q(struct Queue *q) {
    /* TODO */
    struct Node *it;
    while(!is_empty_q(q)) {
        dequeue(q);
    }
}

void purge_q(struct Queue *q) {
    /* TODO */
    clear_q(q);
    free_list(&q->list);
}