status_t list_create(List ** l) {

	if(! l)
		return ST_NULL_PTR;

	if((*l = (List *)calloc(1, sizeof(List))) == NULL)
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
		return ST_LIST_FULL;

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

	Node * aux1 = NULL, * aux2 = NULL;

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
	free(aux1);

	if(! aux2) {
		free(list);
		return NULL;
	}

	aux2->next = NULL;

	return list_delete(list);

}

status_t popleft(List * list, tkpt_s ** tkpt) {

	Node * aux;

	if(! list || ! tkpt)
		return ST_NULL_PTR;

	if(list->len == 0)
		return ST_LIST_EMPTY;

	*tkpt = list->first_node->data;

	aux = list->first_node->next;

	free(list->first_node);

	list->first_node = aux;
	
	list->len--;

	return ST_OK;

}