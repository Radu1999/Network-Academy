

#include "heap.h"

heap_t *heap_create(int (*cmp_f) (void *, void *))
{

	heap_t *heap;

	heap = malloc(sizeof(heap_t));

	heap->cmp       = cmp_f;
	heap->size      = 0;
	heap->capacity  = 2;
	heap->arr       = malloc(heap->capacity * sizeof(art_t *));

	return heap;
}

static void __heap_insert_fix(heap_t *heap, int pos)
{
	art_t *tmp_team;
	int p = GO_UP(pos);

	/* TODO */
	if(p < 0) {
		return;
	}
	if(heap->cmp(heap->arr[pos], heap->arr[p]) > 0) {
		tmp_team = heap->arr[p];
		heap->arr[p] = heap->arr[pos];
		heap->arr[pos] = tmp_team;
		__heap_insert_fix(heap, p);
	}

}

void heap_insert(heap_t *heap, art_t *art)
{
	char *rc;

	heap->arr[heap->size] = malloc(sizeof(art_t));

	heap->arr[heap->size]->title = calloc(strlen(art->title) + 1, sizeof(char));
	memcpy(heap->arr[heap->size]->title, art->title, strlen(art->title) + 1);
	heap->arr[heap->size]->id = art->id;
	heap->arr[heap->size]->quotations = art->quotations;
	heap->arr[heap->size]->year = art->year;

	__heap_insert_fix(heap, heap->size);

	heap->size++;
	if (heap->size == heap->capacity) {
		heap->capacity *= 2;
		heap->arr = realloc(heap->arr, heap->capacity * sizeof(art_t *));
	}
}

art_t* heap_top(heap_t *heap)
{
	/* TODO */
	return heap->arr[0];
}

static void __heap_pop_fix(heap_t *heap, int pos)
{
	art_t *tmp_team;
	int p = pos;
	int l = GO_LEFT(pos);
	int r = GO_RIGHT(pos);

	/* TODO */
	if(r > heap->size) {
		return;
	}
	if(heap->cmp(heap->arr[r], heap->arr[l]) > 0 && heap->cmp(heap->arr[r],heap->arr[p]) > 0) {
		tmp_team = heap->arr[p];
		heap->arr[p] = heap->arr[r];
		heap->arr[r] = tmp_team;
		__heap_pop_fix(heap, r);
	} else if(heap->cmp( heap->arr[r], heap->arr[l]) <= 0 && heap->cmp(heap->arr[l], heap->arr[p]) > 0) {
		tmp_team = heap->arr[p];
		heap->arr[p] = heap->arr[l];
		heap->arr[l] = tmp_team;
		__heap_pop_fix(heap, l);
	}
	
	
}

void heap_pop(heap_t *heap)
{
	free(heap->arr[0]->title);
	free(heap->arr[0]);

	heap->arr[0] = heap->arr[heap->size - 1];

	heap->size--;

	__heap_pop_fix(heap, 0);
}

int heap_empty(heap_t *heap)
{
	return heap->size <= 0;
}

void heap_free(heap_t *heap)
{
	/* TODO */
	while(!heap_empty(heap)) {
		heap_pop(heap);
	}
	free(heap->arr);
	free(heap);
}
