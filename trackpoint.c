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

	

}


tkpt_s * clone(tkpt_s * data) {

	tkpt_s * aux;

	if(! data)
		return NULL;

	if((aux = (tkpt_s *)calloc(1, sizeof(tkpt_s))) != NULL)
		*aux = *data;

	return aux;
}


status_t list_append(List * l, void * data, void *(*clone)(void *)) {

	status_t st;

	if(! l || ! data || ! clone)
		return ST_NULL_PTR;


}
