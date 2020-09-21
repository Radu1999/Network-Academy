#include "publications.h"
#include "Hashtable.h"
#include "Queue.h"
#include "heap.h"
#define MAX_YEAR 2021

struct publications_data {
    /* TODO: add whichever data you need here */
    struct Hashtable *art_id;
    struct Hashtable *referenced_by;
    struct Hashtable *venue;
    struct Hashtable *field;
    struct Hashtable *author;
    struct Hashtable *author_link;
    struct HashtableS *visited;
    struct Hashtable *in_heap;
    struct HashtableS *distance;
    struct Hashtable *author_name;
    int* year_fq;
};


struct article {
    int64_t id;
    char *title;
    char *venue;
    int year;
    char **author_names;
    char **institutions;
    char **fields;
    int64_t* author_ids;
    int num_fields;
    int num_authors;
    int64_t* references;
    int num_refs;

};


PublData* init_publ_data(void) {
    /* TODO: implement init_publ_data */
    PublData *data = malloc(sizeof(PublData));
    data->art_id  = malloc(sizeof(struct Hashtable));
    data->referenced_by = malloc(sizeof(struct Hashtable));
    data->venue = malloc(sizeof(struct Hashtable));
    data->year_fq = calloc(MAX_YEAR, sizeof(int));
    data->field = malloc(sizeof(struct Hashtable));
    data->author = malloc(sizeof(struct Hashtable));
    data->author_link = malloc(sizeof(struct Hashtable));
    data->visited = malloc(sizeof(struct HashtableS));
    data->in_heap = malloc(sizeof(struct Hashtable));
    data->distance = malloc(sizeof(struct HashtableS));
    data->author_name = malloc(sizeof(struct Hashtable));
    init_ht(data->art_id, HMAX, hash_function_int64, compare_function_int64);
    init_ht(data->referenced_by, HMAX, hash_function_int64, compare_function_int64);
    init_ht(data->venue, HMAX, hash_function_string, compare_function_strings);
    init_ht(data->field, HMAX, hash_function_string, compare_function_strings);
    init_ht(data->author, HMAX, hash_function_int64, compare_function_int64);
    init_ht(data->author_link, HMAX, hash_function_int64, compare_function_int64);
    init_htS(data->visited, HMAX, hash_function_int64, compare_function_int64);
    init_ht(data->in_heap, HMAX, hash_function_int64, compare_function_int64);
    init_htS(data->distance, HMAX, hash_function_int64, compare_function_int64);
    init_ht(data->author_name, HMAX, hash_function_int64, compare_function_strings);
    return data;
}

static void free_paper_rbt(rb_node_t *root) {
    if(root == NULL) {
        return;
    }
    free_paper_rbt(root->left);
    free_paper_rbt(root->right);
    free(((struct info*)root->data)->key);
    Article *paper = ((struct info*)root->data)->value;
    int i;
    for(i = 0; i < paper->num_authors; i++) {
        free(paper->institutions[i]);
        free(paper->author_names[i]);
    }
    free(paper->institutions);
    free(paper->author_names);
    free(paper->author_ids);
    free(paper->references);
    for(i = 0; i < paper->num_fields; i++) {
        free(paper->fields[i]);
    }
    free(paper->fields);
    free(paper->venue);
    free(paper->title);
    free(paper);
    free(root->data);
    free(root);
}

static void free_referenced_rbt(rb_node_t *root) {
    if(root == NULL) {
        return;
    }
    free_referenced_rbt(root->left);
    free_referenced_rbt(root->right);
    free(((struct info*)root->data)->key);
    struct LinkedList *list = ((struct info*)root->data)->value;
    free_list(&list);
    free(root->data);
    free(root);

}

static void free_author_name(rb_node_t *root) {
    if (root == NULL)
		return;
	free_author_name(root->left);
	free_author_name(root->right);
	free(((struct info*)root->data)->key);
	free(root->data);
	free(root);
}


