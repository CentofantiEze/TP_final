#include <stdio.h>
#include <stdlib.h>

struct tkpt {
	struct tm tkpt_time;
	int tkpt_msec;
	double latitude;
	double longitude;
	double elevation;
};

typedef struct tkpt tkpt_s;

struct node {
	void * data;
	struct node * next;
};

struct list {
	struct node * first_node;
	size_t len;
};

typedef struct node Node;
typedef struct list List;

status_t list_create(List ** l);
status_t node_create(Node ** node, void * data, void *(*clone)(void *));
status_t list_append(List * l, void * data, void *(*clone)(void *));
void * list_pop_left(List * l, status_t *);


status_t list_create(List * l) {

	if(! l)
		return ST_NULL_PTR;

	if((l = (List *)calloc(1, sizeof(List))) == NULL)
		return ST_NO_MEM;

	return ST_OK;

}


status_t node_create(Node ** node, void * data, void *(*clone)(void *)) {

	if(! node || ! data || ! clone)
		return ST_NULL_PTR;

	if((*node = (Node *)calloc(1, sizeof(Node))) == NULL)
		return ST_NO_MEM;

	if((*node)->data = (*clone)(data) == NULL)
		free(*node);
		return ST_NO_MEM;

	return ST_OK;	

}


tkpt_s * clone_tkpt(tkpt_s * data) {

	tkpt_s * aux;

	if(! data)
		return NULL;

	if((aux = (tkpt_s *)calloc(1, sizeof(tkpt_s))) != NULL)
		*aux = *data;

	return aux;
}


status_t list_append_tkpt(List * l, void * data, size_t maxlen) {

	status_t st;
	Node * new_node, aux;

	if(! l || ! data || ! clone)
		return ST_NULL_PTR;

	if(l->len == maxlen)
		return ST_FULL_LIST;

	if(st = node_create(&new_node, data, &clone_tkpt) != ST_OK)
		return st;

	if(! l->first_node) {
		l->first_node = new_node;
		l->len++;
		return ST_OK;
	}

	aux = l->first_node

	while(aux->next)
		aux = aux->next;

	aux->next = new_node;
	l->len++;
	return ST_OK;

}
