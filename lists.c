status_t list_create(List * l) {

	if((l = (List *)calloc(1, sizeof(List))) == NULL)
		return ST_NO_MEM;

	return ST_OK;

}


status_t node_create(Node ** node, void * data, tkpt_s *(*clone)(tkpt_s *)) {

	if(! node || ! data || ! clone)
		return ST_NULL_PTR;

	if((*node = (Node *)calloc(1, sizeof(Node))) == NULL)
		return ST_NO_MEM;

	if(((*node)->data = clone(data)) == NULL) {
		free(*node);
		return ST_NO_MEM;
	}

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
	Node * new_node, * aux;

	if(! l || ! data)
		return ST_NULL_PTR;

	if(l->len == maxlen)
		return ST_FULL_LIST;

	if((st = node_create(&new_node, data, &clone_tkpt)) != ST_OK)
		return st;

	if(! l->first_node) {
		l->first_node = new_node;
		l->len++;
		return ST_OK;
	}

	aux = l->first_node;

	while(aux->next)
		aux = aux->next;

	aux->next = new_node;
	l->len++;
	return ST_OK;

}


List * list_delete(List * list) {

	Node * aux1, * aux2;

	if(! list)
		return NULL;

	if(! list->first_node) {
		free(list);
		return NULL;
	}

	aux1 = list->first_node;

	while(aux1->next) {
		aux2 = aux1;
		aux1 = aux1->next;
	}

	free(aux1->data);
	free(aux2->next);
	aux2->next = NULL;

	return list_delete(list);

}