void destroy_publ_data(PublData* data) {
    /* TODO: implement destroy_publ_data */
    struct Hashtable *ht = data->art_id;
    int i;
    for(i = 0; i < ht->hmax; i++) {
       free_paper_rbt(ht->buckets[i]->root);
       free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
    ht = data->referenced_by;
    for(i = 0; i < ht->hmax; i++) {
       free_referenced_rbt(ht->buckets[i]->root);
       free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
    ht = data->venue;
    for(i = 0; i < ht->hmax; i++) {
       free_referenced_rbt(ht->buckets[i]->root);
       free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
    ht = data->field;
    for(i = 0; i < ht->hmax; i++) {
       free_referenced_rbt(ht->buckets[i]->root);
       free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
    ht = data->author;
    for(i = 0; i < ht->hmax; i++) {
       free_referenced_rbt(ht->buckets[i]->root);
       free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
    ht = data->author_link;
    for(i = 0; i < ht->hmax; i++) {
       free_referenced_rbt(ht->buckets[i]->root);
       free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
    ht = data->author_name;
    for(i = 0; i < ht->hmax; i++) {
       free_author_name(ht->buckets[i]->root);
       free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
    free_htS(data->visited);
    free_htS(data->distance);
    free_ht(data->in_heap);
    free(data->year_fq);
    free(data);
}


void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, const int64_t id, const int64_t* references,
    const int num_refs) {
    Article *paper = malloc(sizeof(Article)), *ref_paper, *curr_paper, *oldest, *best_paper;
    paper->id = id;
    paper->title = malloc(sizeof(char) * (strlen(title) + 1));
    memcpy(paper->title, title, sizeof(char) * (strlen(title) + 1));
    paper->venue = malloc(sizeof(char) * (strlen(venue) + 1));
    memcpy(paper->venue, venue, sizeof(char) * (strlen(venue) + 1));
    struct LinkedList *members = get(data->venue, paper->venue);
    if(members == NULL) {
        members = malloc(sizeof(struct LinkedList));
        init_list(members);
        add_nth_node(members, members->size, &paper->id);
        put(data->venue, paper->venue, strlen(venue) + 1, members);
    } else {
        add_nth_node(members, members->size, &paper->id);
    }
    paper->year = year;
    data->year_fq[year]++;
    paper->num_fields = num_fields;
    paper->fields = malloc(sizeof(char*) * num_fields);
    int i;
    for(i = 0; i < num_fields; i++) {
        paper->fields[i] = malloc(sizeof(char) * (strlen(fields[i]) + 1 ));
        memcpy(paper->fields[i], fields[i], sizeof(char) * (strlen(fields[i]) + 1));
        members = get(data->field, paper->fields[i]);
        if(members == NULL) {
            members = malloc(sizeof(struct LinkedList));
            init_list(members);
            add_nth_node(members, members->size, &paper->id);
            put(data->field, paper->fields[i], strlen(fields[i]) + 1, members);
        } else {
            add_nth_node(members, members->size, &paper->id);
        }
    }
    paper->num_refs = num_refs;
    paper->num_authors = num_authors;
    paper->references = malloc(sizeof(int64_t) * num_refs);
    struct LinkedList *referenced;
    for(i = 0; i < num_refs; i++) {
        paper->references[i] = references[i];
        referenced = get(data->referenced_by, &paper->references[i]);
        if(referenced == NULL) {
            referenced = malloc(sizeof(struct LinkedList));
            init_list(referenced);
            add_nth_node(referenced, referenced->size, &paper->id);
            put(data->referenced_by, &paper->references[i], sizeof(int64_t), referenced);
        } else {
            add_nth_node(referenced, referenced->size, &paper->id);
        }
    }
    paper->author_ids = malloc(sizeof(int64_t) * num_authors);
    paper->author_names = malloc(sizeof(char *) * num_authors);
    paper->institutions = malloc(sizeof(char *) * num_authors);
    char *name;
    for(i = 0; i < num_authors; i++) {
        paper->author_ids[i] = author_ids[i];
        members = get(data->author, &paper->author_ids[i]);
        if(members == NULL) {
            members = malloc(sizeof(struct LinkedList));
            init_list(members);
            add_nth_node(members, members->size, &paper->id);
            put(data->author, &paper->author_ids[i], sizeof(int64_t), members);
        } else {
            add_nth_node(members, members->size, &paper->id);
        }
        paper->author_names[i] = malloc(sizeof(char) * (strlen(author_names[i]) + 1));
        name = get(data->author_name, &paper->author_ids[i]);
        if(name == NULL) {
            put(data->author_name, &paper->author_ids[i], sizeof(int64_t), paper->author_names[i]);
        }
        paper->institutions[i] = malloc(sizeof(char) * (strlen(institutions[i]) + 1));
        memcpy(paper->author_names[i], author_names[i], sizeof(char) * (strlen(author_names[i]) + 1));
        memcpy(paper->institutions[i], institutions[i], sizeof(char) * (strlen(institutions[i]) + 1));
    }
    int j;
    for(i = 0; i < num_authors; i++) {
        members = get(data->author_link, &paper->author_ids[i]);
        for(j = 0; j < num_authors; j++) {
            if(i != j) {
                if(members == NULL) {
                    members = malloc(sizeof(struct LinkedList));
                    init_list(members);
                    add_nth_node(members, members->size, &paper->author_ids[j]);
                    put(data->author_link, &paper->author_ids[i], sizeof(int64_t), members);
                } else {
                    add_nth_node(members, members->size, &paper->author_ids[j]);
                }
            }
        }
    }
    put(data->art_id, &paper->id, sizeof(int64_t), paper);
}

char* get_oldest_influence(PublData* data, const int64_t id_paper) {
    /* TODO: implement get_oldest_influence */
     struct Queue q;
    int64_t current_id = id_paper;
    Article *curr_paper = get(data->art_id, &current_id);
    init_q(&q);
    static int viz = 0;
    viz++;
    int *used;
    int64_t best_id;
    int best_year = MAX_YEAR, quoted_num, best_quoted, i;
    struct LinkedList *referenced;
    used = getS(data->visited, &current_id);
    if(used == NULL) {
        used = malloc(sizeof(int));
        putS(data->visited, &current_id, sizeof(int64_t), used);

    }
    *used = viz;
    int *is, *find;
    for(i = 0; i < curr_paper->num_refs; i++) {
        is = get(data->art_id, &curr_paper->references[i]);
        if(is == NULL) {
            continue;
        }
        if(curr_paper->references[i] != current_id) {
            enqueue(&q, &curr_paper->references[i]);
            used = getS(data->visited, &curr_paper->references[i]);
            if(used == NULL) {
                used = malloc(sizeof(int));
                putS(data->visited, &curr_paper->references[i], sizeof(int64_t), used);

            }
            *used = viz;
        }
    }
    while(!is_empty_q(&q)) {
        current_id = *(int64_t *)front(&q);
        dequeue(&q);
        curr_paper = get(data->art_id, &current_id);
        referenced = get(data->referenced_by, &curr_paper->id);
        quoted_num = referenced != NULL ? referenced->size : 0;
        if(curr_paper->year < best_year) {
            best_year = curr_paper->year;
            best_id = curr_paper->id;
            best_quoted = quoted_num;
        } else if(curr_paper->year == best_year) {
            if(quoted_num > best_quoted) {
                best_id = curr_paper->id;
                best_quoted = quoted_num;
            } else if(quoted_num == best_quoted && curr_paper->id < best_id) {
                best_id = curr_paper->id;
            }
        }
        for(i = 0; i < curr_paper->num_refs; i++) {
            is = get(data->art_id, &curr_paper->references[i]);
            if(is == NULL) {
                continue;
            }
            find = getS(data->visited, &curr_paper->references[i]);
            if(find == NULL || *find != viz) {
                enqueue(&q, &curr_paper->references[i]);
                used = getS(data->visited, &curr_paper->references[i]);
                if(used == NULL) {
                    used = malloc(sizeof(int));
                    putS(data->visited, &curr_paper->references[i], sizeof(int64_t), used);

                }
                *used = viz;
            }
        }
    }
    purge_q(&q);
    if(best_year == MAX_YEAR) {
        return "None";
    }
    curr_paper = get(data->art_id, &best_id);
    return curr_paper->title;
}

float get_venue_impact_factor(PublData* data, const char* venue) {
    /* TODO: implement get_venue_impact_factor */
   struct LinkedList *members = get(data->venue, (char *)venue), *quoted_by;
   struct Node *it;
   float impact_factor = 0.0;
   if(members == NULL) {
       return 0.0;
   }

   for(it = members->head; it != NULL; it = it->next) {
       quoted_by = get(data->referenced_by, it->data);
       if(quoted_by != NULL) {
           impact_factor += quoted_by->size;
       }
   }
    return impact_factor / ((1.0) * members->size);
}

struct Dst{
    int64_t id;
    int dst;
};

static int compare_fct(void *a, void *b) {
    int64_t va = *(int64_t *)a;
    int64_t vb = *(int64_t *)b;
    if(va < vb) {
        return -1;
    } else if(va > vb) {
        return 1;
    }
    return 0;
}

void free_visited(rb_node_t *root) {
    if(root == NULL) {
        return;
    }
    free_visited(root->left);
    free_visited(root->right);
    struct info *pair = root->data;
    free(pair->key);
    free(pair->value);
    free(pair);
    free(root);

}

int get_number_of_influenced_papers(PublData* data, const int64_t id_paper,
    const int distance) {
    /* TODO: implement get_number_of_influenced_papers */
    int64_t current_id = id_paper;
    struct LinkedList* quoted_by = get(data->referenced_by, &current_id);
    struct Node *it;
    if(quoted_by == NULL || distance == 0) {
        return 0;
    }
    int answer = 0;
    struct Queue q;
    init_q(&q);
    enqueue(&q, &current_id);
    rb_tree_t *visited = rb_tree_create(compare_fct);
    struct info *make_pair = malloc(sizeof(struct info)) , *pair;
    make_pair->key = malloc(sizeof(int64_t));
    memcpy(make_pair->key, &current_id, sizeof(int64_t));
    make_pair->value = calloc(1, sizeof(int));
    rb_tree_insert(visited, make_pair);
    while(!is_empty_q(&q)) {
        current_id = *(int64_t*)front(&q);
        dequeue(&q);
        quoted_by = get(data->referenced_by, &current_id);
        pair = rb_tree_find(visited, &current_id);
        if(*(int*)pair->value < distance && quoted_by != NULL) {
            for(it = quoted_by->head; it != NULL; it = it->next) {
                if(rb_tree_find(visited, it->data) == NULL) {
                    enqueue(&q, it->data);
                    answer++;
                    make_pair = malloc(sizeof(struct info));
                    make_pair->key = malloc(sizeof(int64_t));
                    memcpy(make_pair->key, it->data, sizeof(int64_t));
                    make_pair->value = calloc(1, sizeof(int));
                    *(int*)make_pair->value = *(int*)pair->value + 1;
                    rb_tree_insert(visited, make_pair);
                }
            }
        }
    }
    free_visited(visited->root);
    free(visited);
    purge_q(&q);
    return answer;
}

int get_erdos_distance(PublData* data, const int64_t id1, const int64_t id2) {
    /* TODO: implement get_erdos_distance */
    struct Queue q;
    init_q(&q);
    int64_t current_id = id1;
    static int viz = 0;
    viz++;
    int *dst = getS(data->visited, &current_id);
    int *new_dst;
    if(dst == NULL) {
        dst = calloc(1, sizeof(int));
        putS(data->visited, &current_id, sizeof(int64_t), dst);
    }
    if(*dst != viz) {
       *dst = viz; 
    }
    dst = getS(data->distance, &current_id);
    if(dst == NULL) {
        dst = calloc(1, sizeof(int));
        putS(data->distance, &current_id, sizeof(int64_t), dst);
    } else {
        *dst = 0;
    }
    int *used;
    struct LinkedList *worked_with;
    struct Node *it;
    int answer = -1, ok = 0;
    enqueue(&q, &current_id);
    while(!is_empty_q(&q) && !ok) {
        current_id = *(int64_t*)front(&q);
        dequeue(&q);
        worked_with = get(data->author_link, &current_id);
        if(worked_with == NULL) {
            continue;
        }
        dst = getS(data->distance, &current_id);
        for(it = worked_with->head; it != NULL; it = it->next) {
            used = getS(data->visited, it->data);
            if(used == NULL || *used != viz) {
                if(*(int64_t *)it->data == id2) {
                    answer = *dst + 1;
                    ok = 1;
                    break;
                }
                if(used == NULL) {
                    used = malloc(sizeof(int));
                    putS(data->visited, it->data, sizeof(int64_t), used);
                }
                *used = viz;
                new_dst = getS(data->distance, it->data);
                if(new_dst == NULL) {
                    new_dst = malloc(sizeof(int));
                    putS(data->distance, it->data, sizeof(int64_t), new_dst);
                }
                *new_dst = *dst + 1;
                enqueue(&q, it->data);

            }
        }
    }
    purge_q(&q);
    return answer;
}

static int compare_heap(void *a, void *b) {
    art_t *va = (art_t *)a;
    art_t *vb = (art_t *)b;
    if(va->quotations > vb->quotations) {
        return 1;
    } else if(va->quotations == vb->quotations){
        if(va->year > vb->year) {
          return 1;
        } else if(va->year == vb->year && va->id < vb->id) {
            return 1;
        }
    }
    return -1;
}

char** get_most_cited_papers_by_field(PublData* data, const char* field,
    int* num_papers) {
    /* TODO: implement get_most_cited_papers_by_field */
    heap_t *heap = heap_create(compare_heap);
    struct LinkedList *members = get(data->field, (char *)field), *quoted_by;
    struct Node *it;
    Article *paper;
    if(members == NULL) {
        *num_papers = 0;
        return NULL;
    }
    art_t* find  = malloc(sizeof(art_t));
    for(it = members->head; it != NULL; it = it->next) {
        paper = get(data->art_id, it->data);
        find->id = paper->id;
        find->year = paper->year;
        quoted_by = get(data->referenced_by, it->data);
        find->title = paper->title;
        find->quotations = quoted_by != NULL ? quoted_by->size : 0;
        heap_insert(heap, find);
    }
    free(find);
    if(heap->size < *num_papers) {
        *num_papers = heap->size;
    }
    char** answer = malloc((*num_papers) * sizeof(char *));
    int i;
    for(i = 0; i < *num_papers; i++) {
        if(heap->size == 0) {
            break;
        }
        find = heap_top(heap);
        while(i > 0 && !strcmp(find->title, answer[i - 1])) {
            heap_pop(heap);
            find = heap_top(heap);
        }
        answer[i] = malloc(strlen(find->title) + 1);
        memcpy(answer[i], find->title, strlen(find->title) + 1);
        heap_pop(heap);
    }
    if(i < *num_papers) {
        *num_papers = i;
    }
    heap_free(heap);


    return answer;
}

int get_number_of_papers_between_dates(PublData* data, const int early_date,
    const int late_date) {
    /* TODO: implement get_number_of_papers_between_dates */
    int nr = 0;
    int i;
    for(i = early_date; i <= late_date; i++){
        nr += data->year_fq[i];
    }

    return nr;
}

int get_number_of_authors_with_field(PublData* data, const char* institution,
    const char* field) {
    /* TODO: implement get_number_of_authors_with_field */
    struct LinkedList *ids = get(data->field, (char *)field);
    Article *paper;
    struct LinkedList *authors = malloc(sizeof(struct LinkedList));
    init_list(authors);
    struct Node *it, *it2;
    int i, ok, ans = 0;
    if(ids != NULL) {
        for(it = ids->head; it != NULL; it = it->next) {
            paper = get(data->art_id, it->data);
            for(i = 0; i < paper->num_authors; i++) {
                if(!strcmp(paper->institutions[i], institution)) {
                    ok = 0;
                    for(it2 = authors->head; it2 != NULL; it2 = it2->next) {
                        if(*(int64_t*)it2->data == paper->author_ids[i]) {
                            ok = 1;
                            break;
                        }
                    }
                    if(!ok) {
                        add_nth_node(authors, authors->size, &paper->author_ids[i]);
                        ans++;
                    }
                }
            }
        }
    }
    free_list(&authors);
    return ans;
}

int* get_histogram_of_citations(PublData* data, const int64_t id_author,
    int* num_years) {
    /* TODO: implement get_histogram_of_citations */
    int *histogram = calloc(3000, sizeof(int));
    int num_cit;
    *num_years = 0;
    Article* paper;
    int64_t id = id_author;
    struct LinkedList *members = get(data->author, &id);
    struct LinkedList *quoted_by;
    struct Node *it;
    if(members == NULL) {
        return NULL;
    }
    for(it = members->head; it != NULL; it = it->next) {
        quoted_by = get(data->referenced_by, it->data);
        num_cit = quoted_by != NULL ? quoted_by->size : 0;
        paper = get(data->art_id, it->data);
        if(2020 - paper->year >= *num_years) {
            *num_years = 2020 - paper->year + 1;
        }
        histogram[2020 - paper->year] += num_cit;

    }
    return histogram;
}


static int compare_reading_order(void *a, void *b) {
    art_t *va = (art_t *)a;
    art_t *vb = (art_t *)b;
    if(va->year < vb->year) {
        return 1;
    } else if(va->year == vb->year && va->id < vb->id) {
        return 1;
    }
    return -1;
}


char** get_reading_order(PublData* data, const int64_t id_paper,
    const int distance, int* num_papers) {
    /* TODO: implement get_reading_order */
    struct Queue q;
    init_q(&q);
    int ok = 0;
    heap_t *heap = heap_create(compare_reading_order);
    int64_t current_id = id_paper;
    int *used = getS(data->visited, &current_id);
    int *dst = getS(data->distance, &current_id);
    if(dst == NULL) {
        dst = calloc(1, sizeof(int));
        putS(data->distance, &current_id, sizeof(int64_t), dst);
    } else {
        *dst = 0;
    }
    if(used == NULL) {
        used = malloc(sizeof(int));
        putS(data->visited, &current_id, sizeof(int64_t), used);
    } 
    *used = 0;
    enqueue(&q, &current_id);
    int i = 0;
    Article *paper, *new_paper;
    art_t *insert = malloc(sizeof(art_t));
    *num_papers = 0;
    while(!is_empty_q(&q)) {
        ok = 0;
        current_id = *(int64_t *)front(&q);
        dequeue(&q);
        paper = get(data->art_id, &current_id);
        used = getS(data->visited, &current_id);
        int degree = 0;
        dst = getS(data->distance, &current_id);
        if(*dst  < distance) {
            for(i = 0; i < paper->num_refs; i++) {
                if(get(data->art_id, &paper->references[i]) == NULL) {
                    continue;
                }
                int *r = getS(data->visited, &paper->references[i]);
                if(r != NULL && *r != -1) {
                    degree++;
                    continue;
                }
                degree++;
                if(r == NULL) {
                    r = malloc(sizeof(int));
                    putS(data->visited, &paper->references[i], sizeof(int64_t), r);
                }
                *r = 0;
                int *d = getS(data->distance, &paper->references[i]);
                if(d == NULL) {
                    d = malloc(sizeof(int));
                    putS(data->distance, &paper->references[i], sizeof(int64_t), d);
                }
                *d  = *dst + 1;
                enqueue(&q, &paper->references[i]);
                *num_papers += 1;
            }
        } else {
            for(i = 0; i < paper->num_refs; i++) {
                if(get(data->art_id, &paper->references[i]) == NULL) {
                    continue;
                }
                    int *r = getS(data->visited, &paper->references[i]);
                if(r != NULL && *r != -1) {
                    degree++;
                }
            }
        }
        *used = degree;
        if(degree == 0 && paper->id != id_paper) {
            insert->title = paper->title;
            insert->year = paper->year;
            insert->id = paper->id;
            used = get(data->in_heap, &insert->id);
            if(used == NULL) {
                used = malloc(sizeof(int*));
                *used = 1;
                put(data->in_heap, &insert->id, sizeof(int64_t), used);
            } else {
                *used = 1;
            }
            heap_insert(heap, insert);
        }
    }
    current_id = id_paper;
    used = getS(data->visited, &current_id);
    *used = -1;
    char **titles;
    int *is_in;
    int cnt = 0;
    struct LinkedList *back;
    art_t *best;
    struct Node *it;
    titles = malloc(*num_papers * sizeof(char *));
    int *n_degree;
    while(heap->size > 0) {
        best = heap_top(heap);
        titles[cnt] = malloc(strlen(best->title) + 1);
        memcpy(titles[cnt++], best->title, strlen(best->title) + 1);
       // printf("%s\n", titles[cnt - 1]);
        used = getS(data->visited, &best->id);
        *used = -1;
        used = get(data->in_heap, &best->id);
        *used = -1;
        back = get(data->referenced_by, &best->id);
        heap_pop(heap);
        if(back != NULL) {
            for(it = back->head; it != NULL; it = it->next) {
                n_degree = getS(data->visited, it->data);
                if(n_degree == NULL || *n_degree == -1 || *(int64_t*)it->data == id_paper) {
                    continue;
                }
                *n_degree -= 1;
                if(*n_degree == 0) {
                    *n_degree =  -1;
                    paper = get(data->art_id, it->data);
                    is_in = get(data->in_heap, it->data);
                    if(is_in == NULL) {
                        is_in = malloc(sizeof(int));
                        put(data->in_heap, it->data, sizeof(int64_t), is_in);
                    }
                    *is_in = 1;
                    insert->title = paper->title;
                    insert->year = paper->year;
                    insert->id = paper->id;
                    heap_insert(heap, insert);
                }
            }
        }
         
    }
    purge_q(&q);
    heap_free(heap);
    free(insert);
    return titles;

     
    
  
}

static float get_e_pow( int b) {
    float e = 2.71;
    for(int i = 1; i < b; i++) {
        e *= e;
    }
    return 1/e;
}

char* find_best_coordinator(PublData* data, const int64_t id_author) {
    /* TODO: implement find_best_coordinator */
    struct Queue q;
    init_q(&q);
    int64_t current_id = id_author;
    static int viz = 0;
    viz++;
    int *dst = getS(data->visited, &current_id);
    int *new_dst;
    if(dst == NULL) {
        dst = calloc(1, sizeof(int));
        putS(data->visited, &current_id, sizeof(int64_t), dst);
    }
    if(*dst != viz) {
       *dst = viz; 
    }
    dst = getS(data->distance, &current_id);
    if(dst == NULL) {
        dst = calloc(1, sizeof(int));
        putS(data->distance, &current_id, sizeof(int64_t), dst);
    } else {
        *dst = 0;
    }
    int *used;
    struct LinkedList *worked_with;
    struct Node *it;
    enqueue(&q, &current_id);
    float max_score = -1;
    int64_t min_id = 0;
    float coef;
    float sum = 0;
    struct LinkedList *ids;
    struct Node *it2;
    Article *paper;
    float impact;
    struct LinkedList *quoted_by;
    int quote_num;
    float score;
    while(!is_empty_q(&q)) {
        current_id = *(int64_t*)front(&q);
        dequeue(&q);
        worked_with = get(data->author_link, &current_id);
        if(worked_with == NULL) {
            continue;
        }
        dst = getS(data->distance, &current_id);
        for(it = worked_with->head; it != NULL; it = it->next) {
            used = getS(data->visited, it->data);
            if(used == NULL || *used != viz) {
                if(used == NULL) {
                    used = malloc(sizeof(int));
                    putS(data->visited, it->data, sizeof(int64_t), used);
                }
                *used = viz;
                new_dst = getS(data->distance, it->data);
                if(new_dst == NULL) {
                    new_dst = malloc(sizeof(int));
                    putS(data->distance, it->data, sizeof(int64_t), new_dst);
                }
                *new_dst = *dst + 1;
                coef = get_e_pow(*new_dst);
                sum = 0.0;
                ids = get(data->author, it->data);
                for(it2 = ids->head; it2 != NULL; it2 = it2->next) {
                    paper = get(data->art_id, it2->data);
                    if(paper == NULL) {
                        continue;
                    }
                    impact = get_venue_impact_factor(data, paper->venue);
                    quoted_by = get(data->referenced_by, it2->data);
                    quote_num = quoted_by != NULL ? quoted_by->size : 0;
                    sum += impact * quote_num;
                }
                score = sum * coef;
                if(score > max_score) {
                    max_score = score;
                    min_id = *(int64_t*)it->data;
                } else if(score == max_score && *(int64_t*)it->data < min_id) {
                    min_id = *(int64_t*)it->data;
                }
                if(*new_dst < 5);
                enqueue(&q, it->data);

            }
        }
    }
    purge_q(&q);
    char *title = get(data->author_name, &min_id);
    if(title == NULL) {
        return "None";
    }
    return title;
}

